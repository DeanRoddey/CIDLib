//
// FILE NAME: CIDCrypto_SHA256.cpp
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
//  This file implements the TSHA256Hasher class.
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
//  Some core SHA256 functions, implemented as macros for convenience and
//  compactness.
// ---------------------------------------------------------------------------
#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA256_F1(x) (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))
#define SHA2_UNPACK32(x, str)                 \
{                                             \
    *((str) + 3) = (tCIDLib::TCard1) ((x)      );       \
    *((str) + 2) = (tCIDLib::TCard1) ((x) >>  8);       \
    *((str) + 1) = (tCIDLib::TCard1) ((x) >> 16);       \
    *((str) + 0) = (tCIDLib::TCard1) ((x) >> 24);       \
}
#define SHA2_PACK32(str, x)                   \
{                                             \
    *(x) =   ((tCIDLib::TCard4) *((str) + 3)      )    \
           | ((tCIDLib::TCard4) *((str) + 2) <<  8)    \
           | ((tCIDLib::TCard4) *((str) + 1) << 16)    \
           | ((tCIDLib::TCard4) *((str) + 0) << 24);   \
}

static const tCIDLib::TCard4 ac4K[kCIDCrypto::c4SHA256BlockSize] =
{
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5
    , 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5
    , 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3
    , 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
    , 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc
    , 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da
    , 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7
    , 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967
    , 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13
    , 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
    , 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3
    , 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070
    , 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5
    , 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3
    , 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208
    , 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSHA256Hasher,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSHA256Hasher
//  PREFIX: mdig
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSHA256Hasher: Constructors and Destructor
// ---------------------------------------------------------------------------
TSHA256Hasher::TSHA256Hasher() :

    THashDigest(kCIDCrypto::c4SHA256BlockSize)
    , m_c4ByteCnt(0)
    , m_c4PartialCnt(0)
{
    //
    //  Do an initial reset, so they can start calling Digest() with out
    //  a call to StartNew(), for a one time use hasher.
    //
    StartNew();
}

TSHA256Hasher::~TSHA256Hasher()
{
}


// ---------------------------------------------------------------------------
//  TSHA256Hasher: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TSHA256Hasher::Complete(TMsgHash& mhashToFill)
{
    // Process any trailing bytes and pad it out
    PadMsg();

    // And now we can get the hash out.
    tCIDLib::TCard1 ac1Buf[kCIDCrypto::c4SHA256HashBytes];

    //
    //  We store the data in big endian format in the buffer, so swap if we
    //  are on a little endian host.
    //
    #if defined(CIDLIB_LITTLEENDIAN)
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BufCnt; c4Index++)
        *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[c4Index * 4]) = TRawBits::c4SwapBytes(m_ac4H[c4Index]);
    #else
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BufCnt; c4Index++)
        *reinterpret_cast<tCIDLib::TCard4*>(&ac1Buf[c4Index * 4]) = m_ac4H[c4Index];
    #endif

    // Set up the caller's hash
    mhashToFill.Set(ac1Buf, kCIDCrypto::c4SHA256HashBytes);

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
TSHA256Hasher::DigestRaw( const   tCIDLib::TCard1* const  pc1ToDigest
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
        while ((c4Index < c4Bytes) && (m_c4PartialCnt < kCIDCrypto::c4SHA256BlockSize))
        {
            m_ac1Partial[m_c4PartialCnt++] = *pc1Cur++;
            m_c4ByteCnt++;
            c4Index++;
        }

        //
        //  If we didn't get a full block yet, just return and leave the
        //  info in the partial for next time.
        //
        if (m_c4PartialCnt < kCIDCrypto::c4SHA256BlockSize)
            return;

        // We got a full block so process and it and reset the partial count
        ProcessMsgBlock(m_ac1Partial);
        m_c4PartialCnt = 0;
    }

    // And now process the remaining full blocks
    while (c4Index < c4Bytes)
    {
        //
        //  We process a block of bytes at a time, so see if we have that many left
        //  If not, we have to pad a block at the end, so break out.
        //
        if (c4Index + kCIDCrypto::c4SHA256BlockSize > c4Bytes)
            break;

        // We can do another whole block
        ProcessMsgBlock(pc1Cur);

        // Move forward now by a whole block
        pc1Cur += kCIDCrypto::c4SHA256BlockSize;
        c4Index += kCIDCrypto::c4SHA256BlockSize;

        // Bump the count of bytes we've processed so far
        m_c4ByteCnt += kCIDCrypto::c4SHA256BlockSize;
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
TSHA256Hasher::DigestSrc(TBinInStream& strmSrc, const tCIDLib::TCard4 c4Bytes)
{
    tCIDLib::TCard4 c4Index = 0;

    //
    //  If we have partial stuff from the previous pass, handle that. We
    //  will try to fill up this partial buffer and process it. Add any
    //  consumed bytes to the overall byte count.
    //
    if (m_c4PartialCnt)
    {
        while ((c4Index < c4Bytes) && (m_c4PartialCnt < kCIDCrypto::c4SHA256BlockSize))
        {
            strmSrc >> m_ac1Partial[m_c4PartialCnt++];
            m_c4ByteCnt++;
            c4Index++;
        }

        //
        //  If we didn't get a full block yet, just return and leave the
        //  info in the partial for next time.
        //
        if (m_c4PartialCnt < kCIDCrypto::c4SHA256BlockSize)
            return;

        // We got a full block so process and it and reset the partial count
        ProcessMsgBlock(m_ac1Partial);
        m_c4PartialCnt = 0;
    }

    // And now process the remaining full blocks
    while (c4Index < c4Bytes)
    {
        //
        //  We process 64 bytes at a time, so see if we have that many left
        //  If not, we have to pad a block at the end, so break out.
        //
        if (c4Index + kCIDCrypto::c4SHA256BlockSize > c4Bytes)
            break;

        //
        //  We can do another whole block. We can use the partial buffer here
        //  as a temp since we know we have flushed out any partial at this point.
        //
        strmSrc.c4ReadRawBuffer(m_ac1Partial, kCIDCrypto::c4SHA256BlockSize);
        ProcessMsgBlock(m_ac1Partial);

        // Move forward now by a whole block
        c4Index += 64;

        // Bump the count of bytes we've processed so far
        m_c4ByteCnt += kCIDCrypto::c4SHA256BlockSize;
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


tCIDLib::TVoid TSHA256Hasher::StartNew()
{
    InitContext();
}



// ---------------------------------------------------------------------------
//  TSHA256Hasher: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Preps us for another round of hashing
tCIDLib::TVoid TSHA256Hasher::InitContext()
{
    m_ac4H[0] = 0x6a09e667;
    m_ac4H[1] = 0xbb67ae85;
    m_ac4H[2] = 0x3c6ef372;
    m_ac4H[3] = 0xa54ff53a;
    m_ac4H[4] = 0x510e527f;
    m_ac4H[5] = 0x9b05688c;
    m_ac4H[6] = 0x1f83d9ab;
    m_ac4H[7] = 0x5be0cd19;

    m_c4PartialCnt = 0;
    m_c4ByteCnt = 0;
}


//
//  Handles any final partial block and pads it out appropriately.
//
tCIDLib::TVoid TSHA256Hasher::PadMsg()
{
    //
    //  Start with the number of bytes we have left. If there's enough
    //  space left for the size to go in, we do one more final one. Else,
    //  we pad the partial and then do another to get the size in.
    //
    tCIDLib::TCard4 c4Index = m_c4PartialCnt;
    if (m_c4PartialCnt > 55)
    {
        // Not enough room, so pad one and start another
        m_ac1Partial[c4Index++] = 0x80;
        while(c4Index < kCIDCrypto::c4SHA256BlockSize)
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
        while(c4Index < 56)
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
tCIDLib::TVoid TSHA256Hasher::ProcessMsgBlock(const tCIDLib::TCard1* const pc1Block)
{
    tCIDLib::TCard4 ac4W[kCIDCrypto::c4SHA256BlockSize];
    tCIDLib::TCard4 ac4WV[c4BufCnt];
    tCIDLib::TCard4 c4T1, c4T2;

    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 16; c4Ind++)
    {
        SHA2_PACK32(&pc1Block[c4Ind << 2], &ac4W[c4Ind]);
    }

    for (tCIDLib::TCard4 c4Ind = 16; c4Ind < kCIDCrypto::c4SHA256BlockSize; c4Ind++)
    {
        ac4W[c4Ind] = SHA256_F4(ac4W[c4Ind -  2]) + ac4W[c4Ind -  7]
                      + SHA256_F3(ac4W[c4Ind - 15]) + ac4W[c4Ind - 16];
    }

    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4BufCnt; c4Ind++)
        ac4WV[c4Ind] = m_ac4H[c4Ind];

    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < kCIDCrypto::c4SHA256BlockSize; c4Ind++)
    {
        c4T1 = ac4WV[7] + SHA256_F2(ac4WV[4]) + SHA2_CH(ac4WV[4], ac4WV[5], ac4WV[6])
            + ac4K[c4Ind] + ac4W[c4Ind];
        c4T2 = SHA256_F1(ac4WV[0]) + SHA2_MAJ(ac4WV[0], ac4WV[1], ac4WV[2]);
        ac4WV[7] = ac4WV[6];
        ac4WV[6] = ac4WV[5];
        ac4WV[5] = ac4WV[4];
        ac4WV[4] = ac4WV[3] + c4T1;
        ac4WV[3] = ac4WV[2];
        ac4WV[2] = ac4WV[1];
        ac4WV[1] = ac4WV[0];
        ac4WV[0] = c4T1 + c4T2;
    }

    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4BufCnt; c4Ind++)
        m_ac4H[c4Ind] += ac4WV[c4Ind];
}


// Clean ou the context info for discretion
tCIDLib::TVoid TSHA256Hasher::ScrubContext()
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < tCIDLib::c4ArrayElems(m_ac4H); c4Index++)
        m_ac4H[c4Index] = 0;
}

