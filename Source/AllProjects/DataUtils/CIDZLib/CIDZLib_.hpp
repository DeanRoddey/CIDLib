//
// FILE NAME: CIDZLib_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/2004
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
//  This is the main internal header for the facility. It includes the main
//  public header, any internal headers that are needed, intrafacility function
//  prototypes, intrafacility constants, and macros.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our own public header and any internal headers we need
// ---------------------------------------------------------------------------
#include    "CIDZLib.hpp"


// ---------------------------------------------------------------------------
//  Include any needed internal headers
// ---------------------------------------------------------------------------
#include    "CIDZLib_MessageIds.hpp"


// ---------------------------------------------------------------------------
//  This is the intra-facilities types namespace. It contains non-class types
//  for use within the facility.
// ---------------------------------------------------------------------------
namespace tCIDZLib_
{
    // -----------------------------------------------------------------------
    //  Used to create some compiled data to initialize the static trees. The
    //  declarations of the class that uses these is seen publically so this
    //  structure must be visible, but the data itself is private and is in
    //  the CIDZLib_StaticTrees.hpp.
    // -----------------------------------------------------------------------
    struct TTreeVals
    {
        tCIDLib::TCard2 c2Val1;
        tCIDLib::TCard2 c2Val2;
    };


    // -----------------------------------------------------------------------
    //  Used to hold decoding operations data
    //
    //  c1Op bits are defined as:
    //      00000000 - Literal
    //      0000tttt - Table link, tttt != 0 is the number of table index bits
    //      0001eeee - Length or distance, eeee is the number of extra bits
    //      01100000 - End of block
    //      01000000 - Invalid code
    // -----------------------------------------------------------------------
    struct TCode
    {
        tCIDLib::TCard1 c1Op;
        tCIDLib::TCard1 c1Bits;
        tCIDLib::TCard2 c2Val;
    };


    // -----------------------------------------------------------------------
    //  A different compression function is used to optimize for various
    //  compression levels.
    // -----------------------------------------------------------------------
    enum class ECompFuncs
    {
        Store
        , Fast
        , Slow
    };


    // -----------------------------------------------------------------------
    //  The deflate compression method, of which Deflate is the only one
    //  currently defined.
    // -----------------------------------------------------------------------
    enum class ECompMethods
    {
        Deflated        = 8
    };


    // -----------------------------------------------------------------------
    //  The data type being compressed.
    // -----------------------------------------------------------------------
    enum class EDataTypes
    {
        Unknown
        , ASCII
        , Binary
    };


    // -----------------------------------------------------------------------
    //  The decompression states used in a state machine used by the Inflate
    //  method.
    //      Head     : i: Waiting for magic header
    //      DictId   : i: Waiting for dictionary check value
    //      Dict     : i: Waiting for inflateSetDictionary() call
    //      Type     : i: Waiting for type bits, including last-flag bit
    //      Stored   : i: Waiting for stored size (length and complement)
    //      Copy     : i/o: Waiting for input or output to copy stored block
    //      Table    : i: Waiting for dynamic block table lengths
    //      LenLens  : i: Waiting for code length code lengths
    //      CodeLens : i: Waiting for length/lit and distance code lengths
    //      Len      : i: Waiting for length/lit code
    //      LenExt   : i: Waiting for length extra bits
    //      Dist     : i: Waiting for distance code
    //      DistExt  : i: Waiting for distance extra bits
    //      Match    : o: Waiting for output space to copy string
    //      Lit      : o: Waiting for output space to write literal
    //      Check    : i: Waiting for 32-bit check value
    //      Done     : Finished check, done -- remain here until reset
    // -----------------------------------------------------------------------
    enum class EInfModes
    {
        Head
        , DictId
        , Dict
        , Type
        , Stored
        , Copy
        , Table
        , LenLens
        , CodeLens
        , Len
        , LenExt
        , Dist
        , DistExt
        , Match
        , Lit
        , Check
        , Done
    };


    // -----------------------------------------------------------------------
    //  Used by InflateTable, to indicate to it what type of table to inflate.
    // -----------------------------------------------------------------------
    enum class EInflTbls
    {
        Codes
        , Lens
        , Dists
    };


    // -----------------------------------------------------------------------
    //  The overall modes of operation of the zlib system
    // -----------------------------------------------------------------------
    enum class EModes
    {
        Compress
        , Decompress
    };


    // -----------------------------------------------------------------------
    //  The compression strategies
    // -----------------------------------------------------------------------
    enum class EStrategies
    {
        Default
        , Filtered
        , HuffmanOnly
        , RLE
    };
}


// ---------------------------------------------------------------------------
//  This is the intra-facility constants namespace
// ---------------------------------------------------------------------------
namespace kCIDZLib_
{
    // -----------------------------------------------------------------------
    //  The bits for the compression type in block headers
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1NoComp  = 0;
    const tCIDLib::TCard1   c1Fixed   = 1;
    const tCIDLib::TCard1   c1Dynamic = 2;
    const tCIDLib::TCard1   c1Reserved= 3;


    // -----------------------------------------------------------------------
    //  Some ZLib constants related to tree and code sizes
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4BitLenCodes   = 19;
    const tCIDLib::TCard2   c2EndBlock      = 256;
    const tCIDLib::TCard4   c4DistCodes     = 30;
    const tCIDLib::TCard4   c4DistCodeLen   = 512;
    const tCIDLib::TCard4   c4LengthCodes   = 29;
    const tCIDLib::TCard4   c4Literals      = 256;
    const tCIDLib::TCard4   c4LLCodes       = c4Literals + 1 + c4LengthCodes;
    const tCIDLib::TCard4   c4HeapSz        = (2 * c4LLCodes) + 1;
    const tCIDLib::TCard4   c4MaxBits       = 15;
    const tCIDLib::TCard4   c4MaxLenBits    = 7;
    const tCIDLib::TCard2   c2Rep_3_6       = 16;
    const tCIDLib::TCard2   c2RepZ_3_10     = 17;
    const tCIDLib::TCard2   c2RepZ_11_138   = 18;
    const tCIDLib::TCard4   c4TooFar        = 4096;


    // -----------------------------------------------------------------------
    //  We don't bother to support non-32K windows, so we can hard code the
    //  window info. We actually use a buffer twice the size of the sliding
    //  window size, since as a practical matter we don't want to constantly
    //  have to be sliding the data downwards as we move through it.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4WndMask       =  0x7FFF;
    const tCIDLib::TCard4   c4WndSz         =  0x8000;
    const tCIDLib::TCard4   c4WndBufSz      = 0x10000;
    const tCIDLib::TCard4   c4WndBits       = 15;


    // -----------------------------------------------------------------------
    //  Min and match strings we'll try to match and related values
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MinMatch      = 3;
    const tCIDLib::TCard4   c4MaxMatch      = 258;
    const tCIDLib::TCard4   c4MinLookAhead  = c4MaxMatch - c4MinMatch + 1;
    const tCIDLib::TCard4   c4MaxDist       = c4WndSz - c4MinLookAhead;


    // -----------------------------------------------------------------------
    //  Hash table related values
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4HashBits      = 15;
    const tCIDLib::TCard4   c4HashSz        = 1UL << c4HashBits;
    const tCIDLib::TCard4   c4HashMask      = 0x7FFF;
    const tCIDLib::TCard4   c4HashShift     = (c4HashBits + c4MinMatch - 1)
                                              / c4MinMatch;


    // -----------------------------------------------------------------------
    //  Some buffer sizes
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4LitBufSz      = 0x4000;


    // -----------------------------------------------------------------------
    //  The 'extra bits' arrays, for each possible length code, distance code,
    //  and bit length code.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   ac1ExtraLBits[c4LengthCodes] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2
      , 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
    };

    const tCIDLib::TCard1   ac1ExtraDBits[c4DistCodes] =
    {
        0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6
      , 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
    };

    const tCIDLib::TCard1   ac1ExtraLenBits[c4BitLenCodes] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7
    };

    const tCIDLib::TCard4   ac4BitLenOrder[c4BitLenCodes]=
    {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
    };


    // -----------------------------------------------------------------------
    //  We create a set of strategy info objects, one per compression level,
    //  which set various compression parameters to optimize for that level
    //  of compression.
    // -----------------------------------------------------------------------
    struct StratInfo
    {
        tCIDLib::TCard4         c4GoodLen;
        tCIDLib::TCard4         c4MaxLazyLen;
        tCIDLib::TCard4         c4NiceLen;
        tCIDLib::TCard4         c4MaxChainLen;
        tCIDZLib_::ECompFuncs   eFunc;
    };

    const StratInfo aStratTable[tCIDZLib::ECompLevels::Count] =
    {
        {  0,    0,   0,    0, tCIDZLib_::ECompFuncs::Store }
      , {  4,    4,   8,    4, tCIDZLib_::ECompFuncs::Fast }
      , {  4,    5,  16,    8, tCIDZLib_::ECompFuncs::Fast }
      , {  4,    6,  32,   32, tCIDZLib_::ECompFuncs::Fast }
      , {  4,    4,  16,   16, tCIDZLib_::ECompFuncs::Slow }
      , {  8,   16,  32,   32, tCIDZLib_::ECompFuncs::Slow }
      , {  8,   16, 128,  128, tCIDZLib_::ECompFuncs::Slow }
      , {  8,   32, 128,  256, tCIDZLib_::ECompFuncs::Slow }
      , { 32,  128, 258, 1024, tCIDZLib_::ECompFuncs::Slow }
      , { 32,  258, 258, 4096, tCIDZLib_::ECompFuncs::Slow }
    };
}


// ---------------------------------------------------------------------------
//  This header needs to see the constants above
// ---------------------------------------------------------------------------
#include    "CIDZLib_StaticTrees_.hpp"
#include    "CIDZLib_Tree_.hpp"
#include    "CIDZLib_CompImpl_.hpp"


// ---------------------------------------------------------------------------
//  Some simple inline methods for speed
// ---------------------------------------------------------------------------
inline tCIDLib::TCard1 c1DistCode(const tCIDLib::TCard4 c4Dist)
{
    if (c4Dist < 256)
        return ac1DistCode[c4Dist];
     return ac1DistCode[256 + (c4Dist >> 7)];
}

