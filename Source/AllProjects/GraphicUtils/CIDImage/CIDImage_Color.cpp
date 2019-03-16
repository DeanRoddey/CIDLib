//
// FILE NAME: CIDImage_Color.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/05/1993
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
//  This file implements the TFRGBClr class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CIDLib includes
// ---------------------------------------------------------------------------
#include    "CIDImage_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFRGBClr,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TFRGBClr
// PREFIX: frgb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRGBClr: Public, static methods
// ---------------------------------------------------------------------------
TFRGBClr& TFRGBClr::Nul_TFRGBClr()
{
    static TFRGBClr* pfrgbNull = 0;
    if (!pfrgbNull)
    {
        TBaseLock lockInit;
        if (!pfrgbNull)
            TRawMem::pExchangePtr(&pfrgbNull, new TFRGBClr);
    }
    return *pfrgbNull;
}


// ---------------------------------------------------------------------------
//  TFRGBClr: Constructors and Destructor
// ---------------------------------------------------------------------------
TFRGBClr::TFRGBClr( const   tCIDLib::TFloat8&   f8Red
                    , const tCIDLib::TFloat8&   f8Green
                    , const tCIDLib::TFloat8&   f8Blue
                    , const tCIDLib::TFloat8&   f8Alpha) :
    m_f8Red(f8Red)
    , m_f8Green(f8Green)
    , m_f8Blue(f8Blue)
    , m_f8Alpha(f8Alpha)
{
}

TFRGBClr::TFRGBClr(const TFRGBClr& frgbToCopy) :

    m_f8Red(frgbToCopy.m_f8Red)
    , m_f8Green(frgbToCopy.m_f8Green)
    , m_f8Blue(frgbToCopy.m_f8Blue)
    , m_f8Alpha(frgbToCopy.m_f8Alpha)
{
}

TFRGBClr::TFRGBClr() :

    m_f8Red(0.0)
    , m_f8Green(0.0)
    , m_f8Blue(0.0)
    , m_f8Alpha(0.0)
{
}

TFRGBClr::~TFRGBClr()
{
}


// ---------------------------------------------------------------------------
//  TFRGBClr: Public operators
// ---------------------------------------------------------------------------
TFRGBClr& TFRGBClr::operator=(const TFRGBClr& frgbToAssign)
{
    if (this == &frgbToAssign)
        return *this;

    m_f8Red     = frgbToAssign.m_f8Red;
    m_f8Green   = frgbToAssign.m_f8Green;
    m_f8Blue    = frgbToAssign.m_f8Blue;
    m_f8Alpha   = frgbToAssign.m_f8Alpha;

    return *this;
}

tCIDLib::TVoid TFRGBClr::operator+=(const tCIDLib::TFloat8& f8Inc)
{
    m_f8Red += f8Inc;
    m_f8Green += f8Inc;
    m_f8Blue += f8Inc;
}

tCIDLib::TVoid TFRGBClr::operator+=(const TFRGBClr& frgbSrc)
{
    m_f8Red += frgbSrc.m_f8Red;
    m_f8Green += frgbSrc.m_f8Green;
    m_f8Blue += frgbSrc.m_f8Blue;
}

tCIDLib::TVoid TFRGBClr::operator*=(const tCIDLib::TFloat8& f8Scale)
{
    m_f8Red *= f8Scale;
    m_f8Green *= f8Scale;
    m_f8Blue *= f8Scale;
}

tCIDLib::TVoid TFRGBClr::operator*=(const TFRGBClr& frgbSrc)
{
    m_f8Red *= frgbSrc.m_f8Red;
    m_f8Green *= frgbSrc.m_f8Green;
    m_f8Blue *= frgbSrc.m_f8Blue;
}

tCIDLib::TVoid TFRGBClr::operator-=(const TFRGBClr& frgbSrc)
{
    m_f8Red -= frgbSrc.m_f8Red;
    m_f8Green -= frgbSrc.m_f8Green;
    m_f8Blue -= frgbSrc.m_f8Blue;
}

tCIDLib::TVoid TFRGBClr::operator-=(const tCIDLib::TFloat8& f8Dec)
{
    m_f8Red -= f8Dec;
    m_f8Green -= f8Dec;
    m_f8Blue -= f8Dec;
}

tCIDLib::TVoid TFRGBClr::operator/=(const tCIDLib::TFloat8& f8Divisor)
{
    m_f8Red /= f8Divisor;
    m_f8Green /= f8Divisor;
    m_f8Blue /= f8Divisor;
}

tCIDLib::TVoid TFRGBClr::operator/=(const TFRGBClr& frgbSrc)
{
    m_f8Red /= frgbSrc.m_f8Red;
    m_f8Green /= frgbSrc.m_f8Green;
    m_f8Blue /= frgbSrc.m_f8Blue;
}

tCIDLib::TBoolean TFRGBClr::operator==(const TFRGBClr& frgbToTest) const
{
    if (this == &frgbToTest)
        return kCIDLib::True;

    // Compare the values of the objects
    if (m_f8Red != frgbToTest.m_f8Red)
        return kCIDLib::False;

    if (m_f8Green != frgbToTest.m_f8Green)
        return kCIDLib::False;

    if (m_f8Blue != frgbToTest.m_f8Blue)
        return kCIDLib::False;

    if (m_f8Alpha != frgbToTest.m_f8Alpha)
        return kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean TFRGBClr::operator!=(const TFRGBClr& frgbToTest) const
{
    return !operator==(frgbToTest);
}


TFRGBClr operator+(const TFRGBClr& frgb1, const TFRGBClr& frgb2)
{
    return TFRGBClr( frgb1.m_f8Red + frgb2.m_f8Red
                    , frgb1.m_f8Green + frgb2.m_f8Green
                    , frgb1.m_f8Blue + frgb2.m_f8Blue
                    , frgb1.m_f8Alpha);
}

TFRGBClr operator+(const TFRGBClr& frgb1, const tCIDLib::TFloat8& f8Inc)
{
    return TFRGBClr(frgb1.m_f8Red + f8Inc
                    , frgb1.m_f8Green + f8Inc
                    , frgb1.m_f8Blue + f8Inc
                    , frgb1.m_f8Alpha);
}

TFRGBClr operator-(const TFRGBClr& frgb1, const TFRGBClr& frgb2)
{
    return TFRGBClr(frgb1.m_f8Red - frgb2.m_f8Red
                    , frgb1.m_f8Green - frgb2.m_f8Green
                    , frgb1.m_f8Blue - frgb2.m_f8Blue
                    , frgb1.m_f8Alpha);
}

TFRGBClr operator-(const TFRGBClr& frgb1, const tCIDLib::TFloat8& f8Dec)
{
    return TFRGBClr(frgb1.m_f8Red - f8Dec
                    , frgb1.m_f8Green - f8Dec
                    , frgb1.m_f8Blue - f8Dec
                    , frgb1.m_f8Alpha);
}

TFRGBClr operator*(const TFRGBClr& frgb1, const TFRGBClr& frgb2)
{
    return TFRGBClr(frgb1.m_f8Red * frgb2.m_f8Red
                    , frgb1.m_f8Green * frgb2.m_f8Green
                    , frgb1.m_f8Blue * frgb2.m_f8Blue
                    , frgb1.m_f8Alpha);
}

TFRGBClr
operator*(const TFRGBClr& frgb1, const tCIDLib::TFloat8& f8Scale)
{
    return TFRGBClr(frgb1.m_f8Red * f8Scale
                    , frgb1.m_f8Green * f8Scale
                    , frgb1.m_f8Blue * f8Scale
                    , frgb1.m_f8Alpha);
}

TFRGBClr operator/(const TFRGBClr& frgb1, const TFRGBClr& frgb2)
{
    return TFRGBClr(frgb1.m_f8Red / frgb2.m_f8Red
                    , frgb1.m_f8Green / frgb2.m_f8Green
                    , frgb1.m_f8Blue / frgb2.m_f8Blue
                    , frgb1.m_f8Alpha);
}

TFRGBClr
operator/(const TFRGBClr& frgb1, const tCIDLib::TFloat8& f8Divisor)
{
    return TFRGBClr(frgb1.m_f8Red / f8Divisor
                    , frgb1.m_f8Green / f8Divisor
                    , frgb1.m_f8Blue / f8Divisor
                    , frgb1.m_f8Alpha);
}



// ---------------------------------------------------------------------------
//  TFRGBClr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TFRGBClr::AddScaled(const TFRGBClr& frgbBase, const tCIDLib::TFloat8& f8Scale)
{
    m_f8Red     += (frgbBase.m_f8Red * f8Scale);
    m_f8Green   += (frgbBase.m_f8Green * f8Scale);
    m_f8Blue    += (frgbBase.m_f8Blue * f8Scale);
}


tCIDLib::TVoid TFRGBClr::Adjust(const   tCIDLib::TFloat8&   f8Red
                                , const tCIDLib::TFloat8&   f8Green
                                , const tCIDLib::TFloat8&   f8Blue
                                , const tCIDLib::TFloat8&   f8Alpha)
{
    m_f8Red += f8Red;
    m_f8Green += f8Green;
    m_f8Blue += f8Blue;
}


tCIDLib::TVoid
TFRGBClr::AlphaBlend(const TFRGBClr& frgbSrc, const tCIDLib::TFloat8& f8Alpha)
{
    // If we are totally transparent, don't waste time
    if (m_f8Alpha >= 1.0)
        return;

    // Calc the multiplier
    const tCIDLib::TFloat8 f8Mul = (1.0 - f8Alpha);

    m_f8Red   += frgbSrc.m_f8Red * f8Mul;
    m_f8Green += frgbSrc.m_f8Green * f8Mul;
    m_f8Blue  += frgbSrc.m_f8Blue * f8Mul;
}


tCIDLib::TVoid TFRGBClr::Clip()
{
    if (m_f8Red > 1.0)
        m_f8Red = 1.0;
    else if (m_f8Red < 0.0)
        m_f8Red = 0.0;

    if (m_f8Green > 1.0)
        m_f8Green = 1.0;
    else if (m_f8Green < 0.0)
        m_f8Green = 0.0;

    if (m_f8Blue > 1.0)
        m_f8Blue = 1.0;
    else if (m_f8Blue < 0.0)
        m_f8Blue = 0.0;

    if (m_f8Alpha > 1.0)
        m_f8Alpha = 1.0;
    else if (m_f8Alpha < 0.0)
        m_f8Alpha = 0.0;
}


tCIDLib::TVoid TFRGBClr::ConvertToRGB(TRGBClr& rgbToFill) const
{
    tCIDLib::TFloat8 f8Red(m_f8Red);
    tCIDLib::TFloat8 f8Green(m_f8Green);
    tCIDLib::TFloat8 f8Blue(m_f8Blue);

    // Normalize the values first
    tCIDLib::TFloat8 f8Largest(f8Red);
    if (f8Green > f8Largest)
        f8Largest = f8Green;
    if (f8Blue > f8Largest)
        f8Largest = f8Blue;

    if (f8Largest > 1.0)
    {
        f8Red /= f8Largest;
        f8Green /= f8Largest;
        f8Blue /= f8Blue;
    }

    rgbToFill.Set
    (
        tCIDLib::TCard1(f8Red * kCIDLib::c1MaxCard)
        , tCIDLib::TCard1(f8Green * kCIDLib::c1MaxCard)
        , tCIDLib::TCard1(f8Blue * kCIDLib::c1MaxCard)
    );
}


tCIDLib::TFloat8 TFRGBClr::f8Alpha() const
{
    return m_f8Alpha;
}

tCIDLib::TFloat8 TFRGBClr::f8Alpha(const tCIDLib::TFloat8& f8New)
{
    m_f8Alpha = f8New;
    return m_f8Alpha;
}


tCIDLib::TFloat8 TFRGBClr::f8Blue() const
{
    return m_f8Blue;
}

tCIDLib::TFloat8 TFRGBClr::f8Blue(const tCIDLib::TFloat8& f8New)
{
    m_f8Blue = f8New;
    return m_f8Blue;
}


tCIDLib::TFloat8
f8Distance(const TFRGBClr& frgb1, const TFRGBClr& frgb2)
{
    return  TMathLib::f8Abs(frgb1.m_f8Red - frgb2.m_f8Red)
            + TMathLib::f8Abs(frgb1.m_f8Green - frgb2.m_f8Green)
            + TMathLib::f8Abs(frgb1.m_f8Blue - frgb2.m_f8Blue);
}

tCIDLib::TFloat8 TFRGBClr::f8Distance(const TFRGBClr& frgb2)
{
    return  TMathLib::f8Abs(m_f8Red - frgb2.m_f8Red)
            + TMathLib::f8Abs(m_f8Green - frgb2.m_f8Green)
            + TMathLib::f8Abs(m_f8Blue - frgb2.m_f8Blue);
}


tCIDLib::TFloat8 TFRGBClr::f8Green() const
{
    return m_f8Green;
}

tCIDLib::TFloat8 TFRGBClr::f8Green(const tCIDLib::TFloat8& f8New)
{
    m_f8Green = f8New;
    return m_f8Green;
}


tCIDLib::TFloat8 TFRGBClr::f8Magnitude() const
{
    return tCIDLib::TFloat8(m_f8Red + m_f8Green + m_f8Blue);
}


tCIDLib::TFloat8 TFRGBClr::f8Red() const
{
    return m_f8Red;
}

tCIDLib::TFloat8 TFRGBClr::f8Red(const tCIDLib::TFloat8& f8New)
{
    m_f8Red = f8New;
    return m_f8Red;
}


tCIDLib::TVoid TFRGBClr::FadeToBlack()
{
    m_f8Red = 0.0;
    m_f8Green = 0.0;
    m_f8Blue = 0.0;
}


tCIDLib::TVoid
TFRGBClr::Interpolate(  const   tCIDLib::TFloat8&   f8Frac
                        , const TFRGBClr&           frgbStartClr
                        , const TFRGBClr&           frgbEndClr)
{
    m_f8Red   = frgbStartClr.m_f8Red
                + (f8Frac * (frgbEndClr.m_f8Red - frgbStartClr.m_f8Red));

    m_f8Green = frgbStartClr.m_f8Green
                + (f8Frac * (frgbEndClr.m_f8Green - frgbStartClr.m_f8Green));

    m_f8Blue  = frgbStartClr.m_f8Blue
                + (f8Frac * (frgbEndClr.m_f8Blue - frgbStartClr.m_f8Blue));
}


//
//  We find the largest component. If it is larger than 1.0, then we divide
//  all components by that value. The largest value will become 1.0 and the
//  others will be scaled to maintain the original relationship.
//
tCIDLib::TVoid TFRGBClr::Normalize()
{
    tCIDLib::TFloat8 f8Largest(m_f8Red);
    if (m_f8Green > f8Largest)
        f8Largest = m_f8Green;
    if (m_f8Blue > f8Largest)
        f8Largest = m_f8Blue;

    if (f8Largest > 1.0)
    {
        m_f8Red /= f8Largest;
        m_f8Green /= f8Largest;
        m_f8Blue /= f8Largest;
    }
}


tCIDLib::TBoolean
TFRGBClr::bParseFromString(const TString& strSrc)
{
    TStringTokenizer stokSrc(&strSrc, L", ");

    try
    {
        TString strTmp;
        if (!stokSrc.bGetNextToken(strTmp))
            return kCIDLib::False;
        m_f8Red = strTmp.f8Val();

        if (!stokSrc.bGetNextToken(strTmp))
            return kCIDLib::False;
        m_f8Green = strTmp.f8Val();

        if (!stokSrc.bGetNextToken(strTmp))
            return kCIDLib::False;
        m_f8Blue = strTmp.f8Val();
    }

    catch(...)
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TVoid
TFRGBClr::SetToScaled(  const    TFRGBClr&          frgbBase
                        , const tCIDLib::TFloat8&   f8Scale)
{
    m_f8Red     = frgbBase.m_f8Red * f8Scale;
    m_f8Green   = frgbBase.m_f8Green * f8Scale;
    m_f8Blue    = frgbBase.m_f8Blue * f8Scale;
}


tCIDLib::TVoid TFRGBClr::SrcAlphaBlend(const TFRGBClr& frgbSrc)
{
    // If the source is totally transparent, don't waste time
    if (frgbSrc.m_f8Alpha >= 1.0)
        return;

    // Calc the multiplier
    const tCIDLib::TFloat8 f8Mul = (1.0 - frgbSrc.m_f8Alpha);

    m_f8Red   += frgbSrc.m_f8Red * f8Mul;
    m_f8Green += frgbSrc.m_f8Green * f8Mul;
    m_f8Blue  += frgbSrc.m_f8Blue * f8Mul;
}


tCIDLib::TVoid TFRGBClr::TarAlphaBlend(const TFRGBClr& frgbSrc)
{
    // If we are totally transparent, don't waste time
    if (m_f8Alpha >= 1.0)
        return;

    // Calc the multiplier
    const tCIDLib::TFloat8 f8Mul = (1.0 - m_f8Alpha);

    m_f8Red   += frgbSrc.m_f8Red * f8Mul;
    m_f8Green += frgbSrc.m_f8Green * f8Mul;
    m_f8Blue  += frgbSrc.m_f8Blue * f8Mul;
}


// ---------------------------------------------------------------------------
//  TFRGBClr: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFRGBClr::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Format the attribute info into the string
    strmToWriteTo   << TFloat(m_f8Red, 5)
                    << L"," << TFloat(m_f8Green, 5)
                    << L"," << TFloat(m_f8Blue, 5);
}


tCIDLib::TVoid TFRGBClr::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_f8Red
                    >> m_f8Green
                    >> m_f8Blue
                    >> m_f8Alpha;
}


tCIDLib::TVoid TFRGBClr::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_f8Red
                    << m_f8Green
                    << m_f8Blue
                    << m_f8Alpha;
}


