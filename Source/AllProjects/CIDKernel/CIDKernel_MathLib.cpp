//
// FILE NAME: CIDKernel_MathLib.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This file provides the wrapper functions that turn around and call the
//  RTL math operations.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  Include some system headers
// ---------------------------------------------------------------------------
#include    <math.h>



// ---------------------------------------------------------------------------
//  Log functions
// ---------------------------------------------------------------------------
tCIDLib::TFloat4 TMathLib::f4Log10(const tCIDLib::TFloat4 f4Value)
{
    return log10f(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Log10(const tCIDLib::TFloat8& f8Value)
{
    return log10(f8Value);
}

tCIDLib::TFloat4 TMathLib::f4Log(const tCIDLib::TFloat4 f4Value)
{
    return logf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Log(const tCIDLib::TFloat8& f8Value)
{
    return log(f8Value);
}


// ---------------------------------------------------------------------------
//  Geometrical functions
// ---------------------------------------------------------------------------
tCIDLib::TFloat4
TMathLib::f4Hypotenuse(const tCIDLib::TFloat4 f4X, const tCIDLib::TFloat4 f4Y)
{
    // hypot() is not available in some cases, so do it ourself for now
    return sqrtf((f4X * f4X) + (f4Y * f4Y));
}

tCIDLib::TFloat8
TMathLib::f8Hypotenuse(const tCIDLib::TFloat8& f8X, const tCIDLib::TFloat8& f8Y)
{
    // hypot() is not available in some cases, so do it ourself for now
    return sqrt((f8X * f8X) + (f8Y * f8Y));
}



// ---------------------------------------------------------------------------
//  Miscellaneous functions
// ---------------------------------------------------------------------------
tCIDLib::TFloat4 TMathLib::f4Abs(const tCIDLib::TFloat4& f4Value)
{
    return fabsf(f4Value);
}


tCIDLib::TFloat8 TMathLib::f8Abs(const tCIDLib::TFloat8& f8Value)
{
    return fabs(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4Ceil(const tCIDLib::TFloat4 f4Value)
{
    return ceilf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Ceil(const tCIDLib::TFloat8& f8Value)
{
    return ceil(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4Exponent(const tCIDLib::TFloat4 f4Value)
{
    return expf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Exponent(const tCIDLib::TFloat8& f8Value)
{
    return exp(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4Floor(const tCIDLib::TFloat4 f4Value)
{
    return floorf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Floor(const tCIDLib::TFloat8& f8Value)
{
    return floor(f8Value);
}


tCIDLib::TFloat8 TMathLib::f8MakeReal(  const   tCIDLib::TFloat8&   f8Mantissa
                                        , const tCIDLib::TInt4      i4Exponent)
{
    return ldexp(f8Mantissa, i4Exponent);
}


tCIDLib::TFloat4
TMathLib::f4Mod(const   tCIDLib::TFloat4 f4Value
                , const tCIDLib::TFloat4 f4Divisor)
{
    return fmodf(f4Value, f4Divisor);
}

tCIDLib::TFloat8
TMathLib::f8Mod(const   tCIDLib::TFloat8& f8Value
                , const tCIDLib::TFloat8& f8Divisor)
{
    return fmod(f8Value, f8Divisor);
}


tCIDLib::TFloat4
TMathLib::f4Power(  const   tCIDLib::TFloat4 f4Value
                    , const tCIDLib::TFloat4 f4Power)
{
    return powf(f4Value, f4Power);
}

tCIDLib::TFloat8
TMathLib::f8Power(  const   tCIDLib::TFloat8& f8Value
                    , const tCIDLib::TFloat8& f8Power)
{
    return pow(f8Value, f8Power);
}


tCIDLib::TFloat4
TMathLib::f4Split(  const   tCIDLib::TFloat4    f4Value
                    ,       tCIDLib::TFloat4&   f4Integral)
{
    return modff(f4Value, &f4Integral);
}

tCIDLib::TFloat8
TMathLib::f8Split(  const   tCIDLib::TFloat8& f8Value
                    ,       tCIDLib::TFloat8& f8Integral)
{
    return modf(f8Value, &f8Integral);
}


tCIDLib::TInt4 TMathLib::i4Floor(const tCIDLib::TFloat4 f4Value)
{
    return tCIDLib::TInt4(floorf(f4Value));
}

tCIDLib::TInt4 TMathLib::i4Floor(const tCIDLib::TFloat8& f8Value)
{
    return tCIDLib::TInt4(floor(f8Value));
}


// ---------------------------------------------------------------------------
//  Rounding functions
// ---------------------------------------------------------------------------

// Round to the closest 32 bit unsigned. If less than zero, it is clipped to zero
tCIDLib::TCard4 TMathLib::c4Round(const tCIDLib::TFloat8 f8ToRound)
{
    tCIDLib::TFloat8 f8Int;
    tCIDLib::TFloat8 f8Frac;
    f8Frac = modf(f8ToRound, &f8Int);

    // Can't represent it, so return zero
    if (f8Int < 0)
        return 0;

    tCIDLib::TCard4 c4Ret = tCIDLib::TCard4(f8Int);
    if (f8Frac >= 0.5)
        c4Ret++;
    return c4Ret;
}

// Round to the closest 32 bit signed
tCIDLib::TInt4 TMathLib::i4Round(const tCIDLib::TFloat8 f8ToRound)
{
    tCIDLib::TFloat8 f8Int;
    tCIDLib::TFloat8 f8Frac;
    f8Frac = modf(f8ToRound, &f8Int);

    tCIDLib::TInt4 i4Ret = tCIDLib::TInt4(f8Int);
    if (f8Frac >= 0.5)
    {
        // Round down for negative, up for positive
        if (i4Ret < 0)
            i4Ret--;
        else
            i4Ret++;
    }
    return i4Ret;
}

tCIDLib::TVoid TMathLib::Round(         tCIDLib::TFloat4&       f4ToRound
                                , const tCIDLib::ERoundTypes    eType)
{
    // Split it out into integral and fractional parts
    tCIDLib::TFloat4 f4Int;
    tCIDLib::TFloat4 f4Frac;
    f4Frac = modff(f4ToRound, &f4Int);

    if (eType == tCIDLib::ERoundTypes::Closest)
    {
        if (f4Frac < 0.5F)
            f4ToRound = f4Int;
        else
            f4ToRound = f4Int + ((f4Int < 0) ? -1.0F : 1.0F);
    }
     else if (eType == tCIDLib::ERoundTypes::Up)
    {
        if (f4Frac > 0)
            f4ToRound = f4Int + ((f4Int < 0) ? -1.0F : 1.0F);
        else
            f4ToRound = f4Int;
    }
     else if (eType == tCIDLib::ERoundTypes::Down)
    {
        f4ToRound = f4Int;
    }
}

tCIDLib::TVoid TMathLib::Round(         tCIDLib::TFloat8&       f8ToRound
                                , const tCIDLib::ERoundTypes    eType)
{
    // Split it out into integral and fractional parts
    tCIDLib::TFloat8 f8Int;
    tCIDLib::TFloat8 f8Frac;
    f8Frac = modf(f8ToRound, &f8Int);

    if (eType == tCIDLib::ERoundTypes::Closest)
    {
        if (f8Frac < 0.5)
            f8ToRound = f8Int;
        else
            f8ToRound = f8Int + ((f8Int < 0) ? -1.0 : 1.0);
    }
     else if (eType == tCIDLib::ERoundTypes::Up)
    {
        if (f8Frac > 0)
            f8ToRound = f8Int + ((f8Int < 0) ? -1.0 : 1.0F);
        else
            f8ToRound = f8Int;
    }
     else if (eType == tCIDLib::ERoundTypes::Down)
    {
        f8ToRound = f8Int;
    }
}


// ---------------------------------------------------------------------------
//  Geographical functions
// ---------------------------------------------------------------------------

//
//  Calculate the distance between two lat/long positions, in either miles or
//  kilometers.
//
inline tCIDLib::TFloat8 f8Haversin(const tCIDLib::TFloat8 f8Theta)
{
    return (1.0 - cos(f8Theta)) / 2;
}

tCIDLib::TCard4
TMathLib::c4GeoDistance(const   tCIDLib::TFloat8    f8LatSrc
                        , const tCIDLib::TFloat8    f8LongSrc
                        , const tCIDLib::TFloat8    f8LatTar
                        , const tCIDLib::TFloat8    f8LongTar
                        , const tCIDLib::TBoolean   bMiles)
{
    const tCIDLib::TFloat8 f8LatS = f8ToRadians(f8LatSrc);
    const tCIDLib::TFloat8 f8LongS = f8ToRadians(f8LongSrc);
    const tCIDLib::TFloat8 f8LatT = f8ToRadians(f8LatTar);
    const tCIDLib::TFloat8 f8LongT = f8ToRadians(f8LongTar);

    const tCIDLib::TFloat8 f8H
    (
        f8Haversin(f8LatT - f8LatS)
        + cos(f8LatS) * cos(f8LatT)
        * f8Haversin(f8LongT - f8LongS)
    );

    // Convert to miles or km
    tCIDLib::TFloat8 f8Ret;
    if (bMiles)
        f8Ret = 2.0 * 3959;
    else
        f8Ret = 2.0 * 6371.8;

    f8Ret *= f8ArcSine(f8SqrRoot(f8H));

    Round(f8Ret);
    return tCIDLib::TCard4(f8Ret);
}


// ---------------------------------------------------------------------------
//  Transcendental functions
// ---------------------------------------------------------------------------
tCIDLib::TFloat4 TMathLib::f4ArcCosine(const tCIDLib::TFloat4 f4Value)
{
    return acosf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8ArcCosine(const tCIDLib::TFloat8& f8Value)
{
    return acos(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4ArcSine(const tCIDLib::TFloat4 f4Value)
{
    return asinf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8ArcSine(const tCIDLib::TFloat8& f8Value)
{
    return asin(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4ArcTangent(const tCIDLib::TFloat4 f4Value)
{
    return atanf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8ArcTangent(const tCIDLib::TFloat8& f8Value)
{
    return atan(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4ArcTangentXY(  const   tCIDLib::TFloat4 f4X
                                            , const tCIDLib::TFloat4 f4Y)
{
    return atan2f(f4Y, f4X);
}

tCIDLib::TFloat8 TMathLib::f8ArcTangentXY(  const   tCIDLib::TFloat8& f8X
                                            , const tCIDLib::TFloat8& f8Y)
{
    return atan2(f8Y, f8X);
}


tCIDLib::TFloat4 TMathLib::f4Cosine(const tCIDLib::TFloat4 f4Value)
{
    return cosf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Cosine(const tCIDLib::TFloat8& f8Value)
{
    return cos(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4HyperCosine(const tCIDLib::TFloat4 f4Value)
{
    return coshf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8HyperCosine(const tCIDLib::TFloat8& f8Value)
{
    return cosh(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4HyperSine(const tCIDLib::TFloat4 f4Value)
{
    return sinhf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8HyperSine(const tCIDLib::TFloat8& f8Value)
{
    return sinh(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4HyperTangent(const tCIDLib::TFloat4 f4Value)
{
    return tanhf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8HyperTangent(const tCIDLib::TFloat8& f8Value)
{
    return tanh(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4Sine(const tCIDLib::TFloat4 f4Value)
{
    return sinf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Sine(const tCIDLib::TFloat8& f8Value)
{
    return sin(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4SqrRoot(const tCIDLib::TFloat4 f4Value)
{
    return sqrtf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8SqrRoot(const tCIDLib::TFloat8& f8Value)
{
    return sqrt(f8Value);
}


tCIDLib::TFloat4 TMathLib::f4Tangent(const tCIDLib::TFloat4 f4Value)
{
    return tanf(f4Value);
}

tCIDLib::TFloat8 TMathLib::f8Tangent(const tCIDLib::TFloat8& f8Value)
{
    return tan(f8Value);
}


