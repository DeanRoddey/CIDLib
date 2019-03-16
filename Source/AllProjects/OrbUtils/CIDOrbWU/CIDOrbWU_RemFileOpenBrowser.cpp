//
// FILE NAME: CIDOrbWU_RemFileOpenBrowser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/2004
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
//  This file implements a derivative of the standard file open dialog's
//  pluggable browser interface. In this case, it works in terms of a remote
//  file browsing ORB interface.
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
#include "CIDOrbWU_.hpp"


// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TRemFileOpenBrowser,TFileOpenBrowser)



// ---------------------------------------------------------------------------
//   CLASS: TRemFileOpenBrowser
//  PREFIX: fob
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFileOpenBrowser: Public Destructor (ctors are hidden)
// ---------------------------------------------------------------------------
TRemFileOpenBrowser::TRemFileOpenBrowser()
{
}

TRemFileOpenBrowser::TRemFileOpenBrowser(const TString& strBindingName) :

    m_strBinding(strBindingName)
{
}

TRemFileOpenBrowser::~TRemFileOpenBrowser()
{
}


// ---------------------------------------------------------------------------
//  TFileOpenBrowser: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TRemFileOpenBrowser::bBuildSelectedPath(        THListWnd&  wndList
                                        , const TString&    strBase
                                        ,       TString&    strToFill)
{
    TListWndFileItem* plwiCheck = wndList.plwiCurrentAs<TListWndFileItem>();
    if (!plwiCheck)
    {
        // They shouldn't have called us
        #if CID_DEBUG_ON
        TPopUp::PopUpMsg
        (
            CID_FILE
            , CID_LINE
            , L"File Open Dialog 2"
            , 0
            , L"bBuildSelectedPath was called with empty file list"
        );
        #endif
    }

    m_pathTmp = strBase;
    m_pathTmp.AddLevel(plwiCheck->strText());
    m_pathTmp.Normalize();
    strToFill = m_pathTmp;

    // Return whether it's a directory or file
    return plwiCheck->fndbThis().bIsDirectory();
}


tCIDLib::TBoolean
TRemFileOpenBrowser::bFileExists(const TString& strToCheck)
{
    // Try to connect
    tCIDOrbUC::TRemBrowserProxy orbcRemHost
    (
        facCIDOrbUC().orbcRemBrowserProxy(m_strBinding, 2500)
    );
    return orbcRemHost->bFileExists(strToCheck);
}


tCIDLib::TBoolean
TRemFileOpenBrowser::bHasWildCards(const TString& strToCheck)
{
    return TFileSys::bHasWildCards(strToCheck);
}


tCIDLib::TBoolean
TRemFileOpenBrowser::bIsFQDirectory(const TString& strToCheck)
{
    m_pathTmp = strToCheck;
    if (!m_pathTmp.bIsFullyQualified() || m_pathTmp.bHasWildCards())
        return kCIDLib::False;

    // Try to connect
    tCIDOrbUC::TRemBrowserProxy orbcRemHost
    (
        facCIDOrbUC().orbcRemBrowserProxy(m_strBinding, 2500)
    );
    return orbcRemHost->bIsDirectory(strToCheck);
}


tCIDLib::TBoolean TRemFileOpenBrowser::bIsSelectedADir(THListWnd& wndList)
{
    TListWndFileItem* plwiCheck = wndList.plwiCurrentAs<TListWndFileItem>();
    if (plwiCheck)
        return plwiCheck->fndbThis().bIsDirectory();
    return kCIDLib::False;
}


tCIDLib::TBoolean
TRemFileOpenBrowser::bListSelect(       TEntryField&        wndEF
                                ,       THListWnd&          wndList
                                ,       TString&            strDirToFill)
{
    //
    //  If an invocation, we either take it as the selection if its a file,
    //  or change directories if its a directory. If its just a selection
    //  change, we output the text of the selected file to the entry field.
    //  If its a directory, then do nothing.
    //
    TListWndFileItem* plwiSel = wndList.plwiCurrentAs<TListWndFileItem>();
    if (!plwiSel->fndbThis().bIsDirectory())
        wndEF.strText(plwiSel->strText());
    else
        strDirToFill = plwiSel->strText();
    return plwiSel->fndbThis().bIsDirectory();
}


tCIDLib::TBoolean
TRemFileOpenBrowser::bLoadFiles(const   TFrameWindow&             wndOwner
                                , const TString&                  strSrcPath
                                , const TString&                  strWildCard
                                ,       TRefVector<TListWndItem>& colToFill)
{
    try
    {
        //
        //  Ask the remote host for a list of subdirs and files that match
        //  the wildcard.
        //
        TVector<TFindBuf>   colSubDirs;
        TVector<TFindBuf>   colFiles;
        tCIDOrbUC::TRemBrowserProxy orbcRemHost
        (
            facCIDOrbUC().orbcRemBrowserProxy(m_strBinding, 2500)
        );
        orbcRemHost->LoadFiles(strSrcPath, strWildCard, colSubDirs, colFiles);

        // Now load up list window items for them
        const tCIDLib::TCard4 c4Subs = colSubDirs.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Subs; c4Index++)
            colToFill.Add(new TListWndFileItem(colSubDirs[c4Index]));

        const tCIDLib::TCard4 c4Files = colFiles.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Files; c4Index++)
            colToFill.Add(new TListWndFileItem(colFiles[c4Index]));
    }

    catch(TError& errToCatch)
    {
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcDIter_FindFailure))
        {
            BadDirMsg(wndOwner, errToCatch, strSrcPath);
            return kCIDLib::False;
        }
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TRemFileOpenBrowser::bRemovePathLevel(TString& strTarget, const TString& strVolume)
{
    m_pathTmp = strTarget;
    if (m_pathTmp.bRemoveLevel())
    {
        //
        //  As long as it's still starting with the volume, we haven't gone too far.
        //  This can happen when a UNC volume is used, since it appears to have a
        //  removable level that's really part of the volume.
        //
        if (m_pathTmp.bStartsWithI(strVolume))
        {
            m_pathTmp.bRemoveTrailingSeparator();
            strTarget = m_pathTmp;
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TRemFileOpenBrowser::BuildPath(const   TString&    strBase
                                , const TString&    strName
                                ,       TString&    strToFill)
{
    m_pathTmp = strName;
    m_pathTmp.AddToBasePath(strBase);
    strToFill = m_pathTmp;
}


tCIDLib::TCard4
TRemFileOpenBrowser::c4ReadFile(const TString& strToRead, THeapBuf& mbufToFill)
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

    return flRead.c4ReadBuffer
    (
        mbufToFill
        , tCIDLib::TCard4(flRead.c8CurSize())
    );
}


tCIDLib::TVoid
TRemFileOpenBrowser::ChangeVolume(TComboBox& wndVolumes, TString& strLastCurDir)
{
    //
    //  Get the selected volume item, and ask it what directory was last in
    //  use on it.
    //
    TListWndVolItem* plwiNew = wndVolumes.plwiCurrentAs<TListWndVolItem>();
    strLastCurDir = plwiNew->strLastPath();
}


tCIDLib::TVoid
TRemFileOpenBrowser::DeleteSelected(THListWnd& wndList)
{
    TListWndFileItem* plwiSel = wndList.plwiCurrentAs<TListWndFileItem>();

    // Try to connect
    tCIDOrbUC::TRemBrowserProxy orbcRemHost
    (
        facCIDOrbUC().orbcRemBrowserProxy(m_strBinding, 2500)
    );
    orbcRemHost->RemoveSelected(plwiSel->fndbThis().pathFileName());
}


// The new name will be fully qualified
tCIDLib::ERenameRes
TRemFileOpenBrowser::eRenameSelected(           THListWnd&  wndList
                                        , const TString&    strNewName)
{
    TListWndFileItem* plwiSel = wndList.plwiCurrentAs<TListWndFileItem>();
    TPathStr pathOld(plwiSel->fndbThis().pathFileName());

    // Try to connect
    tCIDOrbUC::TRemBrowserProxy orbcRemHost
    (
        facCIDOrbUC().orbcRemBrowserProxy(m_strBinding, 5000)
    );

    // See if the new name already exists. If so, return a failure status
    if (orbcRemHost->bFileExists(strNewName))
        return tCIDLib::ERenameRes::AlreadyExists;

    // Make sure the src does exist
    if (!orbcRemHost->bFileExists(pathOld))
        return tCIDLib::ERenameRes::NotFound;

    // Looks reasonable so give it a try
    orbcRemHost->Rename(pathOld, strNewName, plwiSel->fndbThis().bIsDirectory());

    //
    //  It worked, so update the list window item text. It may get removed by
    //  the file dialog, if the change has moved it out of the displayed
    //  scope. But we aren't responsible for that.
    //
    plwiSel->UpdatePath(strNewName);

    return tCIDLib::ERenameRes::OK;
}


tCIDLib::TVoid
TRemFileOpenBrowser::Initialize(        TString&    strInitPath
                                ,       TTypesList& colFileTypes
                                , const TString&    strTitleText)
{
    // Save away the title text we are to use for error dialogs
    strTitle(strTitleText);

    //
    //  If the initial path is empty, set it to the user data path. Else, they
    //  are passing one in so check it.
    //
    if (strInitPath.bIsEmpty())
    {
        strInitPath = TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::UserAppData);
    }
     else
    {
        // It has to be a fully qualified path
        m_pathTmp = strInitPath;
        if (!m_pathTmp.bIsFullyQualified())
        {
            facCIDWUtils().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kWUtilErrs::errcFOpen_NotFQDirectory
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strInitPath
            );
        }
    }

    // Put a single entry into the file types list if its empty
    if (colFileTypes.bIsEmpty())
    {
        colFileTypes.objAdd
        (
            TKeyValuePair
            (
                facCIDWUtils().strMsg(kWUtilMsgs::midDlg_FOpen_AllFiles)
                , kCIDLib::pszAllFilesSpec
            )
        );
    }
}


tCIDLib::TVoid
TRemFileOpenBrowser::LoadVolumes(const  TString&    strPath
                                ,       TComboBox&  wndToLoad)
{
    // Ask the remote server for it's volume list
    TVolComboBox::TVolList  colVols
    (
        29
        , new TStringKeyOps
        , &TVolumeInfo::strVolKey
    );

    TVolComboBox::TFailList colFailures
    (
        29
        , new TStringKeyOps
        , &TVolFailureInfo::strVolKey
    );

    // Try to connect
    tCIDOrbUC::TRemBrowserProxy orbcRemHost
    (
        facCIDOrbUC().orbcRemBrowserProxy(m_strBinding, 2500)
    );
    if (orbcRemHost->c4QueryVolumes(colVols, colFailures))
    {
        static_cast<TVolComboBox&>(wndToLoad).Reload
        (
            strPath, colVols, colFailures
        );
    }
     else
    {
        wndToLoad.RemoveAll();
    }
}


tCIDLib::TVoid
TRemFileOpenBrowser::MakeNewDir(const   TString&    strCurDir
                                , const TString&    strSubDir
                                ,       THListWnd&  wndFileList)
{
    // Try to connect
    tCIDOrbUC::TRemBrowserProxy orbcRemHost
    (
        facCIDOrbUC().orbcRemBrowserProxy(m_strBinding, 2500)
    );
    orbcRemHost->MakeDirectory(strCurDir, strSubDir);

    TFindBuf fndbNew;
    if (!orbcRemHost->bFind(strCurDir, strSubDir, fndbNew))
    {
        facCIDWUtils().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kWUtilErrs::errcFOpen_NewDirNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }
    wndFileList.AddItem(new TListWndFileItem(fndbNew));
}


tCIDLib::TVoid TRemFileOpenBrowser::RemovePathPart(TString& strTarget)
{
    m_pathTmp = strTarget;
    m_pathTmp.bRemovePath();
    strTarget = m_pathTmp;
}


TString TRemFileOpenBrowser::strSelected(THListWnd& wndList)
{
    TListWndFileItem* plwiSel = wndList.plwiCurrentAs<TListWndFileItem>();
    TPathStr pathTmp(plwiSel->fndbThis().pathFileName());
    pathTmp.bRemovePath();
    return pathTmp;
}


TString TRemFileOpenBrowser::strVolComboClass() const
{
    // We use the standard volume combo box
    return TString(L"TVolComboBox");
}


tCIDLib::TVoid
TRemFileOpenBrowser::SelectVolume(          TComboBox&  wndVolumes
                                    , const TString&    strVolume)
{
    static_cast<TVolComboBox&>(wndVolumes).SelectVolume(strVolume);
}


tCIDLib::TVoid
TRemFileOpenBrowser::SetCurDir(TComboBox& wndVolumes, const TString& strToSet)
{
    TListWndVolItem* plwiCur = wndVolumes.plwiCurrentAs<TListWndVolItem>();
    plwiCur->strLastPath(strToSet);
}


tCIDLib::TVoid
TRemFileOpenBrowser::SplitPath(const   TString&    strToSplit
                                ,       TString&    strPath
                                ,       TString&    strNameExt)
{
    m_pathTmp = strToSplit;
    m_pathTmp.bExtractNameExt(strNameExt);
    m_pathTmp.bRemoveTrailingSeparator();
    strPath = m_pathTmp;
}



// ---------------------------------------------------------------------------
//  TRemFileOpenBrowser: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TRemFileOpenBrowser::SetBinding(const TString& strBindingName)
{
    m_strBinding = strBindingName;
}

