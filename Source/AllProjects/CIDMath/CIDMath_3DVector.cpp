//
// FILE NAME: CIDMath_3DVector.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
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
//  This file implements the T3DVector class.
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
#include    "CIDMath_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(T3DVector,TObject)


// ---------------------------------------------------------------------------
//  Our global binary operators and friends
// ---------------------------------------------------------------------------
T3DVector operator+(const T3DVector& vec1, const T3DVector& vec2)
{
    return T3DVector
    (
        vec1.m_f8XMag + vec2.m_f8XMag
        , vec1.m_f8YMag + vec2.m_f8YMag
        , vec1.m_f8ZMag + vec2.m_f8ZMag
    );
}

T3DVector operator+(const T3DVector& vec1, const tCIDLib::TFloat8& f8Inc)
{
    return T3DVector
    (
        vec1.m_f8XMag + f8Inc
        , vec1.m_f8YMag + f8Inc
        , vec1.m_f8ZMag + f8Inc
    );
}
T3DVector operator-(const T3DVector& vec1, const T3DVector& vec2)
{
    return T3DVector
    (
        vec1.m_f8XMag - vec2.m_f8XMag
        , vec1.m_f8YMag - vec2.m_f8YMag
        , vec1.m_f8ZMag - vec2.m_f8ZMag
    );
}

T3DVector operator-(const T3DVector& vec1, const tCIDLib::TFloat8& f8Div)
{
    return T3DVector
    (
        vec1.m_f8XMag - f8Div
        , vec1.m_f8YMag - f8Div
        , vec1.m_f8ZMag - f8Div
    );
}

T3DVector operator*(const T3DVector& vec1, const tCIDLib::TFloat8& f8Scale)
{
    return T3DVector
    (
        vec1.m_f8XMag * f8Scale
        , vec1.m_f8YMag * f8Scale
        , vec1.m_f8ZMag * f8Scale
    );
}

T3DVector operator*(const T3DVector& vec1, const T3DVector& vec2)
{
    return T3DVector
    (
        vec1.m_f8XMag * vec2.m_f8XMag
        , vec1.m_f8YMag * vec2.m_f8YMag
        , vec1.m_f8ZMag * vec2.m_f8ZMag
    );
}

T3DVector operator/(const T3DVector& vec1, const T3DVector& vec2)
{
    return T3DVector(vec1.m_f8XMag / vec2.m_f8XMag
                    , vec1.m_f8YMag / vec2.m_f8YMag
                    , vec1.m_f8ZMag / vec2.m_f8ZMag);
}

T3DVector operator/(const   T3DVector&          vec1
                    , const tCIDLib::TFloat8&   f8Div)
{
    return T3DVector
    (
        vec1.m_f8XMag / f8Div
        , vec1.m_f8YMag / f8Div
        , vec1.m_f8ZMag / f8Div
    );
}

T3DVector vecCross(const T3DVector& vec1, const T3DVector& vec2)
{
    return T3DVector
    (
        (vec1.m_f8YMag * vec2.m_f8ZMag) - (vec1.m_f8ZMag * vec2.m_f8YMag)
        , (vec1.m_f8ZMag * vec2.m_f8XMag) - (vec1.m_f8XMag * vec2.m_f8ZMag)
        , (vec1.m_f8XMag * vec2.m_f8YMag) - (vec1.m_f8YMag * vec2.m_f8XMag)
    );
}


T3DVector vecHalfBetween(const T3DVector& vec1, const T3DVector& vec2)
{
    return T3DVector
    (
        0.5 * (vec1.m_f8XMag + vec2.m_f8XMag)
        , 0.5  * (vec1.m_f8YMag + vec2.m_f8YMag)
        , 0.5  * (vec1.m_f8ZMag + vec2.m_f8ZMag)
    );
}


T3DVector vecMix(const T3DVector& vec1, const T3DVector& vec2)
{
    return T3DVector
    (
        vec1.m_f8XMag * vec2.m_f8YMag
        , vec1.m_f8XMag * vec2.m_f8ZMag
        , vec1.m_f8YMag * vec2.m_f8ZMag
    );
}




// ---------------------------------------------------------------------------
//   CLASS: T3DVector
//  PREFIX: vec3
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  T3DVector: Constructor and Destructors
// ---------------------------------------------------------------------------
T3DVector::T3DVector() :

    m_f8XMag(0.0)
    , m_f8YMag(0.0)
    , m_f8ZMag(0.0)
{
}

T3DVector::T3DVector(   const   tCIDLib::TFloat8&   f8XMag
                        , const tCIDLib::TFloat8&   f8YMag
                        , const tCIDLib::TFloat8&   f8ZMag) :
    m_f8XMag(f8XMag)
    , m_f8YMag(f8YMag)
    , m_f8ZMag(f8ZMag)
{
}

T3DVector::~T3DVector()
{
}



// ---------------------------------------------------------------------------
//  T3DVector: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TVoid T3DVector::operator-=(const tCIDLib::TFloat8& f8Dec)
{
    m_f8XMag -= f8Dec;
    m_f8YMag -= f8Dec;
    m_f8ZMag -= f8Dec;
}

tCIDLib::TVoid T3DVector::operator+=(const tCIDLib::TFloat8& f8Inc)
{
    m_f8XMag += f8Inc;
    m_f8YMag += f8Inc;
    m_f8ZMag += f8Inc;
}

tCIDLib::TVoid T3DVector::operator*=(const tCIDLib::TFloat8& f8Mul)
{
    m_f8XMag *= f8Mul;
    m_f8YMag *= f8Mul;
    m_f8ZMag *= f8Mul;
}

tCIDLib::TVoid T3DVector::operator/=(const tCIDLib::TFloat8& f8Div)
{
    m_f8XMag /= f8Div;
    m_f8YMag /= f8Div;
    m_f8ZMag /= f8Div;
}


tCIDLib::TBoolean T3DVector::operator==(const T3DVector& vecToTest) const
{
    if (&vecToTest != this)
    {
        // Compare the values of the objects
        if ((vecToTest.m_f8XMag != m_f8XMag)
        ||  (vecToTest.m_f8YMag != m_f8YMag)
        ||  (vecToTest.m_f8ZMag != m_f8ZMag))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean T3DVector::operator!=(const T3DVector& vecToAssign) const
{
    return !operator==(vecToAssign);
}


tCIDLib::TVoid T3DVector::operator+=(const T3DVector& vecSrc)
{
    m_f8XMag += vecSrc.m_f8XMag;
    m_f8YMag += vecSrc.m_f8YMag;
    m_f8ZMag += vecSrc.m_f8ZMag;
}

tCIDLib::TVoid T3DVector::operator-=(const T3DVector& vecSrc)
{
    m_f8XMag -= vecSrc.m_f8XMag;
    m_f8YMag -= vecSrc.m_f8YMag;
    m_f8ZMag -= vecSrc.m_f8ZMag;
}

tCIDLib::TVoid T3DVector::operator*=(const T3DVector& vecSrc)
{
    m_f8XMag *= vecSrc.m_f8XMag;
    m_f8YMag *= vecSrc.m_f8YMag;
    m_f8ZMag *= vecSrc.m_f8ZMag;
}


tCIDLib::TVoid T3DVector::operator/=(const T3DVector& vecDiv)
{
    m_f8XMag /= vecDiv.m_f8XMag;
    m_f8YMag /= vecDiv.m_f8YMag;
    m_f8ZMag /= vecDiv.m_f8ZMag;
}


// ---------------------------------------------------------------------------
//  T3DVector: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid T3DVector::Abs()
{
    m_f8XMag = TMathLib::f8Abs(m_f8XMag);
    m_f8YMag = TMathLib::f8Abs(m_f8YMag);
    m_f8ZMag = TMathLib::f8Abs(m_f8ZMag);
}


tCIDLib::TVoid
T3DVector::AddScaled(const T3DVector& vecSrc, const tCIDLib::TFloat8& f8Scale)
{
    m_f8XMag += vecSrc.m_f8XMag * f8Scale;
    m_f8YMag += vecSrc.m_f8YMag * f8Scale;
    m_f8ZMag += vecSrc.m_f8ZMag * f8Scale;
}


tCIDLib::TVoid
T3DVector::Adjust(  const   tCIDLib::TFloat8&   f8X
                    , const tCIDLib::TFloat8&   f8Y
                    , const tCIDLib::TFloat8&   f8Z)
{
    m_f8XMag += f8X;
    m_f8YMag += f8Y;
    m_f8ZMag += f8Z;
}


tCIDLib::TBoolean T3DVector::bAtOrg() const
{
    if ((m_f8XMag == 0.0) && (m_f8YMag == 0.0) && (m_f8ZMag == 0.0))
        return kCIDLib::True;
     else
        return kCIDLib::False;
}


tCIDLib::TBoolean
T3DVector::bWithinLimits(const tCIDLib::TFloat8& f8Limit) const
{
    if ((m_f8XMag > f8Limit)
    ||  (m_f8YMag > f8Limit)
    ||  (m_f8ZMag > f8Limit)
    ||  (m_f8XMag < -f8Limit)
    ||  (m_f8YMag < -f8Limit)
    ||  (m_f8ZMag < -f8Limit))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TFloat8 T3DVector::f8Dot(const T3DVector& vec2) const
{
    return  (m_f8XMag * vec2.m_f8XMag)
            + (m_f8YMag * vec2.m_f8YMag)
            + (m_f8ZMag * vec2.m_f8ZMag);
}


tCIDLib::TFloat8 T3DVector::f8Magnitude() const
{
    return TMathLib::f8SqrRoot
    (
        (m_f8XMag * m_f8XMag)
        + (m_f8YMag * m_f8YMag)
        + (m_f8ZMag * m_f8ZMag)
    );
}


tCIDLib::TFloat8 T3DVector::f8MagSum() const
{
    return m_f8XMag + m_f8YMag + m_f8ZMag;
}


tCIDLib::TFloat8 T3DVector::f8X() const
{
    return m_f8XMag;
}

tCIDLib::TFloat8 T3DVector::f8X(const tCIDLib::TFloat8& f8New)
{
    m_f8XMag=f8New;
    return m_f8XMag;
}


tCIDLib::TFloat8 T3DVector::f8Y() const
{
    return m_f8YMag;
}

tCIDLib::TFloat8 T3DVector::f8Y(const tCIDLib::TFloat8& f8New)
{
    m_f8YMag=f8New;
    return m_f8YMag;
}


tCIDLib::TFloat8 T3DVector::f8Z() const
{
    return m_f8ZMag;
}

tCIDLib::TFloat8 T3DVector::f8Z(const tCIDLib::TFloat8& f8New)
{
    m_f8ZMag=f8New;
    return m_f8ZMag;
}


tCIDLib::TVoid T3DVector::ClipToDegrees()
{
    if (m_f8XMag > 360.0)
        m_f8XMag = TMathLib::f8Mod(m_f8XMag, 360.0);

    if (m_f8YMag > 360.0)
        m_f8YMag = TMathLib::f8Mod(m_f8YMag, 360.0);

    if (m_f8ZMag > 360.0)
        m_f8ZMag = TMathLib::f8Mod(m_f8ZMag, 360.0);
}


tCIDLib::TVoid T3DVector::Negate()
{
    m_f8XMag *= -1;
    m_f8YMag *= -1;
    m_f8ZMag *= -1;
}


tCIDLib::TVoid T3DVector::Normalize()
{
    tCIDLib::TFloat8 f8Mag = TMathLib::f8SqrRoot
    (
        (m_f8XMag * m_f8XMag)
        + (m_f8YMag * m_f8YMag)
        + (m_f8ZMag * m_f8ZMag)
    );

    if (f8Mag)
    {
        m_f8XMag /= f8Mag;
        m_f8YMag /= f8Mag;
        m_f8ZMag /= f8Mag;
    }
}


tCIDLib::TVoid T3DVector::ToRadians()
{
    if (TMathLib::f8Abs(m_f8XMag) > 360.0)
        m_f8XMag = TMathLib::f8Mod(m_f8XMag, 360.0);

    if (TMathLib::f8Abs(m_f8YMag) > 360.0)
        m_f8YMag = TMathLib::f8Mod(m_f8YMag, 360.0);

    if (TMathLib::f8Abs(m_f8ZMag) > 360.0)
        m_f8ZMag = TMathLib::f8Mod(m_f8ZMag, 360.0);

    m_f8XMag *= kCIDLib::f8PI / 180.0;
    m_f8YMag *= kCIDLib::f8PI / 180.0;
    m_f8ZMag *= kCIDLib::f8PI / 180.0;
}


tCIDLib::TVoid T3DVector::Cross(const T3DVector& vec2)
{
    tCIDLib::TFloat8 f8XTmp =   (m_f8YMag * vec2.m_f8ZMag)
                                - (m_f8ZMag * vec2.m_f8YMag);
    tCIDLib::TFloat8 f8YTmp =   (m_f8ZMag * vec2.m_f8XMag)
                                - (m_f8XMag * vec2.m_f8ZMag);
    tCIDLib::TFloat8 f8ZTmp =   (m_f8XMag * vec2.m_f8YMag)
                                - (m_f8YMag * vec2.m_f8XMag);
    m_f8XMag = f8XTmp;
    m_f8YMag = f8YTmp;
    m_f8ZMag = f8ZTmp;
}


tCIDLib::TVoid T3DVector::HalfBetween(const T3DVector& vec2)
{
    m_f8XMag = 0.5 * (m_f8XMag + vec2.m_f8XMag);
    m_f8YMag = 0.5 * (m_f8YMag + vec2.m_f8YMag);
    m_f8ZMag = 0.5 * (m_f8ZMag + vec2.m_f8ZMag);
}


tCIDLib::TVoid T3DVector::Mix(const T3DVector& vec2)
{
    m_f8XMag *= vec2.m_f8YMag;
    m_f8XMag *= vec2.m_f8ZMag;
    m_f8YMag *= vec2.m_f8ZMag;
}


tCIDLib::TVoid
T3DVector::Set( const   tCIDLib::TFloat8&   f8XMag
                , const tCIDLib::TFloat8&   f8YMag
                , const tCIDLib::TFloat8&   f8ZMag)
{
    m_f8XMag = f8XMag;
    m_f8YMag = f8YMag;
    m_f8ZMag = f8ZMag;
}


tCIDLib::TVoid T3DVector::Square()
{
    m_f8XMag *= m_f8XMag;
    m_f8YMag *= m_f8YMag;
    m_f8ZMag *= m_f8ZMag;
}


tCIDLib::TVoid
T3DVector::SubScaled(const T3DVector& vecSrc, const tCIDLib::TFloat8& f8Scale)
{
    m_f8XMag -= vecSrc.m_f8XMag * f8Scale;
    m_f8YMag -= vecSrc.m_f8YMag * f8Scale;
    m_f8ZMag -= vecSrc.m_f8ZMag * f8Scale;
}



// ---------------------------------------------------------------------------
//  T3DVector: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid T3DVector::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo   << L"{" << m_f8XMag
                    << L"," << m_f8YMag
                    << L"," << m_f8ZMag
                    << L"}";
}


tCIDLib::TVoid T3DVector::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Get the values out
    strmToReadFrom  >> m_f8XMag
                    >> m_f8YMag
                    >> m_f8ZMag;
}

tCIDLib::TVoid T3DVector::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_f8XMag
                    << m_f8YMag
                    << m_f8ZMag;
}

