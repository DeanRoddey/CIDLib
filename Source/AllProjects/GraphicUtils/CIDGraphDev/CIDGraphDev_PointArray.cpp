//
// FILE NAME: CIDGraphDev_PointArray.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/09/1995
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
//  This file implements the TPointArray class. The reason for this class
//  is to provide very fast plotting of large numbers of lines. Having this
//  specialized array type means that we can internally store the points in
//  the form that the system accepts, as apposed to having a collection of
//  our TPoint object and having to translate them on each call.
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
#include    "CIDGraphDev_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TPointArray,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TPointArray
//  PREFIX: pnta
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPointArray: Constructors and Destructor
// ---------------------------------------------------------------------------

// Just set up a 1 pointe array
TPointArray::TPointArray() :

    m_c4Count(1)
    , m_paptPoints(new tCIDLib::THostPoint[1])
{
    m_paptPoints[0] = {0, 0};
}

TPointArray::TPointArray(const tCIDLib::TCard4 c4PointCount) :

    m_c4Count(c4PointCount)
    , m_paptPoints(nullptr)
{
    CheckCount(c4PointCount);

    // Allocate the point array and zero it
    m_paptPoints = new tCIDLib::THostPoint[m_c4Count];
    TRawMem::SetMemBuf
    (
        m_paptPoints
        , tCIDLib::TCard1(0)
        , m_c4Count * sizeof(tCIDLib::THostPoint)
    );
}

TPointArray::TPointArray(const TPointArray& pntaSrc) :

    m_c4Count(pntaSrc.m_c4Count)
    , m_paptPoints(nullptr)
{
    // Allocate the array
    m_paptPoints = new tCIDLib::THostPoint[m_c4Count];

    // And copy over the contents of the source array
    TRawMem::CopyMemBuf
    (
        m_paptPoints
        , pntaSrc.m_paptPoints
        , sizeof(tCIDLib::THostPoint) * m_c4Count
    );
}

TPointArray::TPointArray(TPointArray&& pntaSrc) :

    TPointArray()
{
    *this = operator=(tCIDLib::ForceMove(pntaSrc));
}

TPointArray::~TPointArray()
{
    delete [] m_paptPoints;
    m_paptPoints = nullptr;
}


// ---------------------------------------------------------------------------
//  TPointArray: Public operators
// ---------------------------------------------------------------------------
TPointArray& TPointArray::operator=(const TPointArray& pntaSrc)
{
    if (this != &pntaSrc)
    {
        // Delete the current contents
        delete [] m_paptPoints;
        m_paptPoints = nullptr;

        // And now duplicate the source
        m_c4Count = pntaSrc.m_c4Count;
        m_paptPoints = new tCIDLib::THostPoint[m_c4Count];

        // And copy over the contents of the source array
        TRawMem::CopyMemBuf
        (
            m_paptPoints
            , pntaSrc.m_paptPoints
            , sizeof(tCIDLib::THostPoint) * m_c4Count
        );
    }
    return *this;
}


TPointArray& TPointArray::operator=(TPointArray&& pntaSrc)
{
    if (this != &pntaSrc)
    {
        tCIDLib::Swap(m_c4Count, pntaSrc.m_c4Count);
        tCIDLib::Swap(m_paptPoints, pntaSrc.m_paptPoints);
    }
    return *this;
}


tCIDLib::TBoolean TPointArray::operator==(const TPointArray& pntaToTest) const
{
    if (&pntaToTest == this)
        return kCIDLib::True;

    // If not the same count, then clearly not equal
    if (pntaToTest.m_c4Count != m_c4Count)
        return kCIDLib::False;

    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4Count; c4Ind++)
    {
        if (m_paptPoints[c4Ind].i4X != pntaToTest.m_paptPoints[c4Ind].i4X)
            return kCIDLib::False;

        if (m_paptPoints[c4Ind].i4Y != pntaToTest.m_paptPoints[c4Ind].i4Y)
            return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TPointArray::operator!=(const TPointArray& pntaToTest) const
{
    return !operator==(pntaToTest);
}


const TPoint TPointArray::operator[](const tCIDLib::TCard4 c4Index)
{
    CheckIndex(CID_LINE, c4Index);
    return TPoint(m_paptPoints[c4Index]);
}


// ---------------------------------------------------------------------------
//  TPointArray: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Given a graph object, a vertical scale type, and min/max values, set up our
//  points so that we display the graph data, appropriately re-sampled for the
//  number of points we have vs. the number the graph has, and the number of
//  vertical pixels indicated. Any values outside of the min/max are clipped
//  to those values.
//
tCIDLib::TCard4
TPointArray::c4SetFromGraph(const   TGraphData&             grdatSrc
                            , const tCIDMath::EGraphScales  eScale
                            , const tCIDLib::TCard4         c4Height
                            , const tCIDLib::TFloat4        f4MinVal
                            , const tCIDLib::TFloat4        f4MaxVal)
{
    // Get some source graph related values together that we'll use below
    const tCIDLib::TCard4   c4SrcWidth = grdatSrc.c4MaxSamples();
    const tCIDLib::TFloat4  f4YRange(f4MaxVal - f4MinVal);
    const tCIDLib::TFloat4  f4SrcPerX(1.0F / tCIDLib::TFloat4(c4SrcWidth));
    const tCIDLib::TFloat4  f4RangeLog(TMathLib::f4Log10(f4YRange));

    // Do some checking in debug mode
    CIDAssert(f4YRange > 4, L"Vertical graph range is too small");

    //
    //  Allocate a list of Y percentages and go through the graph data and
    //  calculate the percentage of the Y range that each of the graph points
    //  represents.
    //
    //  We want the newest graph point (at the highest index) to end up in
    //  the last point. So we see how many samples we have, and figure out
    //  how many points (on the left of the graph) that we are missing. WE
    //  start at the missing sample count as the index of the first useful
    //  source sample when setting up our source percentages list. The values
    //  of the ones before that are meaningless, but we zero them just in
    //  case.
    //
    tCIDLib::TFloat4* pf4SrcPers = new tCIDLib::TFloat4[c4SrcWidth];
    TArrayJanitor<tCIDLib::TFloat4> janSrcPers(pf4SrcPers);

    const tCIDLib::TCard4 c4AvailSamples = grdatSrc.c4SamplesAvail();
    const tCIDLib::TCard4 c4MissingSamples(grdatSrc.c4MaxSamples() - c4AvailSamples);
    TRawMem::SetMemBuf
    (
        pf4SrcPers, tCIDLib::TCard1(0), sizeof(tCIDLib::TFloat4) * c4MissingSamples
    );
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4AvailSamples; c4Index++)
    {
        const tCIDLib::TCard4 c4ActInd = c4Index + c4MissingSamples;

        //
        //  Get the current sample out. Test it against the min and max and
        //  clip it to 0.0 or 1.0 if it exceeds those bounds.
        //
        //  If ok, then we want to convert it to a postive value relative to
        //  the range, so subtract the min value from it and then convert to
        //  a percentage relative to the range. If we are in a log scale, then
        //  take the log of the value first and use the log of the range as
        //  the divisor.
        //
        tCIDLib::TFloat4 f4Cur = grdatSrc.f4SampleAt(c4Index);

        if (f4Cur < f4MinVal)
        {
            pf4SrcPers[c4ActInd] = 0;
        }
         else if (f4Cur > f4MaxVal)
        {
            pf4SrcPers[c4ActInd] = 1;
        }
         else
        {
            f4Cur -= f4MinVal;

            if (eScale == tCIDMath::EGraphScales::Log10)
            {
                // Scale the value and divide by the scaled range
                f4Cur = TMathLib::f4Log10(f4Cur);
                pf4SrcPers[c4ActInd] = f4Cur / f4RangeLog;
            }
             else
            {
                // It's linear so just divide value by range
                pf4SrcPers[c4ActInd] = f4Cur / f4YRange;
            }
        }
    }

    //
    //  Now decide now many of our own points we will set up (max, though we
    //  may not use this many if the graph wasn't full), and the percent per
    //  such points. We limit ourselves to 3x the number of source graph
    //  points, since doing more is a waste of overhead. 3x is enough over
    //  sampling to give a good curve.
    //
    const tCIDLib::TCard4 c4MultFact = m_c4Count / c4SrcWidth;
    const tCIDLib::TCard4 c4TarWidth
    (
        c4MultFact > 3 ? m_c4Count / 3 : m_c4Count
    );
    const tCIDLib::TFloat4  f4TarPerX(1.0F / tCIDLib::TFloat4(c4TarWidth));

    //
    //  OK, now let's go back and do the interpolation of the source percentages
    //  into a list of target percentages.
    //
    //  On the off chance that both source and target have the same number of
    //  X values, check for that. If so, we can do a quick and easy pass Else
    //  do it the hard way.
    //
    tCIDLib::TInt4  i4FirstX = -1;
    tCIDLib::TCard4 c4OutCnt = 0;
    tCIDLib::TFloat4* pf4TarPers = new tCIDLib::TFloat4[m_c4Count];
    TArrayJanitor<tCIDLib::TFloat4> janTarPers(pf4TarPers);
    if (c4SrcWidth == m_c4Count)
    {
        //
        //  Just copy over the values that we have, i.e. we start at the
        //  missing samples count, which is the index of the first good
        //  source value and copy to the output list.
        //
        for (tCIDLib::TCard4 c4Index = c4MissingSamples; c4Index < c4SrcWidth; c4Index++)
            pf4TarPers[c4OutCnt++] = pf4SrcPers[c4Index];
    }
     else
    {
        //
        //  Start a target percentage which we will move across the output, by
        //  the percentage per our output point.
        //
        tCIDLib::TFloat4 f4TarPer = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TarWidth; c4Index++)
        {
            if (f4TarPer >= 1.0)
            {
                //
                //  There's no need to interpolate. This has to be some rounding
                //  slop which has allowed us to get to or beyond the 100%. So in
                //  this case, just take the final source percentage, since we
                //  know both the target and source are in the same place.
                //
                pf4TarPers[c4OutCnt++] = pf4SrcPers[c4SrcWidth - 1];
            }
             else
            {
                //
                //  Calculate the low point. We take the percent we are currently
                //  through the target range and divide by the percent per X point
                //  of the source, and round down to an unsigned. The target point
                //  has to be between this source point and the next one up.
                //
                //  If the source point is not at or beyond the missing samples
                //  count, then we don't store it.
                //
                tCIDLib::TCard4 c4SrcPnt = tCIDLib::TCard4(f4TarPer / f4SrcPerX);
                if (c4SrcPnt >= c4MissingSamples)
                {
                    //
                    //  If this is the first hit we got, then remember the x pixel
                    //  for this guy.
                    //
                    if (i4FirstX == -1)
                    {
                        i4FirstX = tCIDLib::TInt4(c4Index);
                        if (c4MultFact > 3)
                            i4FirstX *= 3;
                    }

                    //
                    //  If at or beyond the last point, then just take the last
                    //  source percent. Else, we need to do the interpolation.
                    //
                    if (c4SrcPnt + 1 >= c4SrcWidth)
                    {
                        pf4TarPers[c4OutCnt++] = pf4SrcPers[c4SrcWidth - 1];
                    }
                     else
                    {
                        //
                        //  Calculate the mu. It's the percentage we are between the
                        //  low src point and the next point. So we take the
                        //  current percent we are at, and subtract the percent
                        //  of the low source point, and divide by the amount
                        //  between each source point.
                        //
                        tCIDLib::TFloat4 f4SrcMu
                        (
                            (f4TarPer - (c4SrcPnt * f4SrcPerX)) / f4SrcPerX
                        );

                        // And now we can do the interpolation of the target percent
                        pf4TarPers[c4OutCnt++] =
                            (pf4SrcPers[c4SrcPnt] * (1.0F - f4SrcMu))
                            + (pf4SrcPers[c4SrcPnt + 1] * f4SrcMu);
                    }
                }
            }

            // Move forward to the next target percentage
            f4TarPer += f4TarPerX;
        }
    }

    //
    //  OK, now we can go through and set our actual points. Note that we are
    //  flipping them around here to go from our 1st quadrant values to the
    //  4th quadrant display layout.
    //
    //  We also have to deal with the fact that our points don't necessarily
    //  correspond to pixels, because of the fact that limit ourselves to no
    //  more than 3x the number of sample points.
    //
    if (i4FirstX == -1)
        i4FirstX = 0;

    const tCIDLib::TInt4 i4Height = tCIDLib::TInt4(c4Height);
    tCIDLib::TInt4* pi4Cur = &m_paptPoints[0].i4X;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4OutCnt; c4Index++)
    {
        // Start the X with the current index
        tCIDLib::TInt4 i4X = tCIDLib::TInt4(c4Index);

        // If we adjusted to limit the number of output points, then handle that
        if (c4MultFact > 3)
            i4X *= 3;

        // And adjust over by the first X pixel we had a sample for.
        *pi4Cur++ = i4X + i4FirstX;

        //
        //  Scale the height by the target percentage for this output point,
        //  flipped quadarants, and clipping it back if needed.
        //
        tCIDLib::TInt4 i4Y = tCIDLib::TInt4(c4Height - (pf4TarPers[c4Index] * c4Height));
        if (i4Y > i4Height)
            i4Y = i4Height;
        else if (i4Y < 0)
            i4Y = 0;
        *pi4Cur++ = i4Y;
    }

    return c4OutCnt;
}


// Copy a point internally from one slot to another
tCIDLib::TVoid
TPointArray::CopyPoint( const   tCIDLib::TCard4 c4FromIndex
                        , const tCIDLib::TCard4 c4ToIndex)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4FromIndex);
    CheckIndex(CID_LINE, c4ToIndex);
    #endif

    if (c4FromIndex == c4ToIndex)
        return;

    m_paptPoints[c4ToIndex] = m_paptPoints[c4FromIndex];
}


tCIDLib::TVoid
TPointArray::CopyPointAndOffset(const   tCIDLib::TCard4 c4FromIndex
                                , const tCIDLib::TCard4 c4ToIndex
                                , const tCIDLib::TInt4  i4XOfs
                                , const tCIDLib::TInt4  i4YOfs)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4FromIndex);
    CheckIndex(CID_LINE, c4ToIndex);
    #endif

    if (c4FromIndex == c4ToIndex)
        return;

    m_paptPoints[c4ToIndex].i4X = m_paptPoints[c4FromIndex].i4X + i4XOfs;
    m_paptPoints[c4ToIndex].i4Y = m_paptPoints[c4FromIndex].i4Y + i4YOfs;
}

tCIDLib::TVoid
TPointArray::CopyPointAndOffset(const   tCIDLib::TCard4 c4FromIndex
                                , const tCIDLib::TCard4 c4ToIndex
                                , const TPoint&         pntOfs)
{
    CopyPointAndOffset(c4FromIndex, c4ToIndex, pntOfs.i4X(), pntOfs.i4Y());
}


tCIDLib::TVoid
TPointArray::CopyRange( const   tCIDLib::TCard4 c4FromIndex
                        , const tCIDLib::TCard4 c4ToIndex
                        , const tCIDLib::TCard4 c4Count)
{
    //
    //  Check the index if in debug mode. Just check the indexes plus the
    //  count. If that does not overflow, then the original index could not
    //  have been bad.
    //
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4FromIndex + c4Count);
    CheckIndex(CID_LINE, c4ToIndex + c4Count);
    #endif

    if (c4FromIndex == c4ToIndex)
        return;

    //
    //  Create the two pointer and then call MoveMemory() which understands
    //  the issues of overlapping ranges.
    //
    TRawMem::MoveMemBuf
    (
        &m_paptPoints[c4FromIndex]
        , &m_paptPoints[c4ToIndex]
        , c4Count * sizeof(tCIDLib::THostPoint)
    );
}


tCIDLib::TVoid
TPointArray::GetPoint(  const   tCIDLib::TCard4 c4Index
                        ,       tCIDLib::TInt4& i4XToFill
                        ,       tCIDLib::TInt4& i4YToFill) const
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4Index);
    #endif

    i4XToFill = m_paptPoints[c4Index].i4X;
    i4YToFill = m_paptPoints[c4Index].i4Y;
}

tCIDLib::TVoid
TPointArray::GetPoint(const tCIDLib::TCard4 c4Index, TPoint& pntToFill) const
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4Index);
    #endif

    pntToFill.Set(m_paptPoints[c4Index].i4X, m_paptPoints[c4Index].i4Y);
}


tCIDLib::TVoid
TPointArray::OffsetAll( const   tCIDLib::TInt4  i4XOfs
                        , const tCIDLib::TInt4  i4YOfs)
{
    tCIDLib::TInt4*  pi4Cur = &m_paptPoints[0].i4X;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Count; c4Index++)
    {
        *pi4Cur++ += i4XOfs;
        *pi4Cur++ += i4YOfs;
    }
}

tCIDLib::TVoid TPointArray::OffsetAll(const TPoint& pntOfs)
{
    // Just call the other version
    OffsetAll(pntOfs.i4X(), pntOfs.i4Y());
}


tCIDLib::TVoid
TPointArray::OffsetPoint(   const   tCIDLib::TCard4 c4Index
                            , const tCIDLib::TInt4  i4XOfs
                            , const tCIDLib::TInt4  i4YOfs)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4Index);
    #endif

    m_paptPoints[c4Index].i4X += i4XOfs;
    m_paptPoints[c4Index].i4Y += i4YOfs;
}

tCIDLib::TVoid
TPointArray::OffsetPoint(   const   tCIDLib::TCard4 c4Index
                            , const TPoint&         pntOfs)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4Index);
    #endif

    m_paptPoints[c4Index].i4X += pntOfs.i4X();
    m_paptPoints[c4Index].i4Y += pntOfs.i4Y();
}


tCIDLib::TVoid
TPointArray::OffsetRange(   const   tCIDLib::TCard4 c4FromIndex
                            , const tCIDLib::TCard4 c4ToIndex
                            , const tCIDLib::TInt4  i4XOfs
                            , const tCIDLib::TInt4  i4YOfs)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4FromIndex);
    CheckIndex(CID_LINE, c4ToIndex);
    #endif

    if (c4FromIndex == c4ToIndex)
        return;

    // Get modifiable copies of the indexes
    tCIDLib::TCard4 c4ActualFrom = c4FromIndex;
    tCIDLib::TCard4 c4ActualTo = c4ToIndex;

    // If they are reversed, then flip them
    if (c4ActualFrom > c4ActualTo)
    {
        tCIDLib::TCard4  c4Tmp = c4ActualFrom;
        c4ActualFrom = c4ActualTo;
        c4ActualTo = c4Tmp;
    }

    //
    //  Address the correct duple and loop through the rest of the indicated
    //  indexes.
    //
    tCIDLib::TInt4* pi4Cur = &m_paptPoints[c4ActualFrom].i4X;
    for (tCIDLib::TCard4 c4Index = c4ActualFrom; c4Index <= c4ActualTo; c4Index++)
    {
        *pi4Cur++ += i4XOfs;
        *pi4Cur++ += i4YOfs;
    }
}


tCIDLib::TVoid TPointArray::Reallocate(const tCIDLib::TCard4 c4NewCount)
{
    CheckCount(c4NewCount);

    //
    //  If the new is not equal to the old then reallocate
    if (c4NewCount != m_c4Count)
    {
        // Make sure we can allocate and init a new array
        TArrayJanitor<tCIDLib::THostPoint> janNew(c4NewCount);
        TRawMem::SetMemBuf
        (
            janNew.paThis(), tCIDLib::TCard1(0), m_c4Count * sizeof(tCIDLib::THostPoint)
        );

        // Looks good so clean up the old and orphan out the new
        delete [] m_paptPoints;
        m_c4Count = c4NewCount;
        m_paptPoints = janNew.paOrphan();
    }
}


// Set all or some points to specific values
tCIDLib::TVoid
TPointArray::SetAll(const tCIDLib::TInt4 i4XNew, const tCIDLib::TInt4 i4YNew)
{
    //
    //  Address the 0th element, then loop through all of the duples,
    //  updating them.
    //
    tCIDLib::TInt4* pi4Cur = &m_paptPoints[0].i4X;
    const tCIDLib::TInt4* pi4End = pi4Cur + (m_c4Count * 2);
    while (pi4Cur < pi4End)
    {
        *pi4Cur++ = i4XNew;
        *pi4Cur++ = i4YNew;
    }
}

tCIDLib::TVoid TPointArray::SetAll(const TPoint& pntNew)
{
    // Just call the other version
    SetAll(pntNew.i4X(), pntNew.i4Y());
}


//
//  Set our points from the source array's points, but offset. If the two
//  are diffferent sizes, then this one is reallocated to match.
//
tCIDLib::TVoid
TPointArray::SetFromOffset(const TPointArray& pntaSrc, const TPoint& pntOffset)
{
    const tCIDLib::TCard4 c4Count = pntaSrc.m_c4Count;
    if (c4Count != m_c4Count)
        Reallocate(c4Count);

    const tCIDLib::TInt4 i4XOfs = pntOffset.i4X();
    const tCIDLib::TInt4 i4YOfs = pntOffset.i4Y();

    const tCIDLib::TInt4* pi4Src = &pntaSrc.m_paptPoints[0].i4X;
    tCIDLib::TInt4* pi4Tar = &m_paptPoints[0].i4X;
    const tCIDLib::TInt4* pi4End = pi4Src + (m_c4Count * 2);
    while (pi4Src < pi4End)
    {
        *pi4Tar++ = *pi4Src++ + i4XOfs;
        *pi4Tar++ = *pi4Src++ + i4YOfs;
    }
}



tCIDLib::TVoid
TPointArray::SetPoint(  const   tCIDLib::TCard4 c4Index
                        , const tCIDLib::TInt4  i4XNew
                        , const tCIDLib::TInt4  i4YNew)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4Index);
    #endif

    m_paptPoints[c4Index].i4X = i4XNew;
    m_paptPoints[c4Index].i4Y = i4YNew;
}

tCIDLib::TVoid
TPointArray::SetPoint(  const   tCIDLib::TCard4 c4Index
                        , const TPoint&         pntNew)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4Index);
    #endif

    m_paptPoints[c4Index].i4X = pntNew.i4X();
    m_paptPoints[c4Index].i4Y = pntNew.i4Y();
}


tCIDLib::TVoid
TPointArray::SetRange(  const   tCIDLib::TCard4 c4FromIndex
                        , const tCIDLib::TCard4 c4ToIndex
                        , const tCIDLib::TInt4  i4XNew
                        , const tCIDLib::TInt4  i4YNew)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4FromIndex);
    CheckIndex(CID_LINE, c4ToIndex);
    #endif

    if (c4FromIndex == c4ToIndex)
        return;

    // Get modifiable copies of the indexes
    tCIDLib::TCard4 c4ActualFrom  = c4FromIndex;
    tCIDLib::TCard4 c4ActualTo    = c4ToIndex;

    // If they are reversed, then flip them
    if (c4ActualFrom > c4ActualTo)
    {
        tCIDLib::TCard4  c4Tmp = c4ActualFrom;
        c4ActualFrom = c4ActualTo;
        c4ActualTo = c4Tmp;
    }

    //
    //  Address the correct duple and loop through the rest of the indicated
    //  indexes.
    //
    tCIDLib::TInt4* pi4Cur = &m_paptPoints[c4ActualFrom].i4X;
    for (tCIDLib::TCard4 c4Index = c4ActualFrom; c4Index <= c4ActualTo; c4Index++)
    {
        *pi4Cur++ = i4XNew;
        *pi4Cur++ = i4YNew;
    }
}

tCIDLib::TVoid
TPointArray::SetRange(  const   tCIDLib::TCard4 c4FromIndex
                        , const tCIDLib::TCard4 c4ToIndex
                        , const TPoint&         pntNew)
{
    SetRange(c4FromIndex, c4ToIndex, pntNew.i4X(), pntNew.i4Y());
}


tCIDLib::TVoid TPointArray::ZeroAll()
{
    TRawMem::SetMemBuf
    (
        m_paptPoints
        , tCIDLib::TCard1(0)
        , m_c4Count * sizeof(tCIDLib::THostPoint)
    );
}

tCIDLib::TVoid TPointArray::ZeroPoint(const tCIDLib::TCard4 c4Index)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4Index);
    #endif

    m_paptPoints[c4Index].i4X = 0;
    m_paptPoints[c4Index].i4Y = 0;
}

tCIDLib::TVoid
TPointArray::ZeroRange( const   tCIDLib::TCard4 c4FromIndex
                        , const tCIDLib::TCard4 c4ToIndex)
{
    // Check the index if in debug mode
    #if CID_DEBUG_ON
    CheckIndex(CID_LINE, c4FromIndex);
    CheckIndex(CID_LINE, c4ToIndex);
    #endif

    if (c4FromIndex == c4ToIndex)
        return;

    // Get modifiable copies of the indexes
    tCIDLib::TCard4 c4ActualFrom  = c4FromIndex;
    tCIDLib::TCard4 c4ActualTo    = c4ToIndex;

    // If they are reversed, then flip them
    if (c4ActualFrom > c4ActualTo)
    {
        tCIDLib::TCard4  c4Tmp = c4ActualFrom;
        c4ActualFrom = c4ActualTo;
        c4ActualTo = c4Tmp;
    }

    // Zero out the points in the range.
    TRawMem::SetMemBuf
    (
        &m_paptPoints[c4ActualFrom]
        , tCIDLib::TCard1(0)
        , ((c4ActualTo - c4ActualFrom) + 1) * sizeof(tCIDLib::THostPoint)
    );
}


// ---------------------------------------------------------------------------
//  TPointArray:: Protected, non-virtual methods
// ---------------------------------------------------------------------------
const tCIDLib::THostPoint* TPointArray::aptList() const
{
     return m_paptPoints;
}


// ---------------------------------------------------------------------------
//  TPointArray: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPointArray::CheckCount(const tCIDLib::TCard4 c4Check) const
{
    if (c4Check < 2)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Check)
            , TString(L"point count")
        );
    }
}

//
//  This method will check the passed index and log an error if its invalid.
//
tCIDLib::TVoid
TPointArray::CheckIndex(const   tCIDLib::TCard4     c4Line
                        , const tCIDLib::TCard4     c4Index) const
{
    if (c4Index >= m_c4Count)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , clsThis()
            , TCardinal(m_c4Count)
        );
    }
}

