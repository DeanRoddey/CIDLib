//
// FILE NAME: CIDKernel_USB_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/12/2019
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

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlUSBDev::bFindHIDDev(const  tCIDLib::TCard2     c2VendorId
                        , const tCIDLib::TCard2     c2ProductId
                        ,       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars
                        ,       tCIDLib::TBoolean&  bFound)
{
    bFound = kCIDLib::False;

    return kCIDLib::True;
}

