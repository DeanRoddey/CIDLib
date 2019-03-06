//
// FILE NAME: CIDCtrls_MColListBox.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements our ubiquitous multi-column list box control.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes. We bring in our own internal implementation class
//  in our case.
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"

// Temporary so we can work on this without causing constant rebuilds downstream
#include    "CIDCtrls_ColHeader.hpp"
#include    "CIDCtrls_MColListBox.hpp"

#include    "CIDCtrls_MColListBoxImpl_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TMColListBox, TCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TMCLBColDef
// PREFIX: mclbcd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMCLBColDef: Constructors and Destructor
// ---------------------------------------------------------------------------
TMCLBColDef::TMCLBColDef() :

    m_bBold(kCIDLib::False)
    , m_c4Width(24)
    , m_eHJustify(tCIDLib::EHJustify::Left)
    , m_prgbBgnClr(nullptr)
    , m_prgbTextClr(nullptr)
{
}
TMCLBColDef::TMCLBColDef(const  TString&            strTitle
                        , const tCIDLib::EHJustify  eHJustify
                        , const tCIDLib::TCard4     c4Width
                        , const tCIDLib::TBoolean   bBold) :
    m_bBold(bBold)
    , m_c4Width(c4Width)
    , m_eHJustify(eHJustify)
    , m_prgbBgnClr(nullptr)
    , m_prgbTextClr(nullptr)
    , m_strTitle(strTitle)
{
}

TMCLBColDef::TMCLBColDef(const  TString&            strTitle
                        , const tCIDLib::EHJustify  eHJustify
                        , const TRGBClr&            rgbBgnClr
                        , const TRGBClr&            rgbTextClr
                        , const tCIDLib::TCard4     c4Width
                        , const tCIDLib::TBoolean   bBold) :
    m_bBold(bBold)
    , m_c4Width(c4Width)
    , m_eHJustify(eHJustify)
    , m_prgbBgnClr(new TRGBClr(rgbBgnClr))
    , m_prgbTextClr(new TRGBClr(rgbTextClr))
    , m_strTitle(strTitle)
{
}

TMCLBColDef::TMCLBColDef(const TMCLBColDef& mclbcdSrc) :

    m_bBold(mclbcdSrc.m_bBold)
    , m_c4Width(mclbcdSrc.m_c4Width)
    , m_eHJustify(mclbcdSrc.m_eHJustify)
    , m_prgbBgnClr(nullptr)
    , m_prgbTextClr(nullptr)
    , m_strTitle(mclbcdSrc.m_strTitle)
{
    if (mclbcdSrc.m_prgbBgnClr)
        m_prgbBgnClr = new TRGBClr(*mclbcdSrc.m_prgbBgnClr);

    if (mclbcdSrc.m_prgbTextClr)
        m_prgbTextClr = new TRGBClr(*mclbcdSrc.m_prgbTextClr);
}

TMCLBColDef::~TMCLBColDef()
{
    if (m_prgbBgnClr)
        delete m_prgbBgnClr;

    if (m_prgbTextClr)
        delete m_prgbTextClr;
}


// ---------------------------------------------------------------------------
//  TMCLBColDef: Public operators
// ---------------------------------------------------------------------------
TMCLBColDef& TMCLBColDef::operator=(const TMCLBColDef& mclbcdSrc)
{
    if (this != &mclbcdSrc)
    {
        m_bBold     = mclbcdSrc.m_bBold;
        m_c4Width   = mclbcdSrc.m_c4Width;
        m_eHJustify = mclbcdSrc.m_eHJustify;
        m_strTitle  = mclbcdSrc.m_strTitle;

        if (mclbcdSrc.m_prgbBgnClr)
        {
            if (m_prgbBgnClr)
                *m_prgbBgnClr = *mclbcdSrc.m_prgbBgnClr;
            else
                m_prgbBgnClr = new TRGBClr(*mclbcdSrc.m_prgbBgnClr);
        }

        if (mclbcdSrc.m_prgbTextClr)
        {
            if (m_prgbTextClr)
                *m_prgbTextClr = *mclbcdSrc.m_prgbTextClr;
            else
                m_prgbTextClr = new TRGBClr(*mclbcdSrc.m_prgbTextClr);
        }
    }
    return *this;
}




// ---------------------------------------------------------------------------
//  CLASS: TMColListBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TMColListBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TMColListBox::TMColListBox() :

    TCtrlWnd()
    , m_eMCLBStyles(tCIDCtrls::EMCLBStyles::None)
    , m_pwndDisplay(nullptr)
    , m_pwndHeader(nullptr)
    , m_pwndScrollH(nullptr)
    , m_pwndScrollV(nullptr)
{
}

TMColListBox::~TMColListBox()
{
}


// ---------------------------------------------------------------------------
// TMColListBox: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMColListBox::InitFromDesc( const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisGroupChild;

    CreateMColLB
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
    );
}


tCIDLib::TVoid TMColListBox::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
}


TSize TMColListBox::szDefault() const
{
    TSize szInit(96, 10);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TMColListBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We just create ourself, which kicks off the initialization of our window.
//
tCIDLib::TVoid
TMColListBox::CreateMColLB( const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EMCLBStyles  eMCLBStyles
                            , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Make sure that clip children is on
    tCIDCtrls::EWndStyles eRealStyles = eStyles;
    eRealStyles = eRealStyles | tCIDCtrls::EWndStyles::ClipChildren;

    tCIDCtrls::EExWndStyles eRealExStyles = eExStyles;
    // eRealExStyles |= tCIDCtrls::EExWndStyles::Composited;

    // Store our class specific styles
    m_eMCLBStyles = eMCLBStyles;

    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , eRealStyles
        , eRealExStyles
        , widThis
    );
}


//
//  This is called to set (or reset) our columns. We clear the display window contents,
//  then we set up the header control to match the indicated column set, then we call
//  the display winodw and pass on the column info to him to use moving forward.
//
tCIDLib::TVoid TMColListBox::SetColumns(const TColDefs& colToSet)
{
    m_pwndDisplay->RemoveAll();

    //
    //  We have to set up an equivalent set of column info objects in terms of the header
    //  control's scheme and set them on the header.
    //
    const tCIDLib::TCard4 c4Count = colToSet.c4ElemCount();
    TColHeader::TColList colCols(c4Count);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TMCLBColDef& mclbcdCur = colToSet[c4Index];
        TColHeaderInfo& colhiCur = colCols.objAdd
        (
            TColHeaderInfo
            (
                mclbcdCur.m_strTitle, mclbcdCur.m_c4Width, mclbcdCur.m_eHJustify
            )
        );

        // Set any other stuff that isn't done in the col info ctor
        colhiCur.bBold(mclbcdCur.m_bBold);
    }
    m_pwndHeader->SetColumns(colCols);

    // Now pass the info to our display winodw
    m_pwndDisplay->SetColumns(colToSet);
}


// ---------------------------------------------------------------------------
// TMColListBox: Protected, inherited methods
// ---------------------------------------------------------------------------

// We need to keep our child windows positioned correctly
tCIDLib::TVoid
TMColListBox::AreaChanged(  const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if (bSizeChanged)
    {
        if (m_pwndHeader && m_pwndDisplay)
        {
            TArea areaHdr;
            TArea areaDisplay;
            TArea areaHScroll, areaVScroll;
            CalcChildAreas(areaClient(), areaHdr, areaDisplay, areaHScroll, areaVScroll);

            // The header height never changes, just update its width
            m_pwndHeader->SetColHdrWidth(areaHdr.c4Width());

            // The client area will only change in size
            m_pwndHeader->SetSize(areaDisplay.szArea(), kCIDLib::True);

            // Adjust the scroll bars
            m_pwndScrollH->SetSizePos(areaHScroll, kCIDLib::False);
            m_pwndScrollV->SetSizePos(areaVScroll, kCIDLib::False);
        }
    }
}


//
//  Create and initially place our two child windows
//
tCIDLib::TBoolean TMColListBox::bCreated()
{
    // Get the area inside any border
    const TArea& areaUs = areaClient();

    // Store our front height for ongoing use
    TGraphWndDev gdevTmp(*this);
    TFontMetrics fmtrDef;
    gdevTmp.QueryFontMetrics(fmtrDef);
    m_c4FontHeight = fmtrDef.c4Height();

    //
    //  Calculate a row height which is a big larger than the font size. It's room for the
    //  divider lines plus some margin.
    //
    const tCIDLib::TCard4 c4RowHeight = m_c4FontHeight + 8;

    // Calculate the sizes of our child windows based on the client area
    TArea areaHdr;
    TArea areaDisplay;
    TArea areaHScroll, areaVScroll;
    CalcChildAreas(areaUs, areaHdr, areaDisplay, areaHScroll, areaVScroll);

    //
    //  Create and place the header, setting the appropriate styles. Somne of them are driven
    //  by whether we have those styles.
    //
    m_pwndHeader = new TColHeader();
    m_pwndHeader->CreateColHeader
    (
        *this
        , kWndId_Hdr
        , tCIDCtrls::EWndStyles
          (
            tCIDCtrls::EWndStyles::VisChild
          )
    );
    m_pwndHeader->SetSizePos(areaHdr, kCIDLib::True);

    //
    //  And the display window, giving him the row height we calculated
    //
    m_pwndDisplay = new TMColListBoxImpl();
    m_pwndDisplay->CreateMCLBImpl
    (
        *this
        , kWndId_Display
        , areaDisplay
        , tCIDCtrls::EWndStyles::VisTabChild
        , c4RowHeight
    );

    // Set the width/height for the lower/right corner area. We'll update the origin as required
    m_areaLRCorner.c4Width(facCIDCtrls().c4ScrollBarWidth());
    m_areaLRCorner.c4Height(facCIDCtrls().c4ScrollBarHeight());

    // Create the scroll bars and position them
    m_pwndScrollH = new TScrollBar();
    m_pwndScrollV = new TScrollBar();

    m_pwndScrollH->CreateScrollBar
    (
        *this, kWndId_HScroll
        , areaHScroll
        , kCIDLib::True
        , tCIDCtrls::EWndStyles::VisTabChild
        , tCIDCtrls::ESBStyles::None
    );
    m_pwndScrollV->CreateScrollBar
    (
        *this
        , kWndId_VScroll
        , areaVScroll
        , kCIDLib::False
        , tCIDCtrls::EWndStyles::VisTabChild
        , tCIDCtrls::ESBStyles::None
    );

    //
    //  Initially the vertical scroll bar should be set so that there's nothing to scroll
    //  since we have no content.
    //
    m_pwndScrollV->Setup(areaDisplay.c4Height() / c4RowHeight, 0);

    // The horizontal is based on the width of the colums
    SetHorzScroll();

    return kCIDLib::True;
}


// We suppress this and only do bPaint below
tCIDLib::TBoolean TMColListBox::bEraseBgn(TGraphDrawDev& gdevDraw)
{
    return kCIDLib::True;
}


//
//  The only bit of our window area that is not covered is the border (which we don't draw even
//  if it is present, that is done for us) and the little square at the bottom right where the
//  H/V scroll bars meet. So we just fill that pre-calculated area.
//
tCIDLib::TBoolean TMColListBox::bPaint(TGraphDrawDev& gdevDraw, const TArea& areaUpdate)
{
    if (m_areaLRCorner.bIntersects(areaUpdate))
        gdevDraw.Fill(m_areaLRCorner & areaUpdate, gbrBgn());
    return kCIDLib::True;
}


// Clean up our child windows if we got them created
tCIDLib::TVoid TMColListBox::Destroyed()
{
    if (m_pwndHeader)
    {
        m_pwndHeader->Destroy();
        delete m_pwndHeader;
        m_pwndHeader = nullptr;
    }

    if (m_pwndDisplay)
    {
        m_pwndDisplay->Destroy();
        delete m_pwndDisplay;
        m_pwndDisplay = nullptr;
    }

    TParent::Destroyed();
}


// ---------------------------------------------------------------------------
// TMColListBox: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Calculates the appropripate areas for our header and display windows, within an available
//  area, which would typically be our client area.
//
tCIDLib::TVoid
TMColListBox::CalcChildAreas(const  TArea& areaAvail
                            ,       TArea& areaHdr
                            ,       TArea& areaDisplay
                            ,       TArea& areaHScroll
                            ,       TArea& areaVScroll)
{
    const tCIDLib::TCard4 c4SBHeight = facCIDCtrls().c4ScrollBarHeight();
    const tCIDLib::TCard4 c4SBWidth = facCIDCtrls().c4ScrollBarWidth();
    const tCIDLib::TCard4 c4HdrHeight = m_c4FontHeight + (m_c4FontHeight >> 1);

    //
    //  To keep things sane, we will just clip if it gets too small, so limit it to a
    //  minimum size and caculate based on that.
    //
    TArea areaActual(areaAvail);
    if (areaActual.c4Height() < c4HdrHeight + c4SBHeight + 4)
        areaActual.c4Height(c4HdrHeight + c4SBHeight + 4);
    if (areaActual.c4Width() < c4SBWidth + 4)
        areaActual.c4Width(c4SBWidth + 4);

    //
    //  The vert scroll bar goes to the far right and takes the height minus the horz
    //  SB height, taking zero if we are too small.
    //
    areaVScroll.Set(0, 0, c4SBWidth, areaActual.c4Height() - c4SBHeight);
    areaVScroll.JustifyIn(areaActual, tCIDLib::EHJustify::Right, tCIDLib::EVJustify::Top);

    // The horz scroll bar is on the bottom, left, and one SB width less than the whole width
    areaHScroll.Set(0, 0, areaActual.c4Width() - c4SBWidth, c4SBHeight);
    areaHScroll.JustifyIn(areaActual, tCIDLib::EHJustify::Left, tCIDLib::EVJustify::Bottom);

    // The header is on the top left of the vert scroll bar
    areaHdr.Set(0, 0, 0, c4HdrHeight);
    areaHdr.i4Right(areaVScroll.i4Left() - 1);

    //
    //  And the display window gets what is left. We made sure above that we have enough
    //  that it won't underflow, we'll just clip our content if it gets too small.
    //
    areaDisplay.Set(0, areaHdr.i4Bottom() + 1, areaHdr.c4Width(), 0);
    areaDisplay.i4Bottom(areaHScroll.i4Top() - 1);

    //
    //  Calculate the little area in the lower right where the two scroll bars meet. This is
    //  the only part of us visible, and we have to fill that when needed. The origin is all
    //  we need to change. We get the width/height of the scroll bars up front and set those
    //  on the area so we don't need to keep getting them.
    //
    m_areaLRCorner.JustifyIn(areaAvail, tCIDLib::EHJustify::Right, tCIDLib::EVJustify::Bottom);
}


//
//  On startup and any time we get column drag events from the header, this is called to update
//  the horizontal scroll bar.
//
tCIDLib::TVoid TMColListBox::SetHorzScroll()
{
    //
    //  Ask the header for the width of all columns. That's the full size. Our width is the
    //  visible size. We arbitrarily set the width of a 'line' to 32 pixels.
    //
    const tCIDLib::TCard4 c4ContentWidth = m_pwndHeader->c4FullWidth();
    m_pwndScrollH->Setup
    (
        m_pwndDisplay->areaWndSize().c4Width() / 32, c4ContentWidth / 32
    );
}
