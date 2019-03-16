//
// FILE NAME: TestMacroDbg_MainFrame.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
//  This file implements the TMacroDbgFrameWnd class, which provides the top
//  level window of the test program.
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
RTTIDecls(TMEngDbgFrameWnd,TMDIAppFrameWnd)


// ---------------------------------------------------------------------------
//  CLASS: TMEngDbgFrameWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDbgFrameWnd: Constructors and destructor
// ---------------------------------------------------------------------------
TMEngDbgFrameWnd::TMEngDbgFrameWnd() :

    m_mecmToUse(this)
    , m_pwndDbgClient(nullptr)
    , m_strState_NoSession(L"NoSession")
    , m_strState_Session(L"Session")
    , m_strTitleText(kTMDbgMsgs::midMsg_Title, facTestMacroDbg)
{
}

TMEngDbgFrameWnd::~TMEngDbgFrameWnd()
{
}


// ---------------------------------------------------------------------------
//  TMEngDbgFrameWnd: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this because we need to recursively distribute accellerator
//  processing. We first give the macro debugger window a shot at it, and he
//  will also pass it on to the appropriate active child window. If not handled,
//  we pass call our parent which will do any based on the menu we loaded.
//
tCIDLib::TBoolean
TMEngDbgFrameWnd::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    tCIDLib::TBoolean bHandled = kCIDLib::False;
    if (m_pwndDbgClient)
        bHandled = m_pwndDbgClient->bProcessAccel(pMsgData);

    if (!bHandled)
        bHandled = TParent::bProcessAccel(pMsgData);

    return bHandled;
}



// ---------------------------------------------------------------------------
//  TMEngDbgFrameWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngDbgFrameWnd::CreateDbgFrame()
{
    //
    //  If we have any state data from a previous run, then load it up and
    //  use it.
    //
    TArea areaUs(200, 200, 640, 520);
    TMacroDbgStateInfo stiSave;

    //
    //  Load our state file, if we find one. If not, then we get the
    //  default info created above.
    //
    if (TFileSys::bExists(L"Save.State"))
    {
        TBinFileInStream strmLoad
        (
            L"Save.State"
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        strmLoad >> areaUs >> stiSave;
        strmLoad.Close();
    }

    //
    //  Set the pointer to indicate we are starting, and then create us
    //  not visible yet.
    //
    facCIDCtrls().SetPointer(tCIDCtrls::ESysPtrs::AppStarting);

    CreateFrame
    (
        nullptr
        , facCIDCtrls().areaDefWnd()
        , L"Macro Debugger Test Harness"
        , tCIDCtrls::EWndStyles::StdFrame
        , tCIDCtrls::EExWndStyles::None
        , tCIDCtrls::EFrameStyles::None
        , kCIDLib::False
    );

    //
    //  Create the main debugger window. It will create its own child windowd
    //  as required.
    //
    TArea areaClient;
    QueryClientArea(areaClient, kCIDLib::True);

    m_pwndDbgClient =  new TMacroDbgMainWnd(&m_mecmToUse, &m_mefrToUse);
    m_pwndDbgClient->CreatePaneCont
    (
        *this
        , areaClient
        , 1
        , tCIDCtrls::EWndStyles::PaneContainer
        , tCIDCtrls::EExWndStyles::None // tCIDCtrls::EExWndStyles::Composited
    );
    m_pwndDbgClient->Init(stiSave, 0);

    // And now show ourself
    SetVisibility(kCIDLib::True);
}


TMacroDbgMainWnd& TMEngDbgFrameWnd::wndDbgClient()
{
    return *m_pwndDbgClient;
}


// ---------------------------------------------------------------------------
//  TMEngDbgFrameWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMEngDbgFrameWnd::AreaChanged(  const   TArea&                  areaPrev
                                , const TArea&                  areaNew
                                , const tCIDCtrls::EPosStates   ePosState
                                , const tCIDLib::TBoolean       bOrgChanged
                                , const tCIDLib::TBoolean       bSizeChanged
                                , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Keep the client sized to our client area
    if (m_pwndDbgClient)
    {
        TArea areaClient;
        QueryClientArea(areaClient, kCIDLib::True);
        m_pwndDbgClient->SetSize(areaClient.szArea(), kCIDLib::False);
    }
}


tCIDLib::TBoolean TMEngDbgFrameWnd::bAllowShutdown()
{
    TYesNoBox msgbAsk
    (
        facTestMacroDbg.strMsg(kTMDbgMsgs::midMsg_Title)
        , L"Do you want to exit the program?"
    );
    if (msgbAsk.bShowIt(*this))
    {
        if (bCloseCurrent())
        {
            SaveState();
            facCIDCtrls().ExitLoop(0);
        }
        kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TMEngDbgFrameWnd::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    // Set our menu bar
    SetMenuBar(facTestMacroDbg, kTestMacroDbg::ridMenu_Main);

#if 0
    // Set an icon for ourself
    SetFrameIcons
    (
        facCIDCtrls().icoLoadAppIcon(L"Tools")
        , facCIDCtrls().icoLoadSmallAppIcon(L"Tools")
    );
#endif

    //
    //  Update our class manager and file resolver now. We CANNOT do it in
    //  the constructor because that happens before this information is
    //  available.
    //
    m_mecmToUse.strBasePath(facTestMacroDbg.strClassPathRoot());
    m_mefrToUse.strBasePath(facTestMacroDbg.strFileSysRoot());


    //
    //  Create some window states that we will invoke to set up menus and controls
    //  and such.
    //
    TWndState wstateAdd(m_strState_NoSession);
    wstateAdd.AddMenuEnable(kTestMacroDbg::ridMenu_Session_Open);
    wstateAdd.AddMenuDisable(kTestMacroDbg::ridMenu_Session_Close);
    AddWndState(wstateAdd);

    wstateAdd.Reset(m_strState_Session);
    wstateAdd.AddMenuDisable(kTestMacroDbg::ridMenu_Session_Open);
    wstateAdd.AddMenuEnable(kTestMacroDbg::ridMenu_Session_Close);
    AddWndState(wstateAdd);

    // Start in no session state
    ApplyWndState(m_strState_NoSession);

    return kCIDLib::True;
}


tCIDLib::TVoid TMEngDbgFrameWnd::Destroyed()
{
    // And be sure to call our parent last
    TParent::Destroyed();
}


tCIDLib::TVoid
TMEngDbgFrameWnd::MenuCommand(  const   tCIDLib::TResId     ridItem
                                , const tCIDLib::TBoolean   bChecked
                                , const tCIDLib::TBoolean   bEnabled)
{
    if (ridItem == kTestMacroDbg::ridMenu_Session_Close)
    {
        bCloseCurrent();
    }
     else if (ridItem == kTestMacroDbg::ridMenu_Session_Exit)
    {
        bAllowShutdown();
    }
     else if (ridItem == kTestMacroDbg::ridMenu_Session_Open)
    {
        NewSession();
    }
}




// ---------------------------------------------------------------------------
//  TMEngDbgFrameWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngDbgFrameWnd::bCloseCurrent()
{
    // Make sure that they want to close the current session if we have one
    if (m_pwndDbgClient->bSessionOpen())
    {
        //
        //  Ask if they want to close. Use braces to make sure it destructs
        //  before we move on, sine the close session call we make below
        //  will open another one.
        //
        {
            TYesNoBox msgbTest
            (
                facTestMacroDbg.strMsg(kTMDbgMsgs::midQ_CloseSession)
            );
            if (!msgbTest.bShowIt(*this))
                return kCIDLib::False;
        }

        // Ask the debugger to stop this existing session
        if (!m_pwndDbgClient->bCloseSession())
            return kCIDLib::False;
    }

    // Update our title bar to get rid of the macro name
    TString strTmp(m_strTitleText);
    strTmp.Append(L" []");
    strWndText(strTmp);

    // Ok, it worked, so go back to the no session state
    ApplyWndState(m_strState_NoSession);

    return kCIDLib::True;
}


tCIDLib::TVoid TMEngDbgFrameWnd::NewSession()
{
    // If we currently have a file open, then close it down
    if (bCloseCurrent())
    {
        // Just use the class manager, which does the right thing
        const tCIDLib::EOpenRes eRes = m_mecmToUse.eSelectClass
        (
            m_pathCurClass, tCIDMacroEng::EResModes::ReadWrite
        );

        if (eRes != tCIDLib::EOpenRes::Cancel)
        {
            if (m_pwndDbgClient->bOpenSession(m_pathCurClass
                                             , eRes == tCIDLib::EOpenRes::NewFile))
            {
                TString strTmp(m_strTitleText);
                strTmp.Append(L" [");
                strTmp.Append(m_pathCurClass);
                strTmp.Append(L"]");
                strWndText(strTmp);

                // Ok, it worked, so go to the open session state
                ApplyWndState(m_strState_Session);
            }
        }
    }
}


tCIDLib::TVoid TMEngDbgFrameWnd::SaveState()
{
    const TArea areaUs(areaWnd());
    TMacroDbgStateInfo stiSave;
    m_pwndDbgClient->QueryStateInfo(stiSave);

    //
    //  Store our state file. We write out our current frame size first, then
    //  we store the debugger's state info.
    //
    TBinFileOutStream strmSave
    (
        L"Save.State"
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );
    strmSave << areaUs << stiSave << kCIDLib::FlushIt;
    strmSave.Close();
}

