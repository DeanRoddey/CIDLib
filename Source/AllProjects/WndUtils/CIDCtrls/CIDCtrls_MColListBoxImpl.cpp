//
// FILE NAME: CIDCtrls_MColListBoxImpl.cpp
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
RTTIDecls(TMColListBoxImpl, TWindow)


// ---------------------------------------------------------------------------
//  CLASS: TMCLBRow
// PREFIX: mclbr
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  Constructors and Destructor
// -------------------------------------------------------------------
TMCLBRow::TMCLBRow(TMColListBoxImpl& wndCont, const tCIDLib::TCard4 c4ColCount) :

    m_bTitle(kCIDLib::False)
    , m_colCells(c4ColCount)
    , m_rgbBgn(wndCont.rgbBgnFill())
{
}

TMCLBRow::~TMCLBRow()
{
}




// ---------------------------------------------------------------------------
//  CLASS: TMColListBoxImpl
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TMColListBoxImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TMColListBoxImpl::TMColListBoxImpl() :

    TWindow(kCIDLib::True)
    , m_c4RowHeight(24)
    , m_colRows(tCIDLib::EAdoptOpts::Adopt, 256)
    , m_pwndParent(nullptr)
{
}

TMColListBoxImpl::~TMColListBoxImpl()
{
}


// ---------------------------------------------------------------------------
// TMColListBoxImpl: Public, inherited methods
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// TMColListBoxImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMColListBoxImpl::CreateMCLBImpl(       TMColListBox&           wndParent
                                , const tCIDCtrls::TWndId       widThis
                                , const TArea&                  areaInit
                                , const tCIDCtrls::EWndStyles   eStyles
                                , const tCIDLib::TCard4         c4RowHeight)
{
    // Keep around a pointer to our parent and store the provided row height
    m_c4RowHeight = c4RowHeight;
    m_pwndParent = &wndParent;


    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , eStyles
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


// We just clear the row list and do force a redraw
tCIDLib::TVoid TMColListBoxImpl::RemoveAll()
{
    m_colRows.RemoveAll();

    if (bIsValid())
        Redraw(tCIDCtrls::ERedrawFlags::Immediate);
}


//
//  Replace our current column layout info. We remove any rows if there are any. These new
//  columns will be used for any subsequent operations.
//
tCIDLib::TVoid TMColListBoxImpl::SetColumns(const TMColListBox::TColDefs& colToSet)
{
    if (!m_colRows.bIsEmpty())
    {
        m_colRows.RemoveAll();
        if (bIsValid())
            Redraw(tCIDCtrls::ERedrawFlags::Immediate);
    }

    m_colCols = colToSet;
}



// ---------------------------------------------------------------------------
// TMColListBoxImpl: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TMColListBoxImpl::bCreated()
{


    return kCIDLib::True;
}


//
//  We suppress this. We have to be very efficient so we handle it during the paint
//  operation, only filling any area not covered by cells, which is often none.
//
tCIDLib::TBoolean TMColListBoxImpl::bEraseBgn(TGraphDrawDev&)
{
    // Return true to say we did it
    return kCIDLib::True;
}


//
//  We have to figure out what of our area is covered in cell content and what is bgn.
//  The bgn we fill, and the cell content area we have to let the cells draw into.
//
//  Our scroll bars tell us where our cell content is scrolled to.
//
tCIDLib::TBoolean
TMColListBoxImpl::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    const TArea areaUs = areaWndSize();

    gdevToUse.Fill(areaUpdate, rgbBgnFill());

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
// TMColListBoxImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

