//
// FILE NAME: CIDWUtils_2ColSectList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/20/2016
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
//  This file implements the a 2 column, sectional list box.
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
AdvRTTIDecls(T2ColSectList,TMultiColListBox)



// ---------------------------------------------------------------------------
//  CLASS: T2ColSectList
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// T2ColSectList: Constructors and Destructor
// ---------------------------------------------------------------------------
T2ColSectList::T2ColSectList() :

    m_colTmpCols(3)
{
    // Preload it so we acn just assign values
    m_colTmpCols.objAdd(TString::strEmpty());
    m_colTmpCols.objAdd(TString::strEmpty());
    m_colTmpCols.objAdd(TString::strEmpty());
}

T2ColSectList::~T2ColSectList()
{
}


// ---------------------------------------------------------------------------
//  T2ColSectList: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
T2ColSectList::InitFromDesc(const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    // For now, just pass it on
    TParent::InitFromDesc(wndParent, dlgiSrc, eTheme);
}


tCIDLib::TVoid T2ColSectList::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    // For now, just pass it on
    TParent::QueryHints(colToFill);
}


// ---------------------------------------------------------------------------
//  T2ColSectList: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Some conveniences to add our two types of rows
tCIDLib::TVoid
T2ColSectList::AddDataRow(  const   TString&        strKey
                            , const TString&        strValue
                            , const tCIDLib::TCard4 c4RowId)
{
    m_colTmpCols[1] = strKey;
    m_colTmpCols[2] = strValue;
    c4AddItem(m_colTmpCols, c4RowId);
}

tCIDLib::TVoid T2ColSectList::AddSectTitle(const TString& strTitle)
{
    m_colTmpCols[1] = strTitle;
    m_colTmpCols[2].Clear();
    const tCIDLib::TCard4 c4Index = c4AddItem(m_colTmpCols, kCIDLib::c4MaxCard);

    // Set the row flag to true for this new row
    SetFlagAt(c4Index, kCIDLib::True);
}


// ---------------------------------------------------------------------------
//  T2ColSectList: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean T2ColSectList::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    //
    //  Set our columns. They can set it via the window text in the form col1/col2, so
    //  check that. If not set, then we provide a default.
    //
    TString strCol1 = strWndText();

    m_colTmpCols[0].Clear();
    m_colTmpCols[1] = strWndText();
    if (!m_colTmpCols[1].bIsEmpty())
        m_colTmpCols[1].bSplit(m_colTmpCols[2], kCIDLib::chForwardSlash);

    if (m_colTmpCols[1].bIsEmpty())
        m_colTmpCols[1] = L"Setting Name";
    if (m_colTmpCols[2].bIsEmpty())
        m_colTmpCols[2] = L"Setting Value";
    SetColumns(m_colTmpCols);

    // Set the 0th column to zero size. The rest the caller can size as he sees fit
    SetColOpts(0, 0, tCIDLib::EHJustify::Left);

    // Right justify the first column
    SetColOpts(1, 0, tCIDLib::EHJustify::Right);

    //
    //  And auto-size it to the title. The caller may later size it up again to fit the
    //  actual content.
    //
    AutoSizeCol(1, kCIDLib::True);

    //
    //  Set custom draw options. These are the phases that we need to be called
    //  back on.
    //
    SetCustomDrawOpts
    (
        tCIDLib::eOREnumBits
        (
            tCIDCtrls::EMCLBCustOpts::PreRow
            , tCIDCtrls::EMCLBCustOpts::PostDraw
            , tCIDCtrls::EMCLBCustOpts::PostRow
            , tCIDCtrls::EMCLBCustOpts::PreCol
        )
    );

    // Let creation continue
    return kCIDLib::True;
}


tCIDLib::TBoolean T2ColSectList::bEraseBgn(TGraphDrawDev& gdevToUse)
{
    return TParent::bEraseBgn(gdevToUse);
}


// We don't want anything to happen here
tCIDLib::TVoid
T2ColSectList::CellClicked(  const   tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bLeftButton)
{
}


//
//  Handle custom drawing. We use use the pre-row to bgn fill rows that are marked
//  and to set the return colors.
//
tCIDCtrls::EMCLBCustRets
T2ColSectList::eCustomDraw(          TGraphDrawDev&      gdevTar
                            , const tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bPost
                            , const TArea&              areaAt
                            ,       TRGBClr&            rgbBgn
                            ,       TRGBClr&            rgbText)
{
    tCIDCtrls::EMCLBCustRets eRet = tCIDCtrls::EMCLBCustRets::None;
    if (bPost)
    {
        if ((c4Row == kCIDLib::c4MaxCard) && (c4Column == kCIDLib::c4MaxCard))
        {
            //
            //  Its the final post draw, so draw our vertical separators. We get the
            //  area of the 2nd and 3rd cells, and those provide the right sides
            //  of the lines. And we get the area of the last row, and that provides
            //  the bottom of the line. If it's off the end of the window, we just
            //  let windows clip it.
            //
            const tCIDLib::TCard4 c4Count = c4ItemCount();
            if (c4Count)
            {
                TArea areaName;
                TArea areaVal;
                QueryColArea(0, 1, areaName);
                QueryColArea(0, 2, areaVal);

                TArea areaLast;
                QueryRowArea(c4Count - 1, areaLast);

                //
                //  Since we are drawing lines we have to adjust for the exclusive nature
                //  of the right/bottom points.
                //
                TPoint pntTop(areaName.i4Right() - 1, areaName.i4Y());
                TPoint pntBottom(pntTop.i4X(), areaLast.i4Bottom() - 1);
                gdevTar.DrawLine(pntTop, pntBottom, facCIDGraphDev().rgbPaleGrey);

                pntTop.i4X(areaVal.i4Right() - 1);
                pntBottom.i4X(pntTop.i4X());
                gdevTar.DrawLine(pntTop, pntBottom, facCIDGraphDev().rgbPaleGrey);
            }
        }
         else if ((c4Row != kCIDLib::c4MaxCard) && (c4Column == kCIDLib::c4MaxCard))
        {
            //
            //  It's post row, so we draw a line under our row. Since we are drawing
            //  lines, we have to adjust for the non-exclusive nature of the right
            //  bottom points.
            //
            TArea areaRow;
            QueryRowArea(c4Row, areaRow);
            areaRow.AdjustSize(-1, -1);
            gdevTar.DrawLine(areaRow.pntLL(), areaRow.pntLR(), facCIDGraphDev().rgbPaleGrey);
        }
    }
     else
    {
        if ((c4Row < c4ItemCount()) && (c4Column == kCIDLib::c4MaxCard))
        {
            // It's pre-row. If this one is a separator, we want to color fill it.
            if (bFlagAt(c4Row))
            {
                rgbBgn = facCIDGraphDev().rgbPaleGrey;
                eRet = tCIDLib::eOREnumBits(eRet, tCIDCtrls::EMCLBCustRets::NewBgnClr);
            }
        }
         else
        {
            // For column 1, set bold font, else for a valid column set normal
            if (c4Column == 1)
            {
                gdevTar.hfontSetFont(TGUIFacility::gfontDefBold());
                eRet = tCIDCtrls::EMCLBCustRets::NewFont;
            }
             else if (c4Column != kCIDLib::c4MaxCard)
            {
                gdevTar.hfontSetFont(TGUIFacility::gfontDefNorm());
                eRet = tCIDCtrls::EMCLBCustRets::NewFont;
            }
        }
    }
    return eRet;
}


