//
// FILE NAME: CIDCtrls_ScrollArea.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/11/2015
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
//  This file implements the scrollable area control.
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
AdvRTTIDecls(TScrollArea, TStdCtrlWnd)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDCtrls_ScrollArea
{
    const tCIDLib::TInt4 i4LnSz = 16;
}


// ---------------------------------------------------------------------------
//  CLASS: TScrollArea
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TScrollArea: Constructors and Destructor
// ---------------------------------------------------------------------------
TScrollArea::TScrollArea() :

    TStdCtrlWnd()
    , m_c4Margin(0)
    , m_eScrAStyles(tCIDCtrls::EScrAStyles::None)
    , m_pwndChild(0)
{
}

TScrollArea::~TScrollArea()
{
}


// ---------------------------------------------------------------------------
// TScrollArea: Public, inherited methods
// ---------------------------------------------------------------------------

// If we have a child, pass on the accel table handler in case he has one
tCIDLib::TBoolean TScrollArea::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    if (m_pwndChild && m_pwndChild->bProcessAccel(pMsgData))
        return kCIDLib::True;

    return kCIDLib::False;
}


tCIDLib::TVoid
TScrollArea::InitFromDesc(  const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EScrAStyles eScrAStyles = tCIDCtrls::EScrAStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        // Set a border unless asked not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles |= tCIDCtrls::EWndStyles::Border;
    }
     else if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a border unless asked not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles |= tCIDCtrls::EExWndStyles::StaticEdge;
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eScrAStyles
        , eExStyles
    );
}


tCIDLib::TVoid TScrollArea::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

}


// ---------------------------------------------------------------------------
// TScrollArea: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Get the current size of the child window being managed. If none, we throw. It
//  really returns the size, since it's at zero origin relative to us.
//
TArea TScrollArea::areaGetChildSize() const
{
    CheckHaveChild(CID_LINE);
    return m_pwndChild->areaWndSize();
}


tCIDLib::TBoolean TScrollArea::bAtBottom(const tCIDLib::TBoolean bHorz) const
{
    return (c4CurScrollPos(bHorz) == c4MaxScrollPos(bHorz));
}


tCIDLib::TBoolean TScrollArea::bAtTop(const tCIDLib::TBoolean bHorz) const
{
    return (c4CurScrollPos(bHorz) == 0);
}


// Get or set a margin that will be used to indent the child window inwards.
tCIDLib::TCard4 TScrollArea::c4Margin() const
{
    return m_c4Margin;
}

tCIDLib::TCard4 TScrollArea::c4Margin(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Margin = c4ToSet;
    return m_c4Margin;
}


// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TScrollArea::Create(const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EScrAStyles  eScrAStyles
                    , const tCIDCtrls::EExWndStyles eExStyles)
{
    // If we haven't registered our window class, then do that
    static const tCIDLib::TCh* pszClass = L"CIDScrollArea";
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
                pszClass
                , kCIDLib::False
                , kCIDLib::False
                , 0
                , rgbBgn
                , kCIDLib::False
            );
            bInitDone = kCIDLib::True;
        }
    }

    // Store away our class specific styles
    m_eScrAStyles = eScrAStyles;

    // Force on the H/V scroll bars, since it makes no sense otherwise
    CreateWnd
    (
        wndParent.hwndSafe()
        , pszClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(WS_HSCROLL | WS_VSCROLL | WS_CLIPCHILDREN)
        , eExStyles | tCIDCtrls::EExWndStyles::ControlParent
        , widThis
    );
}


// Move us to the end of the scrollable area
tCIDLib::TVoid TScrollArea::GoToEnd(const tCIDLib::TBoolean bHorz)
{
    SetScrollPos(bHorz, c4MaxScrollPos(bHorz));
}


// Move us to the start of the scrollable area
tCIDLib::TVoid TScrollArea::GoToStart(const tCIDLib::TBoolean bHorz)
{
    SetScrollPos(bHorz, 0);
}


//
//  Set a new child window. Any existing child is destroyed.
//
tCIDLib::TVoid TScrollArea::SetChild(TWindow* const pwndToAdopt)
{
    // If we hvae a current child, delete it
    if (m_pwndChild)
        CleanupChild();

    // And let's take the new one
    pwndToAdopt->SetParent(*this);
    m_pwndChild = pwndToAdopt;

    // Set the child window's position bsaed on our margin
    TPoint pntOrg(m_c4Margin, m_c4Margin);
    pwndToAdopt->SetPos(pntOrg);

    // Do an initial adjustment of the scroll bars for this new child
    AdjustBars();
}


//
//  We update our contained window's size, which causes us to get a size change
//  notification and we'll adjust the scroll bars.
//
tCIDLib::TVoid TScrollArea::SetChildSize(const TSize& szToSet)
{
    if (!m_pwndChild)
        return;

    m_pwndChild->SetSize(szToSet, kCIDLib::True);
}



// ---------------------------------------------------------------------------
//  TScrollArea: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  If we are in restored state and the size has changed, we update our scroll bars
//  to reflect the new size relationship.
//
tCIDLib::TVoid
TScrollArea::AreaChanged(const  TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
        AdjustBars();
}


// Do it all in the paint, where we are clipped to our children
tCIDLib::TBoolean TScrollArea::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean TScrollArea::bMouseWheel(const tCIDLib::TInt4 i4Clicks)
{
    // We send two next/previous scroll events to ourself for each click
    tCIDCtrls::EScrollEvs eEvent = tCIDCtrls::EScrollEvs::None;
    tCIDLib::TCard4 c4Count;

    if (i4Clicks > 0)
    {
        c4Count = tCIDLib::TCard4(i4Clicks);
        eEvent = tCIDCtrls::EScrollEvs::PrevLn;
    }
     else
    {
        c4Count = tCIDLib::TCard4(i4Clicks * -1);
        eEvent = tCIDCtrls::EScrollEvs::NextLn;
    }

    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            ScrollEvent(eEvent, kCIDLib::False);
            ScrollEvent(eEvent, kCIDLib::False);
        }
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TScrollArea::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    gdevToUse.Fill(areaUpdate, rgbBgnFill());
    return kCIDLib::True;
}


// A child changed size, so update our scroll bars
tCIDLib::TVoid
TScrollArea::ChildAreaChanged(  const   tCIDCtrls::TWndId   widChild
                                , const TArea&              areaNew)
{
    AdjustBars();
}


// Clean up any child we were assigned, then pass it on to our parent
tCIDLib::TVoid TScrollArea::Destroyed()
{
    // Destroy any child window
    CleanupChild();

    TParent::Destroyed();
}


//
//  Handle scroll events and move our client window appropriately, updating the
//  scroll bar to match our calculated position. AreaChanged above keeps the
//  bars updated, so if we get called here, it's because some amount of movement
//  is legal.
//
tCIDLib::TVoid
TScrollArea::ScrollDrag(const   tCIDLib::TBoolean       bHorz
                        , const tCIDLib::TCard4         c4Position
                        , const tCIDLib::TBoolean       bFinal)
{
    if (m_pwndChild)
    {
        // Update the scroll bar
        SetSBarPos(bHorz, c4Position);

        //
        //  We set our controlled window's position to be the negation of the passed
        //  position. The scroll bar positions are in terms of i4LnSz units, so
        //  multiply it to get the actual value.
        //
        TPoint pntNew;
        tCIDLib::TInt4 i4NewPos(c4Position);
        i4NewPos *= CIDCtrls_ScrollArea::i4LnSz;

        if (bHorz)
            pntNew.i4X(i4NewPos * -1);
        else
            pntNew.i4Y(i4NewPos * -1);

        // Add any margin to the calculated position
        pntNew.Adjust(m_c4Margin, m_c4Margin);

        m_pwndChild->SetPos(pntNew);
    }
}


tCIDLib::TVoid
TScrollArea::ScrollEvent(const  tCIDCtrls::EScrollEvs   eEvent
                        , const tCIDLib::TBoolean       bHorz)
{
    if (!m_pwndChild)
        return;

    // We need to get the current info
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(SCROLLINFO);
    Info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;

    if (!GetScrollInfo(hwndSafe(), bHorz ? SB_HORZ : SB_VERT, &Info))
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSBar_GetScrollPos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Decide the new position based on the event
    tCIDLib::TInt4 i4NewPos = Info.nPos;

    switch(eEvent)
    {
        case tCIDCtrls::EScrollEvs::End :
            i4NewPos = Info.nMax - Info.nPage + 1;
            break;

        case tCIDCtrls::EScrollEvs::Home :
            i4NewPos = 0;
            break;

        case tCIDCtrls::EScrollEvs::NextLn :
            i4NewPos++;
            break;

        case tCIDCtrls::EScrollEvs::NextPg :
             i4NewPos += Info.nPage;
            break;

        case tCIDCtrls::EScrollEvs::PrevLn :
            i4NewPos--;
            break;

        case tCIDCtrls::EScrollEvs::PrevPg :
            i4NewPos -= Info.nPage;
            break;

        default :
            return;
    };

    // Clip the value back to valid range
    if (i4NewPos < 0)
        i4NewPos = 0;
    else if (i4NewPos > (Info.nMax - int(Info.nPage) + 1))
        i4NewPos = (Info.nMax - Info.nPage + 1);

    // Update the scroll bar
    SetSBarPos(bHorz, i4NewPos);

    //
    //  We set our controlled window's position to be the negation of the passed
    //  position. The scroll bars are in terms of i4LnSz units, so multiply it by
    //  that.
    //
    i4NewPos *= CIDCtrls_ScrollArea::i4LnSz;
    TPoint pntNew;
    if (bHorz)
        pntNew.i4X(i4NewPos * -1);
    else
        pntNew.i4Y(i4NewPos * -1);

    // Add any margin to it
    pntNew.Adjust(m_c4Margin, m_c4Margin);

    m_pwndChild->SetPos(pntNew);
}


// ---------------------------------------------------------------------------
//  TScrollArea: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called any time a new client is set, or the client size is changed. We
//  get the horz/vert size of the client relative to ours and set the scroll bars
//  to reflect that.
//
tCIDLib::TVoid TScrollArea::AdjustBars()
{
    if (!m_pwndChild)
        return;

    //
    //  Get the two relative areas. Make the client area larger by any margin that
    //  we have, so that at the end we can take it's original size and place it
    //  within the scroll area centered.
    //
    TArea areaClient = m_pwndChild->areaWndSize();
    areaClient.Inflate(m_c4Margin, m_c4Margin);
    TArea areaUs = areaWndSize();

    // We'll get info on both scroll bars
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(SCROLLINFO);

    // Do the horizontal one
    Info.fMask = SIF_PAGE | SIF_RANGE;
    Info.nMin = 0;
    Info.nPage = areaUs.c4Width() / CIDCtrls_ScrollArea::i4LnSz;
    Info.nMax = (areaClient.c4Width() / CIDCtrls_ScrollArea::i4LnSz) + 1;
    ::SetScrollInfo(hwndSafe(), SB_HORZ, &Info, TRUE);

    // Do the vertical one
    Info.fMask = SIF_PAGE | SIF_RANGE;
    Info.nMin = 0;
    Info.nPage = areaUs.c4Height() / CIDCtrls_ScrollArea::i4LnSz;
    Info.nMax = (areaClient.c4Height() / CIDCtrls_ScrollArea::i4LnSz) + 1;
    ::SetScrollInfo(hwndSafe(), SB_VERT, &Info, TRUE);

    //
    //  If we got bigger, then this could have affected our position as well, so
    //  deal with that. We have to turn around and get the new positions.
    //
    int iXPos = 0;
    int iYPos = 0;
    Info.fMask = SIF_POS;
    if (GetScrollInfo(hwndSafe(), SB_HORZ, &Info))
        iXPos = Info.nPos;
    if (GetScrollInfo(hwndSafe(), SB_VERT, &Info))
        iYPos = Info.nPos;

    TPoint pntNew
    (
        (iXPos * CIDCtrls_ScrollArea::i4LnSz) * -1
        , (iYPos * CIDCtrls_ScrollArea::i4LnSz) * -1
    );

    // Add in any argin
    pntNew.Adjust(m_c4Margin, m_c4Margin);

    m_pwndChild->SetPos(pntNew);
}


// Called to check if we have a child and throw if not
tCIDLib::TVoid TScrollArea::CheckHaveChild(const tCIDLib::TCard4 c4Line) const
{
    if (!m_pwndChild)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcScrA_NoChild
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TCardinal(widThis())
        );
    }
}


tCIDLib::TVoid TScrollArea::CleanupChild()
{
    // If we have a child window, then destroy it
    if (m_pwndChild)
    {
        try
        {
            m_pwndChild->Destroy();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // And clean up the actual object
        try
        {
            delete m_pwndChild;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    m_pwndChild = 0;
}

