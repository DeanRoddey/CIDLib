//
// FILE NAME: CIDKernel_CDROM.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2006
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
//  This is the header for the CIDKernel_RemMedia.Cpp file. This file implements
//  the TKrnlRemMedia class, which encapsulates the functionality required to
//  do low level CDROM and DVD access, for for doing CD ripping and for getting
//  TOC info for CD and DVD ROM discs.
//
//  Note that all CD track numbers are zero based. They are 1 based on the CD,
//  but we convert them to zero based before the outside world sees them. It's
//  just too much of a pain to deal with mixed indexing. In the few places
//  where a 1 based index is required, we handle that.
//
//  This file is seldom used so it is not included in the main facility
//  public header. Anyone who wants to use it can include it directly.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

//
//  We need to have some stuff first, then we'll have some classes, and then
//  the rest of the namespace contents.
//
namespace TKrnlRemMedia
{
    // -----------------------------------------------------------------------
    //  We have to expose the CD TOC stuff, so we have to have our own version
    //  of this stuff.
    // -----------------------------------------------------------------------
    #pragma CIDLIB_PACK(1)
    struct TCDTrackInfo
    {
        tCIDLib::TCard1 c1Rsvd;
        tCIDLib::TCard1 c1Addr;
        tCIDLib::TCard1 c1TrackNumber;
        tCIDLib::TCard1 c1Rsvd2;
        tCIDLib::TCard1 ac1Addr[4];
    };

    struct TCDTOCInfo
    {
        tCIDLib::TCard2 c2TOCLen;
        tCIDLib::TCard1 c1FirstTrack;
        tCIDLib::TCard1 c1LastTrack;
        TCDTrackInfo    aTracks[100];
    };
    #pragma CIDLIB_POPPACK

    // Number of LBA blocks per second
    constexpr tCIDLib::TCard4 c4CDBlksPerSec = 75;

    // Number of bytes in a raw sector
    constexpr tCIDLib::TCard4 c4CDRawSectorSz = 2352;

    // Number of bytes in a cookied sector
    constexpr tCIDLib::TCard4 c4CDCookedSectorSz = 2048;

    // Max chars we'll store for a drive id
    constexpr tCIDLib::TCard4 c4MaxDriveIdLen = 64;


    // -----------------------------------------------------------------------
    //  Returned by eMediaType() below, and a text map for them.
    // -----------------------------------------------------------------------
    enum class EMediaTypes
    {
        None
        , Unknown
        , CDR
        , CDRW
        , DVDR
        , DVDRW
        , BDR
        , BDRW
        , HDDVDR
        , HDDVDRW
    };

    const tCIDLib::TCh* const apszMediaTypes[] =
    {
        L"None"
        , L"Unknown"
        , L"CD-R"
        , L"CR-RW"
        , L"DVD-R"
        , L"DVD-RW"
        , L"BD-R"
        , L"BD-RW"
        , L"HDDVD-R"
        , L"HDDVD-RW"
    };
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlRemMediaDrv
//  PREFIX: krmmd
//
//  This is a simple class to manipulate removable media drives.
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlRemMediaDrv
{
    public :
        // -------------------------------------------------------------------
        //  Forward ref our per-platform data structure, which is opaque publically
        // -------------------------------------------------------------------
        struct TPlatData;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlRemMediaDrv();

        TKrnlRemMediaDrv
        (
            const   tCIDLib::TCh* const     pszDevicePath
        );

        TKrnlRemMediaDrv(const TKrnlRemMediaDrv&) = delete;

        ~TKrnlRemMediaDrv();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TKrnlRemMediaDrv& operator=(const TKrnlRemMediaDrv&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckIsReady();

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TBoolean bLockEject
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TBoolean bOpenDoor
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TBoolean bOpen();

        tCIDLib::TBoolean bMediaType
        (
                    TKrnlRemMedia::EMediaTypes& eToFill
        )   const;

        tCIDLib::TBoolean bQueryCDTOC
        (
                    TKrnlRemMedia::TCDTOCInfo&   ToFill
        );

        tCIDLib::TBoolean bReadSectors
        (
            const   tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Count
            ,       tCIDLib::TCard1* const  pc1Buf
            ,       tCIDLib::TCard4&        c4Bytes
        );

        tCIDLib::TBoolean bSetPath
        (
            const   tCIDLib::TCh* const     pszToSet
        );

        tCIDLib::TVoid Close();

        const tCIDLib::TCh* pszDriveId() const;

        const tCIDLib::TCh* pszDrivePath() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4DriveNum
        //      The number of the drive, in whatever format makes sense for
        //      the local platform.
        //
        //  m_pPlatData
        //      Any platform data that the per-platform layer requires. It is
        //      opaque to the outside world.
        //
        //  m_szDriveId
        //      This holds the drive id info, which is what get's displayed
        //      to the user to identify the drive.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4DriveNum;
        TPlatData*      m_pPlatData;
        tCIDLib::TCh    m_szDriveId[TKrnlRemMedia::c4MaxDriveIdLen + 1];
};



// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlRemMedia
// ---------------------------------------------------------------------------
namespace TKrnlRemMedia
{
    KRNLEXPORT tCIDLib::TBoolean bEnumDrvs
    (
                TKrnlLList<TKrnlString>& kllstToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bExtractCDTrackInfo
    (
        const   TKrnlRemMedia::TCDTOCInfo& SrcTOC
        , const tCIDLib::TCard4         c4TrackNum
        ,       tCIDLib::TCard4&        c4StartBlock
        ,       tCIDLib::TCard4&        c4BlockCnt
        ,       tCIDLib::TCard4&        c4Minutes
        ,       tCIDLib::TCard4&        c4Seconds
    );

    KRNLEXPORT tCIDLib::TBoolean bIsBDType
    (
        const   EMediaTypes             eToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsCDType
    (
        const   EMediaTypes             eToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsDVDType
    (
        const   EMediaTypes             eToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsHDDVDType
    (
        const   EMediaTypes             eToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsReadOnly
    (
        const   EMediaTypes             eToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsValidType
    (
        const   EMediaTypes             eToCheck
    );

    KRNLEXPORT tCIDLib::TCard4 c4MSFToBlocks
    (
        const   tCIDLib::TCard1         ac1MSF[4]
    );
}

#pragma CIDLIB_POPPACK


