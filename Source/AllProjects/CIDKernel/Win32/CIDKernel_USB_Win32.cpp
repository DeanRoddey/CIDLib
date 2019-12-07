//
// FILE NAME: CIDKernel_USB_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2004
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides some core support for interacting with USB and HID
//  devices.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"

#pragma     warning(push)
#include    <CodeAnalysis\Warnings.h>
#pragma     warning(disable : ALL_CODE_ANALYSIS_WARNINGS 26812)
#include    <setupapi.h>
#pragma     warning(pop)


//
//  Some stuff we define ourself, because otherwise we'd have to bring in a
//  whole raft of device driver kit stuff.
//
extern "C"
{
    #pragma CIDLIB_PACK(4)

    typedef struct _HIDP_PREPARSED_DATA * PHIDP_PREPARSED_DATA;

    typedef struct _HIDP_CAPS
    {
      USHORT    Usage;
      USHORT    UsagePage;
      USHORT    InputReportByteLength;
      USHORT    OutputReportByteLength;
      USHORT    FeatureReportByteLength;
      USHORT    Reserved[17];
      USHORT    NumberLinkCollectionNodes;
      USHORT    NumberInputButtonCaps;
      USHORT    NumberInputValueCaps;
      USHORT    NumberInputDataIndices;
      USHORT    NumberOutputButtonCaps;
      USHORT    NumberOutputValueCaps;
      USHORT    NumberOutputDataIndices;
      USHORT    NumberFeatureButtonCaps;
      USHORT    NumberFeatureValueCaps;
      USHORT    NumberFeatureDataIndices;
    } HIDP_CAPS, *PHIDP_CAPS;

    typedef struct _HIDD_ATTRIBUTES
    {
        ULONG   Size;
        USHORT  VendorID;
        USHORT  ProductID;
        USHORT  VersionNumber;

    } HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

    #pragma CIDLIB_POPPACK


    //
    //  Some APIs we will call. We don't have the header, so we have to
    //  provide the signatures ourself.
    //
    extern BOOLEAN __stdcall HidD_GetAttributes
    (
        HANDLE  HidDeviceObject
        , PHIDD_ATTRIBUTES Attributes
    );

    extern void __stdcall HidD_GetHidGuid(struct _GUID *);

    extern BOOLEAN __stdcall HidD_GetPreparsedData
    (
        HANDLE  HidDeviceObject
        , PHIDP_PREPARSED_DATA* PreparsedData
    );

    extern void __stdcall HidP_GetCaps
    (
        PHIDP_PREPARSED_DATA  PreparsedData
        , HIDP_CAPS* Capabilities
    );
}



// ---------------------------------------------------------------------------
//  Local helpers
// ---------------------------------------------------------------------------
static tCIDLib::TBoolean
bStringToGUID(const tCIDLib::TCh* const pszText, GUID& guidToFill)
{
    //
    //  Create the GUIID from the text version. The format of the id string
    //  must be:
    //
    //  {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
    //
    //  where X is a hex digit. The first is an long value (4 2 digit values),
    //  then three shorts (2 2 digit values), then 2 and 6 individual bytes
    //  (8 2 digit values.) The braces are optional, so it must be either 36 or
    //  38 characters.
    //
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszText);
    if ((c4Len != 36) && (c4Len != 38))
        return kCIDLib::False;

    //
    //  Make a copy of the string that we can mangle, and get rid of the
    //  braces.
    //
    tCIDLib::TCh* pszGUID = TRawStr::pszReplicate(pszText);
    TArrayJanitor<tCIDLib::TCh> janGUID(pszGUID);
    if (c4Len == 38)
    {
        *(pszGUID + 37) = kCIDLib::chNull;
        pszGUID++;
    }

    // There must be a dash at 8, 13, 18, and 23
    if ((pszGUID[8] != kCIDLib::chHyphenMinus)
    ||  (pszGUID[13] != kCIDLib::chHyphenMinus)
    ||  (pszGUID[18] != kCIDLib::chHyphenMinus)
    ||  (pszGUID[23] != kCIDLib::chHyphenMinus))
    {
        return kCIDLib::False;
    }

    // Make it more convenient for us by upper casing it
    TRawStr::pszUpperCase(pszGUID);

    // Ok, put nulls in those places to create separate strings
    pszGUID[8] = kCIDLib::chNull;
    pszGUID[13] = kCIDLib::chNull;
    pszGUID[18] = kCIDLib::chNull;
    pszGUID[23] = kCIDLib::chNull;

    // Ok, fill in the GUID now, by converting the strings to binary
    tCIDLib::TBoolean bOk;
    tCIDLib::TCard4   c4Tmp;
    guidToFill.Data1 = TRawStr::c4AsBinary(pszGUID, bOk, tCIDLib::ERadices::Hex);
    if (!bOk)
        return kCIDLib::False;

    pszGUID += 9;
    c4Tmp = TRawStr::c4AsBinary(pszGUID, bOk, tCIDLib::ERadices::Hex);
    if (!bOk)
        return kCIDLib::False;
    guidToFill.Data2 = tCIDLib::TCard2(c4Tmp);

    pszGUID += 5;
    c4Tmp = TRawStr::c4AsBinary(pszGUID, bOk, tCIDLib::ERadices::Hex);
    if (!bOk)
        return kCIDLib::False;
    guidToFill.Data3 = tCIDLib::TCard2(c4Tmp);

    //
    //  The next one gets stored as two bytes but for convenience we convert
    //  it as a short.
    //
    pszGUID += 5;
    c4Tmp = TRawStr::c4AsBinary(pszGUID, bOk, tCIDLib::ERadices::Hex);
    if (!bOk)
        return kCIDLib::False;
    guidToFill.Data4[0] = tCIDLib::TCard1(c4Tmp >> 8);
    guidToFill.Data4[1] = tCIDLib::TCard1(c4Tmp & 0xFF);

    //
    //  And now we have 6 2 digit bytes left to fill in the last 6 bytes
    //  of Data4. There are no separators, so it's a little more of a pain
    //  than it would be otherwise.
    //
    pszGUID += 5;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 6; c4Index++)
    {
        const tCIDLib::TCh ch1(*pszGUID++);
        const tCIDLib::TCh ch2(*pszGUID++);
        if (!TRawStr::bIsHexDigit(ch1) || !TRawStr::bIsHexDigit(ch2))
            return kCIDLib::False;

        tCIDLib::TCard1 c1Val;
        if ((ch1 >= kCIDLib::chDigit0) && (ch1 <= kCIDLib::chDigit9))
            c1Val = tCIDLib::TCard1(ch1 - kCIDLib::chDigit0);
        else
            c1Val = tCIDLib::TCard1(10 + (ch1 - kCIDLib::chLatin_A));
        c1Val <<= 4;

        if ((ch2 >= kCIDLib::chDigit0) && (ch2 <= kCIDLib::chDigit9))
            c1Val |= tCIDLib::TCard1(ch2 - kCIDLib::chDigit0);
        else
            c1Val |= tCIDLib::TCard1(10 + (ch2 - kCIDLib::chLatin_A));

        guidToFill.Data4[2 + c4Index] = c1Val;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  USBDev namespace methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlUSBDev::bCheckForDevice(const  tCIDLib::TCh* const pszDevId
                            , const tCIDLib::TCard2     c2VendorId
                            , const tCIDLib::TCard2     c2ProductId
                            ,       tCIDLib::TBoolean&  bFound)
{
    // Assume worst case
    bFound = kCIDLib::False;

    // Set up the data structures we'll need for this
    SP_DEVICE_INTERFACE_DATA IntfInfo = {0};
    IntfInfo.cbSize = sizeof(IntfInfo);

    HIDD_ATTRIBUTES Attrs = {0};
    Attrs.Size = sizeof(Attrs);

    //
    //  Try to convert the string id to a GUID that we'll need in order to
    //  search for devices of that type. If bad, return false now.
    //
    GUID guidDev;
    if (!bStringToGUID(pszDevId, guidDev))
        return kCIDLib::False;


    // And get the enumerator handle, only for devices actually present
    HDEVINFO hDevList = ::SetupDiGetClassDevs
    (
        &guidDev, 0, 0, DIGCF_INTERFACEDEVICE | DIGCF_PRESENT
    );

    if (!hDevList)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Now loop through the devices till we find our guy, or fail
    tCIDLib::TCard4 c4Index = 0;
    bFound = kCIDLib::False;
    HANDLE hDevFl;
    while (1)
    {
        //
        //  Break out when done. It could be an error as well, but we don't
        //  distinguish.
        //
        if (!::SetupDiEnumDeviceInterfaces(hDevList, 0, &guidDev, c4Index++, &IntfInfo))
            break;

        // Allocate the buffer for this one
        tCIDLib::TCard4  c4DetailSz = 0;
        tCIDLib::TCard1* pc1Buf = 0;
        ::SetupDiGetDeviceInterfaceDetail
        (
            hDevList, &IntfInfo, 0, 0, &c4DetailSz, 0
        );

        if (!c4DetailSz)
            continue;

        // Point the details stucture pointer at it and set the size info
        hDevFl = 0;
        {
            pc1Buf = new tCIDLib::TCard1[c4DetailSz];
            SP_DEVICE_INTERFACE_DETAIL_DATA* pDetails
            (
                (SP_DEVICE_INTERFACE_DETAIL_DATA*)pc1Buf
            );
            pDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

            //
            //  Get the interface details, which gets us a path that we can
            //  use to get the device attributes.
            //
            ::SetupDiGetDeviceInterfaceDetail
            (
                hDevList, &IntfInfo, pDetails, c4DetailSz, 0, 0
            );

            // Temporarly open this device
            hDevFl = ::CreateFile
            (
                pDetails->DevicePath
                , GENERIC_READ
                , FILE_SHARE_READ | FILE_SHARE_WRITE
                , 0
                , OPEN_EXISTING
                , 0
                , 0
            );
        }

        // Get the attrs. If it's our guy, break out
        ::HidD_GetAttributes(hDevFl, &Attrs);

        // And close it now that we have the info
        ::CloseHandle(hDevFl);

        if ((Attrs.VendorID == c2VendorId)
        &&  (Attrs.ProductID == c2ProductId))
        {
            bFound = kCIDLib::True;
            break;
        }
    }

    // And now we can dump the device list enumerator
    ::SetupDiDestroyDeviceInfoList(hDevList);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlUSBDev::bFindHIDDev(const  tCIDLib::TCard2     c2VendorId
                        , const tCIDLib::TCard2     c2ProductId
                        ,       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars
                        ,       tCIDLib::TBoolean&  bFound)
{
    // Set up the data structures we'll need for this
    SP_DEVICE_INTERFACE_DATA IntfInfo = {0};
    IntfInfo.cbSize = sizeof(IntfInfo);

    HIDD_ATTRIBUTES Attrs = {0};
    Attrs.Size = sizeof(Attrs);

    //
    //  Get the GUID for HID devices that we need to iterate the devices
    //  available.
    //
    GUID guidHID;
    ::HidD_GetHidGuid(&guidHID);

    // And get the enumerator handle, only for devices actually present
    HDEVINFO hDevList = ::SetupDiGetClassDevs
    (
        &guidHID
        , 0
        , 0
        , DIGCF_INTERFACEDEVICE | DIGCF_PRESENT
    );

    if (!hDevList)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Now loop through the devices till we find our guy, or fail
    tCIDLib::TCard4 c4Index = 0;
    bFound = kCIDLib::False;
    HANDLE hDevFl;
    while (1)
    {
        //
        //  Break out when done. It could be an error as well, but we don't
        //  distinguish.
        //
        if (!::SetupDiEnumDeviceInterfaces(hDevList, 0, &guidHID, c4Index++, &IntfInfo))
            break;

        // Allocate the buffer for this one
        tCIDLib::TCard4  c4DetailSz = 0;
        tCIDLib::TCard1* pc1Buf = 0;
        ::SetupDiGetDeviceInterfaceDetail
        (
            hDevList, &IntfInfo, 0, 0, &c4DetailSz, 0
        );

        if (!c4DetailSz)
            continue;

        // Point the details stucture pointer at it and set the size info
        {
            pc1Buf = new tCIDLib::TCard1[c4DetailSz];
            SP_DEVICE_INTERFACE_DETAIL_DATA* pDetails
            (
                (SP_DEVICE_INTERFACE_DETAIL_DATA*)pc1Buf
            );
            pDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

            //
            //  Get the interface details, which gets us a path that we can use
            //  to get the device attributes.
            //
            ::SetupDiGetDeviceInterfaceDetail
            (
                hDevList, &IntfInfo, pDetails, c4DetailSz, 0, 0
            );

            // Temporarly open this device
            hDevFl = ::CreateFile
            (
                pDetails->DevicePath
                , GENERIC_READ
                , FILE_SHARE_READ | FILE_SHARE_WRITE
                , 0
                , OPEN_EXISTING
                , 0
                , 0
            );

            // Get the attrs. If it's our guy, break out
            ::HidD_GetAttributes(hDevFl, &Attrs);

            // And close it now that we have the info
            ::CloseHandle(hDevFl);

            if ((Attrs.VendorID == c2VendorId)
            &&  (Attrs.ProductID == c2ProductId))
            {
                // Copy the device name back if we can, else it's an error
                if (TRawStr::c4StrLen(pDetails->DevicePath) > c4MaxChars)
                {
                    TKrnlError::SetLastError(kKrnlErrs::errcData_InsufficientBuffer);
                    return kCIDLib::False;
                }

                // We can handle it, so store it and clean up the buffer
                TRawStr::CopyStr(pszToFill, pDetails->DevicePath, c4MaxChars);
                bFound = kCIDLib::True;
                break;
            }
        }
    }

    // And now we can dump the device list enumerator
    ::SetupDiDestroyDeviceInfoList(hDevList);

    return kCIDLib::True;
}

