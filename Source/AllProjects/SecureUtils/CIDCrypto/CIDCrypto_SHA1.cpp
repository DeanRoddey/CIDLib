//
// FILE NAME: CIDCrypto_SHA1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/19/2009
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TSHA1Hasher class.
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
#include    "CIDCrypto_.hpp"



// ---------------------------------------------------------------------------
//  Some core SHA1 functions, implemented as macros for convenience and
//  compactness.
// ---------------------------------------------------------------------------
static inline tCIDLib::TCard4
c4CircularShift(const tCIDLib::TCard4 c4Bits, const tCIDLib::TCard4 c4Word)
{
    return ((c4Word << c4Bits) & 0xFFFFFFFF)
            | ((c4Word & 0xFFFFFFFF) >> (32 - c4Bits));
}



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSHA1Hasher,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSHA1Hasher
//  PREFIX: mdig
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSHA1Hasher: Constructors and Destructor
// ---------------------------------------------------------------------------
TSHA1Hasher::TSHA1Hasher() :

    THashDigest(c4BlockSz)
    , m_c4ByteCnt(0)
    , m_c4PartialCnt(0)
{
    //
    //  Do an initial reset, so they can start calling Digest() with out
    //  a call to StartNew(), for a one time use hasher.
    //
    StartNew();
}

TSHA1Hasher::~TSHA1Hasher()
{
}


// ---------------------------------------------------------------------------
//  TSHA1Hasher: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TSHA1Hasher::Complete(TMsgHash& mhashToFill)
{
    // Process any trailing bytes and padd it out
    PadMsg();

    //
    //  And now we can get the hash out. We have to build it up as a byte
    //  array, since that's how the hashes are set. So go through the 5
    //  accumulators and lay them out as bytes.
    //
    tCIDLib::TCard1 ac1Buf[20];

    //
    //  We store the data in big endian format in the buffer, so swap if we
    //  are on a little endian host.
    //
    #if defined(CIDLIB_LITTLEENDIAN)
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[0]) = TRawBits::c4SwapBytes(m_ac4H[0]);
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[4]) = TRawBits::c4SwapBytes(m_ac4H[1]);
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[8]) = TRawBits::c4SwapBytes(m_ac4H[2]);
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[12]) = TRawBits::c4SwapBytes(m_ac4H[3]);
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[16]) = TRawBits::c4SwapBytes(m_ac4H[4]);
    #else
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[0]) = m_ac4H[0];
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[4]) = m_ac4H[1];
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[8]) = m_ac4H[2];
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[12]) = m_ac4H[3];
    *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[16]) = m_ac4H[4];
    #endif

    // Set up the caller's hash
    mhashToFill.Set(ac1Buf, kCIDCrypto::c4SHA1HashBytes);

    // And scrub the context info
    ScrubContext();
}


//
//  For efficiency we have two separate methods. One takes a raw buffer the other
//  takes a stream. For this algorithm, support both makes a big difference sice
//  this one can process directly out of the source buffer. So the raw version
//  doesn't have to copy each block, whereas the stream version does.
//
tCIDLib::TVoid
TSHA1Hasher::DigestRaw( const   tCIDLib::TCard1* const  pc1ToDigest
                        , const tCIDLib::TCard4         c4Bytes)
{
    const tCIDLib::TCard1* pc1Cur = pc1ToDigest;
    tCIDLib::TCard4 c4Index = 0;

    //
    //  If we have partial stuff from the previous pass, handle that. We
    //  will try to fill up this partial buffer and process it. Add any
    //  consumed bytes to the overall byte count.
    //
    if (m_c4PartialCnt)
    {
        while ((c4Index < c4Bytes) && (m_c4PartialCnt < c4BlockSz))
        {
            m_ac1Partial[m_c4PartialCnt++] = *pc1Cur++;
            m_c4ByteCnt++;
            c4Index++;
        }

        //
        //  If we didn't get a full block yet, just return and leave the
        //  info in the partial for next time.
        //
        if (m_c4PartialCnt < c4BlockSz)
            return;

        // We got a full block so process and it and reset the partial count
        ProcessMsgBlock(m_ac1Partial);
        m_c4PartialCnt = 0;
    }

    // And now process the remaining full blocks
    while (c4Index < c4Bytes)
    {
        //
        //  We process blocksz bytes at a time, so see if we have that many left
        //  If not, we have to pad a block at the end, so break out.
        //
        if (c4Index + c4BlockSz > c4Bytes)
            break;

        // We can do another whole block
        ProcessMsgBlock(pc1Cur);

        // Move forward now by a whole block
        pc1Cur += c4BlockSz;
        c4Index += c4BlockSz;

        // Bump the count of bytes we've processed so far
        m_c4ByteCnt += c4BlockSz;
    }

    //
    //  Store any final partial buffer. Add them to the byte count as
    //  well since we'll process them eventually.
    //
    m_c4PartialCnt = c4Bytes - c4Index;
    if (m_c4PartialCnt)
    {
        TRawMem::CopyMemBuf(m_ac1Partial, pc1Cur, m_c4PartialCnt);
        m_c4ByteCnt += m_c4PartialCnt;
    }
}


tCIDLib::TVoid
TSHA1Hasher::DigestSrc(TBinInStream& strmSrc, const tCIDLib::TCard4 c4Bytes)
{
    tCIDLib::TCard4 c4Index = 0;

    //
    //  If we have partial stuff from the previous pass, handle that. We
    //  will try to fill up this partial buffer and process it. Add any
    //  consumed bytes to the overall byte count.
    //
    if (m_c4PartialCnt)
    {
        while ((c4Index < c4Bytes) && (m_c4PartialCnt < c4BlockSz))
        {
            strmSrc >> m_ac1Partial[m_c4PartialCnt++];
            m_c4ByteCnt++;
            c4Index++;
        }

        //
        //  If we didn't get a full block yet, just return and leave the
        //  info in the partial for next time.
        //
        if (m_c4PartialCnt < c4BlockSz)
            return;

        // We got a full block so process and it and reset the partial count
        ProcessMsgBlock(m_ac1Partial);
        m_c4PartialCnt = 0;
    }

    // And now process the remaining full blocks
    while (c4Index < c4Bytes)
    {
        //
        //  We process blocksz bytes at a time, so see if we have that many left
        //  If not, we have to pad a block at the end, so break out.
        //
        if (c4Index + c4BlockSz > c4Bytes)
            break;

        //
        //  We can do another whole block. We can use the partial buffer here
        //  as a temp since we know we have flushed out any partial at this point.
        //
        strmSrc.c4ReadRawBuffer(m_ac1Partial, c4BlockSz);
        ProcessMsgBlock(m_ac1Partial);

        // Move forward now by a whole block
        c4Index += c4BlockSz;

        // Bump the count of bytes we've processed so far
        m_c4ByteCnt += c4BlockSz;
    }

    //
    //  Store any final partial buffer. Add them to the byte count as
    //  well since we'll process them eventually.
    //
    m_c4PartialCnt = c4Bytes - c4Index;
    if (m_c4PartialCnt)
    {
        strmSrc.c4ReadRawBuffer(m_ac1Partial, m_c4PartialCnt);
        m_c4ByteCnt += m_c4PartialCnt;
    }
}


tCIDLib::TVoid TSHA1Hasher::StartNew()
{
    InitContext();
}



// ---------------------------------------------------------------------------
//  TSHA1Hasher: Protected, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TSHA1Hasher::InitContext()
{
    m_ac4H[0] = 0x67452301;
    m_ac4H[1] = 0xEFCDAB89;
    m_ac4H[2] = 0x98BADCFE;
    m_ac4H[3] = 0x10325476;
    m_ac4H[4] = 0xC3D2E1F0;

    // Reset counts
    m_c4ByteCnt = 0;
    m_c4PartialCnt = 0;
}


//
//  Handles any final partial block and pads it out appropriately.
//
tCIDLib::TVoid TSHA1Hasher::PadMsg()
{
    //
    //  Start with the number of bytes we have left. If there's enough
    //  space left for the size to go in, we do one more final one. Else,
    //  we padd the partial and then do another to get the size in.
    //
    tCIDLib::TCard4 c4Index = m_c4PartialCnt;
    if (m_c4PartialCnt > 55)
    {
        // Not enough room, so pad one and start another
        m_ac1Partial[c4Index++] = 0x80;
        while(c4Index < c4BlockSz)
            m_ac1Partial[c4Index++] = 0;
        ProcessMsgBlock(m_ac1Partial);

        // Fill up another up to the point where we put in the length
        c4Index = 0;
        while (c4Index < 56)
            m_ac1Partial[c4Index++] = 0;
    }
     else
    {
        // We have room in the last partial one for the length
        m_ac1Partial[c4Index++] = 0x80;
        while (c4Index < 56)
            m_ac1Partial[c4Index++] = 0;
    }

    //
    //  Store the message length as the last 8 bytes. We only support
    //  lengths of 32 bit, so the first 4 will be zero always. We are
    //  storing bit length here, not byte length.
    //
    tCIDLib::TCard4 c4BitCnt = m_c4ByteCnt * 8;
    m_ac1Partial[56] = 0;
    m_ac1Partial[57] = 0;
    m_ac1Partial[58] = 0;
    m_ac1Partial[59] = 0;
    m_ac1Partial[60] = tCIDLib::TCard1((c4BitCnt >> 24) & 0xFF);
    m_ac1Partial[61] = tCIDLib::TCard1((c4BitCnt >> 16) & 0xFF);
    m_ac1Partial[62] = tCIDLib::TCard1((c4BitCnt >> 8) & 0xFF);
    m_ac1Partial[63] = tCIDLib::TCard1((c4BitCnt ) & 0xFF);

    // And spit out this last one
    ProcessMsgBlock(m_ac1Partial);
    m_c4PartialCnt = 0;
}


//
//  Handles the full blocks. The last block will need to be padded out if
//  partial.
//
tCIDLib::TVoid
TSHA1Hasher::ProcessMsgBlock(const tCIDLib::TCard1* const pc1Block)
{
    static const tCIDLib::TCard4 ac4K[] =
    {
        0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
    };
    tCIDLib::TCard4 ac4WSeq[80];
    tCIDLib::TCard4 c4Index;
    tCIDLib::TCard4 c4Temp;
    tCIDLib::TCard4 c4A, c4B, c4C, c4D, c4E;

    // Initialize the first 16 words in the array word seq array
    for (c4Index = 0; c4Index < 16; c4Index++)
    {
        ac4WSeq[c4Index] = ((tCIDLib::TCard4) pc1Block[c4Index * 4]) << 24;
        ac4WSeq[c4Index] |= ((tCIDLib::TCard4) pc1Block[c4Index * 4 + 1]) << 16;
        ac4WSeq[c4Index] |= ((tCIDLib::TCard4) pc1Block[c4Index * 4 + 2]) << 8;
        ac4WSeq[c4Index] |= ((tCIDLib::TCard4) pc1Block[c4Index * 4 + 3]);
    }

    for(c4Index = 16; c4Index < 80; c4Index++)
    {
        ac4WSeq[c4Index] = c4CircularShift
        (
            1
            , ac4WSeq[c4Index - 3] ^ ac4WSeq[c4Index - 8]
               ^ ac4WSeq[c4Index - 14] ^ ac4WSeq[c4Index - 16]
        );
    }

    c4A = m_ac4H[0];
    c4B = m_ac4H[1];
    c4C = m_ac4H[2];
    c4D = m_ac4H[3];
    c4E = m_ac4H[4];

    for (c4Index = 0; c4Index < 20; c4Index++)
    {
        c4Temp = c4CircularShift(5, c4A) + ((c4B & c4C) | ((~c4B) & c4D)) + c4E + ac4WSeq[c4Index] + ac4K[0];
        c4Temp &= 0xFFFFFFFF;
        c4E = c4D;
        c4D = c4C;
        c4C = c4CircularShift(30,c4B);
        c4B = c4A;
        c4A = c4Temp;
    }

    for (c4Index = 20; c4Index < 40; c4Index++)
    {
        c4Temp = c4CircularShift(5, c4A) + (c4B ^ c4C ^ c4D) + c4E + ac4WSeq[c4Index] + ac4K[1];
        c4Temp &= 0xFFFFFFFF;
        c4E = c4D;
        c4D = c4C;
        c4C = c4CircularShift(30,c4B);
        c4B = c4A;
        c4A = c4Temp;
    }

    for (c4Index = 40; c4Index < 60; c4Index++)
    {
        c4Temp = c4CircularShift(5 , c4A)
               + ((c4B & c4C) | (c4B & c4D) | (c4C & c4D)) + c4E + ac4WSeq[c4Index] + ac4K[2];
        c4Temp &= 0xFFFFFFFF;
        c4E = c4D;
        c4D = c4C;
        c4C = c4CircularShift(30,c4B);
        c4B = c4A;
        c4A = c4Temp;
    }

    for (c4Index = 60; c4Index < 80; c4Index++)
    {
        c4Temp = c4CircularShift(5, c4A) + (c4B ^ c4C ^ c4D) + c4E + ac4WSeq[c4Index] + ac4K[3];
        c4Temp &= 0xFFFFFFFF;
        c4E = c4D;
        c4D = c4C;
        c4C = c4CircularShift(30,c4B);
        c4B = c4A;
        c4A = c4Temp;
    }

    m_ac4H[0] = (m_ac4H[0] + c4A) & 0xFFFFFFFF;
    m_ac4H[1] = (m_ac4H[1] + c4B) & 0xFFFFFFFF;
    m_ac4H[2] = (m_ac4H[2] + c4C) & 0xFFFFFFFF;
    m_ac4H[3] = (m_ac4H[3] + c4D) & 0xFFFFFFFF;
    m_ac4H[4] = (m_ac4H[4] + c4E) & 0xFFFFFFFF;
}


// Clean ou the context info for discretion
tCIDLib::TVoid TSHA1Hasher::ScrubContext()
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < tCIDLib::c4ArrayElems(m_ac4H); c4Index++)
        m_ac4H[c4BufCnt] = 0;
}
