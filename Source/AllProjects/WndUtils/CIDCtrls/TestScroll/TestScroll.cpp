//
// CID_FILE NAME: TestCIDCtrls2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/1997
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
//  This is the main module of the program. This program tests the pane window
//  container/layout manager stuff.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestScroll.hpp"



// ----------------------------------------------------------------------------
//  Local, const data
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);


// ----------------------------------------------------------------------------
//  Magic main module code
// ----------------------------------------------------------------------------
CIDLib_CtrlsModule(TThread(L"MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TTestFrameWnd, TFrameWnd)


// ---------------------------------------------------------------------------
//  Global data
// ---------------------------------------------------------------------------
TGUIFacility facTestScroll
(
    L"TestScroll"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::HasMsgFile
);



// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TTextFrameWnd: PConstructors and Destructor
// ----------------------------------------------------------------------------
TTestFrameWnd::TTestFrameWnd() :

    m_bPaused(kCIDLib::True)
    , m_i4XOfs(1)
    , m_i4YOfs(1)
    , m_tmidMove(kCIDCtrls::tmidInvalid)
{
}

TTestFrameWnd::~TTestFrameWnd()
{
}


// ----------------------------------------------------------------------------
//  TTestFrameWnd: Public, non-virtual method
// ----------------------------------------------------------------------------

tCIDLib::TVoid TTestFrameWnd::CreateMain()
{
    CreateFrame
    (
        nullptr
        , facCIDCtrls().areaDefWnd()
        , L"TestScroll Frame"
        , tCIDCtrls::EWndStyles::StdFrameNV
        , tCIDCtrls::EExWndStyles::None
        , tCIDCtrls::EFrameStyles::None
        , kCIDLib::False
    );
}


// ----------------------------------------------------------------------------
//  TTestFrameWnd: Protected, inherited method
// ----------------------------------------------------------------------------

tCIDLib::TVoid
TTestFrameWnd::AreaChanged( const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

}


tCIDLib::TBoolean
TTestFrameWnd::bClick(  const   tCIDCtrls::EMouseButts  eButton
                        , const tCIDCtrls::EMouseClicks eClickType
                        , const TPoint&                 pntAt
                        , const tCIDLib::TBoolean       bCtrlShift
                        , const tCIDLib::TBoolean       bShift)
{
    // We only care about left down
    if ((eButton == tCIDCtrls::EMouseButts::Left)
    &&  (eClickType == tCIDCtrls::EMouseClicks::Down))
    {
        m_bPaused = !m_bPaused;
        return kCIDLib::True;
    }

    return kCIDLib::False;
}


//
//  When we are created, in turn create all of our child windows, which we do manually
//  in this simple test.
//
tCIDLib::TBoolean TTestFrameWnd::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    m_bmpTest = facCIDCtrls().bmpLoadLargeAppImg(L"Computer System");
    m_areaBmp.Set(TPoint(12, 10), m_bmpTest.szBitmap());

//    SetBgnColor(facCIDGraphDev().rgbPaleGrey);

    // Start a timer to move the bitmap around
    m_tmidMove = tmidStartTimer(5);
    return kCIDLib::True;
}


tCIDLib::TBoolean TTestFrameWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    // Do the initial draw of the bitmap
    static tCIDLib::TBoolean bFirst = kCIDLib::True;
    if (bFirst)
    {
        /*
        TArea areaSrc(m_areaBmp);
        areaSrc.ZeroOrg();

        TGraphWndDev gdevTar(*this);
        gdevTar.AlphaBlit
        (
            m_bmpTest
            , areaSrc
            , m_areaBmp
            , 0xFF
            , kCIDLib::True
            , kCIDLib::False
        );
        */

        TString strText(L"Some Text to Scroll");
        TFontSelAttrs fselTest(gdevToUse);
        fselTest.bScalable(kCIDLib::True);
        fselTest.eWeight(tCIDGraphDev::EFontWeights::Light);
        fselTest.strFaceName(L"Segoe UI Light");
        TGUIFont gfontTest(fselTest);
        TFontJanitor janFont(&gdevToUse, &gfontTest);

        m_areaBmp = gdevToUse.areaString(strText);
        m_areaBmp.Inflate(1);
        m_areaBmp.SetOrg(10, 11);

        gdevToUse.SetTextColor(facCIDGraphDev().rgbDarkGrey);
        gdevToUse.DrawString(strText, m_areaBmp, tCIDLib::EHJustify::Center);

        bFirst = kCIDLib::False;
    }

//    gdevToUse.Fill(areaUpdate, rgbBgnFill());

    return kCIDLib::True;
}


tCIDLib::TVoid TTestFrameWnd::Destroyed()
{
    if (m_tmidMove != kCIDCtrls::tmidInvalid)
    {
        StopTimer(m_tmidMove);
        m_tmidMove = kCIDCtrls::tmidInvalid;
    }

    TParent::Destroyed();
}


tCIDLib::TVoid TTestFrameWnd::Timer(const tCIDCtrls::TTimerId tmidSrc)
{
    if ((tmidSrc == m_tmidMove) && !m_bPaused)
    {
        // Get the new adjusted position
        TArea areaNew(m_areaBmp);
        areaNew.AdjustOrg(m_i4XOfs, m_i4YOfs);

        // If either direction is outside of our client area, flip it
        const TArea& areaLim = areaClient();
        if ((areaNew.i4X() < areaLim.i4X()) || (areaNew.i4Right() > areaLim.i4Right()))
            m_i4XOfs *= -1;
        if ((areaNew.i4Y() < areaLim.i4Y()) || (areaNew.i4Bottom() > areaLim.i4Bottom()))
            m_i4YOfs *= -1;

        // Now adjust our actual area, keeping the original as well
        areaNew = m_areaBmp;
        m_areaBmp.AdjustOrg(m_i4XOfs, m_i4YOfs);

        // The area we actually scroll is the combination of these
        TArea areaToScroll(areaNew | m_areaBmp);

        // And scroll the bitmap to this new position, no invalidation
//        TGraphWndDev::WaitRetrace();
        TArea areaInvalid;
        Scroll
        (
            areaToScroll
            , areaToScroll
            , areaInvalid
            , m_i4XOfs
            , m_i4YOfs
            , kCIDLib::False
        );
    }
}


// ----------------------------------------------------------------------------
//  TTestFrameWnd: Private, non-virtual method
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Create a frame window
        TTestFrameWnd wndMain;
        wndMain.CreateMain();
        facCIDCtrls().ShowGUI(wndMain);
        facCIDCtrls().uMainMsgLoop(wndMain);
        wndMain.Destroy();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
    }

    return tCIDLib::EExitCodes::Normal;
}

