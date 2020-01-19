//
// FILE NAME: CIDPack_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2015
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
//  This file implements the facility class for this facility.
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
#include    "CIDPack_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macro
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDPack,TFacility)




// ---------------------------------------------------------------------------
//   CLASS: TFacCIDPack
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDPack: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDPack::TFacCIDPack() :

    TFacility
    (
        L"CIDPack"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDPack::~TFacCIDPack()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDPack: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method will create a package. Given a tree of files that have been
//  looked up, we go through the tree, depth first, process all of the files
//  and write out the package. We take an overwrite flag to indicate if it's
//  safe to overwrite an existing one.
//
tCIDLib::TVoid
TFacCIDPack::CreatePackage( const   TString&                strTarFile
                            , const TString&                strSrcPath
                            , const tCIDLib::TBoolean       bOverwrite
                            , const TFindBuf&               fndbTreeTop
                            , const tCIDLib::TCard8         c8Version
                            , const tCIDLib::TEncodedTime   enctStamp
                            , const TString&                strNotes
                            ,       TTextOutStream* const   pstrmStatus
                            ,       tCIDLib::TCard4&        c4SoFar
                            , const tCIDLib::TBoolean       bVerbose)
{
    // The target file cannot be within the source tree
    if (strTarFile.bStartsWithI(strSrcPath))
    {
        if (pstrmStatus)
        {
            *pstrmStatus    << facCIDPack().strMsg(kPackErrs::errcDbg_OutWithinSrc)
                            << L"\n    " << strTarFile
                            << kCIDLib::EndLn;
        }

        facCIDPack().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPackErrs::errcDbg_OutWithinSrc
            , strTarFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Create the full target path by adding the standard extension if it does
    //  not already have it.
    //
    TPathStr pathTar(strTarFile);
    TString  strExt;
    if (!pathTar.bQueryExt(strExt) || !strExt.bCompareI(L"CIDPACK"))
        pathTar.AppendExt(L".CIDPack");

    //
    //  Try to open the output path. The one we adjusted, not the original
    //  incoming one!.
    //
    TBinFileOutStream strmTar
    (
        pathTar
        , bOverwrite ? tCIDLib::ECreateActs::CreateAlways : tCIDLib::ECreateActs::CreateIfNew
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );

    //
    //  We have to make one precursor iteration through the tree of files, to
    //  get the full file count, which has to go into the main file header.
    //
    tCIDLib::TCard4 c4FileCount = 0;
    CountFiles(fndbTreeTop, c4FileCount);

    // Let's set up the overall file header now and write it out
    TCIDPackHdr pkhdrNew(c4FileCount, c8Version, enctStamp);
    pkhdrNew.strUser(strNotes);
    strmTar << pkhdrNew;

    //
    //  Now let's iterate the files and process them. The order doesn't really
    //  matter, but it'll do a simple in order iteration.
    //
    TMessageDigest5         mdigHasher;
    TZLibCompressor         zlibComp;
    TChunkedBinOutStream    strmComp((1024 * 1024) * 200);

    PackFiles
    (
        strmTar
        , fndbTreeTop
        , strSrcPath
        , mdigHasher
        , zlibComp
        , strmComp
        , pstrmStatus
        , c4SoFar
        , bVerbose
    );

    // Flush the output stream and we are done
    strmTar.Flush();

    if (pstrmStatus)
        *pstrmStatus << L"\n<Package complete>" << kCIDLib::NewEndLn;
}


//
//  Extract just the main file header info not the files.
//
tCIDLib::TVoid
TFacCIDPack::ExtractDetails(const   TString&                strSrcFile
                            ,       tCIDLib::TCard8&        c8Version
                            ,       tCIDLib::TEncodedTime&  enctStamp
                            ,       TString&                strNotes
                            ,       tCIDLib::TCard4&        c4TotalFiles)
{
    // The source package file has to exist
    if (!TFileSys::bExists(strSrcFile, tCIDLib::EDirSearchFlags::NormalFiles))
    {
        facCIDPack().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPackErrs::errcDbg_SrcPackNotFound
            , strSrcFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // Put a binary file input stream over it
    TBinFileInStream strmSrc
    (
        strSrcFile
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );

    // Read in the overall header
    TCIDPackHdr pkhdrLoad;
    strmSrc >> pkhdrLoad;

    // And pull out the info
    c8Version = pkhdrLoad.c8Version();
    enctStamp = pkhdrLoad.enctStamp();
    c4TotalFiles = pkhdrLoad.c4FileCount();
    strNotes = pkhdrLoad.strUser();
}


//
//  Give a source cqc package file, we extract the files and write them out to
//  the indicated target path. We overwrite any files that already exist.
//
tCIDLib::TVoid
TFacCIDPack::ExtractPackage(const   TString&                strSrcFile
                            , const TString&                strTarPath
                            , const tCIDLib::TBoolean       bOverwrite
                            ,       tCIDLib::TCard8&        c8Version
                            ,       tCIDLib::TEncodedTime&  enctStamp
                            ,       TString&                strNotes
                            ,       tCIDLib::TCard4&        c4TotalFiles
                            ,       TTextOutStream* const   pstrmStatus
                            ,       tCIDLib::TCard4&        c4SoFar
                            , const tCIDLib::TBoolean       bVerbose)
{
    c4SoFar = 0;

    // The source package file has to exist
    if (!TFileSys::bExists(strSrcFile, tCIDLib::EDirSearchFlags::NormalFiles))
    {
        facCIDPack().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPackErrs::errcDbg_SrcPackNotFound
            , strSrcFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // Make sure the target directory exists, else create it
    if (!TFileSys::bIsDirectory(strTarPath))
        TFileSys::MakePath(strTarPath);

    // Put a binary file input stream over the package file
    TBinFileInStream strmSrc
    (
        strSrcFile
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );

    // Read in the overall header
    TCIDPackHdr pkhdrLoad;
    strmSrc >> pkhdrLoad;

    // Give back the header info
    c8Version = pkhdrLoad.c8Version();
    enctStamp = pkhdrLoad.enctStamp();
    c4TotalFiles = pkhdrLoad.c4FileCount();
    strNotes = pkhdrLoad.strUser();


    // For all of the reported files, let's process them
    TPathStr            pathDir;
    TPathStr            pathCur;
    TCIDPackFlHdr       pkfhdrCur;
    TMessageDigest5     mdigHasher;
    TMD5Hash            mhashOrg;
    TZLibCompressor     zlibComp;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TotalFiles; c4Index++)
    {
        // Read in the next file header
        strmSrc >> pkfhdrCur;

        if (bVerbose && pstrmStatus)
            *pstrmStatus << L"   " << pkfhdrCur.strRelPath() << kCIDLib::EndLn;

        // Make the real target path
        pathCur = strTarPath;
        pathCur.AddLevel(pkfhdrCur.strRelPath());

        //
        //  If it wasn't compressed, just write out the original data. Else
        //  decompress.
        //
        const tCIDLib::TCard4 c4OrgSz = pkfhdrCur.c4OrgBytes();
        THeapBuf mbufOrg(c4OrgSz, c4OrgSz);

        if (pkfhdrCur.c4CompBytes() == c4OrgSz)
        {
            strmSrc.c4ReadBuffer(mbufOrg, c4OrgSz);
        }
         else
        {
            // Read in from the source the reported compressed size
            THeapBuf mbufComp(pkfhdrCur.c4CompBytes(), pkfhdrCur.c4CompBytes());
            strmSrc.c4ReadBuffer(mbufComp, pkfhdrCur.c4CompBytes());

            // Decompress it to another buffer
            {
                TBinMBufInStream  strmComp(&mbufComp, pkfhdrCur.c4CompBytes());
                TBinMBufOutStream strmOrg(&mbufOrg);
                if (zlibComp.c4Decompress(strmComp, strmOrg) != c4OrgSz)
                {
                    facCIDPack().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kPackErrs::errcDbg_NotOrgSize
                        , strSrcFile
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::NotFound
                    );
                }
            }
        }

        // Generate a hash and make sure it matches the stored one
        {
            mdigHasher.StartNew();
            mdigHasher.DigestBuf(mbufOrg, c4OrgSz);
            mdigHasher.Complete(mhashOrg);

            if (mhashOrg != pkfhdrCur.mhashOrg())
            {
                facCIDPack().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kPackErrs::errcDbg_BadHash
                    , strSrcFile
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
        }

        // Make sure the path exists
        pathDir = pathCur;
        pathDir.bRemoveNameExt();
        if (!TFileSys::bIsDirectory(pathDir))
            TFileSys::MakePath(pathDir);

        // Create the output stream and write the data out
        TBinFileOutStream strmTar
        (
            pathCur
            , bOverwrite ? tCIDLib::ECreateActs::CreateAlways : tCIDLib::ECreateActs::CreateIfNew
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );

        strmTar.c4WriteBuffer(mbufOrg, c4OrgSz);
        strmTar.Flush();

        // Bump the count of files done
        c4SoFar++;
    }
}



// ---------------------------------------------------------------------------
//  TFacCIDPack: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We just recursively trans the tree of files, counting them as we go. We just
//  do a simple in order deal. The find buffer that gets passed to us (or back
//  to ourself) is a directory, so we don't worry about that one.
//
tCIDLib::TVoid
TFacCIDPack::CountFiles(const TFindBuf& fndbCurDir, tCIDLib::TCard4& c4ToSet)
{
    TFindBuf::TCursor cursCur = fndbCurDir.cursChildren();
    for (; cursCur; ++cursCur)
    {
        const TFindBuf& fndbCur = *cursCur;

        // Either recurse if a directory, or bump the count
        if (fndbCur.bIsDirectory())
            CountFiles(fndbCur, c4ToSet);
        else
            c4ToSet++;
    }
}


//
//  Package a single file. We iterate the directory tree and for each file :
//
//  1.  Insure it's under the src path and remove the source path from it
//      so that we store the relative path
//  2.  Read in the file contents, and hash it
//  3.  Compress it to a local buffer
//  4.  Create the file header and write it out
//  5.  Write out the compressed data.
//
tCIDLib::TVoid
TFacCIDPack::PackFile(          TBinOutStream&          strmTar
                        , const TFindBuf&               fndbFile
                        , const TString&                strSrcPath
                        ,       TMessageDigest5&        mdigToUse
                        ,       TZLibCompressor&        zlibToUse
                        ,       TChunkedBinOutStream&   strmComp
                        ,       TTextOutStream* const   pstrmStatus
                        ,       tCIDLib::TCard4&        c4SoFar
                        , const tCIDLib::TBoolean       bVerbose)
{
    // The file path must be under the source path
    if (!fndbFile.pathFileName().bStartsWithI(strSrcPath))
    {
        if (pstrmStatus)
        {
            *pstrmStatus    << facCIDPack().strMsg(kPackErrs::errcDbg_NotInTree)
                            << L"\n    " << fndbFile.pathFileName()
                            << kCIDLib::EndLn;
        }

        facCIDPack().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPackErrs::errcDbg_NotInTree
            , fndbFile.pathFileName()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    TPathStr pathRel(fndbFile.pathFileName());
    pathRel.Cut(0, strSrcPath.c4Length());

    // Make sure it didn't end up empty
    if (pathRel.bIsEmpty())
    {
        if (pstrmStatus)
        {
            *pstrmStatus    << facCIDPack().strMsg(kPackErrs::errcDbg_EmptyRepPath)
                            << L"\n    " << fndbFile.pathFileName()
                            << kCIDLib::EndLn;
        }

        facCIDPack().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPackErrs::errcDbg_EmptyRepPath
            , fndbFile.pathFileName()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // And remove any leading slash
    if (pathRel.chFirst() == kCIDLib::chBackSlash)
        pathRel.Cut(0, 1);

    const tCIDLib::TCard4 c4OrgSz(tCIDLib::TCard4(fndbFile.c8Size()));

    // Read the file contents into a buffer
    THeapBuf mbufSrc(c4OrgSz, c4OrgSz);
    {
        TBinaryFile bflSrc(fndbFile.pathFileName());
        bflSrc.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        bflSrc.c4ReadBuffer(mbufSrc, c4OrgSz, tCIDLib::EAllData::FailIfNotAll);
    }

    // Put a binary input stream over the buffer
    TBinMBufInStream strmSrc(&mbufSrc, c4OrgSz);

    //
    //  Let's hash the file now
    //
    TMD5Hash mhashFile;

    if (bVerbose && pstrmStatus)
    {
        *pstrmStatus    << facCIDPack().strMsg(kPackMsgs::midStatus_FileStart)
                        << pathRel << L" ... " << kCIDLib::FlushIt;
    }

    mdigToUse.StartNew();
    mdigToUse.DigestSrc(strmSrc, c4OrgSz);
    mdigToUse.Complete(mhashFile);

    //
    //  Reset the file stream and now let's compress it, remembering the size.
    //  We have to deal here with the problem of hitting some already ZLib
    //  compressed file, which will cause the ZLib engine to get an errror. We
    //  watch for this error and assume this file is incompressible.
    //
    strmSrc.Reset();
    strmComp.Reset();
    tCIDLib::TCard4 c4CompSz = 0;
    try
    {
        c4CompSz = zlibToUse.c4Compress(strmSrc, strmComp);
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDZLib().strName(), kZLibErrs::errcDbg_NegBlockStart))
        {
            // It's not the special case, so rethrow
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }

        // Set the comp size to the original size, to say no compression
        c4CompSz = c4OrgSz;
    }

    if (c4CompSz >= c4OrgSz)
    {
        if (bVerbose && pstrmStatus)
            *pstrmStatus << L"(no reduction)";

        //
        //  Force the comp size to the same as the original. That will tell
        //  us on expansion that we don't need to do anything, i.e. it's the
        //  original, uncompressed content.
        //
        c4CompSz = c4OrgSz;
    }
     else
    {
        tCIDLib::TCard4 c4Percent
        (
            tCIDLib::TCard4
            (
                (1.0 - (tCIDLib::TFloat4(c4CompSz) / tCIDLib::TFloat4(c4OrgSz))) * 100.0
            )
        );

        // It's never 100%, but rounding may make it that, so adjust
        if (c4Percent >= 100)
            c4Percent = 99;

        if (bVerbose && pstrmStatus)
            *pstrmStatus << L"(" << c4Percent << L"% reduced)";
    }

    if (bVerbose && pstrmStatus)
        *pstrmStatus << kCIDLib::EndLn;

    // Create the file header and write it out
    TCIDPackFlHdr pkfhdrNew(c4CompSz, c4OrgSz, mhashFile, pathRel);
    strmTar << pkfhdrNew;

    //
    //  Write out the compressed data. To avoid having to create huge buffers
    //  potentially, we just do it in chunks.
    //
    //  If we got no reduction, we use the original file content, not the
    //  compressed content, since it could be bigger.
    //
    if (c4CompSz == c4OrgSz)
    {
        strmTar.c4WriteBuffer(mbufSrc, c4OrgSz);
    }
     else
    {
        //
        //  Read the compressed data out of the compressed output stream. We
        //  can't use the stream size, we have to use the returned compressed
        //  data size value from above.
        //
        THeapBuf mbufComp(c4CompSz, c4CompSz);
        TChunkedBinInStream strmCompSrc(strmComp);
        strmCompSrc.c4ReadBuffer(mbufComp, c4CompSz);

        // And write it to the target
        strmTar.c4WriteBuffer(mbufComp, c4CompSz);
    }
}


//
//  Package recursively all files in the list. For each directory we recurse
//  on ourself. For each file, we call PackFile.
//
tCIDLib::TVoid
TFacCIDPack::PackFiles(         TBinOutStream&          strmTar
                        , const TFindBuf&               fndbCurDir
                        , const TString&                strSrcPath
                        ,       TMessageDigest5&        mdigToUse
                        ,       TZLibCompressor&        zlibToUse
                        ,       TChunkedBinOutStream&   strmComp
                        ,       TTextOutStream* const   pstrmStatus
                        ,       tCIDLib::TCard4&        c4SoFar
                        , const tCIDLib::TBoolean       bVerbose)
{
    c4SoFar = 0;

    TFindBuf::TCursor cursCur = fndbCurDir.cursChildren();
    for (; cursCur; ++cursCur)
    {
        const TFindBuf& fndbCur = *cursCur;
        if (fndbCur.bIsDirectory())
        {
            PackFiles
            (
                strmTar
                , fndbCur
                , strSrcPath
                , mdigToUse
                , zlibToUse
                , strmComp
                , pstrmStatus
                , c4SoFar
                , bVerbose
            );
        }
         else
        {
            // Process this one
            PackFile
            (
                strmTar
                , fndbCur
                , strSrcPath
                , mdigToUse
                , zlibToUse
                , strmComp
                , pstrmStatus
                , c4SoFar
                , bVerbose
            );
        }
    }
}


