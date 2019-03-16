//
// FILE NAME: TestCIDWnd_EdTest.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/25/2014
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This module implements a simple dialog that just creates a line editor
//  window for us to test with.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "TestCIDWnd.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TEdTestDlg,TDlgBox)


// ---------------------------------------------------------------------------
//  TEdTestDlg: Constructors and destructor
// ---------------------------------------------------------------------------
TEdTestDlg::TEdTestDlg()
{
}

TEdTestDlg::~TEdTestDlg()
{
    Destroy();
}


// ---------------------------------------------------------------------------
//  TEdTestDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TEdTestDlg::bRunDlg(TFrameWindow& wndOwner)
{
    if (c4Run(wndOwner, facTestCIDWnd, kTestRes::ridDlg_EdTest))
        return kCIDLib::True;
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  TEdTestDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TEdTestDlg::bPreLoop()
{
    // Call our parent first
    TParent::bPreLoop();

    // Get our client area and move inwards a bit, then create our editor window there
    TArea areaEd = areaAdjustedClient();
    areaEd.Deflate(8);
    m_wndEd.CreateEditor
    (
        *this, tCIDWnd::EWndStyles_VisibleChild, areaEd, 1
    );

    // Indicate that the dialog box should be made visible now
    return kCIDLib::True;
}


tCIDLib::TVoid TEdTestDlg::Destroyed()
{
    // Clean up our editor first so the parent won't try to
    if (m_wndEd.bIsValid())
        m_wndEd.Destroy();

    TParent::Destroyed();
}


tCIDLib::TVoid
TEdTestDlg::Paint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    TParent::Paint(gdevToUse, areaUpdate);

}


tCIDLib::TVoid TEdTestDlg::SetChildFocus()
{
    m_wndEd.TakeFocus();
}

