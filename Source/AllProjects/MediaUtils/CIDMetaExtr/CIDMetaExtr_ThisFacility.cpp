//
// FILE NAME: CIDMetaExtr_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2007
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
//  This file implements TFacCIDMetaExtr class.
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


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDMetaExtr,TFacility)



// ---------------------------------------------------------------------------
//  TFacCIDMetaExtr: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDMetaExtr::TFacCIDMetaExtr() :

    TFacility
    (
        L"CIDMetaExtr"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDMetaExtr::~TFacCIDMetaExtr()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDMetaExtr: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Given the path to the root directory of a blu-ray, we generate a simple
//  unique id by doing an MD5 hash of various bits of info available there.
//
tCIDLib::TBoolean
TFacCIDMetaExtr::bGenUniqueBDId(const TString& strPath, TString& strToFill) const
{
    TFindBuf fndbTmp;
    TPathStr pathTmp;

    // Some info on the index file
    pathTmp = strPath;
    pathTmp.AddLevel(L"BDMV");
    pathTmp.AddLevel(L"index.bdmv");

    tCIDLib::TCard4       c4SizeIndex = 0;
    if (TFileSys::bExists(pathTmp, fndbTmp))
        c4SizeIndex = tCIDLib::TCard4(fndbTmp.c8Size());

    // And the movie object file
    pathTmp = strPath;
    pathTmp.AddLevel(L"BDMV");
    pathTmp.AddLevel(L"MovieObject.bdmv");

    tCIDLib::TCard4       c4SizeMO = 0;
    if (TFileSys::bExists(pathTmp, fndbTmp))
        c4SizeMO = tCIDLib::TCard4(fndbTmp.c8Size());

    //
    //  And let's iterate the files in the STREAM directory. We'll sort it
    //  and read in the first 64K of the lowest sorting file.
    //
    pathTmp = strPath;
    pathTmp.AddLevel(L"BDMV");
    pathTmp.AddLevel(L"STREAM");

    const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::NormalFiles;
    TDirIter diterFiles;
    if (!diterFiles.bFindFirst(pathTmp, kCIDLib::pszAllFilesSpec, fndbTmp, eFlags))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcBDID_NoFiles
            , pathTmp
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // Loop and load them all up
    TVector<TFindBuf> colFiles;
    do
    {
        colFiles.objAdd(fndbTmp);
    }   while(diterFiles.bFindNext(fndbTmp));

    //
    //  Sort them by the path, non-case sensitive, if we found any, and get
    //  the size/time of the first one, clipping the size back to 32K if it
    //  is greater and it would always be.
    //
    tCIDLib::TCard4       c4SizeFirst = 0;
    if (!colFiles.bIsEmpty())
    {
        colFiles.Sort(&TFindBuf::eCompByPathI);
        const TFindBuf& fndbFirst = colFiles[0];
        c4SizeFirst = tCIDLib::TCard4(fndbFirst.c8Size());
        if (c4SizeFirst > 0x7FFF)
            c4SizeFirst = 0x7FFF;
    }

    //
    //  OK, let's calculate the size for the buffer we need to allocate. We
    //  put the data in in this order:
    //
    //  1. Index size (card4)
    //  3. MO size
    //  5. First stream file size
    //  7. The contents of the index file
    //  8. The contents of the MO file
    //  9. Up to the first 32K of the first stream file
    //
    const tCIDLib::TCard4 c4Size
    (
        (sizeof(tCIDLib::TCard4) * 3) + c4SizeIndex + c4SizeMO + c4SizeFirst
    );
    TSysBuf mbufRead(0x7FFF, 0x20000);
    TSysBuf mbufHash(c4Size, c4Size);

    // Put out the sizes and times
    tCIDLib::TCard4 c4Index = 0;
    mbufHash.PutCard4(c4SizeIndex, c4Index);
    c4Index += 4;
    mbufHash.PutCard4(c4SizeMO, c4Index);
    c4Index += 4;
    mbufHash.PutCard4(c4SizeFirst, c4Index);
    c4Index += 4;

    // If we got an index file put it into the buffer
    if (c4SizeIndex)
    {
        pathTmp = strPath;
        pathTmp.AddLevel(L"BDMV");
        pathTmp.AddLevel(L"index.bdmv");
        TBinaryFile bflIndex(pathTmp);
        bflIndex.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        bflIndex.c4ReadBuffer(mbufRead, c4SizeIndex, tCIDLib::EAllData::FailIfNotAll);

        mbufHash.CopyIn(mbufRead, c4SizeIndex, c4Index);
        c4Index += c4SizeIndex;
    }

    // If we got a MO file put it into the buffer
    if (c4SizeMO)
    {
        pathTmp = strPath;
        pathTmp.AddLevel(L"BDMV");
        pathTmp.AddLevel(L"MovieObject.bdmv");

        TBinaryFile bflMO(pathTmp);
        bflMO.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        bflMO.c4ReadBuffer(mbufRead, c4SizeMO, tCIDLib::EAllData::FailIfNotAll);

        mbufHash.CopyIn(mbufRead, c4SizeMO, c4Index);
        c4Index += c4SizeMO;
    }

    //
    //  If we got any stream files, get the lowest sorted one. Read in up to
    //  the first 32K of it.
    //
    if (c4SizeFirst != 0)
    {
        TBinaryFile bflFirst(colFiles[0].pathFileName());
        bflFirst.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );

        bflFirst.c4ReadBuffer(mbufRead, c4SizeFirst, tCIDLib::EAllData::FailIfNotAll);
        mbufHash.CopyIn(mbufRead, c4SizeFirst, c4Index);
        c4Index += c4SizeFirst;
    }

    // Make sure we ended up with the bytes we calculated
    CIDAssert(c4Index == c4Size, L"The accumulated bytes did not add up");

    // And now let's hash this buffer
    TMessageDigest5 mdigId;
    TMD5Hash        mhashId;
    mdigId.StartNew();
    mdigId.DigestBuf(mbufHash, c4Size);
    mdigId.Complete(mhashId);
    mhashId.FormatToStr(strToFill);

    return kCIDLib::False;
}


//
//  Given a CD TOC, we generate a unique id for the CD that is almost
//  guaranteed not to match any other. We'll build up a list of the
//  track times in blocks, followed by the start time and the total
//  title. These are put into an array and that is fed into an MD5 hash.
//
//  We have one that returns the TOC since the caller will often need
//  to look at it also, and one that doesn't.
//
tCIDLib::TBoolean
TFacCIDMetaExtr::bGenUniqueCDId(const   TString&    strPath
                                ,       TString&    strToFill) const
{
    // Just call the other version and eat the TOC info
    TKrnlRemMedia::TCDTOCInfo TOCDummy;
    return bGenUniqueCDId(strPath, TOCDummy, strToFill);
}

tCIDLib::TBoolean
TFacCIDMetaExtr::bGenUniqueCDId(const   TString&                    strPath
                                ,       TKrnlRemMedia::TCDTOCInfo&  TOCData
                                ,       TString&                    strToFill) const
{
    // Get the TOC info
    {
        TKrnlRemMediaDrv rmmdCur(strPath.pszBuffer());
        if (!rmmdCur.bOpen())
            return kCIDLib::False;

        if (!rmmdCur.bQueryCDTOC(TOCData))
            return kCIDLib::False;
    }

    // We have it, so let's process the data
    const tCIDLib::TSInt iNumTracks
    (
        (TOCData.c1LastTrack - TOCData.c1FirstTrack) + 1
    );

    //
    //  We have to fill in an array of track lengths in frames. Note that
    //  the TOC has a dummy entry at the end, to allow for the calculation
    //  of the length of the last actual track.
    //
    tCIDLib::TSInt* piTLens = new tCIDLib::TSInt[iNumTracks + 2];
    TArrayJanitor<tCIDLib::TSInt> janTLens(piTLens);
    for (tCIDLib::TSInt iIndex = 0; iIndex < iNumTracks; iIndex++)
    {
        piTLens[iIndex] =
        (
            TKrnlRemMedia::c4MSFToBlocks(TOCData.aTracks[iIndex + 1].ac1Addr)
            - TKrnlRemMedia::c4MSFToBlocks(TOCData.aTracks[iIndex].ac1Addr)
        );
    }

    // Get the start time, i.e. the block of the first track
    const tCIDLib::TSInt iStartTime
    (
        TKrnlRemMedia::c4MSFToBlocks(TOCData.aTracks[0].ac1Addr)
    );
    piTLens[iNumTracks] = iStartTime;

    // For the total, we do the last minus the first
    piTLens[iNumTracks + 1] =
    (
        TKrnlRemMedia::c4MSFToBlocks(TOCData.aTracks[iNumTracks].ac1Addr)
        - iStartTime
    );

    // And feed our built up array into an MD5 hash
    TMessageDigest5 mdigId;
    TMD5Hash        mhashId;
    mdigId.StartNew();
    mdigId.DigestRaw
    (
        reinterpret_cast<const tCIDLib::TCard1*>(piTLens)
        , (iNumTracks + 2) * sizeof(piTLens[0])
    );
    mdigId.Complete(mhashId);
    mhashId.FormatToStr(strToFill);
    return kCIDLib::True;
}


//
//  Given the path to the root directory of a DVD, we generate a simple
//  unique id by doing an MD5 hash of the VIDEO_TS.IFO file. This is used as
//  a unique id generator by the CQC media repository for DVDs currently.
//
tCIDLib::TBoolean
TFacCIDMetaExtr::bGenUniqueDVDId(const TString& strPath, TString& strToFill) const
{
    TPathStr pathVTS(strPath);
    pathVTS.AddLevel(L"VIDEO_TS");
    pathVTS.AddLevel(L"VIDEO_TS.IFO");

    if (!TFileSys::bExists(pathVTS))
        return kCIDLib::False;

    tCIDLib::TCard4 c4Size;
    THeapBuf mbufData;
    try
    {
        TBinaryFile flCur;
        flCur.strName(pathVTS);
        flCur.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        c4Size = tCIDLib::TCard4(flCur.c8CurSize());
        flCur.c4ReadBuffer(mbufData, c4Size);
        flCur.Close();
    }

    catch(const TError& errToCatch)
    {
        if (bShouldLog(errToCatch))
            LogEventObj(errToCatch);
        return kCIDLib::False;
    }

    catch(...)
    {
        return kCIDLib::False;
    }

    // We read it ok, so hash it
    TMessageDigest5 mdigId;
    TMD5Hash        mhashId;
    mdigId.StartNew();
    mdigId.DigestBuf(mbufData, c4Size);
    mdigId.Complete(mhashId);
    mhashId.FormatToStr(strToFill);
    return kCIDLib::True;
}



