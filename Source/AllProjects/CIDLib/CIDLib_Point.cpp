//
// FILE NAME: CIDLib_Point.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/06/1993
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
//  This file implements the TPoint class, which is a 2D point with signed,
//  integral coordinates.
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
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TPoint,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TPoint
//  PREFIX: pnt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPoint: Public, static data
// ---------------------------------------------------------------------------
const TPoint TPoint::pntOrigin(0, 0);


// ---------------------------------------------------------------------------
//  TPoint: Public, static methods
// ---------------------------------------------------------------------------
TPoint& TPoint::Nul_TPoint()
{
    static TPoint pntNull;
    return pntNull;
}


// ---------------------------------------------------------------------------
//  TPoint: Constructors and Destructor
// ---------------------------------------------------------------------------
TPoint::TPoint(const tCIDLib::THostPoint& ptSrc) :

    m_i4X(ptSrc.i4X)
    , m_i4Y(ptSrc.i4Y)
{
}

TPoint::TPoint(const tCIDLib::TRawPoint& ptSrc) :

    m_i4X(ptSrc.i4X)
    , m_i4Y(ptSrc.i4Y)
{
}




// ---------------------------------------------------------------------------
//  TPoint: Public operators
// ---------------------------------------------------------------------------
TPoint::operator const tCIDLib::THostPoint*() const
{
    return (const tCIDLib::THostPoint*)&m_i4X;
}

TPoint::operator tCIDLib::THostPoint*()
{
    return (tCIDLib::THostPoint*)&m_i4X;
}

TPoint& TPoint::operator=(const tCIDLib::THostPoint& ptSrc)
{
    m_i4X = ptSrc.i4X;
    m_i4Y = ptSrc.i4Y;
    return *this;
}

TPoint& TPoint::operator=(const tCIDLib::TRawPoint& ptSrc)
{
    m_i4X = ptSrc.i4X;
    m_i4Y = ptSrc.i4Y;
    return *this;
}


tCIDLib::TVoid TPoint::operator+=(const TPoint& pntToAdd)
{
    m_i4X += pntToAdd.m_i4X;
    m_i4Y += pntToAdd.m_i4Y;
}

tCIDLib::TVoid TPoint::operator-=(const TPoint& pntToSub)
{
    m_i4X -= pntToSub.m_i4X;
    m_i4Y -= pntToSub.m_i4Y;
}

TPoint operator+(const TPoint& pnt1, const TPoint& pnt2)
{
    return TPoint(pnt1.m_i4X + pnt2.m_i4X, pnt1.m_i4Y + pnt2.m_i4Y);
}

TPoint operator-(const TPoint& pnt1, const TPoint& pnt2)
{
    return TPoint(pnt1.m_i4X - pnt2.m_i4X, pnt1.m_i4Y - pnt2.m_i4Y);
}



// ---------------------------------------------------------------------------
//  TPoint: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Sets the pntDiff parm to the absolute difference between this point and
//  the other point, i.e. the difference with the values always positive.
//
tCIDLib::TVoid
TPoint::AbsDiffFrom(const TPoint& pntOther, TPoint& pntDiff) const
{
    if (m_i4X > pntOther.m_i4X)
        pntDiff.m_i4X = m_i4X - pntOther.m_i4X;
    else
        pntDiff.m_i4X = pntOther.m_i4X - m_i4X;

    if (m_i4Y > pntOther.m_i4Y)
        pntDiff.m_i4Y = m_i4Y - pntOther.m_i4Y;
    else
        pntDiff.m_i4Y = pntOther.m_i4Y - m_i4Y;
}


// Just adjusts this point by a signed amount in both axes
tCIDLib::TVoid
TPoint::Adjust(const tCIDLib::TInt4 i4XOfs, const tCIDLib::TInt4 i4YOfs)
{
    m_i4X += i4XOfs;
    m_i4Y += i4YOfs;
}


// And these adjust one just axis
tCIDLib::TVoid TPoint::AdjustX(const tCIDLib::TInt4 i4Ofs)
{
    m_i4X += i4Ofs;
}

tCIDLib::TVoid TPoint::AdjustY(const tCIDLib::TInt4 i4Ofs)
{
    m_i4Y += i4Ofs;
}


// Indicates whether this point is within the passed area, border included
tCIDLib::TBoolean TPoint::bInArea(const TArea& areaTarget) const
{
    if ((m_i4X < areaTarget.m_i4X)
    ||  (m_i4Y < areaTarget.m_i4Y)
    ||  (m_i4X > areaTarget.i4Right())
    ||  (m_i4Y > areaTarget.i4Bottom()))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Parses a point of out a string. It expects the format to be x,y but
//  you can indicate another separate character as a parameter, as long
//  as the values are in the right order.
//
tCIDLib::TBoolean
TPoint::bParseFromText( const   TString&            strText
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

    // Looks ok, so store the values
    m_i4X = i4X;
    m_i4Y = i4Y;

    return kCIDLib::True;
}


//
//  Returns the sum of the absolute differnces between this point and
//  the other point.
//
tCIDLib::TCard4 TPoint::c4AbsDiff(const TPoint& pntOther) const
{
    tCIDLib::TInt4 i4XDiff;
    tCIDLib::TInt4 i4YDiff;

    if (m_i4X > pntOther.m_i4X)
        i4XDiff = m_i4X - pntOther.m_i4X;
    else
        i4XDiff = pntOther.m_i4X - m_i4X;

    if (m_i4Y > pntOther.m_i4Y)
        i4YDiff = m_i4Y - pntOther.m_i4Y;
    else
        i4YDiff = pntOther.m_i4Y - m_i4Y;

    // Return the sum of the two differences
    return i4XDiff + i4YDiff;
}


//
//  Returns this value as two 16 bit values, packed into a 32 bit value,
//  with the X coordinate in the high word and the Y coordinate in the low
//  word.
//
tCIDLib::TCard4 TPoint::c4Packed() const
{
    if ((m_i4X > kCIDLib::i2MaxInt) || (m_i4Y > kCIDLib::i2MaxInt))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPoint_CantPack
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return TRawBits::c4From16(tCIDLib::TCard2(m_i4X), tCIDLib::TCard2(m_i4Y));
}


// Format to text in a controlled fashin
tCIDLib::TVoid
TPoint::FormatToText(       TString&            strToFill
                    , const tCIDLib::ERadices   eRadix
                    , const tCIDLib::TCh        chSepChar
                    , const tCIDLib::TBoolean   bAppend) const
{
    tCIDLib::ERadices eRealRad = eRadix;
    if (eRealRad == tCIDLib::ERadices::Auto)
        eRealRad = tCIDLib::ERadices::Dec;

    // If not appending, start with a cleared string
    if (!bAppend)
        strToFill.Clear();

    strToFill.AppendFormatted(m_i4X, eRealRad);
    strToFill.Append(chSepChar);
    strToFill.AppendFormatted(m_i4Y, eRealRad);
}


//
//  Forces this point within the passed area, if not already within it, by
//  doing the minimal movement required.
//
tCIDLib::TVoid TPoint::ForceWithin(const TArea& areaLimits)
{
    if (m_i4X < areaLimits.m_i4X)
        m_i4X = areaLimits.m_i4X;
    else if (m_i4X > areaLimits.i4Right())
        m_i4X = areaLimits.i4Right();

    if (m_i4Y < areaLimits.i4Y())
        m_i4Y = areaLimits.i4Y();
    else if (m_i4Y > areaLimits.i4Bottom())
        m_i4Y = areaLimits.i4Bottom();
}


//
//  Converts from the passed polar coordinates (one for degrees, and one for
//  radians) to a 2D cartesian point.
//
tCIDLib::TVoid TPoint::FromPolarDegrees(const   tCIDLib::TCard4 c4Theta
                                        , const tCIDLib::TCard4 c4Radius)
{
    // Optimize for 0 theta
    if (!c4Theta)
    {
        m_i4X = c4Radius;
        m_i4Y = 0;
        return;
    }

    // Convert the reduced degree angle to radians
    tCIDLib::TFloat8 f8Angle = (c4Theta > 360) ? (c4Theta % 360) : c4Theta;
    f8Angle *= (kCIDLib::f8PI / 180.0);

    // And calculate our new positions
    m_i4X = tCIDLib::TInt4(c4Radius * TMathLib::f8Cosine(f8Angle));
    m_i4Y = tCIDLib::TInt4(c4Radius * TMathLib::f8Sine(f8Angle));
}


tCIDLib::TVoid TPoint::FromPolarRadians(const   tCIDLib::TFloat8&   f8Theta
                                        , const tCIDLib::TCard4     c4Radius)
{
    // Optimize for 0 theta
    if (!f8Theta)
    {
        m_i4X = c4Radius;
        m_i4Y = 0;
        return;
    }

    m_i4X = tCIDLib::TInt4(c4Radius * TMathLib::f8Cosine(f8Theta));
    m_i4Y = tCIDLib::TInt4(c4Radius * TMathLib::f8Sine(f8Theta));
}


// Return the larger of the dimensions
tCIDLib::TInt4 TPoint::i4LargestDim() const
{
    if (m_i4X > m_i4Y)
        return m_i4X;
    return m_i4Y;
}


//
//  Updates the passed host version of a point to have the coordinates of
//  this point.
//
tCIDLib::TVoid TPoint::ToHostPt(tCIDLib::THostPoint& ptTarget) const
{
    ptTarget.i4X = m_i4X;
    ptTarget.i4Y = m_i4Y;
}


//
//  Updates the passed raw version of a point to have the coordinates of
//  this point.
//
tCIDLib::TVoid TPoint::ToRawPt(tCIDLib::TRawPoint& ptTarget) const
{
    ptTarget.i4X = m_i4X;
    ptTarget.i4Y = m_i4Y;
}


//
//  Scales this point by the passed horizontal and vertical scaling factors.
//  Sign is preserved, so it scales up into the same quadrant.
//
tCIDLib::TVoid
TPoint::Scale(const tCIDLib::TFloat8 f8XScale, const tCIDLib::TFloat8 f8YScale)
{
    // Insure the scaling values are legal
    if ((f8XScale < 0.0) || (f8YScale < 0.0))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPoint_BadScaling
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f8XScale)
            , TFloat(f8YScale)
        );
    }

    tCIDLib::TFloat8 f8Tmp = m_i4X * f8XScale;
    TMathLib::Round(f8Tmp);
    if ((f8Tmp < kCIDLib::i4MinInt) || (f8Tmp > kCIDLib::i4MaxInt))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPoint_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TFloat(f8Tmp, 0)
        );
    }
    m_i4X = tCIDLib::TInt4(f8Tmp);

    f8Tmp = m_i4Y * f8YScale;
    TMathLib::Round(f8Tmp);
    if ((f8Tmp < kCIDLib::i4MinInt) || (f8Tmp > kCIDLib::i4MaxInt))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPoint_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TFloat(f8Tmp, 0)
        );
    }
    m_i4Y = tCIDLib::TInt4(f8Tmp);
}


tCIDLib::TVoid TPoint::TakeLarger(const TPoint& pntOther)
{
    if (pntOther.m_i4X > m_i4X)
        m_i4X = pntOther.m_i4X;

    if (pntOther.m_i4Y > m_i4Y)
        m_i4Y = pntOther.m_i4Y;
}


tCIDLib::TVoid TPoint::TakeSmaller(const TPoint& pntOther)
{
    if (pntOther.m_i4X < m_i4X)
        m_i4X = pntOther.m_i4X;

    if (pntOther.m_i4Y < m_i4Y)
        m_i4Y = pntOther.m_i4Y;
}


//
//  Converts this 2D cartesian point to teh equiv polar coordinates, one for
//  degrees and one for radians.
//
tCIDLib::TVoid TPoint::ToPolarDegrees(  tCIDLib::TCard4&    c4Theta
                                        , tCIDLib::TCard4&  c4Radius) const
{
    // Use Pythagoras to calc the radius of the point
    c4Radius = tCIDLib::TCard4
    (
        TMathLib::f8SqrRoot((m_i4X * m_i4X) + (m_i4Y * m_i4Y))
    );

    // Calculate theta in radians first
    const tCIDLib::TFloat8 f8Theta = TMathLib::f8ArcTangentXY(m_i4X, m_i4Y);
    const tCIDLib::TInt4 i4Tmp = tCIDLib::TInt4
    (
        f8Theta * (180.0 / kCIDLib::f8PI)
    );

    if (i4Tmp < 0)
        c4Theta = tCIDLib::TCard4(360 + i4Tmp);
    else
        c4Theta = tCIDLib::TCard4(i4Tmp);
}


tCIDLib::TVoid TPoint::ToPolarRadians(  tCIDLib::TFloat8&   f8Theta
                                        , tCIDLib::TCard4&  c4Radius) const
{
    // Use Pythagoras to calc the radius of the point
    c4Radius = tCIDLib::TCard4
    (
        TMathLib::f8SqrRoot((m_i4X * m_i4X) + (m_i4Y * m_i4Y))
    );

    f8Theta = TMathLib::f8ArcTangentXY(m_i4X, m_i4Y);
}


// ---------------------------------------------------------------------------
//  TPoint: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPoint::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_i4X << L"," << m_i4Y;
}


tCIDLib::TVoid TPoint::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_i4X
                    >> m_i4Y;
}

tCIDLib::TVoid TPoint::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_i4X
                    << m_i4Y;
}

