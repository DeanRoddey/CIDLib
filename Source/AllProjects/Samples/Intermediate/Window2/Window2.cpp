//
// CID_FILE NAME: Window2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/1019
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
//  This sample program is step up from Window1. This one creates a custom window,
//  and sets it as the client area of the main frame window. The frame window will
//  keep it sized appropriately to fill the client area.
//
//  The custom window watches for clicks and draws a circle wherever you click,
//  so demonstrating the basics of how user input is handled and some simple
//  graphics stuff. If you click in the area of one of the existing ones, it is
//  removed.
//
//  It maintains a list of clicks and overrides the bPaint() method so that it can
//  redraw the previously clicked points if the window is resized. Since bPaint()
//  is given the area that needs updating, we'll only redraw the ones that need it,
//  so it's reasonably efficient even in this simple form.
//
//  We do simple hit testing by just defining an area that encloses the circle, so
//  we don't do polar coordinate type hit testing.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that the parent window owns any child windows, so we don't clean up the
//      custom window. We allocate the child window dynamically and we keep a typed
//      pointer in case we want to interact with it directly. But the frame window
//      owns it and will clean it up.
//
//      If you create a by value window object as a member, then you MUST override
//      Destroyed() in the parent window and destroy the window, then call the parent's
//      Destroyed(). That way it doesn't see that window anymore and won't try to
//      destroy it again.
//
//  2)  As with most of these samples, it's not complex enough to justify having a
//      facility object, so we just start it up on a local function.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include  "CIDCtrls.hpp"


// ----------------------------------------------------------------------------
//  Magic main module code. Note this one is different from the console based
//  samples!
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_CtrlsModule(TThread(L"Window2MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//   CLASS: TClientWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------
class TClientWnd : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TClientWnd() :

            TWindow(kCIDLib::False)
            , m_rgbCircle(facCIDGraphDev().rgbPaleGrey)
        {
        }

        TClientWnd(const TClientWnd&) = delete;
        TClientWnd(TClientWnd&&) = delete;

        ~TClientWnd() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TClientWnd& operator=(const TClientWnd&) = delete;
        TClientWnd& operator=(TClientWnd&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  We are a direct derivative of TWindow so we have to call the low level
        //  window creation method, which takes the raw window handle and raw strings.
        //  Normally you'd probably derive from something a step above TWindow.
        //
        tCIDLib::TVoid CreateClient(const TFrameWnd& wndParent, const TArea& areaAt)
        {
            TWindow::CreateWnd
            (
                wndParent.hwndThis()
                , kCIDCtrls::pszCustClass
                , kCIDLib::pszEmptyZStr
                , areaAt
                , tCIDCtrls::EWndStyles::VisChild
                , tCIDCtrls::EExWndStyles::None
                , kCIDCtrls::widFirstCtrl
            );

            //
            //  Set our bgn color to make it clear that we are there. Note that, in
            //  theory, we could get drawn before this is set, but in practice it
            //  never happens. But, most of the time for such setup you should override
            //  bCreated().
            //
            SetBgnColor(facCIDGraphDev().rgbCornflowerBlue);
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------

        //
        //  We need to redraw any circles that intersect the update area. We aren't
        //  super-strict, we just check if an area that encloses the circle intersects
        //  the update area. The base class will handle the bgn erase fill and we don't
        //  suppress that for this simple program, though you might for something that
        //  is more actively updating, to avoid flicker.
        //
        tCIDLib::TBoolean bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate) final
        {
            // For variety, we use for each to process the points here
            m_colPoints.bForEach
            (
                [&gdevToUse, &areaUpdate](const TClickPnt& cpntCur)
                {
                    if (cpntCur.m_areaTest.bIntersects(areaUpdate))
                        gdevToUse.DrawFilledCircle(c4Radius, cpntCur.m_pntAt);
                    return kCIDLib::True;
                }
            );
            return kCIDLib::True;
        }

        //
        //  If it's a left down click, either draw a new circle at the clicked
        //  point and add it to the list, or remove it from the list and redraw
        //  it away if there is already a circle there.
        //
        tCIDLib::TBoolean bClick(const  tCIDCtrls::EMouseButts  eButton
                                , const tCIDCtrls::EMouseClicks eClickType
                                , const TPoint&                 pntAt
                                , const tCIDLib::TBoolean
                                , const tCIDLib::TBoolean       ) final
        {
            if ((eButton == tCIDCtrls::EMouseButts::Left)
            &&  (eClickType == tCIDCtrls::EMouseClicks::Down))
            {
                //
                //  Find one that includes the click point, using an algorithm this time. This
                //  one takes a key to look for, pntAt in this case. which is passed to the
                //  callback along with each element to check.
                //
                TPntList::TCursor cursPoints = tCIDColAlgo::cursFindByKey
                (
                    m_colPoints, pntAt, [](const TClickPnt& cpntTest, const TPoint& pntTest)
                    {
                        return cpntTest.m_areaTest.bContainsPoint(pntTest);
                    }
                );

                TGraphWndDev gdevDraw(*this);
                if (cursPoints.bIsValid())
                {
                    //
                    //  We need to remove this guy. We can't just fill its area with
                    //  the bgn color since it might be partially under another circle.
                    //
                    //  Also, we are doing a sync redraw, so we have to remove the point
                    //  first or we will just redraw it again. But that means we also
                    //  have to get the update area out before we remove the point.
                    //
                    //  We could just invalidate for an async update, but it's less
                    //  efficient for interactive stuff.
                    //
                    const TArea areaUpdate = cursPoints->m_areaTest;
                    m_colPoints.RemoveAt(cursPoints);
                    Redraw(areaUpdate, tCIDCtrls::ERedrawFlags::ImmedErase);
                }
                 else
                {
                    gdevDraw.DrawFilledCircle(c4Radius, pntAt);
                    m_colPoints.objAdd(TClickPnt(pntAt));
                }

                // Say we handled this click
                return kCIDLib::True;
            }

            // Not one of ours
            return kCIDLib::False;
        }

    private :
        // -------------------------------------------------------------------
        //  Private types and constants
        // -------------------------------------------------------------------

        // A small class to store click points and the hit testing areas for them
        static constexpr tCIDLib::TCard4 c4Radius = 8;
        static constexpr tCIDLib::TCard4 c4Diameter = c4Radius * 2;
        class TClickPnt
        {
            public :
                TClickPnt() = default;
                TClickPnt(const TPoint& pntAt) : m_pntAt(pntAt)
                {
                    // Set up a hit testing area that encloses the circle
                    m_areaTest.Set(pntAt - TPoint(c4Radius), TSize(c4Diameter));
                    m_areaTest.Inflate(1);
                }
                TClickPnt(const TClickPnt&) = default;
                TClickPnt(TClickPnt&&) = default;
                TClickPnt& operator=(const TClickPnt&) = default;
                TClickPnt& operator=(TClickPnt&&) = default;

                TArea   m_areaTest;
                TPoint  m_pntAt;
        };
        using TPntList = TVector<TClickPnt>;


        // -------------------------------------------------------------------
        //  Private class members
        // -------------------------------------------------------------------
        const TRGBClr   m_rgbCircle;
        TPntList        m_colPoints;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TClientWnd, TWindow)
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
        TTestFrameWnd() = default;
        TTestFrameWnd(const TTestFrameWnd&) = delete;
        TTestFrameWnd(TTestFrameWnd&&) = delete;
        ~TTestFrameWnd() = default;

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTestFrameWnd& operator=(const TTestFrameWnd&) = delete;
        TTestFrameWnd& operator=(TTestFrameWnd&&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------

        // We have to handle this to create our client window
        tCIDLib::TBoolean bCreated() final
        {
            // Call our parent first
            TParent::bCreated();

            // Get our client area, gen up a window, and create it at this area
            TArea areaClient;
            QueryClientArea(areaClient, kCIDLib::False);
            m_pwndClient = new TClientWnd();
            m_pwndClient->CreateClient(*this, areaClient);

            //
            //  Tell our parent class this is our 'client' window and it will keep it
            //  sized to fit for us.
            //
            SetClientId(m_pwndClient->widThis());

            return kCIDLib::True;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndClient
        //      Our custom client window that we keep sized to fit our client area. We
        //      don't own it. This is just a typed pointer for convenience.
        // -------------------------------------------------------------------
        TClientWnd* m_pwndClient;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFrameWnd, TFrameWnd)
};


// ----------------------------------------------------------------------------
//  Magic macros for our two windows
// ----------------------------------------------------------------------------
RTTIDecls(TClientWnd, TWindow)
RTTIDecls(TTestFrameWnd, TFrameWnd)



// ----------------------------------------------------------------------------
//  We start up the main thread here
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Create a frame window, let the system decide on placement
        TTestFrameWnd wndMain;
        wndMain.CreateFrame
        (
            nullptr
            , facCIDCtrls().areaDefWnd()
            , L"Window 2 Sample (Click Me)"
            , tCIDCtrls::EWndStyles::StdFrame
            , tCIDCtrls::EExWndStyles::None
            , tCIDCtrls::EFrameStyles::StdFrame
            , kCIDLib::False
        );

        // Now just process msgs until the user closes us
        facCIDCtrls().uMainMsgLoop(wndMain);
    }

    catch(TError& errToCatch)
    {
        // Use the desktop as owner, since our frame window may not have worked
        TErrBox msgbErr(L"Window 2 Sample", errToCatch.strErrText());
        msgbErr.ShowIt(TWindow::wndDesktop());
    }

    return tCIDLib::EExitCodes::Normal;
}
