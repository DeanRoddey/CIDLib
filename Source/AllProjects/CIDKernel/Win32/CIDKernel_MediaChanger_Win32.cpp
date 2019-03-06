//
// FILE NAME: CIDKernel_MediaChanger_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation for the class
//  TKrnlMediaChanger. Everything is platform specific in this case,
//  since this class is dealing with low level IOCTL interfaces.
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
#include    <setupapi.h>
#include    <devguid.h>
#include    <mountdev.h>


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDKernel_MediaChanger_Win32
{
    // An enum returned by our bGetChangerInfo helper method below
    enum EGCIRes
    {
        EGCIRes_Success
        , EGCIRes_NotFound
        , EGCIRes_Error
    };
}



// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------

//
//  This method, given a device list handle and an index, will gather up the
//  info that we track about changers and fill in the passed changer info
//  structure. We have to do this from multiple places and it's a lot of grut
//  work code that we don't want to duplicate.
//
static CIDKernel_MediaChanger_Win32::EGCIRes
eGetChangerInfo(        HANDLE                              hDevList
                , const tCIDLib::TCard4                     c4Index
                ,       TKrnlMediaChanger::TMediaChgInfo&   kmciToFill)
{
    BOOL bRes;

    // Get the device interface data for this index
    SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
    DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    bRes = ::SetupDiEnumDeviceInterfaces
    (
        hDevList
        , NULL
        , (LPGUID)&GUID_DEVINTERFACE_MEDIUMCHANGER
        , c4Index
        , &DeviceInterfaceData
    );

    //
    //  It failed. If it's just no more items, then we are done. Else,
    //  it's an error.
    //
    if (bRes == FALSE)
    {
        DWORD LastErr = ::GetLastError();
        if (LastErr == ERROR_NO_MORE_ITEMS)
            return CIDKernel_MediaChanger_Win32::EGCIRes_NotFound;

        // Not just no more items, so some real failure
        TKrnlError::SetLastHostError(LastErr);
        return CIDKernel_MediaChanger_Win32::EGCIRes_Error;
    }

    // Allocate the buffer for this one
    tCIDLib::TCard4  c4DetailSz = 0;
    bRes = ::SetupDiGetDeviceInterfaceDetail
    (
        hDevList, &DeviceInterfaceData, 0, 0, &c4DetailSz, 0
    );

    if (bRes == FALSE)
    {
        DWORD LastErr = ::GetLastError();
        if (LastErr != ERROR_INSUFFICIENT_BUFFER)
        {
            // Not the expected buffer size error
            TKrnlError::SetLastHostError(::GetLastError());
            return CIDKernel_MediaChanger_Win32::EGCIRes_Error;
        }
    }

    // If no details, then we kind of failed. Shouldn't happen, but...
    if (!c4DetailSz)
        return CIDKernel_MediaChanger_Win32::EGCIRes_Error;

    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4DetailSz];
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
    bRes = ::SetupDiGetDeviceInterfaceDetail
    (
        hDevList, &DeviceInterfaceData, pDetails, c4DetailSz, 0, 0
    );

    if (!bRes)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return CIDKernel_MediaChanger_Win32::EGCIRes_Error;
    }

    // Open it using the product id path
    HANDLE hVolume = ::CreateFile
    (
        pDetails->DevicePath
        , FILE_READ_ATTRIBUTES
        , FILE_SHARE_READ | FILE_SHARE_WRITE
        , 0
        , OPEN_EXISTING
        , 0
        , 0
    );

    // If we couldn't open it, then obviously we've failed
    if (hVolume == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return CIDKernel_MediaChanger_Win32::EGCIRes_Error;
    }

    //
    //  Get the device name, is of the \Device\ChangerX type of
    //  form.
    //
    const tCIDLib::TCard4 c4DevNameBufSz = 1024;
    tCIDLib::TCard1* pc1DevName = new tCIDLib::TCard1[c4DevNameBufSz];
    TArrayJanitor<tCIDLib::TCard1> janDevName(pc1DevName);
    PMOUNTDEV_NAME pDevName = (PMOUNTDEV_NAME)pc1DevName;
    DWORD dwBytesReturned = 0;
    bRes = ::DeviceIoControl
    (
        hVolume
        , IOCTL_MOUNTDEV_QUERY_DEVICE_NAME
        , NULL
        , 0
        , pDevName
        , c4DevNameBufSz
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        ::CloseHandle(hVolume);
        return CIDKernel_MediaChanger_Win32::EGCIRes_Error;
    }

    // And we can close the handle now that we've got what we need
    ::CloseHandle(hVolume);

    // Copy over the product id
    TRawStr::CopyStr
    (
        kmciToFill.achProductId
        , pDetails->DevicePath
        , tCIDLib::c4ArrayElems(kmciToFill.achProductId)
    );

    //
    //  Copy over the name. Zero terminate it first. The size in the structure
    //  is in bytes, not chars, so adjust for that.
    //
    pDevName->Name[pDevName->NameLength / kCIDLib::c4CharBytes] = 0;
    TRawStr::CopyStr
    (
        kmciToFill.achDevPath, pDevName->Name, tCIDLib::c4ArrayElems(kmciToFill.achDevPath)
    );

    // And we just store the caller's device number
    kmciToFill.c4ChangerNum = c4Index;

    return CIDKernel_MediaChanger_Win32::EGCIRes_Success;
}



// ---------------------------------------------------------------------------
//   CLASS: TMediaChgHandle
//  PREFIX: hev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMediaChgHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TMediaChgHandle::TMediaChgHandle() :

    m_phmchgiThis(nullptr)
{
    m_phmchgiThis = new TMediaChgHandleImpl;
    m_phmchgiThis->hChanger = 0;
}

TMediaChgHandle::TMediaChgHandle(const TMediaChgHandle& hevSrc) :

    m_phmchgiThis(nullptr)
{
    m_phmchgiThis = new TMediaChgHandleImpl;
    m_phmchgiThis->hChanger = hevSrc.m_phmchgiThis->hChanger;
}

TMediaChgHandle::~TMediaChgHandle()
{
    delete m_phmchgiThis;
    m_phmchgiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TMediaChgHandle&
TMediaChgHandle::operator=(const TMediaChgHandle& hevSrc)
{
    if (this == &hevSrc)
        return *this;

    m_phmchgiThis->hChanger = hevSrc.m_phmchgiThis->hChanger;
    return *this;
}


tCIDLib::TBoolean
TMediaChgHandle::operator==(const TMediaChgHandle& hevSrc) const
{
    return (m_phmchgiThis->hChanger == hevSrc.m_phmchgiThis->hChanger);
}

tCIDLib::TBoolean
TMediaChgHandle::operator!=(const TMediaChgHandle& hevSrc) const
{
    return !operator==(hevSrc);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TMediaChgHandle::bIsValid() const
{
    return (m_phmchgiThis->hChanger != 0);
}


tCIDLib::TVoid TMediaChgHandle::Clear()
{
    m_phmchgiThis->hChanger = 0;
}


tCIDLib::TVoid
TMediaChgHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phmchgiThis->hChanger)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}


const TMediaChgHandleImpl& TMediaChgHandle::hmchgiThis() const
{
    return *m_phmchgiThis;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlMediaChanger
//  PREFIX: rmmd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlMediaChanger: Public, static methods
// ---------------------------------------------------------------------------

//
//  Enumerates all of the media changers and returns a list of info structures
//  about the ones we found.
//
tCIDLib::TBoolean
TKrnlMediaChanger::bEnumChangers(       TMediaChgInfo* const    pkmciToFill
                                , const tCIDLib::TCard4         c4ListSz
                                ,       tCIDLib::TCard4&        c4Found)
{
    // Start off the found count at zero
    c4Found = 0;

    SP_DEVICE_INTERFACE_DATA IntfInfo = {0};
    IntfInfo.cbSize = sizeof(IntfInfo);

    // And get the enumerator handle, only for devices actually present
    HDEVINFO hDevList = ::SetupDiGetClassDevs
    (
        (LPGUID)&GUID_DEVINTERFACE_MEDIUMCHANGER
        , 0
        , 0
        , DIGCF_PRESENT | DIGCF_INTERFACEDEVICE
    );

    if (!hDevList)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // We got one, so enumerate all the changers
    CIDKernel_MediaChanger_Win32::EGCIRes eResult;
    while (c4Found < c4ListSz)
    {
        // Get info on the current index
        eResult = eGetChangerInfo(hDevList, c4Found, pkmciToFill[c4Found]);

        // If we get an error, then bail out with a failure result
        if (eResult == CIDKernel_MediaChanger_Win32::EGCIRes_Error)
            break;

        // If not found, then we hit the end of the list so break out
        if (eResult == CIDKernel_MediaChanger_Win32::EGCIRes_NotFound)
            break;

        // We got one, so bump the found count/index
        c4Found++;
    }

    // And now we can dump the device list enumerator
    ::SetupDiDestroyDeviceInfoList(hDevList);

    return (eResult != CIDKernel_MediaChanger_Win32::EGCIRes_Error);
}



//
//  Returns information on the c4ChangerNum'th changer in the iteration list.
//  The caller is responsible for comparing the returned information to
//  any previous stored information and making sure it's the same changer
//  they were talking to before.
//
tCIDLib::TBoolean
TKrnlMediaChanger::bFindChangerNum( const   tCIDLib::TCard4     c4ChangerNum
                                    ,       TMediaChgInfo&      kmciToFill
                                    ,       tCIDLib::TBoolean&  bStatus)
{
    SP_DEVICE_INTERFACE_DATA IntfInfo = {0};
    IntfInfo.cbSize = sizeof(IntfInfo);

    // And get the enumerator handle, only for devices actually present
    HDEVINFO hDevList = ::SetupDiGetClassDevs
    (
        (LPGUID)&GUID_DEVINTERFACE_MEDIUMCHANGER
        , 0
        , 0
        , DIGCF_PRESENT | DIGCF_INTERFACEDEVICE
    );

    if (!hDevList)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    const CIDKernel_MediaChanger_Win32::EGCIRes eResult = eGetChangerInfo
    (
        hDevList, c4ChangerNum, kmciToFill
    );

    // And now we can dump the device list enumerator
    ::SetupDiDestroyDeviceInfoList(hDevList);

    // The status is whether we found it or not, not the error status.
    bStatus = (eResult == CIDKernel_MediaChanger_Win32::EGCIRes_Success);

    // The return is the error status
    return (eResult != CIDKernel_MediaChanger_Win32::EGCIRes_Error);
}



// ---------------------------------------------------------------------------
//  TKrnlMediaChanger: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlMediaChanger::TKrnlMediaChanger()
{
    //
    //  For now, set the changer number to max card, so we'll know it's not
    //  been set later if they try to do an open via the method that doesn't
    //  take the changer number.
    //
    m_kmchiThis.c4ChangerNum = kCIDLib::c4MaxCard;
}


TKrnlMediaChanger::TKrnlMediaChanger(const tCIDLib::TCard4 c4ChangerNum)
{
    //
    //  Just store the changer number in the info structure for now. We'll get
    //  the rest when they do an open.
    //
    m_kmchiThis.c4ChangerNum = c4ChangerNum;
}

TKrnlMediaChanger::~TKrnlMediaChanger()
{
}


// ---------------------------------------------------------------------------
//  TKrnlMediaChanger: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Closes this changer if it's open
tCIDLib::TBoolean TKrnlMediaChanger::bClose()
{
    // If already closed, nothing to do
    if (m_hmchgThis.m_phmchgiThis->hChanger)
    {
        if (!::CloseHandle(m_hmchgThis.m_phmchgiThis->hChanger))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


//
//  Let's client code know when we are opened or not, since they can't do
//  certain things unless we are.
//
tCIDLib::TBoolean TKrnlMediaChanger::bIsOpened() const
{
    return (m_hmchgThis.m_phmchgiThis->hChanger != 0);
}


tCIDLib::TBoolean
TKrnlMediaChanger::bIsSlotOccupied( const   tCIDLib::TCard4     c4SlotNum
                                    ,       tCIDLib::TBoolean&  bResult) const
{
    // If we've not been opened, then an error
    if (!m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Load up the read element status request structure
    CHANGER_READ_ELEMENT_STATUS ReadElementStatus;
    ReadElementStatus.ElementList.Element.ElementAddress = c4SlotNum;
    ReadElementStatus.ElementList.Element.ElementType = ChangerSlot;
    ReadElementStatus.ElementList.NumberOfElements = 1;
    ReadElementStatus.VolumeTagInfo = FALSE;
    CHANGER_ELEMENT_STATUS StatusInfo;

    DWORD dwBytesReturned;
    BOOL bRes = ::DeviceIoControl
    (
        m_hmchgThis.m_phmchgiThis->hChanger
        , IOCTL_CHANGER_GET_ELEMENT_STATUS
        , &ReadElementStatus
        , sizeof(ReadElementStatus)
        , &StatusInfo
        , sizeof(StatusInfo)
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    bResult = ((StatusInfo.Flags & ELEMENT_STATUS_FULL) == ELEMENT_STATUS_FULL);
    return kCIDLib::True;
}


//
//  Asks the changer to allow the user to load up the indicated target slot,
//  whihc of course must not already be occupied.
//
tCIDLib::TBoolean
TKrnlMediaChanger::bLoadSlot(const tCIDLib::TCard4 c4TargetSlot)
{
    // If we've not been opened, then an error
    if (!m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    //
    //  Load up the move medium request structure. In this case, the
    //  source is the IEPort, which is the loading slot.
    //
    CHANGER_MOVE_MEDIUM MoveMedium;
    MoveMedium.Source.ElementAddress = 0;
    MoveMedium.Source.ElementType = ChangerIEPort;
    MoveMedium.Destination.ElementAddress = c4TargetSlot;
    MoveMedium.Destination.ElementType = ChangerSlot;
    MoveMedium.Transport.ElementAddress = 0;
    MoveMedium.Transport.ElementType = ChangerTransport;
    MoveMedium.Flip = FALSE;

    DWORD dwBytesReturned;
    BOOL bRes = ::DeviceIoControl
    (
        m_hmchgThis.m_phmchgiThis->hChanger
        , IOCTL_CHANGER_MOVE_MEDIUM
        , &MoveMedium
        , sizeof(MoveMedium)
        , NULL
        , 0
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
        TKrnlError::SetLastHostError(::GetLastError());

    return (bRes != 0);
}


//
//  Queries the status of the changer, which is just wether there is any
//  slot currently loaded into the indicated drive. If there isn't any
//  disc in that drive, the value is max card.
//
tCIDLib::TBoolean
TKrnlMediaChanger::bQueryDriveInfo( const   tCIDLib::TCard4     c4DriveNum
                                    ,       tCIDLib::TCard4&    c4LoadedSlot) const
{
    // If we've not been opened, then an error
    if (!m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Load up the read element status request structure
    CHANGER_READ_ELEMENT_STATUS ReadElementStatus;
    ReadElementStatus.ElementList.Element.ElementAddress = c4DriveNum;
    ReadElementStatus.ElementList.Element.ElementType = ChangerDrive;
    ReadElementStatus.ElementList.NumberOfElements = 1;
    ReadElementStatus.VolumeTagInfo = FALSE;
    CHANGER_ELEMENT_STATUS StatusInfo;

    DWORD dwBytesReturned;
    BOOL bRes = ::DeviceIoControl
    (
        m_hmchgThis.m_phmchgiThis->hChanger
        , IOCTL_CHANGER_GET_ELEMENT_STATUS
        , &ReadElementStatus
        , sizeof(ReadElementStatus)
        , &StatusInfo
        , sizeof(StatusInfo)
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  If it shows full, then return the element address for it. Else
    //  just return max card.
    //
    if ((StatusInfo.Flags & ELEMENT_STATUS_FULL) == ELEMENT_STATUS_FULL)
        c4LoadedSlot = StatusInfo.SrcElementAddress.ElementAddress;
    else
        c4LoadedSlot = kCIDLib::c4MaxCard;

    return kCIDLib::True;
}


//
//  Moves the disc in the source slot to the target slot. Obviously the
//  source has to be full and the target not.
//
tCIDLib::TBoolean
TKrnlMediaChanger::bMoveDisc(const  tCIDLib::TCard4 c4SrcSlot
                            , const tCIDLib::TCard4 c4TarSlot)
{
    // If we've not been opened, then an error
    if (!m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Load up the move medium request structure
    CHANGER_MOVE_MEDIUM MoveMedium;
    MoveMedium.Source.ElementAddress = c4SrcSlot;
    MoveMedium.Source.ElementType = ChangerSlot;
    MoveMedium.Destination.ElementAddress = c4TarSlot;
    MoveMedium.Destination.ElementType = ChangerSlot;
    MoveMedium.Transport.ElementAddress = 0;
    MoveMedium.Transport.ElementType = ChangerTransport;
    MoveMedium.Flip = FALSE;

    DWORD dwBytesReturned;
    BOOL bRes = ::DeviceIoControl
    (
        m_hmchgThis.m_phmchgiThis->hChanger
        , IOCTL_CHANGER_MOVE_MEDIUM
        , &MoveMedium
        , sizeof(MoveMedium)
        , NULL
        , 0
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
        TKrnlError::SetLastHostError(::GetLastError());

    return (bRes != 0);
}


//
//  Loads the disc in the indicated slot into the indicated drive. The slot
//  must of course be occupied and the drive must not be, or it will fail.
//
tCIDLib::TBoolean
TKrnlMediaChanger::bMountDisc(  const   tCIDLib::TCard4 c4DriveNum
                                , const tCIDLib::TCard4 c4SlotToPlay)
{
    // If we've not been opened, then an error
    if (!m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Load up the move medium request structure
    CHANGER_MOVE_MEDIUM MoveMedium;
    MoveMedium.Source.ElementAddress = c4SlotToPlay;
    MoveMedium.Source.ElementType = ChangerSlot;
    MoveMedium.Destination.ElementAddress = 0;
    MoveMedium.Destination.ElementType = ChangerDrive;
    MoveMedium.Transport.ElementAddress = c4DriveNum;
    MoveMedium.Transport.ElementType = ChangerTransport;
    MoveMedium.Flip = FALSE;

    DWORD dwBytesReturned;
    BOOL bRes = ::DeviceIoControl
    (
        m_hmchgThis.m_phmchgiThis->hChanger
        , IOCTL_CHANGER_MOVE_MEDIUM
        , &MoveMedium
        , sizeof(MoveMedium)
        , NULL
        , 0
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
        TKrnlError::SetLastHostError(::GetLastError());

    return (bRes != 0);
}


//
//  Tries to get the changer opened using the changer number we were given
//  in the ctor. If it works, we store away the handle for later use.
//
tCIDLib::TBoolean TKrnlMediaChanger::bOpen()
{
    // If already open, that's an error
    if (m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    // If they've not set a changer number yet, that's an error
    if (m_kmchiThis.c4ChangerNum == kCIDLib::c4MaxCard)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    //
    //  Look up the changer info for the changer number we were assigned.
    //  Note that this will fill in the sturcture itself, where we are getting
    //  our number from. But it would only fill it if it finds the changer
    //  number we asked for, so the number won't ever come back different.
    //
    //  Note that we try to look it up every time, instead of seeing if we
    //  have already looked it up, just in case something may have changed.
    //
    tCIDLib::TBoolean bFound;
    if (!bFindChangerNum(m_kmchiThis.c4ChangerNum, m_kmchiThis, bFound))
        return kCIDLib::False;

    //
    //  If we didn't get an error, but didn't find it, then that's an error
    //  from our perspecive.
    //
    if (!bFound)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotFound);
        return kCIDLib::False;
    }

    // Try to open this guy up
    m_hmchgThis.m_phmchgiThis->hChanger = ::CreateFile
    (
        m_kmchiThis.achProductId
        , GENERIC_READ | GENERIC_WRITE
        , FILE_SHARE_READ
        , NULL
        , OPEN_EXISTING
        , m_kmchiThis.c4ChangerNum
        , NULL
    );

    if (m_hmchgThis.m_phmchgiThis->hChanger == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  This one lets them pass in a changer number to open. We just store
//  the number and call the other version. It can't already be open.
//
tCIDLib::TBoolean TKrnlMediaChanger::bOpen(const tCIDLib::TCard4 c4ChangerNum)
{
    // If already open, that's an error
    if (m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    m_kmchiThis.c4ChangerNum = c4ChangerNum;
    return bOpen();
}


//
//  Unloads the disc in the indicated source slot. Which will cause the changer
//  to spit it out the slot. The source slot obviously must be occupied.
//
//
tCIDLib::TBoolean
TKrnlMediaChanger::bUnloadSlot(const tCIDLib::TCard4 c4SrcSlot)
{
    // If we've not been opened, then an error
    if (!m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    //
    //  Load up the move medium request structure. In this case, the
    //  target is the IEPort and the source is the slot to be unloaded.
    //
    CHANGER_MOVE_MEDIUM MoveMedium;
    MoveMedium.Destination.ElementAddress = 0;
    MoveMedium.Destination.ElementType = ChangerIEPort;
    MoveMedium.Source.ElementAddress = c4SrcSlot;
    MoveMedium.Source.ElementType = ChangerSlot;
    MoveMedium.Transport.ElementAddress = 0;
    MoveMedium.Transport.ElementType = ChangerTransport;
    MoveMedium.Flip = FALSE;

    DWORD dwBytesReturned;
    BOOL bRes = ::DeviceIoControl
    (
        m_hmchgThis.m_phmchgiThis->hChanger
        , IOCTL_CHANGER_MOVE_MEDIUM
        , &MoveMedium
        , sizeof(MoveMedium)
        , NULL
        , 0
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
        TKrnlError::SetLastHostError(::GetLastError());
    return (bRes != 0);
}


//
//  Unmounts the disc in the indicated drive back to the indicated slot.
//  Obviously there must be a disc in the drive and the target slot must
//  not be occupied.
//
tCIDLib::TBoolean
TKrnlMediaChanger::bUnmountDisc(const   tCIDLib::TCard4 c4DriveNum
                                , const tCIDLib::TCard4 c4ToSlot)
{
    // If we've not been opened, then an error
    if (!m_hmchgThis.m_phmchgiThis->hChanger)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Load up the move medium request structure
    CHANGER_MOVE_MEDIUM MoveMedium;
    MoveMedium.Destination.ElementAddress = c4ToSlot;
    MoveMedium.Destination.ElementType = ChangerSlot;
    MoveMedium.Source.ElementAddress = 0;
    MoveMedium.Source.ElementType = ChangerDrive;
    MoveMedium.Transport.ElementAddress = c4DriveNum;
    MoveMedium.Transport.ElementType = ChangerTransport;
    MoveMedium.Flip = FALSE;

    DWORD dwBytesReturned;
    BOOL bRes = ::DeviceIoControl
    (
        m_hmchgThis.m_phmchgiThis->hChanger
        , IOCTL_CHANGER_MOVE_MEDIUM
        , &MoveMedium
        , sizeof(MoveMedium)
        , NULL
        , 0
        , &dwBytesReturned
        , NULL
    );

    if (!bRes)
        TKrnlError::SetLastHostError(::GetLastError());

    return (bRes != 0);
}


//
//  Provides access to our media changer info. It's only valid when we are
//  opened, but we leave it to the client code to be sure only to use it
//  at that time.
//
const TKrnlMediaChanger::TMediaChgInfo& TKrnlMediaChanger::kmchiThis() const
{
    return m_kmchiThis;
}

