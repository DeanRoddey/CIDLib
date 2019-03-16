//
// FILE NAME: TestMacroDbg_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
#include    "TestMacroDbg.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacTestMacroDbg,TGUIFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacTestMacroDbg
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacTestMacroDbg: Constructors and operators
// ---------------------------------------------------------------------------
TFacTestMacroDbg::TFacTestMacroDbg() :

    TGUIFacility
    (
        L"TestMacroDbg"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgsAndRes
    )
{
}

TFacTestMacroDbg::~TFacTestMacroDbg()
{
}


// ---------------------------------------------------------------------------
//  TFacTestMacroDbg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes
TFacTestMacroDbg::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    try
    {
        //
        //  We get two command line parms, the file system and class path
        //  roots.
        //
        if (TSysInfo::c4CmdLineParmCount() != 2)
        {
            TErrBox msgbErr
            (
                strMsg(kTMDbgMsgs::midMsg_Title)
                , strMsg(kTMDbgMsgs::midMsg_Usage)
            );
            msgbErr.ShowIt(TWindow::wndDesktop());
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  Get the parms, then make sure that they both exist as real
        //  directories out there.
        //
        TPathStr pathFileRoot;
        TFileSys::QueryFullPath(TSysInfo::strCmdLineParmAt(0), pathFileRoot);

        TPathStr pathClassRoot;
        TFileSys::QueryFullPath(TSysInfo::strCmdLineParmAt(1), pathClassRoot);

        // Remove any trailing separators
        pathFileRoot.bRemoveTrailingSeparator();
        pathClassRoot.bRemoveTrailingSeparator();

        // Make sure that they can both be found
        if (!TFileSys::bIsDirectory(pathFileRoot)
        ||  !TFileSys::bIsDirectory(pathClassRoot))
        {
            TErrBox msgbErr
            (
                strMsg(kTMDbgMsgs::midMsg_Title)
                , strMsg(kTMDbgErrs::errcFile_BadRootDir)

            );
            msgbErr.ShowIt(TWindow::wndDesktop());
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Complete both of the paths
        TFileSys::QueryFullPath(pathClassRoot, m_strClassPathRoot);
        TFileSys::QueryFullPath(pathFileRoot, m_strFileSysRoot);

        // Create the main window and enter the dispatch loop
        m_wndMainFrame.CreateDbgFrame();
    }

    catch(const TError& errToCatch)
    {
        TExceptDlg dlgErr
        (
            TWindow::wndDesktop()
            , strMsg(kTMDbgMsgs::midMsg_Title)
            , strMsg(kTMDbgMsgs::midMsg_Exception)
            , errToCatch
        );
        return tCIDLib::EExitCodes::InitFailed;
    }

    catch(...)
    {
        TErrBox msgbErr
        (
            strMsg(kTMDbgMsgs::midMsg_Title)
            , strMsg(kTMDbgMsgs::midMsg_UnknownExcept)
        );
        msgbErr.ShowIt(TWindow::wndDesktop());
        return tCIDLib::EExitCodes::InitFailed;
    }

    facCIDCtrls().uMainMsgLoop(m_wndMainFrame);

    try
    {
        m_wndMainFrame.Destroy();
    }

    catch(const TError& errToCatch)
    {
        TExceptDlg dlgErr
        (
            TWindow::wndDesktop()
            , strMsg(kTMDbgMsgs::midMsg_Title)
            , strMsg(kTMDbgMsgs::midMsg_Exception)
            , errToCatch
        );
    }

    catch(...)
    {
        TErrBox msgbErr
        (
            strMsg(kTMDbgMsgs::midMsg_Title)
            , strMsg(kTMDbgMsgs::midMsg_UnknownExcept)
        );
        msgbErr.ShowIt(TWindow::wndDesktop());
    }

    return tCIDLib::EExitCodes::Normal;
}


const TString& TFacTestMacroDbg::strClassPathRoot() const
{
    return m_strClassPathRoot;
}


const TString& TFacTestMacroDbg::strFileSysRoot() const
{
    return m_strFileSysRoot;
}


