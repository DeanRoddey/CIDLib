//
// FILE NAME: CIDMath_Lib.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/02/1994
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file just maps the C runtime math routines to different names that
//  meet the CIDLib naming standards.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TMathLib
{
    // ------------------------------------------------------------------------
    //  Coordinate functions
    // ------------------------------------------------------------------------
    inline tCIDLib::TFloat4 f4ToDegrees(const tCIDLib::TFloat4 f4Radians)
    {
        return f4Radians * (180.0F / kCIDLib::f4PI);
    }

    inline tCIDLib::TFloat4 f4ToRadians(const tCIDLib::TFloat4 f4Degrees)
    {
        return f4Degrees * (kCIDLib::f4PI / 180.0F);
    }

    inline tCIDLib::TFloat8 f8ToDegrees(const tCIDLib::TFloat8& f8Radians)
    {
        return f8Radians * (180.0 / kCIDLib::f8PI);
    }

    inline tCIDLib::TFloat8 f8ToRadians(const tCIDLib::TFloat8& f8Degrees)
    {
        return f8Degrees * (kCIDLib::f8PI / 180.0);
    }



    // ------------------------------------------------------------------------
    //  Log functions
    // ------------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TFloat4 f4Log10(const tCIDLib::TFloat4 f8Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Log10(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4Log(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Log(const tCIDLib::TFloat8& f8Value);


    // ------------------------------------------------------------------------
    //  Geometrical functions
    // ------------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TFloat4 f4Hypotenuse
    (
        const   tCIDLib::TFloat4    f4X
        , const tCIDLib::TFloat4    f4Y
    );

    KRNLEXPORT tCIDLib::TFloat8 f8Hypotenuse
    (
        const   tCIDLib::TFloat8&   f8X
        , const tCIDLib::TFloat8&   f8Y
    );


    // ------------------------------------------------------------------------
    //  Miscellaneous functions
    // ------------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TFloat4 f4Abs(const tCIDLib::TFloat4& f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Abs(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4Ceil(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Ceil(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4Exponent(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Exponent(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4Floor(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Floor(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat8 f8MakeReal
    (
        const   tCIDLib::TFloat8&   f8Mantissa
        , const tCIDLib::TInt4      i4Exponent
    );

    KRNLEXPORT tCIDLib::TFloat4 f4Mod
    (
        const   tCIDLib::TFloat4    f4Value
        , const tCIDLib::TFloat4    f4Divisor
    );

    KRNLEXPORT tCIDLib::TFloat8 f8Mod
    (
        const   tCIDLib::TFloat8&   f8Value
        , const tCIDLib::TFloat8&   f8Divisor
    );

    KRNLEXPORT tCIDLib::TFloat4 f4Power
    (
        const   tCIDLib::TFloat4    f4Value
        , const tCIDLib::TFloat4    f4Power
    );

    KRNLEXPORT tCIDLib::TFloat8 f8Power
    (
        const   tCIDLib::TFloat8&   f8Value
        , const tCIDLib::TFloat8&   f8Power
    );

    KRNLEXPORT tCIDLib::TFloat4 f4Split
    (
        const   tCIDLib::TFloat4    f4Value
        ,       tCIDLib::TFloat4&   f4Integral
    );

    KRNLEXPORT tCIDLib::TFloat8 f8Split
    (
        const   tCIDLib::TFloat8&   f8Value
        ,       tCIDLib::TFloat8&   f8Integral
    );

    KRNLEXPORT tCIDLib::TInt4 i4Floor(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TInt4 i4Floor(const tCIDLib::TFloat8& f8Value);


    // ------------------------------------------------------------------------
    //  Rounding functions
    // ------------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TCard4 c4Round
    (
        const   tCIDLib::TFloat8    f8ToRound
    );

    KRNLEXPORT tCIDLib::TInt4 i4Round
    (
        const   tCIDLib::TFloat8    f8ToRound
    );

    KRNLEXPORT tCIDLib::TVoid Round
    (
                tCIDLib::TFloat4&   f4ToRound
        , const tCIDLib::ERoundTypes eType = tCIDLib::ERoundTypes::Closest
    );

    KRNLEXPORT tCIDLib::TVoid Round
    (
                tCIDLib::TFloat8&   f8ToRound
        , const tCIDLib::ERoundTypes eType = tCIDLib::ERoundTypes::Closest
    );


    // ------------------------------------------------------------------------
    //  Geographical
    // ------------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TCard4 c4GeoDistance
    (
        const   tCIDLib::TFloat8    f8LatSrc
        , const tCIDLib::TFloat8    f8LongSrc
        , const tCIDLib::TFloat8    f8LatTar
        , const tCIDLib::TFloat8    f8LongTar
        , const tCIDLib::TBoolean   bMiles
    );


    // ------------------------------------------------------------------------
    //  Transcendental functions
    // ------------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TFloat4 f4ArcCosine(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8ArcCosine(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4ArcSine(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat4 f4ArcSine(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8ArcSine(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4ArcTangent(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8ArcTangent(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4ArcTangentXY
    (
        const   tCIDLib::TFloat4    f4X
        , const tCIDLib::TFloat4    f4Y
    );

    KRNLEXPORT tCIDLib::TFloat8 f8ArcTangentXY
    (
        const   tCIDLib::TFloat8&   f8X
        , const tCIDLib::TFloat8&   f8Y
    );

    KRNLEXPORT tCIDLib::TFloat4 f4Cosine(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Cosine(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4HyperCosine(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8HyperCosine(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4HyperSine(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8HyperSine(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4HyperTangent(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8HyperTangent(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4Sine(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Sine(const tCIDLib::TFloat8& f8Value);

    inline tCIDLib::TFloat4 f4Square(const tCIDLib::TFloat4 f4Value)
    {
        return f4Value * f4Value;
    }

    inline tCIDLib::TFloat8 f8Square(const tCIDLib::TFloat8& f8Value)
    {
        return f8Value * f8Value;
    }

    KRNLEXPORT tCIDLib::TFloat4 f4SqrRoot(const tCIDLib::TFloat4 f8Value);

    KRNLEXPORT tCIDLib::TFloat8 f8SqrRoot(const tCIDLib::TFloat8& f8Value);

    KRNLEXPORT tCIDLib::TFloat4 f4Tangent(const tCIDLib::TFloat4 f4Value);

    KRNLEXPORT tCIDLib::TFloat8 f8Tangent(const tCIDLib::TFloat8& f8Value);
}


