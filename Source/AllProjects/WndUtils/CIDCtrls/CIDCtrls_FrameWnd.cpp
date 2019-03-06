//
// FILE NAME: CIDCtrls_FrameWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the
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
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFrameWnd, TWindow)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDCtrls_FrameWnd
{
    static const tCIDLib::TCh* pszClass = L"CIDCtrlsStdFrame";
    static const tCIDLib::TCh* pszClassHV = L"CIDCtrlsStdHVFrame";
}



// ---------------------------------------------------------------------------
//  CLASS: TFrameWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TFrameWnd: Public, static methods
// ---------------------------------------------------------------------------

// Calculate a frame of appropriate size to hold the indicated client area
tCIDLib::TVoid
TFrameWnd::AreaForClient(const  TArea&                  areaClient
                        ,       TArea&                  areaNew
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EExWndStyles eExStyles
                        , const tCIDLib::TBoolean       bHasMenu)
{
    RECT rectClient;
    areaClient.ToRectl(*(tCIDLib::THostRectl*)&rectClient, tCIDLib::ERectlTypes::NonInclusive);
    ::AdjustWindowRectEx
    (
        &rectClient, tCIDLib::c4EnumOrd(eStyles), bHasMenu, tCIDLib::c4EnumOrd(eExStyles)
    );

    // And convert back to the output area
    areaNew.FromRectl(*(tCIDLib::THostRectl*)&rectClient, tCIDLib::ERectlTypes::NonInclusive);
}



// ---------------------------------------------------------------------------
// TFrameWnd: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  Tell TWindow to call the default dialog msg processing function since we want
//  to handle focus.
//
TFrameWnd::TFrameWnd() :

    TWindow(kCIDLib::True)
    , m_bActive(kCIDLib::False)
    , m_bFSMode(kCIDLib::False)
    , m_c4FSStyles(0)
    , m_c4FSExStyles(0)
    , m_colWndStates(32)
    , m_hwndLastFocus(kCIDCtrls::hwndInvalid)
    , m_pmenuBar(nullptr)
    , m_pwndStatusBar(nullptr)
    , m_widNext(kCIDCtrls::widFirstCtrl)
{
}

TFrameWnd::~TFrameWnd()
{
}


// ---------------------------------------------------------------------------
//  TFrameWnd: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  Called by (directly or indirectly) by the main message loop to see if an
//  incoming message is an accelerator. If a menu has been set on us, we created
//  an accellerator table for it (if it had any hot keys.) If not, we just say no.
//  If we never created the table due to no hot keys, it will also say no.
//
//  Derived classes can override this and pass this on to child windows it creates.
//
tCIDLib::TBoolean
TFrameWnd::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    if (m_pmenuBar)
        return m_accelMenu.bProcessAccel(*this, pMsgData);

    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
// TFrameWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TFrameWnd::Activate()
{
    // We don't check the error. There are legitimate reasons this could fail
    ::SetActiveWindow(hwndSafe());
}


// Add the passed window state to our list of states
tCIDLib::TVoid TFrameWnd::AddWndState(const TWndState& wstateToAdd)
{
    // Make sure it's not already in our list
    if (c4FindWndState(wstateToAdd.strName()) != kCIDLib::c4MaxCard)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWState_AlreadyExists
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , wstateToAdd.strName()
        );
    }

    m_colWndStates.objAdd(wstateToAdd);
}


//
//  Returns true if this window owns any active popups. We can optionaly only
//  loop for modal ones, which is often desirable, since secondary top level
//  windows aren't a problem in many cases.
//
tCIDLib::TBoolean
TFrameWnd::bAnyActivePopup(const tCIDLib::TBoolean bModalOnly) const
{
    return facCIDCtrls().bOwnsActivePopups(hwndThis(), bModalOnly);
}


// If we have the named window state, invoke it
tCIDLib::TVoid TFrameWnd::ApplyWndState(const TString& strToSet)
{
    const tCIDLib::TCard4 c4Index = c4FindWndState(strToSet);
    if (c4Index == kCIDLib::c4MaxCard)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWState_NotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strToSet
        );
    }

    m_colWndStates[c4Index].ApplyState(*this, m_pmenuBar);
}


// Returns our fulls creen mode flag
tCIDLib::TBoolean TFrameWnd::bInFSMode() const
{
    return m_bFSMode;
}


// Just returns the most recent activation state we were sent
tCIDLib::TBoolean TFrameWnd::bIsActive() const
{
    return m_bActive;
}


// Returns true if this window has an owner
tCIDLib::TBoolean TFrameWnd::bIsOwned() const
{
    return (::GetWindow(hwndSafe(), GW_OWNER) != 0);
}


// Returns true if the frame window is maximized
tCIDLib::TBoolean TFrameWnd::bIsMaximized() const
{
    // Get the window placement info
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);
    return (WndPlace.showCmd == SW_SHOWMAXIMIZED);
}


tCIDLib::TBoolean TFrameWnd::bIsMinimized() const
{
    // Get the window placement info
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);
    return (WndPlace.showCmd == SW_SHOWMINIMIZED);
}


tCIDLib::TBoolean TFrameWnd::bIsRestored() const
{
    // Get the window placement info
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);
    return (WndPlace.showCmd == SW_SHOWNORMAL);
}


//
//  Creates a new standard frame window class, that can be used as a top level
//  window. It will almost always be derived from in order to do extra work, such
//  as create child windows, load data into them, etc...
//
//  And we have a second variation that creates us with a status bar and assumes
//  up front a menu and never HVRedraw.
//
tCIDLib::TVoid
TFrameWnd::CreateFrame( const   TWindow* const          pwndOwner
                        , const TArea&                  areaInit
                        , const TString&                strText
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EExWndStyles eExStyles
                        , const tCIDCtrls::EFrameStyles eFrameStyles
                        , const tCIDLib::TBoolean       bHVRedraw)
{
    // Do our one time init if not already done
    DoInit();

    // Store our class specific styles away
    m_eFrameStyles = eFrameStyles;

    //
    //  Expand on the window styles because some of our frame styles are really
    //  window styles at the windows level.
    //
    tCIDLib::TCard4 c4OtherStyles = 0;
    if (tCIDLib::bAllBitsOn(eFrameStyles, tCIDCtrls::EFrameStyles::MinButton))
        c4OtherStyles |= WS_MINIMIZEBOX;
    if (tCIDLib::bAllBitsOn(eFrameStyles, tCIDCtrls::EFrameStyles::MaxButton))
        c4OtherStyles |= WS_MAXIMIZEBOX;
    if (tCIDLib::bAllBitsOn(eFrameStyles, tCIDCtrls::EFrameStyles::SysMenu))
        c4OtherStyles |= WS_SYSMENU;

    CreateWnd
    (
        pwndOwner ? pwndOwner->hwndSafe() : HWND_DESKTOP
        , bHVRedraw ? CIDCtrls_FrameWnd::pszClassHV : CIDCtrls_FrameWnd::pszClass
        , strText.pszBuffer()
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4OtherStyles)
        , eExStyles
        , 0
    );

    // If it's a no close frame, then disable the close button
    if (tCIDLib::bAllBitsOn(eFrameStyles, tCIDCtrls::EFrameStyles::NoClose))
    {
        ::EnableMenuItem
        (
            ::GetSystemMenu(hwndThis(), FALSE)
            , SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
        );
    }
}


tCIDLib::TVoid
TFrameWnd::CreateFrame( const   TWindow* const      pwndOwner
                        , const TArea&              areaInit
                        , const TString&            strText
                        , const tCIDLib::TCardList& fcolStatusBarSlots
                        , const TGUIFacility&       facSrc
                        , const tCIDLib::TResId     ridMenu)
{
    // Do our one time init if not already done
    DoInit();

    //
    //  Force the creation of a status bar, and set some others that ultimately are not
    //  'frame styles' at the Windows level but we treat them as such and are forcing them
    //  on below. We need to set them here in case the containing application checks for
    //  them.
    //
    m_eFrameStyles = tCIDCtrls::EFrameStyles::StdFrame | tCIDCtrls::EFrameStyles::StatusBar;
    CreateWnd
    (
        pwndOwner ? pwndOwner->hwndSafe() : HWND_DESKTOP
        , CIDCtrls_FrameWnd::pszClass
        , strText.pszBuffer()
        , areaInit
        , tCIDCtrls::EWndStyles
          (
              tCIDLib::TCard4(tCIDCtrls::EWndStyles::StdFrameNV)
              | WS_CAPTION
              | WS_MINIMIZEBOX
              | WS_MAXIMIZEBOX
              | WS_SYSMENU
              | WS_CLIPCHILDREN
          )
        , tCIDCtrls::EExWndStyles::None
        , 0
    );

    // Set the menu
    SetMenuBar(facSrc, ridMenu);

    // Set up the status bar slots. We assume pixel sizes, not percentages
    m_pwndStatusBar->SetAreas(fcolStatusBarSlots, kCIDLib::False);
}



// If we have the inicted window state, delete it
tCIDLib::TVoid TFrameWnd::DelWindowState(const TString& strToRemove)
{
    const tCIDLib::TCard4 c4Index = c4FindWndState(strToRemove);
    if (c4Index != kCIDLib::c4MaxCard)
        m_colWndStates.RemoveAt(c4Index);
}


// Proivde access to our frame styles
tCIDCtrls::EFrameStyles TFrameWnd::eFrameStyles() const
{
    return m_eFrameStyles;
}


//
//  These operations MUST be done in this order to get the correct effect!
//
tCIDLib::TVoid TFrameWnd::ExitFullScreenMode()
{
    // If not in full screen mode, then do nothing
    if (!m_bFSMode)
        return;

    // Remember that we are now in back out of full screen
    m_bFSMode = kCIDLib::False;

    // Restore our previous position
    SetSizePos(m_areaPreFS, kCIDLib::False);

    // Put back the window styles we saved
    ::SetWindowLong(hwndThis(), GWL_STYLE, m_c4FSStyles);
    ::SetWindowLong(hwndThis(), GWL_EXSTYLE, m_c4FSExStyles);

    // Put our menu handle, if any, back
    if (m_pmenuBar)
        RestoreMenuBar();
}


//
//  Does a force of this window to the front, as apposed to the polite version
//  which will just flash the task bar if we aren't the foreground app.
//
tCIDLib::TVoid TFrameWnd::ForceToFront()
{
    facCIDCtrls().SetForeground(hwndThis(), kCIDLib::True);
}


// Take us into full screen mode
tCIDLib::TVoid TFrameWnd::FullScreenMode()
{
    // If already in full screen mode, do nothing
    if (m_bFSMode)
        return;

    // Remember that we are now in full screen
    m_bFSMode = kCIDLib::True;

    // If we have a menu bar, disassociate it from us
    if (m_pmenuBar)
        m_pmenuBar->Disassociate();

    // Remember our current area for later restoral
    m_areaPreFS = areaWnd();

    // Save our current window styles
    m_c4FSStyles = ::GetWindowLong(hwndThis(), GWL_STYLE);
    m_c4FSExStyles = ::GetWindowLong(hwndThis(), GWL_EXSTYLE);

    // And get rid of the system frame's ornamentation styles
    tCIDLib::TCard4 c4Styles = m_c4FSStyles;
    c4Styles &= ~
    (
        WS_MAXIMIZEBOX
        | WS_MINIMIZEBOX
        | WS_SYSMENU
        | WS_CAPTION
        | WS_THICKFRAME
        | WS_POPUP
    );
    ::SetWindowLong(hwndThis(), GWL_STYLE, c4Styles);

    // Make sure top-most is on
    tCIDLib::TCard4 c4ExStyles = m_c4FSExStyles | WS_EX_TOPMOST;
    ::SetWindowLong(hwndThis(), GWL_EXSTYLE, c4ExStyles);

    // And size us up to fit the monitor we are on, asking for all available monitor size
    TArea areaNew;
    facCIDCtrls().QueryMonArea(*this, areaNew, kCIDLib::False);
    SetSizePos(areaNew, kCIDLib::True);
}


// Maxmimize us not already and the derived class allows it
tCIDLib::TVoid TFrameWnd::Maximize()
{
    // If already maximized, then do nothing
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);
    if (WndPlace.showCmd == SW_SHOWMAXIMIZED)
        return;

    // Ask the derived class if we can do it. If no, then don't
    if (!bMaximizing())
        return;

    // Store the new state and then maximize us
    WndPlace.showCmd = SW_MAXIMIZE;
    ::SetWindowPlacement(hwndThis(), &WndPlace);
}


// Minimize us not already and the derived class allows it
tCIDLib::TVoid TFrameWnd::Minimize()
{
    // If already minimized, then do nothing
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);
    if (WndPlace.showCmd == SW_SHOWMINIMIZED)
        return;

    // Ask the derived class if we can do it. If no, then don't
    if (!bMinimizing())
        return;

    // And do the minimize
    WndPlace.showCmd = SW_MINIMIZE;
    ::SetWindowPlacement(hwndThis(), &WndPlace);
}


// Provide access to the currently set menu bar, if any
const TMenuBar& TFrameWnd::menuCur() const
{
    if (!m_pmenuBar)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcFrame_NoMenuBar
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return *m_pmenuBar;
}

TMenuBar& TFrameWnd::menuCur()
{
    if (!m_pmenuBar)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcFrame_NoMenuBar
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return *m_pmenuBar;
}

const TMenuBar* TFrameWnd::pmenuCur() const
{
    return m_pmenuBar;
}

TMenuBar* TFrameWnd::pmenuCur()
{
    return m_pmenuBar;
}


//
//  If we have a current menu, then destroy it and close our accellerator table,
//  which is driven by the menu.
//
tCIDLib::TVoid TFrameWnd::RemoveMenu()
{
    if (m_pmenuBar)
    {
        // Disassociate it from us
        ::SetMenu(hwndThis(), nullptr);

        // And destroy the menu itself
        try
        {
            delete m_pmenuBar;
            m_pmenuBar = nullptr;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }

        // Clean up any accellerator table
        m_accelMenu.Close();
    }
}


//
//  If we have a menu bar and it's not already set on us, then set it. This allows
//  the menu to be hidden and restred without losing its context.
//
tCIDLib::TVoid TFrameWnd::RestoreMenuBar()
{
    if (m_pmenuBar)
    {
        m_pmenuBar->AssignToWnd(this);
        m_pmenuBar->Redraw();
    }
}


// Set us back to restored if not already and the derived class allows it
tCIDLib::TVoid TFrameWnd::Restore()
{
    // If already restored, then do nothing
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);
    if (WndPlace.showCmd == SW_SHOWNORMAL)
        return;

    // Ask the derived class if we can do it. If no, then don't
    if (!bRestoring())
        return;

    // Ok, now do the actual restore
    WndPlace.showCmd = SW_RESTORE;
    ::SetWindowPlacement(hwndThis(), &WndPlace);
}


// Used to restore position state gotten by way of ePosState(TArea&)
tCIDLib::TVoid
TFrameWnd::RestorePosState( const   tCIDCtrls::EPosStates   eState
                            , const TArea&                  areaNormal
                            , const tCIDLib::TBoolean       bShow)
{
    // Ask the derived class if we can do it. If no, then don't
    if (!bRestoring())
        return;

    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);

    if (eState == tCIDCtrls::EPosStates::Maximized)
    {
        if (bShow)
            WndPlace.showCmd = SW_SHOWMAXIMIZED;
        else
            WndPlace.showCmd = SW_MAXIMIZE;
    }
     else if (eState == tCIDCtrls::EPosStates::Minimized)
    {
        if (bShow)
            WndPlace.showCmd = SW_SHOWMINIMIZED;
        else
            WndPlace.showCmd = SW_MINIMIZE;
    }
     else
    {
        if (bShow)
           WndPlace.showCmd = SW_SHOWNORMAL;
        else
           WndPlace.showCmd = SW_RESTORE;
    }

    areaNormal.ToRectl
    (
        *(tCIDLib::THostRectl*)&WndPlace.rcNormalPosition
        , tCIDLib::ERectlTypes::NonInclusive
    );
    ::SetWindowPlacement(hwndThis(), &WndPlace);
}

tCIDLib::TVoid
TFrameWnd::RestorePosState( const   TArea&              areaTo
                            , const tCIDLib::TBoolean   bShow)
{
    // Ask the derived class if we can do it. If no, then don't
    if (!bRestoring())
        return;

    // If already restored, then do nothing
    WINDOWPLACEMENT WndPlace = {0};
    WndPlace.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(hwndThis(), &WndPlace);

    // Put the passed area in as the restore area
    areaTo.ToRectl
    (
        *(tCIDLib::THostRectl*)&WndPlace.rcNormalPosition
        , tCIDLib::ERectlTypes::NonInclusive
    );

    // Ok, now do the actual restore
    if (bShow)
        WndPlace.showCmd = SW_SHOWNORMAL;
    else
        WndPlace.showCmd = SW_RESTORE;
    ::SetWindowPlacement(hwndThis(), &WndPlace);
}


//
//  The caller gives us a facility and resource id for a menu. We load it up and
//  create a menu bar object from it. We also create an accelerator table for it
//  if it has any hot keys.
//
tCIDLib::TVoid
TFrameWnd::SetMenuBar(  const   TGUIFacility&       facSrc
                        , const tCIDLib::TResId     ridMenu)
{
    // If we have a current menu, then destroy it
    RemoveMenu();

    // Create a new one and try to load it from the resource
    TMenuBar* pmenuBar = new TMenuBar();
    TJanitor<TMenuBar> janMenu(pmenuBar);
    pmenuBar->Create(facSrc, ridMenu);

    //
    //  Try to associate the new one with us. It will throw if this menu is
    //  already assigned, but we checked for that above already.
    //
    pmenuBar->AssignToWnd(this);

    // Try to create an accelerator table for it
    m_accelMenu.Create(*pmenuBar);

    // It worked, so store it, orphaning it out of the janitor
    m_pmenuBar = janMenu.pobjOrphan();
}


// Set the min/man size that this frame window can be sized to by dragging
tCIDLib::TVoid
TFrameWnd::SetMinMaxSize(const TSize& szMin, const TSize& szMax)
{
    m_szMaximum = szMax;
    m_szMinimum = szMin;
}


//
//  Let's the client set up the status bar if he requested one be created. It's
//  just a passthrough to the status bar.
//
tCIDLib::TVoid
TFrameWnd::SetStatusBarAreas(const tCIDLib::TCardList&  fcolValues
                            , const tCIDLib::TBoolean   bPercents)
{
    if (m_pwndStatusBar)
        m_pwndStatusBar->SetAreas(fcolValues, bPercents);
}


// Set the text of one of our status bar slots
tCIDLib::TVoid
TFrameWnd::SetStatusBarText(const tCIDLib::TCard4 c4Index, const TString& strText)
{
    //
    //  If the status bar is valid, try it. It may still fail if the index is
    //  not valid.
    //
    if (m_pwndStatusBar && m_pwndStatusBar->bIsValid())
        m_pwndStatusBar->SetAreaText(c4Index, strText);
}


//
//
//  Adjust our size to fit the indicated client area, given the styesl we have.
//
tCIDLib::TVoid TFrameWnd::SizeToClient(const TArea& areaFitTo)
{
    RECT rectClient;
    areaFitTo.ToRectl
    (
        *(tCIDLib::THostRectl*)&rectClient
        , tCIDLib::ERectlTypes::NonInclusive
    );

    tCIDCtrls::TWndHandle hwndMe = hwndSafe();
    ::AdjustWindowRectEx
    (
        &rectClient
        , ::GetWindowLong(hwndMe, GWL_STYLE)
        , ::GetMenu(hwndMe) != 0
        , ::GetWindowLong(hwndMe, GWL_EXSTYLE)
    );

    // And convert back to the output area
    TArea areaNew;
    areaNew.FromRectl
    (
        *(tCIDLib::THostRectl*)&rectClient
        , tCIDLib::ERectlTypes::NonInclusive
    );
    SetSize(areaNew.szArea(), kCIDLib::False);
}


// ---------------------------------------------------------------------------
// TFrameWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  If our size changes and we are restored, then update the size of the
//  client window
//
tCIDLib::TVoid
TFrameWnd::AreaChanged( const   TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
    {
        // If we have a status bar, tell it we changed size
        if (m_pwndStatusBar)
            m_pwndStatusBar->ParentSizeChanged();
    }
}


tCIDLib::TBoolean TFrameWnd::bCreated()
{
    if (!TParent::bCreated())
        return kCIDLib::False;

    if (tCIDLib::bAllBitsOn(m_eFrameStyles, tCIDCtrls::EFrameStyles::StatusBar))
    {
        m_pwndStatusBar = new TStatusBar();
        m_pwndStatusBar->Create(*this, widNext());
    }
    return kCIDLib::True;
}


// We need to clean up any currently set menu bar
tCIDLib::TVoid TFrameWnd::Destroyed()
{
    if (m_pmenuBar)
    {
        try
        {
            delete m_pmenuBar;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        m_pmenuBar = nullptr;

        // Close the associated accelerator table
        try
        {
            m_accelMenu.Close();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    if (m_pwndStatusBar)
    {
        try
        {
            m_pwndStatusBar->Destroy();
            delete m_pwndStatusBar;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        m_pwndStatusBar = nullptr;
    }

    TParent::Destroyed();
}


// If we had a focus child when we lost activation, put it back
tCIDLib::TVoid TFrameWnd::GettingFocus()
{
    TParent::GettingFocus();

    RestoreLastFocus();
}


tCIDCtrls::TMsgResult
TFrameWnd::mresDispatch(const   TWindow&            wndThis
                        , const tCIDCtrls::TWndMsg  wmsgToDo
                        , const tCIDCtrls::TWParam  wParam
                        , const tCIDCtrls::TLParam  lParam)
{
    switch(wmsgToDo)
    {
        case WM_ACTIVATE :
        {
            // Remember our current activation state
            m_bActive = (wParam & 0xFFFF) != WA_INACTIVE;
            try
            {
                ActivationChange(m_bActive, (HWND)lParam);
            }

            catch(TError& errToCatch)
            {
                if (facCIDCtrls().bShouldLog(errToCatch))
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);
                }

                facCIDCtrls().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcFrame_ActivationErr
                    , errToCatch.strErrText()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppError
                    , TString(m_bActive ? L"Yes" : L"No")
                );
            }

            catch(...)
            {
                facCIDCtrls().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcFrame_ActivationErr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppError
                    , TString(m_bActive ? L"Yes" : L"No")
                );
            }
            break;
        }

        case WM_GETMINMAXINFO :
        {
            MINMAXINFO* pInfo = reinterpret_cast<MINMAXINFO*>(lParam);

            // Set any non-zero sizes
            if (m_szMinimum.c4Width())
                pInfo->ptMinTrackSize.x = int(m_szMinimum.c4Width());
            if (m_szMinimum.c4Height())
                pInfo->ptMinTrackSize.y = int(m_szMinimum.c4Height());

            if (m_szMaximum.c4Width())
            {
                pInfo->ptMaxTrackSize.x = int(m_szMaximum.c4Width());
                pInfo->ptMaxSize.x = int(m_szMaximum.c4Width());
            }
            if (m_szMaximum.c4Height())
            {
                pInfo->ptMaxTrackSize.y = int(m_szMaximum.c4Height());
                pInfo->ptMaxSize.y = int(m_szMaximum.c4Height());
            }
            break;
        }

        case WM_SYSCOMMAND :
        {
            if (wParam == SC_CLOSE)
            {
                //
                //  Ask the derived class (if any) if it's ok to shut down. If
                //  so then do it. If it's an unowned window, then we assume it's the
                //  top level and do a message loop exit. Else, we just destroy our
                //  own window.
                //
                if (bAllowShutdown())
                {
                    if (bIsOwned())
                        Destroy();
                    else
                        facCIDCtrls().ExitLoop(0);
                }
            }
             else
            {
                return TParent::mresDispatch(wndThis, wmsgToDo, wParam, lParam);
            }
            break;
        }

        default :
            // Pass it on to our parent class
            return TParent::mresDispatch(wndThis, wmsgToDo, wParam, lParam);
    };

    return 0;
}


//
//  We are not custom windows, and our window text is stored on the window itself,
//  but we aren't derived from TStdCtrlWnd, so we have to handle getting/storing
//  window text.
//
tCIDLib::TVoid TFrameWnd::WndTextPreQuery(TString& strToFill) const
{
    tCIDCtrls::TMsgResult mresRet;
    mresRet = ::SendMessage(hwndSafe(), WM_GETTEXTLENGTH, 0, 0);
    if (mresRet)
    {
        tCIDLib::TCh* pszBuf = new tCIDLib::TCh[mresRet + 2];
        pszBuf[0] = kCIDLib::chNull;
        TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);

        mresRet = ::SendMessage
        (
            hwndSafe(), WM_GETTEXT, mresRet + 1, tCIDCtrls::TLParam(pszBuf)
        );
        strToFill.FromZStr(pszBuf, mresRet);
    }
     else
    {
        strToFill.Clear();
    }
}


//
//  For standard controls, the text hsa to be stored in the window text.
//
tCIDLib::TVoid TFrameWnd::WndTextStore(const TString& strToStore)
{
    ::SendMessage
    (
        hwndSafe(), WM_SETTEXT, 0, tCIDCtrls::TLParam(strToStore.pszBuffer())
    );
}



// ---------------------------------------------------------------------------
// TFrameWnd: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  Derived classes typically would want to call our version since we handle
//  getting the focus back to the window that last had it when we lost activation.
//
tCIDLib::TVoid
TFrameWnd::ActivationChange(const tCIDLib::TBoolean bNewState, const tCIDCtrls::TWndHandle)
{
    //
    //  If activiating, try to put the focus back on the last widget that
    //  had it.
    //
    if (bNewState)
        RestoreLastFocus();
    else
        m_hwndLastFocus = facCIDCtrls().hwndChildWithFocus(hwndThis());
}


// The default is say yes, allow it
tCIDLib::TBoolean TFrameWnd::bAllowShutdown()
{
    return kCIDLib::True;
}


tCIDLib::TBoolean TFrameWnd::bMaximizing()
{
    // Default empty implementation, says go ahead and do it
    return kCIDLib::True;
}


tCIDLib::TBoolean TFrameWnd::bMinimizing()
{
    // Default empty implementation, says go ahead and do it
    return kCIDLib::True;
}


tCIDLib::TBoolean TFrameWnd::bRestoring()
{
    // Default empty implementation, says go ahead and do it
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
// TFrameWnd: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Return the next available child id
tCIDCtrls::TWndId TFrameWnd::widNext()
{
    return m_widNext++;
}



// ---------------------------------------------------------------------------
// TFrameWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Look up the indicated window state name and return the index, max card if not
tCIDLib::TCard4 TFrameWnd::c4FindWndState(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colWndStates.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colWndStates[c4Index].strName() == strToFind)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}


// Hnadle one time initailization
tCIDLib::TVoid TFrameWnd::DoInit()
{
    //
    //  If we haven't registered our frame window class, then do that. We create
    //  two actually, one that does H/V redraw and one that doesn't, since that's
    //  a sort of fundamental requirement of some windows. The caller indicates
    //  which he wants.
    //
    //  NOTE that we don't use the standard custom window classes that TWindow
    //  registers for use by everyone else. We want our own
    //
    static tCIDLib::TBoolean bInitDone = kCIDLib::False;
    if (!bInitDone)
    {
        // Lock while we do this
        TBaseLock lockInit;
        if (!bInitDone)
        {
            const TRGBClr rgbBgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window);
            RegWndClass
            (
                CIDCtrls_FrameWnd::pszClass
                , kCIDLib::False
                , kCIDLib::False
                , 0
                , rgbBgn
                , kCIDLib::True
            );

            RegWndClass
            (
                CIDCtrls_FrameWnd::pszClassHV
                , kCIDLib::True
                , kCIDLib::False
                , 0
                , rgbBgn
                , kCIDLib::True
            );
            bInitDone = kCIDLib::True;
        }
    }
}


tCIDLib::TVoid TFrameWnd::RestoreLastFocus()
{
    //
    //  If there is a last focus stored, see if we can put the focus back
    //  there.
    //
    if (m_hwndLastFocus)
    {
        //
        //  Make sure its not been destroyed. If not, then give it the
        //  focus back. Note that we have to avoid a race condition
        //  here. If we just called TakeFocus() on the window and then
        //  cleared it, we'd zero out the new focus that got set during
        //  the focus change. So we have to use a temp!!
        //
        if (::IsWindow(m_hwndLastFocus))
        {
            tCIDCtrls::TWndHandle hwndTmp = m_hwndLastFocus;
            ::SetFocus(hwndTmp);
        }
         else
        {
            // Its no longer valid so just zero it out
            m_hwndLastFocus = kCIDCtrls::hwndInvalid;
        }
    }
}


