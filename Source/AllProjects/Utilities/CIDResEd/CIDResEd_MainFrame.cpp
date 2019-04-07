//
// FILE NAME: CIDResEd_MainFrame.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/17/2000
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
//  Includes
// ---------------------------------------------------------------------------
#include "CIDResEd.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdMainFrame,TStdEditorFrame)


// ---------------------------------------------------------------------------
//   CLASS: TResEdMainFrame
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdMainFrame: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdMainFrame::TResEdMainFrame() :

    m_pwndAttrsPane(nullptr)
    , m_pwndClient(nullptr)
    , m_pwndEditPane(nullptr)
    , m_pwndEditCont(nullptr)
    , m_pwndTabsPane(nullptr)
    , m_pwndTab_Dialogs(nullptr)
    , m_strState_NoSession(L"NoSession")
    , m_strState_Session(L"Session")
{
}

TResEdMainFrame::~TResEdMainFrame()
{
}


// ---------------------------------------------------------------------------
//  TResEdMainFrame: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We first let our windows have a try at it, if it has the focus. If they don't claim
//  the input or the focus is elsewhere,  we pass it on to our parent for the menu bar table.
//
tCIDLib::TBoolean
TResEdMainFrame::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    if (m_pwndClient && m_pwndClient->bContainsFocus())
    {
        if (m_pwndClient->bProcessAccel(pMsgData))
            return kCIDLib::True;
    }
    return TParent::bProcessAccel(pMsgData);
}



// ---------------------------------------------------------------------------
//  TResEdMainFrame: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TResEdMainFrame::bChanges() const
{
    //
    //  If there's a session open and there are changes, then return true.
    //  We have to check any editor windows as well, since they could have
    //  currently open changes.
    //
    return
    (
        !m_strFacBaseName.bIsEmpty()
        && ((m_redatCur != m_redatOrg) || m_pwndEditCont->bChanges())
    );
}


//
//  This is a helper for the dialog attribute editors. Both the dialog and the items
//  have text symbols the user can set. When a symbol has been validated it will
//  call us here to find the info and update the passed text symbol with the
//  info.
//
//  This is kind of redundant, since the validation also does similar stuff, and
//  calls bTextSymExists() below, but validation and storage have to be done
//  separately.
//
tCIDLib::TBoolean
TResEdMainFrame::bFindTextSym(const TString& strToFind, TTextSym& tsymToLoad) const
{
    // If it's empty, just reset the symbol and return success
    if (strToFind.bIsEmpty())
    {
        tsymToLoad.Reset();
        return kCIDLib::True;
    }

    // Parse it out into type and real symbol
    TString strRealSym;
    tCIDResEd::EMsgTypes eType = facCIDResEd.eParseTextSym(strToFind, strRealSym);
    if (eType == tCIDResEd::EMsgTypes::Count)
        return kCIDLib::False;

    // Get the message list for this type of message
    const TResEdMsgList& remilTar = m_redatCur.remilByType(eType);

    // And search that guy
    const tCIDLib::TCard4 c4Index = remilTar.c4FindByName(strRealSym);

    if (c4Index != kCIDLib::c4MaxCard)
    {
        const TResEdMsgItem& remitSym = remilTar.remitAt(c4Index);
        tsymToLoad.Set(strToFind, eType);
        return kCIDLib::True;
    }

    return kCIDLib::False;
}


//
//  Just checks if the passed text symbol exists. An empty string is considered
//  fine, it's just a 'no text' symbol and can be stored as is.
//
tCIDLib::TBoolean TResEdMainFrame::bTextSymExists(const TString& strToFind) const
{
    // If it's empty, just say it does
    if (strToFind.bIsEmpty())
        return kCIDLib::True;

    // Parse it out into type and real symbol
    TString strRealSym;
    tCIDResEd::EMsgTypes eType = facCIDResEd.eParseTextSym(strToFind, strRealSym);
    if (eType == tCIDResEd::EMsgTypes::Count)
        return kCIDLib::False;

    // Get the message list for this type of message and search it
    const TResEdMsgList& remilTar = m_redatCur.remilByType(eType);
    return (remilTar.c4FindByName(strRealSym) != kCIDLib::c4MaxCard);
}



// Indicate whether we have a session open
tCIDLib::TBoolean TResEdMainFrame::bSessionOpen() const
{
    return !m_strFacBaseName.bIsEmpty();
}


// Return the current count of dialogs
tCIDLib::TCard4 TResEdMainFrame::c4DialogCount() const
{
    return m_redatCur.colDlgs().c4ElemCount();
}


//
//  Clear out the dialog or item ifno. THese show multiple values so it's cleaner
//  to have a dedicated clear for them. One with single values can be cleared by
//  just sending an empty value.
//
tCIDLib::TVoid TResEdMainFrame::ClearDlgInfo()
{
    SetStatusBarText(kCIDResEd::c4SBar_DlgInfo, TString::strEmpty());
}

tCIDLib::TVoid TResEdMainFrame::ClearItemInfo()
{
    SetStatusBarText(kCIDResEd::c4SBar_ItemInfo, TString::strEmpty());
}


//
//  This is called to close down a session. If there is one and there are
//  changes, we let the user optionally save it before closing. If they
//  choose to save, we clear everything out and reset the menus back to
//  default state.
//
tCIDLib::TVoid TResEdMainFrame::CloseSession()
{
    //
    //  If there's a session open, and there are any changes, or there's
    //  outstanding changes in any editor window, then ask if we should
    //  save first.
    //
    if (bChanges())
    {
        // Make sure that they really want to exit
        TYesNoBox msgbAsk(facCIDResEd.strMsg(kResEdMsgs::midQ_SaveSession));
        if (msgbAsk.bShowIt(*this))
            SaveSession();
    }

    // Clear out all the data let's clear it out
    m_redatOrg.Reset();
    m_redatCur.Reset();
    m_strCurFile.Clear();
    m_strFacBaseName.Clear();
    m_strFacPath.Clear();

    // Tell the editor windows to go away
    m_pwndEditCont->Reset(kCIDLib::True);

    // Update the main frame sesssion state
    SetSessionState(kCIDLib::False, facCIDResEd.strMsg(kResEdMsgs::midGen_Title1));
}



//
//  Called to create the us. We call our parent to kick it off, passing in the
//  info he needs to set up the status bar and our menu. He'll call our
//  CreateLayout() method when it's time to create our panes.
//
tCIDLib::TVoid TResEdMainFrame::CreateMain()
{
    //
    //  We want three slots in the status bar, so pass the sizes of the first two.
    //  The last one gets the remainder.
    //
    tCIDLib::TCardList fcolAreas(3);
    fcolAreas.c4AddElement(TGUIFacility::c4AdjustHDlgUnit(120));
    fcolAreas.c4AddElement(TGUIFacility::c4AdjustHDlgUnit(240));
    fcolAreas.c4AddElement(TGUIFacility::c4AdjustHDlgUnit(60));
    TParent::CreateFrame
    (
        nullptr
        , facCIDCtrls().areaDefWnd()
        , facCIDResEd.strMsg(kResEdMsgs::midGen_Title1)
        , fcolAreas
        , facCIDResEd
        , kCIDResEd::ridMenu_Main
    );

    // Set up the menus for initially no seession state
    SetSessionState(kCIDLib::False, facCIDResEd.strMsg(kResEdMsgs::midGen_Title1));

    SendCode(1, 1);
}



//
//  We handle list events from any lists in the tab area on the left. Currently
//  there's only one. If we add more we'll have to do separate handlers, since
//  these are not direct children and so the window id isn't of much use to
//  distinguish them.
//
tCIDCtrls::EEvResponses
TResEdMainFrame::eListHandler(TListChangeInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Cleared)
    {
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        //
        //  If it's from the dialogs tab, then they want to edit a new dialog.
        //  If there are changes in the current one then save them.
        //
        if (m_pwndEditCont->bChanges())
            m_pwndEditCont->wndEditor().SaveChanges(m_redatCur);

        //
        //  Ask the editor to load up the new one. It has to come from the current
        //  changes data of course.
        //
        const tCIDLib::TCard4 c4Index = m_redatCur.c4FindDlgIndex(wnotEvent.c4Id());
        if (c4Index == kCIDLib::c4MaxCard)
        {
            TErrBox msgbErr(L"Received dialog id didn't map to a valid index");
            msgbErr.ShowIt(*this);
        }
         else
        {
            m_pwndEditCont->EditDlg(m_redatCur.reddAt(c4Index));
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


//
//  Called when the user asks to open a new session. We close any existing
//  session (if the users agrees), saving any changes if they want to, then we
//  open a new one.
//
tCIDLib::TVoid TResEdMainFrame::OpenSession()
{
    //
    //  Do a close session in case there's currently one open. This will
    //  give them a chance to save if there are changes, and will clean
    //  us back to a blank slate either way.
    //
    CloseSession();

    tCIDLib::TKVPList colTypes;
    colTypes.objAdd(TKeyValuePair
    (
        facCIDResEd.strMsg(kResEdMsgs::midGen_ResType), L"*.CIDRC")
    );

    //
    //  Get the current directory, which is what we want the dialog
    //  to start in.
    //
    static TPathStr pathPrevious;
    if (pathPrevious.bIsEmpty())
        TFileSys::QueryCurrentDir(pathPrevious);

    tCIDLib::TStrList colSel;
    const tCIDLib::TBoolean bRes = facCIDCtrls().bOpenFileDlg
    (
        *this
        , L"Open a Resource File"
        , pathPrevious
        , colSel
        , colTypes
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EFOpenOpts::StrictTypes
            , tCIDCtrls::EFOpenOpts::PathMustExist
          )
    );

    if (!bRes)
        return;

    // They opened a file, so let's get ourselves set up
    const TString& strToOpen = colSel[0];
    if (bOpenSession(strToOpen, !TFileSys::bExists(strToOpen)))
    {
        // Store the new path as the previous path
        pathPrevious = strToOpen;
        pathPrevious.bRemoveNameExt();

        // Update frame's sesssion state
        TString strTitle = facCIDResEd.strMsg(kResEdMsgs::midGen_Title1);
        strTitle.Append(L" [");
        strTitle.Append(m_strFacBaseName);
        strTitle.Append(L"]");
        SetSessionState(kCIDLib::True, strTitle);
    }
}


//
//  Called to get information about a loadable text value in one of the
//  lists at a specific index.
//
tCIDLib::TVoid
TResEdMainFrame::QueryTextSym(  const   tCIDResEd::EMsgTypes    eType
                                , const tCIDLib::TCard4         c4Index
                                ,       TString&                strSym
                                ,       tCIDLib::TCard4&        c4TextId
                                ,       TString&                strText)
{
    const TResEdMsgList& remilSrc(m_redatCur.remilByType(eType));
    const TResEdMsgItem& remitSrc = remilSrc.remitAt(c4Index);

    strSym = remitSrc.strName();
    c4TextId = remitSrc.c4Id();
    strText = remitSrc.strMsgText();
}


//
//  Find a dialog by name or resource id or edit id
//
const TResEdDlgDesc*
TResEdMainFrame::preddByEditId(const tCIDLib::TCard4 c4ToFind) const
{
    return m_redatCur.preddByEditId(c4ToFind);
}

const TResEdDlgDesc* TResEdMainFrame::preddByResId(const tCIDLib::TResId ridToFind) const
{
    return m_redatCur.preddByResId(ridToFind);
}

const TResEdDlgDesc* TResEdMainFrame::preddByName(const TString& strName) const
{
    return m_redatCur.preddByName(strName);
}


//
//  Find a particular message item, based on its type and symbol name.
//
const TResEdMsgItem*
TResEdMainFrame::premitFind(const   tCIDResEd::EMsgTypes    eType
                            , const TString&                strToFind) const

{
    const TResEdMsgList& remilSrc(m_redatCur.remilByType(eType));
    return remilSrc.premitByName(strToFind);
}


// Provide access to one of the message lists by type
const TResEdMsgList&
TResEdMainFrame::remilByType(const tCIDResEd::EMsgTypes eType) const
{
    return m_redatCur.remilByType(eType);
}


// Access to the current file name, for any errors that need it
const TString& TResEdMainFrame::strCurFile() const
{
    return m_strCurFile;
}


// Access to the facility base name and path
const TString& TResEdMainFrame::strFacBaseName() const
{
    return m_strFacBaseName;
}


const TString& TResEdMainFrame::strFacPath() const
{
    return m_strFacPath;
}


//
//  This allows everyone to load text resource data for display purposes
//  internally, using the symbolic text name.
//
const TString& TResEdMainFrame::strLoadResText(const TTextSym& tsymToLoad)
{
    // If it's an empty text symbol, then just return an empty string
    if (!tsymToLoad.bIsValid())
        return TString::strEmpty();
    return m_redatCur.strLoadResText(tsymToLoad);
}


//
//  Called to update our menus  and title bar when a session is opened or
//  closed.
//
tCIDLib::TVoid
TResEdMainFrame::SetSessionState(const  tCIDLib::TBoolean   bOpen
                                , const TString&            strTitleText)
{
    TMenuBar& menuBar = menuCur();

    // Reset our tabs back to defaults
    m_pwndTab_Dialogs->Reset();

    //
    //  Ask the content scroll area window to size down it's contained editor
    //  window nice and small so that the scroll bars won't seem to be scrolling
    //  some unseen object.
    //
    m_pwndEditPane->SetChildSize(TSize(16, 16));

    if (bOpen)
    {
        ApplyWndState(m_strState_Session);

        // Load up the available dialogs into the dialog list tab
        m_pwndTab_Dialogs->LoadList(m_redatCur);
    }
     else
    {
        m_pwndEditCont->Reset(kCIDLib::True);
        ApplyWndState(m_strState_NoSession);
    }

    // Update the title text
    strWndText(strTitleText);
}



// This is called to save any outstanding changes
tCIDLib::TVoid TResEdMainFrame::SaveSession()
{
    // We shouldn't ever get called without a session open
    CIDAssert
    (
        bSessionOpen(), L"SaveSession called with empty name or path"
    );

    // If there's any outstanding dialog editor changes, bring those in
    const tCIDLib::TBoolean bDlgChanges = m_pwndEditCont->bChanges();
    if (bDlgChanges)
        m_pwndEditCont->wndEditor().SaveChanges(m_redatCur);

    try
    {
        //
        //  Write out the data to the original file. We pass it the original
        //  data as well, so it can see what has changed and only update the files
        //  that need it.
        //
        m_redatCur.WriteTo(m_strFacPath, m_strFacBaseName, m_redatOrg);

        // It worked, so update our original copy since this is new baseline
        m_redatOrg = m_redatCur;

        //
        //  And tell the editor to commit any changes if we saved any. It will copy
        //  its working data to its orignal data so it doesn't think there are any
        //  chanes anymore.
        //
        if (bDlgChanges)
            m_pwndEditCont->wndEditor().CommitChanges();
    }

    catch(const TError& errToCatch)
    {
        TExceptDlg dlgErr
        (
            *this, facCIDResEd.strMsg(kResEdErrs::errcSess_SaveFailed), errToCatch
        );
    }
}


//
//  If the user makes changes to one of the message lists, the editor will
//  call here to store the changes.
//
tCIDLib::TVoid
TResEdMainFrame::StoreMsgText(  const   TResEdMsgList&  remilErrs
                                , const TResEdMsgList&  remilMsgs)
{
    // Pass it on the current resource data
    m_redatCur.StoreMsgText(remilErrs, remilMsgs);
}


//
//  If the editor window sees that the id or symbolic name of a dialog has changed,
//  it needs to get that to the left side dialog list tab. So it calls this and we
//  pass it on.
//
tCIDLib::TVoid TResEdMainFrame::UpdateDlg(const TResEdDlgDesc& reddNew)
{
    m_pwndTab_Dialogs->UpdateDlg(reddNew);
}


//
//  The editor window sends us text to show in the status bar. He sends it indirectly
//  via the facility object, which knows about us and he has access to the facility
//  object.
//
tCIDLib::TVoid
TResEdMainFrame::UpdateDlgInfo( const   TString&            strSymbol
                                , const tCIDCtrls::TWndId   widSymbol)
{
    //
    //  If the symbol is empty, we just clear the text, else format the symbol
    //  and id for display.
    //
    if (strSymbol.bIsEmpty() && !widSymbol)
    {
        SetStatusBarText(kCIDResEd::c4SBar_DlgInfo, L"[No symbol]");
    }
     else
    {
        m_strSBTmpFmt = strSymbol;
        m_strSBTmpFmt .Append(L"/");
        m_strSBTmpFmt .AppendFormatted(widSymbol);
        SetStatusBarText(kCIDResEd::c4SBar_DlgInfo, m_strSBTmpFmt);
    }
}

tCIDLib::TVoid
TResEdMainFrame::UpdateItemInfo(const   TString&            strSymbol
                                , const tCIDCtrls::TWndId   widSymbol
                                , const TString&            strType
                                , const TArea&              areaPos)
{
    if (strSymbol.bIsEmpty() && !widSymbol)
    {
        m_strSBTmpFmt = L"[No Symbol]";
    }
     else
    {
        m_strSBTmpFmt = strSymbol;
        m_strSBTmpFmt.Append(L"/");
        m_strSBTmpFmt.AppendFormatted(widSymbol);
    }

    m_strSBTmpFmt.Append(L"  (");
    m_strSBTmpFmt.Append(strType);
    m_strSBTmpFmt.Append(L")    ");
    areaPos.FormatToText(m_strSBTmpFmt, kCIDLib::chComma, kCIDLib::True);

    SetStatusBarText(kCIDResEd::c4SBar_ItemInfo, m_strSBTmpFmt);
}

tCIDLib::TVoid TResEdMainFrame::UpdateMousePos(const TString& strPos)
{
    // It will be empty if the mouse has moved out of the editor window
    SetStatusBarText(kCIDResEd::c4SBar_MousePos, strPos);
}


// ---------------------------------------------------------------------------
//  TResEdMainFrame: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TResEdMainFrame::AreaChanged(const  TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && m_pwndClient && bSizeChanged)
    {
        TArea areaClient;
        QueryClientArea(areaClient, kCIDLib::True, *m_pwndClient);
        m_pwndClient->SetSize(areaClient.szArea(), kCIDLib::True);
    }

    // Works best to do this last for redrawing purposes
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);
}


// We override this to have a chance to save any changes
tCIDLib::TBoolean TResEdMainFrame::bAllowShutdown()
{
    TYesNoBox msgbAsk
    (
        facCIDResEd.strMsg(kResEdMsgs::midDlg_About_Text1)
        , L"Do you want to exit the program?"
    );

    if (msgbAsk.bShowIt(*this))
    {
        // See if changes exist
        if (bChanges())
        {
            TYesNoBox msgbAsk(facCIDResEd.strMsg(kResEdMsgs::midQ_Exit));
            if (msgbAsk.bShowIt(*this))
                SaveSession();
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TResEdMainFrame::bCreated()
{
    if (!TParent::bCreated())
        return kCIDLib::False;

    //
    //  Get our client area now, before we create any other windows. It won't
    //  include any status bar, menu etc... We'll use this to initially set the
    //  height of our pane windows.
    //
    TArea areaClient;
    QueryClientArea(areaClient, kCIDLib::True);

    // Create the main client pane container window and set it as our client
    m_pwndClient = new TPaneWndCont();
    m_pwndClient->CreatePaneCont
    (
        *this
        , areaClient
        , widNext()
        , tCIDCtrls::EWndStyles::PaneContainer
        , tCIDCtrls::EExWndStyles::None
    );
    m_pwndClient->SetBgnColor
    (
        facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::EditWndBgn), kCIDLib::True
    );

    // Set our layout, which is the three vertical panes scheme
    m_pwndClient->SetLayout(new TPaneLayout3Vert);

    //
    //  Create the tabbed window and add it as the left pane, with a minimum
    //  width of the calculated tab width.
    //
    m_pwndTabsPane = new TTabbedWnd();
    m_pwndTabsPane->CreateTabbedWnd
    (
        *m_pwndClient
        , widNext()
        , TArea(0, 0, 64, areaClient.c4Height())
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::ETabWStyles::None
        , tCIDCtrls::EExWndStyles::ControlParent
    );
    m_pwndTabsPane->SetBgnColor
    (
        facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window), kCIDLib::True
    );
    m_pwndClient->SetPane(m_pwndTabsPane, 0, TSize(64, 0), TSize(180, 0));

    // Create the scroll window and set it as the 2nd pane
    m_pwndEditPane = new TScrollArea();
    m_pwndEditPane->Create
    (
        *m_pwndClient
        , widNext()
        , TArea(0, 0, 64, areaClient.c4Height())
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::EScrAStyles::None
    );
    m_pwndClient->SetPane(m_pwndEditPane, 1, TSize(64, 0), TSize(128, 0));


    //
    //  Create an attribute editor window and set it as the settings pane child.
    //  Initial size doesn't matter. It will be sized to fit. Now that we use our
    //  own derivative, so we can do custom validation and value entry.
    //
    m_pwndAttrsPane = new TResAttrEdPane();
    m_pwndAttrsPane->CreatePaneCont
    (
        *m_pwndClient
        , TArea(0, 0, 64, areaClient.c4Height())
        , widNext()
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::EExWndStyles::None
    );
    m_pwndClient->SetPane(m_pwndAttrsPane, 2, TSize(64, 0), TSize(128, 0));

    //
    //  Set the editor pane wnd to the standard edit wnd bgn color, so that the
    //  dialog will contrast against it, making it easy to see where it ends.
    //
    m_pwndEditPane->SetBgnColor
    (
        facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::EditWndBgn), kCIDLib::True
    );


    // Create the editor window and set it on the scroll area
    m_pwndEditCont = new TResEdCont;
    m_pwndEditCont->Create(*m_pwndEditPane, widNext(), m_pwndAttrsPane);
    m_pwndEditPane->SetChild(m_pwndEditCont);

    //
    //  Insert the tabs, and install event handlers on them to keep us informed
    //  where appropriate. We pass them pointers to our event handlers.
    //
    {
        m_pwndTab_Dialogs = new TDialogsTab;
        m_pwndTab_Dialogs->CreateDlgsTab(*this, *m_pwndTabsPane);
    }

    //
    //  Create some window states that we will invoke to set up menus and controls
    //  and such.
    //
    TWndState wstateAdd(m_strState_NoSession);
    wstateAdd.AddMenuEnable(kCIDResEd::ridMenu_File_Open);
    wstateAdd.AddMenuDisable(kCIDResEd::ridMenu_File_Close);
    wstateAdd.AddMenuDisable(kCIDResEd::ridMenu_File_Save);
    wstateAdd.AddMenuDisable(kCIDResEd::ridMenu_New);
    wstateAdd.AddMenuDisable(kCIDResEd::ridMenu_Opts_ShowZOrder);
    AddWndState(wstateAdd);

    wstateAdd.Reset(m_strState_Session);
    wstateAdd.AddMenuDisable(kCIDResEd::ridMenu_File_Open);
    wstateAdd.AddMenuEnable(kCIDResEd::ridMenu_File_Close);
    wstateAdd.AddMenuEnable(kCIDResEd::ridMenu_File_Save);
    wstateAdd.AddMenuEnable(kCIDResEd::ridMenu_New);
    wstateAdd.AddMenuEnable(kCIDResEd::ridMenu_Opts_ShowZOrder);
    AddWndState(wstateAdd);

    m_pwndClient->InitPanes(kCIDLib::False);
    m_pwndAttrsPane->InitPanes(kCIDLib::False);

    return kCIDLib::True;
}


tCIDLib::TVoid
TResEdMainFrame::CodeReceived(  const   tCIDLib::TInt4  i4Code
                                , const tCIDLib::TCard4 c4Data)
{
    facCIDCtrls().ShowGUI(*this);

    TArea areaNew = areaWndSize();
    areaNew.Inflate(1, 1);
    SetSize(areaNew.szArea(), kCIDLib::True);
}


// Clean up the windows we created
tCIDLib::TVoid TResEdMainFrame::Destroyed()
{

    TParent::Destroyed();
}


tCIDLib::TVoid TResEdMainFrame::DragEnd()
{
    TParent::DragEnd();

    //
    //  Force a full redraw of the client, because use of the composited style
    //  on the main client causes various drawing goobers.
    //
    Redraw(areaWndSize(), tCIDCtrls::ERedrawFlags::Full);
}


//
//  React to commands from our menu bar
//
tCIDLib::TVoid
TResEdMainFrame::MenuCommand(const  tCIDLib::TResId     ridItem
                            , const tCIDLib::TBoolean   bChecked
                            , const tCIDLib::TBoolean   bEnabled)
{
    if (ridItem == kCIDResEd::ridMenu_File_Close)
    {
        CloseSession();
    }
     else if (ridItem == kCIDResEd::ridMenu_File_Exit)
    {
        if (bAllowShutdown())
            facCIDCtrls().ExitLoop(0);
    }
     else if (ridItem == kCIDResEd::ridMenu_File_Open)
    {
        OpenSession();
    }
     else if (ridItem == kCIDResEd::ridMenu_File_Save)
    {
        SaveSession();
    }
     else if (ridItem == kCIDResEd::ridMenu_New_Dialog)
    {
        // Get the name of the new dialog
        TString strName;
        TResEdNewDlgDlg dlgNewDlg;
        if (dlgNewDlg.bRunDlg(*this, m_redatCur.colDlgs(), strName))
        {
            tCIDLib::TResId ridNew;
            const tCIDLib::TCard4 c4At = m_redatCur.c4AddDialog(strName, ridNew);

            // Add this guy to our dialogs tab
            m_pwndTab_Dialogs->InsertNew(m_redatCur.reddAt(c4At));
        }
    }
     else if (ridItem == kCIDResEd::ridMenu_Opts_ShowZOrder)
    {
#if 0
        // Get the current state of the menu option and set it to the other
        tCIDLib::TBoolean bCur = !TMenu::bIsItemChecked(hmenuMenuBar(), ridItem);
        TMenu::CheckItem(hmenuMenuBar(), ridItem, bCur);

        // And pass that on to the editor window
        m_pwndEditor->bShowZOrder(bCur);
#endif
    }
     else if (ridItem == kCIDResEd::ridMenu_Help_About)
    {
        // Pop up the about dialog
        // TAboutDlg dlgAbout;
        // dlgAbout.c4RunDlg(*this);
    }
}



// ---------------------------------------------------------------------------
//  TResEdMainFrame: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Called to open a new session. We load up the files and get all the
//  data ready for editing.
//
tCIDLib::TBoolean
TResEdMainFrame::bOpenSession(  const   TString&            strToOpen
                                , const tCIDLib::TBoolean   bNew)
{
    //
    //  Break out the path and the base part of the name of the CIDRC file.
    //  This provides us with the base part of any other files we need to
    //  open, and later to write out.
    //
    TPathStr pathTmp(strToOpen);
    if (!pathTmp.bQueryPath(m_strFacPath)
    ||  !pathTmp.bQueryName(m_strFacBaseName))
    {
        m_strFacPath.Clear();
        m_strFacBaseName.Clear();

        TErrBox msgbErr(facCIDResEd.strMsg(kResEdErrs::errcInt_BadFileName, strToOpen));
        msgbErr.ShowIt(*this);
        return kCIDLib::False;
    }

    try
    {
        //
        //  If a new session, just make sure our data is reset. The path and
        //  base name are already stored above.
        //
        if (bNew)
        {
            m_redatCur.Reset();
        }
         else
        {
            //
            //  See if there's a message text file. If so, then ask the
            //  resource data object to parse the file and fill in its msg
            //  lists.
            //
            pathTmp = m_strFacPath;
            pathTmp.AddLevel(m_strFacBaseName);
            pathTmp.Append(L"_en.MsgText");
            if (TFileSys::bExists(pathTmp))
                m_redatCur.ParseMsgText(pathTmp, kCIDLib::False);

            //
            //  We have to parse the common text messages. They aren't
            //  editable and they don't count as resource data and we never
            //  write them back out. We find them via an environment variable.
            //
            if (TProcEnvironment::bFind(L"CIDLIB_SRCDIR", pathTmp))
            {
                pathTmp.AddLevel(L"Source");
                pathTmp.AddLevel(L"AllProjects");
                pathTmp.AddLevel(L"CIDCommonText_en.MsgText");

                if (TFileSys::bExists(pathTmp))
                {
                    m_redatCur.ParseMsgText(pathTmp, kCIDLib::True);
                }
                 else
                {
                    TErrBox msgbErr(facCIDResEd.strMsg(kResEdErrs::errcSrcFile_NoCIDCommText));
                    msgbErr.ShowIt(*this);
                }
            }
             else
            {
                TErrBox msgbErr(facCIDResEd.strMsg(kResEdErrs::errcSrcFile_NoCIDSrcVar));
                msgbErr.ShowIt(*this);
            }

            // And now let's parse the resource file
            pathTmp = m_strFacPath;
            pathTmp.AddLevel(m_strFacBaseName);
            pathTmp.AppendExt(L"CIDRC");
            if (TFileSys::bExists(pathTmp))
                m_redatCur.ParseResources(pathTmp);

            // Let's now check for errors, such as duplicate ids and such
            if (!m_redatCur.bValidate())
                return kCIDLib::False;

            // Make this the new baseline
            m_redatOrg = m_redatCur;
        }
    }

    catch(const TError& errToCatch)
    {
        // It failed, so reset the data again and display the error
        m_redatOrg.Reset();
        m_redatCur.Reset();
        m_strCurFile.Clear();
        m_strFacPath.Clear();
        m_strFacBaseName.Clear();

        TExceptDlg dlgErr
        (
            *this
            , facCIDResEd.strMsg(kResEdErrs::errcSess_OpenFailed)
            , errToCatch
        );
        return kCIDLib::False;
    }

    // Store the new file name
    m_strCurFile = strToOpen;

    return kCIDLib::True;
}


