//
// FILE NAME: CIDKernel_DVDTypes.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 14/17/2006
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
//  This is a specialized header for programs that need to read the IFO
//  files and other content from DVD files. It defines the structures and
//  types that are used in the files.
//
// CAVEATS/GOTCHAS:
//
//  1)  This file is not included normally. Facilities that need it must
//      include it directly.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// This stuff is all byte packed
#pragma CIDLIB_PACK(1)


// ---------------------------------------------------------------------------
//  General constants
// ---------------------------------------------------------------------------
namespace kDVDTypes
{
    const   tCIDLib::TCard4     c4MaxTitles     = 99;
    const   tCIDLib::TCard4     c4MaxChapters   = 999;
    const   tCIDLib::TCard4     c4SectorSz      = 2048;
};



// ---------------------------------------------------------------------------
//  Types
// ---------------------------------------------------------------------------
namespace tDVDTypes
{
    // -----------------------------------------------------------------------
    //  Some generally used structures
    // -----------------------------------------------------------------------
    struct TDVDTime
    {
        tCIDLib::TCard1     c1Hour;
        tCIDLib::TCard1     c1Minute;
        tCIDLib::TCard1     c1Second;
        tCIDLib::TCard1     c1Frames;
    };

    struct TVMCmd
    {
        tCIDLib::TCard1     ac1CmdBytes[8];
    };


    // -----------------------------------------------------------------------
    //  These structures make up the per-title IFO files, the ones in the form
    //  VTS_xx_0.IFO.
    // -----------------------------------------------------------------------

    // Video attribute flags, which for now we just have a place holder for
    struct TVideoAttrs
    {
        tCIDLib::TCard2     c2Flags;
    };

    // Audio attributes
    struct TAudioAttrs
    {
        tCIDLib::TCard2     c2Flags1;
        tCIDLib::TCard2     c2LangCode;
        tCIDLib::TCard1     c1LangExt;
        tCIDLib::TCard1     c1CodeExt;
        tCIDLib::TCard1     c1Dummy1;
        tCIDLib::TCard2     c2Flags2;
        tCIDLib::TCard2     c2Flags3;
    };

    // Sub-pic attributes
    struct TSubPicAttrs
    {
        tCIDLib::TCard1     c1Flags1;
        tCIDLib::TCard1     c1Zero1;
        tCIDLib::TCard2     c2LangCode;
        tCIDLib::TCard1     c1LangExt;
        tCIDLib::TCard1     c1CodeExt;
    };


    // Multichannel extension
    struct TMultiChExt
    {
        tCIDLib::TCard4     c4Flags1;
        tCIDLib::TCard1     c1Flags2;
        tCIDLib::TCard1     ac1Zero1[19];
    };


    // Program Chain Information Table stuff
    struct TPGCCmdTbl
    {
        tCIDLib::TCard2     c2NumPre;
        tCIDLib::TCard2     c2NumPost;
        tCIDLib::TCard2     c2NumCell;
        tCIDLib::TCard2     c2Zero1;
        TVMCmd              PreCmds;
        TVMCmd              PostCmds;
        TVMCmd              CellCmds;
    };

    struct TCellPlayback
    {
        tCIDLib::TCard2     c2Flags;
        tCIDLib::TCard1     c1StillTime;
        tCIDLib::TCard1     c1CellCmdNum;
        TDVDTime            PlaybackTime;
        tCIDLib::TCard4     c4FirstSector;
        tCIDLib::TCard4     c4FrstILVUEndSector;
        tCIDLib::TCard4     c4LastVobuStartSector;
        tCIDLib::TCard4     c4LastSector;
    };

    struct TCellPos
    {
        tCIDLib::TCard2     c2VOBIdNum;
        tCIDLib::TCard1     c1Zero1;
        tCIDLib::TCard1     c1CellNum;
    };

    struct TProgramChain
    {
        tCIDLib::TCard2     c2Zero1;
        tCIDLib::TCard1     c1NumPrograms;
        tCIDLib::TCard1     c1NumCells;
        TDVDTime            PlaybackTime;
        tCIDLib::TCard4     c4ProhibitedOps;
        tCIDLib::TCard2     c2AudioControl[8];
        tCIDLib::TCard4     c4SubpControl[32];
        tCIDLib::TCard2     c2NextPCRNum;
        tCIDLib::TCard2     c2PrevPGCNum;
        tCIDLib::TCard2     c2GroupPGCNum;
        tCIDLib::TCard1     c1StillTime;
        tCIDLib::TCard1     c1PgPlaybackMode;
        tCIDLib::TCard4     c4Palette[16];
        tCIDLib::TCard2     c2CmdTblOfs;
        tCIDLib::TCard2     c2ProgramMapOfs;
        tCIDLib::TCard2     c2CellPlaybackOfs;
        tCIDLib::TCard2     c2CellPositionOfs;
    };

    struct TPCSPtrTbl
    {
        tCIDLib::TCard1     c1EntryId;
        tCIDLib::TCard1     c1Flags;
        tCIDLib::TCard2     c2PTLIdMask;
        tCIDLib::TCard4     c4PGCStartByte;
    };

    struct TProgChainITbl
    {
        tCIDLib::TCard2     c2NumPtrs;
        tCIDLib::TCard2     c2Zero1;
        tCIDLib::TCard4     c4LastByte;
    };


    // The part of title stuff (PTT_SRPTI)
    struct TPTTSPtrTbl
    {
        tCIDLib::TCard2     c2NumPtrs;
        tCIDLib::TCard2     c2Zero1;
        tCIDLib::TCard4     c4LastByte;
    };

    struct TTitleUnit
    {
        tCIDLib::TCard2     c2NumPTTs;
    };

    struct TPTTInfo
    {
        tCIDLib::TCard2     c2ChainNum;
        tCIDLib::TCard2     c2ProgNum;
    };

    // The main VTS structure which is at the start of the file
    struct TVTSIMgTbl
    {
        tCIDLib::TSCh       szId[12];
        tCIDLib::TCard4     c4VTSLastSector;
        tCIDLib::TCard1     ac1Zero1[12];
        tCIDLib::TCard4     c4VTSILastSector;
        tCIDLib::TCard1     c1Zero2;
        tCIDLib::TCard1     c1SpecificationVersion;
        tCIDLib::TCard4     c4VTSCategory;
        tCIDLib::TCard2     c2Zero3;
        tCIDLib::TCard2     c2Zero4;
        tCIDLib::TCard1     c1Zero5;
        tCIDLib::TCard1     ac1Zero6[19];
        tCIDLib::TCard2     c2Zero7;
        tCIDLib::TCard1     ac1Zero8[32];
        tCIDLib::TCard8     c8Zero9;
        tCIDLib::TCard1     ac1Zero10[24];
        tCIDLib::TCard4     c4VTSILastByte;
        tCIDLib::TCard4     c4Zero11;
        tCIDLib::TCard1     ac1Zero12[56];
        tCIDLib::TCard4     c4VTSM_Vobs;
        tCIDLib::TCard4     c4VTSTTVobs;
        tCIDLib::TCard4     c4VTSPTTSRPT;
        tCIDLib::TCard4     c4VTSPGCIT;
        tCIDLib::TCard4     c4VTSMPGCIUT;
        tCIDLib::TCard4     c4VTSTMapT;
        tCIDLib::TCard4     c4VTSMCADT;
        tCIDLib::TCard4     c4VTSMVobUADMap;
        tCIDLib::TCard4     c4VTSCADT;
        tCIDLib::TCard4     c4VTSVobUADMap;
        tCIDLib::TCard1     ac1Zero13[24];

        TVideoAttrs         VTSMVideoAttr;
        tCIDLib::TCard1     c1Zero14;
        tCIDLib::TCard1     c1NumVTSMAudioStreams;  // 0 or 1
        TAudioAttrs         TVTSMAudioAttr;
        TAudioAttrs         aZero15[7];
        tCIDLib::TCard1     c1Zero16[17];
        tCIDLib::TCard1     c1NumVTSMSubpStreams;   // 0 or 1
        TSubPicAttrs        TVTSMSubpAttr;
        TSubPicAttrs        aZero17[27];
        tCIDLib::TCard1     c1Zero18[2];

        TVideoAttrs         VTSVideoAttr;
        tCIDLib::TCard1     c1Zero19;
        tCIDLib::TCard1     c1NumVTSAudioStreams;
        TAudioAttrs         aVTSAudioAttr[8];
        tCIDLib::TCard1     ac1Zero20[17];
        tCIDLib::TCard1     c1NumVTSSubpStreams;
        TSubPicAttrs        aVTSSubpAttr[32];
        tCIDLib::TCard2     c2Zero21;
        TMultiChExt         aVTSMUAudioAttr[8];
    };
}


// A macro to access fields, taking into account endianness
#if defined(CID_BIGENDIAN)
#define c2DVDTypeFld(o,f) o.f
#define c4DVDTypeFld(o,f) o.f
#else
#define c2DVDTypeFld(o,f) TRawBits::c2SwapBytes(o.f)
#define c4DVDTypeFld(o,f) TRawBits::c4SwapBytes(o.f)
#endif

// A few helper methods
namespace TDVDTypes
{
    KRNLEXPORT tCIDLib::TCard8 c8DVDTimeToFrames
    (
        const   tDVDTypes::TDVDTime&    ToCvt
    );
};

#pragma CIDLIB_POPPACK

