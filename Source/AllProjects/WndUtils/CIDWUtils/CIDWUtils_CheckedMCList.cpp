//
// FILE NAME: CIDWUtils_CheckedMCList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/21/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the 'checked multi-column list box'.
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
AdvRTTIDecls(TCheckedMCListWnd, TMultiColListBox)




// ---------------------------------------------------------------------------
//  CLASS: TCheckedMCList
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TCheckedMCList: Constructors and Destructor
// ---------------------------------------------------------------------------
TCheckedMCListWnd::TCheckedMCListWnd() :

    TMultiColListBox()
    , m_bAutoCheck(kCIDLib::True)
    , m_bInitDone(kCIDLib::False)
{
}

TCheckedMCListWnd::~TCheckedMCListWnd()
{
}


// ---------------------------------------------------------------------------
// TCheckedMCListWnd: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We are really mainly just worried about our parent class, but we want to be
//  able to have some hints of our own.
//
tCIDLib::TVoid
TCheckedMCListWnd::InitFromDesc(const   TWindow&                wndParent
                                , const TDlgItem&               dlgiSrc
                                , const tCIDCtrls::EWndThemes   eTheme)
{
    // Just call our parent to do most of the work
    TParent::InitFromDesc(wndParent, dlgiSrc, eTheme);

    // If we have the no-auto-check hint, then clear the default setting
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_NoAutoCheck))
        bAutoCheck(kCIDLib::False);
}


tCIDLib::TVoid TCheckedMCListWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_NoAutoCheck);
}


// ---------------------------------------------------------------------------
// TCheckedMCListWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TCheckedMCListWnd::bAutoCheck() const
{
    return m_bAutoCheck;
}

tCIDLib::TBoolean TCheckedMCListWnd::bAutoCheck(const tCIDLib::TBoolean bToSet)
{
    m_bAutoCheck = bToSet;
    return m_bAutoCheck;
}


tCIDLib::TBoolean TCheckedMCListWnd::bIsCheckedAt(const tCIDLib::TCard4 c4At) const
{
    return bFlagAt(c4At);
}


// Store the passed flag and redraw the row
tCIDLib::TVoid
TCheckedMCListWnd::SetCheckAt(const tCIDLib::TCard4 c4At, const tCIDLib::TBoolean bState)
{
    SetFlagAt(c4At, bState);
    RedrawRow(c4At);
}



// ---------------------------------------------------------------------------
// TCheckedMCListWnd: Protected, inherited method
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TCheckedMCListWnd::bCreated()
{
    // For now, nothing to do but pass it to our parent
    return TParent::bCreated();
}


//
//  If in auto-check mode, then we handle a invocation and toggle the check mark
//  ourself. An invoked notification should still be sent so that they can react
//  to it, so we still return false that we didn't handle it.
//
tCIDLib::TBoolean
TCheckedMCListWnd::bInvoked(const tCIDLib::TCard4 c4Row, const tCIDLib::TCard4)
{
    if (m_bAutoCheck)
        SetCheckAt(c4Row, !bIsCheckedAt(c4Row));
    return kCIDLib::False;
}


//
//  Handle custom drawing. We use it to draw a grid around our cells, and to bold
//  the font in the name column. We only enable column level callbacks, so that's
//  all we have to check for.
//
tCIDCtrls::EMCLBCustRets
TCheckedMCListWnd::eCustomDraw(         TGraphDrawDev&      gdevTar
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
        //
        //  If it's the 0th column, override it and jut display our check mark (or not)
        //
        if ((c4Row != kCIDLib::c4MaxCard) && !c4Column)
        {
            if (bFlagAt(c4Row))
            {
                // Query the area of this cell
                QueryColArea(c4Row, 0, m_areaCell);

                // And draw the check mark centered
                m_areaTar = m_areaBmpSrc;
                m_areaTar.CenterIn(m_areaCell);
                gdevTar.DrawBitmap(m_bmpCheck, m_areaBmpSrc, m_areaTar);
            }

            // Tell him to do nothing
            eRet = tCIDCtrls::EMCLBCustRets::DontDraw;
        }
    }
    return eRet;
}


//
//  When the columns are set, make sure our 0th one is set to fit the check mark.
//
tCIDLib::TVoid TCheckedMCListWnd::ColumnsSet(const tCIDLib::TCard4 c4Count)
{
    // Fault in some init if not done yet
    if (!m_bInitDone)
    {
        m_bInitDone = kCIDLib::True;

        // Load our checkmark bitmap
        m_bmpCheck = facCIDWUtils().bmpLoadSysBmp(tCIDCtrls::ESysBmps::Check);

        // Set up the source area of the checkmark bitmap
        m_areaBmpSrc.Set(TPoint::pntOrigin, m_bmpCheck.szBitmap());

        //
        //  Set custom draw options. These are the phases that we need to be called
        //  back on. We only care about pre-column.
        //
        SetCustomDrawOpts(tCIDCtrls::EMCLBCustOpts(tCIDCtrls::EMCLBCustOpts::PreCol));
    }

    if (c4Count)
    {
        tCIDLib::TCard4 c4Width = m_areaBmpSrc.c4Width() * 2;
        SetColOpts(0, c4Width, tCIDLib::EHJustify::Left, 0);
    }
}

