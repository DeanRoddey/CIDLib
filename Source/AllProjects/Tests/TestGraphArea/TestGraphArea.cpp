//
// CID_FILE NAME: TestGraphArea.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/2019
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
//  This guy just creates a list of tests, and then draws any that need to be
//  redrawn.
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
#include    "TestGraphArea.hpp"

tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_CtrlsModule(TThread(L"MainThread", eMainThreadFunc))
RTTIDecls(TTestFrameWnd, TFrameWnd)


const tCIDLib::TInt4 i4TestInner   = 60;
const tCIDLib::TInt4 i4TestSize    = 64;
const TPoint pntTestSize(i4TestSize, i4TestSize);
const TRGBClr rgbRed(0xFF, 0, 0);
const TRGBClr rgbYellow(0xFF, 0xFF, 0);
const TRGBClr rgbTestFill1(0xDC, 0xDC, 0xDC);
const TRGBClr rgbTestFill2(0xDC, 0xDC, 0xFF);


//
//  Draw dots on the middle of each side of an area. These coordinates are assumed
//  to be literal (i.e. inclusive) so that we know we are drawing them at specific
//  places.
//
tCIDLib::TVoid
TAreaTest::DrawSideDots(        TGraphDrawDev&      gdevTar
                        , const tCIDLib::TInt4      i4Left
                        , const tCIDLib::TInt4      i4Top
                        , const tCIDLib::TInt4      i4Right
                        , const tCIDLib::TInt4      i4Bottom
                        , const TRGBClr&            rgbColor) const
{
    gdevTar.SetPixel(TPoint(i4Left, i4Top + ((i4Bottom - i4Top) / 2)), rgbColor);
    gdevTar.SetPixel(TPoint(i4Right, i4Top + ((i4Bottom - i4Top) / 2)), rgbColor);

    gdevTar.SetPixel(TPoint(i4Left + ((i4Right - i4Left) / 2), i4Top), rgbColor);
    gdevTar.SetPixel(TPoint(i4Left + ((i4Right - i4Left) / 2), i4Bottom), rgbColor);
}


// #1.  Draws points on four sides, then does a fill that should fit just within them.
struct TBasicAreaFill : public TAreaTest
{
    tCIDLib::TVoid operator()(          TGraphDrawDev&  gdevTar
                                , const tCIDLib::TInt4  i4Left
                                , const tCIDLib::TInt4  i4Top
                                , const tCIDLib::TInt4  i4Right
                                , const tCIDLib::TInt4  i4Bottom) final
    {
        DrawSideDots(gdevTar, i4Left + 8, i4Top + 8, i4Right - 8, i4Bottom - 8, rgbRed);
        TArea areaFill;
        areaFill.FromEdges(i4Left + 9, i4Top + 9, i4Right - 8, i4Bottom - 8);
        gdevTar.Fill(areaFill, rgbYellow);
    }
};

// #2.  The same as #1, but we get create an area from corner points
struct TBasicAreaFill2 : public TAreaTest
{
    tCIDLib::TVoid operator()(          TGraphDrawDev&  gdevTar
                                , const tCIDLib::TInt4  i4Left
                                , const tCIDLib::TInt4  i4Top
                                , const tCIDLib::TInt4  i4Right
                                , const tCIDLib::TInt4  i4Bottom) final
    {
        DrawSideDots(gdevTar, i4Left + 8, i4Top + 8, i4Right - 8, i4Bottom - 8, rgbRed);
        TArea areaFill(TPoint(i4Left + 9, i4Top + 9), TPoint(i4Right - 8, i4Bottom - 8));
        gdevTar.Fill(areaFill, rgbYellow);
    }
};

// #3.  The same as #1, but we do the area via deflation
struct TBasicAreaFill3 : public TAreaTest
{
    tCIDLib::TVoid operator()(          TGraphDrawDev&  gdevTar
                                , const tCIDLib::TInt4  i4Left
                                , const tCIDLib::TInt4  i4Top
                                , const tCIDLib::TInt4  i4Right
                                , const tCIDLib::TInt4  i4Bottom) final
    {
        DrawSideDots(gdevTar, i4Left + 8, i4Top + 8, i4Right - 8, i4Bottom - 8, rgbRed);

        // Add one since these have to be non-inclusive, then deflat
        TArea areaFill(TPoint(i4Left, i4Top), TPoint(i4Right + 1, i4Bottom + 1));
        areaFill.Deflate(9, 9);
        gdevTar.Fill(areaFill, rgbYellow);
    }
};

// #4.  The same as #3, but creates a region from the area and fills that
struct TBasicAreaFill4 : public TAreaTest
{
    tCIDLib::TVoid operator()(          TGraphDrawDev&  gdevTar
                                , const tCIDLib::TInt4  i4Left
                                , const tCIDLib::TInt4  i4Top
                                , const tCIDLib::TInt4  i4Right
                                , const tCIDLib::TInt4  i4Bottom) final
    {
        DrawSideDots(gdevTar, i4Left + 8, i4Top + 8, i4Right - 8, i4Bottom - 8, rgbRed);

        // Add one since these have to be non-inclusive, then deflat
        TArea areaFill(TPoint(i4Left, i4Top), TPoint(i4Right + 1, i4Bottom + 1));
        areaFill.Deflate(9, 9);
        TGUIRegion grgnFill(areaFill);
        gdevTar.Fill(grgnFill, rgbYellow);
    }
};

// #5.  The same as #4, but creates a rounded region
struct TBasicAreaFill5 : public TAreaTest
{
    tCIDLib::TVoid operator()(          TGraphDrawDev&  gdevTar
                                , const tCIDLib::TInt4  i4Left
                                , const tCIDLib::TInt4  i4Top
                                , const tCIDLib::TInt4  i4Right
                                , const tCIDLib::TInt4  i4Bottom) final
    {
        DrawSideDots(gdevTar, i4Left + 8, i4Top + 8, i4Right - 8, i4Bottom - 8, rgbRed);

        // Add one since these have to be non-inclusive, then deflat
        TArea areaFill(TPoint(i4Left, i4Top), TPoint(i4Right + 1, i4Bottom + 1));
        areaFill.Deflate(9, 9);
        TGUIRegion grgnFill(areaFill, 4);
        gdevTar.Fill(grgnFill, rgbYellow);
    }
};

// #6.  The same as #3, but we create the area via origin/size
struct TBasicAreaFill6 : public TAreaTest
{
    tCIDLib::TVoid operator()(          TGraphDrawDev&  gdevTar
                                , const tCIDLib::TInt4  i4Left
                                , const tCIDLib::TInt4  i4Top
                                , const tCIDLib::TInt4  i4Right
                                , const tCIDLib::TInt4  i4Bottom) final
    {
        DrawSideDots(gdevTar, i4Left + 8, i4Top + 8, i4Right - 8, i4Bottom - 8, rgbRed);

        // The points we get are inclusive, but he wants non-inclusive, so add one
        TArea areaFill
        (
            i4Left
            , i4Top
            , tCIDLib::TCard4(i4Right - i4Left) + 1
            , tCIDLib::TCard4(i4Bottom - i4Top) + 1
        );
        areaFill.Deflate(9, 9);
        TGUIRegion grgnFill(areaFill, 4);
        gdevTar.Fill(grgnFill, rgbYellow);
    }
};




// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TTextFrameWnd: PConstructors and Destructor
// ----------------------------------------------------------------------------
TTestFrameWnd::TTestFrameWnd() :

    m_colTests(tCIDLib::EAdoptOpts::Adopt)
{
    // Basic area fill tests
    m_colTests.Add(new TBasicAreaFill());
    m_colTests.Add(new TBasicAreaFill2());
    m_colTests.Add(new TBasicAreaFill3());
    m_colTests.Add(new TBasicAreaFill4());
    m_colTests.Add(new TBasicAreaFill5());
    m_colTests.Add(new TBasicAreaFill6());
}

TTestFrameWnd::~TTestFrameWnd()
{
}



// ----------------------------------------------------------------------------
//  TTestFrameWnd: Protected, inherited method
// ----------------------------------------------------------------------------

//
//  When we are created, in turn create all of our child windows, which we do manually
//  in this simple test.
//
tCIDLib::TBoolean TTestFrameWnd::bCreated()
{
    // Call our parent first
    TParent::bCreated();


    return kCIDLib::True;
}


tCIDLib::TBoolean TTestFrameWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    const tCIDLib::TCard4 c4PerRow = 4;
    tCIDLib::TInt4 i4X = 0;
    tCIDLib::TInt4 i4Y = 0;

    // We'll do up to 16 tests horizontally, then wrap
    tCIDLib::TCard4 c4HorzCnt = 0;
    tCIDLib::TCard4 c4VertCnt = 0;
    m_colTests.bForEachNC
    (
        [&](TAreaTest& testCur)
        {
            //
            //  We do a checkerboard background to make it easier to see the
            //  individual tests. On even rows we slightly darken even columns.
            //  And vice versa for odd rows. We use an area here, but that's ok
            //  since this isn't part of the test itself.
            //
            const tCIDLib::TBoolean bEvenRow((c4VertCnt & 1) == 0);
            const tCIDLib::TBoolean bEvenCol((c4HorzCnt & 1) == 0);
            gdevToUse.Fill
            (
                TPoint(i4X, i4Y), TPoint(i4X + i4TestSize, i4Y + i4TestSize)
                , bEvenRow == bEvenCol ? rgbTestFill1 : rgbTestFill2
            );


            testCur(gdevToUse, i4X, i4Y, i4X + (i4TestSize - 1), i4Y + (i4TestSize - 1));

            c4HorzCnt++;
            if (c4HorzCnt == c4PerRow)
            {
                c4HorzCnt = 0;
                i4X = 0;
                i4Y += i4TestSize;
                c4VertCnt++;
            }
             else
            {
                i4X += i4TestSize;
            }

            return kCIDLib::True;
        }
    );

    return kCIDLib::True;
}



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
        wndMain.CreateFrame
        (
            nullptr
            , facCIDCtrls().areaDefWnd()
            , L"Graphic Area Tests"
            , tCIDCtrls::EWndStyles::StdFrame
            , tCIDCtrls::EExWndStyles::None
            , tCIDCtrls::EFrameStyles::StdFrame
            , kCIDLib::False
        );

        facCIDCtrls().uMainMsgLoop(wndMain);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
    }

    return tCIDLib::EExitCodes::Normal;
}

