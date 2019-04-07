//
// FILE NAME: CIDMath_Complex.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1999
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
//  This file implements the TComplex class
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
RTTIDecls(TComplex,TObject)


// ---------------------------------------------------------------------------
//  These have to be global
// ---------------------------------------------------------------------------
TComplex operator+(const TComplex& cplxLHS, const TComplex& cplxRHS)
{
    return TComplex
    (
        cplxLHS.m_f8Real + cplxRHS.m_f8Real
        , cplxLHS.m_f8Imag + cplxRHS.m_f8Imag
    );
}

TComplex operator-(const TComplex& cplxLHS, const TComplex& cplxRHS)
{
    return TComplex
    (
        cplxLHS.m_f8Real - cplxRHS.m_f8Real
        , cplxLHS.m_f8Imag - cplxRHS.m_f8Imag
    );
}

TComplex operator*(const TComplex& cplxLHS, const TComplex& cplxRHS)
{
    return TComplex
    (
        (cplxLHS.m_f8Real * cplxRHS.m_f8Real)
            - (cplxLHS.m_f8Imag * cplxRHS.m_f8Imag)
        , (cplxLHS.m_f8Real * cplxRHS.m_f8Imag)
            + (cplxLHS.m_f8Imag * cplxRHS.m_f8Real)
    );
}


TComplex operator/(const TComplex& cplxLHS, const TComplex& cplxRHS)
{
    const tCIDLib::TFloat8 f8Tmp1 = cplxRHS.m_f8Real / cplxRHS.m_f8Imag;
    const tCIDLib::TFloat8 f8Tmp2 = cplxRHS.m_f8Imag
                                    + (f8Tmp1 * cplxRHS.m_f8Real);
    return TComplex
    (
        (cplxLHS.m_f8Real * f8Tmp1 + cplxLHS.m_f8Imag) / f8Tmp2
        , (cplxLHS.m_f8Imag * f8Tmp1 - cplxLHS.m_f8Real) / f8Tmp2
    );
}



// ---------------------------------------------------------------------------
//   CLASS: TComplex
//  PREFIX: cplx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TComplex: Constructors and Destructor
// ---------------------------------------------------------------------------
TComplex::TComplex( const   tCIDLib::TFloat8    f8Real
                    , const tCIDLib::TFloat8    f8Imag) :
    m_f8Real(f8Real)
    , m_f8Imag(f8Imag)
{
}

TComplex::TComplex(const tCIDLib::TFloat8 f8Real) :

    m_f8Real(f8Real)
    , m_f8Imag(0.0)
{
}

TComplex::~TComplex()
{
}


// ---------------------------------------------------------------------------
//  TComplex: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TComplex::operator==(const TComplex& cplxToCompare) const
{
    if (this == &cplxToCompare)
        return kCIDLib::True;

    return (m_f8Real == cplxToCompare.m_f8Real)
            && (m_f8Imag == cplxToCompare.m_f8Imag);
}

tCIDLib::TBoolean TComplex::operator!=(const TComplex& cplxToCompare) const
{
    return !operator==(cplxToCompare);
}


tCIDLib::TVoid TComplex::operator+=(const TComplex& cplxToAdd)
{
    m_f8Real += cplxToAdd.m_f8Real;
    m_f8Imag += cplxToAdd.m_f8Imag;
}

tCIDLib::TVoid TComplex::operator-=(const TComplex& cplxToSub)
{
    m_f8Real -= cplxToSub.m_f8Real;
    m_f8Imag -= cplxToSub.m_f8Imag;
}


tCIDLib::TVoid TComplex::operator*=(const tCIDLib::TFloat8 f8MulBy)
{
    m_f8Real *= f8MulBy;
    m_f8Imag *= f8MulBy;
}

tCIDLib::TVoid TComplex::operator*=(const TComplex& cplxToMul)
{
    // Don't affect either until both are done, do new real to temp
    const tCIDLib::TFloat8 f8TmpReal = (m_f8Real * cplxToMul.m_f8Real)
                                        - (m_f8Imag * cplxToMul.m_f8Imag);
    m_f8Imag = (m_f8Real * cplxToMul.m_f8Imag) + (m_f8Imag * cplxToMul.m_f8Real);
    m_f8Real = f8TmpReal;
}


tCIDLib::TVoid TComplex::operator/=(const tCIDLib::TFloat8 f8DivBy)
{
    m_f8Real /= f8DivBy;
    m_f8Imag /= f8DivBy;
}

tCIDLib::TVoid TComplex::operator/=(const TComplex& cplxToDiv)
{
    const tCIDLib::TFloat8 f8Tmp1 = cplxToDiv.m_f8Real / cplxToDiv.m_f8Imag;
    const tCIDLib::TFloat8 f8Tmp2 = cplxToDiv.m_f8Imag
                                    + (f8Tmp1 * cplxToDiv.m_f8Real);

    // Don't affect either until both are done, do new real to temp
    const tCIDLib::TFloat8 f8TmpReal = (m_f8Real * f8Tmp1 + m_f8Imag) / f8Tmp2;

    m_f8Imag = (m_f8Imag * f8Tmp1 - m_f8Real) / f8Tmp2;
    m_f8Real = f8TmpReal;
}



// ---------------------------------------------------------------------------
//  TComplex: Public, non-virtual methods
// ---------------------------------------------------------------------------
TComplex TComplex::cplxConjugate() const
{
    return TComplex(m_f8Real, 0.0 - m_f8Imag);
}

tCIDLib::TFloat8 TComplex::f8Imag() const
{
    return m_f8Imag;
}

tCIDLib::TFloat8 TComplex::f8Modulus() const
{
    return ((m_f8Real * m_f8Real) + (m_f8Imag * m_f8Imag));
}

tCIDLib::TFloat8 TComplex::f8Real() const
{
    return m_f8Real;
}

tCIDLib::TVoid TComplex::SetToConjugate()
{
    // Flip the sign of the imaginary component
    m_f8Imag = 0.0 - m_f8Imag;
}


// ---------------------------------------------------------------------------
//  TComplex: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TComplex::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << L"{" << m_f8Real << L"," << m_f8Imag << L"}";
}

tCIDLib::TVoid TComplex::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom >> m_f8Real >> m_f8Imag;
}

tCIDLib::TVoid TComplex::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_f8Real << m_f8Imag;
}
