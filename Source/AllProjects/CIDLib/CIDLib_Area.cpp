//
// FILE NAME: CIDLib_Area.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/1993
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
//  This file implements the TArea class, a 2D rectangular area.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TArea,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TArea
//  PREFIX: area
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TArea: Public, static data
// ---------------------------------------------------------------------------
const TArea  TArea::areaEmpty(0, 0, 0, 0);


// ---------------------------------------------------------------------------
//  TArea: Public, static methods
// ---------------------------------------------------------------------------
TArea& TArea::Nul_TArea()
{
    static TArea* pareaNull = nullptr;
    if (!pareaNull)
    {
        TBaseLock lockInit;
        if (!pareaNull)
            TRawMem::pExchangePtr(&pareaNull, new TArea);
    }
    return *pareaNull;
}


// ---------------------------------------------------------------------------
//  TArea: Constructors and Destructor
// ---------------------------------------------------------------------------
TArea::TArea(   const   tCIDLib::THostRectl&    rectlSrc
                , const tCIDLib::ERectlTypes    eInclusive)
{
    FromRectl(rectlSrc, eInclusive);
}

TArea::TArea(   const   tCIDLib::THostPoint&    ptULeft
                , const tCIDLib::THostPoint&    ptLRt
                , const tCIDLib::ERectlTypes    eInclusive) :

    // Note that this depends upon correct member order!!
    m_i4X(tCIDLib::MinVal(ptULeft.i4X, ptLRt.i4X))
    , m_i4Y(tCIDLib::MinVal(ptULeft.i4Y, ptLRt.i4Y))
    , m_c4CX(tCIDLib::MaxVal(ptULeft.i4X, ptLRt.i4X) - m_i4X)
    , m_c4CY(tCIDLib::MaxVal(ptULeft.i4Y, ptLRt.i4Y) - m_i4Y)
{
    // If they were inclusive, then bump the sizes
    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        m_c4CX++;
        m_c4CY++;
    }
}

TArea::TArea(const  TPoint&                 pntULeft
            , const TPoint&                 pntLRt
            , const tCIDLib::ERectlTypes    eInclusive)
{
    // Get the values out of the points for greater efficiency
    tCIDLib::TInt4 i4X1 = pntULeft.m_i4X;
    tCIDLib::TInt4 i4Y1 = pntULeft.m_i4Y;
    tCIDLib::TInt4 i4X2 = pntLRt.m_i4X;
    tCIDLib::TInt4 i4Y2 = pntLRt.m_i4Y;

    m_i4X   = tCIDLib::MinVal(i4X1, i4X2);
    m_c4CX  = i4X1 > i4X2 ? i4X1 - i4X2 : i4X2 - i4X1;
    m_i4Y   = tCIDLib::MinVal(i4Y1, i4Y2);
    m_c4CY  = i4Y1 > i4Y2 ? i4Y1 - i4Y2 : i4Y2 - i4Y1;

    // If they were inclusive, then bump the sizes
    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        m_c4CX++;
        m_c4CY++;
    }
}


// Just a direct copy of the values
TArea::TArea(const tCIDLib::TRawArea& areaSrc) :

    m_i4X(areaSrc.i4X)
    , m_i4Y(areaSrc.i4Y)
    , m_c4CX(areaSrc.c4CX)
    , m_c4CY(areaSrc.c4CY)
{
}

TArea::TArea(   const   tCIDLib::TInt4  i4X
                , const tCIDLib::TInt4  i4Y
                , const tCIDLib::TCard4 c4CX
                , const tCIDLib::TCard4 c4CY) :
    m_i4X(i4X)
    , m_i4Y(i4Y)
    , m_c4CX(c4CX)
    , m_c4CY(c4CY)
{
}

TArea::TArea(const TPoint& pntOrg, const TSize& szExtent) :

    m_i4X(pntOrg.m_i4X)
    , m_i4Y(pntOrg.m_i4Y)
    , m_c4CX(szExtent.c4Width())
    , m_c4CY(szExtent.c4Height())
{
}

TArea::TArea(   const   TPoint&         pntOrg
                , const tCIDLib::TCard4 c4CX
                , const tCIDLib::TCard4 c4CY) :

    m_i4X(pntOrg.m_i4X)
    , m_i4Y(pntOrg.m_i4Y)
    , m_c4CX(c4CX)
    , m_c4CY(c4CY)
{
}


// ---------------------------------------------------------------------------
//  TArea: Public operators
// ---------------------------------------------------------------------------
TArea& TArea::operator=(const tCIDLib::TRawArea& areaSrc)
{
    m_i4X = areaSrc.i4X;
    m_i4Y = areaSrc.i4Y;
    m_c4CX = areaSrc.c4CX;
    m_c4CY = areaSrc.c4CY;
    return *this;
}


// OR returns the area that contains both input areas
TArea operator|(const TArea& area1, const TArea& area2)
{
    TArea areaNew(area1);
    areaNew |= area2;
    return areaNew;
}

tCIDLib::TVoid TArea::operator|=(const TArea& areaToOR)
{
    // If the same object, then just return
    if (&areaToOR == this)
        return;

    tCIDLib::TInt4   i4L, i4R, i4B, i4T;

    //
    //  Take the smaller of the origin points and the larger of the upper
    //  and right points.
    //
    i4L = tCIDLib::MinVal(m_i4X, areaToOR.m_i4X);
    i4T = tCIDLib::MinVal(m_i4Y, areaToOR.m_i4Y);
    i4R = tCIDLib::MaxVal(i4Right(), areaToOR.i4Right());
    i4B = tCIDLib::MaxVal(i4Bottom(), areaToOR.i4Bottom());

    m_i4X   = i4L;
    m_i4Y   = i4T;
    m_c4CX  = (i4R - i4L);
    m_c4CY  = (i4B - i4T);
}


//
//  Removes the second area from the first area. It still leaves a rectangle, so it's
//  not creating a complex path. So, if area 2's X is less than area 1's right, then
//  area 1's right is set to area 2's X, and so forth. If no overlap, then the first
//  area is left unchanged.
//
TArea operator-(const TArea& area1, const TArea& area2)
{
    TArea areaNew(area1);
    areaNew -= area2;
    return areaNew;
}

tCIDLib::TVoid TArea::operator-=(const TArea& areaToSub)
{
    // If the same object, then just return
    if (&areaToSub == this)
        return;

    // To much simplify things, get both our right/bottom positions
    tCIDLib::TInt4  i4ThisRight    = i4Right() ;
    tCIDLib::TInt4  i4ThisBottom   = i4Bottom();
    tCIDLib::TInt4  i4TestRight    = areaToSub.i4Right();
    tCIDLib::TInt4  i4TestBottom   = areaToSub.i4Bottom();

    //
    //  If his origin is is past our LR or his LR is before our origin,
    //  then nothing to do.
    //
    if (((areaToSub.m_i4X >= i4ThisRight) && (areaToSub.m_i4Y >= i4ThisBottom))
    ||  ((i4TestRight < m_i4X) && (i4TestBottom < m_i4Y)))
    {
        return;
    }

    //
    //  Ok, looks like we have something in common. If his origin is past
    //  ours, then his left becomes our right and his top becomes our
    //  bottom.
    //
    //  Otherwise, his bottom becomes our top and his right becomes our
    //  left.
    //
    if (areaToSub.m_i4X > m_i4X)
        i4Right(areaToSub.m_i4X);
    else if (areaToSub.m_i4X < m_i4X)
        i4Left(i4TestRight, kCIDLib::True);

    if (areaToSub.m_i4Y > m_i4Y)
        i4Bottom(areaToSub.m_i4Y);
    else if (areaToSub.m_i4Y < m_i4Y)
        i4Top(i4TestBottom, kCIDLib::True);
}


// Calculates the overlapped part of the two input areas, empty if they don't overlap
TArea operator&(const TArea& area1, const TArea& area2)
{
    TArea areaRet(area1);
    areaRet &= area2;
    return areaRet;
}

tCIDLib::TVoid TArea::operator&=(const TArea& areaToAND)
{
    // If the same object, then nothing to do
    if (&areaToAND == this)
        return;

    tCIDLib::TInt4   i4L, i4T, i4R, i4B;

    //
    //  Take the larger of the origin points and the smaller of the upper
    //  and right points. Bump the L/R values for the calculations.
    //
    i4L = tCIDLib::MaxVal(m_i4X, areaToAND.m_i4X);
    i4T = tCIDLib::MaxVal(m_i4Y, areaToAND.m_i4Y);
    i4R = tCIDLib::MinVal(i4Right(), areaToAND.i4Right());
    i4B = tCIDLib::MinVal(i4Bottom(), areaToAND.i4Bottom());

    //
    //  See if there is any common area. If the right is less than the
    //  left or the bottom less than the top, then none. So we would return
    //  and empty area.
    //
    m_i4X = i4L;
    m_i4Y = i4T;

    if (i4R < i4L)
        m_c4CX = 0;
    else
        m_c4CX = i4R - i4L;

    if (i4B < i4T)
        m_c4CY = 0;
    else
        m_c4CY = i4B - i4T;
}


// Test for equal areas
tCIDLib::TBoolean TArea::operator==(const TArea& areaToTest) const
{
    if (this == &areaToTest)
        return kCIDLib::True;

    if (m_i4X != areaToTest.m_i4X)
        return kCIDLib::False;
    if (m_i4Y != areaToTest.m_i4Y)
        return kCIDLib::False;
    if (m_c4CX != areaToTest.m_c4CX)
        return kCIDLib::False;
    if (m_c4CY != areaToTest.m_c4CY)
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean TArea::operator!=(const TArea& areaToTest) const
{
    return !operator==(areaToTest);
}


// Adjust the origin by the indicated point
tCIDLib::TVoid TArea::operator+=(const TPoint& pntToAddToOrg)
{
    AdjustOrg(pntToAddToOrg);
}

tCIDLib::TVoid TArea::operator-=(const TPoint& pntToSubFromOrg)
{
    AdjustOrg(-pntToSubFromOrg.i4X(), -pntToSubFromOrg.i4Y());
}


// ---------------------------------------------------------------------------
//  TArea: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If either length isn't large enough to handle the adjustment, we just
//  zero it out. This is the best we can do.
//
tCIDLib::TVoid TArea::AdjustAndShrink(  const   tCIDLib::TCard4 c4XAdj
                                        , const tCIDLib::TCard4 c4YAdj)
{
    m_i4X += tCIDLib::TInt4(c4XAdj);
    if (m_c4CX < c4XAdj)
        m_c4CX = 0;
    else
        m_c4CX -= c4XAdj;

    m_i4Y += tCIDLib::TInt4(c4YAdj);
    if (m_c4CY < c4YAdj)
        m_c4CY = 0;
    else
        m_c4CY -= c4YAdj;
}


// Adusts the origin of this area by the indicated values
tCIDLib::TVoid
TArea::AdjustOrg(const tCIDLib::TInt4 i4XOfs, const tCIDLib::TInt4 i4YOfs)
{
    m_i4X += i4XOfs;
    m_i4Y += i4YOfs;
}

tCIDLib::TVoid TArea::AdjustOrg(const TPoint& pntOfs)
{
    m_i4X += pntOfs.m_i4X;
    m_i4Y += pntOfs.m_i4Y;
}


// Adjusts the sizes of this area by the passed values
tCIDLib::TVoid
TArea::AdjustSize(const tCIDLib::TInt4 i4CXOfs, const tCIDLib::TInt4 i4CYOfs)
{
    m_c4CX += i4CXOfs;
    m_c4CY += i4CYOfs;

}

tCIDLib::TVoid TArea::AdjustSize(const TPoint& pntOfs)
{
    m_c4CX += pntOfs.m_i4X;
    m_c4CY += pntOfs.m_i4Y;
}


// Sees if the passed area overlaps this area horizontally
tCIDLib::TBoolean TArea::bHorzOverlap(const TArea& areaTest) const
{
    if ((areaTest.m_i4X > i4Right()) || (areaTest.i4Right() < m_i4X))
        return kCIDLib::False;
    return kCIDLib::True;
}


//
//  Indicates if the passed area is all within our area. It's inclusive so
//  any side can be equal to our side, i.e. it doesn't have to be less than
//  our sides.
//
tCIDLib::TBoolean TArea::bContainsArea(const TArea& areaTest) const
{
    // Make sure that target's coordinates are all inside or equal to ours
    if (areaTest.m_i4X < m_i4X)
        return kCIDLib::False;
    if (areaTest.m_i4Y < m_i4Y)
        return kCIDLib::False;
    if (areaTest.i4Right() > i4Right())
        return kCIDLib::False;
    if (areaTest.i4Bottom() > i4Bottom())
        return kCIDLib::False;

    return kCIDLib::True;
}


//
//  Indicates if this area contains the passed point. It's inclusive, so it can
//  be on our edges, not just inside the edges.
//
tCIDLib::TBoolean
TArea::bContainsPoint(  const   tCIDLib::TInt4  i4X
                        , const tCIDLib::TInt4  i4Y) const
{
    if (i4X < m_i4X)
        return kCIDLib::False;
    if (i4Y < m_i4Y)
        return kCIDLib::False;
    if (i4X > i4Right())
        return kCIDLib::False;
    if (i4Y > i4Bottom())
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean TArea::bIntersects(const TArea& areaTest) const
{
    // If either is empty, then cannot intersect
    if (bIsEmpty() || areaTest.bIsEmpty())
        return kCIDLib::False;

    // Get the upper extremes of each area
    tCIDLib::TInt4  i4ThisRight    = i4Right();
    tCIDLib::TInt4  i4ThisBottom   = i4Bottom();
    tCIDLib::TInt4  i4TestRight    = areaTest.i4Right();
    tCIDLib::TInt4  i4TestBottom   = areaTest.i4Bottom();

    // Check the sides
    if (areaTest.m_i4X > i4ThisRight)
        return kCIDLib::False;
    if (m_i4X > i4TestRight)
        return kCIDLib::False;

    if (areaTest.m_i4Y > i4ThisBottom)
        return kCIDLib::False;
    if (m_i4Y > i4TestBottom)
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean TArea::bIsInside(const TArea& areaTest) const
{
    //
    //  Make sure that we are fully inside (i.e. don't touch the edges of)
    //  the target target.
    //
    const tCIDLib::TInt4 i4TR = areaTest.i4Right();
    const tCIDLib::TInt4 i4TB = areaTest.i4Bottom();
    const tCIDLib::TInt4 i4R = i4Right();
    const tCIDLib::TInt4 i4B = i4Bottom();

    if ((m_i4X <= areaTest.m_i4X)
    ||  (m_i4Y <= areaTest.m_i4Y)
    ||  (i4R >= i4TR)
    ||  (i4B >= i4TB))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Moves the point the minimium amount to bring it within this area. So if
//  it is outside this area in either axis, it will be brought to the nearest
//  edge in that axis. The return indicates whether we adjusted the point
//  or not.
//
tCIDLib::TBoolean TArea::bMoveWithin(TPoint& pntToMove) const
{
    const tCIDLib::TInt4 i4R = i4Right();
    const tCIDLib::TInt4 i4B = i4Bottom();

    const tCIDLib::TInt4 i4OrgX = pntToMove.i4X();
    const tCIDLib::TInt4 i4OrgY = pntToMove.i4Y();

    tCIDLib::TInt4 i4NewX = i4OrgX;
    tCIDLib::TInt4 i4NewY = i4OrgY;

    if (i4OrgX < m_i4X)
        i4NewX = m_i4X;
    else if (i4OrgX > i4R)
        i4NewX = i4R;

    if (i4OrgY < m_i4Y)
        i4NewY = m_i4Y;
    else if (i4OrgY > i4B)
        i4NewY = i4B;

    if ((i4OrgX == i4NewX) && (i4OrgY == i4NewY))
        return kCIDLib::False;

    pntToMove.Set(i4NewX, i4NewY);
    return kCIDLib::True;
}


//
//  Parses an area of out a string. It expects the format to be x,y,cx,cy
//  but you can indicate another separate character as a parameter, as long
//  as the values are in the right order.
//
tCIDLib::TBoolean
TArea::bParseFromText(  const   TString&            strText
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chSepChar)
{
    // We need to pull the separate tokens out from the string
    TString strTmp(chSepChar);
    strTmp.Append(kCIDLib::chSpace);
    TStringTokenizer stokParse(&strText, strTmp);

    tCIDLib::TBoolean bOk;
    tCIDLib::TInt4 i4X;
    tCIDLib::TInt4 i4Y;
    tCIDLib::TCard4 c4CX;
    tCIDLib::TCard4 c4CY;

    if (!stokParse.bGetNextToken(strTmp))
        return kCIDLib::False;
    i4X = TRawStr::i4AsBinary(strTmp.pszBuffer(), bOk, eRadix);
    if (!bOk)
        return kCIDLib::False;

    if (!stokParse.bGetNextToken(strTmp))
        return kCIDLib::False;
    i4Y = TRawStr::i4AsBinary(strTmp.pszBuffer(), bOk, eRadix);
    if (!bOk)
        return kCIDLib::False;

    if (!stokParse.bGetNextToken(strTmp))
        return kCIDLib::False;
    c4CX = TRawStr::c4AsBinary(strTmp.pszBuffer(), bOk, eRadix);
    if (!bOk)
        return kCIDLib::False;

    if (!stokParse.bGetNextToken(strTmp))
        return kCIDLib::False;
    c4CY = TRawStr::c4AsBinary(strTmp.pszBuffer(), bOk, eRadix);
    if (!bOk)
        return kCIDLib::False;

    // Looks ok, so store the values
    m_i4X = i4X;
    m_i4Y = i4Y;
    m_c4CX = c4CX;
    m_c4CY = c4CY;

    return kCIDLib::True;
}


//
//  Calculates the position of the passed point relative to the origin of this
//  area, in terms of a percentage of the size of this area in those axes. So
//  if the point was at the center of this area, they would be 0.5 in both
//  cases.
//
tCIDLib::TBoolean
TArea::bPercentFromOrg( const   TPoint&             pntToTest
                        ,       tCIDLib::TFloat8&   f8XPercent
                        ,       tCIDLib::TFloat8&   f8YPercent) const
{
    if (!bContainsPoint(pntToTest))
        return kCIDLib::False;

    tCIDLib::TFloat8 f8Relative = tCIDLib::TFloat8(pntToTest.m_i4X - m_i4X);
    tCIDLib::TFloat8 f8Range = tCIDLib::TFloat8(m_c4CX);
    f8XPercent = f8Relative / f8Range;

    f8Relative = tCIDLib::TFloat8(pntToTest.m_i4Y - m_i4Y);
    f8Range = tCIDLib::TFloat8(m_c4CY);
    f8YPercent = f8Relative / f8Range;

    return kCIDLib::True;
}


//
//  Indicates whether this area is the same origin as the passed area,
//  ignoring the sizes.
//
tCIDLib::TBoolean TArea::bSameOrg(const TArea& areaToTest) const
{
    return ((areaToTest.m_i4X == m_i4X) && (areaToTest.m_i4Y == m_i4Y));
}

//
//  Indicates whether this area is the same size as the passed area,
//  ignoring the origins.
//
tCIDLib::TBoolean TArea::bSameSize(const TArea& areaToTest) const
{
    return ((areaToTest.m_c4CX == m_c4CX) && (areaToTest.m_c4CY == m_c4CY));
}


// Indicates whether this area overlaps the passed area vertically
tCIDLib::TBoolean TArea::bVertOverlap(const TArea& areaTest) const
{
    if ((areaTest.m_i4Y > i4Bottom()) || (areaTest.i4Bottom() < m_i4Y))
        return kCIDLib::False;
    return kCIDLib::True;
}


//
//  Make the bottom come out at this new bottom, by adjusting the origin. It is
//  assumed to be non-inclusive.
//
tCIDLib::TVoid TArea::BottomAlign(const tCIDLib::TInt4 i4NewBottom)
{
    // Set the origin to put the bottom at this new bottom
    m_i4Y = (i4NewBottom - tCIDLib::TInt4(m_c4CY));
}


//
//  Bottom justifies this area in the passed area, optionally centering it
//  horizontally at the same time.
//
tCIDLib::TVoid TArea::BottomJustifyIn(  const   TArea&              areaIn
                                        , const tCIDLib::TBoolean   bHCenter)
{
    m_i4Y = (areaIn.i4Bottom() - m_c4CY);
    if (bHCenter)
    {
        m_i4X = areaIn.m_i4X + (areaIn.m_c4CX >> 1);
        m_i4X -= (m_c4CX >> 1);
    }
}


// Centers this area within the passed area
tCIDLib::TVoid TArea::CenterIn(const TArea& areaTarget)
{
    JustifyIn(areaTarget, tCIDLib::EHJustify::Center, tCIDLib::EVJustify::Center);
}


// Centers this area on the passed point
tCIDLib::TVoid TArea::CenterOn(const TPoint& pntTarget)
{
    m_i4X = pntTarget.i4X() - tCIDLib::TInt4(m_c4CX / 2);
    m_i4Y = pntTarget.i4Y() - tCIDLib::TInt4(m_c4CY / 2);
}


// Takes the size of the passed area but sets our orign to zero
tCIDLib::TVoid TArea::FromAreaAtOrg(const TArea& areaSource)
{
    m_i4X = 0;
    m_i4Y = 0;
    m_c4CX = areaSource.m_c4CX;
    m_c4CY = areaSource.m_c4CY;
}


//
//  Takes the origin of the passed source area and takes the size, but scaled
//  by the passed percentage factors. It can't be larger than 1.0, i.e. 100%.
//
tCIDLib::TVoid
TArea::FromAreaScaled(  const   TArea&              areaSource
                        , const tCIDLib::TFloat8    f8CXScale
                        , const tCIDLib::TFloat8    f8CYScale)
{
    // Insure the scaling values are legal
    if ((f8CXScale < 0.0) || (f8CXScale > 1.0)
    ||  (f8CYScale < 0.0) || (f8CYScale > 1.0))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcArea_BadScaling
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f8CXScale)
            , TFloat(f8CYScale)
        );
    }

    m_i4X = areaSource.m_i4X;
    m_i4Y = areaSource.m_i4Y;
    m_c4CX = TMathLib::c4Round(areaSource.m_c4CX * f8CXScale);
    m_c4CY = TMathLib::c4Round(areaSource.m_c4CY * f8CYScale);
}


//
//  Sets our origin and size from the passed UL/LR corner points.
//
tCIDLib::TVoid
TArea::FromPoints(  const   TPoint&                 pntULeft
                    , const TPoint&                 pntLRight
                    , const tCIDLib::ERectlTypes    eInclusive)
{
    if (pntULeft.m_i4X > pntLRight.m_i4X)
    {
        m_i4X  = pntLRight.m_i4X;
        m_c4CX = (pntULeft.m_i4X - pntLRight.m_i4X);
    }
     else
    {
        m_i4X  = pntULeft.m_i4X;
        m_c4CX = (pntLRight.m_i4X - pntULeft.m_i4X);
    }

    if (pntULeft.m_i4Y > pntLRight.m_i4Y)
    {
        m_i4Y  = pntLRight.m_i4Y;
        m_c4CY = (pntULeft.m_i4Y - pntLRight.m_i4Y);
    }
     else
    {
        m_i4Y  = pntULeft.m_i4Y;
        m_c4CY = (pntLRight.m_i4Y - pntULeft.m_i4Y);
    }

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (m_c4CX)
            m_c4CX++;
        if (m_c4CY)
            m_c4CY++;
    }
}

tCIDLib::TVoid
TArea::FromPoints(  const   tCIDLib::THostPoint&    ptlULeft
                    , const tCIDLib::THostPoint&    ptlLRight
                    , const tCIDLib::ERectlTypes    eInclusive)
{
    if (ptlULeft.i4X > ptlLRight.i4X)
    {
        m_i4X  = ptlLRight.i4X;
        m_c4CX = tCIDLib::TCard4(ptlULeft.i4X - ptlLRight.i4X);
    }
     else
    {
        m_i4X  = ptlULeft.i4X;
        m_c4CX = tCIDLib::TCard4(ptlLRight.i4X - ptlULeft.i4X);
    }

    if (ptlULeft.i4Y > ptlLRight.i4Y)
    {
        m_i4Y  = ptlLRight.i4Y;
        m_c4CY = tCIDLib::TCard4(ptlULeft.i4Y - ptlLRight.i4Y);
    }
     else
    {
        m_i4Y  = ptlULeft.i4Y;
        m_c4CY = tCIDLib::TCard4(ptlLRight.i4Y - ptlULeft.i4Y);
    }

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (m_c4CX)
            m_c4CX++;
        if (m_c4CY)
            m_c4CY++;
    }
}


//
//  Sets our origin/size based on a host rectangle, which can be inclusive or non-
//  inclusive. We just subtract left/top from right/bottom which is correct for a
//  non-inclusive rectangle (effectively throwing away the extra pixel.) If it's
//  inclusive then we increment the width/height.
//
tCIDLib::TVoid TArea::FromRectl(const   tCIDLib::THostRectl&    rectlSrc
                                , const tCIDLib::ERectlTypes    eInclusive)
{
    // We initially assume non-inclusive
    if (rectlSrc.i4Left > rectlSrc.i4Right)
    {
        m_i4X = rectlSrc.i4Right;
        m_c4CX = tCIDLib::TCard4(rectlSrc.i4Left - rectlSrc.i4Right);
    }
     else
    {
        m_i4X  = rectlSrc.i4Left;
        m_c4CX = tCIDLib::TCard4(rectlSrc.i4Right - rectlSrc.i4Left);
    }

    if (rectlSrc.i4Bottom > rectlSrc.i4Top)
    {
        m_i4Y  = rectlSrc.i4Top;
        m_c4CY = tCIDLib::TCard4(rectlSrc.i4Bottom - rectlSrc.i4Top);
    }
     else
    {
        m_i4Y  = rectlSrc.i4Bottom;
        m_c4CY = tCIDLib::TCard4(rectlSrc.i4Top - rectlSrc.i4Bottom);
    }

    // If the original was inclusive, then bump the values
    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        m_c4CX++;
        m_c4CY++;
    }
}


//
//  Forces this area within the passed on. It doesn't move the origin unless
//  it's behind the fit area, in which case we move them up to the fit area's
//  origin. Then, if that would hang the right or bottom off, it will ajdust
//  the origins back towards the fit origin if it can in order to get the
//  fit. If it still hangs off the bottom or right, it's allowed to unless
//  bTotal is true, which case our sizes are truncated to make it fit.
//
tCIDLib::TVoid
TArea::ForceWithin(const TArea& areaToFit, const tCIDLib::TBoolean bTotal)
{
    if (m_i4X < areaToFit.m_i4X)
        m_i4X = areaToFit.m_i4X;

    if (m_i4Y < areaToFit.m_i4Y)
        m_i4Y = areaToFit.m_i4Y;


    if (i4Right() > areaToFit.i4Right())
    {
        //
        //  The right is hanging off. If the area to fit is bigger than
        //  us, just right justify us. Else, set our x to his, and if
        //  total, truncate us to fit.
        //
        if (m_c4CX <= areaToFit.m_c4CX)
        {
            m_i4X = areaToFit.i4Right() - tCIDLib::TInt4(m_c4CX);
        }
         else
        {
            if (m_i4X > areaToFit.m_i4X)
                m_i4X = areaToFit.m_i4X;

            if (bTotal)
            {
                if (i4Right() > areaToFit.i4Right())
                    m_c4CX = areaToFit.m_c4CX;
            }
        }
    }

    if (i4Bottom() > areaToFit.i4Bottom())
    {
        //
        //  The bottom is hanging off. If the area to fit is shorter than
        //  us, just bottom justify it. Else, set it's y to ours, and if
        //  total, truncate it to fit.
        //
        if (m_c4CY <= areaToFit.m_c4CY)
        {
            m_i4Y = areaToFit.i4Bottom() - tCIDLib::TInt4(m_c4CY);
        }
         else
        {
            if (m_i4Y >= areaToFit.m_i4Y)
                m_i4Y = areaToFit.m_i4Y;

            if (bTotal)
            {
                if (i4Bottom() > areaToFit.i4Bottom())
                    m_c4CY = areaToFit.m_c4CY;
            }
        }
    }
}


// Format us out to a string
tCIDLib::TVoid
TArea::FormatToText(        TString&            strTar
                    , const tCIDLib::TCh        chSepChar
                    , const tCIDLib::TBoolean   bAppend) const
{
    if (!bAppend)
        strTar.Clear();

    strTar.AppendFormatted(m_i4X);
    strTar.Append(chSepChar);
    strTar.AppendFormatted(m_i4Y);
    strTar.Append(chSepChar);
    strTar.AppendFormatted(m_c4CX);
    strTar.Append(chSepChar);
    strTar.AppendFormatted(m_c4CY);
}


// This is always inclusive, so if y=1 and cy=1, you get 1
tCIDLib::TInt4 TArea::i4Bottom() const
{
    return m_i4Y + tCIDLib::TInt4(m_c4CY);
}


//
//  Set a new bototm . Assumed to be non-inclusive. So if the bottom is equal the
//  top, it's zero.
//
tCIDLib::TInt4 TArea::i4Bottom(const tCIDLib::TInt4 i4NewBottom)
{
    // Make sure that it is not less than the origin
    if (i4NewBottom < m_i4Y)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcArea_BehindOrg
            , facCIDLib().strMsg(kCIDMsgs::midGen_vertical)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(i4NewBottom)
            , TInteger(m_i4Y)
        );
    }
    m_c4CY = tCIDLib::TCard4(i4NewBottom - m_i4Y);
    return i4NewBottom;
}


//
//  Sets the various sides. When setting the left or top, it can optionally
//  lock the other side, which will adjust the size in that axis to keep
//  the right or bottom at the same postion.
//
//  We have some synonyms here, which do the same thing, just under a
//  different name (i.e. i4Left and i4X.)
//
tCIDLib::TInt4 TArea::i4Left(const  tCIDLib::TInt4      i4NewLeft
                            , const tCIDLib::TBoolean   bLockRight)
{
    //
    //  If the right is locked, we need to adjust the horz size to keep
    //  the right at the same spot. We clip it back if needed so that
    //  it never underflows.
    //
    if (bLockRight)
    {
        // Calc the amount to adjust, and clip if needed
        const tCIDLib::TInt4 i4Adj = m_i4X - i4NewLeft;
        if (tCIDLib::TInt4(m_c4CX) + i4Adj >= 0)
            m_c4CX += i4Adj;
        else
            m_c4CX = 0;
    }
    m_i4X = i4NewLeft;
    return m_i4X;
}


// This is always inclusive! So if x=1 and cx=1, then you get 1
tCIDLib:: TInt4 TArea::i4Right() const
{
    return m_i4X + tCIDLib::TInt4(m_c4CX);
}


//
//  Set a new right side. Assumed to be non-inclusive. So if the right is equal the
//  left, it's 0 pixel in size.
//
tCIDLib:: TInt4 TArea::i4Right(const tCIDLib::TInt4 i4NewXRight)
{
    // Make sure that it is not less than the origin
    if (i4NewXRight < m_i4X)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcArea_BehindOrg
            , facCIDLib().strMsg(kCIDMsgs::midGen_horizontal)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(i4NewXRight)
            , TInteger(m_i4X)
        );
    }
    m_c4CX = tCIDLib::TCard4(i4NewXRight - m_i4X);
    return i4NewXRight;
}


tCIDLib::TInt4 TArea::i4Top(const tCIDLib::TInt4        i4NewTop
                            , const tCIDLib::TBoolean   bLockBottom)
{
    //
    //  If the bottom is locked, we need to adjust the vert size to keep
    //  the bottom at the same spot. We clip it back if needed so that
    //  it never underflows.
    //
    if (bLockBottom)
    {
        // Calc the amount to adjust, and clip if needed
        const tCIDLib::TInt4 i4Adj = m_i4Y - i4NewTop;
        if (tCIDLib::TInt4(m_c4CY) + i4Adj >= 0)
            m_c4CY += i4Adj;
        else
            m_c4CY = 0;
    }
    m_i4Y = i4NewTop;
    return m_i4Y;
}


tCIDLib::TInt4 TArea::i4X(  const   tCIDLib::TInt4      i4NewX
                            , const tCIDLib::TBoolean   bLockRight)
{
    //
    //  If the right is locked, we need to adjust the horz size to keep
    //  the right at the same spot. We clip it back if needed so that
    //  it never underflows.
    //
    if (bLockRight)
    {
        // Calc the amount to adjust, and clip if needed
        const tCIDLib::TInt4 i4Adj = m_i4X - i4NewX;
        if (tCIDLib::TInt4(m_c4CX) + i4Adj >= 0)
            m_c4CX += i4Adj;
        else
            m_c4CX = 0;
    }
    m_i4X = i4NewX;
    return m_i4X;
}

tCIDLib::TInt4 TArea::i4Y(  const   tCIDLib::TInt4      i4NewY
                            , const tCIDLib::TBoolean   bLockBottom)
{
    //
    //  If the bottom is locked, we need to adjust the vert size to keep
    //  the bottom at the same spot. We clip it back if needed so that
    //  it never underflows.
    //
    if (bLockBottom)
    {
        // Calc the amount to adjust, and clip if needed
        const tCIDLib::TInt4 i4Adj = m_i4Y - i4NewY;
        if (tCIDLib::TInt4(m_c4CY) + i4Adj >= 0)
            m_c4CY += i4Adj;
        else
            m_c4CY = 0;
    }
    m_i4Y = i4NewY;
    return m_i4Y;
}


// Justifies this area within the target area
tCIDLib::TVoid
TArea::JustifyIn(   const   TArea&              areaTarget
                    , const tCIDLib::EHJustify  eHJustify
                    , const tCIDLib::EVJustify  eVJustify)
{
    switch(eHJustify)
    {
        case tCIDLib::EHJustify::Left    :
            //  Set the x origin of this area to that of the target
            m_i4X = areaTarget.m_i4X;
            break;

        case tCIDLib::EHJustify::Center  :
            // Set the y to target.center/2 - cx/2
            m_i4X = areaTarget.m_i4X + (areaTarget.m_c4CX >> 1);
            m_i4X -= (m_c4CX >> 1);
            break;

        case tCIDLib::EHJustify::Right   :
            //  Set the x origin of this area to target.i4Right-cx
            m_i4X = areaTarget.i4Right() - m_c4CX;
            break;

        case tCIDLib::EHJustify::Count :
            // Keep our original value
            break;

        default :
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TInteger(tCIDLib::i4EnumOrd(eHJustify))
                , TString(L"tCIDLib::EHJustify")
            );
            break;
    }

    switch(eVJustify)
    {
        case tCIDLib::EVJustify::Top  :
            //  Set the y origin of this area to that of the target
            m_i4Y = areaTarget.m_i4Y;
            break;

        case tCIDLib::EVJustify::Center  :
            // Set the y to target.center/2 - cy/2
            m_i4Y = areaTarget.m_i4Y + (areaTarget.m_c4CY >> 1);
            m_i4Y -= (m_c4CY >> 1);
            break;

        case tCIDLib::EVJustify::Bottom :
            //  Set the y origin of this area to target.i4Bottom-cy
            m_i4Y = areaTarget.i4Bottom() - m_c4CY;
            break;

        case tCIDLib::EVJustify::Count :
            // Keep our original value
            break;

        default :
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TInteger(tCIDLib::i4EnumOrd(eVJustify))
                , TString(L"tCIDLib::EVJustify")
            );
            break;
    }
}


tCIDLib::TVoid TArea::LeftJustifyIn(const   TArea&              areaIn
                                    , const tCIDLib::TBoolean   bVCenter)
{
    m_i4X = areaIn.m_i4X;
    if (bVCenter)
    {
        m_i4Y = areaIn.m_i4Y + (areaIn.m_c4CY >> 1);
        m_i4Y -= (m_c4CY >> 1);
    }
}


//
//  Moves this area's origin in the indicated direction, by the size of the
//  area in that direction.
//
tCIDLib::TVoid TArea::Move(const tCIDLib::EDirs eDir)
{
    switch(eDir)
    {
        case tCIDLib::EDirs::Up :
            m_i4Y -= m_c4CY;
            break;

        case tCIDLib::EDirs::Down :
            m_i4Y += m_c4CY;
            break;

        case tCIDLib::EDirs::Left :
            m_i4X -= m_c4CX;
            break;

        case tCIDLib::EDirs::Right :
            m_i4X += m_c4CX;
            break;

        default :
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TInteger(tCIDLib::i4EnumOrd(eDir))
                , TString(L"tCIDLib::EDirections")
            );
            break;
    }
}


// Returns the center point of this area.
TPoint TArea::pntCenter() const
{
    return TPoint(m_i4X + (m_c4CX >> 1), m_i4Y + (m_c4CY >> 1));
}


// Like bParseFromText but throws if bad
tCIDLib::TVoid
TArea::ParseFromText(   const   TString&            strText
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chSepChar)
{
    // Call the other version and throw if it returns false
    if (!bParseFromText(strText, eRadix, chSepChar))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcArea_BadFormat
            , strText
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
}


//
//  Make the right come out at this new right, by adjusting the origin. It is
//  assumed to be non-inclusive.
//
tCIDLib::TVoid TArea::RightAlign(const tCIDLib::TInt4 i4NewRight)
{
    // Set the origin to put the right at this new right
    m_i4X = (i4NewRight - m_c4CX);
}


//
//  Right justifies the passed area within this area, optionally centering
//  it vertically.
//
tCIDLib::TVoid TArea::RightJustifyIn(const  TArea&              areaIn
                                    , const tCIDLib::TBoolean   bVCenter)
{
    m_i4X = (areaIn.i4Right() - m_c4CX);
    if (bVCenter)
    {
        m_i4Y = areaIn.m_i4Y + (areaIn.m_c4CY >> 1);
        m_i4Y -= (m_c4CY >> 1);
    }

}


//
//  Scales up the size components by the passed horz/vertical scaling factors.
//  They can be larger than 1.0.
//
tCIDLib::TVoid
TArea::ScaleSize(   const   tCIDLib::TFloat8 f8CXScale
                    , const tCIDLib::TFloat8 f8CYScale)
{
    // Insure the scaling values are legal
    if ((f8CXScale < 0.0) || (f8CYScale < 0.0))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcArea_BadScaling
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f8CXScale)
            , TFloat(f8CYScale)
        );
    }

    tCIDLib::TFloat8 f8Tmp = m_c4CX * f8CXScale;
    if ((f8Tmp < 0.0) || (f8Tmp > kCIDLib::c4MaxCard))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcArea_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TFloat(f8Tmp, 0)
            , facCIDLib().strMsg(kCIDMsgs::midGen_size)
        );
    }
    m_c4CX = TMathLib::c4Round(f8Tmp);

    f8Tmp = m_c4CY * f8CYScale;
    if ((f8Tmp < 0.0) || (f8Tmp > kCIDLib::c4MaxCard))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcArea_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TFloat(f8Tmp, 0)
            , facCIDLib().strMsg(kCIDMsgs::midGen_size)
        );
    }
    m_c4CY = TMathLib::c4Round(f8Tmp);
}


// Directly various components of the area
tCIDLib::TVoid TArea::Set(  const   tCIDLib::TInt4  i4X
                            , const tCIDLib::TInt4  i4Y
                            , const tCIDLib::TCard4 c4CX
                            , const tCIDLib::TCard4 c4CY)
{
    m_i4X = i4X;
    m_i4Y = i4Y;
    m_c4CX = c4CX;
    m_c4CY = c4CY;
}

tCIDLib::TVoid TArea::Set(const TPoint& pntOrg, const TSize& szSize)
{
    m_i4X = pntOrg.i4X();
    m_i4Y = pntOrg.i4Y();
    m_c4CX = szSize.c4Width();
    m_c4CY = szSize.c4Height();
}


tCIDLib::TVoid TArea::SetOrg(const TPoint& pntNew)
{
    m_i4X = pntNew.i4X();
    m_i4Y = pntNew.i4Y();
}

tCIDLib::TVoid
TArea::SetOrg(const tCIDLib::TInt4 i4X, const tCIDLib::TInt4 i4Y)
{
    m_i4X = i4X;
    m_i4Y = i4Y;
}


tCIDLib::TVoid TArea::SetSize(const TSize& szNew)
{
    m_c4CX = szNew.c4Width();
    m_c4CY = szNew.c4Height();
}

tCIDLib::TVoid
TArea::SetSize(const tCIDLib::TCard4 c4CX, const tCIDLib::TCard4 c4CY)
{
    m_c4CX = c4CX;
    m_c4CY = c4CY;
}


//
//  Sub a point from our origin. It can be a point or the origin of an
//  area.
//
tCIDLib::TVoid TArea::SubFromOrg(const TArea& areaSub)
{
    if (areaSub.m_i4X <= m_i4X)
        m_i4X -= areaSub.m_i4X;
    if (areaSub.m_i4Y <= m_i4Y)
        m_i4Y -= areaSub.m_i4Y;
}

tCIDLib::TVoid TArea::SubFromOrg(const TPoint& pntSub)
{
    if (pntSub.m_i4X <= m_i4X)
        m_i4X -= pntSub.m_i4X;
    if (pntSub.m_i4Y <= m_i4Y)
        m_i4Y -= pntSub.m_i4Y;
}


//
//  Take the larger sizes of ours vs. a passed area or size.
//
tCIDLib::TVoid TArea::TakeLarger(const TArea& areaOther)
{
    if (areaOther.m_c4CX > m_c4CX)
        m_c4CX = areaOther.m_c4CX;
    if (areaOther.m_c4CY > m_c4CY)
        m_c4CY = areaOther.m_c4CY;
}

tCIDLib::TVoid TArea::TakeLarger(const TSize& szOther)
{
    if (szOther.c4X() > m_c4CX)
        m_c4CX = szOther.c4X();
    if (szOther.c4Y() > m_c4CY)
        m_c4CY = szOther.c4Y();
}


//
//  Take the smaller sizes of ours vs. a possed area or size.
//
tCIDLib::TVoid TArea::TakeSmaller(const TArea& areaOther)
{
    if (areaOther.m_c4CX < m_c4CX)
        m_c4CX = areaOther.m_c4CX;
    if (areaOther.m_c4CY < m_c4CY)
        m_c4CY = areaOther.m_c4CY;
}

tCIDLib::TVoid TArea::TakeSmaller(const TSize& szOther)
{
    if (szOther.c4X() < m_c4CX)
        m_c4CX = szOther.c4X();
    if (szOther.c4Y() < m_c4CY)
        m_c4CY = szOther.c4Y();
}


//
//  Convert the area to an array of host points, which can be passed to poly line
//  host APIs to draw a line around the border of the area.
//
tCIDLib::TVoid
TArea::ToPointArray(        tCIDLib::THostPoint* const  aptlTarget
                    , const tCIDLib::ERectlTypes        eInclusive) const
{
    tCIDLib::TInt4 i4R = i4Right();
    tCIDLib::TInt4 i4B = i4Bottom();

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (i4R > m_i4X)
            i4R--;
        if (i4B > m_i4Y)
            i4B--;
    }

    aptlTarget[0].i4X = m_i4X;
    aptlTarget[0].i4Y = i4B;
    aptlTarget[1].i4X = m_i4X;
    aptlTarget[1].i4Y = m_i4Y;
    aptlTarget[2].i4X = i4R;
    aptlTarget[2].i4Y = m_i4Y;
    aptlTarget[3].i4X = i4R;
    aptlTarget[3].i4Y = i4B;
    aptlTarget[4].i4X = m_i4X;
    aptlTarget[4].i4Y = i4B;
}


//
//  Give the corner points of this area as host points or our own point objects.
//
tCIDLib::TVoid
TArea::ToCornerPoints(          tCIDLib::THostPoint&    ptlULeft
                        ,       tCIDLib::THostPoint&    ptlURight
                        ,       tCIDLib::THostPoint&    ptlLRight
                        ,       tCIDLib::THostPoint&    ptlLLeft
                        , const tCIDLib::ERectlTypes    eInclusive) const
{
    tCIDLib::TInt4 i4R = i4Right();
    tCIDLib::TInt4 i4B = i4Bottom();

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (i4R > m_i4X)
            i4R--;
        if (i4B > m_i4Y)
            i4B--;
    }

    ptlULeft.i4X  = m_i4X;
    ptlULeft.i4Y  = m_i4Y;

    ptlURight.i4X = i4R;
    ptlURight.i4Y = m_i4Y;

    ptlLRight.i4X = i4R;
    ptlLRight.i4Y = i4B;

    ptlLLeft.i4X = m_i4X;
    ptlLLeft.i4Y = i4B;
}

tCIDLib::TVoid
TArea::ToCornerPoints(          TPoint&                 pntULeft
                        ,       TPoint&                 pntURight
                        ,       TPoint&                 pntLRight
                        ,       TPoint&                 pntLLeft
                        , const tCIDLib::ERectlTypes    eInclusive) const
{
    tCIDLib::TInt4 i4R = i4Right();
    tCIDLib::TInt4 i4B = i4Bottom();

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (i4R > m_i4X)
            i4R--;
        if (i4B > m_i4Y)
            i4B--;
    }

    pntULeft.m_i4X  = m_i4X;
    pntULeft.m_i4Y  = m_i4Y;

    pntURight.m_i4X = i4R;
    pntURight.m_i4Y = m_i4Y;

    pntLRight.m_i4X = i4R;
    pntLRight.m_i4Y = i4B;

    pntLLeft.m_i4X = m_i4X;
    pntLLeft.m_i4Y = i4B;
}


//
//  Save as above but just converst to the upper left and lower right
//  corners, from which the others can be inferred.
//
tCIDLib::TVoid
TArea::ToPoints(        TPoint&                 pntULeft
                ,       TPoint&                 pntLRight
                , const tCIDLib::ERectlTypes    eInclusive) const
{
    pntULeft.m_i4X = m_i4X;
    pntULeft.m_i4Y = m_i4Y;

    pntLRight.m_i4X = i4Right();
    pntLRight.m_i4Y = i4Bottom();

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (pntLRight.m_i4X > m_i4X)
            pntLRight.m_i4X--;
        if (pntLRight.m_i4Y > m_i4Y)
            pntLRight.m_i4Y--;
    }
}

tCIDLib::TVoid
TArea::ToPoints(        tCIDLib::THostPoint&    ptlULeft
                ,       tCIDLib::THostPoint&    ptlLRight
                , const tCIDLib::ERectlTypes    eInclusive) const
{
    ptlULeft.i4X = m_i4X;
    ptlULeft.i4Y = m_i4Y;

    ptlLRight.i4X = i4Right();
    ptlLRight.i4Y = i4Bottom();

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (ptlLRight.i4X > m_i4X)
            ptlLRight.i4X--;
        if (ptlLRight.i4Y > m_i4Y)
            ptlLRight.i4Y--;
    }
}


//
//  Fill in a raw (non-platform dependent) area structure from our contents.
//
tCIDLib::TVoid TArea::ToRawArea(tCIDLib::TRawArea& areaTarget) const
{
    areaTarget.i4X = m_i4X;
    areaTarget.i4Y = m_i4Y;
    areaTarget.c4CX = m_c4CX;
    areaTarget.c4CY = m_c4CY;
}


//
//  Convert our area to a host rectangle. They indicate whether the target rect should be
//  inclusive or non-inclusive. We initially create a non-inclusive rect by setting it to
//  our right/bottom values. If they want inclusive, we have to bump it down. But we can
//  only do that if we wouldn't invert, so they can lose info if they ask for inclusive.
//
//  We have a separate one that takes an offset, which is often convenient for callers to
//  be able to add an offset while doing the conversion.
//
tCIDLib::TVoid
TArea::ToRectl(         tCIDLib::THostRectl&    rectlDest
                , const tCIDLib::ERectlTypes    eInclusive) const
{
    // Create a inclusive rectangle
    rectlDest.i4Left  = m_i4X;
    rectlDest.i4Top   = m_i4Y;

    // These are initially non-inclusive
    rectlDest.i4Right = i4Right();
    rectlDest.i4Bottom = i4Bottom();

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (rectlDest.i4Right > rectlDest.i4Left)
            rectlDest.i4Right--;
        if (rectlDest.i4Bottom > rectlDest.i4Top)
            rectlDest.i4Bottom--;
    }
}

tCIDLib::TVoid
TArea::ToRectl(         tCIDLib::THostRectl&    rectlDest
                , const tCIDLib::ERectlTypes    eInclusive
                , const TPoint&                 pntOffset) const
{
    // Create a inclusive rectangle
    rectlDest.i4Left  = m_i4X + pntOffset.i4X();
    rectlDest.i4Top   = m_i4Y + pntOffset.i4Y();

    // These are initially non-inclusive
    rectlDest.i4Right  = i4Right() + pntOffset.i4X();
    rectlDest.i4Bottom = i4Bottom() + pntOffset.i4Y();

    if (eInclusive == tCIDLib::ERectlTypes::Inclusive)
    {
        if (rectlDest.i4Right > rectlDest.i4Left)
            rectlDest.i4Right--;
        if (rectlDest.i4Bottom > rectlDest.i4Top)
            rectlDest.i4Bottom--;
    }
}


//
//  Top justify us within the passed area, optionally also horizontally
//  centering us within it.
//
tCIDLib::TVoid TArea::TopJustifyIn( const   TArea&              areaIn
                                    , const tCIDLib::TBoolean   bHCenter)
{
    m_i4Y = areaIn.m_i4Y;
    if (bHCenter)
    {
        m_i4X = areaIn.m_i4X + (areaIn.m_c4CX >> 1);
        m_i4X -= (m_c4CX >> 1);
    }
}


//
//  Updates our size values with the passed sizes if ours are zero
//
tCIDLib::TVoid TArea::UpdateSizesIfZero(const   tCIDLib::TCard4 c4CX
                                        , const tCIDLib::TCard4 c4CY)
{
    if (!m_c4CX)
        m_c4CX = c4CX;
    if (!m_c4CY)
        m_c4CY = c4CY;
}


//
//  Zero out various components of this area.
//
tCIDLib::TVoid TArea::ZeroAll()
{
    m_i4X = 0;
    m_i4Y = 0;
    m_c4CX = 0;
    m_c4CY = 0;
}

tCIDLib::TVoid TArea::ZeroOrg()
{
    m_i4X = 0;
    m_i4Y = 0;
}

tCIDLib::TVoid TArea::ZeroSizes()
{
    m_c4CX = 0;
    m_c4CY = 0;
}


// ---------------------------------------------------------------------------
//  TArea: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TArea::FormatTo(TTextOutStream& strmDest) const
{
    // Uses same sep char as the default char of bParseFromText()
    strmDest << m_i4X
             << kCIDLib::chComma
             << m_i4Y
             << kCIDLib::chComma
             << m_c4CX
             << kCIDLib::chComma
             << m_c4CY;
}


tCIDLib::TVoid TArea::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_i4X
                    >> m_i4Y
                    >> m_c4CX
                    >> m_c4CY;
}

tCIDLib::TVoid TArea::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_i4X
                    << m_i4Y
                    << m_c4CX
                    << m_c4CY;
}


// ---------------------------------------------------------------------------
//  TArea: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method will adjust the sides of the area by the given x and y
//  offsets. Negative values will cause the area to become smaller and
//  positive values will cause it to become larger. Both sides are pushed
//  outward or inward by the offset amount.
//
tCIDLib::TVoid
TArea::DoAdjustments(   const   tCIDLib::TInt4  i4XOfs
                        , const tCIDLib::TInt4  i4YOfs)
{
    tCIDLib::TBoolean  bNegX, bNegY;

    // Get copies of the values that we can abuse
    tCIDLib::TInt4  i4XAdj = i4XOfs;
    tCIDLib::TInt4  i4YAdj = i4YOfs;

    //
    //  If the x offset is negative, then see if the area is large enough to
    //  adjust by this offset. If not, then set the offset to the best we can
    //  do, which is to make it a 0 sized area with a new origin that is at
    //  the middle of the old size. Do the same thing for the y offset also.
    //
    if (i4XAdj < 0)
    {
        bNegX = kCIDLib::True;
        i4XAdj *= -1;
    }
     else
    {
        bNegX = kCIDLib::False;
    }
    if (bNegX && (tCIDLib::TCard4(i4XAdj << 1) >= m_c4CX))
        i4XAdj = tCIDLib::TInt4(m_c4CX >> 1);

    if (i4YAdj < 0)
    {
        bNegY = kCIDLib::True;
        i4YAdj *= -1;
    }
     else
    {
        bNegY = kCIDLib::False;
    }
    if (bNegY && (tCIDLib::TCard4(i4YAdj << 1) >= m_c4CY))
        i4YAdj = tCIDLib::TInt4(m_c4CY >> 1);

    //
    //  Now adjust the sides. Adjust the orgin first, then adjust the length
    //  and width. When doing the length and width, be careful to check for
    //  a potential underflow of the m_c4CX and m_c4CY values. Despite the
    //  checks done above, the offset may have been an odd number, in which
    //  case there could still be an overflow of 1. The division done above
    //  was via a shift operation, which would just truncate the extra odd
    //  bit.
    //
    if (bNegX)
    {
        m_i4X += i4XAdj;
        if (m_c4CX <= tCIDLib::TCard4(i4XAdj * 2))
            m_c4CX = 0;
         else
            m_c4CX -= (i4XAdj * 2);
    }
     else
    {
        m_i4X  -= i4XAdj;
        m_c4CX += (i4XAdj * 2);
    }

    if (bNegY)
    {
        m_i4Y += i4YAdj;
        if (m_c4CY <= tCIDLib::TCard4(i4YAdj * 2))
            m_c4CY = 0;
         else
            m_c4CY -= (i4YAdj * 2);
    }
     else
    {
        m_i4Y  -= i4YAdj;
        m_c4CY += (i4YAdj * 2);
    }
}

