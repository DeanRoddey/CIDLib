//
// FILE NAME: CIDLib_BackFileMgr.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2002
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
//  This file implements the TBackFileMgr helper class.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic Macros
// ---------------------------------------------------------------------------
RTTIDecls(TBackFileMgr,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_BackFileMgr
{
    const   tCIDLib::TCard4     c4MaxBackups = 8;
}



// ---------------------------------------------------------------------------
//   CLASS: TBackFileMgr
//  PREFIX: bfm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBackFileMgr: Constructors and Destructor
// ---------------------------------------------------------------------------
TBackFileMgr::TBackFileMgr() :

    m_c4Backups(0)
{
}

TBackFileMgr::TBackFileMgr( const   TString&        strFileName
                            , const tCIDLib::TCard4 c4Backups
                            , const TString&        strTargetPath) :
    m_c4Backups(c4Backups)
    , m_strFileName(strFileName)
    , m_strTargetPath(strTargetPath)
{
    // Make sure the passed data is legal, and setup some derived stuff
    CheckInitData();
}

TBackFileMgr::~TBackFileMgr()
{
}


// ---------------------------------------------------------------------------
//  TBackFileMgr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TBackFileMgr::bPromote(const TString& strSaveCurAs)
{
    // Make sure they've set us up first
    CheckReady();

    //
    //  Look back from the 1st backup to the last, until we find a backup
    //  file.
    //
    tCIDLib::TCard4 c4Index = 1;
    TPathStr        pathBak(m_strBakName);
    for (; c4Index <= m_c4Backups; c4Index++)
    {
        pathBak.CapAt(m_strBakName.c4Length());
        pathBak.AppendFormatted(c4Index);
        if (TFileSys::bExists(pathBak))
            break;
    }

    // We never found one
    if (c4Index > m_c4Backups)
        return kCIDLib::False;

    // We have one, so lets save the current file first if we have one
    bSaveCur(strSaveCurAs);

    // And lets rename the backup file to the current file
    TPathStr pathCur(m_strTargetPath);
    pathCur.AddLevel(m_strFileName);

    try
    {
        TFileSys::Rename(pathBak, pathCur);
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::FileSystem))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_CantPromote
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pathBak
            , pathCur
        );
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TBackFileMgr::bSaveCur(const TString& strSaveAs)
{
    // Build the full path to the current file
    TPathStr pathSrc(m_strTargetPath);
    pathSrc.AddLevel(m_strFileName);

    // Make sure it actually exists, else nothing to do
    if (!TFileSys::bExists(pathSrc))
        return kCIDLib::False;

    //
    //  If the passed save as has no path component, then set it to our
    //  path. If it has a path but is relative, then add it to our base
    //  path. If its fully qualified, take it as is.
    //
    TPathStr pathTarget(strSaveAs);
    if (!pathTarget.bIsFullyQualified())
        pathTarget.AddToBasePath(m_strTargetPath);

    try
    {
        // If the target file already exists, then delete it
        if (TFileSys::bExists(pathTarget))
            TFileSys::DeleteFile(pathTarget);
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::FileSystem))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_CantSaveCur
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pathSrc
            , pathTarget
        );
    }

    try
    {
        // And try to rename the source to the target file
        TFileSys::Rename(pathSrc, pathTarget);
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::FileSystem))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_CantRenameCur
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pathSrc
            , pathTarget
        );
    }

    return kCIDLib::True;
}


tCIDLib::TCard4
TBackFileMgr::c4QueryNextFile(          TString&        strToFill
                                , const tCIDLib::TCard4 c4StartAt)
{
    // Make sure they've set us up first
    CheckReady();

    //
    //  Validate the start at index. When its the number of backup files
    //  plus one, then we return c4MaxCard, because we have no more and
    //  they've hit the end of the iteration. If its past that, we throw
    //  an error.
    //
    if (c4StartAt > m_c4Backups + 1)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_BadStartInd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4StartAt)
            , TCardinal(m_c4Backups)
        );
    }

    if (c4StartAt == m_c4Backups + 1)
        return kCIDLib::c4MaxCard;

    // If its zero, then return the main file name, else find the next backup
    tCIDLib::TCard4 c4Index = c4StartAt;
    if (!c4StartAt)
    {
        TPathStr pathFile(m_strTargetPath);
        pathFile.AddLevel(m_strFileName);
        strToFill = pathFile;
    }
     else
    {
        //
        //  Look back from the 1st backup to the last, until we find a backup
        //  file.
        //
        TPathStr pathBak(m_strBakName);
        for (; c4Index <= m_c4Backups; c4Index++)
        {
            pathBak.CapAt(m_strBakName.c4Length());
            pathBak.AppendFormatted(c4Index);
            if (TFileSys::bExists(pathBak))
                break;
        }

        // We never found one, so return max card
        if (c4Index > m_c4Backups)
            c4Index = kCIDLib::c4MaxCard;
        else
            strToFill = pathBak;
    }
    return c4Index;
}


tCIDLib::TVoid TBackFileMgr::Demote()
{
    // Make sure they've set us up first
    CheckReady();

    // If there is a current max backup file, delete it
    TPathStr pathCur(m_strBakName);
    pathCur.AppendFormatted(m_c4Backups);

    try
    {
        if (TFileSys::bExists(pathCur))
            TFileSys::DeleteFile(pathCur);
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::FileSystem))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_CantDelMaxBak
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , pathCur
        );
    }

    //
    //  Ok, lets run through the files, from max-1 down to 1. For every one
    //  we find, we rename it to the next higher number.
    //
    TPathStr pathSrc(m_strBakName);
    for (tCIDLib::TCard4 c4Index = m_c4Backups - 1; c4Index >= 1; c4Index--)
    {
        // Build up the source and target file names
        pathSrc.CapAt(m_strBakName.c4Length());
        pathCur.CapAt(m_strBakName.c4Length());
        pathSrc.AppendFormatted(c4Index);
        pathCur.AppendFormatted(c4Index + 1);

        // And if the src exists, rename to the target
        try
        {
            if (TFileSys::bExists(pathSrc))
                TFileSys::Rename(pathSrc, pathCur);
        }

        catch(TError& errToCatch)
        {
            if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::FileSystem))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
            }

            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcBMgr_CantDemote
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , pathSrc
                , pathCur
            );
        }
    }

    // And now we can rename the current file to bak1
    pathSrc = m_strTargetPath;
    pathSrc.AddLevel(m_strFileName);
    pathCur.CapAt(m_strBakName.c4Length());
    pathCur.AppendFormatted(tCIDLib::TCard4(1));

    try
    {
        if (TFileSys::bExists(pathSrc))
            TFileSys::Rename(pathSrc, pathCur);
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::FileSystem))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_CantDemoteCur
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pathSrc
            , pathCur
        );
    }
}


tCIDLib::TVoid TBackFileMgr::PromoteFrom(const  tCIDLib::TCard4 c4TargetInd
                                        , const TString&        strSaveCurAs)
{
    // Make sure they've set us up first
    CheckReady();

    //
    //  They want to promote a particular backup file to the current file.
    //  So we have to toss any between the main file and the target, then
    //  promote the indicated file up.
    //
    //  Make sure that the target index is remotely valid.
    //
    if (c4TargetInd > m_c4Backups)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_BadStartInd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4TargetInd)
            , TCardinal(m_c4Backups)
        );
    }

    // Delete all the files up to the target
    TPathStr pathBak(m_strBakName);
    for (tCIDLib::TCard4 c4Index = 1; c4Index < c4TargetInd; c4Index++)
    {
        pathBak.CapAt(m_strBakName.c4Length());
        pathBak.AppendFormatted(c4Index);
        if (TFileSys::bExists(pathBak))
            TFileSys::DeleteFile(pathBak);
    }

    // Save the current file
    bSaveCur(strSaveCurAs);

    // Ok lets move the target file up to the current file
    pathBak.CapAt(m_strBakName.c4Length());
    pathBak.AppendFormatted(c4TargetInd);
    TPathStr pathTarget(m_strTargetPath);
    pathTarget.AddLevel(m_strFileName);
    TFileSys::Rename(pathBak, pathTarget);
}


tCIDLib::TVoid TBackFileMgr::Setup( const   TString&        strFileName
                                    , const tCIDLib::TCard4 c4Backups
                                    , const TString&        strTargetPath)
{
    m_c4Backups     = c4Backups;
    m_strFileName   = strFileName;
    m_strTargetPath = strTargetPath;

    // Make sure the passed data is legal, and setup some derived stuff
    CheckInitData();
}


const TString& TBackFileMgr::strFileName() const
{
    return m_strFileName;
}


const TString& TBackFileMgr::strTargetPath() const
{
    return m_strTargetPath;
}


// ---------------------------------------------------------------------------
//  TBackFileMgr: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBackFileMgr::CheckInitData()
{
    // Make sure the backup count is legal
    if (!m_c4Backups || (m_c4Backups > CIDLib_BackFileMgr::c4MaxBackups))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_BadCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TCardinal(CIDLib_BackFileMgr::c4MaxBackups)
        );
    }

    // And get out the base name part, and make sure that there is one
    TPathStr pathTmp(m_strFileName);
    if (!pathTmp.bQueryName(m_strBakName))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_HasNoNamePart
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , m_strFileName
        );
    }

    // Add it to the target path, and add the bak extension and store
    pathTmp.AddToBasePath(m_strTargetPath);
    pathTmp.AppendExt(L"Bak");
    m_strBakName = pathTmp;
}


tCIDLib::TVoid TBackFileMgr::CheckReady()
{
    if (m_strFileName.bIsEmpty())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBMgr_NotSet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
}



