//
// FILE NAME: CIDMath_2DVector.cpp
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
//  This file implements the T2DVector class.
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
RTTIDecls(T2DVector,TObject)


// ---------------------------------------------------------------------------
//  Our global binary operators and some friend methods
// ---------------------------------------------------------------------------
T2DVector operator+(const T2DVector& vec1, const T2DVector& vec2)
{
    return T2DVector
    (
        vec1.m_f8XMag + vec2.m_f8XMag
        , vec1.m_f8YMag + vec2.m_f8YMag
    );
}
T2DVector operator-(const T2DVector& vec1, const T2DVector& vec2)
{
    return T2DVector
    (
        vec1.m_f8XMag - vec2.m_f8XMag
        , vec1.m_f8YMag - vec2.m_f8YMag
    );
}
T2DVector operator*(const T2DVector& vec1, const T2DVector& vec2)
{
    return T2DVector
    (
        vec1.m_f8XMag * vec2.m_f8XMag
        , vec1.m_f8YMag * vec2.m_f8YMag
    );
}
T2DVector operator/(const T2DVector& vec1, const T2DVector& vec2)
{
    return T2DVector(vec1.m_f8XMag / vec2.m_f8XMag
                    , vec1.m_f8YMag / vec2.m_f8YMag);
}

T2DVector operator/(const T2DVector& vec1, const tCIDLib::TFloat8& f8Div)
{
    return T2DVector
    (
        vec1.m_f8XMag / f8Div
        , vec1.m_f8YMag / f8Div
    );
}

T2DVector vecHalfBetween(const T2DVector& vec1, const T2DVector& vec2)
{
    return T2DVector
    (
        0.5 * (vec1.m_f8XMag + vec2.m_f8XMag)
        , 0.5  * (vec1.m_f8YMag + vec2.m_f8YMag)
    );
}



// ---------------------------------------------------------------------------
//   CLASS: T2DVector
//  PREFIX: vec2
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  T2DVector: Constructor and Destructors
// ---------------------------------------------------------------------------
T2DVector::T2DVector() :

    m_f8XMag(0.0)
    , m_f8YMag(0.0)
{
}

T2DVector::T2DVector(   const   tCIDLib::TFloat8&   f8XMag
                        , const tCIDLib::TFloat8&   f8YMag) :

    m_f8XMag(f8XMag)
    , m_f8YMag(f8YMag)
{
}

T2DVector::~T2DVector()
{
}


// ---------------------------------------------------------------------------
//  T2DVector: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TVoid T2DVector::operator-=(const T2DVector& vecSrc)
{
    m_f8XMag -= vecSrc.m_f8XMag;
    m_f8YMag -= vecSrc.m_f8YMag;
}

tCIDLib::TVoid T2DVector::operator+=(const T2DVector& vecSrc)
{
    m_f8XMag += vecSrc.m_f8XMag;
    m_f8YMag += vecSrc.m_f8YMag;
}

tCIDLib::TVoid T2DVector::operator*=(const T2DVector& vecSrc)
{
    m_f8XMag *= vecSrc.m_f8XMag;
    m_f8YMag *= vecSrc.m_f8YMag;
}

tCIDLib::TVoid T2DVector::operator*=(const tCIDLib::TFloat8& f8Factor)
{
    m_f8XMag *= f8Factor;
    m_f8YMag *= f8Factor;
}


tCIDLib::TVoid T2DVector::operator/=(const T2DVector& vecDiv)
{
    m_f8XMag /= vecDiv.m_f8XMag;
    m_f8YMag /= vecDiv.m_f8YMag;
}

tCIDLib::TBoolean T2DVector::operator==(const T2DVector& vecToTest) const
{
    if (this == &vecToTest)
        return kCIDLib::True;

    // Compare the values of the objects
    if (vecToTest.m_f8XMag != m_f8XMag)
        return kCIDLib::False;

    if (vecToTest.m_f8YMag != m_f8YMag)
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean T2DVector::operator!=(const T2DVector& vecToAssign) const
{
    return !operator==(vecToAssign);
}



// ---------------------------------------------------------------------------
//  T2DVector: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean T2DVector::bAtOrg() const
{
    if ((m_f8XMag == 0.0) && (m_f8YMag == 0.0))
        return kCIDLib::True;
     else
        return kCIDLib::False;
}


tCIDLib::TFloat8 f8Dot(const T2DVector& vec1, const T2DVector& vec2)
{
    return  (vec1.m_f8XMag * vec2.m_f8XMag) + (vec1.m_f8YMag * vec2.m_f8YMag);
}


tCIDLib::TFloat8 T2DVector::f8Magnitude() const
{
    return TMathLib::f8SqrRoot((m_f8XMag * m_f8XMag) + (m_f8YMag * m_f8YMag));
}


tCIDLib::TFloat8 T2DVector::f8X() const
{
    return m_f8XMag;
}

tCIDLib::TFloat8 T2DVector::f8X(const tCIDLib::TFloat8& f8New)
{
    m_f8XMag = f8New;
    return m_f8XMag;
}


tCIDLib::TFloat8 T2DVector::f8Y() const
{
    return m_f8YMag;
}

tCIDLib::TFloat8 T2DVector::f8Y(const tCIDLib::TFloat8& f8New)
{
    m_f8YMag = f8New;
    return m_f8YMag;
}


tCIDLib::TVoid T2DVector::Negate()
{
    m_f8XMag = -m_f8XMag;
    m_f8YMag = -m_f8YMag;
}


tCIDLib::TVoid T2DVector::Normalize()
{
    tCIDLib::TFloat8 f8Mag = TMathLib::f8SqrRoot
    (
        (m_f8XMag * m_f8XMag) + (m_f8YMag * m_f8YMag)
    );
    m_f8XMag /= f8Mag;
    m_f8YMag /= f8Mag;
}


tCIDLib::TVoid
T2DVector::Set( const   tCIDLib::TFloat8&   f8XMag
                , const tCIDLib::TFloat8&   f8YMag)
{
    m_f8XMag = f8XMag;
    m_f8YMag = f8YMag;
}


tCIDLib::TVoid T2DVector::Square()
{
    m_f8XMag *= m_f8XMag;
    m_f8YMag *= m_f8YMag;
}


tCIDLib::TVoid T2DVector::ToRadians()
{
    if (TMathLib::f8Abs(m_f8XMag) > 360.0)
         m_f8XMag = TMathLib::f8Mod(m_f8XMag, 360.0);

    if (TMathLib::f8Abs(m_f8YMag) > 360.0)
        m_f8YMag = TMathLib::f8Mod(m_f8YMag, 360.0);

    m_f8XMag *= kCIDLib::f8PI / 180.0;
    m_f8YMag *= kCIDLib::f8PI / 180.0;
}


// ---------------------------------------------------------------------------
//  T2DVector: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid T2DVector::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << L"{" << m_f8XMag << L","
                  << m_f8YMag << L"}";
}

tCIDLib::TVoid T2DVector::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Get the values out
    strmToReadFrom  >> m_f8XMag
                    >> m_f8YMag;
}

tCIDLib::TVoid T2DVector::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Write out the data members
    strmToWriteTo   << m_f8XMag
                    << m_f8YMag;
}

