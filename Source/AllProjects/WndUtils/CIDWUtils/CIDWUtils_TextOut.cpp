//
// FILE NAME: CIDWUtils_TextOut.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/23/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements some read only, scroll text output type windows.
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
#include    "CIDWUtils_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TTextOutWnd,TWindow)
AdvRTTIDecls(TTextOutWnd2,TCtrlWnd)



// ---------------------------------------------------------------------------
//   CLASS: TTextOutWnd::TTOLine
//  PREFIX: tol
// ---------------------------------------------------------------------------
TTextOutWnd::TTOLine::TTOLine()
{
}

TTextOutWnd::TTOLine::TTOLine(  const   TString& strText
                                , const TRGBClr& rgbBgn
                                , const TRGBClr& rgbText) :

    m_rgbBgn(rgbBgn)
    , m_rgbText(rgbText)
    , m_strText(strText)
{
}

TTextOutWnd::TTOLine::~TTOLine()
{
}

tCIDLib::TVoid TTextOutWnd::TTOLine::Reset(const TRGBClr& rgbBgn, const TRGBClr& rgbText)
{
    m_rgbBgn = rgbBgn;
    m_rgbText = rgbText;
    m_strText.Clear();
}



// ---------------------------------------------------------------------------
//   CLASS: TTextOutWnd
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextOutWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextOutWnd::TTextOutWnd(const tCIDLib::TCard4 c4MaxStrs) :

    m_c4LineHeight(0)
    , m_colBuf(c4MaxStrs)
    , m_eTOStyles(tCIDWUtils::ETOutStyles::None)
{
}


TTextOutWnd::~TTextOutWnd()
{
}


// ---------------------------------------------------------------------------
//  TTextOutWnd2: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTextOutWnd::InitFromDesc(  const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles = tCIDLib::eOREnumBits(eExStyles, tCIDCtrls::EExWndStyles::SunkenEdge);
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        // Set a regular border unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles = tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles::Border);
    }

    CreateTextOut
    (
        wndParent
        , dlgiSrc.areaPos()
        , dlgiSrc.ridChild()
        , eStyles
        , tCIDWUtils::ETOutStyles::None
        , eExStyles
    );
}


tCIDLib::TVoid TTextOutWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    // For now, just pass it on to our parent
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
}



// -------------------------------------------------------------------
//  TTextOutWnd: Public, non-virtual methods
// -------------------------------------------------------------------

//
//  We add the passed line(s) to the circular buffer. Then we we have to get the
//  line displayed.
//
tCIDLib::TVoid TTextOutWnd::AddString(const TString& strToAdd)
{
    m_colBuf.bAdd(TTOLine(strToAdd, m_rgbCurBgn, m_rgbCurText));

    TArea areaInvalid;
    Scroll
    (
        m_areaDisplay
        , areaInvalid
        , 0
        , -tCIDLib::TInt4(m_c4LineHeight)
        , kCIDLib::False
    );
    Redraw(areaInvalid, tCIDCtrls::ERedrawFlags::Full);
}

tCIDLib::TVoid TTextOutWnd::AddStrings(const tCIDLib::TStrList& colToAdd)
{
    const tCIDLib::TCard4 c4Count = colToAdd.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colBuf.bAdd(TTOLine(colToAdd[c4Index], m_rgbCurBgn, m_rgbCurText));

    TArea areaInvalid;
    Scroll
    (
        m_areaDisplay
        , areaInvalid
        , 0
        , -tCIDLib::TInt4(m_c4LineHeight * c4Count)
        , kCIDLib::False
    );
    Redraw(areaInvalid, tCIDCtrls::ERedrawFlags::Full);
}


// Clear all of the lines from the buffer and force a repaint.
tCIDLib::TVoid TTextOutWnd::Clear()
{
    // Set all of the strings to empty again
    m_rgbCurBgn = rgbBgnFill();
    m_rgbCurText = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::WindowText);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_colBuf.c4MaxElemCount(); c4Index++)
        m_colBuf.tAt(c4Index).Reset(m_rgbCurBgn, m_rgbCurText);

    // Push the scroll bar to the end and set the virtual content initially to the end
    SetScrollPos(kCIDLib::False, c4MaxScrollPos(kCIDLib::False));
    SetVContArea();

    // And force a redraw (it may have already been at the end or close to it)
    Redraw(tCIDCtrls::ERedrawFlags::Full);
}


tCIDLib::TVoid
TTextOutWnd::CreateTextOut( const   TWindow&                wndParent
                            , const TArea&                  areaInit
                            , const tCIDCtrls::TWndId       widThis
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDWUtils::ETOutStyles eTOStyles
                            , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Store our type specific styles
    m_eTOStyles = eTOStyles;

    //
    //  And now create us. We have to use the size redraw basic type since we move the
    //  content around to keep it centered. So we can't allow clients to resize us and
    //  copy bits. That will not work.
    //
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles(WS_VSCROLL))
        , eExStyles
        , widThis
    );

}


// Give back the text currently displayed
tCIDLib::TVoid TTextOutWnd::QueryText(TString& strToLoad) const
{
    // Create a text output stream over the target string
    strToLoad.Clear();
    TTextStringOutStream strmOut(&strToLoad);

    //
    //  Set the newline translation to CR/LF, since we aren't going to a file
    //  that will get translated on the way in, but straight to the clipboard.
    //
    strmOut.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

    //
    //  Now let's go through and dump out the strings. Go to the first non-empty one before
    //  we start collecting.
    //
    const tCIDLib::TCard4 c4Count = m_colBuf.c4MaxElemCount();;
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4Count)
    {
        if (!m_colBuf[c4Index].m_strText.bIsEmpty())
            break;
        c4Index++;
    }

    while (c4Index < c4Count)
        strmOut << m_colBuf[c4Index++].m_strText << kCIDLib::NewLn;

    // One last new line and flush
    strmOut << kCIDLib::EndLn;
}


const TRGBClr& TTextOutWnd::rgbCurBgn() const
{
    return m_rgbCurBgn;
}


const TRGBClr& TTextOutWnd::rgbCurText() const
{
    return m_rgbCurText;
}


tCIDLib::TVoid
TTextOutWnd::SetLineColors(const TRGBClr& rgbBgn, const TRGBClr& rgbText)
{
    m_rgbCurBgn = rgbBgn;
    m_rgbCurText = rgbText;
}


// ---------------------------------------------------------------------------
//  TTextOutWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TTextOutWnd::AreaChanged(const  TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if (ePosState != tCIDCtrls::EPosStates::Minimized)
    {
        // Adjust our display area
        SetDisplayArea();

        // And update the scroll bar to reflect the new visible size
        SetScrollBar
        (
            kCIDLib::False
            , m_c4LineHeight * m_colBuf.c4MaxElemCount()
            , m_areaDisplay.c4Height()
            , m_c4LineHeight
        );

        // And adjust our virtual content area
        SetVContArea();

        Redraw(tCIDCtrls::ERedrawFlags::Immediate);
    }
}


tCIDLib::TBoolean TTextOutWnd::bCreated()
{
    TWindow::bCreated();

    // Preload the circular buffer with empty strings so it's always full
    m_rgbCurBgn = rgbBgnFill();
    m_rgbCurText = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::WindowText);
    TTOLine tolInit(TString::strEmpty(), m_rgbCurBgn, m_rgbCurText);
    const tCIDLib::TCard4 c4Count = m_colBuf.c4MaxElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colBuf.bAdd(tolInit);

    //
    //  Figure out what the line height will be. Get a temp graphic device on our
    //  parent, and set the standard font on it. Then we can get the metrics and
    //  calculate our full size.
    //
    TGraphWndDev gdevTmp(*this);
    TFontJanitor janFont(&gdevTmp, &TGUIFacility::gfontDefNorm());
    TFontMetrics fmtrDef(gdevTmp);
    m_c4LineHeight = fmtrDef.c4Height();

    // Get our client area and see how many lines we can evenly fit
    SetDisplayArea();

    // Set up our scroll bar now
    SetScrollBar
    (
        kCIDLib::False
        , m_c4LineHeight * m_colBuf.c4MaxElemCount()
        , m_areaDisplay.c4Height()
        , m_c4LineHeight
    );

    // And force our scroll bar to the end
    SetScrollPos(kCIDLib::False, c4MaxScrollPos(kCIDLib::False));
    SetVContArea();

    return kCIDLib::True;
}


// Just eat this. We do any drawing required
tCIDLib::TBoolean TTextOutWnd::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


//
//  We figure out which lines fall into our display area given the current scroll bar
//  position and draw them.
//
tCIDLib::TBoolean TTextOutWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    //
    //  Take the area to update, and remove the display area from it. This is the bit
    //  around the display area (our margin area.) This is the area we want to fill with
    //  our background color. The rest either doesn't need to be updated, or it does and
    //  will be filled with the bgn colors of the lines.
    //
    TGUIRegion grgnFill(areaClient());

    //
    //  <TBD>
    //  Need to figure this out. If we don't reduce the display area before we remove
    //  it from the client area, the clip region goes one pixel too far on the right
    //  (which removes those pixels from the fill and so they don't get filled in.
    //  That shouldn't be the case.
    //
    TArea areaTest(m_areaDisplay);
    areaTest.AdjustSize(-1, 0);
    grgnFill.CombineWith(areaTest, tCIDGraphDev::EClipModes::Diff);
    if (!grgnFill.bIsEmpty())
        gdevToUse.Fill(grgnFill, rgbBgnFill());

    // If the update intersects the display area we have to display text
    if (areaUpdate.bIntersects(m_areaDisplay))
    {
        // Set up the standard normal font
        TFontJanitor janFont(&gdevToUse, &TGUIFacility::gfontDefNorm());

        //
        //  Get the worst case update area, which is the intersection of the update
        //  and display areas.
        //
        TArea areaDraw(m_areaDisplay);
        areaDraw &= areaUpdate;

        //
        //  Calculate the index of the first and last lines that intersect the drawing
        //  area.
        //
        const tCIDLib::TCard4 c4StartInd
        (
            tCIDLib::TCard4(areaDraw.i4Y() - m_areaVCont.i4Y()) / m_c4LineHeight
        );

        const tCIDLib::TCard4 c4EndInd
        (
            tCIDLib::TCard4(areaDraw.i4Bottom() - m_areaVCont.i4Y()) / m_c4LineHeight
        );

        //
        //  We have something to draw, so calc the area of the first one. We multiply the
        //  line height times the calculated initial line index, and then add it to the
        //  virtual content origin. It wll come out somewhere within a single line of the
        //  top of our client area.
        //
        TArea areaCur
        (
            m_areaDisplay.i4X()
            , tCIDLib::TInt4(c4StartInd * m_c4LineHeight) + m_areaVCont.i4Y()
            , m_areaDisplay.c4Width()
            , m_c4LineHeight
        );

        // Set the text bgn transparent
        gdevToUse.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);

        //
        //  Make sure out put is limited to the intersection of the update and display areas,
        //  since we could have a last partial one.
        //
        TArea areaText;
        gdevToUse.bIntersectClipArea(m_areaDisplay);
        for (tCIDLib::TCard4 c4Index = c4StartInd; c4Index <= c4EndInd; c4Index++)
        {
            const TTOLine& tolCur = m_colBuf[c4Index];

            // Fill the background with this line's color
            gdevToUse.Fill(areaCur, tolCur.m_rgbBgn);


            // If there's text draw it, pushed in a couple pixels inside the color fill
            if (!tolCur.m_strText.bIsEmpty())
            {
                gdevToUse.SetTextColor(tolCur.m_rgbText);
                areaText = areaCur;
                areaText.Deflate(2, 0);
                gdevToUse.DrawString(tolCur.m_strText, areaText);
            }

            areaCur.Move(tCIDLib::EDirs::Down);
        }
    }

    return kCIDLib::True;
}


// If they right click, we'll get called here and show a little context menu
tCIDCtrls::ECtxMenuOpts
TTextOutWnd::eShowContextMenu(const TPoint& pntAt, const tCIDCtrls::TWndId widSrc)
{
    // Invoke our context menu
    TPopupMenu menuPopup(L"LineOut Context Menu");
    menuPopup.Create(facCIDWUtils(), kCIDWUtils::ridMenu_TextOut);

    tCIDLib::TCard4 c4Res = menuPopup.c4Process
    (
        *this, pntAt, tCIDLib::EVJustify::Bottom
    );

    if (c4Res == kCIDWUtils::ridMenu_TextOut_CopyText)
    {
        // Query the text into a string
        TString strText(8192UL);
        QueryText(strText);

        // And paste it to the clipboard
        TGUIClipboard gclipCopy(*this);
        gclipCopy.Clear();
        gclipCopy.StoreText(strText);
    }
     else if (c4Res == kCIDWUtils::ridMenu_TextOut_ClearText)
    {
        Clear();
    }

    // Indicate that we handled it
    return tCIDCtrls::ECtxMenuOpts::Done;
}


// Handle scroll page/line events from the base window class
tCIDLib::TVoid
TTextOutWnd::ScrollEvent(const tCIDCtrls::EScrollEvs eEvent, const tCIDLib::TBoolean bHorz)
{
    // Decide the new position based on the event
    const tCIDLib::TCard4 c4InitPos = c4CurScrollPos(bHorz);
    tCIDLib::TCard4 c4NewPos = c4InitPos;

    tCIDLib::TCard4 c4CurPos;
    tCIDLib::TCard4 c4MaxPos;
    tCIDLib::TCard4 c4PageSz;
    QueryScrollInfo(bHorz, c4CurPos, c4MaxPos, c4PageSz);

    switch(eEvent)
    {
        case tCIDCtrls::EScrollEvs::End :
            c4NewPos = c4MaxPos - c4PageSz + 1;
            break;

        case tCIDCtrls::EScrollEvs::Home :
            c4NewPos = 0;
            break;

        case tCIDCtrls::EScrollEvs::NextLn :
            c4NewPos++;
            break;

        case tCIDCtrls::EScrollEvs::NextPg :
             c4NewPos += c4PageSz;
            break;

        case tCIDCtrls::EScrollEvs::PrevLn :
            if (c4NewPos)
                c4NewPos--;
            break;

        case tCIDCtrls::EScrollEvs::PrevPg :
            if (c4NewPos >= c4PageSz)
                c4NewPos -= c4PageSz;
            else
                c4NewPos = 0;
            break;

        default :
            return;
    };

    // Clip the value back to valid range
    if (c4NewPos > c4MaxPos)
        c4NewPos = c4MaxPos;

    if (c4NewPos != c4InitPos)
    {
        // Update the scroll bar and adjust our virtual content area to match
        SetSBarPos(bHorz, c4NewPos);
        SetVContArea();

        //
        //  And update our displayed content. If the difference between them is less than
        //  a page's worth, we can scroll some of the content and just draw the rest. Else
        //  we just redraw the display area.
        //
        tCIDLib::TInt4 i4ScrollDir;
        tCIDLib::TCard4 c4Diff;
        if (c4NewPos < c4InitPos)
        {
            c4Diff = c4InitPos - c4NewPos;
            i4ScrollDir = 1;
        }
         else
        {
            c4Diff = c4NewPos - c4InitPos;
            i4ScrollDir = -1;
        }

        if (c4Diff < c4PageSz)
        {
            // Scroll the window contents and force a redraw of the uncovered bit
            TArea areaInvalid;
            Scroll
            (
                m_areaDisplay
                , areaInvalid
                , 0
                , tCIDLib::TInt4(c4Diff * m_c4LineHeight) * i4ScrollDir
                , kCIDLib::False
            );
            Redraw(areaInvalid, tCIDCtrls::ERedrawFlags::Full);
        }
         else
        {
            Redraw(m_areaDisplay, tCIDCtrls::ERedrawFlags::Full);
        }
    }
}


// Handle scroll thumb drag events from the base window class
tCIDLib::TVoid
TTextOutWnd::ScrollDrag(const   tCIDLib::TBoolean   bHorz
                        , const tCIDLib::TCard4     c4Position
                        , const tCIDLib::TBoolean   bFinal)
{
    // Save our current position
    const tCIDLib::TCard4 c4InitPos = c4CurScrollPos(bHorz);

    // Update the scroll bar and virtual content area
    SetSBarPos(bHorz, c4Position);
    SetVContArea();

    tCIDLib::TCard4 c4CurPos;
    tCIDLib::TCard4 c4MaxPos;
    tCIDLib::TCard4 c4PageSz;
    QueryScrollInfo(bHorz, c4CurPos, c4MaxPos, c4PageSz);

    // And update our displayed content. Though in theory they could drag it fast enough
    tCIDLib::TInt4 i4ScrollDir;
    tCIDLib::TCard4 c4Diff;
    if (c4Position < c4InitPos)
    {
        c4Diff = c4InitPos - c4Position;
        i4ScrollDir = 1;
    }
     else
    {
        c4Diff = c4Position - c4InitPos;
        i4ScrollDir = -1;
    }

    if (c4Diff < c4PageSz)
    {
        // Scroll the window contents and force a redraw of the uncovered bit
        TArea areaInvalid;
        Scroll
        (
            m_areaDisplay
            , areaInvalid
            , 0
            , tCIDLib::TInt4(c4Diff * m_c4LineHeight) * i4ScrollDir
            , kCIDLib::True
        );
        Redraw(areaInvalid, tCIDCtrls::ERedrawFlags::Full);
    }
     else
    {
        Redraw(m_areaDisplay, tCIDCtrls::ERedrawFlags::Full);
    }
}


// Sets up our display area
tCIDLib::TVoid TTextOutWnd::SetDisplayArea()
{
    const TArea& areaAvail = areaClient();
    m_areaDisplay = areaAvail;

    // If we have a border, move in again
    if (tCIDLib::bAllBitsOn(eWndStyles(), tCIDCtrls::EWndStyles::Border))
        m_areaDisplay.Deflate(1);

    tCIDLib::TCard4 c4Lines = m_areaDisplay.c4Height() / m_c4LineHeight;
    tCIDLib::TCard4 c4Height = c4Lines * m_c4LineHeight;

    // Calculate any partial line amount
    const tCIDLib::TCard4 c4Partial(areaAvail.c4Height() - c4Height);

    //
    //  If we have more than 6 pixels of partial (enough for 3 on top and at least 3 bottom),
    //  then we just use that as the vertical margin. Else, we have to reduce the number of
    //  lines in order to get some margin.
    //
    if (c4Partial < 6)
    {
        c4Lines--;
        c4Height -= m_c4LineHeight;
    }

    // Keep the calculated height and create some horizontal margin
    m_areaDisplay.c4Height(c4Height);
    m_areaDisplay.Deflate(6, 0);

    // And ut the display area 4 down
    m_areaDisplay.i4Y(areaAvail.i4Y() + 3);
}


// ---------------------------------------------------------------------------
//  TTextOutWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Sets up our virtual content area for the current scroll position. It is based relative
//  to the display area. The overall virtual content area is already set, we just need to
//  adjust the y origin.
//
tCIDLib::TVoid TTextOutWnd::SetVContArea()
{
    m_areaVCont.i4Y
    (
        (c4CurScrollPos(kCIDLib::False) * m_c4LineHeight) * -1
    );

    // Make it relative to the display area
    m_areaVCont += m_areaDisplay.pntOrg();
}



// ---------------------------------------------------------------------------
//   CLASS: TTextOutWnd2
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextOutWnd2: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextOutWnd2::TTextOutWnd2() :

    m_c4LineSpacing(8)
{
}

TTextOutWnd2::~TTextOutWnd2()
{
}


// ---------------------------------------------------------------------------
//  TTextOutWnd2: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTextOutWnd2::InitFromDesc( const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{

    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles = tCIDLib::eOREnumBits(eExStyles, tCIDCtrls::EExWndStyles::SunkenEdge);
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        // Set a regular border unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles = tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles::Border);
    }

    CreateTextOutWnd
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eExStyles
    );
}


tCIDLib::TVoid TTextOutWnd2::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    // For now, just pass it on to our parent
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
}



// ---------------------------------------------------------------------------
//  TTextOutWnd2: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTextOutWnd2::AppendText(const TString& strToAppend, const TRGBClr& rgbColor)
{
    TGraphWndDev gdevThis(*this);

    // Calculate the area of the text at our available width
    const TArea areaText = gdevThis.areaMLText(strToAppend, m_areaZero.c4Width());
    const tCIDLib::TInt4 i4Height(areaText.c4Height());

    // Scroll the bitmap contents up by this much plus the inter-msg padding
    TGraphMemDev gdevBitmap(gdevThis, m_bmpImg);

    TArea areaUpdate;
    gdevBitmap.ScrollBits
    (
        m_areaZero
        , m_areaZero
        , 0
        , (i4Height + 16) * -1
        , areaUpdate
        , kCIDLib::False
    );

    // Fill the uncovered area
    gdevBitmap.Fill(areaUpdate, rgbBgnFill());

    // Now draw the new text into the update area, moved down by the extra padding
    TArea areaDraw(areaUpdate);
    areaDraw.AdjustAndShrink(0, 16);

    //
    //  Draw the text into that area, bottom justified, so that if it's longer than the
    //  available window size we'll see the last part. We have to set our font on the
    //  bitmap graphics device first, else it'll use some default.
    //
    TFontSelAttrs fselToUse(gdevThis);
    TGUIFont gfontToUse(fselToUse);
    TFontJanitor janFont(&gdevBitmap, &gfontToUse);

    gdevBitmap.SetTextColor(rgbColor);
    gdevBitmap.DrawMText
    (
        strToAppend
        , areaDraw
        , tCIDLib::EHJustify::Left
        , tCIDLib::EVJustify::Bottom
    );

    // Now blit it to the display area, waiting for retrace
    gdevThis.CopyBits
    (
        gdevBitmap, m_areaZero, m_areaImg, tCIDGraphDev::EBmpModes::SrcCopy, kCIDLib::True
    );
}


tCIDLib::TVoid TTextOutWnd2::Clear()
{
    TGraphWndDev gdevThis(*this);
    TGraphMemDev gdevBitmap(gdevThis, m_bmpImg);
    gdevBitmap.Fill(m_areaZero, rgbBgnFill());
    gdevThis.CopyBits
    (
        gdevBitmap, m_areaZero, m_areaImg, tCIDGraphDev::EBmpModes::SrcCopy, kCIDLib::True
    );
}


tCIDLib::TVoid
TTextOutWnd2::CreateTextOutWnd( const   TWindow&                wndParent
                                , const tCIDCtrls::TWndId       widToUse
                                , const TArea&                  areaInit
                                , const tCIDCtrls::EWndStyles   eWndStyles
                                , const tCIDCtrls::EExWndStyles eExWndStyles)
{
    // And pass on to our parent class
    TParent::CreateWnd
    (
        wndParent.hwndSafe()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , eWndStyles
        , eExWndStyles
        , widToUse
    );
}


tCIDLib::TVoid TTextOutWnd2::SetLineSpacing(const tCIDLib::TCard4 c4ToSet)
{
    m_c4LineSpacing = c4ToSet;
}



// ---------------------------------------------------------------------------
//  TTextOutWnd2: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We have to keep our bitmap the same size as us, moved in slighly to provide
//  some margin.
//
tCIDLib::TVoid
TTextOutWnd2::AreaChanged(  const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Only do this if not minimized the size changed.
    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
        DoSizeCalcs();
}


// Make sure we get our bitmap and size info initalized
tCIDLib::TBoolean TTextOutWnd2::bCreated()
{
    TParent::bCreated();

    // Do an initial size calculation
    DoSizeCalcs();
    return kCIDLib::True;
}


//
//  Just blit the area of our bitmap that intersects the update area.
//
tCIDLib::TBoolean
TTextOutWnd2::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    const TArea areaUs(areaClient());

    // Do a background fill, leaving out the image area
    TGUIRegion grgnFill(areaUs);
    grgnFill.CombineWith(m_areaImg, tCIDGraphDev::EClipModes::Diff);
    gdevToUse.Fill(grgnFill, rgbBgnFill());

    // And update the image
    TGraphMemDev gdevBitmap(gdevToUse, m_bmpImg);
    gdevToUse.CopyBits
    (
        gdevBitmap, m_areaZero, m_areaImg, tCIDGraphDev::EBmpModes::SrcCopy, kCIDLib::True
    );
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TTextOutWnd2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTextOutWnd2::DoSizeCalcs()
{
    TGraphWndDev gdevThis(*this);

    m_areaImg = areaClient();
    m_areaImg.Deflate(4);

    // Create a zero origined version for blitting from
    m_areaZero = m_areaImg;
    m_areaZero.ZeroOrg();

    // Create the new bitmap and fill the background
    m_bmpImg = TBitmap(gdevThis, m_areaZero.szArea());

    TGraphMemDev gdevBitmap(gdevThis, m_bmpImg);
    gdevBitmap.Fill(m_areaZero, rgbBgnFill());
}

