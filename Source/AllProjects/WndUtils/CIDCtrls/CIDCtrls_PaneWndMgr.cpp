//
// FILE NAME: CIDCtrls_TPaneWndCont.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2015
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
//  This file implements the pane window manager class.
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
#include    "CIDCtrls_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TPaneWndCont, TWindow)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDCtrls_PaneWndMgr
    {
        // The pixels between panes (where the resizing is done)
        constexpr tCIDLib::TCard4 c4DivMargin = 4;

        //
        //  The space around the div that we use to decide whether the mouse is on the div
        //  or not.
        //
        constexpr tCIDLib::TInt4  i4DivMinus = 7;
        constexpr tCIDLib::TInt4  i4DivPlus = 8;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TPaneWnd
//  PREFIX: panew
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPaneWnd Constructors and destructor
// ---------------------------------------------------------------------------
TPaneWnd::TPaneWnd() :

    m_pwndPane(0)
    , m_szMin(32, 32)
{
}

TPaneWnd::TPaneWnd(         TWindow* const  pwndMng
                    , const TSize&          szMin
                    , const TSize&          szInit) :

    m_pwndPane(pwndMng)
    , m_szInit(szInit)
    , m_szMin(szMin)
{
    m_szMin.TakeLarger(TSize(32, 32));
}

// We don't copy runtime only stuff
TPaneWnd::TPaneWnd(const TPaneWnd& panewSrc) :

    m_pwndPane(panewSrc.m_pwndPane)
    , m_szInit(panewSrc.m_szInit)
    , m_szMin(panewSrc.m_szMin)
{
}

TPaneWnd::~TPaneWnd()
{
}


// ---------------------------------------------------------------------------
//  TPaneWndCont::TPaneWnd: Public operators
// ---------------------------------------------------------------------------

// We don't copy runtime only stuff
TPaneWnd& TPaneWnd::operator=(const TPaneWnd& panewSrc)
{
    if (&panewSrc != this)
    {
        m_pwndPane   = panewSrc.m_pwndPane;
        m_szInit    = panewSrc.m_szInit;
        m_szMin     = panewSrc.m_szMin;
    }
    return *this;
}


tCIDLib::TVoid TPaneWnd::operator=(const TArea& areaToSet)
{
    m_areaCur = areaToSet;
}


// ---------------------------------------------------------------------------
//  TPaneWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Provide access to the old and new areas
const TArea& TPaneWnd::areaCur() const
{
    return m_areaCur;
}

const TArea& TPaneWnd::areaPrev() const
{
    return m_areaPrev;
}


//
//  If the current and previous areas are not the same on the passed panel window,
//  then return the new area and a True result.
//
tCIDLib::TBoolean TPaneWnd::bNewArea(TArea& areaAdjNew, TArea& areaAdjPrev)
{
    if (m_areaCur != m_areaPrev)
    {
        // Get a copy of the new area we can modify and store it as the previous
        areaAdjNew = m_areaCur;
        areaAdjPrev = m_areaPrev;
        m_areaPrev = areaAdjNew;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TCard4 TPaneWnd::c4Min(const tCIDLib::TBoolean bHorz) const
{
    if (bHorz)
        return m_szMin.c4Width();
    return m_szMin.c4Height();
}


tCIDLib::TCard4 TPaneWnd::c4Size(const tCIDLib::TBoolean bHorz) const
{
    if (bHorz)
        return m_areaCur.c4Width();

    return m_areaCur.c4Height();
}


tCIDLib::TInt4 TPaneWnd::i4Bottom() const
{
    return m_areaCur.i4Bottom();
}


tCIDLib::TInt4 TPaneWnd::i4Left() const
{
    return m_areaCur.i4Left();
}


tCIDLib::TInt4 TPaneWnd::i4Right() const
{
    return m_areaCur.i4Right();
}


tCIDLib::TInt4 TPaneWnd::i4Top() const
{
    return m_areaCur.i4Top();
}


tCIDLib::TVoid TPaneWnd::JustifyIn(const TArea& areaTar, const tCIDLib::TBoolean bHorz)
{
    if (bHorz)
        m_areaCur.RightJustifyIn(areaTar);
    else
        m_areaCur.BottomJustifyIn(areaTar);
}


// Provide access to our pane window
const TWindow* TPaneWnd::pwndPane() const
{
    return m_pwndPane;
}

TWindow* TPaneWnd::pwndPane()
{
    return m_pwndPane;
}


const TSize& TPaneWnd::szInit() const
{
    return m_szInit;
}

const TSize& TPaneWnd::szMin() const
{
    return m_szMin;
}


// Set the bottom and then adjust for min height if needed
tCIDLib::TVoid TPaneWnd::SetBottom(const tCIDLib::TInt4 i4ToSet)
{
    // If it's not under our min, take it, else take the min
    const tCIDLib::TCard4 c4Min = m_szMin.c4Height();

    if (i4ToSet > m_areaCur.i4Y())
        m_areaCur.i4Bottom(i4ToSet);
    else
        m_areaCur.c4Height(0);

    if (m_areaCur.c4Height() < c4Min)
        m_areaCur.c4Height(c4Min);
}


tCIDLib::TVoid TPaneWnd::SetLeft(const tCIDLib::TInt4 i4ToSet)
{
    m_areaCur.i4Left(i4ToSet);
}


// Set the height to the passed value or min if less than min
tCIDLib::TVoid TPaneWnd::SetHeight(const tCIDLib::TCard4 c4ToSet)
{
    // If it's not under our min, take it, else take the min
    const tCIDLib::TCard4 c4Min = m_szMin.c4Height();
    if (c4ToSet >= c4Min)
        m_areaCur.c4Height(c4ToSet);
    else
        m_areaCur.c4Height(c4Min);
}


// Set the bottom and then adjust for min height if needed
tCIDLib::TVoid TPaneWnd::SetRight(const tCIDLib::TInt4 i4ToSet)
{
    // If it's not under our min, take it, else take the min
    const tCIDLib::TCard4 c4Min = m_szMin.c4Width();

    if (i4ToSet > m_areaCur.i4X())
        m_areaCur.i4Right(i4ToSet);
    else
        m_areaCur.c4Width(0);

    if (m_areaCur.c4Width() < c4Min)
        m_areaCur.c4Width(c4Min);
}


tCIDLib::TVoid TPaneWnd::SetTop(const tCIDLib::TInt4 i4ToSet)
{
    m_areaCur.i4Y(i4ToSet);
}


// Set the width to the passed value or min if less than min
tCIDLib::TVoid TPaneWnd::SetWidth(const tCIDLib::TCard4 c4ToSet)
{
    // If it's not under our min, take it, else take the min
    const tCIDLib::TCard4 c4Min = m_szMin.c4Width();
    if (c4ToSet >= c4Min)
        m_areaCur.c4Width(c4ToSet);
    else
        m_areaCur.c4Width(c4Min);
}





// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout
//  PREFIX: play
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPaneLayout: Constructors and Destructor
// ---------------------------------------------------------------------------
TPaneLayout::~TPaneLayout()
{
}


// -------------------------------------------------------------------
//  TPaneLayout: Hidden constructors
// -------------------------------------------------------------------
TPaneLayout::TPaneLayout()
{
}




// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout2Horz
//  PREFIX: play
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPaneLayout2Horz: Constructors and Destructor
// ---------------------------------------------------------------------------
TPaneLayout2Horz::TPaneLayout2Horz()
{
}

TPaneLayout2Horz::~TPaneLayout2Horz()
{
}


// ---------------------------------------------------------------------------
//  TPaneLayout2Horz: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TPaneLayout2Horz::AdjustPanes(          tCIDCtrls::TPaneList&   colPanes
                                , const TArea&                  areaNew
                                , const TArea&                  areaOld
                                , const tCIDLib::TBoolean       bEmphDir)
{
    TPaneWnd& panew1 = colPanes[0];
    TPaneWnd& panew2 = colPanes[1];

    // Only need to do the real work if the height changed
    if (areaNew.c4Height() != areaOld.c4Height())
    {
        const tCIDLib::TCard4 c41Sz = panew1.c4Size(kCIDLib::False);
        const tCIDLib::TCard4 c42Sz = panew2.c4Size(kCIDLib::False);

        const tCIDLib::TCard4 c41MinSz = panew1.c4Min(kCIDLib::False);
        const tCIDLib::TCard4 c42MinSz = panew2.c4Min(kCIDLib::False);

        if (c41Sz + c42Sz < areaNew.c4Height())
        {
            //  We have more size so one can grow. We give it to the emphasis side.
            if (bEmphDir)
            {
                // Give it to the bottom pane
                panew2.SetBottom(areaNew.i4Bottom());
            }
             else
            {
                // Move the bottom pane down and give the rest to the top
                panew2.JustifyIn(areaNew, kCIDLib::False);
                panew1.SetBottom(panew2.i4Top() - 1);
            }
        }
         else if (c41Sz + c42Sz > areaNew.c4Height())
        {
            // Both can't fit, so we have to adjust
            if (c41MinSz + c42MinSz > areaNew.c4Height())
            {
                //
                //  If the two min sizes are too much to fit, we just set them both to min and
                //  we just have to overhang.
                //
                panew1.SetHeight(c41MinSz);
                panew2.SetTop(panew1.i4Bottom());
                panew2.SetHeight(c42MinSz);
            }
             else if (c41MinSz + c42MinSz < areaNew.c4Height())
            {
                //
                //  At least one doesn't have to be min size, so see what we have to get
                //  rid of.
                //
                const tCIDLib::TCard4 c4Reduce = (c41Sz + c42Sz) - areaNew.c4Height();

                //
                //  They will both fit without going to minimum, so we reduce the emphasis
                //  one down until it hits the minimum.
                //
                if (bEmphDir)
                {
                    // Reduce the bottom pane if not the min already
                    if (c42Sz > c42MinSz)
                    {
                        if (c42Sz > c4Reduce)
                            panew2.SetHeight(c42Sz - c4Reduce);
                        if (panew2.c4Size(kCIDLib::False) < c42MinSz)
                            panew2.SetHeight(c42MinSz);
                    }

                    panew2.JustifyIn(areaNew, kCIDLib::False);
                    panew1.SetBottom(panew2.i4Top() - 1);
                }
                 else
                {
                    if (c41Sz > c41MinSz)
                    {
                        if (c41Sz > c4Reduce)
                            panew1.SetHeight(c41Sz - c4Reduce);
                        if (panew1.c4Size(kCIDLib::False) < c41MinSz)
                            panew1.SetHeight(c41MinSz);
                    }

                    panew2.SetTop(panew1.i4Bottom());
                    panew2.SetBottom(areaNew.i4Bottom());
                }
            }
        }
    }

    // They all take up the cross dimension fully
    const tCIDLib::TCard4 c4XDim = areaNew.c4Width();
    panew1.SetWidth(c4XDim);
    panew2.SetWidth(c4XDim);
}


//
//  We just see if the mouse is vertically within our panes and within a couple
//  pixels of the right sides of the 1st or 2nd pane.
//
tCIDLib::TBoolean
TPaneLayout2Horz::bCheckOnDiv(  const   tCIDCtrls::TPaneList&   colPanes
                                , const TPoint&                 pntAt
                                ,       tCIDLib::TCard4&        c4First
                                ,       tCIDLib::TCard4&        c4Sec
                                ,       tCIDLib::TBoolean&      bHorz) const
{
    // Has to be horizontally within areas. They are all full width , so any is fine
    if ((pntAt.i4Y() < colPanes[0].i4Top())
    ||  (pntAt.i4Y() > colPanes[0].i4Bottom()))
    {
        return kCIDLib::False;
    }

    // It has to be a vertical divider
    bHorz = kCIDLib::False;

    // See if it's within a few pixels of the bottom of the first pane
    if ((pntAt.i4Y() >= colPanes[0].i4Bottom() - CIDCtrls_PaneWndMgr::i4DivMinus)
    &&  (pntAt.i4Y() <= colPanes[0].i4Bottom() + CIDCtrls_PaneWndMgr::i4DivPlus))
    {
        c4First = 0;
        c4Sec = 1;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TPaneLayout2Horz::bDragAdjust(          tCIDCtrls::TPaneList&   colPanes
                                , const tCIDLib::TCard4         c4First
                                , const tCIDLib::TCard4         c4Sec
                                , const TPoint&                 pntAt
                                , const TArea&                  areaAvail)
{
    CIDAssert((c4First == 0) && (c4Sec == 1), L"Invalid drag adjust pane indices");

    // Figure out the min max values
    const tCIDLib::TInt4 i4Min = tCIDLib::TInt4
    (
        colPanes[c4First].i4Top() + colPanes[c4First].c4Min(kCIDLib::False)
    );
    const tCIDLib::TInt4 i4Max = tCIDLib::TInt4
    (
        colPanes[c4Sec].i4Bottom() - colPanes[c4Sec].c4Min(kCIDLib::False)
    );

    //
    //  If they are the same or crossed, just do nothing. The later release of the
    //  mouse button will just get eaten.
    //
    if (i4Min >= i4Max)
        return kCIDLib::False;

    // If not within the valid range, then obviously not
    if ((pntAt.i4Y() < i4Min) || (pntAt.i4Y() > i4Max))
        return kCIDLib::False;

    //
    //  Looks ok, so let's adjust.
    //
    TPaneWnd& panew1 = colPanes[c4First];
    TPaneWnd& panew2 = colPanes[c4Sec];

    panew1.SetBottom(pntAt.i4Y());
    panew2.SetTop(panew1.i4Bottom());

    // The 2nd is ended at the availale area
    panew2.SetBottom(areaAvail.i4Bottom());

    return kCIDLib::True;
}


// We have horizontal panes, so we do drag vertically
tCIDLib::TBoolean TPaneLayout2Horz::bVertDrag() const
{
    return kCIDLib::True;
}


tCIDLib::TCard4 TPaneLayout2Horz::c4PaneCount() const
{
    return 2;
}


//
//  The are all set to the full area before we are called, so they are correct
//  horizontally. We set their height to the initial heights indicated to us. If they
//  don't take up the whole area, then either the first or second is given the rest,
//  depending on the emphasis area flag.
//
tCIDLib::TVoid
TPaneLayout2Horz::InitPanes(        tCIDCtrls::TPaneList&   colPanes
                            , const TArea&                  areaAvail
                            , const tCIDLib::TBoolean       bEmphDir)
{
    tCIDLib::TCard4 c4Height1 = colPanes[0].szInit().c4Height();
    tCIDLib::TCard4 c4Height2 = colPanes[1].szInit().c4Height();;

    // Expand them to reasonable minimums
    if (c4Height1 < 32)
        c4Height1 = 32;
    if (c4Height2 < 32)
        c4Height2 = 32;

    if (c4Height1 + c4Height2 < areaAvail.c4Height())
    {
        if (bEmphDir)
            c4Height2 = areaAvail.c4Height() - c4Height1;
        else
            c4Height1 = areaAvail.c4Height() - c4Height2;
    }

    colPanes[0].SetHeight(c4Height1);
    colPanes[1].SetTop(colPanes[0].i4Bottom());
    colPanes[1].SetHeight(c4Height2);
}




// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout2Vert
//  PREFIX: play
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPaneLayout2Vert: Constructors and Destructor
// ---------------------------------------------------------------------------
TPaneLayout2Vert::TPaneLayout2Vert()
{
}

TPaneLayout2Vert::~TPaneLayout2Vert()
{
}


// ---------------------------------------------------------------------------
//  TPaneLayout2Vert: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TPaneLayout2Vert::AdjustPanes(          tCIDCtrls::TPaneList&   colPanes
                                , const TArea&                  areaNew
                                , const TArea&                  areaOld
                                , const tCIDLib::TBoolean       bEmphDir)
{
    TPaneWnd& panew1 = colPanes[0];
    TPaneWnd& panew2 = colPanes[1];

    // Only need to do this if the width changed
    if (areaNew.c4Width() != areaOld.c4Width())
    {
        const tCIDLib::TCard4 c41Sz = panew1.c4Size(kCIDLib::True);
        const tCIDLib::TCard4 c42Sz = panew2.c4Size(kCIDLib::True);

        const tCIDLib::TCard4 c41MinSz = panew1.c4Min(kCIDLib::True);
        const tCIDLib::TCard4 c42MinSz = panew2.c4Min(kCIDLib::True);

        if (c41Sz + c42Sz < areaNew.c4Width())
        {
            // #1 We have more size. Give the emphasis pane the extra space
            if (bEmphDir)
            {
                // Give it to the right one
                panew2.SetRight(areaNew.i4Right());
            }
             else
            {
                // Give it to the left one
                panew2.JustifyIn(areaNew, kCIDLib::True);
                panew1.SetRight(panew2.i4Left() - 1);
            }
        }
         else if (c41Sz + c42Sz > areaNew.c4Width())
        {
            // Both won't fit as is, so someone has to give
            if (c41MinSz + c42MinSz > areaNew.c4Width())
            {
                // They both have to go to min, even if one hangs off
                panew1.SetWidth(c41MinSz);
                panew2.SetLeft(panew1.i4Right());
                panew2.SetWidth(c42MinSz);
            }
             else if (c41MinSz + c42MinSz < areaNew.c4Width())
            {
                //
                //  At least one doesn't have to be min size, so see what we have to get
                //  rid of.
                //
                const tCIDLib::TCard4 c4Reduce = (c41Sz + c42Sz) - areaNew.c4Width();

                if (bEmphDir)
                {
                    // Reduce the right pane if not the min already
                    if (c42Sz > c42MinSz)
                    {
                        if (c42Sz > c4Reduce)
                            panew2.SetWidth(c42Sz - c4Reduce);
                        if (panew2.c4Size(kCIDLib::True) < c42MinSz)
                            panew2.SetWidth(c42MinSz);
                    }

                    panew2.JustifyIn(areaNew, kCIDLib::True);
                    panew1.SetRight(panew2.i4Left() - 1);
                }
                 else
                {
                    // Reduce the left pane if not at the min already
                    if (c41Sz > c41MinSz)
                    {
                        if (c41Sz > c4Reduce)
                            panew1.SetWidth(c41Sz - c4Reduce);
                        if (panew1.c4Size(kCIDLib::True) < c41MinSz)
                            panew1.SetWidth(c41MinSz);
                    }

                    panew2.SetLeft(panew1.i4Right());
                    panew2.SetRight(areaNew.i4Right());
                }
            }
        }
    }

    // They all take up the cross dimension fully
    const tCIDLib::TCard4 c4XDim = areaNew.c4Height();
    panew1.SetHeight(c4XDim);
    panew2.SetHeight(c4XDim);
}


//
//  We just see if the mouse is vertically within our panes and within a couple
//  pixels of the right sides of the 1st/2nd pane divider.
//
tCIDLib::TBoolean
TPaneLayout2Vert::bCheckOnDiv(  const   tCIDCtrls::TPaneList&   colPanes
                                , const TPoint&                 pntAt
                                ,       tCIDLib::TCard4&        c4First
                                ,       tCIDLib::TCard4&        c4Sec
                                ,       tCIDLib::TBoolean&      bHorz) const
{
    // Has to be vertical within areas. They are all full height, so any is fine
    if ((pntAt.i4X() < colPanes[0].i4Left())
    ||  (pntAt.i4X() > colPanes[0].i4Right()))
    {
        return kCIDLib::False;
    }

    // It has to be a horizontal divider
    bHorz = kCIDLib::True;

    // See if it's within a few pixels of the right of the first pane
    if ((pntAt.i4X() >= colPanes[0].i4Right() - CIDCtrls_PaneWndMgr::i4DivMinus)
    &&  (pntAt.i4X() <= colPanes[0].i4Right() + CIDCtrls_PaneWndMgr::i4DivPlus))
    {
        c4First = 0;
        c4Sec = 1;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TPaneLayout2Vert::bDragAdjust(          tCIDCtrls::TPaneList&   colPanes
                                , const tCIDLib::TCard4         c4First
                                , const tCIDLib::TCard4         c4Sec
                                , const TPoint&                 pntAt
                                , const TArea&                  areaAvail)
{
    CIDAssert((c4First == 0) && (c4Sec == 1), L"Invalid drag adjust pane indices");

    // Figure out the min max values
    const tCIDLib::TInt4 i4Min = tCIDLib::TInt4
    (
        colPanes[c4First].i4Left() + colPanes[c4First].c4Min(kCIDLib::True)
    );
    const tCIDLib::TInt4 i4Max = tCIDLib::TInt4
    (
        colPanes[c4Sec].i4Right() - colPanes[c4Sec].c4Min(kCIDLib::True)
    );

    //
    //  If they are the same or crossed, just do nothing. The later release of the
    //  mouse button will just get eaten.
    //
    if (i4Min >= i4Max)
        return kCIDLib::False;

    // If not within the valid range, then obviously not
    if ((pntAt.i4X() < i4Min) || (pntAt.i4X() > i4Max))
        return kCIDLib::False;

    //
    //  Looks ok, so let's adjust.
    //
    TPaneWnd& panew1 = colPanes[c4First];
    TPaneWnd& panew2 = colPanes[c4Sec];

    panew1.SetRight(pntAt.i4X());
    panew2.SetLeft(panew1.i4Right());

    // The 2nd is ended at the availale area
    panew2.SetRight(areaAvail.i4Right());

    return kCIDLib::True;
}


// We have verical panes, so we drag horz
tCIDLib::TBoolean TPaneLayout2Vert::bVertDrag() const
{
    return kCIDLib::False;
}


tCIDLib::TCard4 TPaneLayout2Vert::c4PaneCount() const
{
    return 2;
}


//
//  The are all set to the full area before we are called, so they are correct
//  vertically. We set their widths to the provided defaults. If all of the space
//  is not taken up, then one or the other is given the rest of the space, depending
//  on the emphasis direction parm.
//
tCIDLib::TVoid
TPaneLayout2Vert::InitPanes(        tCIDCtrls::TPaneList&   colPanes
                            , const TArea&                  areaAvail
                            , const tCIDLib::TBoolean       bEmphDir)
{
    tCIDLib::TCard4 c4Width1 = colPanes[0].szInit().c4Width();
    tCIDLib::TCard4 c4Width2 = colPanes[1].szInit().c4Width();;

    // Expand them to reasonable minimums
    if (c4Width1 < 32)
        c4Width1 = 32;
    if (c4Width2 < 32)
        c4Width2 = 32;

    if (c4Width1 + c4Width2 < areaAvail.c4Width())
    {
        if (bEmphDir)
            c4Width2 = areaAvail.c4Width() - c4Width1;
        else
            c4Width1 = areaAvail.c4Width() - c4Width2;
    }

    colPanes[0].SetWidth(c4Width1);
    colPanes[1].SetLeft(colPanes[0].i4Right());
    colPanes[1].SetWidth(c4Width2);
}




// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout3Vert
//  PREFIX: play
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPaneLayout3Vert: Constructors and Destructor
// ---------------------------------------------------------------------------
TPaneLayout3Vert::TPaneLayout3Vert()
{
}

TPaneLayout3Vert::~TPaneLayout3Vert()
{
}


// ---------------------------------------------------------------------------
//  TPaneLayout3Vert: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TPaneLayout3Vert::AdjustPanes(          tCIDCtrls::TPaneList&   colPanes
                                , const TArea&                  areaNew
                                , const TArea&                  areaOld
                                , const tCIDLib::TBoolean       bEmphDir)
{
    TPaneWnd& panew1 = colPanes[0];
    TPaneWnd& panew2 = colPanes[1];
    TPaneWnd& panew3 = colPanes[2];

    // Only need to do the real work if the width changed
    if (areaNew.c4Width() != areaOld.c4Width())
    {
        const tCIDLib::TCard4 c41Sz = panew1.c4Size(kCIDLib::True);
        const tCIDLib::TCard4 c42Sz = panew2.c4Size(kCIDLib::True);
        const tCIDLib::TCard4 c43Sz = panew3.c4Size(kCIDLib::True);

        const tCIDLib::TCard4 c41MinSz = panew1.c4Min(kCIDLib::True);
        const tCIDLib::TCard4 c42MinSz = panew2.c4Min(kCIDLib::True);
        const tCIDLib::TCard4 c43MinSz = panew3.c4Min(kCIDLib::True);

        if (c41Sz + c42Sz + c43Sz < areaNew.c4Width())
        {
            //
            //  #1 We have more size. Give it to the center. So position the 3
            //  pane at the far right and then move pane 2's right up to
            //  3's new left.
            //
            panew3.JustifyIn(areaNew, kCIDLib::True);
            panew2.SetRight(panew3.i4Left() - 1);
        }
         else if (c41Sz + c42Sz + c43Sz > areaNew.c4Width())
        {
            // Somebody has to give. See if we can take it all from the center
            if (c41Sz + c43Sz + c42MinSz <= areaNew.c4Width())
            {
                // #2 we can take it from the center
                panew3.JustifyIn(areaNew, kCIDLib::True);
                panew2.SetRight(panew3.i4Left() - 1);
            }
             else if (c41Sz + c42MinSz + c43MinSz <= areaNew.c4Width())
            {
                // #3 We can take it from 2 and 3 panes
                panew2.SetLeft(panew1.i4Right());
                panew2.SetWidth(c42MinSz);
                panew3.SetLeft(panew2.i4Right());
                panew3.SetRight(areaNew.i4Right());
            }
             else
            {
                // #4 They all have to go min, even if they overhang
                panew1.SetWidth(c41MinSz);
                panew2.SetLeft(panew1.i4Right());
                panew2.SetWidth(c42MinSz);
                panew3.SetLeft(panew2.i4Right());
                panew3.SetWidth(c43MinSz);
            }
        }
    }

    // They all take up the cross dimension fully
    const tCIDLib::TCard4 c4XDim = areaNew.c4Height();
    panew1.SetHeight(c4XDim);
    panew2.SetHeight(c4XDim);
    panew3.SetHeight(c4XDim);
}


//
//  We just see if the mouse is vertically within our panes and within a couple
//  pixels of the right sides of the 1st or 2nd pane.
//
tCIDLib::TBoolean
TPaneLayout3Vert::bCheckOnDiv(  const   tCIDCtrls::TPaneList&   colPanes
                                , const TPoint&                 pntAt
                                ,       tCIDLib::TCard4&        c4First
                                ,       tCIDLib::TCard4&        c4Sec
                                ,       tCIDLib::TBoolean&      bHorz) const
{
    // Has to be vertically within areas. They are all full height, so any is fine
    if ((pntAt.i4Y() < colPanes[0].i4Top())
    ||  (pntAt.i4Y() > colPanes[0].i4Bottom()))
    {
        return kCIDLib::False;
    }

    //
    //  It has to be along one of the vertical dividers. We only have to check the
    //  first two.
    //
    bHorz = kCIDLib::True;
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < 2)
    {
        const TPaneWnd& panewCur = colPanes[c4Index];

        // See if it's within a few pixels of the end or the start of th enext
        if ((pntAt.i4X() >= panewCur.i4Right() - CIDCtrls_PaneWndMgr::i4DivMinus)
        &&  (pntAt.i4X() <= panewCur.i4Right() + CIDCtrls_PaneWndMgr::i4DivPlus))
        {
            c4First = c4Index;
            c4Sec = c4Index + 1;
            break;
        }
        c4Index++;
    }
    return (c4Index < 2);
}


tCIDLib::TBoolean
TPaneLayout3Vert::bDragAdjust(          tCIDCtrls::TPaneList&   colPanes
                                , const tCIDLib::TCard4         c4First
                                , const tCIDLib::TCard4         c4Sec
                                , const TPoint&                 pntAt
                                , const TArea&                  areaAvail)
{
    // Figure out the min max values
    const tCIDLib::TInt4 i4Min = tCIDLib::TInt4
    (
        colPanes[c4First].i4Left() + colPanes[c4First].c4Min(kCIDLib::True)
    );
    const tCIDLib::TInt4 i4Max = tCIDLib::TInt4
    (
        colPanes[c4Sec].i4Right() - colPanes[c4Sec].c4Min(kCIDLib::True)
    );

    //
    //  If they are the same or crossed, just do nothing. The later release of the
    //  mouse button will just get eaten.
    //
    if (i4Min >= i4Max)
        return kCIDLib::False;

    // If not within the valid range, then obviously not
    if ((pntAt.i4X() < i4Min) || (pntAt.i4X() > i4Max))
        return kCIDLib::False;

    //
    //  Looks ok, so let's adjust.
    //
    TPaneWnd& panew1 = colPanes[c4First];
    TPaneWnd& panew2 = colPanes[c4Sec];

    panew1.SetRight(pntAt.i4X());
    panew2.SetLeft(panew1.i4Right());

    // The 2nd is ended either on the left of the last pane, or the available area
    if (c4First == 0)
        panew2.SetRight(colPanes[2].i4Left() - 1);
    else
        panew2.SetRight(areaAvail.i4Right());

    return kCIDLib::True;
}


// We have verical panes, so we drag horz
tCIDLib::TBoolean TPaneLayout3Vert::bVertDrag() const
{
    return kCIDLib::False;
}


tCIDLib::TCard4 TPaneLayout3Vert::c4PaneCount() const
{
    return 3;
}


//
//  The are all set to the full area before we are called, so they are correct
//  vertically. We set them all to their configured initial sizes. If that doesn't
//  take up all of the available space, the middle one gets the rest.
//
tCIDLib::TVoid
TPaneLayout3Vert::InitPanes(        tCIDCtrls::TPaneList&   colPanes
                            , const TArea&                  areaAvail
                            , const tCIDLib::TBoolean       )
{
    tCIDLib::TCard4 c4Width1 = colPanes[0].szInit().c4Width();
    tCIDLib::TCard4 c4Width2 = colPanes[1].szInit().c4Width();
    tCIDLib::TCard4 c4Width3 = colPanes[2].szInit().c4Width();

    // Expand them all to reasonable minimums
    if (c4Width1 < 32)
        c4Width1 = 32;
    if (c4Width2 < 32)
        c4Width2 = 32;
    if (c4Width3 < 32)
        c4Width3 = 32;

    // If this isn't enough to fill the area, give the rest to the middle one
    if (c4Width1 + c4Width2 + c4Width3 < areaAvail.c4Width())
        c4Width2 = areaAvail.c4Width() - (c4Width1 + c4Width3);

    colPanes[0].SetWidth(c4Width1);

    colPanes[1].SetLeft(colPanes[0].i4Right());
    colPanes[1].SetWidth(c4Width2);

    colPanes[2].SetLeft(colPanes[1].i4Right());
    colPanes[2].SetWidth(c4Width3);
}





// ---------------------------------------------------------------------------
//   CLASS: TPaneWndCont
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPaneWndCont: Constructors and Destructor
// ---------------------------------------------------------------------------
TPaneWndCont::TPaneWndCont() :

    TWindow(kCIDLib::False)
    , m_bEmphDir(kCIDLib::False)
    , m_colPanes(kCIDCtrls::c4MaxPaneWnds)
    , m_playMgr(nullptr)
{
    //
    //  Add three pane objects, which is the worst case. This way we don't have to
    //  worry about expanding it and such.
    //
    TPaneWnd msgwDef;
    m_colPanes.objAdd(msgwDef);
    m_colPanes.objAdd(msgwDef);
    m_colPanes.objAdd(msgwDef);
}

TPaneWndCont::~TPaneWndCont()
{
    // Clean up the layout manager if one was set
    if (m_playMgr)
    {
        try
        {
            delete m_playMgr;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TPaneWndCont: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We just pass this on to our pane window that contains the focus. Presumably we would
//  not have been called if the focus was not somewhere within us.
//
tCIDLib::TBoolean TPaneWndCont::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    const tCIDLib::TCard4 c4PCount = m_playMgr->c4PaneCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4PCount; c4Index++)
    {
        const TPaneWnd& panewCur = m_colPanes[c4Index];
        if (panewCur.pwndPane()->bContainsFocus())
        {
            bRet = panewCur.pwndPane()->bProcessAccel(pMsgData);
            break;
        }
    }
    return bRet;
}


// ---------------------------------------------------------------------------
//  TPaneWndCont: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return the number of panes we have
tCIDLib::TCard4 TPaneWndCont::c4PaneCount() const
{
    if (!m_playMgr)
        return 0;

    return m_playMgr->c4PaneCount();
}



//
//  Called to create the us. We call our parent to kick it off. Most of the init
//  is done in Created().
//
tCIDLib::TVoid
TPaneWndCont::CreatePaneCont(const  TWindow&                wndParent
                            , const TArea&                  areaInit
                            , const tCIDCtrls::TWndId       widThis
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EExWndStyles eExStyles
                            )
{
    TParent::CreateWnd
    (
        wndParent.hwndSafe()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles::ClipChildren
        , eExStyles | tCIDCtrls::EExWndStyles::ControlParent
        , widThis
    );
}


tCIDLib::TVoid TPaneWndCont::InitPanes(const tCIDLib::TBoolean bEmphDir)
{
    // For this initial scenario, we let the layout manager set initial sizes
    const tCIDLib::TCard4 c4PCount = m_playMgr->c4PaneCount();

    const TArea& areaAvail = areaClient();

    // All of them will start at the full area and adjust from there
    m_colPanes[0] = areaAvail;
    m_colPanes[1] = areaAvail;
    m_colPanes[2] = areaAvail;
    m_playMgr->InitPanes(m_colPanes, areaAvail, bEmphDir);

    //
    //  Loop through the current panels and add any that changed to a multi-pos
    //  object. Then complete it to move all of the windows.
    //
    TWndMultiPos wmposUpdate(c4PCount);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4PCount; c4Index++)
    {
        TPaneWnd& panewCur = m_colPanes[c4Index];
        if (panewCur.bNewArea(m_areaTmp, m_areaTmp2))
        {
            AdjustForDiv(m_areaTmp, m_playMgr->bVertDrag(), c4PCount, c4Index);
            wmposUpdate.AddWindow(*panewCur.pwndPane(), m_areaTmp);
        }
    }
    wmposUpdate.Complete();

    // Keep the emphasis direction for later use
    m_bEmphDir = bEmphDir;
}


// Proivde access to the pane window at the indicated index
TWindow* TPaneWndCont::pwndPaneAt(const tCIDLib::TCard4 c4At)
{
    return m_colPanes[c4At].pwndPane();
}


// Just store the layout.
tCIDLib::TVoid TPaneWndCont::SetLayout(TPaneLayout* const  playToAdopt)
{
    // Delete any previous layout and take the new one
    delete m_playMgr;
    m_playMgr = playToAdopt;

    // Reset everything else
    TPaneWnd msgwDef;
    m_colPanes[0] = msgwDef;
    m_colPanes[1] = msgwDef;
    m_colPanes[2] = msgwDef;
}


//
//  Set up one of the panes. It's the caller's responsibility to set them in some
//  way that's reasonable for the layout they've set. THey must have set the layout
//  already.
//
tCIDLib::TVoid
TPaneWndCont::SetPane(      TWindow* const  pwndAdd
                    , const tCIDLib::TCard4 c4PaneInd
                    , const TSize&          szMin
                    , const TSize&          szInit)
{
    // Create a new managed window structure and set it up, then add it
    TPaneWnd panewNew(pwndAdd, szMin, szInit);
    m_colPanes[c4PaneInd] = panewNew;
}



tCIDLib::TVoid TPaneWndCont::ShowPanes()
{
    const tCIDLib::TCard4 c4Count = c4PaneCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colPanes[c4Index].pwndPane()->SetVisibility(kCIDLib::True);

    SetVisibility(kCIDLib::True);
}


// ---------------------------------------------------------------------------
//  TPaneWndCont: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We have to keep our status bar and tiled panes positioned. The status bar we
//  handle. For the panes we use the pane window manager object, telling him about
//  the new available area.
//
tCIDLib::TVoid
TPaneWndCont::AreaChanged(  const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    // Call our parent first
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
    {
        //
        //  And now let our pane manager arrange them based on our new size. Then
        //  we do a background fill
        //
        if (c4PaneCount())
            AdjustWnds(areaNew, areaPrev);
    }
}


//
//  We don't handle mouse input ourself. But we want to allow the user to move
//  the pane dividers. We call the pane window manager to see if the mouse is in a
//  position that would be in between two panes. If so, we change the pointer.
//
//  If we get a down click we see if we are on such a boundary. If so, we watch for
//  drag begin. If we get it, we call the pane window manager to allow it to addjust
//  the window sizes. He does it in a modal loop, so we don't come back until the
//  user releases the mouse.
//
tCIDLib::TBoolean
TPaneWndCont::bClick(const  tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    tCIDLib::TBoolean   bHorz;
    tCIDLib::TCard4     c4First;
    tCIDLib::TCard4     c4Sec;

    if (eButton == tCIDCtrls::EMouseButts::Left)
    {
        if ((eClickType == tCIDCtrls::EMouseClicks::Down)
        &&  bCheckOnDiv(pntAt, c4First, c4Sec, bHorz))
        {
            // If the user starts dragging, then start the process
            if (bDragDetect(pntAt))
                DragDivider(*this, areaClient(), pntAt, c4First, c4Sec, bHorz);
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  We want to minimize the flicker, so we do nothing here and just do it in the paint
//  where we are clipped to our children.
//
tCIDLib::TBoolean TPaneWndCont::bEraseBgn(TGraphDrawDev& gdevToUse)
{
    return kCIDLib::True;
}


//
//  See if the mouse is at an inter-pane draggable position. THe pane wnd manager
//  does the actual work.
//
tCIDLib::TBoolean
TPaneWndCont::bMouseMove(const  TPoint&             pntAt
                        , const tCIDLib::TBoolean   bCtrlShift
                        , const tCIDLib::TBoolean   bShift)
{
    tCIDLib::TBoolean   bHorz;
    tCIDLib::TCard4     c4First;
    tCIDLib::TCard4     c4Sec;

    if (bCheckOnDiv(pntAt, c4First, c4Sec, bHorz))
    {
        tCIDCtrls::ESysPtrs ePtr;
        if (bHorz)
            ePtr = tCIDCtrls::ESysPtrs::SizeWE;
        else
            ePtr = tCIDCtrls::ESysPtrs::SizeNS;

        facCIDCtrls().SetPointer(ePtr);
        return kCIDLib::True;
    }

    // Let the default pointer be created
    return kCIDLib::False;
}


// Just fill our bgn
tCIDLib::TBoolean
TPaneWndCont::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    gdevToUse.Fill(areaUpdate, rgbBgnFill());
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TPaneWndCont: Private, inherited methods
// ---------------------------------------------------------------------------

//
//  We leave a small divider between the panes in the direction of travel. So any time we
//  get a new size for the pane, we have to calculate the size of the window inside that
//  pane. In the direction of travel we take half the divider pixels from each pane area at
//  the point where panes touch.
//
tCIDLib::TVoid
TPaneWndCont::AdjustForDiv(         TArea&              areaAdjust
                            , const tCIDLib::TBoolean   bVertDivs
                            , const tCIDLib::TCard4     c4PaneCnt
                            , const tCIDLib::TCard4     c4PaneInd)
{
    // Take half of pixels from each
    const tCIDLib::TCard4 c4Adjust(CIDCtrls_PaneWndMgr::c4DivMargin / 2);
    const tCIDLib::TInt4  i4Adjust(c4Adjust);

    // Adjust for the pane divider
    if (!c4PaneInd)
    {
        // It's the first one, so only adjust the far edge down
        if (bVertDivs)
            areaAdjust.AdjustSize(0, -i4Adjust);
        else
            areaAdjust.AdjustSize(-i4Adjust, 0);
    }
     else if (c4PaneInd + 1 == c4PaneCnt)
    {
        // It's the last one, so only adjust the near edge upwards
        if (bVertDivs)
            areaAdjust.AdjustAndShrink(0, c4Adjust);
        else
            areaAdjust.AdjustAndShrink(c4Adjust, 0);
    }
     else
    {
        // It's a middle one, so just deflate
        if (bVertDivs)
            areaAdjust.Deflate(0, c4Adjust);
        else
            areaAdjust.Deflate(c4Adjust, 0);
    }
}


//
//  The derived class calls this when he wants us to rearrange the windows. He
//  provides us with the area we have to work with.
//
tCIDLib::TVoid TPaneWndCont::AdjustWnds(const TArea& areaNew, const TArea& areaOld)
{
    CIDAssert(m_playMgr != 0, L"No pane layout object has been set");

    const tCIDLib::TCard4 c4PCount = m_playMgr->c4PaneCount();
    m_playMgr->AdjustPanes(m_colPanes, areaNew, areaOld, m_bEmphDir);

    //
    //  Loop through the current panels and add any that changed to a multi-pos
    //  object. Then complete it to move all of the windows.
    //
    TWndMultiPos wmposUpdate(c4PCount);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4PCount; c4Index++)
    {
        TPaneWnd& panewCur = m_colPanes[c4Index];
        if (panewCur.bNewArea(m_areaTmp, m_areaTmp2))
        {
            AdjustForDiv(m_areaTmp, m_playMgr->bVertDrag(), c4PCount, c4Index);
            wmposUpdate.AddWindow(*panewCur.pwndPane(), m_areaTmp);
        }
    }
    wmposUpdate.Complete();
}


//
//  Used to support user dragging of pane divisions. We will check to see if the
//  passed point is between two panes within a couple pixels on either side. If so,
//  we return True, and give back the indices of the two panes on either side, and
//  indicate if it's a horizontal or vertical border (i.e. the direction of movement,
//  so vertical panels become horizontal movement, and vice versa.)
//
tCIDLib::TBoolean
TPaneWndCont::bCheckOnDiv(const TPoint&             pntAt
                        ,       tCIDLib::TCard4&    c4First
                        ,       tCIDLib::TCard4&    c4Sec
                        ,       tCIDLib::TBoolean&  bHorz) const
{
    // Just passit on to the layout manager
    CIDAssert(m_playMgr != 0, L"No pane layout object has been set");
    return m_playMgr->bCheckOnDiv(m_colPanes, pntAt, c4First, c4Sec, bHorz);
}



//
//  If the client code gets a mouse down and bCheckOnDiv returns true, it will do
//  drag detection. If dragging starts, it will call us here. We'll do a modal
//  loop where we move the divider and adjust the window sizes as required.
//
tCIDLib::TVoid
TPaneWndCont::DragDivider(          TWindow&            wndParent
                            , const TArea&              areaAvail
                            , const TPoint&             pntAt
                            , const tCIDLib::TCard4     c4First
                            , const tCIDLib::TCard4     c4Sec
                            , const tCIDLib::TBoolean   bHorz)
{
    CIDAssert(m_playMgr != 0, L"No pane layout object has been set");

    // Remember the pointer we'll set on mouse movement
    tCIDCtrls::ESysPtrs ePtr;
    if (bHorz)
        ePtr = tCIDCtrls::ESysPtrs::SizeWE;
    else
        ePtr = tCIDCtrls::ESysPtrs::SizeNS;

    // For convenience get the two panes out
    TPaneWnd& panewF = m_colPanes[c4First];
    TPaneWnd& panewS = m_colPanes[c4Sec];

    try
    {
        // Capture the mouse
        wndParent.CaptureMouse();

        // And enter the loop
        MSG msgCur;
        TPoint pntDiff;
        TPoint pntLast;
        TGraphWndDev gdevFill(*this);
        while (::GetMessage(&msgCur, 0, 0, 0))
        {
            if (msgCur.message == WM_LBUTTONUP)
            {
                // When the left button is released, we are done
                wndParent.ReleaseMouse();
                break;
            }
             else if (msgCur.message == WM_MOUSEMOVE)
            {
                // Get the of the mouse
                const TPoint pntPos
                (
                    TRawBits::i2Low16From32(msgCur.lParam)
                    , TRawBits::i2High16From32(msgCur.lParam)
                );

                // Set the appropriate cursor
                facCIDCtrls().SetPointer(ePtr);

                //
                //  If there's a change of more than one pixel in either direction, then
                //  let the layout manager update. We keep it to two pixels to avoid
                //  some of the overhead of these drag updates, which can be heavy if
                //  the content displayed is.
                //
                pntPos.AbsDiffFrom(pntLast, pntDiff);
                if (pntDiff.i4LargestDim() > 1)
                {
                    pntLast = pntPos;
                    if (m_playMgr->bDragAdjust(m_colPanes, c4First, c4Sec, pntPos, areaAvail))
                        MinPaneAdjust(gdevFill, c4First, c4Sec, areaAvail, bHorz);
                }
            }
             else
            {
                ::TranslateMessage(&msgCur);
                ::DispatchMessage(&msgCur);
            }
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        wndParent.ReleaseMouse();
        throw;
    }

    catch(...)
    {
        wndParent.ReleaseMouse();
        throw;
    }

    // Force a final redraw to be safe
    Redraw(tCIDCtrls::ERedrawFlags::Full);
}


//
//  This is called during dragging of the divider between two panes. We need to do
//  the absolute most efficient adjustment or there will be lots of flicker.
//
tCIDLib::TVoid
TPaneWndCont::MinPaneAdjust(        TGraphDrawDev&      gdevFill
                            , const tCIDLib::TCard4     c4First
                            , const tCIDLib::TCard4     c4Sec
                            , const TArea&              areaAvail
                            , const tCIDLib::TBoolean   bHorz)
{
    // Get our two panes
    TPaneWnd& panewF = m_colPanes[c4First];
    TPaneWnd& panewS = m_colPanes[c4Sec];

    //
    //  Check for changes, which should exist, but we also need todo this in order
    //  to make it update the new/previous areas.
    //
    if (!panewS.bNewArea(m_areaTmp, m_areaTmp2))
        return;

    //
    //  Calculate the offset in the direction of travel. Use the 2nd one always since
    //  it's left/top will be the one moving, and that's easier.
    //
    const tCIDLib::TInt4 i4Ofs
    (
        bHorz ? m_areaTmp.i4X() - m_areaTmp2.i4X()
              : m_areaTmp.i4Y() - m_areaTmp2.i4Y()
    );

    if (!i4Ofs)
        return;

    TArea areaSet;

    TWndMultiPos wmposUpdate(2);

    areaSet = panewF.areaCur();
    AdjustForDiv(areaSet, m_playMgr->bVertDrag(), m_playMgr->c4PaneCount(), c4First);
    wmposUpdate.AddWindow(*panewF.pwndPane(), areaSet);

    areaSet = panewS.areaCur();
    AdjustForDiv(areaSet, m_playMgr->bVertDrag(), m_playMgr->c4PaneCount(), c4Sec);
    wmposUpdate.AddWindow(*panewS.pwndPane(), areaSet);

    wmposUpdate.Complete();
}
