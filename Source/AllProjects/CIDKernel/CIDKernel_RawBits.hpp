//
// FILE NAME: CIDKernel_RawBits.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/22/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_RawBits.Cpp file, which implements a
//  number of methods for manipulating bit fields. This stuff is all placed
//  within a namespace called TRawBits.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TRawBits
{
    // -----------------------------------------------------------------------
    //  Bit rotations for various integral types
    // -----------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TCard4 c4RotateLeft
    (
        const   tCIDLib::TCard4     c4Value
        , const tCIDLib::TCard4     c4RotateCount
    );

    KRNLEXPORT tCIDLib::TCard4 c4RotateRight
    (
        const   tCIDLib::TCard4     c4Value
        , const tCIDLib::TCard4     c4RotateCount
    );


    // -----------------------------------------------------------------------
    //  Extracts various fields from a 32 or 16 bit value
    // -----------------------------------------------------------------------
    inline tCIDLib::TCard1 c1Low8From16(const tCIDLib::TCard2 c2Val)
    {
        return tCIDLib::TCard1(c2Val & 0xFFUL);
    }

    inline tCIDLib::TCard1 c1High8From16(const tCIDLib::TCard2 c2Val)
    {
        return tCIDLib::TCard1((c2Val & 0xFF00UL) >> 8);
    }

    inline tCIDLib::TCard2 c2Low16From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TCard2(c4Val & 0xFFFFUL);
    }

    inline tCIDLib::TCard2 c2High16From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TCard2((c4Val & 0xFFFF0000UL) >> 16);
    }

    inline tCIDLib::TCard1 c1Low8From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TCard1(c4Val & 0xFFUL);
    }

    inline tCIDLib::TCard1 c1Sec8From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TCard1((c4Val & 0xFF00UL) >> 8);
    }

    inline tCIDLib::TCard1 c1Third8From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TCard1((c4Val & 0xFF0000UL) >> 16);
    }

    inline tCIDLib::TCard1 c1High8From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TCard1((c4Val & 0xFF000000UL) >> 24);
    }

    inline tCIDLib::TInt1 i1Low8From16(const tCIDLib::TCard2 c2Val)
    {
        return tCIDLib::TInt1(c2Val & 0xFFUL);
    }

    inline tCIDLib::TInt1 i1High8From16(const tCIDLib::TCard2 c2Val)
    {
        return tCIDLib::TInt1((c2Val & 0xFF00UL) >> 8);
    }
    inline tCIDLib::TInt2 i2Low16From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TInt2(c4Val & 0xFFFFUL);
    }

    inline tCIDLib::TInt2 i2High16From32(const tCIDLib::TCard4 c4Val)
    {
        return tCIDLib::TInt2((c4Val & 0xFFFF0000UL) >> 16);
    }

    inline tCIDLib::TCard4 c4Low32From64(const tCIDLib::TCard8& c8Val)
    {
        return tCIDLib::TCard4(c8Val & kCIDLib::c4MaxCard);
    }

    inline tCIDLib::TCard4 c4High32From64(const tCIDLib::TCard8& c8Val)
    {
        return tCIDLib::TCard4(c8Val >> 32);
    }


    // -----------------------------------------------------------------------
    //  Creates a 32 bit value from two 16 bit values or a 16 bit value from
    //  two 8 bit values or a 32 bit value from 4 8 bit values.
    // -----------------------------------------------------------------------
    inline tCIDLib::TCard2 c2From8( const   tCIDLib::TCard1 c1Low
                                    , const tCIDLib::TCard1 c1High)
    {
        return tCIDLib::TCard2
        (
            (tCIDLib::TCard2(c1High) << 8) | tCIDLib::TCard2(c1Low)
        );
    }

    inline tCIDLib::TCard4 c4From16(const   tCIDLib::TCard2 c2Low
                                    , const tCIDLib::TCard2 c2High)
    {
        return ((tCIDLib::TCard4(c2High) << 16) | c2Low);
    }

    inline tCIDLib::TCard8 c8From32(const   tCIDLib::TCard4 c4Low
                                    , const tCIDLib::TCard4 c4High)
    {
        tCIDLib::TCard8 c8Ret = c4High;
        c8Ret <<= 32;
        c8Ret |= c4Low;
        return c8Ret;
    }

    inline tCIDLib::TInt4 i4From16( const   tCIDLib::TInt2  i2Low
                                    , const tCIDLib::TInt2  i2High)
    {
        tCIDLib::TInt4 i4Ret = i2High;
        i4Ret <<= 16;
        i4Ret |= i2Low;
        return i4Ret;
    }

    inline tCIDLib::TInt4 i4From32( const   tCIDLib::TInt4  i4Low
                                    , const tCIDLib::TInt4  i4High)
    {
        tCIDLib::TInt4 i4Ret = i4High;
        i4Ret <<= 16;
        i4Ret |= i4Low;
        return i4Ret;
    }


    // -----------------------------------------------------------------------
    //  Swap byte order
    // -----------------------------------------------------------------------
    inline tCIDLib::TCard2 c2SwapBytes(const tCIDLib::TCard2 c2ToSwap)
    {
        return((c2ToSwap << 8) | (c2ToSwap >> 8));
    }

    inline tCIDLib::TCard4 c4SwapBytes(const tCIDLib::TCard4 c4Swap)
    {
        return
        (
            (c4Swap << 24)
            | ((c4Swap & 0x00FF0000) >> 8)
            | ((c4Swap & 0x0000FF00) << 8)
            | (c4Swap >> 24)
        );
    }

    KRNLEXPORT tCIDLib::TFloat4 f4SwapBytes
    (
        const   tCIDLib::TFloat4            f4ToSwap
        , const tCIDLib::TBoolean           bIsLittle
    );

    KRNLEXPORT tCIDLib::TFloat8 f8SwapBytes
    (
        const   tCIDLib::TFloat8            f8ToSwap
        , const tCIDLib::TBoolean           bIsLittle
    );

    inline tCIDLib::TInt2 i2SwapBytes(const tCIDLib::TInt2 i2ToSwap)
    {
        return tCIDLib::TInt2(c2SwapBytes(tCIDLib::TCard2(i2ToSwap)));
    }

    inline tCIDLib::TInt4 i4SwapBytes(const tCIDLib::TInt4 i4Swap)
    {
        return tCIDLib::TInt4(c4SwapBytes(tCIDLib::TCard4(i4Swap)));
    }

    inline tCIDLib::TCard8 c8SwapBytes(const tCIDLib::TCard8& c8Swap)
    {
        return
        (
            (c8Swap << 56)
            | ((c8Swap & 0x000000000000FF00) << 40)
            | ((c8Swap & 0x0000000000FF0000) << 24)
            | ((c8Swap & 0x00000000FF000000) << 8)
            | ((c8Swap & 0x000000FF00000000) >> 8)
            | ((c8Swap & 0x0000FF0000000000) >> 24)
            | ((c8Swap & 0x00FF000000000000) >> 40)
            | (c8Swap >> 56)
        );
    }

    inline tCIDLib::TInt8 i8SwapBytes(const tCIDLib::TInt8 i8Swap)
    {
        return tCIDLib::TInt8(c8SwapBytes(tCIDLib::TCard8(i8Swap)));
    }

    KRNLEXPORT tCIDLib::TVoid SwapCard4Array
    (
                tCIDLib::TCard4* const      pc4Data
        , const tCIDLib::TCard4             c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SwapInt4Array
    (
                tCIDLib::TInt4* const       pi4Data
        , const tCIDLib::TCard4             c4Count
    );



    // -----------------------------------------------------------------------
    //  Do swapping of the value based on our current endianness
    // -----------------------------------------------------------------------
    inline tCIDLib::TCard2 c2FromBigEndian(tCIDLib::TCard2 c2ToSwap)
    {
        #if defined(CIDLIB_BIGENDIAN)
        return c2ToSwap;
        #else
        tCIDLib::TCard2 c2Ret = tCIDLib::TCard2(c2ToSwap >> 8);
        c2Ret |= (c2ToSwap & 0xFF) << 8;
        return c2Ret;
        #endif
    }

    inline tCIDLib::TCard4 c4FromBigEndian(tCIDLib::TCard4 c4ToSwap)
    {
        #if defined(CIDLIB_BIGENDIAN)
        return c4ToSwap;
        #else
        tCIDLib::TCard4 c4Ret = c4ToSwap >> 24;
        c4Ret |= ((c4ToSwap >> 16) & 0xFF) << 8;
        c4Ret |= ((c4ToSwap >> 8) & 0xFF) << 16;
        c4Ret |= (c4ToSwap << 24);
        return c4Ret;
        #endif
    }

    inline tCIDLib::TCard2 c2FromLittleEndian(tCIDLib::TCard2 c2ToSwap)
    {
        #if defined(CIDLIB_LITTLEENDIAN)
        return c2ToSwap;
        #else
        tCIDLib::TCard2 c2Ret = tCIDLib::TCard2(c2ToSwap >> 8);
        c2Ret |= (c2ToSwap & 0xFF) << 8;
        return c2Ret;
        #endif
    }

    inline tCIDLib::TCard4 c4FromLittleEndian(tCIDLib::TCard4 c4ToSwap)
    {
        #if defined(CIDLIB_LITTLEENDIAN)
        return c4ToSwap;
        #else
        tCIDLib::TCard4 c4Ret = c4ToSwap >> 24;
        c4Ret |= ((c4ToSwap >> 16) & 0xFF) << 8;
        c4Ret |= ((c4ToSwap >> 8) & 0xFF) << 16;
        c4Ret |= (c4ToSwap << 24);
        return c4Ret;
        #endif
    }
}

