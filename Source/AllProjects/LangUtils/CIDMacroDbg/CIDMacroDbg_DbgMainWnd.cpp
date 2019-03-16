//
// FILE NAME: CIDMacroDbg_DbgMainWnd.cpp
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
//  This file implements the TMacroDbgMainWnd class, which provides the top
//  level window of the debugger.
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
#include    "CIDMacroDbg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCMLDbgChangeInfo,TCtrlNotify)
RTTIDecls(TCMLCustomTab, TTabWindow)
RTTIDecls(TMacroDbgMainWnd,TFrameWindow)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroDbg_DbgMainWnd
{
    // -----------------------------------------------------------------------
    //  Some ids that the callbacks from the macro execution thread post to
    //  us so that we know what the debugger is doing.
    //
    //  i4Broken          - The runner thread saw a break request or hit a
    //                      break point, and is now blocked and waiting.
    //  i4EndSession      - We were forced to exit due to the session being
    //                      closed.
    //  i4Finished        - The macro has finished running
    // -----------------------------------------------------------------------
    const tCIDLib::TInt4    i4Broken            = 1;
    const tCIDLib::TInt4    i4EndSession        = 2;
    const tCIDLib::TInt4    i4Finished          = 3;
    const tCIDLib::TInt4    i4BreakAndGo        = 4;


    // -----------------------------------------------------------------------
    //  Posted to ourself. When the user double clicks on an error in the
    //  build results window we get an event and move the editor cursor to
    //  the location. We also want to take the focus, but can't do it then
    //  so we post ourself an event to take the focus.
    // -----------------------------------------------------------------------
    const tCIDLib::TInt4    i4ErrLocGo          = 10;


    // -----------------------------------------------------------------------
    //  To create unique thread names for each instance
    // -----------------------------------------------------------------------
    tCIDLib::TCard4         c4ThreadInst    = 1;


    // -----------------------------------------------------------------------
    //  The gap between the editor the file list below it
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4EditorGap     = 16;


    // -----------------------------------------------------------------------
    //  Some pseudo menu commands that we want to add to the accel table, but
    //  they aren't really in any menu. It's just for keyboard invocation, and
    //  the accellerator handling invokes the menu handler.
    //
    //  Make sure to keep these out of the possible range of the actual context
    //  menu ids.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4FakeMenu_ToggleBPoint     = 5000;
    const tCIDLib::TCard4   c4FakeMenu_ToggleBPEnable   = 5001;
    const tCIDLib::TCard4   c4FakeMenu_FindNext         = 5002;


    // -----------------------------------------------------------------------
    //  The special characters
    // -----------------------------------------------------------------------
    const tCIDLib::TCh achSpecialChars[] =
    {
        kCIDLib::chAsterisk
        , kCIDLib::chCloseBracket
        , kCIDLib::chCloseParen
        , kCIDLib::chColon
        , kCIDLib::chComma
        , kCIDLib::chEquals
        , kCIDLib::chExclamation
        , kCIDLib::chOpenBracket
        , kCIDLib::chOpenParen
        , kCIDLib::chPeriod
        , kCIDLib::chQuotation
        , kCIDLib::chSemiColon
        , kCIDLib::chNull
    };
}


// ---------------------------------------------------------------------------
//   CLASS: TCMLDbgChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCMLDbgChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TCMLDbgChangeInfo::TCMLDbgChangeInfo( const tCIDMacroDbg::EEvents   eEvent
                                    , const TString&                strCurFile
                                    , const tCIDLib::TCard4         c4LineNum
                                    , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4LineNum(c4LineNum)
    , m_eEvent(eEvent)
    , m_strCurFile(strCurFile)
{
}

TCMLDbgChangeInfo::TCMLDbgChangeInfo(const TString& strNewState, const TWindow& wndSrc) :

    TCtrlNotify(wndSrc)
    , m_c4LineNum(kCIDLib::c4MaxCard)
    , m_eEvent(tCIDMacroDbg::EEvents::IDEState)
    , m_strCurFile(strNewState)
{
}

TCMLDbgChangeInfo::TCMLDbgChangeInfo(const TCMLDbgChangeInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_c4LineNum(wnotSrc.m_c4LineNum)
    , m_eEvent(wnotSrc.m_eEvent)
    , m_strCurFile(wnotSrc.m_strCurFile)
{
}

TCMLDbgChangeInfo::~TCMLDbgChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TCMLDbgChangeInfo: Public operators
// ---------------------------------------------------------------------------
TCMLDbgChangeInfo& TCMLDbgChangeInfo::operator=(const TCMLDbgChangeInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);
        m_c4LineNum = wnotSrc.m_c4LineNum;
        m_eEvent = wnotSrc.m_eEvent;
        m_strCurFile = wnotSrc.m_strCurFile;
    }
    return *this;
}

// ---------------------------------------------------------------------------
//  TCMLDbgChangeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TCMLDbgChangeInfo::c4LineNum() const
{
    return m_c4LineNum;
}


tCIDMacroDbg::EEvents TCMLDbgChangeInfo::eEvent() const
{
    return m_eEvent;
}


const TString& TCMLDbgChangeInfo::strCurFile() const
{
    return m_strCurFile;
}


const TString& TCMLDbgChangeInfo::strState() const
{
    CIDAssert(m_eEvent == tCIDMacroDbg::EEvents::IDEState, L"The event is not an IDE state change");

    // We store the state in the current file
    return m_strCurFile;
}



// ---------------------------------------------------------------------------
//  CLASS: TCMLCustomTab
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCMLCustomTab: Constructors and Destructor
// ---------------------------------------------------------------------------
TCMLCustomTab::~TCMLCustomTab()
{
}


// ---------------------------------------------------------------------------
//  TCMLCustomTab: Hidden constructors
// ---------------------------------------------------------------------------
TCMLCustomTab::TCMLCustomTab() :

    TTabWindow(L"$CustomTab", L"", kCIDLib::False, kCIDLib::True)
{
}




// ---------------------------------------------------------------------------
//  CLASS: TMacroDbgMainWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEntryField: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TMacroDbgMainWnd::nidChange(L"CMLDbgChangeEvent");


// ---------------------------------------------------------------------------
//  TMacroDbgMainWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TMacroDbgMainWnd::TMacroDbgMainWnd( MMEngClassMgr* const        pmecmToUse
                                    , MMEngFileResolver* const  pmefrToUse) :

    m_c2OneTimeMethod(kMacroEng::c2BadId)
    , m_c4GoToLine(0)
    , m_c4LastLine(0)
    , m_c4NewIP(0)
    , m_c4NewLine(0)
    , m_c4TabId_Session(kCIDLib::c4MaxCard)
    , m_eActionReq(EActionReqs::None)
    , m_eCurState(EStates::NoSession)
    , m_eOneTimeFlag(EOneTimes::None)
    , m_evSync(tCIDLib::EEventStates::Reset)
    , m_menuContext(L"Editor Popup")
    , m_mtxSync()
    , m_pcuctxRights(nullptr)
    , m_pmeciLast(nullptr)
    , m_pmeciNew(nullptr)
    , m_pmeciMain(nullptr)
    , m_pmethLast(nullptr)
    , m_pmethNew(nullptr)
    , m_pmethiLast(nullptr)
    , m_pmethiNew(nullptr)
    , m_pmecmToUse(pmecmToUse)
    , m_pmecvLast(nullptr)
    , m_pmecvNew(nullptr)
    , m_pmefrToUse(pmefrToUse)
    , m_pwndCustomTab(nullptr)
    , m_pwndTabBreakPnts(nullptr)
    , m_pwndTabBuildRes(nullptr)
    , m_pwndTabConOut(nullptr)
    , m_pwndTabLocals(nullptr)
    , m_pwndTabMembers(nullptr)
    , m_pwndTabParms(nullptr)
    , m_pwndTabSearch(nullptr)
    , m_pwndTabsSrc(nullptr)
    , m_pwndTabsStatus(nullptr)
    , m_strTmpOutput(8192UL)
    , m_strmConStream
      (
        kCIDMacroDbg_::c4MaxMacroOutLines
        , tCIDLib::EReadModes::Destructive
      )
    , m_tmidMacroOut(0)
    , m_thrRun
      (
        TString(L"CIDMacroDbgThread")
          + TString(TCardinal(CIDMacroDbg_DbgMainWnd::c4ThreadInst++))
        , TMemberFunc<TMacroDbgMainWnd>(this, &TMacroDbgMainWnd::eRunThread)
      )
{
}

TMacroDbgMainWnd::~TMacroDbgMainWnd()
{
}


// ---------------------------------------------------------------------------
//  TMacroDbgMainWnd: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this because we need to recursively distribute accellerator
//  processing. The client program should give us a shot at accellerators (if
//  that is appropriate based on active window.) We will then pass it to the
//  appropriate child window as well and return his return.
//
//  Currently the only one that matters are the editor windows. So we pass this
//  to the active source editor tab.
//
tCIDLib::TBoolean
TMacroDbgMainWnd::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    // Let the tabbed window itself have a shot, to handle tab selection
    if (m_pwndTabsSrc)
    {
        if (m_pwndTabsSrc->bProcessAccel(pMsgData))
            return kCIDLib::True;
    }

    // Let the search tab check it
    if (m_pwndTabSearch && m_pwndTabSearch->bContainsFocus())
    {
        if (m_pwndTabSearch->bProcessAccel(pMsgData))
            return kCIDLib::True;
    }

    // See if it's one of ours or not
    return m_accelDbg.bProcessAccel(*this, pMsgData);
}


//
//  The engine calls this when the CML class does a sleep operation that's
//  long than about a second or so. It will break that sleep time up into
//  second long chunks and call us each time around, so that we can tell him
//  if the user wants to break out.
//
tCIDLib::TBoolean TMacroDbgMainWnd::bSleepTest() const
{
    //
    //  As long as the requested action is none, we just return true to
    //  tell him to keep sleeping for the next round. Otherwise we'll
    //  fall back out and he'll call the next line after the sleep and
    //  will handle the action request in eAtLine below().
    //
    return (m_eActionReq == EActionReqs::None);
}


tCIDLib::TVoid TMacroDbgMainWnd::CallStackChange()
{
    //
    //  Bump the stack changes value. We don't need to sync this, since the
    //  GUI thread only looks at it when in stopped state.
    //
    m_c4StackChanges++;
}


tCIDMacroEng::EDbgActs
TMacroDbgMainWnd::eAtLine(  const   TMEngClassInfo&     meciCurrent
                            , const TMEngMethodInfo&    methiCurrent
                            , const TMEngOpMethodImpl&  methCurrent
                            ,       TMEngClassVal&      mecvInstance
                            , const tCIDLib::TCard4     c4LineNum
                            , const tCIDLib::TCard4     c4NewIP)
{
    //
    //  If no specific action is requested, then lets just do a check for
    //  a break point. If we find none, then just continue. If we find one,
    //  the update the action to break and fall through.
    //
    if (m_eActionReq == EActionReqs::None)
    {
        // See if the class is currently loaded
        TMtxLocker lockSync(&m_mtxSync);

        TSrcTab* pwndCur = pwndFindSrcTab(meciCurrent.strClassPath());
        TSrcEditor* pwndEdit = 0;
        if (pwndCur)
            pwndEdit = pwndCur->pwndEditor();

        //
        //  The lines we get here are 1 based ones from the macro engine, so
        //  bring it down one for the check. If the file is loaded, see if
        //  there is an enabled break point on this line.
        //
        if (pwndEdit && pwndEdit->bHasEnabledBP(c4LineNum - 1))
        {
            m_eActionReq = EActionReqs::Break;
        }
         else
        {
            // If there's an active one time break point, then check it
            if ((m_eOneTimeFlag != EOneTimes::None)
            &&  m_strOneTimeClass.bCompareI(meciCurrent.strClassPath()))
            {
                //
                //  If it's a direct hit type break, then make sure that we've
                //  hit the target line. Else, if it's a step type break, make
                //  sure we are in the same method.
                //
                if ((m_eOneTimeFlag == EOneTimes::Hit)
                &&  (c4LineNum == m_c4OneTimeLine))
                {
                    m_eActionReq = EActionReqs::Break;
                }
                 else if ((m_eOneTimeFlag == EOneTimes::Step)
                      &&  (m_c2OneTimeMethod == methCurrent.c2Id()))
                {
                    m_eActionReq = EActionReqs::Break;
                }
            }
        }
    }

    //
    //  If we are either at a break point, or the GUI made some request, then
    //  take the requested action.
    //
    tCIDMacroEng::EDbgActs eRet;
    if (m_eActionReq == EActionReqs::Exit)
    {
        // Make sure any one time break is now turned off
        m_eOneTimeFlag = EOneTimes::None;
        eRet = tCIDMacroEng::EDbgActs::Exit;

        // Clear the action flag now so that we won't re-trigger on the way down
        m_eActionReq = EActionReqs::None;
    }
     else if ((m_eActionReq == EActionReqs::Break)
          ||  (m_eActionReq == EActionReqs::BreakAndGo))
    {
        // Make sure any one time break is now turned off
        m_eOneTimeFlag = EOneTimes::None;

        // Sync while we update the data members, reset the event, and post
        {
            TMtxLocker lockSync(&m_mtxSync);

            // Save the old new values to the last values
            m_c4LastLine    = m_c4NewLine;
            m_pmeciLast     = m_pmeciNew;
            m_pmethiLast    = m_pmethiNew;
            m_pmethLast     = m_pmethNew;
            m_pmecvLast     = m_pmecvNew;

            // And update the new values
            m_c4NewLine     = c4LineNum - 1;
            m_c4NewIP       = c4NewIP;
            m_pmeciNew      = &meciCurrent;
            m_pmethiNew     = &methiCurrent;
            m_pmethNew      = &methCurrent;
            m_pmecvNew      = &mecvInstance;
            m_evSync.Reset();

            //
            //  SendCode is async by default, so these are just posted to
            //  to ourself.
            //
            if (m_eActionReq == EActionReqs::Break)
                SendCode(CIDMacroDbg_DbgMainWnd::i4Broken, 0);
            else
                SendCode(CIDMacroDbg_DbgMainWnd::i4BreakAndGo, 0);
        }

        //
        //  Now block on the event. DO NOT put this in the locked block above
        //  or we will hang.
        //
        m_evSync.WaitFor();
        eRet = tCIDMacroEng::EDbgActs::Continue;
    }
     else if (m_eActionReq == EActionReqs::None)
    {
        eRet = tCIDMacroEng::EDbgActs::Continue;
    }
     else
    {
        // Make the compiler happy
        eRet = tCIDMacroEng::EDbgActs::Continue;

        // <TBD> Something is really awry here!

    }
    return eRet;
}


tCIDLib::TVoid
TMacroDbgMainWnd::FinishedDbg(const tCIDMacroEng::EDbgActs eReason)
{
    // Clear our the info members
    m_c4LastLine    = 0;
    m_c4NewLine     = 0;
    m_pmeciLast     = nullptr;
    m_pmeciNew      = nullptr;
    m_pmethiLast    = nullptr;
    m_pmethLast     = nullptr;
    m_pmethiNew     = nullptr;
    m_pmecvLast     = nullptr;
    m_pmecvNew      = nullptr;

    //
    //  Send (async) a finished message to ourself. This will cause the GUI
    //  thread to know that the run is completed.
    //
    SendCode(CIDMacroDbg_DbgMainWnd::i4Finished, 0);
}


tCIDLib::TVoid TMacroDbgMainWnd::LocalsChange(const tCIDLib::TBoolean)
{
    // We don't really need to use this one
}



// ---------------------------------------------------------------------------
//  TMacroDbgMainWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMacroDbgMainWnd::AddCustomTab(TCMLCustomTab* const pwndToAdopt, const TString& strTabText)
{
    // If there's already a custom tab, we can't take another
    if (m_pwndCustomTab)
    {
        TErrBox msgbErr(L"There is already a custom tab set on the CML debugger");
        msgbErr.ShowIt(*this);
        return;
    }

    // OK, let's try to create the tab and install it
    m_pwndTabsStatus->c4CreateTab(pwndToAdopt, strTabText);

    // It worked, so let's keep it
    m_pwndCustomTab = pwndToAdopt;
}


tCIDLib::TVoid
TMacroDbgMainWnd::AddRemoveBP(  const   TString&            strClassPath
                                , const tCIDLib::TCard4     c4LineNum
                                , const tCIDLib::TBoolean   bState)
{
    TSrcTab* pwndTar = pwndFindSrcTab(strClassPath);
    if (pwndTar)
        pwndTar->pwndEditor()->AddRemoveBP(c4LineNum, bState, kCIDLib::False);
}


// Iterate all the tabs and see if any report changes
tCIDLib::TBoolean TMacroDbgMainWnd::bAnyChanges() const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;

    const tCIDLib::TCard4 c4Count = m_pwndTabsSrc->c4TabCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TSrcTab* pwndCur = pwndSrcTabAt(c4Index);
        bRet = pwndCur->bChanged();
        if (bRet)
            break;
    }
    return bRet;
}


//
//  Called by the containing application when the user asks it to close the current
//  session. We make sure we are stopped and stuff saved away etc...
//
tCIDLib::TBoolean TMacroDbgMainWnd::bCloseSession()
{
    if (m_eCurState != EStates::NoSession)
    {
        //
        //  If we are currently debugging, then we have to get the macro engine
        //  stopped. So if we are running or stopped (which means we are broken
        //  while in a run), we have to get him to stop.
        //
        if ((m_eCurState == EStates::Running)
        ||  (m_eCurState == EStates::Stopped))
        {
            StopSession();
        }

        //
        //  Check to see if we have any files with changes. If so, then ask
        //  if we should change them.
        //
        if (bAnyChanges())
        {
            tCIDCtrls::EMsgBoxRets eRet;
            {
                TYesNoCancelBox msgbSave
                (
                    facCIDMacroDbg().strMsg(kMDbgMsgs::midQ_CloseChanges)
                );
                eRet = msgbSave.eShowIt(*this);
            }

            if (eRet == tCIDCtrls::EMsgBoxRets::Cancel)
                return kCIDLib::False;

            // Save all and fall through
            SaveAll();
        }

        // Close all of the tabs
        m_pwndTabsSrc->CloseAllTabs();

        // Empty all of the output windows
        m_pwndTabBuildRes->Reset();

        // Set the state for no session
        SetState(EStates::NoSession);
    }

    return kCIDLib::True;
}


// Return true if we have the indicated class path open in a source tabl
tCIDLib::TBoolean TMacroDbgMainWnd::bIsOpen(const TString& strClassPath) const
{
    return (pwndFindSrcTab(strClassPath) != nullptr);
}



tCIDLib::TBoolean
TMacroDbgMainWnd::bOpenSession( const   TString&            strClassPath
                                , const tCIDLib::TBoolean   bNewFile)
{
    //
    //  If we have an open session, then close it. If the user backs out,
    //  then we back out.
    //
    if (!bCloseSession())
        return kCIDLib::False;

    // Load up this new file, which will become the session class
    TSrcTab* pwndLoaded;
    if (bNewFile)
        pwndLoaded = pwndLoadNewFileToTab(strClassPath);
    else
        pwndLoaded = pwndLoadFileToTab(strClassPath, tCIDMacroEng::EResModes::ReadWrite);

    //
    //  Set the state for editing mode. Do this before we make this new file
    //  the current file, so that it'll see we are in edit state and set up
    //  the editor correctly.
    //
    SetState(EStates::Editing);

    // And store this as the new session and set the flag
    m_strCurClassPath = strClassPath;

    // Give this guy the focus
    pwndLoaded->pwndEditor()->TakeFocus();

    return kCIDLib::True;
}


// Just pass this on to the viewer window
tCIDLib::TBoolean
TMacroDbgMainWnd::bQuerySessionClasses(tCIDLib::TStrCollect& colToFill)
{
    // If no session is open, reject it
    if (m_eCurState == EStates::NoSession)
    {
        TErrBox msgbErr(L"There is no session currently opened.");
        msgbErr.ShowIt(*this);
        return kCIDLib::False;
    }

    // Iterate the tabs and query each one's name, which is the class path
    const tCIDLib::TCard4 c4Count = m_pwndTabsSrc->c4TabCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TSrcTab* pwndCur = pwndSrcTabAt(c4Index);
        colToFill.objAdd(pwndCur->strName());
    }
    return kCIDLib::True;
}


//
//  Save any outstanding changes. We return the text of the first error reported
//  and don't do any sort of popups. This is for calling from a containing app that
//  wants to report the error. For internally use we use our own private SaveAll()
//  method which calls this version and then displays the error.
//
tCIDLib::TBoolean TMacroDbgMainWnd::bSaveAll(TString& strErrMsg)
{
    //
    //  We iterate the tabs in the source windows tabbed window and ask each
    //  one's editor to save.
    //
    if (m_pwndTabsSrc)
    {
        TString strText;
        const tCIDLib::TCard4 c4Count = m_pwndTabsSrc->c4TabCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TSrcTab* pwndCurTab = pwndSrcTabAt(c4Index);
            if (pwndCurTab->bChanged())
            {
                try
                {
                    pwndCurTab->pwndEditor()->QuerySrcText(strText, kCIDLib::False);
                    StoreClass(pwndCurTab->strClassPath(), strText);
                }

                catch(TError& errToCatch)
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);

                    strErrMsg = errToCatch.strErrText();
                    return kCIDLib::False;
                }
            }
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TMacroDbgMainWnd::bSessionOpen() const
{
    return (m_eCurState != EStates::NoSession);
}

tCIDLib::TBoolean TMacroDbgMainWnd::bSessionOpen(TString& strClassPath) const
{
    const tCIDLib::TBoolean bRet(m_eCurState != EStates::NoSession);
    if (bRet)
        strClassPath = m_strCurClassPath;
    return bRet;
}


tCIDLib::TVoid TMacroDbgMainWnd::CloseAll()
{
    if (m_pwndTabsSrc)
    {
        // First see if there a any changes, skipping the session class
        tCIDLib::TBoolean bChanges = kCIDLib::False;
        tCIDLib::TCard4 c4Count = m_pwndTabsSrc->c4TabCount();
        for (tCIDLib::TCard4 c4Index = 1; c4Index < c4Count; c4Index++)
        {
            TSrcTab* pwndCurTab = pwndSrcTabAt(c4Index);
            bChanges |= pwndCurTab->bChanged();
        }

        if (bChanges)
        {
            TYesNoBox msgbSave(L"There are unsaved changes. Would you like to save them?");
            if (msgbSave.bShowIt(*this))
            {
                TString strErr;
                if (!bSaveAll(strErr))
                {
                    TErrBox msgbErr(strErr);
                    msgbErr.ShowIt(*this);
                    return;
                }
            }
        }

        // Let's do it. We just keep deleting at 1 till count is 1
        while (c4Count > 1)
        {
            m_pwndTabsSrc->CloseTabAt(1);
            c4Count--;
        }
    }
}


// Return the res mode of the currently selected source tab. If none, return read only
tCIDMacroEng::EResModes TMacroDbgMainWnd::eCurSrcMode() const
{
    const TSrcTab* pwndCurTab = pwndCurSrcTab();
    if (pwndCurTab)
        return pwndCurTab->eMode();

    return tCIDMacroEng::EResModes::ReadOnly;
}


tCIDLib::TVoid
TMacroDbgMainWnd::EnableBP( const   TString&            strClassPath
                            , const tCIDLib::TCard4     c4LineNum
                            , const tCIDLib::TBoolean   bState)
{
    TSrcTab* pwndTar = pwndFindSrcTab(strClassPath);
    if (pwndTar)
        pwndTar->pwndEditor()->SetBPEnable(c4LineNum, bState, kCIDLib::False);
}


//
//  This allows the containing application to force us to close an opened tab, such as
//  when the file is deleted via file management.
//
tCIDLib::TVoid TMacroDbgMainWnd::ForceTabClosed(const TString& strClassPath)
{
    TSrcTab* pwndClose = pwndFindSrcTab(strClassPath);
    if (pwndClose)
        m_pwndTabsSrc->CloseTab(strClassPath);
}


//
//  This is primarily to support the break points tab. When they double click a break
//  point, this is called and we go to that. The line is zero based.
//
tCIDLib::TVoid
TMacroDbgMainWnd::Goto(const TString& strClassPath, const tCIDLib::TCard4 c4Line)
{
    TSrcTab* pwndTar = pwndFindSrcTab(strClassPath);
    if (pwndTar)
    {
        //
        //  Activate this tab and go to the indicated line. We don't mark it as the
        //  current line though. We ask it to center it as much as it can
        //
        m_pwndTabsSrc->c4ActivateById(pwndTar->c4TabId());
        pwndTar->pwndEditor()->SetPosition
        (
            c4Line, 0, kCIDLib::False, kCIDLib::True
        );
    }
}


//
//  For the moment we aren't using the debugger state to try to restore internal content,
//  but we may later.
//
tCIDLib::TVoid
TMacroDbgMainWnd::Init( const   TMacroDbgStateInfo& stiToUse
                        , const TCIDUserCtx* const  pcuctxRights)
{
    m_pcuctxRights = pcuctxRights;
}


tCIDLib::TVoid TMacroDbgMainWnd::QueryStateInfo(TMacroDbgStateInfo& stiToFill)
{
}


const TCIDMacroEngine& TMacroDbgMainWnd::meIDE() const
{
    return m_meDebug;
}

TCIDMacroEngine& TMacroDbgMainWnd::meIDE()
{
    return m_meDebug;
}


//
//  This lets the containing application ask us to load up other files after the
//  session is open, such as from a project browser pane and the like. IF we already
//  have the file open, we just move to that tab (and possibly change the edit/read
//  only mode if it's different.) Else we load the new guy.
//
//  In this case, it must be an existing file or this will fail.
//
tCIDLib::TVoid
TMacroDbgMainWnd::OpenNewFile(  const   TString&            strClassPath
                                , const tCIDLib::TBoolean   bEditMode)
{
    TSrcTab* pwndLoaded = pwndLoadFileToTab
    (
        strClassPath
        , bEditMode ? tCIDMacroEng::EResModes::ReadWrite : tCIDMacroEng::EResModes::ReadOnly
    );

    // Load the text into the editor
    m_pwndTabsSrc->c4ActivateById(pwndLoaded->c4TabId());

    //
    //  Send a file event since this isn't going to cause one from the tabbed window.
    //  We can't assume line 1, since this could have already been loaded and we will
    //  just be switching to it.
    //
    SendFileEvent(strClassPath, pwndLoaded->pwndEditor()->c4CurLine());
}


//
//  A simple one, but avoids extra verbiage and provides future flexibility. ANd
//  it allows things like the search dialog to get ot the current source tab.
//
const TSrcTab* TMacroDbgMainWnd::pwndCurSrcTab() const
{
    return static_cast<TSrcTab*>(m_pwndTabsSrc->pwndSelected());
}

TSrcTab* TMacroDbgMainWnd::pwndCurSrcTab()
{
    return static_cast<TSrcTab*>(m_pwndTabsSrc->pwndSelected());
}


tCIDLib::TVoid TMacroDbgMainWnd::RemoveAllBPs()
{
    tCIDLib::TCard4 c4Count = m_pwndTabsSrc->c4TabCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TSrcTab* pwndCurTab = pwndSrcTabAt(c4Index);
        pwndCurTab->pwndEditor()->RemoveAllBPs();
    }
}


tCIDLib::TVoid TMacroDbgMainWnd::RemoveAllSrcBPs(const TString& strClassPath)
{
    TSrcTab* pwndTar = pwndFindSrcTab(strClassPath);
    if (pwndTar)
        pwndTar->pwndEditor()->RemoveAllBPs();
}


// The class path of the main class we are editing
const TString& TMacroDbgMainWnd::strCurClassPath() const
{
    return m_strCurClassPath;
}


// Allows the containing app to stop the session if needed, mostly for shutdown
tCIDLib::TVoid TMacroDbgMainWnd::StopSession()
{
    // Just fake a call to our menu command handler
    MenuCommand(kCIDMacroDbg::ridMenu_Debug_Stop, kCIDLib::False, kCIDLib::False);

    //
    //  And wait for the thread to end. We can't just block due to the cooperative multi-
    //  tasking nature of the CML engine. So we pump messages while we wait for it to
    //  end.
    //
    if (m_thrRun.bIsRunning())
    {
        tCIDLib::TEncodedTime enctStart = TTime::enctNow();
        while (m_thrRun.bIsRunning())
        {
            facCIDCtrls().MsgYield(100);

            if ((TTime::enctNow() - enctStart) > kCIDLib::enctOneSecond * 10)
            {
                TErrBox msgbErr(L"The background thread running the macro could not be stopped");
                msgbErr.ShowIt(*this);
                break;
            }
        }
    }
}


// ---------------------------------------------------------------------------
//  TMacroDbgMainWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  From the macro engine's class manager interface, which we implement and plug
//  into the macro engine.
//
tCIDLib::TBoolean TMacroDbgMainWnd::bCheckIfExists(const TString& strToCheck)
{
    // Delegate to the client code's manager to do the actual work
    return m_pmecmToUse->bMacroExists(strToCheck);
}


//
//  Called from the pane window container (parent) class to let us set up the pane
//  window manager for our desired layout.
//
tCIDLib::TBoolean TMacroDbgMainWnd::bCreated()
{
    if (!TParent::bCreated())
        return kCIDLib::False;

    // Create our context menu
    m_menuContext.Create(facCIDMacroDbg(), kCIDMacroDbg::ridMenu_Main);

    // Initially check some options items to match the initial settings
    m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_DecRadix, kCIDLib::True);
    m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_Unhandled, kCIDLib::True);

    // Set a 2 horizontal pane layout controller
    SetLayout(new TPaneLayout2Horz);

    // Create our top pane, the source tabs and add it to the manager.
    m_pwndTabsSrc = new TTabbedWnd();
    m_pwndTabsSrc->CreateTabbedWnd
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , TArea(0, 0, 128, 128)
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::ETabWStyles::None
    );
    m_pwndTabsSrc->SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window), kCIDLib::True);

    m_pwndTabsSrc->pnothRegisterHandler(this, &TMacroDbgMainWnd::eSrcTabHandler);
    SetPane(m_pwndTabsSrc, 0, TSize(0, 128), TSize(0, 128));


    // Create our bottom pane, the output tabs and add it to the manager
    m_pwndTabsStatus = new TTabbedWnd();
    m_pwndTabsStatus->CreateTabbedWnd
    (
        *this
        , kCIDCtrls::widFirstCtrl + 1
        , TArea(0, 0, 128, 128)
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::ETabWStyles::None
    );
    m_pwndTabsStatus->SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window), kCIDLib::True);
    SetPane(m_pwndTabsStatus, 1, TSize(0, 128), TSize(0, 128));

    //
    //  Set our bgn color to the standard edit window bgn. Since we set a margin on
    //  our panes, this will show through and provide a nice contrast border.
    //
    SetBgnColor
    (
        facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::EditWndBgn), kCIDLib::True
    );

    //
    //  Create the output tabs, first the build results, which we want to be the
    //  first one, left-most.
    //
    {
        m_pwndTabBuildRes = new TBuildResTab();
        m_pwndTabBuildRes->CreateBldResTab(*m_pwndTabsStatus);

        // Register for error position notifications from this guy
        m_pwndTabBuildRes->pnothRegisterHandler(this, &TMacroDbgMainWnd::ePrsErrHandler);
    }

    // Console output
    m_pwndTabConOut = new TTextOutTab(kCIDMacroDbg_::pszTab_MacroOutput);
    m_pwndTabConOut->CreateTOTab
    (
        *m_pwndTabsStatus, facCIDMacroDbg().strMsg(kMDbgMsgs::midTab_MacroOut)
    );

    // Class members list
    m_pwndTabMembers = new TObjListTab
    (
        kCIDMacroDbg_::pszTab_DisplayMembers, TObjListTab::EModes::Members
    );
    m_pwndTabMembers->CreateObjListTab
    (
        *m_pwndTabsStatus, facCIDMacroDbg().strMsg(kMDbgMsgs::midTab_Members), &m_meDebug
    );

    // Method locals list
    m_pwndTabLocals = new TObjListTab
    (
        kCIDMacroDbg_::pszTab_DisplayLocals, TObjListTab::EModes::Locals
    );
    m_pwndTabLocals->CreateObjListTab
    (
        *m_pwndTabsStatus, facCIDMacroDbg().strMsg(kMDbgMsgs::midTab_Locals), &m_meDebug
    );

    // Method parameters list
    m_pwndTabParms = new TObjListTab
    (
        kCIDMacroDbg_::pszTab_DisplayParms, TObjListTab::EModes::Parms
    );
    m_pwndTabParms->CreateObjListTab
    (
        *m_pwndTabsStatus, facCIDMacroDbg().strMsg(kMDbgMsgs::midTab_Parms), &m_meDebug
    );

    // The search and replace tab
    m_pwndTabSearch = new TSearchTab();
    m_pwndTabSearch->CreateSearchTab(*m_pwndTabsStatus, this);

    // The break points tab
    m_pwndTabBreakPnts = new TBreakPntsTab();
    m_pwndTabBreakPnts->CreateBreakPntsTab(*m_pwndTabsStatus, this);


    //
    //  Set ourself as the debug output handler for the macro engine. We implement
    //  his debug interface mixin.
    //
    m_meDebug.SetDbgHandler(this);

    // Point the engine at our console output stream
    m_meDebug.SetConsole(&m_strmConStream);

    // Set any file resolver we were given
    m_meDebug.SetFileResolver(m_pmefrToUse);

    // Let the engine know it's running in the IDE. It'll do some things differently
    m_meDebug.bInIDE(kCIDLib::True);

    //
    //  Set up our accelerator table. We have to be careful here not to override anything
    //  that our contained windows needs to see (as regular keyboard input), else we will
    //  eat it before it ever gets to them.
    //
    tCIDLib::TStrList colKeyStrs;
    tCIDLib::TCardList fcolCmds;

    // Do this one twice so as to cover both minus keys
    colKeyStrs.objAdd(L"Ctrl--");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Class_Close);
    colKeyStrs.objAdd(L"Ctrl-Minus");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Class_Close);

    colKeyStrs.objAdd(L"Ctrl-L");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Edit_SelectLine);
    colKeyStrs.objAdd(L"Ctrl-A");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Edit_SelectAll);

    // Open a new file (once the session is open) for viewing or read only
    colKeyStrs.objAdd(L"Ctrl-O");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Class_Edit);
    colKeyStrs.objAdd(L"Ctrl-R");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Class_View);

    // Save the current source tab contents, or all tabs
    colKeyStrs.objAdd(L"Ctrl-S");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Class_Save);
    colKeyStrs.objAdd(L"Ctrl-W");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Class_SaveAll);

    // Start/restart or stop a debugging session
    colKeyStrs.objAdd(L"Ctrl-Alt-G");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Debug_Go);
    colKeyStrs.objAdd(L"Ctrl-Alt-T");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Debug_Stop);

    // Do a compile without running the session
    colKeyStrs.objAdd(L"F7");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Debug_Compile);

    // Search and replace
    colKeyStrs.objAdd(L"F3");
    fcolCmds.c4AddElement(CIDMacroDbg_DbgMainWnd::c4FakeMenu_FindNext);
    colKeyStrs.objAdd(L"F5");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Edit_Search);
    colKeyStrs.objAdd(L"Ctrl-F");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Edit_Search);
    colKeyStrs.objAdd(L"Ctrl-H");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Edit_Replace);

    // Debugging commands
    colKeyStrs.objAdd(L"F9");
    fcolCmds.c4AddElement(CIDMacroDbg_DbgMainWnd::c4FakeMenu_ToggleBPoint);
    colKeyStrs.objAdd(L"F8");
    fcolCmds.c4AddElement(CIDMacroDbg_DbgMainWnd::c4FakeMenu_ToggleBPEnable);

    colKeyStrs.objAdd(L"F10");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Debug_StepOver);
    colKeyStrs.objAdd(L"F11");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Debug_StepInto);


    // General stuff
    colKeyStrs.objAdd(L"Ctrl-G");
    fcolCmds.c4AddElement(kCIDMacroDbg::ridMenu_Edit_GotoLine);

    // Now we can create the table
    m_accelDbg.Create(colKeyStrs, fcolCmds);

    // Initialize the panes, giving the top one the most initial size
    InitPanes(kCIDLib::False);

    // Set an initial state of no session
    SetState(EStates::NoSession);

    //
    //  Create the timer that will do the macro output processing. We create it
    //  initially stopped.
    //
    m_tmidMacroOut = tmidStartTimer(500, kCIDLib::True);

    return kCIDLib::True;
}


//
//  We post ourself some async msgs, to avoid various complications by letting
//  the stack unwind from user input before we do something.
//
tCIDLib::TVoid
TMacroDbgMainWnd::CodeReceived( const   tCIDLib::TInt4  i4Code
                                , const tCIDLib::TCard4 c4Data)
{
    switch(i4Code)
    {
        case CIDMacroDbg_DbgMainWnd::i4Broken :
        case CIDMacroDbg_DbgMainWnd::i4BreakAndGo :
        {
            //
            //  Find the the class that we stopped in, loading it if needed,
            //  and move to the line. Then set the new line state. Tell it also
            //  to make this the current line.
            //
            if (i4Code ==  CIDMacroDbg_DbgMainWnd::i4Broken)
                SetCurEditPos(m_pmeciNew->strClassPath(), m_c4NewLine, 0, kCIDLib::True);
            else
                SetCurEditPos(m_strGoToClass, m_c4GoToLine, 0, kCIDLib::True);

            // Set our state to stopped state, and clear the req flag
            SetState(EStates::Stopped);
            m_eActionReq = EActionReqs::None;

            // Store the last stack changes number we got
            m_c4LastStackUpdate = m_c4StackChanges;
            break;
        }

        case CIDMacroDbg_DbgMainWnd::i4EndSession :
        case CIDMacroDbg_DbgMainWnd::i4Finished :
        {
            //
            //  Disable the macro output timer now. There isn't anything for
            //  it to do when we aren't running.
            //
            PauseTimer(m_tmidMacroOut, kCIDLib::True);

            //
            //  If we ended due to an end session, then we don't really have
            //  to do much other than set the state. Else, we are still in
            //  a session and have to get ourself back to editing state.
            //
            if (i4Code == CIDMacroDbg_DbgMainWnd::i4EndSession)
            {
                SetState(EStates::NoSession);
            }
             else
            {
                SetState(EStates::Editing);

                //
                //  Get any last bit of content out of the macro console output
                //  since we could have some left still.
                //
                tCIDLib::TCard4 c4Index = 0;
                if (m_strmConStream.bQueryNewMsgs(c4Index, m_colTmpStrs, kCIDLib::False))
                    m_pwndTabConOut->AddLines(m_colTmpStrs);

                // And put out a message on the results window indicating it's done
                m_pwndTabBuildRes->ParseEvent
                (
                    tCIDMacroEng::EPrsEvs::InfoMsg
                    , facCIDMacroDbg().strMsg(kMDbgMsgs::midMsg_RunComplete)
                    , 0
                    , 0
                    , m_pmeciMain->strClassPath()
                );
            }

            // And finally reset the macro engine
            m_meDebug.Reset();
            break;
        }

        case CIDMacroDbg_DbgMainWnd::i4ErrLocGo :
        {
            // Move the focus to to the editor of the indicated tab
            TSrcTab* pwndTab = pwndSrcTabAt(c4Data);
            if (pwndTab)
                pwndTab->pwndEditor()->TakeFocus();
            break;
        }

        default :
            break;
    };
}


tCIDLib::TVoid TMacroDbgMainWnd::Destroyed()
{
    // If we are not in idle state, issue an exit command to stop it
    if ((m_eCurState == EStates::Running) || (m_eCurState == EStates::Stopped))
    {
        StopSession();
    }

    //
    //  Ask the macro engine to clean up. Do a full cleanup here since we
    //  are going away.
    //
    try
    {
        m_meDebug.Cleanup(kCIDLib::True);
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    // Clear the console window on the macro engine
    m_meDebug.SetConsole(nullptr);

    //
    //  Destroy the macro output timer. Do this after the stopping of the macro
    //  engine.
    //
    StopTimer(m_tmidMacroOut);

    // And pass it on to our parent to do any remaining cleanup
    TParent::Destroyed();
}


//
//  This is a callback via the class manager mixin we implement. So basically, we are
//  indirectly getting here when we call StoreClass() on ourself or if the engine should
//  call us. So we will use this to mark the editor for this class path (if there is one
//  open) as having no outstanding changes. This way we don't have to do it all over the
//  place. The save will always go through here.
//
//  There may not be an editor open yet, since this is also called to store the default
//  initial contents when a new file is created.
//
tCIDLib::TVoid
TMacroDbgMainWnd::DoStore(const TString& strClassPath, const TString& strText)
{
    // Delegate to the client code's manager to do the actual store
    m_pmecmToUse->StoreClass(strClassPath, strText);

    TSrcTab* pwndSrcTab = pwndFindSrcTab(strClassPath);
    if (pwndSrcTab)
        pwndSrcTab->pwndEditor()->ChangesSaved();
}


tCIDLib::TVoid TMacroDbgMainWnd::DoUndoWriteMode(const TString& strClassPath)
{
    //
    //  Just delegate to the client code's manager for this one, but it should
    //  never really be called, since we handle this kind of stuff directly
    //  through the client's code manager.
    //
    return m_pmecmToUse->UndoWriteMode(strClassPath);
}


tCIDLib::EOpenRes
TMacroDbgMainWnd::eDoSelect(TString& strToFill, const tCIDMacroEng::EResModes eMode)
{
    // Just delegate to the client code's class manager for this one
    return m_pmecmToUse->eSelectClass(strToFill, eMode);
}


//
//  We have to handle context menus from either a source tab or one of the status
//  tabs.
//
tCIDCtrls::ECtxMenuOpts
TMacroDbgMainWnd::eShowContextMenu(const TPoint& pntAt, const tCIDCtrls::TWndId)
{
    //
    //  See if the click falls within the source editor pane. If not, then we
    //  don't do anything. If the point is -1,-1, then is'a hot key driven one
    //  so make sure that the focus is contained within the tabs pane.
    //
    if (pntAt == TPoint(-1, -1))
    {
        if (!m_pwndTabsSrc->bContainsFocus())
            return tCIDCtrls::ECtxMenuOpts::Done;
    }
     else
    {
        TPoint pntRel;
        m_pwndTabsSrc->FromScreenCoordinates(pntAt, pntRel);
        if (!m_pwndTabsSrc->areaWndSize().bContainsPoint(pntRel))
            return tCIDCtrls::ECtxMenuOpts::Done;
    }

    // Get the current tab. If none, we are done
    const TSrcTab* pwndCurSrc = pwndCurSrcTab();
    if (!pwndCurSrc)
        return tCIDCtrls::ECtxMenuOpts::Done;

    //
    //  Based on the read only state and whether it has any selection, enable
    //  some cut, copy, and paste stuff.
    //
    m_menuContext.SetItemEnable
    (
        kCIDMacroDbg::ridMenu_Edit_Paste, !pwndCurSrc->pwndEditor()->bReadOnly()
    );

    m_menuContext.SetItemEnable
    (
        kCIDMacroDbg::ridMenu_Edit_Copy, pwndCurSrc->pwndEditor()->bHasSelection()
    );

    m_menuContext.SetItemEnable
    (
        kCIDMacroDbg::ridMenu_Edit_Cut
        , pwndCurSrc->pwndEditor()->bHasSelection()
          && !pwndCurSrc->pwndEditor()->bReadOnly()
    );

    m_menuContext.SetItemEnable
    (
        kCIDMacroDbg::ridMenu_Edit_Redo, pwndCurSrc->pwndEditor()->bCanRedo()
    );

    m_menuContext.SetItemEnable
    (
        kCIDMacroDbg::ridMenu_Edit_Undo, pwndCurSrc->pwndEditor()->bCanUndo()
    );


    //
    //  If the point is -1,-1, we just put it in our center. Else we take it as
    //  is. It's in screen coordinates already.
    //
    TPoint pntReal(pntAt);
    if (pntReal == TPoint(-1, -1))
        ToScreenCoordinates(areaWnd().pntCenter(), pntReal);

    const tCIDLib::TCard4 c4Res = m_menuContext.c4Process
    (
        *this
        , pntReal
        , tCIDLib::EVJustify::Bottom
    );

    if (c4Res)
        MenuCommand(c4Res, kCIDLib::False, kCIDLib::True);

    // Indicate we handled it
    return tCIDCtrls::ECtxMenuOpts::Done;
}


//
//  Do our poup menu and accellerator drive commands. Note that, though we disable
//  menu commands based on state, we still have to check for the appropriate state
//  before we allow things to be done, because we can also get here via accelerator
//  keys.
//
tCIDLib::TVoid
TMacroDbgMainWnd::MenuCommand(  const   tCIDLib::TResId     ridItem
                                , const tCIDLib::TBoolean   bChecked
                                , const tCIDLib::TBoolean   bEnabled)
{
    //
    //  These are commands that come from or match our popup menu
    //
    if (ridItem == kCIDMacroDbg::ridMenu_Class_Close)
    {
        //
        //  Get the current tab. If we have one, and it's not the session class
        //  tab, then we can close it.
        //
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab && !pwndCurTab->strName().bCompareI(m_strCurClassPath))
        {
            // See if there are changes and ask to save them if so
            if (pwndCurTab->bChanged())
            {
                TYesNoBox msgbSave(facCIDMacroDbg().strMsg(kMDbgMsgs::midQ_SaveChanges));
                if (msgbSave.bShowIt(*this))
                {
                    // Save this one
                    TString strText;
                    pwndCurTab->pwndEditor()->QuerySrcText(strText, kCIDLib::False);
                    StoreClass(pwndCurTab->strClassPath(), strText);
                }
            }

            //
            //  And let's close the tab. Save the class path first, since it's
            //  in the thing we are destroying.
            //
            TString strClosePath = pwndCurTab->strName();
            m_pwndTabsSrc->CloseTab(strClosePath);
        }
    }
     else if ((ridItem == kCIDMacroDbg::ridMenu_Class_Edit)
          ||  (ridItem == kCIDMacroDbg::ridMenu_Class_View))
    {
        // Only valid when a session is open and not currenting running
        if ((m_eCurState == EStates::Editing) || (m_eCurState == EStates::Stopped))
            OpenMacro(ridItem == kCIDMacroDbg::ridMenu_Class_Edit);
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Class_Export)
    {
        //
        //  Just let them save it to any arbitrary local file. We'll give them
        //  an initial name based on the class name. Generally they'll just
        //  select the target directory. Only valid in editing mode.
        //
        if (m_eCurState == EStates::Editing)
            ExportClass();
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Class_Save)
    {
        // If there's a current tab, and it has changes, save them
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab && pwndCurTab->bChanged())
        {
            TString strText;
            pwndCurTab->pwndEditor()->QuerySrcText(strText, kCIDLib::False);
            StoreClass(pwndCurTab->strClassPath(), strText);
        }
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Class_SaveAll)
    {
        SaveAll();
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Debug_Break)
    {
        //
        //  Stop the macro thread, set the request, and let it go again. Only
        //  valid if running.
        //
        if (m_eCurState == EStates::Running)
        {
            TMtxLocker mtxSync(&m_mtxSync);
            m_eActionReq = EActionReqs::Break;
        }
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Debug_Compile)
    {
        if (m_eCurState == EStates::Running)
            TAudio::Beep(660, 100);
        else
            GoOrRestart(EGoActions::Compile);
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Debug_Go)
    {
        // If not debugging yet, or we are but are stopped
        if ((m_eCurState == EStates::Editing) || (m_eCurState == EStates::Stopped))
            GoOrRestart(EGoActions::Go);
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Debug_StepInto)
    {
        // The parm tells it to do a single step go, only valid if stopped
        if (m_eCurState == EStates::Stopped)
            GoOrRestart(EGoActions::StepInto);
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Debug_StepOver)
    {
        // The parm tells it to do a single step go, only valid if stopped
        if (m_eCurState == EStates::Stopped)
            GoOrRestart(EGoActions::StepOver);
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Debug_Stop)
    {
        //
        //  Stop the macro thread, set the request, and let it go again. Only
        //  valid if running.
        //
        if ((m_eCurState == EStates::Running) || (m_eCurState == EStates::Stopped))
        {
            TMtxLocker mtxSync(&m_mtxSync);
            m_eActionReq = EActionReqs::Exit;
            if (m_eCurState == EStates::Stopped)
                m_evSync.Trigger();
        }
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_Copy)
    {
        // Ask the editor to copy the selection to the clipboard
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab &&  pwndCurTab->pwndEditor()->bHasSelection())
        {
            pwndCurTab->pwndEditor()->CopySelection(kCIDLib::False);
        }
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_Cut)
    {
        // Ask the editor to cut the selection to the clipboard
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab && !pwndCurTab->pwndEditor()->bReadOnly())
        {
            pwndCurTab->pwndEditor()->CopySelection(kCIDLib::True);
        }
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_GotoLine)
    {
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab)
        {
            TString strVal;
            tCIDLib::TBoolean bRes = facCIDWUtils().bListSelect2
            (
                *this
                , L"Go To Line"
                , pwndCurTab->colGotoList()
                , strVal
                , tCIDWUtils::EListValFmts::NZCardinal
            );

            if (bRes)
            {
                // Don't let the list get crazy
                if (pwndCurTab->colGotoList().c4ElemCount() >= 32)
                    pwndCurTab->colGotoList().bRemoveLast();

                tCIDLib::TCard4 c4Line;
                if (strVal.bToCard4(c4Line, tCIDLib::ERadices::Auto))
                {
                    CIDAssert(c4Line != 0, L"The line number should not be zero");
                    pwndCurTab->GoToLine(c4Line - 1);
                }
            }
        }
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_Paste)
    {
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab)
            pwndCurTab->pwndEditor()->Paste();
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_Redo)
    {
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        pwndCurTab->pwndEditor()->Redo();
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_SelectAll)
    {
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab)
            pwndCurTab->pwndEditor()->SelectAll();
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_SelectLine)
    {
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        if (pwndCurTab)
            pwndCurTab->pwndEditor()->SelectCurLine();
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Edit_Undo)
    {
        TSrcTab* pwndCurTab = pwndCurSrcTab();
        pwndCurTab->pwndEditor()->Undo();
    }
     else if ((ridItem == kCIDMacroDbg::ridMenu_Opt_AtThrow)
          ||  (ridItem == kCIDMacroDbg::ridMenu_Opt_Unhandled))
    {
        if (ridItem == kCIDMacroDbg::ridMenu_Opt_AtThrow)
        {
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_Unhandled, kCIDLib::False);
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_AtThrow, kCIDLib::True);
            m_meDebug.eExceptReport(tCIDMacroEng::EExceptReps::AtThrow);
        }
         else
        {
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_Unhandled, kCIDLib::True);
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_AtThrow, kCIDLib::False);
            m_meDebug.eExceptReport(tCIDMacroEng::EExceptReps::NotHandled);
        }
    }
     else if ((ridItem == kCIDMacroDbg::ridMenu_Opt_DecRadix)
          ||  (ridItem == kCIDMacroDbg::ridMenu_Opt_HexRadix))
    {
        tCIDLib::ERadices eToSet;
        if (ridItem == kCIDMacroDbg::ridMenu_Opt_DecRadix)
        {
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_DecRadix, kCIDLib::True);
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_HexRadix, kCIDLib::False);
            eToSet = tCIDLib::ERadices::Dec;
        }
         else
        {
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_DecRadix, kCIDLib::False);
            m_menuContext.SetItemCheck(kCIDMacroDbg::ridMenu_Opt_HexRadix, kCIDLib::True);
            eToSet = tCIDLib::ERadices::Hex;
        }

        // Tell the object display windows about the new radix
        SetObjWndRadix(eToSet);
    }
     else if (ridItem == kCIDMacroDbg::ridMenu_Opt_DebugMode)
    {
        // Flip the mode
        if (m_meDebug.bDebugMode())
            m_meDebug.bDebugMode(kCIDLib::False);
        else
            m_meDebug.bDebugMode(kCIDLib::True);

        m_menuContext.SetItemCheck
        (
            kCIDMacroDbg::ridMenu_Opt_DebugMode, m_meDebug.bDebugMode()
        );
    }
     else if ((ridItem == kCIDMacroDbg::ridMenu_Edit_Search)
          ||  (ridItem == kCIDMacroDbg::ridMenu_Edit_Replace))
    {
        // Bring forward the search/replace tab
        m_pwndTabsStatus->c4ActivateById(m_pwndTabSearch->c4TabId());
        m_pwndTabSearch->Find();
    }
     else
    {
        //
        //  We also handle some fake ones that aren't really in the menu but added to
        //  the accellerator table.
        //
        if (ridItem == CIDMacroDbg_DbgMainWnd::c4FakeMenu_ToggleBPoint)
        {
            TSrcTab* pwndCurTab = pwndCurSrcTab();
            if (pwndCurTab)
                pwndCurTab->pwndEditor()->ToggleBPoint();
        }
         else if (ridItem == CIDMacroDbg_DbgMainWnd::c4FakeMenu_ToggleBPEnable)
        {
            TSrcTab* pwndCurTab = pwndCurSrcTab();
            if (pwndCurTab)
                pwndCurTab->pwndEditor()->ToggleBPEnable();
        }
         else if (ridItem == CIDMacroDbg_DbgMainWnd::c4FakeMenu_FindNext)
        {
            m_pwndTabSearch->FindNext();
        }
    }
}


TTextInStream*
TMacroDbgMainWnd::pstrmDoLoad(  const   TString&             strClassPath
                                , const tCIDMacroEng::EResModes eMode)
{
    TString strTmp;

    //
    //  If it's already in our source tab list, let's provide the stream ourself.
    //
    //  NOTE:   This will only get called for compiles, so it will only ever request
    //          read access, so we can always provide access to our existing info.
    //
    // The default is not to adopt, so this is fine for us
    //
    TSrcTab* pwndSrc = reinterpret_cast<TSrcTab*>(m_pwndTabsSrc->pwndFindByName(strClassPath));
    if (pwndSrc)
        return new TTextStringInStream(&pwndSrc->pwndEditor()->strWndText());

    // Not in the cache, so go to the caller's class manaager
    return m_pmecmToUse->pstrmLoadClass(strClassPath, eMode);
}


tCIDLib::TVoid TMacroDbgMainWnd::Timer(const tCIDCtrls::TTimerId tmidSrc)
{
    if (tmidSrc == m_tmidMacroOut)
    {
        //
        //  If there is any output in the macro console stream, yank it out. We
        //  use destructive read mode, so the index we pass in doesn't make any
        //  difference. Tell it to add new lines to the lines, to make it simpler
        //  for us to just dump to the output window.
        //
        tCIDLib::TCard4 c4Index = 0;
        if (m_strmConStream.bQueryNewMsgs(c4Index, m_colTmpStrs, kCIDLib::False))
            m_pwndTabConOut->AddLines(m_colTmpStrs);
    }
}


// ---------------------------------------------------------------------------
//  TMacroDbgMainWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We register for error position notifiations from the build result tab. When the
//  user double clicks, we get a notification. We want to bring the referenced src
//  tab forward (or load it) and move to the indicated position.
//
tCIDCtrls::EEvResponses TMacroDbgMainWnd::ePrsErrHandler(TPrsErrPosInfo& wnotEvent)
{
    //
    //  Move our editor window to the indicated position. The line numbers it
    //  has are from the parser, which uses human readable 1 based numbers, so
    //  we have to subtract one for editor positions.
    //
    //  In this case, tell it not to set the current line marker.
    //
    SetCurEditPos
    (
        wnotEvent.strClassPath()
        , wnotEvent.c4Line() - 1
        , wnotEvent.c4Col() - 1
        , kCIDLib::False
    );

    return tCIDCtrls::EEvResponses::Handled;
}


//
//  We register for notifications from each source editor we load. We use this
//  currently to maintain our breakpoint tab's list of break points. The source
//  tab handler below also is part of that.
//
tCIDCtrls::EEvResponses TMacroDbgMainWnd::eSrcEdHandler(TSrcEdChangeInfo & wnotEvent)
{
    // Just in case, since there might be some race conditions
    if (!m_pwndTabBreakPnts)
        return tCIDCtrls::EEvResponses::Handled;

    // Find the tab for this guy
    TSrcTab* pwndSrc = pwndFindSrcTab(wnotEvent.strPath());
    if (!pwndSrc)
    {
        TOkBox msgbNotFnd
        (
            L"CML IDE"
            , L"Could not find the tab for the indicated source path"
        );
        msgbNotFnd.ShowIt(*this);
        return tCIDCtrls::EEvResponses::Handled;
    }

    if (wnotEvent.eEvent() == tCIDWUtils::ESEdEvents::AddBP)
    {
        m_pwndTabBreakPnts->AddBreakPoint
        (
            *pwndSrc, wnotEvent.c4LineNum(), kCIDLib::True
        );
    }
     else if (wnotEvent.eEvent() == tCIDWUtils::ESEdEvents::DisableBP)
    {
        m_pwndTabBreakPnts->EnableBreakPoint
        (
            *pwndSrc, wnotEvent.c4LineNum(), kCIDLib::False
        );
    }
     else if (wnotEvent.eEvent() == tCIDWUtils::ESEdEvents::EnableBP)
    {
        m_pwndTabBreakPnts->EnableBreakPoint
        (
            *pwndSrc, wnotEvent.c4LineNum(), kCIDLib::True
        );
    }
     else if (wnotEvent.eEvent() == tCIDWUtils::ESEdEvents::Modified)
    {
        //
        //  If it's updated in a way that affects how many lines, then we have
        //  to deal with BPs possibly being deleted or moved, so update them
        //  all for this source tab.
        //
        if (wnotEvent.i4LineChanges() != 0)
            m_pwndTabBreakPnts->UpdateBreakPoints(*pwndSrc);
    }
     else if (wnotEvent.eEvent() == tCIDWUtils::ESEdEvents::RemoveBP)
    {
        m_pwndTabBreakPnts->RemoveBreakPoint(*pwndSrc, wnotEvent.c4LineNum());
    }
     else if (wnotEvent.eEvent() == tCIDWUtils::ESEdEvents::LineNum)
    {
        // We turn around and send a line event to any of our listeners
        SendLineEvent(wnotEvent.c4LineNum());
    }
    return tCIDCtrls::EEvResponses::Handled;
}


//
//  We register for notifications from the tabs in the editing area tabbed window.
//  If we get a close event we will close the tab down, after checking to see if there
//  are changes. If any changes, we ask the user if they want to save or close without
//  saving or cancel.
//
tCIDCtrls::EEvResponses TMacroDbgMainWnd::eSrcTabHandler(TTabEventInfo& wnotEdit)
{
    if (wnotEdit.eEvent() == tCIDCtrls::ETabWEvents::Close)
    {
        // If it's the session class tab, we can't close it
        if (wnotEdit.c4TabId() == m_c4TabId_Session)
        {
            // Get the tab, cast to our base edit tab class
            TSrcTab* pwndTab = static_cast<TSrcTab*>
            (
                m_pwndTabsSrc->pwndFindById(wnotEdit.c4TabId())
            );
            CIDAssert(pwndTab != nullptr, L"The reported tab was not found");

            // If any changes, ask if they want to save, close, both, or cancel it
            if (pwndTab->bChanged())
            {

                // If not, then do nothing
            }

            //
            //  Tell the break points tab to remove all break points for this class.
            //  Passing max card for the line indicates remove all.
            //
            m_pwndTabBreakPnts->RemoveBreakPoint(*pwndTab, kCIDLib::c4MaxCard);

            // Close the tab
            m_pwndTabsSrc->CloseTabById(wnotEdit.c4TabId());
        }
    }
     else if (wnotEdit.eEvent() == tCIDCtrls::ETabWEvents::PreChange)
    {
        // The one losing needs to have any current line indicator removed
        TSrcTab* pwndTab = static_cast<TSrcTab*>
        (
            m_pwndTabsSrc->pwndFindById(wnotEdit.c4TabId())
        );
        CIDAssert(pwndTab != nullptr, L"The reported tab was not found");
        pwndTab->pwndEditor()->RemoveCurLineState();
    }

    //
    //  If it was a close or change, send a file event. We may have no tabs anymore, or we
    //  may have changed to a new one (either by the user selecting it, or because the
    //  previously current one was closed.)
    //
    if ((wnotEdit.eEvent() == tCIDCtrls::ETabWEvents::Close)
    ||  (wnotEdit.eEvent() == tCIDCtrls::ETabWEvents::Change))
    {
        const TSrcTab* pwndCurSrc = pwndCurSrcTab();
        if (pwndCurSrc)
            SendFileEvent(pwndCurSrc->strClassPath(), pwndCurSrc->pwndEditor()->c4CurLine());
        else
            SendFileEvent(TString::strEmpty(), 1);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


//
//  Our macro invocation thread is started up on this method.
//
tCIDLib::EExitCodes TMacroDbgMainWnd::eRunThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the parent thread go
    thrThis.Sync();

    //
    //  Just run the macro by invoking the macro engine. Put a try/catch
    //  block around and catch anything that comes out. The engine will
    //  call us back since we've installed ourself as it's debug handler.
    //
    try
    {
        TMEngClassVal* pmecvTarget = m_pmeciMain->pmecvMakeStorage
        (
            L"$Main$", m_meDebug, tCIDMacroEng::EConstTypes::NonConst
        );
        TJanitor<TMEngClassVal> janTarget(pmecvTarget);

        // Call it's default constructor
        if (m_meDebug.bInvokeDefCtor(*pmecvTarget, m_pcuctxRights))
            m_meDebug.i4Run(*pmecvTarget, m_strCmdLineParms, m_pcuctxRights);
    }

    catch(const TError& errToCatch)
    {
        if (facCIDMacroDbg().bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        if (facCIDMacroDbg().bLogFailures())
        {
            facCIDMacroDbg().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kMDbgErrs::errcGen_EngineException
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
            );
        }
    }

    catch(const TExceptException&)
    {
        //
        //  Some CML level exception was thrown and not caught. It was already
        //  reported to the user because we (the IDE) displayed it to him. So
        //  we just eat it.
        //
    }

    catch(...)
    {
        if (facCIDMacroDbg().bLogFailures())
        {
            facCIDMacroDbg().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kMDbgErrs::errcGen_EngineException
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
            );
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


//
//  Called when the user asks to export. We get the current source tab, grab its
//  text, and let the user select a target file to save to.
//
tCIDLib::TVoid TMacroDbgMainWnd::ExportClass()
{
    // Get the current source tab. If none do nothing
    TSrcTab* pwndCur = pwndCurSrcTab();
    if (!pwndCur)
        return;

    // Get the class path of this guy and keep just the name part
    TString strName;
    TString strPath;
    facCIDMacroEng().SplitClassPath
    (
        pwndCur->strClassPath(), strPath, strName
    );

    // Get the text of this guy's editor
    TString strToSave;
    pwndCur->pwndEditor()->QuerySrcText(strToSave, kCIDLib::False);

    // Set up the file types list
    tCIDLib::TKVPList colTypes;
    colTypes.objAdd(TKeyValuePair(L"CML Macro Text", L"Txt"));

    // Set up an initial name
    TPathStr pathName(strName);
    pathName.AppendExt(L"Txt");

    // Invoke the file save dialog to get the user's choice
    TString strTarPath(strName);
    strTarPath.Append(L".Txt");
    const tCIDLib::TBoolean bRes = facCIDCtrls().bSaveFileDlg
    (
        *this
        , L"Select Export Directory"
        , TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::UserAppData)
        , pathName
        , strTarPath
        , colTypes
        , tCIDCtrls::EFSaveOpts::ConfirmPrompt
    );

    if (bRes)
    {
        //
        //  Create a text output stream and write it out. We put a UTF-8
        //  transcoder on it.
        //
        try
        {
            TTextFileOutStream strmOut
            (
                strTarPath
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Write
                , new TUTF8Converter
            );
            strmOut.eNewLineType(tCIDLib::ENewLineTypes::LF);

            //
            //  Create a string stream over the source. We do it a line at a time
            //  so that we can control the newlines.
            //
            TString strLine;
            TTextStringInStream strmSrc(&strToSave);
            while (!strmSrc.bEndOfStream())
            {
                strmSrc >> strLine;
                strmOut << strLine << kCIDLib::NewLn;
            }
            strmOut << kCIDLib::NewLn;
            strmOut.Flush();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            TExceptDlg dlgErr
            (
                *this
                , L"Save CML Class"
                , L"An error occurred while exporting the class"
                , errToCatch
            );
        }
    }
}


//
//  This is called to start up or continue a session.
//
tCIDLib::TVoid TMacroDbgMainWnd::GoOrRestart(const EGoActions eAction)
{
    //
    //  If we are stopped, they are just telling us to start running again. So
    //  we just need to let the thread go. Else, we are starting a new debug
    //  session and have a lot more work to do.
    //
    if (m_eCurState == EStates::Stopped)
    {
        // Remove the current line marker while we are running
        TSrcTab* pwndCur = pwndCurSrcTab();
        if (m_pmeciNew
        &&  (pwndCur->strClassPath().bCompareI(m_pmeciNew->strClassPath())))
        {
            pwndCur->pwndEditor()->RemoveLineState
            (
                m_c4NewLine, tCIDWUtils::ECEdLStates::CurLine
            );
        }

        //
        //  Set the state to running again and trigger the event. If the
        //  action is step in or step over, set that up.
        //
        if ((eAction == EGoActions::StepInto) || (eAction == EGoActions::StepOver))
        {
            //
            //  If step over, then set a tep over one time break point, else
            //  just set a standard break to stop us on the next line we
            //  hit anywhere. But, if there are no more lines in the current
            //  method, just do a standard step break.
            //
            if (eAction == EGoActions::StepOver)
            {
                if (m_pmethNew->bMoreLines(m_c4NewIP))
                {
                    m_eOneTimeFlag = EOneTimes::Step;
                    m_strOneTimeClass = m_pmeciNew->strClassPath();
                    m_c2OneTimeMethod = m_pmethNew->c2Id();
                }
                 else
                {
                    m_eActionReq = EActionReqs::Break;
                }
            }
             else
            {
                m_eActionReq = EActionReqs::Break;
            }
        }
        SetState(EStates::Running);
        m_evSync.Trigger();
        return;
    }

    //
    //  At this point, we must be in the Editing state, or something is wrong.
    //  If we are, then they are asking us to start up a debug session, so we
    //  have to compile the session class and kick off the run.
    //
    #if CID_DEBUG_ON
    if (m_eCurState != EStates::Editing)
    {
        TErrBox msgbErr(L"Should be in editing state, but we aren't");
        msgbErr.ShowIt(*this);
        return;
    }
    #endif

    try
    {
        // Get the first tab, which is the session class
        TSrcTab* pwndClass = pwndSrcTabAt(0);

        // Clear out all of the output windows
        m_pwndTabLocals->Reset();
        m_pwndTabMembers->Reset();
        m_pwndTabParms->Reset();
        m_pwndTabBuildRes->Reset();
        m_pwndTabConOut->Reset();

        // Force a save of any opened files
        SaveAll();

        //
        //  Try the parse. We pass ourself as the class manager. And we
        //  pass the parser output window as the parser error handler.
        //
        const tCIDLib::TBoolean bRes = m_meprsDebug.bParse
        (
            pwndClass->strClassPath()
            , m_pmeciMain
            , &m_meDebug
            , m_pwndTabBuildRes
            , this
        );

        if (bRes)
        {
            m_pwndTabBuildRes->ParseEvent
            (
                tCIDMacroEng::EPrsEvs::InfoMsg
                , facCIDMacroDbg().strMsg(kMDbgMsgs::midMsg_CompiledOk)
                , 0
                , 0
                , m_pmeciMain->strClassPath()
            );
        }
         else
        {
            // It failed, so make sure the build results tab is selected
            m_pwndTabsStatus->c4ActivateById(m_pwndTabBuildRes->c4TabId());
        }

        //
        //  If they just wanted to compile, all we care about is already
        //  done. Else, we need to go ahead and run the guy.
        //
        if (bRes && (eAction != EGoActions::Compile))
        {
            //
            //  It worked, so we can go on and run it. So make the session
            //  class the current file.
            //
            m_pwndTabsSrc->c4ActivateByName(pwndClass->strName());
            tCIDLib::TCard2 c2EntryPnt;
            try
            {
                c2EntryPnt = m_meDebug.c2FindEntryPoint(*m_pmeciMain, kCIDLib::False);
            }

            catch(const TError& errToCatch)
            {
                TErrBox msgbWarn(errToCatch.strErrText());
                msgbWarn.ShowIt(*this);
                return;
            }

            if (c2EntryPnt == kMacroEng::c2BadId)
            {
                TErrBox msgbWarn
                (
                    facCIDMacroDbg().strMsg(kMDbgMsgs::midMsg_NoStartMethod)
                );
                msgbWarn.ShowIt(*this);
                return;
            }

            //
            //  If the entry point has any parms, then let the user edit
            //  or enter them. Else, clear out the command line parms string.
            //
            TMEngMethodInfo& methiTarget = m_pmeciMain->methiFind(c2EntryPnt);
            if (methiTarget.c4ParmCount())
            {
                TGetParmsDlg dlgParms;
                if (!dlgParms.bRunDlg(*this, m_strCmdLineParms, methiTarget.c4ParmCount()))
                    return;
            }
             else
            {
                m_strCmdLineParms.Clear();
            }

            m_pwndTabBuildRes->ParseEvent
            (
                tCIDMacroEng::EPrsEvs::InfoMsg
                , facCIDMacroDbg().strMsg(kMDbgMsgs::midMsg_RunningMacro)
                , 0
                , 0
                , m_pmeciMain->strClassPath()
            );

            //
            //  Clear out the last and new position stuff. Do this before
            //  setting the state to running since that may cause some of
            //  this stuff to be accessed.
            //
            m_c4LastLine    = 0;
            m_c4NewLine     = 0;
            m_pmeciLast     = nullptr;
            m_pmeciNew      = nullptr;
            m_pmethiLast    = nullptr;
            m_pmethLast     = nullptr;
            m_pmethiNew     = nullptr;
            m_pmecvLast     = nullptr;
            m_pmecvNew      = nullptr;

            // Put us into running state and set the mouse to wait
            SetState(EStates::Running);
            TWndPtrJanitor janPtr(tCIDCtrls::ESysPtrs::Wait);

            // Turn on the timer that pulls out macro output
            PauseTimer(m_tmidMacroOut, kCIDLib::False);

            // Make sure any one time break is now turned off
            m_eOneTimeFlag = EOneTimes::None;

            //
            //  Ok, spin up the debug thread, which is going to start on the
            //  entry point. Set the break request flag up front, so that
            //  he'll stop on the first line of the entry point.
            //
            m_eActionReq = EActionReqs::Break;
            m_thrRun.Start();
        }
    }

    catch(const TError& errToCatch)
    {
        TExceptDlg dlgErr
        (
            *this
            , errToCatch.strErrText()
            , errToCatch
        );
    }

    catch(...)
    {
        #if CID_DEBUG_ON
        TAudio::AudioCue(tCIDLib::EAudioCues::Sinking);
        TPopUp::PopUpMsg
        (
            CID_FILE
            , CID_LINE
            , L"Start macro debug session"
            , 0
            , L"Got an unknown action request"
        );
        #endif
    }
}


//
//  Create a shortened version of the class path, with just the trailing name part
//  to set as the tab display text.
//
tCIDLib::TVoid
TMacroDbgMainWnd::MakeShortName(const TString& strClassPath, TString& strTabText)
{
    // Find the last period in the path
    tCIDLib::TCard4 c4Pos;
    if (strClassPath.bLastOccurrence(kCIDLib::chPeriod, c4Pos))
    {
        // Copy over everything past that period
        strTabText.CopyInSubStr(strClassPath, c4Pos + 1);
    }
     else
    {
        // Shouldn't happen, but just in case
        strTabText = strClassPath;
    }
}


tCIDLib::TVoid TMacroDbgMainWnd::OpenMacro(const tCIDLib::TBoolean bForEdit)
{
    // Let the user select a class
    TString strClassPath;
    tCIDLib::EOpenRes eRes = eSelectClass
    (
        strClassPath
        , bForEdit ? tCIDMacroEng::EResModes::ReadWrite : tCIDMacroEng::EResModes::ReadOnly
    );

    tCIDLib::TBoolean bSendNot = kCIDLib::True;
    TSrcTab* pwndLoaded = nullptr;
    if (eRes == tCIDLib::EOpenRes::ExistingFile)
    {
        pwndLoaded = pwndLoadFileToTab
        (
            strClassPath
            , bForEdit ? tCIDMacroEng::EResModes::ReadWrite : tCIDMacroEng::EResModes::ReadOnly
        );

        // Load the text into the editor
        m_pwndTabsSrc->c4ActivateById(pwndLoaded->c4TabId());
    }
     else if (eRes == tCIDLib::EOpenRes::NewFile)
    {
        //
        //  They could have a file with this name already, because they might
        //  not have saved it, or the original file was deleted behind our
        //  back.
        //
        pwndLoaded = reinterpret_cast<TSrcTab*>(m_pwndTabsSrc->pwndFindByName(strClassPath));
        if (pwndLoaded)
        {
            TErrBox msgbErr(facCIDMacroDbg().strMsg(kMDbgMsgs::midMsg_AlreadyOpened));
            msgbErr.ShowIt(*this);
            bSendNot = kCIDLib::False;
        }
         else
        {
            //
            //  Load up an empty outline of a class for the user to fill
            //  in. If they never save it, it'll never get created, since
            //  we are just directly loading up a string.
            //
            pwndLoaded = pwndLoadNewFileToTab(strClassPath);
            m_pwndTabsSrc->c4ActivateById(pwndLoaded->c4TabId());
        }
    }

    //
    //  Activating a tab won't cause it to send an event, since it's a direct programmatic
    //  change. So we need to send out what we would have had we gotten a tab change
    //  event.
    //
    if (bSendNot && pwndLoaded)
    {
        SendFileEvent(pwndLoaded->strClassPath(), pwndLoaded->pwndEditor()->c4CurLine());
    }
}


//
//  Loads up the passed class path into a new open file item, with the initial
//  mode. It will see if the file is already loaded and not load it again.
//
TSrcTab*
TMacroDbgMainWnd::pwndLoadFileToTab(const   TString&             strClassPath
                                    , const tCIDMacroEng::EResModes eMode)
{
    //
    //  See if this class is already in the cache. If not, then we need to
    //  load it up. We use the client code's class manager to do the load
    //  for us.
    //
    tCIDLib::TCard4 c4Index = m_pwndTabsSrc->c4NameToIndex(strClassPath, kCIDLib::False);

    //
    //  If it's already loaded, then we have some special considerations. if the
    //  requested mode is the same as the current mode, just do a message box
    //  saying this. Else, if it is in read mode, and the new mode is read/write,
    //  then ask if they want to open it for writing. If the other way, ask if
    //  they want to throw away any changes made and go back to read mode.
    //
    TSrcTab* pwndTab = nullptr;
    if (c4Index != kCIDLib::c4MaxCard)
    {
        pwndTab = static_cast<TSrcTab*>(m_pwndTabsSrc->pwndTabAt(c4Index));

        if (pwndTab->eMode() == eMode)
        {
            // Just tell them we are already there
            TErrBox msgbWarn
            (
                facCIDMacroDbg().strMsg(kMDbgMsgs::midMsg_AlreadyInThatMode, strClassPath)
            );
            msgbWarn.ShowIt(*this);
        }
         else if (eMode == tCIDMacroEng::EResModes::ReadOnly)
        {
            TYesNoBox msgbQ
            (
                facCIDMacroDbg().strMsg(kMDbgMsgs::midQ_ChangeToReadMode, strClassPath)
            );

            if (msgbQ.bShowIt(*this))
            {
                UndoWriteMode(strClassPath);
                pwndTab->eMode(tCIDMacroEng::EResModes::ReadOnly);
            }
        }
         else
        {
            TYesNoBox msgbQ
            (
                facCIDMacroDbg().strMsg(kMDbgMsgs::midQ_ChangeToWriteMode, strClassPath)
            );

            if (msgbQ.bShowIt(*this))
                pwndTab->eMode(tCIDMacroEng::EResModes::ReadWrite);
        }

        m_pwndTabsSrc->ActivateAt(c4Index);
    }
     else
    {
        // Create a stream for the class content
        TTextInStream* pstrmSrc = pstrmLoadClass(strClassPath, eMode);
        TJanitor<TTextInStream> janStrm(pstrmSrc);

        // For the display text of the tab we only want the name part
        TString strTabText;
        MakeShortName(strClassPath, strTabText);

        // We want it to use CR/LF new lines
        pstrmSrc->eNewLineType(tCIDLib::ENewLineTypes::CRLF);

        const tCIDLib::TBoolean bIsSessionClass(m_pwndTabsSrc->c4TabCount() == 0);

        // And add a new tab, indicate if this is the first one (the session class)
        pwndTab = new TSrcTab(strClassPath, bIsSessionClass);
        pwndTab->CreateSrcTab(*m_pwndTabsSrc, strTabText);

        TJanitor<TSrcTab> janItem(pwndTab);
        pwndTab->pwndEditor()->LoadText(strClassPath, *pstrmSrc);

        //
        //  NOW THAT THE text is already loaded, we can set make sure that the
        //  the read only attribute it set on the editor to match the mode we
        //  were given. If we did it before, then the editor would ignore any
        //  attempt to load the text.
        //
        pwndTab->eMode(eMode);

        // Register for notifications from the editor
        pwndTab->pwndEditor()->pnothRegisterHandler(this, &TMacroDbgMainWnd::eSrcEdHandler);

        // If this is the first tab, it's the session class, so we want to remember its id.
        if (bIsSessionClass)
            m_c4TabId_Session = pwndTab->c4TabId();

        // Release the new tab from the janitor now that it's safe
        janItem.Orphan();
    }
    return pwndTab;
}


//
//  Unlike pwndLoadFileToTab, this one is called when we know we have no existing
//  tabs, so it can be more efficient. This is usually just called for the initial
//  session class to get the first tab loaded.
//
TSrcTab* TMacroDbgMainWnd::pwndLoadNewFileToTab(const TString& strClassPath)
{
    // For the display text of the tab we only want the name part
    TString strTabText;
    MakeShortName(strClassPath, strTabText);

    //
    //  And add a new item with the default template text for an empty
    //  class. We need to update a replacement token with the actual
    //  class path.
    //
    TString strTmplText(facCIDMacroEng().strEmptyClassTemplate());
    strTmplText.eReplaceToken(strClassPath, kCIDLib::chDigit1);

    // Make sure the containing scope exists and store the default content
    StoreClass(strClassPath, strTmplText);

    const tCIDLib::TBoolean bIsSessionClass(m_pwndTabsSrc->c4TabCount() == 0);

    TSrcTab* pwndTab = new TSrcTab(strClassPath, bIsSessionClass);
    pwndTab->CreateSrcTab(*m_pwndTabsSrc, strTabText);

    //
    //  Tell it what mode it should be in. For new files, it's always read write,
    //  so it doesn't matter if we do it before we try to load the text.
    //
    pwndTab->eMode(tCIDMacroEng::EResModes::ReadWrite);
    pwndTab->pwndEditor()->LoadText(strClassPath, strTmplText);

    // Register for notifications from the editor
    pwndTab->pwndEditor()->pnothRegisterHandler(this, &TMacroDbgMainWnd::eSrcEdHandler);

    return pwndTab;
}


//
//  Find the tab, if any, for the indicated class path, and cast it back to its
//  real type before returning it.
//
const TSrcTab* TMacroDbgMainWnd::pwndFindSrcTab(const TString& strClassPath) const
{
    const TTabWindow* pwndTab = m_pwndTabsSrc->pwndFindByName(strClassPath);
    if (!pwndTab)
        return nullptr;

    return static_cast<const TSrcTab*>(pwndTab);
}

TSrcTab* TMacroDbgMainWnd::pwndFindSrcTab(const TString& strClassPath)
{
    TTabWindow* pwndTab = m_pwndTabsSrc->pwndFindByName(strClassPath);
    if (!pwndTab)
        return nullptr;

    return static_cast<TSrcTab*>(pwndTab);
}

// Return the nth source tab, cast to its real type
const TSrcTab* TMacroDbgMainWnd::pwndSrcTabAt(const tCIDLib::TCard4 c4Index) const
{
    return static_cast<TSrcTab*>(m_pwndTabsSrc->pwndTabAt(c4Index));
}

TSrcTab* TMacroDbgMainWnd::pwndSrcTabAt(const tCIDLib::TCard4 c4Index)
{
    return static_cast<TSrcTab*>(m_pwndTabsSrc->pwndTabAt(c4Index));
}


//
//  Go through all of the tabs and make sure that all but the indicated excluded
//  one do not have a current line marker.
//
tCIDLib::TVoid TMacroDbgMainWnd::RemoveCurLine(const tCIDLib::TCard4 c4Exclude)
{
    const tCIDLib::TCard4 c4Count = m_pwndTabsSrc->c4TabCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (c4Index != c4Exclude)
            pwndSrcTabAt(c4Index)->pwndEditor()->RemoveCurLineState();
    }
}


//
//  Called internally to save all open classes. We call the public version which will
//  return status and an error message if a failure. We display the failure if any.
//
tCIDLib::TVoid TMacroDbgMainWnd::SaveAll()
{
    TString strErr;
    if (!bSaveAll(strErr))
    {
        TErrBox msgbErr
        (
            facCIDMacroDbg().strMsg(kMDbgMsgs::midGen_Title), strErr
        );
        msgbErr.ShowIt(*this);
    }
}


//
//  A helper for derived classes to call when they want to send out a list
//  event. These are al done synchronously.
//
tCIDLib::TVoid TMacroDbgMainWnd::SendStateEvent(const TString& strState)
{
    SendAsyncNotify
    (
        new TCMLDbgChangeInfo(tCIDMacroDbg::EEvents::IDEState, strState, kCIDLib::c4MaxCard, *this)
        , TMacroDbgMainWnd::nidChange
    );
}

tCIDLib::TVoid
TMacroDbgMainWnd::SendFileEvent(const   TString&                strCurFile
                                , const tCIDLib::TCard4         c4LineNum)
{
    SendAsyncNotify
    (
        new TCMLDbgChangeInfo(tCIDMacroDbg::EEvents::NewFile, strCurFile, c4LineNum, *this)
        , TMacroDbgMainWnd::nidChange
    );
}

tCIDLib::TVoid TMacroDbgMainWnd::SendLineEvent(const tCIDLib::TCard4 c4LineNum)
{
    SendAsyncNotify
    (
        new TCMLDbgChangeInfo
        (
            tCIDMacroDbg::EEvents::LineChanged, TString::strEmpty(), c4LineNum, *this
        )
        , TMacroDbgMainWnd::nidChange);
}



//
//  When we need to move to a particular source tab (class) and position within it
//  this is called. Generally it's for when the user double clicks in the build
//  results tab on an error indicator. It's also used when we hit a break point or
//  when doing step operations. The last parameter indicates whether or not we should
//  make this the current line or not, which is used in the latter cases.
//
tCIDLib::TVoid
TMacroDbgMainWnd::SetCurEditPos(const   TString&            strClassPath
                                , const tCIDLib::TCard4     c4Line
                                , const tCIDLib::TCard4     c4Col
                                , const tCIDLib::TBoolean   bSetCurLine)
{
    TSrcTab* pwndTarSrc;
    pwndTarSrc = reinterpret_cast<TSrcTab*>(m_pwndTabsSrc->pwndFindByName(strClassPath));
    if (!pwndTarSrc)
    {
        // Don't have it so we need to load it
        pwndTarSrc = pwndLoadFileToTab(strClassPath, tCIDMacroEng::EResModes::ReadOnly);
    }

    // Activate it if we already had it or got it loaded. The load may have failed.
    if (pwndTarSrc)
    {
        const tCIDLib::TCard4 c4TabIndex
        (
            m_pwndTabsSrc->c4ActivateById(pwndTarSrc->c4TabId())
        );

        // Ask it if this is a valid position
        if (pwndTarSrc->pwndEditor()->bIsValidPos(c4Line, c4Col))
        {
            //
            //  Looks valid, so move the cursor there and set it as the current line. Just
            //  in case, we remove any currently set markers, in case more than one get's
            //  marked. Tell it to keep the position centered vertically.
            //
            pwndTarSrc->pwndEditor()->RemoveCurLineState();
            pwndTarSrc->pwndEditor()->SetPosition
            (
                c4Line, c4Col, kCIDLib::True, kCIDLib::True
            );

            // Make sure all other files have had the current line indicator removed
            RemoveCurLine(c4TabIndex);

            //
            //  We need to get the focus to the tab, but can't do it here, because
            //  we are still in the click handler from the list. So we post ourself
            //  an asynchronous code to do it. We pass along the index of the
            //  new tab
            //
            SendCode(CIDMacroDbg_DbgMainWnd::i4ErrLocGo, c4TabIndex);
        }
         else
        {
            TErrBox msgbWarn(facCIDMacroDbg().strMsg(kMDbgMsgs::midMsg_NotValidSrcPos));
            msgbWarn.ShowIt(*this);
        }
    }
}


//
//  When we start debugging, we set all source editors to read only state. When we come
//  back to edit state, any tabs that weren't opened for read only access we set back to
//  non-read only state.
//
tCIDLib::TVoid TMacroDbgMainWnd::SetEditorROState(const tCIDLib::TBoolean bReadOnly)
{
    const tCIDLib::TCard4 c4Count = m_pwndTabsSrc->c4TabCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TSrcTab* pwndCur = pwndSrcTabAt(c4Index);
        if (bReadOnly)
        {
            pwndCur->pwndEditor()->bReadOnly(kCIDLib::True);
        }
         else
        {
            if (pwndCur->eMode() != tCIDMacroEng::EResModes::ReadOnly)
                pwndCur->pwndEditor()->bReadOnly(kCIDLib::False);
        }
    }
}



//
//  Update any windows that need to be told about the current display radix.
//
tCIDLib::TVoid TMacroDbgMainWnd::SetObjWndRadix(const tCIDLib::ERadices eToSet)
{
    m_pwndTabMembers->SetRadix(eToSet);
    m_pwndTabLocals->SetRadix(eToSet);
    m_pwndTabParms->SetRadix(eToSet);
}


//
//  This is called to set our current state, stopped, running, etc... We only allow
//  certain operations in certain states.
//
tCIDLib::TVoid TMacroDbgMainWnd::SetState(const EStates eToSet)
{
    TSrcTab* pwndCur = pwndCurSrcTab();
    TSrcEditor* pwndEdit = 0;
    if (pwndCur)
        pwndEdit = pwndCur->pwndEditor();

    // Store the new state
    m_eCurState = eToSet;

    switch(eToSet)
    {
        case EStates::NoSession :
            SendStateEvent(TString::strEmpty());

            // Clear all of the status windows
            m_pwndTabMembers->Reset();
            m_pwndTabLocals->Reset();
            m_pwndTabParms->Reset();

            // Tell the search tab that it can't search now
            m_pwndTabSearch->SetState(kCIDLib::False);

            // Disable all but the options submenus
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Edit, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Class, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug, kCIDLib::False);

            break;

        case EStates::Editing :
            //
            //  For any tabs not marked read only, turn off the read only state on the
            //  editors, now that we are back in edit mode.
            //
            SetEditorROState(kCIDLib::False);

            // Update the status window if it's ready yet
            SendStateEvent(L"Edit");

            // Clear all of the status windows
            m_pwndTabMembers->Reset();
            m_pwndTabLocals->Reset();
            m_pwndTabParms->Reset();

            // We can do find/replace now
            m_pwndTabSearch->SetState(kCIDLib::True);

            //
            //  We have a session but we aren't debugging. So enable the
            //  editing and debugging menu, and disable the debug options
            //  except for Go and Compile.
            //
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Edit, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Class, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Go, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Compile, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_StepOver, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_StepInto, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Stop, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Break, kCIDLib::False);
            break;

        case EStates::Stopped :
            SendStateEvent(L"Break");

            // Update all of the status windows and make them all editable
            m_pwndTabMembers->Update(m_pmecvNew);
            m_pwndTabMembers->SetEditOK(kCIDLib::True);

            if (m_pmethNew)
            {
                m_pwndTabLocals->Update(m_pmethNew);
                m_pwndTabLocals->SetEditOK(kCIDLib::True);
                m_pwndTabParms->Update();
                m_pwndTabParms->SetEditOK(kCIDLib::True);
            }
             else
            {
                m_pwndTabLocals->Reset();
                m_pwndTabParms->Reset();
            }

            // We can do find/replace now
            m_pwndTabSearch->SetState(kCIDLib::True);

            //
            //  We have a session and we are debugging, but currently stopped.
            //  So we enable the overall menu, and handle some individual
            //  items on the debug menu.
            //
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Go, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_StepOver, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_StepInto, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Stop, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Break, kCIDLib::False);
            break;

        case EStates::Running :
            SendStateEvent(L"Run");

            //
            //  Clear the locals/parameters windows and make the members window
            //  non-editable. Reseting the others also makes them non-editable.
            //
            m_pwndTabLocals->Reset();
            m_pwndTabParms->Reset();
            m_pwndTabMembers->Reset();

            // Tell the search tab that it can't search now
            m_pwndTabSearch->SetState(kCIDLib::False);

            // Set all source tabs read only
            SetEditorROState(kCIDLib::True);

            //
            //  We have a session and we are debugging, and currently running.
            //  So we enable some individual items on the debug menu.
            //
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Stop, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Break, kCIDLib::True);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Go, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_Compile, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_StepOver, kCIDLib::False);
            m_menuContext.SetItemEnable(kCIDMacroDbg::ridMenu_Debug_StepInto, kCIDLib::False);
            break;
    }
}


