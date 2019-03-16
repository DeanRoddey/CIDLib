//
// FILE NAME: CIDOrbUC_RemDiskBrowserImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/2004
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
//  This file implements a derivative of the remote disk browser ORB
//  interface that allows open ended browsing.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"


// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TRemDiskBrowserImpl,TRemBrowserServerBase)




// ---------------------------------------------------------------------------
//   CLASS: TRemDiskBrowserImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRemDiskBrowserImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TRemDiskBrowserImpl::TRemDiskBrowserImpl()
{
}

TRemDiskBrowserImpl::~TRemDiskBrowserImpl()
{
}


// ---------------------------------------------------------------------------
//  TRemDiskBrowserImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TRemDiskBrowserImpl::bFileExists(const TString& strToCheck)
{
    // Just delegate to the local file system support
    return TFileSys::bExists(strToCheck);
}


tCIDLib::TBoolean
TRemDiskBrowserImpl::bFind( const   TString&    strPath
                            , const TString&    strToFind
                            ,       TFindBuf&   fndbToFill)
{
    // Just delegate to the local file system support
    return TFileSys::bExists(strPath, strToFind, fndbToFill);
}

tCIDLib::TBoolean
TRemDiskBrowserImpl::bFind2(const   TString&    strPath
                            ,       TFindBuf&   fndbToFill)
{
    // Just delegate to the local file system support
    return TFileSys::bExists(strPath, fndbToFill);
}


tCIDLib::TBoolean
TRemDiskBrowserImpl::bIsDirectory(const TString& strToCheck)
{
    // Just delegate to the local file system support
    return TFileSys::bIsDirectory(strToCheck);
}


tCIDLib::TCard4
TRemDiskBrowserImpl::c4QueryVolumes(TVolList& colVols, TFailList& colFails)
{
    // Just delegate to the local file system support
    return TFileSys::c4QueryAvailableVolumes
    (
        colVols, colFails, kCIDLib::True, kCIDLib::False
    );
}


tCIDLib::TVoid
TRemDiskBrowserImpl::LoadFiles( const   TString&            strPath
                                , const TString&            strWildcard
                                ,       TVector<TFindBuf>&  colSubDirs
                                ,       TVector<TFindBuf>&  colFiles)
{
    //
    //  First, load up all the subdirectories, since we always do those, no
    //  matter what file wildcards we have.
    //
    TPathStr pathTmp(strPath);
    pathTmp.AddLevel(kCIDLib::pszAllFilesSpec);

    TFindBuf    fndbCur;
    TDirIter    diterLoad;
    if (diterLoad.bFindFirst(pathTmp, fndbCur, tCIDLib::EDirSearchFlags::NormalDirs))
    {
        do
        {
            colSubDirs.objAdd(fndbCur);
        }   while (diterLoad.bFindNext(fndbCur));
    }

    //
    //  Get a string tokenizer and use it to pull tokens from the wild card
    //  string. There can be multiple, comma separated wild card specs. For
    //  each one, we do a search in the path.
    //
    TStringTokenizer stokWC(&strWildcard, L",");

    TString strCurWC;
    while (stokWC.bGetNextToken(strCurWC))
    {
        pathTmp = strPath;
        pathTmp.AddLevel(strCurWC);

        // And in here we just look for files, not directories
        if (diterLoad.bFindFirst(pathTmp, fndbCur, tCIDLib::EDirSearchFlags::AllFiles))
        {
            do
            {
                colFiles.objAdd(fndbCur);
            }   while (diterLoad.bFindNext(fndbCur));
        }
    }
}


tCIDLib::TVoid
TRemDiskBrowserImpl::MakeDirectory( const   TString&    strBasePath
                                    , const TString&    strNewDir)
{
    // Just delegate to the local file system support
    TFileSys::MakeSubDirectory(strNewDir, strBasePath);
}


tCIDLib::TVoid TRemDiskBrowserImpl::QueryHostName(TString& strToFill)
{
    strToFill = facCIDSock().strIPHostName();
}


// We just return the literal contents of the file
tCIDLib::TVoid
TRemDiskBrowserImpl::ReadFile(  const   TString&            strToRead
                                ,       tCIDLib::TCard4&    c4BufSz
                                ,       THeapBuf&           mbufToFill)
{
    // Try to open the indicated file as a binary file
    TBinaryFile flRead(strToRead);
    flRead.Open
    (
        tCIDLib::EAccessModes::Excl_Read
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );

    // We just do a standard file content buffer
    c4BufSz = tCIDLib::TCard4(flRead.c8CurSize());
    flRead.c4ReadBuffer(mbufToFill, c4BufSz);
}


tCIDLib::TVoid
TRemDiskBrowserImpl::RemoveSelected(const TString& strToRemove)
{
    // Just delegate to the local file system support
    if (TFileSys::bIsDirectory(strToRemove))
        TFileSys::RemovePath(strToRemove);
    else
        TFileSys::DeleteFile(strToRemove);
}


tCIDLib::TVoid
TRemDiskBrowserImpl::Rename(const   TString&            strOrgName
                            , const TString&            strNewName
                            , const tCIDLib::TBoolean)
{
    // Just delegate to the local file system support
    TFileSys::Rename(strOrgName, strNewName);
}


// ---------------------------------------------------------------------------
//  TRemDiskBrowserImpl: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TRemDiskBrowserImpl::Initialize()
{
    // Nothing to do at this time
}


tCIDLib::TVoid TRemDiskBrowserImpl::Terminate()
{
    // Nothing to do at this time
}


