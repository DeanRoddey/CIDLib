//
// FILE NAME: CIDCtrls_RadioButton.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
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
//  This file implements the radio button control.
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
AdvRTTIDecls(TRadioButton, TCtrlWnd)




// ---------------------------------------------------------------------------
//  CLASS: TRadioButton
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TRadioButton: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  The parent ctor parameter is just a dummy, to let us make him invoke the correct
//  ctor on his parent, since we are not a standard control, unlike the other button
//  derivatives.
//
TRadioButton::TRadioButton() :

    TCtrlWnd()
    , m_bInWnd(kCIDLib::False)
    , m_bState(kCIDLib::False)
    , m_bTracking(kCIDLib::False)
    , m_eRBStyles(tCIDCtrls::ERBStyles::None)
{
}

TRadioButton::~TRadioButton()
{
}

// ---------------------------------------------------------------------------
// TRadioButton: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TRadioButton::InitFromDesc( const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ERBStyles eRBStyles = tCIDCtrls::ERBStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_NoAutoCheck))
        eRBStyles |= tCIDCtrls::ERBStyles::NoAuto;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , dlgiSrc.strText()
        , eStyles
        , eRBStyles
    );
}


tCIDLib::TVoid TRadioButton::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_NoAutoCheck);
}


TSize TRadioButton::szDefault() const
{
    TSize szInit(96, 10);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TRadioButton: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return our current checked state
tCIDLib::TBoolean TRadioButton::bCheckedState() const
{
    return m_bState;
}


// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TRadioButton::Create(const  TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const TString&                strText
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::ERBStyles    eRBStyles)
{
    // Store our class specific styles
    m_eRBStyles = eRBStyles;

    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , strText.pszBuffer()
        , areaInit
        , tCIDCtrls::EWndStyles(eStyles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


// Set our checked state
tCIDLib::TVoid
TRadioButton::SetCheckedState(const tCIDLib::TBoolean bToSet, const tCIDLib::TBoolean bForceEvent)
{
    // If the state isn't changing, then nothing to do
    if (bToSet == m_bState)
        return;

    // Store the new state. If our new state is true, then uncheck others in the same group
    m_bState = bToSet;
    if (m_bState)
        UncheckOthers();

    // Force a redraw
    ForceRepaint();

    //
    //  If asked to, send a notification if an auto-check. If not autocheck, this won't work
    //  because we have to send the current state, not a new state. And they are telling us
    //  to set the state anyway, and the app is control of the state when not auto-check
    //  mode.
    //
    if (bForceEvent && !tCIDLib::bAllBitsOn(m_eRBStyles, tCIDCtrls::ERBStyles::NoAuto))
        SendClickNot();
}


// ---------------------------------------------------------------------------
//  TRadioButton: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TRadioButton::AreaChanged(  const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    // If the size changed, and we aren't minimized, then recalc our bitmap area
    if (bSizeChanged && (ePosState != tCIDCtrls::EPosStates::Minimized))
        CalcAreas();
}


tCIDLib::TBoolean
TRadioButton::bClick(const  tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    //
    //  If its a button one event, then we want to deal with it. Otherwise
    //  just let it go.
    //
    if (eButton == tCIDCtrls::EMouseButts::Left)
    {
        //
        //  If a down click, then start the tracking process and push down
        //  the button.
        //
        if ((eClickType == tCIDCtrls::EMouseClicks::Down)
        ||  (eClickType == tCIDCtrls::EMouseClicks::Double))
        {
            // Set the tracking flag and capture the mouse
            CaptureMouse();

            // Set our tracking flags and redraw
            m_bInWnd = kCIDLib::True;
            m_bTracking = kCIDLib::True;
            ForceRepaint();
        }
         else
        {
            //
            //  If an up click and we are tracking, then do the cleanup work. If we
            //  are in the window, then this constitutes a button click so we toggle
            //  the state and let the derived class handle the click action.
            //
            if (m_bTracking)
            {
                //
                //  Clear the tracking flag and release the mouse, remembering first if
                //  we were released in the window.
                //
                const tCIDLib::TBoolean bInvoke = m_bInWnd;
                m_bTracking = kCIDLib::False;
                m_bInWnd = kCIDLib::False;
                ReleaseMouse();

                if (bInvoke)
                {
                    //
                    //  We have a special case here. If this guy is marked as auto-check,
                    //  and his current state is true, then he's the checked one in this
                    //  set. So we ignore this because we don't want to be able to have
                    //  none of them checked. We only allow an unchecked one to become
                    //  checked, which unchecks the others in the group.
                    //
                    if (tCIDLib::bAllBitsOn(m_eRBStyles, tCIDCtrls::ERBStyles::NoAuto)
                    ||  !m_bState)
                    {
                        //
                        //  This will only do something if we are auto-check. If not, then we
                        //  will send the current state to any listeners and they can flip it
                        //  if they wish.
                        //
                        ToggleState();

                        // Send a click notification with (old or new) state
                        SendClickNot();
                    }
                     else
                    {
                        // But still redraw to get rid of the pressed emphasis if needed
                        ForceRepaint();
                    }
                }
            }
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TRadioButton::bCreated()
{
    TParent::bCreated();

    // Fault in the bitmap we use
    if (!s_bLoadedBmp)
    {
        // Make our indicator basically the height of a character cell and the same width
        s_szBitmap.Set(9, 9);
        s_szBitmap.c4Width(s_szBitmap.c4Height());
        TGUIFacility::AdjustDlgUnits(s_szBitmap);

        // Create our bitmaps that size. Make them true/alpha format
        TGraphWndDev gdevCompat(*this);
        s_bmpChecked = TBitmap
        (
            s_szBitmap
            , tCIDImage::EPixFmts::TrueClr
            , tCIDImage::EBitDepths::Eight
        );

        s_bmpUnchecked = TBitmap
        (
            s_szBitmap
            , tCIDImage::EPixFmts::TrueClr
            , tCIDImage::EBitDepths::Eight
        );

        s_bmpPressedChecked = TBitmap
        (
            s_szBitmap
            , tCIDImage::EPixFmts::TrueClr
            , tCIDImage::EBitDepths::Eight
        );

        s_bmpPressedUnchecked = TBitmap
        (
            s_szBitmap
            , tCIDImage::EPixFmts::TrueClr
            , tCIDImage::EBitDepths::Eight
        );

        {
            TGeomPen gpenOutline(facCIDGraphDev().rgbBlack, 1);
            TSolidBrush gbrInter(facCIDGraphDev().rgbWhite);

            // Create the checked one
            {
                TGraphMemDev gdevTar(gdevCompat, s_bmpChecked);
                TArea areaBmp(TPoint::pntOrigin, s_szBitmap);
                gdevTar.Fill(areaBmp, facCIDGraphDev().rgbGreen);

                // Draw a filled circle to fill the area
                {
                    TPenJanitor janPen(&gdevTar, &gpenOutline);
                    TBrushJanitor janBrush(&gdevTar, &gbrInter);
                    gdevTar.DrawFilledCircle((s_szBitmap.c4Width() / 2) - 1, areaBmp.pntCenter());
                }

                // And draw a black filled circle half that size
                {
                    TSolidBrush gbrFill(facCIDGraphDev().rgbBlack);
                    TBrushJanitor janBrush(&gdevTar, &gbrFill);
                    gdevTar.DrawFilledCircle(s_szBitmap.c4Width() / 4, areaBmp.pntCenter());
                }
            }

            // And the unchecked one
            {
                TGraphMemDev gdevTar(gdevCompat, s_bmpUnchecked);
                TArea areaBmp(TPoint::pntOrigin, s_szBitmap);
                gdevTar.Fill(areaBmp, facCIDGraphDev().rgbGreen);

                TPenJanitor janPen(&gdevTar, &gpenOutline);
                TBrushJanitor janBrush(&gdevTar, &gbrInter);
                gdevTar.DrawFilledCircle((s_szBitmap.c4Width() / 2) - 1, areaBmp.pntCenter());
            }
        }

        // Create the pressed versions of those
        {
            TGeomPen gpenOutline(facCIDGraphDev().rgbBlue, 1);
            TSolidBrush gbrInter(facCIDGraphDev().rgbDarkWhite);

            {
                TGraphMemDev gdevTar(gdevCompat, s_bmpPressedChecked);
                TArea areaBmp(TPoint::pntOrigin, s_szBitmap);
                gdevTar.Fill(areaBmp, facCIDGraphDev().rgbGreen);

                // Draw a filled circle to fill the area
                {
                    TPenJanitor janPen(&gdevTar, &gpenOutline);
                    TBrushJanitor janBrush(&gdevTar, &gbrInter);
                    gdevTar.DrawFilledCircle((s_szBitmap.c4Width() / 2) - 1, areaBmp.pntCenter());
                }

                // And draw a black filled circle half that size
                {
                    TSolidBrush gbrFill(facCIDGraphDev().rgbBlue);
                    TBrushJanitor janBrush(&gdevTar, &gbrFill);
                    gdevTar.DrawFilledCircle(s_szBitmap.c4Width() / 4, areaBmp.pntCenter());
                }
            }

            // And the unchecked one
            {
                TGraphMemDev gdevTar(gdevCompat, s_bmpPressedUnchecked);
                TArea areaBmp(TPoint::pntOrigin, s_szBitmap);
                gdevTar.Fill(areaBmp, facCIDGraphDev().rgbGreen);

                TPenJanitor janPen(&gdevTar, &gpenOutline);
                TBrushJanitor janBrush(&gdevTar, &gbrInter);
                gdevTar.DrawFilledCircle((s_szBitmap.c4Width() / 2) - 1, areaBmp.pntCenter());
            }
        }

        s_bLoadedBmp = kCIDLib::True;
    }

    // Do an initial indicator area calculation
    CalcAreas();

    return kCIDLib::True;
}


// On an unshifted space bar we toggle
tCIDLib::TBoolean
TRadioButton::bExtKey(  const   tCIDCtrls::EExtKeys     eKey
                        , const tCIDLib::TBoolean       bAlt
                        , const tCIDLib::TBoolean       bCtrl
                        , const tCIDLib::TBoolean       bShift
                        , const TWindow* const          pwndChild)
{
    if ((eKey == tCIDCtrls::EExtKeys::Space) && !bAlt && !bCtrl && !bShift)
    {
        //
        //  We will not react to this if the button has auto style and is the currently
        //  set one. We don't want to toggle the active one off.
        //
        if (tCIDLib::bAllBitsOn(m_eRBStyles, tCIDCtrls::ERBStyles::NoAuto) || !m_bState)
        {
            // Toggle the state if auto-check
            if (!tCIDLib::bAllBitsOn(m_eRBStyles, tCIDCtrls::ERBStyles::NoAuto))
                ToggleState();

            //
            //  Do a click action. If not autocheck, we are sending the original state
            //  and he can toggle if he wants.
            //
            SendClickNot();
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TRadioButton::bMouseMove(const  TPoint&             pntAt
                        , const tCIDLib::TBoolean   bControlDown
                        , const tCIDLib::TBoolean   bShiftDown)
{
    ProcessMouse(pntAt);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TRadioButton::bPaint(TGraphDrawDev& gdevTarget, const TArea& areaUpdate)
{
    // Draw the indicator if it intersects
    if (m_areaIndicator.bIntersects(areaUpdate))
        DrawIndicator(gdevTarget);

    const TArea areaUs(areaClient());

    // Get an area for the text
    TArea areaPos(areaUs);
    areaPos.Deflate(1);

    //
    //  And adjust for the check mark. We use the fault tolerant AjustSides() method
    //  just in case this would make it go negative.
    //
    areaPos.i4X(m_areaIndicator.i4Right() + 4);
    if (areaPos.i4Left() < areaUs.i4Right() - 4)
        areaPos.i4Right(areaUs.i4Right() - 4);

    // Set us to use the standard font
    TFontJanitor janFont(&gdevTarget, &TGUIFacility::gfontDefNorm());

    //
    //  Set up the text formatting flags. We just turn on mnemonic regardless and let
    //  DrawString() deal with it if there is really not a mnemonic in the text. If the
    //  window is disabled, then we need to set the halftone flag.
    //
    tCIDGraphDev::ETextFmts eFmt = tCIDGraphDev::ETextFmts::Mnemonic;
    if (!bIsEnabled())
        eFmt = tCIDLib::eOREnumBits(eFmt, tCIDGraphDev::ETextFmts::Disabled);

    gdevTarget.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);
    gdevTarget.SetTextColor(m_rgbText);
    gdevTarget.DrawString
    (
        strWndText()
        , areaPos
        , tCIDLib::EHJustify::Left
        , tCIDLib::EVJustify::Center
        , eFmt
    );

    // If we have the focus, then draw the focus indicator
    if (bHasFocus())
    {
        TCosmeticPen gpenLine(facCIDGraphDev().rgbWhite, tCIDGraphDev::ELineStyles::Alternate);
        gdevTarget.eMixMode(tCIDGraphDev::EMixModes::XorPen);
        TPenJanitor janPen(&gdevTarget, &gpenLine);
        gdevTarget.Stroke(areaUs);
    }

    return kCIDLib::True;
}


tCIDLib::TVoid TRadioButton::GettingFocus()
{
    // Redraw to show the focus indicator
    ForceRepaint();
}


tCIDLib::TVoid TRadioButton::LosingFocus()
{
    //
    //  Handle the psycho scenario here of losing the focus with some
    //  states set. This should not happen, but we deal with it just
    //  in case.
    //
    if (m_bTracking)
    {
        m_bTracking = kCIDLib::False;
        m_bInWnd = kCIDLib::False;
        ReleaseMouse();
    }

    // Redraw to get rid of the focus indicator
    ForceRepaint();
}


// ---------------------------------------------------------------------------
//  TRadioButton: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TRadioButton::CalcAreas()
{
    TArea areaTar = areaClient();
    m_areaIndicator.Set(0, 0, s_szBitmap.c4Width(), s_szBitmap.c4Height());
    m_areaIndicator.LeftJustifyIn
    (
        TArea(TPoint::pntOrigin, areaTar.szArea()), kCIDLib::True
    );

    // Clip it to the new area, in case it's smaller
    TArea areaClip(areaTar);
    areaClip.ZeroOrg();
    m_areaIndicator &= areaClip;
}


tCIDLib::TVoid TRadioButton::DrawIndicator(TGraphDrawDev& gdevTarget)
{
    TArea areaDraw(m_areaIndicator);

    TArea areaSrc(TPoint::pntOrigin, s_szBitmap);

    TBitmap* pbmpTar = nullptr;
    if (m_bTracking && m_bInWnd)
    {
        if (m_bState)
            pbmpTar = &s_bmpPressedChecked;
        else
            pbmpTar = &s_bmpPressedUnchecked;
    }
     else
    {
        if (m_bState)
            pbmpTar = &s_bmpChecked;
        else
            pbmpTar = &s_bmpUnchecked;
    }

    if (pbmpTar)
        pbmpTar->DrawTransparent(gdevTarget, areaSrc, areaDraw, 0x00FF00);
}


tCIDLib::TVoid TRadioButton::ProcessMouse(const TPoint& pntAt)
{
    if (m_bTracking)
    {
        // Remember the current state and store the new one
        const tCIDLib::TBoolean bOldInWnd = m_bInWnd;
        m_bInWnd = areaClient().bContainsPoint(pntAt);

        // If the state state changed
        if (bOldInWnd != m_bInWnd)
            ForceRepaint();
    }
}


tCIDLib::TVoid TRadioButton::SendClickNot()
{
    SendAsyncNotify(new TButtClickInfo(m_bState, *this), TButtClickInfo::nidClick);
}


tCIDLib::TVoid TRadioButton::ToggleState()
{
    if (tCIDLib::bAllBitsOn(m_eRBStyles, tCIDCtrls::ERBStyles::NoAuto))
        return;

    // Toggle our state
    m_bState = !m_bState;

    // If our new state is true, then uncheck others
    if (m_bState)
        UncheckOthers();

    // And redraw
    ForceRepaint();
}


tCIDLib::TVoid TRadioButton::UncheckOthers()
{
    //
    //  Radio buttons are special in that auto-mode buttons only allow one
    //  of them in a group to be clicked. So if we are toggling on and this
    //  is an auto-mode radio button, we have to scan all children in this
    //  group and toggle off any other radio buttons.
    //
    if (m_bState && !tCIDLib::bAllBitsOn(m_eRBStyles, tCIDCtrls::ERBStyles::NoAuto))
    {
        TWindow& wndDad = wndParent();
        if (!wndDad.bIsThisWindow(TWindow::wndDesktop()))
        {
            TWindow* pwndCur = wndDad.pwndFirstOfGroup(*this);
            while (pwndCur)
            {
                if (pwndCur != this)
                {
                    //
                    //  Iterate forward and look for radio button (or derived)
                    //  windows and click them all off. Of course we can skip
                    //  ourself.
                    //
                    TRadioButton* pwndButt = dynamic_cast<TRadioButton*>(pwndCur);

                    // If not a radio button, we are done
                    if (!pwndButt)
                        break;

                     pwndButt->SetCheckedState(kCIDLib::False);
                }
                pwndCur = wndDad.pwndNextChild(*pwndCur);

                // If this one is the beginning of the next group, break out
                if (pwndCur
                &&  tCIDLib::bAllBitsOn(pwndCur->eWndStyles(), tCIDCtrls::EWndStyles::Group))
                {
                    break;
                }
            }
        }
    }
}



// ---------------------------------------------------------------------------
//  TRadioButton: Private, static methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TRadioButton::s_bLoadedBmp = kCIDLib::False;
TBitmap TRadioButton::s_bmpChecked;
TBitmap TRadioButton::s_bmpUnchecked;
TBitmap TRadioButton::s_bmpPressedChecked;
TBitmap TRadioButton::s_bmpPressedUnchecked;
TSize TRadioButton::s_szBitmap;
