//
// FILE NAME: CIDCtrls_DlgBox.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/14/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the dialog class
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
RTTIDecls(TDlgBox, TWindow)




// ---------------------------------------------------------------------------
//  CLASS: TDlgBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TDlgBox: Public, static methods
// ---------------------------------------------------------------------------
TRGBClr TDlgBox::rgbDlgBgn(const tCIDCtrls::EWndThemes eTheme)
{
    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
        return facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DialogBgn);
    else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
        return facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window);

    return facCIDGraphDev().rgbBlack;
}


// ---------------------------------------------------------------------------
// TDlgBox: Constructor and Destructor
// ---------------------------------------------------------------------------

//
//  Indicate to TWindow we do not want to use the default dialog proc, since dlg
//  box handlers don't call a default.
//
TDlgBox::TDlgBox() :

    TWindow(kCIDLib::False)
    , m_bAutoSize(kCIDLib::False)
    , m_pdlgdCreate(0)
{
}

TDlgBox::~TDlgBox()
{
}


// ---------------------------------------------------------------------------
// TDlgBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Run a dialog from a dialog resource passed to us
tCIDLib::TCard4
TDlgBox::c4RunDlg(  const   TWindow&    wndOwner
                    , const TDlgDesc&   dlgdCreate)
{
    // Store stuff some so it's availble in Created()
    m_pdlgdCreate = &dlgdCreate;

    // Pass it on to our parent window
    return c4CreateDlgBox(wndOwner, dlgdCreate);
}


//
//  Run a dialog from a dialog resource that we load up, given the facility and
//  id.
//
tCIDLib::TCard4
TDlgBox::c4RunDlg(  const   TWindow&        wndOwner
                    , const TGUIFacility&   facSrc
                    , const tCIDLib::TResId ridDlg)
{
    // Call the other version with a default zero origin
    return c4RunDlg(wndOwner, facSrc, ridDlg, TPoint::pntOrigin);
}

tCIDLib::TCard4
TDlgBox::c4RunDlg(  const   TWindow&        wndOwner
                    , const TGUIFacility&   facSrc
                    , const tCIDLib::TResId ridDlg
                    , const TPoint&         pntOrg)
{
    TDlgDesc dlgdCreate;
    if (!facSrc.bCreateDlgDesc(ridDlg, dlgdCreate))
    {

        return kCIDLib::c4MaxCard;
    }

    //
    //  Update the origin to match the caller's request. They have to have set the
    //  'use origin' flag in the dialog resource or this won't have any effect.
    //
    dlgdCreate.SetOrigin(pntOrg);

    // Store stuff some so it's availble in Created()
    m_pdlgdCreate = &dlgdCreate;

    // Pass it on to our parent window
    return c4CreateDlgBox(wndOwner, dlgdCreate);
}


//
//  Does a force of this window to the front, as apposed to the polite version
//  which will just flash the task bar if we aren't the foreground app.
//
tCIDLib::TVoid TDlgBox::ForceToFront()
{
    facCIDCtrls().SetForeground(hwndThis(), kCIDLib::True);
}


// Set the minimum size that this frame window can be sized by dragging
tCIDLib::TVoid TDlgBox::SetMinSize(const TSize& szToSet)
{
    m_szMinimum = szToSet;
}


// ---------------------------------------------------------------------------
// TDlgBox: Protected, virtual methods
// ---------------------------------------------------------------------------

// If not overridden, we just return true to allow the dialog to be cancelled
tCIDLib::TBoolean TDlgBox::bCancelRequest()
{
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
// TDlgBox: Protected, inherited methods
// ---------------------------------------------------------------------------

// We override this to handle auto-sizing child controls if asked to
tCIDLib::TVoid
TDlgBox::AreaChanged(const  TArea&                  areaPrev
                    , const TArea&                  areaNew
                    , const tCIDCtrls::EPosStates   ePosState
                    , const tCIDLib::TBoolean       bOrgChanged
                    , const tCIDLib::TBoolean       bSizeChanged
                    , const tCIDLib::TBoolean       bStateChanged)
{
    // Call our parent first
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // If our size changed, then auto-resize any children that have anchors set
    if (m_bAutoSize
    && (ePosState != tCIDCtrls::EPosStates::Minimized)
    &&  bSizeChanged)
    {
        AutoAdjustChildren(areaPrev, areaNew);
    }
}


//
//  We will use the dialog description to create all of the child controls. We return
//  true if we set the focus, else false.
//
tCIDLib::TBoolean TDlgBox::bCreated()
{
    TParent::bCreated();

    tCIDCtrls::TWndHandle hwndUs = hwndThis();

    //
    //  Start with the dialog content, which is already in window units, and get
    //  the frame size that would fit this content.
    //
    TArea areaDlg;
    TFrameWnd::AreaForClient
    (
        m_pdlgdCreate->areaPos()
        , areaDlg
        , tCIDCtrls::EWndStyles(::GetWindowLong(hwndUs, GWL_STYLE))
        , tCIDCtrls::EExWndStyles(::GetWindowLong(hwndUs, GWL_EXSTYLE))
        , kCIDLib::False
    );

    // If not using the origin, then indicate auto-center
    if(!tCIDLib::bAllBitsOn(m_pdlgdCreate->eFlags(), tCIDCtrls::EDlgFlags::UseOrigin))
    {
        TArea areaMon;
        facCIDCtrls().QueryMonArea(*this, areaMon, kCIDLib::True);
        areaDlg.CenterIn(areaMon);
    }

    // Set this as our default minimum size
    m_szMinimum = areaDlg.szArea();

    // And now set us to the final position
    SetSizePos(areaDlg, kCIDLib::False);

    //
    //  Set our background color appropriately. This insures that all of the
    //  children also pick up our background color.
    //
    SetBgnColor(rgbDlgBgn(m_pdlgdCreate->eTheme()));

    // Call a helper to cerate all of the dialog units
    tCIDCtrls::TWndId widInitFocus = 0;
    CreateDlgItems(*m_pdlgdCreate, widInitFocus);

    // If the sizeable flag is on, set the auto-size member
    if (tCIDLib::bAllBitsOn(m_pdlgdCreate->eFlags(), tCIDCtrls::EDlgFlags::Sizeable))
        m_bAutoSize = kCIDLib::True;

    //
    //  If we got an initial focus, we try to set the focus and return false if we
    //  do, to suppress the default focus setting. Else we return true to let the
    //  default happen.
    //
    if (widInitFocus)
    {
        TWindow* pwndFocus = pwndChildById(widInitFocus, kCIDLib::False);
        if (pwndFocus)
        {
            pwndFocus->TakeFocus();
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


//
//  Override some raw messages that we can eat if needed, so that they don't go to the
//  base class' msg dispatcher.
//
tCIDLib::TBoolean
TDlgBox::bEatSubClassMsg(const  tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresRet)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    switch(wmsgMsg)
    {
        case WM_GETMINMAXINFO :
        {
            // We can control minimum size we can be adjusted to
            if (!m_szMinimum.bAllZero())
            {
                MINMAXINFO* pInfo = reinterpret_cast<MINMAXINFO*>(lParam);

                if (m_szMinimum.c4Width())
                    pInfo->ptMinTrackSize.x = int(m_szMinimum.c4Width());

                if (m_szMinimum.c4Width())
                    pInfo->ptMinTrackSize.y = int(m_szMinimum.c4Height());

                bRet = kCIDLib::True;
                mresRet = 0;
            }
            break;
        }

        case WM_CLOSE :
        {
            //
            //  Tell the derived class there's been a cancel request. If he returns
            //  true, then let it propogate to the system handler which will cancel
            //  the dialog with the standard cancel code.
            //
            if (!bCancelRequest())
            {
                bRet = kCIDLib::True;
                mresRet = 1;
            }
            break;
        }

        default :
            break;
    };
    return bRet;
}


//
//  We are not custom windows, and our window text is stored on the window itself,
//  but we aren't derived from TStdCtrlWnd, so we have to handle getting/storing
//  window text.
//
tCIDLib::TVoid TDlgBox::WndTextPreQuery(TString& strToFill) const
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
tCIDLib::TVoid TDlgBox::WndTextStore(const TString& strToStore)
{
    ::SendMessage
    (
        hwndSafe(), WM_SETTEXT, 0, tCIDCtrls::TLParam(strToStore.pszBuffer())
    );
}


// ---------------------------------------------------------------------------
// TDlgBox: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Add the passed window state to our list of states
tCIDLib::TVoid TDlgBox::AddWndState(const TWndState& wstateToAdd)
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


// If we have the named window state, invoke it
tCIDLib::TVoid TDlgBox::ApplyWndState(const TString& strToSet)
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

    m_colWndStates[c4Index].ApplyState(*this, nullptr);
}


// Look up the indicated window state name and return the index, max card if not
tCIDLib::TCard4 TDlgBox::c4FindWndState(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colWndStates.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colWndStates[c4Index].strName() == strToFind)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}



// If we have the inicted window state, delete it
tCIDLib::TVoid TDlgBox::DelWindowState(const TString& strToRemove)
{
    const tCIDLib::TCard4 c4Index = c4FindWndState(strToRemove);
    if (c4Index != kCIDLib::c4MaxCard)
        m_colWndStates.RemoveAt(c4Index);
}


//
//  Breaks us out of the dialog's modal loop and returns the passed code back from
//  the c4RunDlg() call above.
//
tCIDLib::TVoid TDlgBox::EndDlg(const tCIDLib::TCard4 c4Code)
{
    ::EndDialog(hwndSafe(), c4Code);
}


//
//  Set or clear the auto-size flag. They can set this via the dialog description
//  but if they create one manually they can call this to set it.
//
tCIDLib::TVoid TDlgBox::SetAutoSize(const tCIDLib::TBoolean bToSet)
{
    m_bAutoSize = bToSet;
}
