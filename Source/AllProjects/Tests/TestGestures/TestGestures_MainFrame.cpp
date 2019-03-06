//
// FILE NAME: TestGestures_MainFrame.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the main window classes
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
#include    "TestGestures.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TTestGest, TTouchWnd)
RTTIDecls(TGestMainWnd, TFrameWnd)



// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Create a GUI facility so that we can load resources
// ---------------------------------------------------------------------------
static TGUIFacility facTestGestures
(
    L"TestGestures"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::None
);


// ----------------------------------------------------------------------------
//  Magic main module code
// ----------------------------------------------------------------------------
CIDLib_CtrlsModule(TThread(L"MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Create teh main window
    tCIDLib::TUInt uReturn = 0;
    TGestMainWnd wndMain;
    wndMain.CreateFrame();

    uReturn = facCIDCtrls().uMainMsgLoop(wndMain);
    wndMain.Destroy();

    return tCIDLib::EExitCodes::Normal;
}



// ---------------------------------------------------------------------------
//  CLASS: TTestGest
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTestGest: Constructors and Destructor
// ---------------------------------------------------------------------------
TTestGest::TTestGest() :

    m_gbrFill(facCIDGraphDev().rgbBlack)
{
}

TTestGest::~TTestGest()
{
}


// ---------------------------------------------------------------------------
//  TTestGest: Protected, inheritd methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TTestGest::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean
TTestGest::bProcessGestEv(  const   tCIDCtrls::EGestEvs eEv
                            , const TPoint&             pntStart
                            , const TPoint&             pntAt
                            , const TPoint&             pntDelta
                            , const tCIDLib::TBoolean   bTwoFingers)
{
    if (eEv == tCIDCtrls::EGestEvs::Start)
    {
        // Move the circle to here
        TGraphWndDev gdevTmp(*this);
        MoveIndicator(gdevTmp, pntAt);
    }
     else if ((eEv == tCIDCtrls::EGestEvs::End)
          ||  (eEv == tCIDCtrls::EGestEvs::Drag)
          ||  (eEv == tCIDCtrls::EGestEvs::Inertia))
    {
        if (!pntDelta.bAtOrg())
        {
            TGraphWndDev gdevTmp(*this);
            AdjustIndicator(gdevTmp, pntDelta);
        }
    }
    return kCIDLib::True;
}


tCIDLib::TVoid TTestGest::GestClick(const TPoint& pntAt)
{
    // Move the circle to here
    TGraphWndDev gdevTmp(*this);
    MoveIndicator(gdevTmp, pntAt);
}


tCIDLib::TVoid TTestGest::GestReset(const tCIDCtrls::EGestReset eType)
{
}


tCIDLib::TVoid
TTestGest::PerGestOpts( const   TPoint&                 pntAt
                        , const tCIDLib::EDirs          eDir
                        , const tCIDLib::TBoolean       bTwoFingers
                        ,       tCIDCtrls::EGestOpts&   eOpts
                        ,       tCIDLib::TFloat4&       f4VScale)
{
    eOpts = tCIDCtrls::EGestOpts::PanInertia;
    f4VScale = 1.0F;
}


tCIDLib::TVoid
TTestGest::GestFlick(const  tCIDLib::EDirs  eDirection
                    , const TPoint&         pntStartPos)
{
}


// ---------------------------------------------------------------------------
//  TTestGest: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TTestGest::AdjustIndicator(TGraphDrawDev& gdevTmp, const TPoint& pntDelta)
{
    // Adjust the current point and move the circle to there
    TPoint pntNew = m_pntLast;
    pntNew += pntDelta;
    MoveIndicator(gdevTmp, pntNew);
}


tCIDLib::TVoid
TTestGest::MoveIndicator(TGraphDrawDev& gdevTmp, const TPoint& pntNew)
{
    // Calc the old and new areas
    TArea areaOld(m_pntLast, TSize(18, 18));
    areaOld.AdjustOrg(-9, -9);

    TArea areaNew(pntNew, TSize(18, 18));
    areaNew.AdjustOrg(-9, -9);

    //
    //  If they overlap, then we want to do a scroll and just fill in the uncovered
    //  bit.
    //
    if (areaOld.bIntersects(areaNew))
    {
        // Get the combined area of the two
        TArea areaScroll(areaNew);
        areaScroll |= areaOld;

        // Figure out which direction to scroll and by how much
        tCIDLib::TInt4 i4HOfs = 0;
        tCIDLib::TInt4 i4VOfs = 0;
        i4HOfs = pntNew.i4X() - m_pntLast.i4X();
        i4VOfs = pntNew.i4Y() - m_pntLast.i4Y();
        TArea areaUpdate;
        Scroll(areaScroll, areaUpdate, i4HOfs, i4VOfs, kCIDLib::False);

        // Fill the uncovered area
        gdevTmp.Fill(areaUpdate, facCIDGraphDev().rgbWhite);
    }
     else
    {
        gdevTmp.Fill(areaOld, facCIDGraphDev().rgbWhite);
        gdevTmp.Fill(areaNew, facCIDGraphDev().rgbBlack);
    }

    // Move to the new position
    m_pntLast = pntNew;
}





// ---------------------------------------------------------------------------
//  CLASS: TGestMainWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGestMainWnd: Constructors and destructor
// ---------------------------------------------------------------------------
TGestMainWnd::TGestMainWnd() :

    m_pwndClient(nullptr)
{
}

TGestMainWnd::~TGestMainWnd()
{
}


// ---------------------------------------------------------------------------
//  TGestMainWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TGestMainWnd::CreateFrame()
{
    TParent::CreateFrame
    (
        nullptr
        , facCIDCtrls().areaDefWnd()
        , L"Gesture Test Harness"
        , tCIDCtrls::EWndStyles::StdFrame
        , tCIDCtrls::EExWndStyles::None
        , tCIDCtrls::EFrameStyles::None
        , kCIDLib::False
    );

    // And now show ourself
    SetVisibility(kCIDLib::True);
}


// ---------------------------------------------------------------------------
//  TGestMainWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TGestMainWnd::AreaChanged(  const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if (m_pwndClient)
    {
        TArea areaClient;
        QueryClientArea(areaClient, kCIDLib::True);
        m_pwndClient->SetSize(areaClient.szArea(), kCIDLib::False);
    }
}


tCIDLib::TBoolean TGestMainWnd::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    TArea areaInit;
    QueryClientArea(areaInit, kCIDLib::True);

    m_pwndClient = new TTestGest();
    m_pwndClient->CreateTouchWnd(*this, 1, areaInit, tCIDCtrls::EWndStyles::VisChild);

    return kCIDLib::True;
}


tCIDLib::TVoid TGestMainWnd::Destroyed()
{
    // And be sure to call our parent last
    TParent::Destroyed();
}


tCIDLib::TVoid
TGestMainWnd::MenuCommand(  const   tCIDLib::TResId     ridItem
                            , const tCIDLib::TBoolean   bChecked
                            , const tCIDLib::TBoolean   bEnabled)
{
}


