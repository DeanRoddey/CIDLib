//
// FILE NAME: CIDWUtils_SectionalList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/06/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a sectionion multi-column list box.
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
AdvRTTIDecls(TSectionalList,TMultiColListBox)



// ---------------------------------------------------------------------------
//  CLASS: TSectionalList
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TSectionalList: Constructors and Destructor
// ---------------------------------------------------------------------------
TSectionalList::TSectionalList()
{
}

TSectionalList::~TSectionalList()
{
}


// ---------------------------------------------------------------------------
//  TSectionalList: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSectionalList::InitFromDesc(const  TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    // For now, just pass it on
    TParent::InitFromDesc(wndParent, dlgiSrc, eTheme);
}


tCIDLib::TVoid TSectionalList::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    // For now, just pass it on
    TParent::QueryHints(colToFill);
}


// ---------------------------------------------------------------------------
//  TSectionalList: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper to add a divider line. We just create a string list with all empty values
//  but for the first column. Once added, we set the row flag to true.
//
tCIDLib::TCard4 TSectionalList::c4AddDivider(const TString& strText)
{
    const tCIDLib::TCard4 c4Count = c4ColCount();
    tCIDLib::TStrList colCols(c4Count);
    colCols.objAdd(strText);
    for (tCIDLib::TCard4 c4Index = 1; c4Index < c4Count; c4Index++)
        colCols.objAdd(TString::strEmpty());

    const tCIDLib::TCard4 c4At = c4AddItem(colCols, kCIDLib::c4MaxCard);
    SetFlagAt(c4At, kCIDLib::True);

    return c4At;
}



// ---------------------------------------------------------------------------
//  TSectionalList: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TSectionalList::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    //
    //  Set custom draw options. These are the phases that we need to be called
    //  back on.
    //
    SetCustomDrawOpts
    (
        tCIDLib::eOREnumBits
        (
            tCIDCtrls::EMCLBCustOpts::PreRow, tCIDCtrls::EMCLBCustOpts::PreCol
        )
    );

    // Let creation continue
    return kCIDLib::True;
}


tCIDLib::TBoolean TSectionalList::bEraseBgn(TGraphDrawDev& gdevToUse)
{
    return TParent::bEraseBgn(gdevToUse);
}


// We don't want anything to happen here
tCIDLib::TVoid
TSectionalList::CellClicked(const   tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bLeftButton)
{
}


//
//  Handle custom drawing. We use it to draw a grid around our cells, and to bold
//  the font in the name column.
//
tCIDCtrls::EMCLBCustRets
TSectionalList::eCustomDraw(        TGraphDrawDev&      gdevTar
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
            //
            //  It's pre-row. If this one is a separator, we want to color fill it, and
            //  draw the column 0 text, ignoring any columns. Then we tell the base class
            //  not to do any more drawing on this one.
            //
            if (bFlagAt(c4Row))
            {
                TArea areaRow;
                QueryRowArea(c4Row, areaRow);

                // Fill the row
                gdevTar.Fill(areaRow, facCIDGraphDev().rgbPaleGrey);

                // Set the font and draw  the divider title
                gdevTar.hfontSetFont(TGUIFacility::gfontDefBold());
                TString strText;
                QueryColText(c4Row, 0, strText);
                areaRow.Deflate(2, 2);
                gdevTar.DrawString(strText, areaRow);

                eRet = tCIDCtrls::EMCLBCustRets::DontDraw;
            }
        }
         else if ((c4Row < c4ItemCount()) && (c4Column <= c4ColCount()))
        {
            //
            //  If it's a divider row, we just say don't draw any more. We drew it at the
            //  pre-row level. If it's not, then we draw a separator line to the right and
            //  then allow the normal drawing to occur.
            //
            if (bFlagAt(c4Row))
            {
                eRet = tCIDCtrls::EMCLBCustRets::DontDraw;
            }
             else
            {
                TArea areaCur;
                QueryColArea(c4Row, c4Column, areaCur);
                gdevTar.DrawLine(areaCur.pntUR(), areaCur.pntLR(), facCIDGraphDev().rgbPaleGrey);
            }
        }
    }
    return eRet;
}


