//
// FILE NAME: CIDCtrls_StaticColor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/15/2015
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
//  This file implements the static color widget.
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
AdvRTTIDecls(TStaticColor, TCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TStaticColor
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TStaticColor: Constructors and Destructor
// ---------------------------------------------------------------------------
TStaticColor::TStaticColor() :

    TCtrlWnd()
    , m_eSClrStyle(tCIDCtrls::ESClrStyles::None)
{
}

TStaticColor::~TStaticColor()
{
}



// ---------------------------------------------------------------------------
// TStaticColor: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticColor::InitFromDesc( const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ESClrStyles eSClrStyle = tCIDCtrls::ESClrStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;
    TRGBClr rgbFill;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReportClicks))
        eSClrStyle |= tCIDCtrls::ESClrStyles::ReportClicks;


    // Set a border unless asked not to
    if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        eStyles |= tCIDCtrls::EWndStyles::Border;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , rgbFill
        , eSClrStyle
        , eExStyles
    );
}


tCIDLib::TVoid TStaticColor::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_ReportClicks);
}


// ---------------------------------------------------------------------------
// TStaticColor: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticColor::Create( const TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const TRGBClr&                rgbFill
                    , const tCIDCtrls::ESClrStyles  eSClrStyle
                    , const tCIDCtrls::EExWndStyles eExStyles)
{
    //
    //  Make sure that the tab and group are off. They have no meaning for this type
    //  and can cause troubles.
    //
    tCIDCtrls::EWndStyles eRealStyles = tCIDLib::eClearEnumBits
    (
        eStyles, tCIDCtrls::EWndStyles::TabGroup
    );

    // Store our class specific styles
    m_eSClrStyle = eSClrStyle;

    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eRealStyles
        , eExStyles
        , widThis
    );

    SetBgnColor(rgbFill);
}

tCIDLib::TVoid
TStaticColor::Create( const TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::ESClrStyles  eSClrStyle
                    , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Call the other version with a default color
    Create
    (
        wndParent
        , widThis
        , areaInit
        , eStyles
        , facCIDGraphDev().rgbBlack
        , eSClrStyle
        , eExStyles
    );
}


// Set or change the fill color
tCIDLib::TVoid TStaticColor::SetColor(const TRGBClr& rgbToSet)
{
    // Just set it as our background color
    SetBgnColor(rgbToSet);
}



// ---------------------------------------------------------------------------
// TStaticColor: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TStaticColor::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    TParent::bPaint(gdevToUse, areaUpdate);

    // If we have the border style, draw it
    if (bStyleOn(tCIDCtrls::EWndStyles::Border))
        gdevToUse.Stroke(areaWndSize(), facCIDGraphDev().rgbBlack);
    else if (bExStyleOn(tCIDCtrls::EExWndStyles::SunkenEdge))
        DrawBorder(gdevToUse, areaWndSize(), kCIDLib::True, kCIDLib::False);

    return kCIDLib::True;
}


//
//  We have to report clicks to our parent if our style indicates so. So we override
//  this to watch for those clicks.
//
tCIDLib::TBoolean
TStaticColor::bClick(const  tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClick
                    , const TPoint&
                    , const tCIDLib::TBoolean
                    , const tCIDLib::TBoolean)

{
    //
    //  If a left down or left double and we have the report clicks style, then
    //  let's report it.
    //
    if (tCIDLib::bAllBitsOn(m_eSClrStyle, tCIDCtrls::ESClrStyles::ReportClicks)
    &&  ((eClick == tCIDCtrls::EMouseClicks::Down)
    ||   (eClick == tCIDCtrls::EMouseClicks::Double))
    &&  (eButton == tCIDCtrls::EMouseButts::Left))
    {
        PassStaticChildClick(eButton, eClick);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


