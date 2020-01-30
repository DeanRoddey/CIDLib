//
// CID_FILE NAME: Window3_Editor.hpp
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
//  This is the interface for our little point editor window, which the main
//  file creates an instance of and embeds in its client area.
//
//  Unlike the previous sample (Window2), this one sets it up up as a control
//  type window. This lets it be created via the standard dialog definition
//  stuff (though we do have to tweak it to make it use this class which it
//  doesn't understand.
//
//  Andt this guy needs to support sending notfications as to how many points
//  currently exist, which is all this little guy reports, though it could send
//  various other things. The frame window will set up a handler for the editor
//  control and display our reported counts.
//
//  Since this is now a control type window, it doesn't have a create method
//  anymore. It has an InitFromDesc() method, which lets it constructor itself
//  from a dialog item definition. This is the bit that lets us plug into the
//  standard control loading system. We could also provide a create method so
//  it could be used as a regular window, but there's no need here.
//
//  Our notification class is a totally base bones deal, with can just pass along
//  a count. It does have to support polymorphic duplication, but we can use a
//  little helper macro for that.
//
//  We provide a DiscardPoints() method for the main frame to call to ask us
//  to discard all points when the user clicks the Discard button.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

// ----------------------------------------------------------------------------
//   CLASS: TWindow3EdNot
//  PREFIX: wnot
// ----------------------------------------------------------------------------
class TWindow3EdNot : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWindow3EdNot(const tCIDLib::TCard4 c4Count) :

            m_c4Count(c4Count)
        {
        }

        tCIDLib::TCard4 m_c4Count;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TWindow3EdNot,TCtrlNotify)
        DefPolyDup(TWindow3EdNot)
};


// ----------------------------------------------------------------------------
//   CLASS: TWindow3Ed
//  PREFIX: wnd
// ----------------------------------------------------------------------------
class TWindow3Ed : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWindow3Ed() :

            TCtrlWnd()
            , m_rgbCircle(facCIDGraphDev().rgbPaleGrey)
        {
        }
        TWindow3Ed(const TWindow3Ed&) = delete;
        ~TWindow3Ed() = default;


        // -------------------------------------------------------------------
        //  Public, inherited methodss
        // -------------------------------------------------------------------
        tCIDLib::TVoid
        TWindow3Ed::InitFromDesc(   const   TWindow&                wndParent
                                    , const TDlgItem&               dlgiSrc
                                    , const tCIDCtrls::EWndThemes   eTheme)
        {

            TWindow::CreateWnd
            (
                wndParent.hwndThis()
                , kCIDCtrls::pszCustClass
                , kCIDLib::pszEmptyZStr
                , dlgiSrc.areaPos()
                , tCIDCtrls::EWndStyles::VisChild | tCIDCtrls::EWndStyles::Border
                , tCIDCtrls::EExWndStyles::None
                , dlgiSrc.ridChild()
            );

            // Set our bgn color to make it clear that we are there
            SetBgnColor(facCIDGraphDev().rgbCornflowerBlue);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DiscardPoints()
        {
            m_colPoints.RemoveAll();
            ForceRepaint();
            SendCountNot(0);
        }


        // -------------------------------------------------------------------
        //  A templated method to install a notification handler on us
        // -------------------------------------------------------------------
        template <typename T> TNotHandlerBase*
        pnothRegisterHandler(        T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TWindow3EdNot&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TWindow3EdNot>(pTarget, TNotificationId(L"Window3EdNotify"))
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TWindow3EdNot>
            (
                pTarget, TNotificationId(L"Window3EdNotify"), pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
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
        tCIDLib::TBoolean
        bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate) override
        {
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
                                , const tCIDLib::TBoolean       ) override
        {
            if ((eButton == tCIDCtrls::EMouseButts::Left)
            &&  (eClickType == tCIDCtrls::EMouseClicks::Down))
            {
                // Find one that includes the click point
                TVector<TClickPnt>::TCursor cursPoints(&m_colPoints);
                for (; cursPoints; ++cursPoints)
                {
                    if (cursPoints->m_areaTest.bContainsPoint(pntAt))
                        break;
                }

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
                    m_colPoints.objPlace(pntAt);
                }
                SendCountNot(m_colPoints.c4ElemCount());

                // Say we handled this click
                return kCIDLib::True;
            }

            // Not one of ours
            return kCIDLib::False;
        }

    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        // A helper to send one of our notifications with the passed count
        tCIDLib::TVoid SendCountNot(const tCIDLib::TCard4 c4Count)
        {
            SendAsyncNotify(new TWindow3EdNot(c4Count), TNotificationId(L"Window3EdNotify"));
        }


        // -------------------------------------------------------------------
        //  Private types and constants
        // -------------------------------------------------------------------

        // A small class to store click points and the hit testing areas for them
        static const tCIDLib::TCard4 c4Radius = 8;
        static const tCIDLib::TCard4 c4Diameter = c4Radius * 2;
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

                TArea   m_areaTest;
                TPoint  m_pntAt;
        };

        const TRGBClr       m_rgbCircle;
        TVector<TClickPnt>  m_colPoints;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TWindow3Ed, TCtrlWnd)
};

RTTIDecls(TWindow3EdNot, TCtrlNotify)
AdvRTTIDecls(TWindow3Ed, TCtrlWnd)
