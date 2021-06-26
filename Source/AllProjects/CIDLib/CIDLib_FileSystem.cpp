//
// FILE NAME: CIDLib_FileSystem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/13/1993
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
//  This file implements the TFileSys class. This class, along with the
//  ancillary classes TFindBuf and TDirIter provide the file system services
//  for the CIDLib system.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TVolFailureInfo, TObject)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDLib_FileSys
    {
        // -----------------------------------------------------------------------
        //  Our persistent format versions
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard2   c2VolFailFmtVer = 2;
    }
}




// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method is called by the public version who has set the c4Matches to
//  whatever desired value already, we do not init it to zero, because this
//  method is recursive.
//
static tCIDLib::TVoid
FindFiles(  const   TString&                    strWildCard
            ,       TFindBuf&                   fndbTarget
            ,       tCIDLib::TCard4&            c4Matches
            , const tCIDLib::EDirSearchFlags    eFlags)
{
    // Flush the child collection to make sure we start empty
    fndbTarget.RemoveAllChildren();

    //
    //  Create a search spec that includes directories. It might have
    //  already, but we need to insure it does. This avoids the need
    //  to do two steps, one for directories and one for files.
    //
    tCIDLib::EDirSearchFlags eSearchFlags = tCIDLib::eOREnumBits
    (
        eFlags, tCIDLib::EDirSearchFlags::NormalDirs
    );

    //
    //  Find all of the matches at this level. Then we will go back and
    //  recursively search each of them. If we don't find any matches
    //  here, then we are done.
    //
    tCIDLib::TCard4 c4Count = TFileSys::c4SearchDir
    (
        fndbTarget.pathFileName(), strWildCard, fndbTarget.colChildren(), eSearchFlags
    );

    if (!c4Count)
        return;

    // Add the matches we got
    c4Matches += c4Count;

    //
    //  We found some, so iterate the bag of child directories and
    //  recurse for each one.
    //
    for (auto cursChildren = fndbTarget.cursChildrenNC(); cursChildren; ++cursChildren)
    {
        //  If this one is a directory, then search it.
        TFindBuf& fndbCur = *cursChildren;
        if (fndbCur.bIsDirectory())
            FindFiles(strWildCard, fndbCur, c4Matches, eFlags);
    }
}


//
//  This method is called by public methods that needs to find all of the
//  directory structure under a starting point.
//
static tCIDLib::TVoid FindDirs( const   TString&            strWildCard
                                ,       TFindBuf&           fndbTarget
                                ,       tCIDLib::TCard4&    c4Matches)
{
    // Flush the child collection to make sure we start empty
    fndbTarget.colChildren().RemoveAll();

    //
    //  Find all of the matches at this level. Then we will go back and
    //  recursively search each of them. If we don't find any matches
    //  here, then we are done.
    //
    tCIDLib::TCard4 c4Count = TFileSys::c4SearchDir
    (
        fndbTarget.pathFileName()
        , strWildCard
        , fndbTarget.colChildren()
        , tCIDLib::EDirSearchFlags::NormalDirs
    );

    if (!c4Count)
        return;

    // Add the matches we got
    c4Matches += c4Count;

    //
    //  We found some, so iterate the bag of child directories and
    //  recurse for each one.
    //
    for (auto cursChildren = fndbTarget.cursChildrenNC(); cursChildren; ++cursChildren)
        FindDirs(strWildCard, *cursChildren, c4Matches);
}


//  A recursive helper for the RemovePath() method.
static tCIDLib::TVoid FSTreeDelete(const TString& strStartDir, const tCIDLib::ETreeDelModes eMode)
{
    TDirIter diterCur;
    TFindBuf fndbCur;

    //
    //  Do sub-dirs first if not in shallow mode. In shallow mode we are just removing
    //  the immediate contents of the start dir.
    //
    TPathStr pathCur;
    if (eMode != tCIDLib::ETreeDelModes::Shallow)
    {
        pathCur = strStartDir;
        pathCur.AddLevel(kCIDLib::pszAllDirsSpec);
        if (diterCur.bFindFirst(pathCur, fndbCur, tCIDLib::EDirSearchFlags::AllDirs))
        {
            do
            {
                //
                //  Recurse on this guy first. We don't get the special current/parent
                //  directories, so we don't have to worry about that.
                //
                FSTreeDelete(fndbCur.pathFileName(), eMode);

                // If not in clean mode, we want to remove this directory now
                if (eMode != tCIDLib::ETreeDelModes::Clean)
                    TFileSys::RemoveDirectory(fndbCur.pathFileName());

            }   while (diterCur.bFindNext(fndbCur));
        }
    }

    // And now do files
    pathCur = strStartDir;
    pathCur.AddLevel(kCIDLib::pszAllFilesSpec);
    if (diterCur.bFindFirst(pathCur, fndbCur, tCIDLib::EDirSearchFlags::AllFiles))
    {
        do
        {
            TFileSys::DeleteFile(fndbCur.pathFileName());
        }   while (diterCur.bFindNext(fndbCur));
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TVolFailureInfo
//  PREFIX: volfi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TVolFailureInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TVolFailureInfo::strVolKey(const TVolFailureInfo& volfiSrc)
{
    return volfiSrc.m_strVolume;
}


// ---------------------------------------------------------------------------
//  TVolFailureInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TVolFailureInfo::TVolFailureInfo() :

    m_eHWType(tCIDLib::EVolHWTypes::Unknown)
{
}

TVolFailureInfo::TVolFailureInfo(const TKrnlVolFailureInfo& kvolfiSrc) :

    m_eHWType(kvolfiSrc.eHWType())
    , m_kerrReason(kvolfiSrc.kerrReason())
    , m_strVolume(kvolfiSrc.pszVolume())
{
}

TVolFailureInfo::TVolFailureInfo(const TVolFailureInfo& volfiToCopy) :

    m_eHWType(volfiToCopy.m_eHWType)
    , m_kerrReason(volfiToCopy.m_kerrReason)
    , m_strVolume(volfiToCopy.m_strVolume)
{
}

TVolFailureInfo::~TVolFailureInfo()
{
}


// ---------------------------------------------------------------------------
//  TVolFailureInfo: Public operators
// ---------------------------------------------------------------------------
TVolFailureInfo&
TVolFailureInfo::operator=(const TVolFailureInfo& volfiToAssign)
{
    if (this == &volfiToAssign)
        return *this;

    m_eHWType    = volfiToAssign.m_eHWType;
    m_kerrReason = volfiToAssign.m_kerrReason;
    m_strVolume  = volfiToAssign.m_strVolume;

    return *this;
}


// ---------------------------------------------------------------------------
//  TVolFailureInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EVolHWTypes TVolFailureInfo::eHWType() const
{
    return m_eHWType;
}

tCIDLib::TErrCode TVolFailureInfo::errcReason() const
{
    return m_kerrReason.errcId();
}

const TString& TVolFailureInfo::strVolume() const
{
    return m_strVolume;
}


// ---------------------------------------------------------------------------
//  TVolFailureInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TVolFailureInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Look for the start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDLib_FileSys::c2VolFailFmtVer))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , TString(L"TVolFailureInfo")
        );
    }

    strmToReadFrom  >> m_eHWType
                    >> m_kerrReason
                    >> m_strVolume;

    // And we should end with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TVolFailureInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream out the non-optional members
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_FileSys::c2VolFailFmtVer
                    << m_eHWType
                    << m_kerrReason
                    << m_strVolume
                    << tCIDLib::EStreamMarkers::EndObject;
}






// ---------------------------------------------------------------------------
//   CLASS: TCurDirJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCurDirJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TCurDirJanitor::TCurDirJanitor(const TString& strNewDir)
{
    // Remember the old directory
    TFileSys::QueryCurrentDir(m_strOldDir);

    // And try to set the new one
    TFileSys::SetCurrentDir(strNewDir);
}

TCurDirJanitor::~TCurDirJanitor()
{
    // Try to put back the old one
    try
    {
        TFileSys::SetCurrentDir(m_strOldDir);
    }

    // Not much we can do here
    catch(...)
    {
    }
}



// ---------------------------------------------------------------------------
//  NAMESPACE: TFileSys
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TFileSys::bExists(  const   TString&                    strSpec
                    , const tCIDLib::EDirSearchFlags    eFlags)
{
    //
    //  If the path ends with a separator, we have to remove it, because
    //  this will cause it to fail on some platforms.
    //
    TString strActual(strSpec);
    if (strActual.chLast() == L'\\')
        strActual.DeleteLast();

    tCIDLib::TBoolean bFound;
    if (!TKrnlFileSys::bExists(strActual.pszBuffer(), bFound, eFlags))
        return kCIDLib::False;

    return bFound;
}

tCIDLib::TBoolean
TFileSys::bExists(  const   TString&                    strDirectory
                    , const TString&                    strName
                    , const tCIDLib::EDirSearchFlags    eFlags)
{
    // Just build up the path in a temp and call the other version
    TPathStr pathTmp(strDirectory);
    pathTmp.AddLevel(strName);
    return bExists(pathTmp, eFlags);
}

tCIDLib::TBoolean
TFileSys::bExists(  const   TString&                    strSpec
                    ,       TFindBuf&                   fndbToFill
                    , const tCIDLib::EDirSearchFlags    eFlags)
{
    //
    //  If the path ends with a separator, we have to remove it, because
    //  this will cause it to fail on some platforms.
    //
    TString strActual(strSpec);
    if (strActual.chLast() == L'\\')
        strActual.DeleteLast();

    try
    {
        TDirIter diterTmp;

        if (!diterTmp.bFindFirst(strActual, fndbToFill, eFlags))
            return kCIDLib::False;
    }

    catch (...)
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TFileSys::bExists(  const   TString&                    strDirectory
                    , const TString&                    strName
                    ,       TFindBuf&                   fndbToFill
                    , const tCIDLib::EDirSearchFlags    eFlags)
{
    // Just build the path in a temp and call the other version
    TPathStr pathTmp(strDirectory);
    pathTmp.AddLevel(strName);
    return bExists(pathTmp, fndbToFill, eFlags);
}


tCIDLib::TBoolean
TFileSys::bExistsInPath(const   TString&                    strSpec
                        , const TString&                    strSearchPath
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    TFindBuf fndTmp;
    return bExistsInPath(strSearchPath, strSpec, fndTmp, eFlags);
}

tCIDLib::TBoolean
TFileSys::bExistsInPath(const   TString&                    strSpec
                        , const TString&                    strSearchPath
                        ,       TFindBuf&                   fndTarget
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen+1];

    if (!TKrnlFileSys::bFindInPath( strSearchPath.pszBuffer()
                                    , strSpec.pszBuffer()
                                    , szTmp
                                    , kCIDLib::c4MaxPathLen))
    {
        return kCIDLib::False;
    }
    return bExists(szTmp, fndTarget, eFlags);
}


tCIDLib::TBoolean
TFileSys::bExistsInSysPath( const   TString&                 strSpec
                            ,       TFindBuf&                fndTarget
                            , const tCIDLib::EDirSearchFlags eFlags)
{
    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen+1];

    // See if its in the system path and, if so, get its full path
    if (!TKrnlFileSys::bFindInSysPath(  strSpec.pszBuffer()
                                        , szTmp
                                        , kCIDLib::c4MaxPathLen))
    {
        return kCIDLib::False;
    }

    // And now see if it exists and get its find info
    return bExists(szTmp, fndTarget, eFlags);
}


tCIDLib::TBoolean TFileSys::bHasWildCards(const TString& strToSearch)
{
    return TKrnlFileSys::bHasWildCards(strToSearch.pszBuffer());
}


tCIDLib::TBoolean TFileSys::bIsDirectory(const TString& strPath)
{
    // This one returns no errors, just a result
    return TKrnlFileSys::bIsDirectory(strPath.pszBuffer());
}


tCIDLib::TBoolean TFileSys::bIsFQPath(const TString& strPath)
{
    return TKrnlPathStr::bIsFullyQualified(strPath.pszBuffer());
}


//
//  We don't return the . and .. type directories in searches (and there may be
//  others on some platforms), but we provide this in case they get directory
//  info from elsewhere.
//
tCIDLib::TBoolean TFileSys::bIsNormalDir(const TString& strPath)
{
    return TKrnlFileSys::bIsNormalDir(strPath.pszBuffer());
}


tCIDLib::TBoolean TFileSys::bIsRedirected(const tCIDLib::EStdFiles eStdFile)
{
    return TKrnlFileSys::bIsRedirected(eStdFile);
}


tCIDLib::TBoolean TFileSys::bRemoveTrailingSeparator(TString& strSrc)
{
    //
    //  If the last char is a path separator, and it's not the one for the
    //  volume itself, then remove it. If it's somethink like X:/ we don't
    //  remove it, because that will create an invalid path.
    //
    if (strSrc.chLast() == L'\\')
    {
        const tCIDLib::TCard4 c4Len = strSrc.c4Length();
        if ((c4Len == 3) && (strSrc.chAt(1) == kCIDLib::chColon))
            return kCIDLib::False;
        strSrc.DeleteLast();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TCard4
TFileSys::c4BuildDirTree(const TString& strWildCard, TFindBuf& fndbMatches)
{
    //  Make sure the find buffer is valid. It has to be a directory
    if (!fndbMatches.bIsDirectory())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_BadFindBuf
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    //
    //  Looks ok, so kick off the recursive operation that builds the
    //  directory structure.
    //
    tCIDLib::TCard4  c4Matches = 0;
    try
    {
        FindDirs(strWildCard, fndbMatches, c4Matches);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    return c4Matches;
}


tCIDLib::TCard4
TFileSys::c4BuildFileTree(  const   TString&                    strWildCard
                            ,       TFindBuf&                   fndbMatches
                            , const tCIDLib::EDirSearchFlags    eFlags)
{
    // The passed match must be a directory
    if (!fndbMatches.bIsDirectory())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_BadFindBuf
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    //
    //  Looks ok, so kick off the recursive operation that finds all of
    //  the files under the start directory.
    //
    tCIDLib::TCard4  c4Matches = 0;
    try
    {
        FindFiles(strWildCard, fndbMatches, c4Matches, eFlags);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    return c4Matches;
}


tCIDLib::TCard4 TFileSys
::c4QueryAvailableVolumes(          TCollection<TVolumeInfo>&       colVolsToFill
                            ,       TCollection<TVolFailureInfo>&   colErrsToFill
                            , const tCIDLib::TBoolean               bIncludeShares
                            , const tCIDLib::TBoolean               bForceUpdate)
{
    // Flush the collections first
    colVolsToFill.RemoveAll();
    colErrsToFill.RemoveAll();

    //
    //  Ask the kernel to give us the list of available volumes. It will
    //  allocate the array of raw volume info structures for us.
    //
    TKrnlLList<TKrnlVolFailureInfo> kllistBad;
    TKrnlLList<TKrnlVolumeInfo>     kllistGood;
    tCIDLib::TCard4 c4ErrCount;
    tCIDLib::TCard4 c4VolCount;
    const tCIDLib::TBoolean bRes = TKrnlFileSys::bQueryVolumeList
    (
        kllistGood
        , c4VolCount
        , kllistBad
        , c4ErrCount
        , bIncludeShares
        , bForceUpdate
    );

    if (!bRes)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_VolumeListQuery
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Copy over the volumes we found
    if (kllistGood.bResetCursor())
    {
        TKrnlVolumeInfo* pkvoliCur = nullptr;
        while (kllistGood.bNext(pkvoliCur))
            colVolsToFill.objAdd(TVolumeInfo(*pkvoliCur));
    }

    // Copy over the failres we got
    if (kllistBad.bResetCursor())
    {
        TKrnlVolFailureInfo* pkvolfiList = nullptr;
        while (kllistBad.bNext(pkvolfiList))
            colErrsToFill.objAdd(TVolFailureInfo(*pkvolfiList));
    }

    // Return the count of volumes we got
    return c4VolCount;
}


//
//  Search the indicate path (or path plus separate wildcard), and return
//  a list of matches.
//
tCIDLib::TCard4
TFileSys::c4SearchDir(  const   TString&                    strPath
                        , const TString&                    strWildCard
                        ,       TCollection<TFindBuf>&      colTarget
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    colTarget.RemoveAll();

    TPathStr    pathToSearch(strPath);
    pathToSearch.AddLevel(strWildCard);
    return c4SearchDir(pathToSearch, colTarget, eFlags);
}

tCIDLib::TCard4
TFileSys::c4SearchDir(  const   TString&                    strPath
                        ,       TCollection<TFindBuf>&      colTarget
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    colTarget.RemoveAll();

    tCIDLib::TCard4 c4Matches = 0;
    TFindBuf        fndBuf;
    TDirIter        iterSearch;
    if (iterSearch.bFindFirst(strPath, fndBuf, eFlags))
    {
        do
        {
            colTarget.objAdd(fndBuf);
            c4Matches++;
        }   while (iterSearch.bFindNext(fndBuf));
    }
    return c4Matches;
}


//
//  Cleans out a whole branch of the directory hierarchy, recursively. See the
//  ETreeDelModes comments for what the modes mean.
//
tCIDLib::TVoid
TFileSys::CleanPath(const   TString&                strStartDir
                    , const tCIDLib::ETreeDelModes  eMode)
{
    // The last parameter means don't remove the start dir
    RemovePath(strStartDir, eMode, kCIDLib::False);
}


//
//  Copies the file indicated by the source path, over to the target path.
//  They can indicate whether we should throw if the target exists, or just
//  try to overwrite it.
//
tCIDLib::TVoid
TFileSys::CopyFile( const   TString&            strSourcePath
                    , const TString&            strTargetPath
                    , const tCIDLib::TBoolean   bFailIfExists)
{
    if (!TKrnlFileSys::bCopy(strSourcePath.pszBuffer()
                            , strTargetPath.pszBuffer()
                            , bFailIfExists))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Copy
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strSourcePath
            , strTargetPath
        );
    }
}


//
//  Returns a temporary file name to use. One does it in the system temp dir and the other
//  in a given directory. If the unique number is zero, then a known unique name is created
//  and the file is created and then closed, leaving it reserved for the caller to use.
//
//  If the number is non-zero, then just the path name is created but the caller has to deal
//  with creating the file and being sure it's unique.
//
tCIDLib::TVoid
TFileSys::CreateTmpFileName(TString& strToFill, const tCIDLib::TCard4 c4UniqueId)
{
    // Create a buffer big enough to hold a legal path on this current platform
    tCIDLib::TCh szBuf[kCIDLib::c4MaxPathLen + 1];

    if (!TKrnlFileSys::bCreateTmpFileName(szBuf, kCIDLib::c4MaxPathLen, c4UniqueId))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_CreateTmpFlName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Copy it to the caller's buffer
    strToFill = szBuf;
}


tCIDLib::TVoid
TFileSys::CreateTmpFileNameIn(  const   TString&        strTarDir
                                ,       TString&        strToFill
                                , const tCIDLib::TCard4 c4UniqueId)
{
    // Create a buffer big enough to hold a legal path on this current platform
    tCIDLib::TCh szBuf[kCIDLib::c4MaxPathLen + 1];

    if (!TKrnlFileSys::bCreateTmpFileNameIn(strTarDir.pszBuffer()
                                            , szBuf
                                            , kCIDLib::c4MaxPathLen
                                            , c4UniqueId))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_CreateTmpFlName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Copy it to the caller's buffer
    strToFill = szBuf;
}


//
//  Build up the path to a subdirectory with the given name, under the system
//  temp directory. If it doesn't exist, create it. Optionally clean it out
//  if it exists.
//
tCIDLib::TVoid
TFileSys::CreateTmpSubDir(  const   TString&            strName
                            ,       TString&            strToFill
                            , const tCIDLib::TBoolean   bCleanContents)
{
    // Query the system temp path
    tCIDLib::TCh szPath[kCIDLib::c4MaxPathLen + 1];
    if (!TKrnlFileSys::bQueryTmpPath(szPath, kCIDLib::c4MaxPathLen))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_GetTmpPath
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Create the full path now
    TPathStr pathNew(kCIDLib::c4MaxPathLen);
    pathNew = szPath;
    pathNew.AddLevel(strName);

    // See if it exists. If so, clean it out if asked. Else create it if possible
    if (TFileSys::bExists(pathNew))
    {
        // Make sure it's a directory
        if (!TFileSys::bIsDirectory(pathNew))
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_NotADir
                , pathNew
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // If asked to clean, do it
        if (bCleanContents)
            TFileSys::CleanPath(pathNew, tCIDLib::ETreeDelModes::Remove);
    }
     else
    {
        // Doesn't exist, so try to create it
        TFileSys::MakeDirectory(pathNew);
    }

    // Looks OK, so return it
    strToFill = pathNew;
}



//
//  Delete the indicate file if possible. One version take a full path and the
//  other a path and a file within that path.
//
tCIDLib::TVoid TFileSys::DeleteFile(const TString& strSpec)
{
    if (!TKrnlFileSys::bRemoveFile(strSpec.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Delete
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strSpec
        );
    }
}

tCIDLib::TVoid
TFileSys::DeleteFile(const TString& strDirectory, const TString& strName)
{
    try
    {
        TPathStr pathTmp(strDirectory);
        pathTmp.AddLevel(strName);
        DeleteFile(pathTmp);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::EConnRes
TFileSys::eAccessRemoteShare(const  TString&            strRemPath
                            , const TString&            strUser
                            , const TString&            strPass
                            , const tCIDLib::TBoolean   bTemporary
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    const tCIDLib::EConnRes eRes = TKrnlFileSys::eAccessRemoteShare
    (
        strRemPath.pszBuffer(), strUser.pszBuffer(), strPass.pszBuffer(), bTemporary
    );

    if (eRes == tCIDLib::EConnRes::Failed)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_RemPathAcc
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return eRes;
}


//
//  Make the indicated directory, optionally throwing if it already exists.
//
tCIDLib::TVoid
TFileSys::MakeDirectory(const   TString&            strToCreate
                        , const tCIDLib::TBoolean   bErrIfExists
                        , const tCIDLib::TBoolean   bAllAccess)
{
    if (!TKrnlFileSys::bMakeDirectory(  strToCreate.pszBuffer()
                                        , bErrIfExists
                                        , bAllAccess))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_DirCreation
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strToCreate
        );
    }
}


//
//  Create the indicated path, if possible, creating sub-dirs as needed to
//  create the entire path.
//
tCIDLib::TVoid TFileSys::MakePath(const TString& strToCreate)
{
    TString strFull;

    // Make sure its fully qualified
    QueryFullPath(strToCreate, strFull);

    //
    //  If the path ends with a separator, we have to remove it, because
    //  this will cause it to fail on some platforms.
    //
    if (strFull.chLast() == L'\\')
        strFull.DeleteLast();

    // And pass in the fully qualified version
    if (!TKrnlFileSys::bMakePath(strFull.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_PathCreation
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strToCreate
        );
    }
}


//
//  Create a subdirectory within the passed parent directory
//
tCIDLib::TVoid
TFileSys::MakeSubDirectory( const   TString& strToCreate
                            , const TString& strParent)
{
    if (!bIsDirectory(strParent))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_MustBeADir
            , strParent
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    try
    {
        TPathStr pathTmp(strParent);
        pathTmp.AddLevel(strToCreate);
        MakeDirectory(pathTmp);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::TVoid
TFileSys::NormalizePath(const   TString&    strToNormalize
                        ,       TString&    strNormalized)
{
    // If the incoming is empty, the outgoign should be empty
    if (strToNormalize.bIsEmpty())
    {
        strNormalized.Clear();
        return;
    }

    // Create a buffer big enough to hold a legal path on this current platform
    tCIDLib::TCh szBuf[kCIDLib::c4MaxPathLen + 1];

    if (!TKrnlFileSys::bNormalizePath(  strToNormalize.pszBuffer()
                                        , szBuf
                                        , kCIDLib::c4MaxPathLen))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Normalize
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToNormalize
        );
    }
    strNormalized = szBuf;
}


tCIDLib::TVoid TFileSys::QueryCurrentDir(TString& strToFill)
{
    tCIDLib::TCh szPath[kCIDLib::c4MaxPathLen + 1];

    if (!TKrnlFileSys::bQueryCurrentDir(szPath, tCIDLib::c4MaxBufChars(szPath)))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_QueryCurDir
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strToFill = szPath;
}


tCIDLib::TVoid
TFileSys::QueryFullPath(const   TString& strPartialPath
                        ,       TString& strFullReturnedPath)
{
    tCIDLib::TCh szBuffer[kCIDLib::c4MaxPathLen + 1];

    if (!TKrnlFileSys::bQueryFullPath(  strPartialPath.pszBuffer()
                                        , szBuffer
                                        , kCIDLib::c4MaxPathLen))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_PathCompletion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPartialPath
        );
    }
    strFullReturnedPath = szBuffer;
}


tCIDLib::TVoid
TFileSys::QueryMountInfo(   const   TString&    strVolume
                            ,       TString&    strHost
                            ,       TString&    strMountInfo)
{
    tCIDLib::TCh szHost[kCIDLib::c4MaxPathLen + 1];
    tCIDLib::TCh szMountInfo[kCIDLib::c4MaxPathLen + 1];

    if (!TKrnlFileSys::bQueryMountInfo( strVolume.pszBuffer()
                                        , szHost
                                        , kCIDLib::c4MaxPathLen
                                        , szMountInfo
                                        , kCIDLib::c4MaxPathLen))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_MountQuery
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strVolume
        );
    }

    strHost = szHost;
    strMountInfo = szMountInfo;
}


tCIDLib::TVoid
TFileSys::QueryVolumeInfo(  const   TString&            strVolume
                            ,       tCIDLib::TCard8&    c8TotalBytes
                            ,       tCIDLib::TCard8&    c8VolFreeBytes
                            ,       tCIDLib::TCard8&    c8UserAvailBytes)
{
    if (!TKrnlFileSys::bQueryVolumeInfo(strVolume.pszBuffer()
                                        , c8TotalBytes
                                        , c8VolFreeBytes
                                        , c8UserAvailBytes))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_VolumeQuery
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strVolume
        );
    }
}


tCIDLib::TVoid TFileSys::RemoveDirectory(const TString& strSpec)
{
    //
    //  If the path ends with a separator, we have to remove it, because
    //  this will cause it to fail on some platforms.
    //
    TString strActual(strSpec);
    if (strActual.chLast() == L'\\')
        strActual.DeleteLast();

    if (!TKrnlFileSys::bRemoveDir(strActual.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_RemoveDir
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strSpec
        );
    }
}


//
//  Recursively removes the contents under the start directory. Depending on the mode
//  we remove more or less stuff.
//
tCIDLib::TVoid
TFileSys::RemovePath(const  TString&                strStartDir
                    , const tCIDLib::ETreeDelModes  eMode
                    , const tCIDLib::TBoolean       bRemoveStart)
{
    //
    //  If the starting point doesn't exist, or is not a directory, then something is
    //  wrong.
    //
    if (!TFileSys::bIsDirectory(strStartDir))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_MustBeADir
            , strStartDir
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // Call a local recursive helper to do the work
    FSTreeDelete(strStartDir, eMode);

    // And now, if asked, remove the start dir
    if (bRemoveStart)
        TFileSys::RemoveDirectory(strStartDir);
}



tCIDLib::TVoid
TFileSys::Rename(const TString& strSource, const TString& strDest)
{
    if (!TKrnlFileSys::bRename(strSource.pszBuffer(), strDest.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Rename
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strSource
            , strDest
        );
    }
}


tCIDLib::TVoid
TFileSys::ReplaceFile(  const   TString&    strToReplace
                        , const TString&    strNewFile
                        , const TString&    strBackupName)
{
    if (!TKrnlFileSys::bReplaceFile(strToReplace.pszBuffer()
                                    , strNewFile.pszBuffer()
                                    , strBackupName.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_ReplaceFile
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strToReplace
            , strNewFile
        );
    }
}

tCIDLib::TVoid
TFileSys::ReplaceFile(  const   TString&    strToReplace
                        , const TString&    strNewFile)
{
    if (!TKrnlFileSys::bReplaceFile(strToReplace.pszBuffer()
                                    , strNewFile.pszBuffer()
                                    , nullptr))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_ReplaceFile
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strToReplace
            , strNewFile
        );
    }
}


tCIDLib::TVoid TFileSys::SetCurrentDir(const TString& strToSet)
{
    //
    //  If the path ends with a separator, we have to remove it, because
    //  this will cause it to fail on some platforms.
    //
    TString strActual(strToSet);
    if (strActual.chLast() == L'\\')
        strActual.DeleteLast();

    if (!TKrnlFileSys::bSetCurrentDir(strActual.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_SetCurrentDir
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strToSet
        );
    }
}


tCIDLib::TVoid TFileSys::SetPerms(  const   TString&            strTarget
                                    , const tCIDLib::EFilePerms eToSet)
{
    //
    //  If the path ends with a separator, we have to remove it, because
    //  this will cause it to fail on some platforms.
    //
    TString strActual(strTarget);
    if (strActual.chLast() == L'\\')
        strActual.DeleteLast();

    if (!TKrnlFileSys::bSetPerms(strActual.pszBuffer(), eToSet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_SetPerms
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strTarget
        );
    }
}

