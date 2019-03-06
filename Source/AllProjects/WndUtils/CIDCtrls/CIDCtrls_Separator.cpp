//
// FILE NAME: CIDCtrls_Separator.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the satic separator control window.
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
AdvRTTIDecls(TSeparatorWnd, TCtrlWnd)
AdvRTTIDecls(TVSeparatorWnd, TCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TSeparatorWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TSeparatorWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TSeparatorWnd::TSeparatorWnd() :

    TCtrlWnd()
    , m_rgbText(0, 0, 0)
{
}

TSeparatorWnd::~TSeparatorWnd()
{
}


// ---------------------------------------------------------------------------
// TSeparatorWnd: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSeparatorWnd::InitFromDesc(const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDLib::EHJustify eHJust = tCIDLib::EHJustify::Left;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisGroupChild;
    TRGBClr rgbText(m_rgbText);

    if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
        rgbText.Set(53, 85, 210);

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Center))
        eHJust = tCIDLib::EHJustify::Center;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , dlgiSrc.strText()
        , rgbText
        , eStyles
        , eHJust
    );
}


tCIDLib::TVoid TSeparatorWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Center);
}


TSize TSeparatorWnd::szDefault() const
{
    TSize szInit(96, 10);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TSeparatorWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSeparatorWnd::Create(  const   TWindow&                wndParent
                        , const tCIDCtrls::TWndId       widThis
                        , const TArea&                  areaInit
                        , const TString&                strText
                        , const TRGBClr&                rgbText
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDLib::EHJustify      eHorzJustify)
{
    //
    //  Make sure tab stop is off, since that will cause issues, and make sure
    //  that group is on.
    //
    tCIDCtrls::EWndStyles eRealStyles = eStyles;
    eRealStyles = tCIDLib::eClearEnumBits(eRealStyles, tCIDCtrls::EWndStyles::TabStop);
    eRealStyles |= eRealStyles | tCIDCtrls::EWndStyles::Group;

    // Save away the passed text color
    m_eHorzJustify = eHorzJustify;
    m_rgbText = rgbText;

    // And create the cnotrol
    CreateWnd
    (
        wndParent.hwndThis()
        , (eHorzJustify == tCIDLib::EHJustify::Left) ? kCIDCtrls::pszCustClass
                                                    : kCIDCtrls::pszCustClassHV
        , strText.pszBuffer()
        , areaInit
        , eRealStyles
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


// ---------------------------------------------------------------------------
// TSeparatorWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TSeparatorWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    const TArea areaUs = areaWndSize();

    //
    //  Get the window text and draw it, left justified and vertically centered in
    //  our area. Set the appropriate font before we query the text area.
    //
    TFontJanitor janFont(&gdevToUse, &TGUIFacility::gfontDefBold());

    // Draw a line centered vertically in our area
    TPoint pntLeft = areaUs.pntCenter();
    pntLeft.i4X(areaUs.i4Left());
    TPoint pntRight(areaUs.i4Right(), pntLeft.i4Y());
    gdevToUse.DrawLine(pntLeft, pntRight, facCIDGraphDev().rgbPaleGrey);

    //
    //  Now draw the text centered. We set the background mix mode to our own
    //  background color so we'll overwrite the line where we draw the text.
    //
    gdevToUse.SetTextColor(m_rgbText);
    gdevToUse.SetBgnColor(rgbBgnFill());
    gdevToUse.DrawString
    (
        strWndText()
        , areaUs
        , m_eHorzJustify
        , tCIDLib::EVJustify::Center
        , tCIDGraphDev::ETextFmts::None
    );


    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  CLASS: TVSeparatorWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TVSeparatorWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TVSeparatorWnd::TVSeparatorWnd() :

    TCtrlWnd()
    , m_eVSepStyles(tCIDCtrls::EVSepStyles::None)
    , m_rgbText(0, 0, 0)
{
}

TVSeparatorWnd::~TVSeparatorWnd()
{
}


// ---------------------------------------------------------------------------
// TVSeparatorWnd: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TVSeparatorWnd::InitFromDesc(const  TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisGroupChild;
    tCIDCtrls::EVSepStyles eVSStyles = tCIDCtrls::EVSepStyles::None;
    TRGBClr rgbText(m_rgbText);

    if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
        rgbText.Set(53, 85, 210);

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_LeftSide))
        eVSStyles |= tCIDCtrls::EVSepStyles::Left;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , dlgiSrc.strText()
        , rgbText
        , eStyles
        , eVSStyles
    );
}


tCIDLib::TVoid TVSeparatorWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_LeftSide);
}


TSize TVSeparatorWnd::szDefault() const
{
    TSize szInit(96, 10);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TVSeparatorWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TVSeparatorWnd::Create( const   TWindow&                wndParent
                        , const tCIDCtrls::TWndId       widThis
                        , const TArea&                  areaInit
                        , const TString&                strText
                        , const TRGBClr&                rgbText
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EVSepStyles  eSepStyles)
{
    //
    //  Make sure tab stop is off, since that will cause issues, and make sure
    //  that group is on.
    //
    tCIDCtrls::EWndStyles eRealStyles = eStyles;
    eRealStyles = tCIDLib::eClearEnumBits(eRealStyles, tCIDCtrls::EWndStyles::TabStop);
    eRealStyles |= tCIDCtrls::EWndStyles::Group;

    // Save away the passed text color and class specific styles
    m_eVSepStyles = eSepStyles;
    m_rgbText = rgbText;

    // And create the cnotrol
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , strText.pszBuffer()
        , areaInit
        , eRealStyles
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


// ---------------------------------------------------------------------------
// TVSeparatorWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TVSeparatorWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    TArea areaDraw = areaWndSize();
    areaDraw.Deflate(1, 1);

    //
    //  If left side, get the window text and draw it. We have to figure out its
    //  area when flipped vertically. We'll draw that left justified and vertically
    //  centered, and then the bracket to the right of it.
    //
    //  If right, we just draw the bracket, right justified.
    //
    TFontJanitor janFont(&gdevToUse, &TGUIFacility::gfontDefBold());

    if (tCIDLib::bAllBitsOn(m_eVSepStyles, tCIDCtrls::EVSepStyles::Left))
    {
        //
        //  Get a copy of our font metric and set up a version that has the
        //  direction set so that it draw bottom to top.
        //
        const TString& strDraw = strWndText();
        TFontSelAttrs fselDraw(gdevToUse);
        fselDraw.eDir(tCIDGraphDev::EFontDirs::D90);

        // Create a font for this
        TGUIFont gfontDraw(fselDraw);
        TFontJanitor janFont(&gdevToUse, &gfontDraw);

        // Draw the text (left justified and vertically centered, which is the default)
        gdevToUse.SetTextColor(m_rgbText);
        gdevToUse.DrawString
        (
            strDraw, areaDraw, tCIDLib::EHJustify::Left, tCIDLib::EVJustify::Bottom
        );

        // Now move over past the text and draw the bracket
        TFontMetrics fmtrDraw(gdevToUse);
        areaDraw.AdjustAndShrink(fmtrDraw.c4Height() + 2, 0);

        // Draw a vertical line on the left
        TPoint pnt1 = areaDraw.pntUL();
        TPoint pnt2 = areaDraw.pntLL();
        gdevToUse.DrawLine(pnt1, pnt2, facCIDGraphDev().rgbDarkGrey);

        // And the little top/bottom lines
        pnt2 = pnt1;
        pnt2.Adjust(4, 0);
        gdevToUse.DrawLine(pnt1, pnt2, facCIDGraphDev().rgbDarkGrey);

        pnt1 = areaDraw.pntLL();
        pnt2 = pnt1;
        pnt2.Adjust(4, 0);
        gdevToUse.DrawLine(pnt1, pnt2, facCIDGraphDev().rgbDarkGrey);
    }
     else
    {
        // Draw a line down the right edge
        TPoint pnt1 = areaDraw.pntUR();
        TPoint pnt2 = areaDraw.pntLR();
        gdevToUse.DrawLine(pnt1, pnt2, facCIDGraphDev().rgbDarkGrey);

        // And draw the small lines at the top and bottom
        pnt2 = pnt1;
        pnt2.Adjust(-4, 0);
        gdevToUse.DrawLine(pnt1, pnt2, facCIDGraphDev().rgbDarkGrey);

        pnt1 = areaDraw.pntLR();
        pnt2 = pnt1;
        pnt2.Adjust(-4, 0);
        gdevToUse.DrawLine(pnt1, pnt2, facCIDGraphDev().rgbDarkGrey);
    }
    return kCIDLib::True;
}



