//
// FILE NAME: TestGraphArea.Hpp
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
//  Areas (or rectangles or thatever they are on a particular system) in graphics are
//  probably the most annoying PITA of all time. It's almost impossible to avoid one-off
//  issues when interacting with system APIs and in the classes that manipulate them.
//  Then you build a system on those one-off errors, adjusting for them unknowingly. Then
//  you find something wrong and you fix it and everything is suddenly wrong.
//
//  So this test attempts to help deal with these issues by displaying various patterns
//  that test lines, fills, regions, areas, clipping, etc... visually such that any
//  failure is visually obvious.
//
//  Our yard stick is mostly points, which are drawn unambiguously at specific pixels.
//  So we can draw pixels at particular points and then draw something that should touch
//  but not cover these points.
//
//  We divide the window into 64x64 pixel squares and each test has to fit into one
//  of those. That's enough to do what is needed for these types of tests. So we can
//  add various tests to a list, and just run through them, passing each one an area
//  to draw within, which will be 4 pixels within its square.
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
#include    "CIDCtrls.hpp"



// ----------------------------------------------------------------------------
//  Global data from TestGraphArea.Cpp
// ----------------------------------------------------------------------------
extern TGUIFacility facTestGraphArea;


// ---------------------------------------------------------------------------
//   CLASS: TAreaTest
//  PREFIX: test
// ---------------------------------------------------------------------------
struct TAreaTest
{
    virtual tCIDLib::TVoid operator()
    (
                TGraphDrawDev&      gdevTar
        , const tCIDLib::TInt4      i4Left
        , const tCIDLib::TInt4      i4Top
        , const tCIDLib::TInt4      i4Right
        , const tCIDLib::TInt4      i4Bottom
    )   = 0;

    // A helper to draw dots on four sides
    tCIDLib::TVoid DrawSideDots
    (
                TGraphDrawDev&      gdevTar
        , const tCIDLib::TInt4      i4Left
        , const tCIDLib::TInt4      i4Top
        , const tCIDLib::TInt4      i4Right
        , const tCIDLib::TInt4      i4Bottom
        , const TRGBClr&            rgbColor
    )   const;
};



// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------
class TTestFrameWnd : public TFrameWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestFrameWnd();

        TTestFrameWnd(const TTestFrameWnd&) = delete;

        ~TTestFrameWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTestFrameWnd& operator=(const TTestFrameWnd&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() final;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colTests
        //      An adopting by-ref list of tests to run
        // -------------------------------------------------------------------
        TRefVector<TAreaTest>   m_colTests;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFrameWnd, TFrameWnd)
};

