//
// FILE NAME: CIDMetaExtr_Sony777DVDID.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a unique DVD ID generator for the Sony 777/AMG
//  ID format.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMetaExtr_.hpp"
#include    "CIDKernel_DVDTypes.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDSony777DVDID,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TCIDSony777DVDID
// PREFIX: dvdid
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDSony777DVDID: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDSony777DVDID::TCIDSony777DVDID()
{
}

TCIDSony777DVDID::~TCIDSony777DVDID()
{
}


// ---------------------------------------------------------------------------
//  TCIDSony777DVDID: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Given the path to the root of a DVD in a drive, we will generate a text
//  formatted unique id.
//
tCIDLib::TVoid
TCIDSony777DVDID::GenSony777DVDID(  const   TString&    strDVDPath
                                    ,       TString&    strFmtID)
{
    #if defined(CIDLIB_BIGENDIAN)
    #define c2GetValAt(p,o) (*(tCIDLib::TCard2*)(((tCIDLib::TCard1*)p) + o))
    #define c4GetValAt(p,o) (*(tCIDLib::TCard4*)(((tCIDLib::TCard1*)p) + o))
    #else
    #define c2GetValAt(p,o) TRawBits::c2SwapBytes(*(tCIDLib::TCard2*)(((tCIDLib::TCard1*)p) + o))
    #define c4GetValAt(p,o) TRawBits::c4SwapBytes(*(tCIDLib::TCard4*)(((tCIDLib::TCard1*)p) + o))
    #endif

    // Build up the path to the VIDEO_TS directory and make sure it exists
    TPathStr pathVTS(strDVDPath);
    pathVTS.AddLevel(L"VIDEO_TS");

    if (!TFileSys::bIsDirectory(pathVTS))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcDVDID_BadPath
            , pathVTS
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    //
    //  Open the main IFO file, and we'll gather up some info:
    //
    //  For each Title
    //      VTS IFO file it is in
    //      Number of chapters
    //      Title number within that VTS file
    //
    TPathStr pathCur(pathVTS);
    pathCur.AddLevel(L"VIDEO_TS.IFO");

    //
    //  It's there so open a the file. We set up a list of up to 99 title
    //  structures that holds the info we need to find what we need.
    //
    tCIDLib::TCard4 c4TitleCnt;
    struct
    {
        tCIDLib::TCard1 c1VTSNum;
        tCIDLib::TCard2 c2Chapters;
        tCIDLib::TCard1 c1TitleNum;
    }   aTitles[99];

    tCIDLib::TCard4  c4BufSz;
    tCIDLib::TCard4  c4Ofs;
    tCIDLib::TCard1* pc1Buf;
    tCIDLib::TCard1* pc1Ptr;
    {
        pc1Buf = pc1ReadDVDFile(pathCur, c4BufSz);
        TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

        // Get a pointer to the VGM_PTT_SRPT, and let's go through the list
        c4Ofs = c4GetValAt(pc1Buf, 0xC4);
        pc1Ptr = pc1Buf + (c4Ofs * kDVDTypes::c4SectorSz);

        // First is the count of titles, then move up to the PPT items
        c4TitleCnt = c2GetValAt(pc1Ptr, 0);
        pc1Ptr += 8;

        //
        //  For each title, there are 12 bytes, and we want to get out
        //  the three values we keep up with for each title.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TitleCnt; c4Index++)
        {
            aTitles[c4Index].c2Chapters = c2GetValAt(pc1Ptr, 2);
            aTitles[c4Index].c1VTSNum   = *(pc1Ptr + 6);
            aTitles[c4Index].c1TitleNum = *(pc1Ptr + 7);
            pc1Ptr += 12;
        }
    }

    //
    //  Loop through all the titles and put out the info for each of them
    //  and their chapters.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TitleCnt; c4Index++)
    {
        // Get out the info for this one
        const tCIDLib::TCard1 c1VTSNum   = aTitles[c4Index].c1VTSNum;
        const tCIDLib::TCard2 c2Chapters = aTitles[c4Index].c2Chapters;
        const tCIDLib::TCard1 c1TitleNum = aTitles[c4Index].c1TitleNum;

        // Put out the title number for this one
        if (c4Index + 1 < 0x10)
            strFmtID.Append(L'0');
        strFmtID.AppendFormatted(c4Index + 1, tCIDLib::ERadices::Hex);

        // And put out the number of chapters
        if (c2Chapters < 0x10)
            strFmtID.Append(L'0');
        strFmtID.AppendFormatted(c2Chapters, tCIDLib::ERadices::Hex);

        //
        //  We need to open up the VTS file indicated for this title, so
        //  build up the path name.
        //
        pathCur.CapAt(pathVTS.c4Length());
        pathCur.AddLevel(L"VTS_");
        if (c1VTSNum < 10)
            pathCur.Append(L'0');
        pathCur.AppendFormatted(c1VTSNum);
        pathCur.Append(L"_0.IFO");

        // And now let's open it up and process it
        try
        {
            pc1Buf = pc1ReadDVDFile(pathCur, c4BufSz);
        }

        catch(TError& errToCatch)
        {
            // It failed, so see if there is a BUP file
            pathCur.bRemoveExt();
            pathCur.AppendExt(L"BUP");

            // If not, then just rethrow original error
            if (!TFileSys::bExists(pathCur))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            // It's there to try it. If not, this throws back to the caller
            pc1Buf = pc1ReadDVDFile(pathCur, c4BufSz);
        }

        // Put a janitor on the buffer now and process it
        {
            TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

            //
            //  Ok, now we need to move up to the PTT_SRPTI section of this
            //  one. This has a table that tells us what program chains/programs
            //  make up each chapter of this title.
            //
            //  At offset 0xC8 is the offset of the PTT_SRPTI section.
            //
            c4Ofs = c4GetValAt(pc1Buf, 0xC8);
            pc1Ptr = pc1Buf + (c4Ofs * kDVDTypes::c4SectorSz);

            //
            //  At offset 8 from this is the starting offset of the table.
            //  There is a Card2 value that indicates each title's offset
            //  into the title unit table.
            //
            pc1Ptr += c4GetValAt(pc1Ptr, 8 + (4 * (c1TitleNum - 1)));

            //
            //  Now there are c2Chapters sets of Card2 values. Each set
            //  indicates the chain and program number for the Part of Title
            //  that makes up a chapter.
            //
            GenChapterLen(pc1Buf, (pc1Ptr - pc1Buf), c2Chapters, strFmtID);
        }
    };
}


// ---------------------------------------------------------------------------
//  TCIDSony777DVDID: Public, non-virtual methods
// ---------------------------------------------------------------------------


//
//  This is called to spit out a chapter length. We get a pointer to a TTU
//  table and the number of Part of Title entries to expect. These are sets
//  of two 16 bit values, which indicate the program chain and program number
//  of the units that make up a chapter. So for each one, we calculate the
//  runtime in frames of the chapter and spit it out as an 8 byte hext value.
//
tCIDLib::TVoid
TCIDSony777DVDID::GenChapterLen(const   tCIDLib::TCard1* const  pc1Buf
                                , const tCIDLib::TCard4         c4TblOfs
                                , const tCIDLib::TCard4         c4PTTCnt
                                ,       TString&                strFmtID) const
{
    // We need to move up to the VTS_PGCITI
    const tCIDLib::TCard1* pc1PGCTbl
    (
        pc1Buf
        + (c4GetValAt(pc1Buf, 0xCC) * kDVDTypes::c4SectorSz)
    );

    // Get the number of program chain entries
//    const tCIDLib::TCard4 c4ChainCnt = c4GetValAt(pc1PGCTbl, 0);

    // Loop through all the chapters
    const tCIDLib::TCard1* pc1TblPtr = pc1Buf + c4TblOfs;
    TString strFld;
    TString strVal;
    for (tCIDLib::TCard4 c4PTTIndex = 0; c4PTTIndex < c4PTTCnt; c4PTTIndex++)
    {
        // Get the program chain and program number for this one
        const tCIDLib::TCard2 c2ChainNum = c2GetValAt(pc1TblPtr, 0);
        const tCIDLib::TCard2 c2ProgNum = c2GetValAt(pc1TblPtr, 2);
        pc1TblPtr += 4;

        //
        //  Ok, let's locate the table for this guy. We can find the program
        //  chain offset at 8 + (chain * 8). This is a structure that contains
        //  info for the chains. At 8 bytes from that is the offset of the
        //  program chain.
        //
        const tCIDLib::TCard2 c2PCOfs
        (
            c2GetValAt(pc1PGCTbl, 6 + 8 + (8 * (c2ChainNum - 1)))
        );

        // So this gets us to the program chain
        const tCIDLib::TCard1* pc1PChain = pc1PGCTbl + c2PCOfs;

        // Get the total number of programs and cells
        const tCIDLib::TCard1 c1ProgCnt = *(pc1PChain + 2);
        const tCIDLib::TCard1 c1CellCnt = *(pc1PChain + 3);

        //
        //  Within that, at 0xe6 is the offset of the program map. So the
        //  chain pointer plus this offset is the table, then move forward
        //  to our program's entry. This gets us the starting cell number.
        //
        const tCIDLib::TCard1 c1StartCell
        (
            *(pc1PChain + c2GetValAt(pc1PChain, 0xe6) + (c2ProgNum - 1))
        );

        //
        //  Now find the ending cell. If this is the last program, then we
        //  take the cell count + 1. Else, we get the starting cell of the
        //  next chapter. That, minus the starting cell, is the cell count.
        //
        tCIDLib::TCard1 c1EndCell;
        if (c2ProgNum == c1ProgCnt)
            c1EndCell = c1CellCnt + 1;
        else
            c1EndCell = *(pc1PChain + c2GetValAt(pc1PChain, 0xe6) + c2ProgNum);

        // Calculate the count of cells
        const tCIDLib::TCard4 c4OurCellCnt = c1EndCell - c1StartCell;

        //
        //  Let's locate this cell. At 0xE8 within the program chain is the
        //  the cell playback table. Each one is 24 bytes so move forward by
        //  24 times our cell's offset, and then 4 bytes within that is the
        //  DVD time we want.
        //
        tCIDLib::TCard8 c8Frames = 0;
        const tDVDTypes::TDVDTime* pTime = (const tDVDTypes::TDVDTime*)
        (
            pc1PChain + c2GetValAt(pc1PChain, 0xE8) + ((c1StartCell - 1) * 24) + 4
        );
        for (tCIDLib::TCard4 c4CInd = 0; c4CInd < c4OurCellCnt; c4CInd++)
        {
            c8Frames += TDVDTypes::c8DVDTimeToFrames(*pTime);
            pTime = tCIDLib::pOffsetPtr<tDVDTypes::TDVDTime>(pTime, 24UL);
        }

        // Format this out to an 8 character hex digit and append to TOC
        strVal.SetFormatted(c8Frames, tCIDLib::ERadices::Hex);
        strFld.FormatToFld(strVal, 8, tCIDLib::EHJustify::Right, L'0');
        strFmtID.Append(strFld);
    }
}


//
//  This is a helper to read in teh contents of the DVD files we are
//  interested in. The calling code goes through the data as a memory
//  buffer and then frees it.
//
tCIDLib::TCard1*
TCIDSony777DVDID::pc1ReadDVDFile(const  TString&            strFilePath
                                ,       tCIDLib::TCard4&    c4Size) const
{
    // Open the file, allocate a buffer, read it in, and close it
    TBinaryFile flCur;
    flCur.strName(strFilePath);
    flCur.Open
    (
        tCIDLib::EAccessModes::Read
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );
    c4Size = tCIDLib::TCard4(flCur.c8CurSize());
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4Size];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);
    tCIDLib::TCard4 c4Retry = 0;
    while (kCIDLib::True)
    {
        try
        {
            flCur.c4ReadBuffer(pc1Buf, c4Size);
            break;
        }

        catch(TError& errToCatch)
        {
            TThread::Sleep(250);
            c4Retry++;
            if (c4Retry >= 12)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }
        }
    }
    flCur.Close();
    return janBuf.paOrphan();
}


