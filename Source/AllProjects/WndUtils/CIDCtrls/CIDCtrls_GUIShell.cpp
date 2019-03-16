//
// FILE NAME: CIDWnd_GUIShell.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/27/2001
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
//  This file implements some Windows shell manipulation APIs.
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
#include    "CIDCtrls_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"

#include    <shlobj.h>
#include    <intshcut.h>



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TGUIShellShortCut,TObject)



// ---------------------------------------------------------------------------
//  A local janitor that makes it safe to create the Windows objects we need
//  to have for link generation operations operation.
// ---------------------------------------------------------------------------
class TShellJan
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TShellJan();

        ~TShellJan();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        IPersistFile& pflThis();

        IShellLink& shlThis();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TShellJan(const TShellJan&);
        tCIDLib::TVoid operator=(const TShellJan&);


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid StaticInit();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pflThis
        //      The persist file that we need to save our changes
        //
        //  m_shlThis
        //      The shell object we need to do our operations.
        // -------------------------------------------------------------------
        IPersistFile*   m_pflThis;
        IShellLink*     m_pshlThis;
};


// ---------------------------------------------------------------------------
//  TShellJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TShellJan::TShellJan() :

    m_pflThis(0)
    , m_pshlThis(0)
{
    //Instantiate the shell COM class
    HRESULT hRes = ::CoCreateInstance
    (
        CLSID_ShellLink
        , NULL
        , CLSCTX_INPROC_SERVER
        , IID_IShellLink
        , (LPVOID*)&m_pshlThis
    );

    if (!SUCCEEDED(hRes))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcShell_ShellLink
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TInteger(hRes)
        );
    }

    //Also get a pointer to IPersistFile
    hRes = m_pshlThis->QueryInterface(IID_IPersistFile, (LPVOID*)&m_pflThis);
    if (!SUCCEEDED(hRes))
    {
        // Let the shell link go since this failed, and then return failure
        m_pshlThis->Release();

        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcShell_PersistFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TInteger(hRes)
        );
    }
}

TShellJan::~TShellJan()
{
    // Let our two interfaces go
    m_pflThis->Release();
    m_pshlThis->Release();
}


// ---------------------------------------------------------------------------
//  TShellJan: Public, non-virtual methods
// ---------------------------------------------------------------------------
IPersistFile& TShellJan::pflThis()
{
    return *m_pflThis;
}


IShellLink& TShellJan::shlThis()
{
    return *m_pshlThis;
}



// ---------------------------------------------------------------------------
//   CLASS: TGUIShellShortCut
//  PREFIX: gssc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGUIShellShortCut: Constructors and Destructor
// ---------------------------------------------------------------------------
TGUIShellShortCut::TGUIShellShortCut() :

    m_eInitState(tCIDCtrls::EPosStates::Count)
{
}

TGUIShellShortCut::TGUIShellShortCut(const  TString&                strPathToSet
                                    , const TString&                strDescrToSet
                                    , const TString&                strArgsToSet
                                    , const TString&                strIconPath
                                    , const TString&                strLinkName
                                    , const TString&                strWorkingPath
                                    , const tCIDCtrls::EPosStates   eInitState) :

    m_eInitState(eInitState)
    , m_strArgs(strArgsToSet)
    , m_strDescr(strDescrToSet)
    , m_strIconPath(strIconPath)
    , m_strLinkName(strLinkName)
    , m_strPath(strPathToSet)
    , m_strWorkingPath(strWorkingPath)
{
}

TGUIShellShortCut::TGUIShellShortCut(const TGUIShellShortCut& gsscToCopy) :

    m_eInitState(gsscToCopy.m_eInitState)
    , m_strArgs(gsscToCopy.m_strArgs)
    , m_strDescr(gsscToCopy.m_strDescr)
    , m_strIconPath(gsscToCopy.m_strIconPath)
    , m_strLinkName(gsscToCopy.m_strLinkName)
    , m_strPath(gsscToCopy.m_strPath)
    , m_strWorkingPath(gsscToCopy.m_strWorkingPath)
{
}

TGUIShellShortCut::~TGUIShellShortCut()
{
}


// ---------------------------------------------------------------------------
//  TGUIShellShortCut: Public operators
// ---------------------------------------------------------------------------
TGUIShellShortCut&
TGUIShellShortCut::operator=(const TGUIShellShortCut&  gsscToAssign)
{
    if (this != &gsscToAssign)
    {
        m_eInitState    = gsscToAssign.m_eInitState;
        m_strArgs       = gsscToAssign.m_strArgs;
        m_strDescr      = gsscToAssign.m_strDescr;
        m_strIconPath   = gsscToAssign.m_strIconPath;
        m_strLinkName   = gsscToAssign.m_strLinkName;
        m_strPath       = gsscToAssign.m_strPath;
        m_strWorkingPath= gsscToAssign.m_strWorkingPath;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TGUIShellShortCut: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGUIShellShortCut::bIsInitialized() const
{
    return (m_eInitState != tCIDCtrls::EPosStates::Count);
}

tCIDCtrls::EPosStates TGUIShellShortCut::eInitState() const
{
    return m_eInitState;
}

const TString& TGUIShellShortCut::strArgs() const
{
    return m_strArgs;
}

const TString& TGUIShellShortCut::strDescr() const
{
    return m_strDescr;
}

const TString& TGUIShellShortCut::strIconPath() const
{
    return m_strIconPath;
}

const TString& TGUIShellShortCut::strLinkName() const
{
    return m_strLinkName;
}

const TString& TGUIShellShortCut::strPath() const
{
    return m_strPath;
}

const TString& TGUIShellShortCut::strWorkingPath() const
{
    return m_strWorkingPath;
}


tCIDLib::TVoid
TGUIShellShortCut::Set( const   TString&                strPathToSet
                        , const TString&                strDescrToSet
                        , const TString&                strArgsToSet
                        , const TString&                strIconPath
                        , const TString&                strLinkName
                        , const TString&                strWorkingPath
                        , const tCIDCtrls::EPosStates   eInitState)
{
    m_eInitState    = eInitState;
    m_strArgs       = strArgsToSet;
    m_strDescr      = strDescrToSet;
    m_strIconPath   = strIconPath;
    m_strLinkName   = strLinkName;
    m_strPath       = strPathToSet;
    m_strWorkingPath= strWorkingPath;
}


// ---------------------------------------------------------------------------
//  NAMESPACE: TGUIShell
// ---------------------------------------------------------------------------

// Query a
tCIDLib::TBoolean
TGUIShell::bQueryShortCut(  const   TString&            strLinkDir
                            ,       TGUIShellShortCut&  gsscToFill
                            ,       tCIDLib::TCard4&    c4Error)
{
    HRESULT hRes;

    // Create a shell janitor to get our needed access
    TShellJan janShell;

    // Try to load the link
    hRes = janShell.pflThis().Load(strLinkDir.pszBuffer(), STGM_READ);
    if (!SUCCEEDED(hRes))
    {
        c4Error = hRes;
        return kCIDLib::False;
    }

    // That worked, so let's pull the data out and fill in the short cut
    tCIDLib::TCh szArgs[kCIDLib::c4MaxPathLen + 1];
    hRes = janShell.shlThis().GetArguments(szArgs, kCIDLib::c4MaxPathLen);
    if (!SUCCEEDED(hRes))
    {
        c4Error = hRes;
        return kCIDLib::False;
    }

    tCIDLib::TCh szPath[kCIDLib::c4MaxPathLen + 1];
    hRes = janShell.shlThis().GetPath
    (
        szPath, kCIDLib::c4MaxPathLen, 0, SLGP_UNCPRIORITY
    );
    if (!SUCCEEDED(hRes))
    {
        c4Error = hRes;
        return kCIDLib::False;
    }

    tCIDLib::TCh szWorkingDir[kCIDLib::c4MaxPathLen + 1];
    hRes = janShell.shlThis().GetWorkingDirectory
    (
        szWorkingDir, kCIDLib::c4MaxPathLen
    );
    if (!SUCCEEDED(hRes))
    {
        c4Error = hRes;
        return kCIDLib::False;
    }

    tCIDLib::TCh szDescr[kCIDLib::c4MaxPathLen + 1];
    hRes = janShell.shlThis().GetDescription(szDescr, kCIDLib::c4MaxPathLen);
    if (!SUCCEEDED(hRes))
    {
        c4Error = hRes;
        return kCIDLib::False;
    }

    tCIDLib::TSInt iDummy;
    tCIDLib::TCh szIconLoc[kCIDLib::c4MaxPathLen + 1];
    hRes = janShell.shlThis().GetIconLocation
    (
        szIconLoc, kCIDLib::c4MaxPathLen, &iDummy
    );
    if (!SUCCEEDED(hRes))
    {
        c4Error = hRes;
        return kCIDLib::False;
    }

    tCIDLib::TSInt iShow;
    hRes = janShell.shlThis().GetShowCmd(&iShow);
    if (!SUCCEEDED(hRes))
    {
        c4Error = hRes;
        return kCIDLib::False;
    }

    // Convert the show command to our enum
    tCIDCtrls::EPosStates eInitState = tCIDCtrls::EPosStates::Restored;
    if (iShow == SW_SHOWMAXIMIZED)
        eInitState = tCIDCtrls::EPosStates::Maximized;
    else if (iShow == SW_SHOWMINIMIZED)
        eInitState = tCIDCtrls::EPosStates::Minimized;


    // OK, we can now set up the short cut object
    gsscToFill.Set
    (
        szPath
        , szDescr
        , szArgs
        , szIconLoc
        , szDescr
        , szWorkingDir
        , eInitState
    );
    return kCIDLib::True;
}



// Create a new short cut
tCIDLib::TVoid TGUIShell::CreateShortCut(const TGUIShellShortCut& gsscToCreate)
{
    // We want this on the desktop so we have to get the desktop path
    tCIDLib::TCh szDesktopBuf[kCIDLib::c4MaxPathLen + 1];
    HRESULT hRes = ::SHGetFolderPathW
    (
        0
        , CSIDL_COMMON_DESKTOPDIRECTORY
        , 0
        , SHGFP_TYPE_CURRENT
        , szDesktopBuf
    );

    if (!SUCCEEDED(hRes))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcShell_GetDesktopPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TInteger(hRes)
        );
    }

    // And now call the other version with this path as the link directory
    CreateShortCut(szDesktopBuf, gsscToCreate);
}


tCIDLib::TVoid
TGUIShell::CreateShortCut(  const   TString&            strLinkDir
                            , const TGUIShellShortCut&  gsscToCreate)
{
    // Make sure the item is set up
    if (!gsscToCreate.bIsInitialized())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcShell_ShortCutNotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , strLinkDir
        );
    }

    HRESULT hRes;

    // Create a shell janitor to get our needed access
    TShellJan janShell;

    // Set the fields that we want this new short cut to have
    tCIDLib::TBoolean bFailed = kCIDLib::False;
    hRes = janShell.shlThis().SetPath(gsscToCreate.strPath().pszBuffer());
    if (!SUCCEEDED(hRes))
        bFailed = kCIDLib::True;

    hRes = janShell.shlThis().SetDescription(gsscToCreate.strDescr().pszBuffer());
    if (!SUCCEEDED(hRes))
        bFailed = kCIDLib::True;

    hRes = janShell.shlThis().SetArguments(gsscToCreate.strArgs().pszBuffer());
    if (!SUCCEEDED(hRes))
        bFailed = kCIDLib::True;

    hRes = janShell.shlThis().SetWorkingDirectory(gsscToCreate.strWorkingPath().pszBuffer());
    if (!SUCCEEDED(hRes))
        bFailed = kCIDLib::True;

    hRes = janShell.shlThis().SetIconLocation(gsscToCreate.strIconPath().pszBuffer(), 0);
    if (!SUCCEEDED(hRes))
        bFailed = kCIDLib::True;

    // Translate our position state into a Win32 one and set it
    tCIDLib::TInt4 i4XlatPos;
    if (gsscToCreate.eInitState() == tCIDCtrls::EPosStates::Minimized)
        i4XlatPos = SW_MINIMIZE;
    else if (gsscToCreate.eInitState() == tCIDCtrls::EPosStates::Maximized)
        i4XlatPos = SW_MAXIMIZE;
    else if (gsscToCreate.eInitState() == tCIDCtrls::EPosStates::Restored)
        i4XlatPos = SW_RESTORE;
    else
    {
        // <TBD> Do something here on debug
        i4XlatPos = SW_RESTORE;
    }

    hRes = janShell.shlThis().SetShowCmd(i4XlatPos);
    if (!SUCCEEDED(hRes))
        bFailed = kCIDLib::True;

    if (bFailed)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcShell_ShortCutSetFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , gsscToCreate.strLinkName()
        );
    }

    TPathStr pathTarget(strLinkDir);
    pathTarget.AddLevel(gsscToCreate.strLinkName());
    pathTarget.AppendExt(L".lnk");

    hRes = janShell.pflThis().Save(pathTarget.pszBuffer(), TRUE);
    if (!SUCCEEDED(hRes))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcShell_SetSavePath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pathTarget
            , TInteger(hRes)
        );
    }

    // Complete the save
    janShell.pflThis().SaveCompleted(pathTarget.pszBuffer());
}


tCIDLib::TVoid
TGUIShell::CreateShortCutGroup( const   TString&        strFolderName
                                , const TShortCutList&  colShortCuts)
{
    //
    //  For all of the short cuts in the list, create a short cut underneath
    //  the indicated path.
    //
    TColCursor<TGUIShellShortCut>* pcursShortCuts = colShortCuts.pcursNew();
    TJanitor<TColCursor<TGUIShellShortCut>> janCursor(pcursShortCuts);

    for (; pcursShortCuts->bIsValid(); pcursShortCuts->bNext())
        CreateShortCut(strFolderName, pcursShortCuts->objRCur());
}


tCIDLib::TVoid
TGUIShell::CreateShortCutGroup( const   tCIDLib::ESpecialPaths  eFolder
                                , const TShortCutList&          colShortCuts)
{
    // And call the other version to do the work
    CreateShortCutGroup(TSysInfo::strSpecialPath(eFolder), colShortCuts);
}


// Causes the default web browser to be invoked with theindicated URL
tCIDLib::TVoid
TGUIShell::InvokeDefBrowser(TWindow& wndOwner, const TString& strURL)
{
    ::ShellExecute(wndOwner.hwndThis(), 0, strURL.pszBuffer(), 0, 0, SW_SHOWNORMAL);
}


// Causes the default shell action for the indicated file to be invoked
tCIDLib::TVoid
TGUIShell::InvokeDefFileAction(         TWindow&            wndOwner
                                , const TString&            strPath
                                , const tCIDCtrls::EPosStates ePosState)
{
    // Translate the position state
    DWORD ShowType;
    if (ePosState == tCIDCtrls::EPosStates::Minimized)
        ShowType = SW_SHOWMINIMIZED;
    else if (ePosState == tCIDCtrls::EPosStates::Maximized)
        ShowType = SW_SHOWMAXIMIZED;
    else
        ShowType = SW_SHOWNORMAL;

    HINSTANCE hRes = ::ShellExecute(0, 0, strPath.pszBuffer(), 0, 0, ShowType);

    // Cast the fake instance handle to a result code
    int iErr = (int)hRes;

    if (iErr <= 32)
    {
        TString strReason;

        // Check for some specific ones
        if ((iErr == ERROR_FILE_NOT_FOUND)
        ||  (iErr == ERROR_PATH_NOT_FOUND))
        {
            strReason = L"Bad path or file name";
        }
         else if (iErr == SE_ERR_ACCESSDENIED)
        {
            strReason = L"Access denied";
        }
         else
        {
            strReason = L"System error ";
            strReason.AppendFormatted(tCIDLib::TInt4(iErr));
        }

        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcShell_InvokeFile
            , strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strReason
        );
    }
}


