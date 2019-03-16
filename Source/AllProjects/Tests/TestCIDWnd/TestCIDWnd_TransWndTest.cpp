//
// FILE NAME: TestCIDWnd_TransWndTest.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/17/2005
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
//  This is the header for the TestCIDWnd_TransWndTest.Cpp file. This guy
//  implements a trivial derivative of the layered window class, for testing
//  layered window functionality.
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
RTTIDecls(TTransWindow,TLayeredWindow)


// ---------------------------------------------------------------------------
//  CLASS: TTransWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TTransWindow: Constructors and destructor
// ---------------------------------------------------------------------------
TTransWindow::TTransWindow() :

    m_c4Frame(1)
{
}

TTransWindow::~TTransWindow()
{
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTransWindow::Create(const  TFrameWindow&   wndOwner
                    , const TBitmap&        bmpShow1
                    , const TBitmap&        bmpShow2)
{
    m_bmpShow1 = bmpShow1;
    m_bmpShow2 = bmpShow2;
    TArea areaInit(TPoint(100, 100), bmpShow1.szBitmap());
    CreateLayered(wndOwner, areaInit, kCIDLib::True, 0xA0);

    EnableTimer(tCIDWnd::ETimer_One, 250);
}


// ---------------------------------------------------------------------------
//  Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTransWindow::DrawLayered(TGraphDrawDev& gdevToUse)
{
    if (m_c4Frame == 1)
        gdevToUse.AlphaBlit(m_bmpShow1, TPoint::pntOrigin, 0xFF, kCIDLib::True);
    else
        gdevToUse.AlphaBlit(m_bmpShow2, TPoint::pntOrigin, 0xFF, kCIDLib::True);
}


tCIDLib::TVoid TTransWindow::Timer(const tCIDWnd::ETimers eTimer)
{
    if (m_c4Frame == 1)
        m_c4Frame = 2;
    else
        m_c4Frame = 1;

    LayeredRedraw();
}


