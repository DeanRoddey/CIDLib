//
// FILE NAME: CIDWUtils_ColoredList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/20/2016
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements an otherwise regular multi-column list box, but allows rows
//  to be colored when the user flag is set.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TColoredList,TMultiColListBox)



// ---------------------------------------------------------------------------
//  CLASS: TColoredList
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TColoredList: Constructors and Destructor
// ---------------------------------------------------------------------------
TColoredList::TColoredList()
{
}

TColoredList::~TColoredList()
{
}


// ---------------------------------------------------------------------------
//  TColoredList: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TColoredList::InitFromDesc(const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    // For now, just pass it on
    TParent::InitFromDesc(wndParent, dlgiSrc, eTheme);
}


tCIDLib::TVoid TColoredList::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    // For now, just pass it on
    TParent::QueryHints(colToFill);
}


// ---------------------------------------------------------------------------
//  TColoredList: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Some conveniences to add our two types of rows
tCIDLib::TVoid
TColoredList::SetMarkedColors(const TRGBClr& rgbBgn, const TRGBClr& rgbText)
{
    m_rgbBgn = rgbBgn;
    m_rgbText = rgbText;
}



// ---------------------------------------------------------------------------
//  TColoredList: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TColoredList::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    //
    //  Set custom draw options. These are the phases that we need to be called
    //  back on.
    //
    SetCustomDrawOpts(tCIDCtrls::EMCLBCustOpts::PreRow);

    // Let creation continue
    return kCIDLib::True;
}


tCIDLib::TBoolean TColoredList::bEraseBgn(TGraphDrawDev& gdevToUse)
{
    return TParent::bEraseBgn(gdevToUse);
}


// We don't want anything to happen here
tCIDLib::TVoid
TColoredList::CellClicked(  const   tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bLeftButton)
{
}


//
//  Handle custom drawing. We use it to draw a grid around our cells, and to bold
//  the font in the name column.
//
tCIDCtrls::EMCLBCustRets
TColoredList::eCustomDraw(          TGraphDrawDev&      gdevTar
                            , const tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bPost
                            , const TArea&              areaAt
                            ,       TRGBClr&            rgbBgn
                            ,       TRGBClr&            rgbText)
{
    tCIDCtrls::EMCLBCustRets eRet = tCIDCtrls::EMCLBCustRets::None;
    if (!bPost)
    {
        if ((c4Row < c4ItemCount()) && (c4Column == kCIDLib::c4MaxCard))
        {
            // It's pre-row. If this one is a separator, we want to set the colors
            if (bFlagAt(c4Row))
            {
                rgbBgn = m_rgbBgn;
                m_rgbText = m_rgbText;
                eRet = tCIDLib::eOREnumBits(eRet, tCIDCtrls::EMCLBCustRets::NewBgnClr);
                eRet = tCIDLib::eOREnumBits(eRet, tCIDCtrls::EMCLBCustRets::NewTextClr);
            }
        }
    }
    return eRet;
}
