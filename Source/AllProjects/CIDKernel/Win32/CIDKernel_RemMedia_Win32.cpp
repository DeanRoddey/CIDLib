//
// FILE NAME: CIDKernel_RemMedia_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/18/2006
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation for the class
//  TKrnlRemMedia. Almost everything is platform specific in this case,
//  since this class is dealing with low level ASPI or IOCTL interfaces.
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
#include    "CIDKernel_RemMedia.hpp"

// Bring in a DDK header with the IOCTLS we need
#define     _ANONYMOUS_UNION
#pragma     warning(disable : 4200)
#include    <ntddcdrm.h>
#include    <ntddscsi.h>

//
// We have to define a couple things that aren't in any of the headers
//
#define     IOCTL_CDROM_GET_CONFIGURATION CTL_CODE(IOCTL_CDROM_BASE, 0x0016, METHOD_BUFFERED, FILE_READ_ACCESS)
typedef struct _GET_CONFIGURATION_HEADER {
  UCHAR  DataLength[4];
  UCHAR  Reserved[2];
  UCHAR  CurrentProfile[2];
  UCHAR  Data[0];
} GET_CONFIGURATION_HEADER, *PGET_CONFIGURATION_HEADER;

typedef enum _FEATURE_NUMBER
{
    FeatureProfileList              = 0x0000
    , FeatureCore                   = 0x0001
    , FeatureMorphing               = 0x0002
    , FeatureRemovableMedium        = 0x0003
    , FeatureWriteProtect           = 0x0004
                                    // Reserved 0x0005 - 0x000f
    , FeatureRandomReadable         = 0x0010
                                    // Reserved 0x0011 - 0x001c
    , FeatureMultiRead              = 0x001d
    , FeatureCdRead                 = 0x001e
    , FeatureDvdRead                = 0x001f
    , FeatureRandomWritable         = 0x0020
    , FeatureIncrStreamingWritable  = 0x0021
    , FeatureSectorErasable         = 0x0022
    , FeatureFormattable            = 0x0023
    , FeatureDefectManagement       = 0x0024
    , FeatureWriteOnce              = 0x0025
    , FeatureRestrictedOverwrite    = 0x0026
    , FeatureCdrwCAVWrite           = 0x0027
    , FeatureMrw                    = 0x0028
                                     // Reserved 0x0029
    , FeatureDvdPlusRW              = 0x002a
                                    // Reserved 0x002b
    , FeatureRigidRestrictedOvr     = 0x002c
    , FeatureCdTrackAtOnce          = 0x002d
    , FeatureCdMastering            = 0x002e
    , FeatureDvdRecordableWrite     = 0x002f
    , FeatureDDCDRead               = 0x0030
    , FeatureDDCDRWrite             = 0x0031
    , FeatureDDCDRWWrite            = 0x0032
                                    // Reserved 0x0030 - 0x00ff
    , FeaturePowerManagement        = 0x0100
    , FeatureSMART                  = 0x0101
    , FeatureEmbeddedChanger        = 0x0102
    , FeatureCDAudioAnalogPlay      = 0x0103
    , FeatureMicrocodeUpgrade       = 0x0104
    , FeatureTimeout                = 0x0105
    , FeatureDvdCSS                 = 0x0106
    , FeatureRealTimeStreaming      = 0x0107
    , FeatureLogicalUnitSerialNumber= 0x0108
                                    // Reserved 0x0109
    , FeatureDiscControlBlocks      = 0x010a
    , FeatureDvdCPRM                = 0x010b
                                    // Reserved 0x010c - 0xfeff
                                    // Vendor Specific 0xff00 - 0xffff
} FEATURE_NUMBER, *PFEATURE_NUMBER;

typedef enum _FEATURE_PROFILE_TYPE
{
    ProfileInvalid              = 0x0000
    , ProfileNonRemovableDisk   = 0x0001
    , ProfileRemovableDisk      = 0x0002
    , ProfileMOErasable         = 0x0003
    , ProfileMOWriteOnce        = 0x0004
    , ProfileAS_MO              = 0x0005
                                // Reserved 0x0006 - 0x0007
    , ProfileCdrom              = 0x0008
    , ProfileCdRecordable       = 0x0009
    , ProfileCdRewritable       = 0x000a

    , ProfileDvdRom             = 0x0010
    , ProfileDvdRecordable      = 0x0011
    , ProfileDvdRam             = 0x0012
    , ProfileDvdRewritable      = 0x0013
    , ProfileDvdRWSequential    = 0x0014
    , ProfileDvdDashRDualLayer  = 0x0015
    , ProfileDvdDashRLayerJump  = 0x0016

    , ProfileDvdPlusRW          = 0x001A
    , ProfileDvdPlusR           = 0x001B

    , ProfileDDCdrom            = 0x0020
    , ProfileDDCdRecordable     = 0x0021
    , ProfileDDCdRewritable     = 0x0022

    , ProfileDvdPlusRWDualLayer = 0x002A
    , ProfileDvdPlusRDualLayer  = 0x002B

    , ProfileBDRom              = 0x0040
    , ProfileBDRSeqWritable     = 0x0041
    , ProfileBDRRandomWritable  = 0x0042
    , ProfileBDRewritable       = 0x0043

    , ProfileHDDVDRom           = 0x0050
    , ProfileHDDVDRecordable    = 0x0051
    , ProfileHDDVDRam           = 0x0052
    , ProfileHDDVDRewritable    = 0x0053
    , ProfileHDDVDRDualLayer    = 0x0058
    , ProfileHDDVDRWDualLayer   = 0x005A

    , ProfileNonStandard        = 0xffff
} FEATURE_PROFILE_TYPE, *PFEATURE_PROFILE_TYPE;


typedef struct _GET_CONFIGURATION_IOCTL_INPUT
{
    FEATURE_NUMBER  Feature;
    ULONG  RequestType;
    PVOID  Reserved[2];
} GET_CONFIGURATION_IOCTL_INPUT, *PGET_CONFIGURATION_IOCTL_INPUT;

#pragma     warning(default : 4200)
#undef      _ANONYMOUS_UNION



// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDKernel_RemMedia
{
    // -----------------------------------------------------------------------
    //  A template for the device path we need to open a drive.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const aszPathTmpl = L"\\\\.\\ :";


    // -----------------------------------------------------------------------
    //  The structure for our per-platform data that we allocate for each
    //  CDROM object. We allocate an event to use in any ioctl calls we make,
    //  to make the async overlapped calls into sync calls. and we pre-setup
    //  the drive path.
    // -----------------------------------------------------------------------
    struct TPlatData
    {
        HANDLE          hDevice;
        HANDLE          hEvent;
        tCIDLib::TCh    szDrivePath[8];
    };
}



// ---------------------------------------------------------------------------
//  TFacCIDKernel: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Not used yet, but here for future use
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermRemMedia(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
    }
     else if (eState == tCIDLib::EInitTerm::Terminate)
    {
    }
    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static tCIDLib::TBoolean
bDoIOCTL(       CIDKernel_RemMedia::TPlatData&  PlatData
        , const tCIDLib::TCard4                 c4IOCode
        , const tCIDLib::TVoid*                 pInBuf
        , const tCIDLib::TCard4                 c4InBytes
        ,       tCIDLib::TVoid* const           pOutBuf
        ,       tCIDLib::TCard4&                c4OutBytes
        ,       tCIDLib::TCard4&                c4ErrCode)
{
    // If the event has not been created yet, then create it
    if (!PlatData.hEvent)
        PlatData.hEvent = ::CreateEvent(0, TRUE, FALSE, 0);

    // Set up an overlapped IO buffer with signaling event
    OVERLAPPED Overlapped = {0};
    Overlapped.hEvent = PlatData.hEvent;

    if (!Overlapped.hEvent)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    c4ErrCode = ::DeviceIoControl
    (
        PlatData.hDevice
        , c4IOCode
        , (VOID*)pInBuf
        , c4InBytes
        , pOutBuf
        , c4OutBytes
        , pOutBuf ? &c4OutBytes : 0
        , &Overlapped
    );

    if (!c4ErrCode)
    {
        //
        //  We generally expect a pending error. If it's that, then wait
        //  for it to complete, else it failed outright.
        //
        c4ErrCode = ::GetLastError();
        if (c4ErrCode == ERROR_IO_PENDING)
        {
            // Wait for the completion. If it fails, then give up
            if (!::GetOverlappedResult(PlatData.hDevice, &Overlapped, &c4OutBytes, TRUE))
            {
                c4ErrCode = ::GetLastError();
                TKrnlError::SetLastHostError(c4ErrCode);
                return kCIDLib::False;
            }
        }
         else
        {
            c4ErrCode = ::GetLastError();
            TKrnlError::SetLastHostError(c4ErrCode);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlRemMediaDrv
//  PREFIX: rmmd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlRemMediaDrv::TKrnlRemMediaDrv() :

    m_pPlatData(0)
{
    // Allocate our platform data and set it up
    CIDKernel_RemMedia::TPlatData* pPlatData = new CIDKernel_RemMedia::TPlatData;
    TRawMem::SetMemBuf
    (
        pPlatData, tCIDLib::TCard1(0), sizeof(CIDKernel_RemMedia::TPlatData)
    );

    // And store it in the publically visible opaque pointer
    m_pPlatData = pPlatData;

    // Initialize the drive id to empty
    m_szDriveId[0] = kCIDLib::chNull;
}

TKrnlRemMediaDrv::TKrnlRemMediaDrv(const tCIDLib::TCh* const pszDevPath) :

    m_pPlatData(0)
{
    // Allocate our platform data and set it up
    CIDKernel_RemMedia::TPlatData* pPlatData = new CIDKernel_RemMedia::TPlatData;
    TRawMem::SetMemBuf
    (
        pPlatData, tCIDLib::TCard1(0), sizeof(CIDKernel_RemMedia::TPlatData)
    );

    // Store the device path
    TRawStr::CopyStr
    (
        pPlatData->szDrivePath
        , pszDevPath
        , c4MaxBufChars(pPlatData->szDrivePath)
    );

    // And store it in the publically visible opaque pointer
    m_pPlatData = pPlatData;

    // Store the device path
    bSetPath(pszDevPath);

    // Initialize the drive id to empty
    m_szDriveId[0] = kCIDLib::chNull;
}

TKrnlRemMediaDrv::~TKrnlRemMediaDrv()
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    // Close any existing handle
    if (pPlatData->hDevice)
    {
        ::CloseHandle(pPlatData->hDevice);
        pPlatData->hDevice = 0;
    }

    if (pPlatData->hEvent)
    {
        ::CloseHandle(pPlatData->hEvent);
        pPlatData->hDevice = 0;
    }

    // And clean up the platform data
    delete pPlatData;
    m_pPlatData = 0;
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Checks to see if the drive has accessible media. This guy doesn't return
//  a separate status. It just returns an error code if the media isn't ready.
//  So the return value indicates both an error and whether the media is
//  ready or not.
//
tCIDLib::TBoolean TKrnlRemMediaDrv::bCheckIsReady()
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    if (pPlatData->hDevice == 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4Dummy;
    tCIDLib::TCard4 c4ErrCode;
    const tCIDLib::TBoolean bRes = bDoIOCTL
    (
        *pPlatData
        , IOCTL_STORAGE_CHECK_VERIFY
        , 0
        , 0
        , 0
        , c4Dummy
        , c4ErrCode
    );
    return bRes;
}


tCIDLib::TBoolean TKrnlRemMediaDrv::bIsOpen() const
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);
    return (pPlatData->hDevice != 0);
}


//
//  Locks the ejection mechanism on the drive based on the passed state.
//  True locks and false unlocks.
//
tCIDLib::TBoolean TKrnlRemMediaDrv::bLockEject(const tCIDLib::TBoolean bState)
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    if (pPlatData->hDevice == 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Set up the input structure
    PREVENT_MEDIA_REMOVAL Info = {0};
    Info.PreventMediaRemoval = bState ? 1 : 0;

    tCIDLib::TCard4 c4Dummy;
    tCIDLib::TCard4 c4ErrCode;
    const tCIDLib::TBoolean bRes = bDoIOCTL
    (
        *pPlatData
        , IOCTL_STORAGE_EJECTION_CONTROL
        , &Info
        , sizeof(Info)
        , 0
        , c4Dummy
        , c4ErrCode
    );
    return bRes;
}


tCIDLib::TBoolean
TKrnlRemMediaDrv::bMediaType(TKrnlRemMedia::EMediaTypes& eToFill) const
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    // We must be opened
    if (pPlatData->hDevice == 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        eToFill = TKrnlRemMedia::EMediaTypes::None;
        return kCIDLib::False;
    }

    // First try the get configuration call
    GET_CONFIGURATION_IOCTL_INPUT Input;
    TRawMem::SetMemBuf(&Input, tCIDLib::TCard1(0), sizeof(Input));
    Input.Feature = FeatureCdRead;

    GET_CONFIGURATION_HEADER Output = {0};
    tCIDLib::TCard4 c4ErrCode;
    tCIDLib::TCard4 c4OutBytes = sizeof(Output);
    const tCIDLib::TBoolean bRes = bDoIOCTL
    (
        *pPlatData
        , IOCTL_CDROM_GET_CONFIGURATION
        , &Input
        , sizeof(Input)
        , &Output
        , c4OutBytes
        , c4ErrCode
    );

    if (!bRes)
    {
        //
        //  See if the error indicates this function is unsupported. If so,
        //  then we return unknown and they'll just have to try to read
        //  some info and see.
        //
        if (c4ErrCode == ERROR_INVALID_FUNCTION)
        {
            eToFill = TKrnlRemMedia::EMediaTypes::Unknown;
            return kCIDLib::True;
        }
        return kCIDLib::False;
    }

    // The profile data is big endian
    tCIDLib::TCard4 c4Profile;
    #if defined(CIDLIB_LITTLEENDIAN)
    c4Profile = Output.CurrentProfile[0];
    c4Profile <<= 8;
    c4Profile |= Output.CurrentProfile[1];
    #else
    c4Profile = Output.CurrentProfile[1];
    c4Profile <<= 8;
    c4Profile |= Output.CurrentProfile[0];
    #endif

    switch(c4Profile)
    {
        case ProfileCdrom :
            eToFill = TKrnlRemMedia::EMediaTypes::CDR;
            break;

        case ProfileCdRecordable :
        case ProfileCdRewritable :
            eToFill = TKrnlRemMedia::EMediaTypes::CDRW;
            break;

        case ProfileDvdRam :
        case ProfileDvdRom :
        case ProfileDvdPlusR :
        case ProfileDvdPlusRDualLayer :
            eToFill = TKrnlRemMedia::EMediaTypes::DVDR;
            break;

        case ProfileDvdRecordable :
        case ProfileDvdRewritable :
        case ProfileDvdPlusRW :
        case ProfileDvdPlusRWDualLayer :
            eToFill = TKrnlRemMedia::EMediaTypes::DVDRW;
            break;

        case ProfileBDRom :
            eToFill = TKrnlRemMedia::EMediaTypes::BDR;
            break;

        case ProfileBDRSeqWritable :
        case ProfileBDRRandomWritable :
        case ProfileBDRewritable :
            eToFill = TKrnlRemMedia::EMediaTypes::BDRW;
            break;

        case ProfileHDDVDRom :
        case ProfileHDDVDRam :
        case ProfileHDDVDRDualLayer :
            eToFill = TKrnlRemMedia::EMediaTypes::HDDVDR;
            break;

        case ProfileHDDVDRecordable :
        case ProfileHDDVDRewritable :
        case ProfileHDDVDRWDualLayer :
            eToFill = TKrnlRemMedia::EMediaTypes::HDDVDRW;
            break;

        case ProfileInvalid :
            eToFill = TKrnlRemMedia::EMediaTypes::None;
            break;

        default :
            eToFill = TKrnlRemMedia::EMediaTypes::Unknown;
            break;
    };
    return kCIDLib::True;
}


//
//  Open or close the drive door based on the passed state. True means open
//  the door, else close the door if false.
//
tCIDLib::TBoolean TKrnlRemMediaDrv::bOpenDoor(const tCIDLib::TBoolean bState)
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    if (pPlatData->hDevice == 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4Dummy;
    tCIDLib::TCard4 c4ErrCode;
    const tCIDLib::TBoolean bRes = bDoIOCTL
    (
        *pPlatData
        , bState ? IOCTL_STORAGE_EJECT_MEDIA : IOCTL_STORAGE_LOAD_MEDIA
        , 0
        , 0
        , 0
        , c4Dummy
        , c4ErrCode
    );
    return bRes;
}


tCIDLib::TBoolean TKrnlRemMediaDrv::bOpen()
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    // Close any existing handle
    if (pPlatData->hDevice != 0)
    {
        ::CloseHandle(pPlatData->hDevice);
        pPlatData->hDevice = 0;
    }

    //
    //  Create a path in the form that is required for opening a device. The
    //  path we get from the outside is in the form that is normally used
    //  to represent volumes, e.g. "X:\". So we pull out the 0th char and put
    //  it into the device style path.
    //
    tCIDLib::TCh szPath[8] = L"\\\\.\\ :";
    szPath[4] = pPlatData->szDrivePath[0];

    // And try to open the new one
    pPlatData->hDevice = ::CreateFile
    (
        szPath
        , GENERIC_READ | GENERIC_WRITE
        , FILE_SHARE_READ | FILE_SHARE_WRITE
        , NULL
        , OPEN_EXISTING
        , FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL
        , NULL
    );

    if (pPlatData->hDevice == INVALID_HANDLE_VALUE)
    {
        pPlatData->hDevice = 0;
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_OpenFailed, ::GetLastError());
        return kCIDLib::False;
    }

    //
    //  Get the drive id info. We have to do a SCSI passthrough command
    //  here to get this info.
    //
    tCIDLib::TCard1 ac1InqData[100];
    tCIDLib::TCard1 ac1PTInfo[1024];

    #pragma CIDLIB_PACK(1)
    struct SPTD_WITH_BUFFER
    {
        SCSI_PASS_THROUGH_DIRECT PTInfo;
        ULONG Filler;
        UCHAR ucSenseBuf[32];
    };
    #pragma CIDLIB_POPPACK

    SPTD_WITH_BUFFER* pPTBuf = (SPTD_WITH_BUFFER*)ac1PTInfo;
    TRawMem::SetMemBuf(pPTBuf, tCIDLib::TCard1(0), sizeof(ac1PTInfo));

    pPTBuf->PTInfo.Length          = sizeof(SCSI_PASS_THROUGH);
    pPTBuf->PTInfo.CdbLength       = 6;
    pPTBuf->PTInfo.SenseInfoLength = 24;
    pPTBuf->PTInfo.DataIn          = SCSI_IOCTL_DATA_IN;
    pPTBuf->PTInfo.DataTransferLength = 100;
    pPTBuf->PTInfo.TimeOutValue    = 2;
    pPTBuf->PTInfo.DataBuffer      = ac1InqData;
    pPTBuf->PTInfo.SenseInfoOffset = c4FieldOfs(SPTD_WITH_BUFFER,ucSenseBuf);
    pPTBuf->PTInfo.Cdb[0]          = 0x12;
    pPTBuf->PTInfo.Cdb[4]          = 100;

    tCIDLib::TCard4 c4Len = sizeof(SPTD_WITH_BUFFER);
    tCIDLib::TCard4 c4ErrCode;
    const tCIDLib::TBoolean bRes = bDoIOCTL
    (
        *pPlatData
        , IOCTL_SCSI_PASS_THROUGH_DIRECT
        , pPTBuf
        , c4Len
        , pPTBuf
        , c4Len
        , c4ErrCode
    );

    if (!bRes)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcRemM_QueryId, ::GetLastError());
        ::CloseHandle(pPlatData->hDevice);
        pPlatData->hDevice = 0;
        return kCIDLib::False;
    }

    //
    //  Ok, we can get format out the data now. We assume it's ASCII data
    //  and do a cheap transcode to Unicode.
    //
    tCIDLib::TCard4 c4FmtInd = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
        m_szDriveId[c4FmtInd++] = tCIDLib::TCh(ac1InqData[c4Index + 8]);
    while ((c4FmtInd >= 1) && (m_szDriveId[c4FmtInd - 1] == kCIDLib::chSpace))
        c4FmtInd--;
    m_szDriveId[c4FmtInd++] = kCIDLib::chComma;
    m_szDriveId[c4FmtInd++] = kCIDLib::chSpace;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
        m_szDriveId[c4FmtInd++] = tCIDLib::TCh(ac1InqData[c4Index + 16]);
    while ((c4FmtInd >= 1) && (m_szDriveId[c4FmtInd - 1] == kCIDLib::chSpace))
        c4FmtInd--;
    m_szDriveId[c4FmtInd++] = kCIDLib::chSpace;
    m_szDriveId[c4FmtInd++] = kCIDLib::chLatin_v;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
        m_szDriveId[c4FmtInd++] = tCIDLib::TCh(ac1InqData[c4Index + 32]);
    m_szDriveId[c4FmtInd] = kCIDLib::chNull;

    return kCIDLib::True;
}


//
//  If this object is opened, then we read the table of contents data into
//  the caller's TOC structure.
//
tCIDLib::TBoolean
TKrnlRemMediaDrv::bQueryCDTOC(TKrnlRemMedia::TCDTOCInfo& ToFill)
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    if (pPlatData->hDevice == 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4IOSz = sizeof(TKrnlRemMedia::TCDTOCInfo);
    TRawMem::SetMemBuf(&ToFill, tCIDLib::TCard1(0), c4IOSz);
    tCIDLib::TCard4 c4ErrCode;
    const tCIDLib::TBoolean bRes = bDoIOCTL
    (
        *pPlatData
        , IOCTL_CDROM_READ_TOC
        , 0
        , 0
        , &ToFill
        , c4IOSz
        , c4ErrCode
    );

    // Adjust the track numbers to be zero based
    if (bRes)
    {
        const tCIDLib::TCard1 c1Cnt = (ToFill.c1LastTrack - ToFill.c1FirstTrack) + 1;
        ToFill.c1FirstTrack = 0;
        ToFill.c1LastTrack = c1Cnt - 1;
        for (tCIDLib::TCard1 c1Index = 0; c1Index < c1Cnt; c1Index++)
            ToFill.aTracks[c1Index].c1TrackNumber = c1Index;
    }
    return bRes;
}


//
//  Reads c4Count sectors (hopefully) from sector c4Start. It may not return
//  exactly the sectors requested, in which case the caller must stitch the
//  previous and current data together by finding the overlap.
//
tCIDLib::TBoolean
TKrnlRemMediaDrv::bReadSectors( const   tCIDLib::TCard4         c4Start
                                , const tCIDLib::TCard4         c4Count
                                ,       tCIDLib::TCard1* const  pc1Buf
                                ,       tCIDLib::TCard4&        c4Bytes)
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    if (pPlatData->hDevice == 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Set up a raw info structure to indicate the data we want
    RAW_READ_INFO RawInfo;
    RawInfo.DiskOffset.QuadPart = c4Start * TKrnlRemMedia::c4CDCookedSectorSz;
    RawInfo.TrackMode = CDDA;
    RawInfo.SectorCount = c4Count;

    tCIDLib::TCard4 c4ErrCode;
    tCIDLib::TBoolean bRes = bDoIOCTL
    (
        *pPlatData
        , IOCTL_CDROM_RAW_READ
        , &RawInfo
        , sizeof(RawInfo)
        , pc1Buf
        , c4Bytes
        , c4ErrCode
    );

    //
    //  We have to do a bit of a kludge here. Some drives report ready, but
    //  then you open them and they act like they've seen a media change and
    //  fail. So, if we get one, we'll close the drive, re-open it again, and
    //  retry the read. If it fails a second time, we give up. In theory this
    //  should only happen once and then we'll be ok, or it'll fail the
    //  second time and we'll be closed. So we won't be constantly closing
    //  and re-opening it.
    //
    //  We will also retry in the case of a basic device error, since the
    //  removeable drives sometimes glitch.
    //
    if (!bRes
    &&  ((c4ErrCode == ERROR_MEDIA_CHANGED) || (c4ErrCode == ERROR_IO_DEVICE)))
    {
        // Remember the original last error
        TKrnlError kerrOrg = TKrnlError::kerrLast();

        // Close and try to reopen if it was a changed error
        bRes = kCIDLib::True;
        if (c4ErrCode == ERROR_MEDIA_CHANGED)
        {
            Close();
            bRes = bOpen();

            // If we cannot re-open, put the original error back
            if (!bRes)
                TKrnlError::SetLastError(kerrOrg);
        }
         else
        {
            // Back off for just a bit
            ::Sleep(50);
        }

        // Try the operation again if we can
        if (bRes)
        {
            RawInfo.DiskOffset.QuadPart = c4Start * TKrnlRemMedia::c4CDCookedSectorSz;
            RawInfo.TrackMode = CDDA;
            RawInfo.SectorCount = c4Count;
            bRes = bDoIOCTL
            (
                *pPlatData
                , IOCTL_CDROM_RAW_READ
                , &RawInfo
                , sizeof(RawInfo)
                , pc1Buf
                , c4Bytes
                , c4ErrCode
            );
        }
    }
    return bRes;
}


// Set's a new device path on this object
tCIDLib::TBoolean TKrnlRemMediaDrv::bSetPath(const tCIDLib::TCh* const pszToSet)
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    // Free up any existing stuff
    Close();

    TRawStr::CopyStr
    (
        pPlatData->szDrivePath
        , pszToSet
        , c4MaxBufChars(pPlatData->szDrivePath)
    );
    return kCIDLib::True;
}


// Close down any existing device handle
tCIDLib::TVoid TKrnlRemMediaDrv::Close()
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);

    if (pPlatData->hDevice)
    {
        ::CloseHandle(pPlatData->hDevice);
        pPlatData->hDevice = 0;
    }

    // Initialize the drive id to empty again
    m_szDriveId[0] = kCIDLib::chNull;
}


const tCIDLib::TCh* TKrnlRemMediaDrv::pszDriveId() const
{
    return m_szDriveId;
}


const tCIDLib::TCh* TKrnlRemMediaDrv::pszDrivePath() const
{
    CIDKernel_RemMedia::TPlatData* pPlatData
                = reinterpret_cast<CIDKernel_RemMedia::TPlatData*>(m_pPlatData);
    return pPlatData->szDrivePath;
}





// ---------------------------------------------------------------------------
//   NAMESPACE: TKrnlRemMedia
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlRemMedia: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Enumerates the devices and returns a new line separate list of device
//  paths for all the DVD/CDROMs found.
//
tCIDLib::TBoolean
TKrnlRemMedia::bEnumDrvs(TKrnlLList<TKrnlString>& kllstToFill)
{
    kllstToFill.RemoveAll();

    // Get the drive map and loop through them looking for CDROMS
    const tCIDLib::TCard4 c4DriveMap = ::GetLogicalDrives();
    tCIDLib::TCard4 c4DrvType;
    tCIDLib::TCh szVol[] = L" :\\";
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 32; c4Index++)
    {
        // If no drive here, then obviously skip this one
        if ((c4DriveMap & (0x1UL << c4Index)) == 0)
            continue;

        // There's a drive here, so check the type
        szVol[0] = tCIDLib::TCh(L'A' + c4Index);
        c4DrvType = ::GetDriveType(szVol);

        // Take it if it's a CD drive
        if (c4DrvType == DRIVE_CDROM)
            kllstToFill.pobjAddNew(new TKrnlString(szVol));
    }
    return kCIDLib::True;
}


//
//  Given a table of contents, and a track number, it extracts some useful
//  info from the track data.
//
tCIDLib::TBoolean
TKrnlRemMedia::bExtractCDTrackInfo( const   TKrnlRemMedia::TCDTOCInfo&  SrcTOC
                                    , const tCIDLib::TCard4             c4TrackNum
                                    ,       tCIDLib::TCard4&            c4StartBlock
                                    ,       tCIDLib::TCard4&            c4BlockCnt
                                    ,       tCIDLib::TCard4&            c4Minutes
                                    ,       tCIDLib::TCard4&            c4Seconds)
{
    // Make sure it's a valid track
    if (c4TrackNum > SrcTOC.c1LastTrack)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_IndexError);
        return kCIDLib::False;
    }

    // Get a pointer to the track structure
    const TCDTrackInfo& CurTrack = SrcTOC.aTracks[c4TrackNum];

    // Do the calculations
    c4StartBlock = c4MSFToBlocks(CurTrack.ac1Addr);
    c4BlockCnt = c4MSFToBlocks(SrcTOC.aTracks[c4TrackNum + 1].ac1Addr) - c4StartBlock;

    const tCIDLib::TCard4 c4Secs = c4BlockCnt / TKrnlRemMedia::c4CDBlksPerSec;

    c4Minutes = c4Secs / 60;
    c4Seconds = c4Secs - (c4Minutes * 60);

    return kCIDLib::True;
}


//
//  Helpers to check a media type and see if it's of some general type, which
//  is usually all the program needs to know.
//
tCIDLib::TBoolean
TKrnlRemMedia::bIsBDType(const TKrnlRemMedia::EMediaTypes eToCheck)
{
    return (eToCheck == TKrnlRemMedia::EMediaTypes::BDR)
           || (eToCheck == TKrnlRemMedia::EMediaTypes::BDRW);
}

tCIDLib::TBoolean
TKrnlRemMedia::bIsCDType(const TKrnlRemMedia::EMediaTypes eToCheck)
{
    return (eToCheck == TKrnlRemMedia::EMediaTypes::CDR)
           || (eToCheck == TKrnlRemMedia::EMediaTypes::CDRW);
}

tCIDLib::TBoolean
TKrnlRemMedia::bIsDVDType(const TKrnlRemMedia::EMediaTypes eToCheck)
{
    return (eToCheck == TKrnlRemMedia::EMediaTypes::DVDR)
           || (eToCheck == TKrnlRemMedia::EMediaTypes::DVDRW);
}

tCIDLib::TBoolean
TKrnlRemMedia::bIsHDDVDType(const TKrnlRemMedia::EMediaTypes eToCheck)
{
    return (eToCheck == TKrnlRemMedia::EMediaTypes::HDDVDR)
           || (eToCheck == TKrnlRemMedia::EMediaTypes::HDDVDRW);
}

tCIDLib::TBoolean
TKrnlRemMedia::bIsReadOnly(const TKrnlRemMedia::EMediaTypes eToCheck)
{
    return (eToCheck == TKrnlRemMedia::EMediaTypes::BDR)
           || (eToCheck == TKrnlRemMedia::EMediaTypes::CDR)
           || (eToCheck == TKrnlRemMedia::EMediaTypes::DVDR)
           || (eToCheck == TKrnlRemMedia::EMediaTypes::HDDVDR);
}

tCIDLib::TBoolean
TKrnlRemMedia::bIsValidType(const TKrnlRemMedia::EMediaTypes eToCheck)
{
    return (eToCheck != TKrnlRemMedia::EMediaTypes::None)
            && (eToCheck != TKrnlRemMedia::EMediaTypes::Unknown);
}


//
//  Converts an MSF style time stamp into an LBA block number. Note that
//  there are two seconds of silence at the start, which isn't included in
//  the MSF, so we have to move backwards 150 blocks from the calculated
//  block number.
//
tCIDLib::TCard4 TKrnlRemMedia::c4MSFToBlocks(const tCIDLib::TCard1 ac1MSF[4])
{
    tCIDLib::TCard4 c4Block
    (
        (ac1MSF[1] * (c4CDBlksPerSec * 60))
        + (ac1MSF[2] * c4CDBlksPerSec)
        + ac1MSF[3]
    );
    return c4Block - 150;
}


