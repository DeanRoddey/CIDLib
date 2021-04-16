//
// FILE NAME: CIDCrypto_MD5.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/23/1997
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
//  This file implements the TMessageDigest5 class.
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


namespace CIDCrypto_MD5
{
    namespace
    {
        // -------------------------------------------------------------------
        //  Local constant data
        // -------------------------------------------------------------------
        constexpr tCIDLib::TCard4    c4S11 = 7;
        constexpr tCIDLib::TCard4    c4S12 = 12;
        constexpr tCIDLib::TCard4    c4S13 = 17;
        constexpr tCIDLib::TCard4    c4S14 = 22;
        constexpr tCIDLib::TCard4    c4S21 = 5;
        constexpr tCIDLib::TCard4    c4S22 = 9;
        constexpr tCIDLib::TCard4    c4S23 = 14;
        constexpr tCIDLib::TCard4    c4S24 = 20;
        constexpr tCIDLib::TCard4    c4S31 = 4;
        constexpr tCIDLib::TCard4    c4S32 = 11;
        constexpr tCIDLib::TCard4    c4S33 = 16;
        constexpr tCIDLib::TCard4    c4S34 = 23;
        constexpr tCIDLib::TCard4    c4S41 = 6;
        constexpr tCIDLib::TCard4    c4S42 = 10;
        constexpr tCIDLib::TCard4    c4S43 = 15;
        constexpr tCIDLib::TCard4    c4S44 = 21;

        constexpr tCIDLib::TCard1    ac1Padding[64] =
        {
            0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };
    }
}


// ---------------------------------------------------------------------------
//  Basic MD5 functions, implemented as macros for convenience and
//  compactness.
// ---------------------------------------------------------------------------
#define F(x,y,z)    (((x) & (y)) | ((~x) & (z)))
#define G(x,y,z)    (((x) & (z)) | ((y) & (~z)))
#define H(x,y,z)    ((x) ^ (y) ^ (z))
#define I(x,y,z)    ((y) ^ ((x) | (~z)))

#define FF(a, b, c, d, x, s, ac) \
{ \
    (a) += F ((b), (c), (d)) + (x) + tCIDLib::TCard4(ac); \
    (a) = TRawBits::c4RotateLeft(a, s); \
    (a) += (b); \
}

#define GG(a, b, c, d, x, s, ac) \
{ \
    (a) += G ((b), (c), (d)) + (x) + tCIDLib::TCard4(ac); \
    (a) = TRawBits::c4RotateLeft(a, s); \
    (a) += (b); \
}

#define HH(a, b, c, d, x, s, ac) \
{ \
    (a) += H ((b), (c), (d)) + (x) + tCIDLib::TCard4(ac); \
    (a) = TRawBits::c4RotateLeft(a, s); \
    (a) += (b); \
}

#define II(a, b, c, d, x, s, ac) \
{ \
    (a) += I ((b), (c), (d)) + (x) + tCIDLib::TCard4(ac); \
    (a) = TRawBits::c4RotateLeft(a, s); \
    (a) += (b); \
}


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMessageDigest5,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TMessageDigest5
//  PREFIX: mdig
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMessageDigest5: Constructors and Destructor
// ---------------------------------------------------------------------------
TMessageDigest5::TMessageDigest5() :

    THashDigest(TMessageDigest5::c4BlockLen)
{
    //
    //  Do an initial init, in case they start calling Digest without
    //  first calling StartNew().
    //
    StartNew();
}

TMessageDigest5::~TMessageDigest5()
{
}


// ---------------------------------------------------------------------------
//  TMessageDigest5: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TMessageDigest5::Complete(TMsgHash& mhashToFill)
{
    // Finish the work and get the raw hash out into a local array
    tCIDLib::TCard1 ac1RawHash[TMessageDigest5::c4HashBytes];
    Complete(ac1RawHash);

    //
    //  Set the collected data on the passed hash. If its not of the correct
    //  size, it will throw.
    //
    mhashToFill.Set(ac1RawHash, TMessageDigest5::c4HashBytes);
}


//
//  To be efficient we have one that takes a raw buffer and another that takes a
//  stream. For MD5, since we can't directly process out of the incoming buffer,
//  there's not that much difference in efficiency, since both have to copy out
//  each block and process it. In others it makes a bigger difference.
//
tCIDLib::TVoid
TMessageDigest5::DigestRaw( const   tCIDLib::TCard1* const  pc1Input
                            , const tCIDLib::TCard4         c4Bytes)
{
    CIDAssert(pc1Input != nullptr, L"Null input to DigestRaw");

    // Compute number of bytes mod block size
    tCIDLib::TCard4 c4Index = tCIDLib::TCard4((m_ac4Count[0] >> 3) & 0x3F);

    // Update number of bits
    m_ac4Count[0] += tCIDLib::TCard4(c4Bytes << 3);
    if (m_ac4Count[0] < (c4Bytes << 3))
        m_ac4Count[1]++;
    m_ac4Count[1] += (c4Bytes >> 29);

    const tCIDLib::TCard4 c4PartLen = TMessageDigest5::c4BlockLen - c4Index;

    // Transform as many times as possible.
    tCIDLib::TCard4  c4XIndex = 0;
    if (c4Bytes >= c4PartLen)
    {
        TRawMem::CopyMemBuf(&m_ac1Buffer[c4Index], pc1Input, c4PartLen);
        Transform(m_ac1Buffer);

        for (c4XIndex = c4PartLen; c4XIndex + 63 < c4Bytes; c4XIndex += 64)
            Transform(&pc1Input[c4XIndex]);

        c4Index = 0;
    }
     else
    {
        c4XIndex = 0;
    }

    // Buffer remaining input if there is any
    if (c4Bytes - c4XIndex)
        TRawMem::CopyMemBuf(&m_ac1Buffer[c4Index], &pc1Input[c4XIndex], c4Bytes - c4XIndex);
}

tCIDLib::TVoid
TMessageDigest5::DigestSrc(TBinInStream& strmSrc, const tCIDLib::TCard4 c4Bytes)
{
    // Compute number of bytes mod block size
    tCIDLib::TCard4 c4Index = tCIDLib::TCard4((m_ac4Count[0] >> 3) & 0x3F);

    // Update number of bits
    m_ac4Count[0] += tCIDLib::TCard4(c4Bytes << 3);
    if (m_ac4Count[0] < (c4Bytes << 3))
        m_ac4Count[1]++;
    m_ac4Count[1] += (c4Bytes >> 29);

    // How much we need to complete any current partial block
    const tCIDLib::TCard4 c4PartLen = TMessageDigest5::c4BlockLen - c4Index;

    // Do as much as we can. Remember how much is left
    tCIDLib::TCard4  c4XCnt = 0;
    if (c4Bytes >= c4PartLen)
    {
        // Handle any current partial data
        strmSrc.c4ReadRawBuffer(&m_ac1Buffer[c4Index], c4PartLen);
        Transform(m_ac1Buffer);

        //
        //  Do any more full blocks. We can use the partial buffer for this since
        //  we know it's not used right now. We just dealt with any partial and
        //  will be resettig the index back to zero.
        //
        const tCIDLib::TCard4 c4FullCnt
        (
            (c4Bytes - c4PartLen) / TMessageDigest5::c4BlockLen
        );
        for (tCIDLib::TCard4 c4BlkInd = 0; c4BlkInd < c4FullCnt; c4BlkInd++)
        {
            strmSrc.c4ReadRawBuffer(m_ac1Buffer, TMessageDigest5::c4BlockLen);
            Transform(m_ac1Buffer);
        }

        // Partial index is now back to the start
        c4Index = 0;

        // And calculate now many bytes are left
        c4XCnt = c4Bytes - ((c4FullCnt * TMessageDigest5::c4BlockLen) + c4PartLen);
    }
     else
    {
        // Not enough to do another block, so it's all extra
        c4XCnt = c4Bytes;
    }

    // Buffer remaining input if there is any
    if (c4XCnt)
        strmSrc.c4ReadRawBuffer(&m_ac1Buffer[c4Index], c4XCnt);
}


//
//  Prep us to start hashing new data
//
tCIDLib::TVoid TMessageDigest5::StartNew()
{
    InitContext();
}


// ---------------------------------------------------------------------------
//  TMessageDigest5: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMessageDigest5::Complete(tCIDLib::TCard1* const pc1Digest)
{
    // Save number of bits
    tCIDLib::TCard1   ac1Bits[8] = {0};
    Encode(m_ac4Count, ac1Bits, 8);

    // Pad out to 56 mod the block size
    const tCIDLib::TCard4 c4Index = tCIDLib::TCard4((m_ac4Count[0] >> 3) & 0x3F);
    const tCIDLib::TCard4 c4PadLen = (c4Index < 56) ? (56 - c4Index) : (120 - c4Index);
    DigestRaw(CIDCrypto_MD5::ac1Padding, c4PadLen);

    // Append length (before padding)
    DigestRaw(ac1Bits, 8);

    // Store state in digest
    Encode(m_ac4State, pc1Digest, kCIDCrypto::c4MD5HashBytes);

    // Zero out sensitive information
    ScrubContext();
}


tCIDLib::TVoid
TMessageDigest5::Decode(const   tCIDLib::TCard1* const  pc1Input
                        ,       tCIDLib::TCard4* const  pc4Output
                        , const tCIDLib::TCard4         c4InputLen)
{
    // If debugging, do a little precondition checking
    CIDAssert((c4InputLen % 4) == 0, L"MD5 block must be a multiple of 4 bytes");

    for (tCIDLib::TCard4 c4InInd = 0, c4OutInd = 0; c4InInd < c4InputLen; c4OutInd++, c4InInd += 4)
    {
        pc4Output[c4OutInd] =   tCIDLib::TCard4(pc1Input[c4InInd])
                                | (tCIDLib::TCard4(pc1Input[c4InInd+1]) << 8)
                                | (tCIDLib::TCard4(pc1Input[c4InInd+2]) << 16)
                                | (tCIDLib::TCard4(pc1Input[c4InInd+3]) << 24);
    }
}


tCIDLib::TVoid
TMessageDigest5::Encode(const   tCIDLib::TCard4* const  pc4Input
                        ,       tCIDLib::TCard1* const  pc1Output
                        , const tCIDLib::TCard4         c4InputLen)
{
    // If debugging, do a little precondition checking
    CIDAssert((c4InputLen % 4) == 0, L"MD5 block must be a multiple of 4 bytes");

    for (tCIDLib::TCard4 c4InInd = 0, c4OutInd = 0; c4OutInd < c4InputLen; c4InInd++, c4OutInd += 4)
    {
        pc1Output[c4OutInd] = tCIDLib::TCard1(pc4Input[c4InInd] & 0xFF);
        pc1Output[c4OutInd + 1] = tCIDLib::TCard1((pc4Input[c4InInd] >> 8) & 0xFF);
        pc1Output[c4OutInd + 2] = tCIDLib::TCard1((pc4Input[c4InInd] >> 16) & 0xFF);
        pc1Output[c4OutInd + 3] = tCIDLib::TCard1((pc4Input[c4InInd] >> 24) & 0xFF);
    }
}


tCIDLib::TVoid TMessageDigest5::InitContext()
{
    m_ac4Count[0] = 0;
    m_ac4Count[1] = 0;

    m_ac4State[0] = 0x67452301;
    m_ac4State[1] = 0xEFCDAB89;
    m_ac4State[2] = 0x98BADCFE;
    m_ac4State[3] = 0x10325476;
}


tCIDLib::TVoid TMessageDigest5::ScrubContext()
{
    m_ac4Count[0] = 0;
    m_ac4Count[1] = 0;

    m_ac4State[0] = 0;
    m_ac4State[1] = 0;
    m_ac4State[2] = 0;
    m_ac4State[3] = 0;

    TRawMem::SetMemBuf(m_ac1Buffer, tCIDLib::TCard1(1) , sizeof(m_ac1Buffer));
}


tCIDLib::TVoid
TMessageDigest5::Transform(const tCIDLib::TCard1* const pac1Block)
{
    tCIDLib::TCard4 c4A = m_ac4State[0];
    tCIDLib::TCard4 c4B = m_ac4State[1];
    tCIDLib::TCard4 c4C = m_ac4State[2];
    tCIDLib::TCard4 c4D = m_ac4State[3];
    tCIDLib::TCard4 ac4X[TMessageDigest5::c4HashBytes];

    Decode(pac1Block, ac4X, TMessageDigest5::c4BlockLen);

    // Round #1
    FF(c4A, c4B, c4C, c4D, ac4X[ 0], CIDCrypto_MD5::c4S11, 0XD76AA478); // 1
    FF(c4D, c4A, c4B, c4C, ac4X[ 1], CIDCrypto_MD5::c4S12, 0XE8C7B756); // 2
    FF(c4C, c4D, c4A, c4B, ac4X[ 2], CIDCrypto_MD5::c4S13, 0X242070DB); // 3
    FF(c4B, c4C, c4D, c4A, ac4X[ 3], CIDCrypto_MD5::c4S14, 0XC1BDCEEE); // 4
    FF(c4A, c4B, c4C, c4D, ac4X[ 4], CIDCrypto_MD5::c4S11, 0XF57C0FAF); // 5
    FF(c4D, c4A, c4B, c4C, ac4X[ 5], CIDCrypto_MD5::c4S12, 0X4787C62A); // 6
    FF(c4C, c4D, c4A, c4B, ac4X[ 6], CIDCrypto_MD5::c4S13, 0XA8304613); // 7
    FF(c4B, c4C, c4D, c4A, ac4X[ 7], CIDCrypto_MD5::c4S14, 0XFD469501); // 8
    FF(c4A, c4B, c4C, c4D, ac4X[ 8], CIDCrypto_MD5::c4S11, 0X698098D8); // 9
    FF(c4D, c4A, c4B, c4C, ac4X[ 9], CIDCrypto_MD5::c4S12, 0X8B44F7AF); // 10
    FF(c4C, c4D, c4A, c4B, ac4X[10], CIDCrypto_MD5::c4S13, 0XFFFF5BB1); // 11
    FF(c4B, c4C, c4D, c4A, ac4X[11], CIDCrypto_MD5::c4S14, 0X895CD7BE); // 12
    FF(c4A, c4B, c4C, c4D, ac4X[12], CIDCrypto_MD5::c4S11, 0X6B901122); // 13
    FF(c4D, c4A, c4B, c4C, ac4X[13], CIDCrypto_MD5::c4S12, 0XFD987193); // 14
    FF(c4C, c4D, c4A, c4B, ac4X[14], CIDCrypto_MD5::c4S13, 0XA679438E); // 15
    FF(c4B, c4C, c4D, c4A, ac4X[15], CIDCrypto_MD5::c4S14, 0X49B40821); // 16

    // Round #2
    GG(c4A, c4B, c4C, c4D, ac4X[ 1], CIDCrypto_MD5::c4S21, 0XF61E2562); // 17
    GG(c4D, c4A, c4B, c4C, ac4X[ 6], CIDCrypto_MD5::c4S22, 0XC040B340); // 18
    GG(c4C, c4D, c4A, c4B, ac4X[11], CIDCrypto_MD5::c4S23, 0X265E5A51); // 19
    GG(c4B, c4C, c4D, c4A, ac4X[ 0], CIDCrypto_MD5::c4S24, 0XE9B6C7AA); // 20
    GG(c4A, c4B, c4C, c4D, ac4X[ 5], CIDCrypto_MD5::c4S21, 0XD62F105D); // 21
    GG(c4D, c4A, c4B, c4C, ac4X[10], CIDCrypto_MD5::c4S22,  0X2441453); // 22
    GG(c4C, c4D, c4A, c4B, ac4X[15], CIDCrypto_MD5::c4S23, 0XD8A1E681); // 23
    GG(c4B, c4C, c4D, c4A, ac4X[ 4], CIDCrypto_MD5::c4S24, 0XE7D3FBC8); // 24
    GG(c4A, c4B, c4C, c4D, ac4X[ 9], CIDCrypto_MD5::c4S21, 0X21E1CDE6); // 25
    GG(c4D, c4A, c4B, c4C, ac4X[14], CIDCrypto_MD5::c4S22, 0XC33707D6); // 26
    GG(c4C, c4D, c4A, c4B, ac4X[ 3], CIDCrypto_MD5::c4S23, 0XF4D50D87); // 27
    GG(c4B, c4C, c4D, c4A, ac4X[ 8], CIDCrypto_MD5::c4S24, 0X455A14ED); // 28
    GG(c4A, c4B, c4C, c4D, ac4X[13], CIDCrypto_MD5::c4S21, 0XA9E3E905); // 29
    GG(c4D, c4A, c4B, c4C, ac4X[ 2], CIDCrypto_MD5::c4S22, 0XFCEFA3F8); // 30
    GG(c4C, c4D, c4A, c4B, ac4X[ 7], CIDCrypto_MD5::c4S23, 0X676F02D9); // 31
    GG(c4B, c4C, c4D, c4A, ac4X[12], CIDCrypto_MD5::c4S24, 0X8D2A4C8A); // 32

    // Round #3
    HH(c4A, c4B, c4C, c4D, ac4X[ 5], CIDCrypto_MD5::c4S31, 0XFFFA3942); // 33
    HH(c4D, c4A, c4B, c4C, ac4X[ 8], CIDCrypto_MD5::c4S32, 0X8771F681); // 34
    HH(c4C, c4D, c4A, c4B, ac4X[11], CIDCrypto_MD5::c4S33, 0X6D9D6122); // 35
    HH(c4B, c4C, c4D, c4A, ac4X[14], CIDCrypto_MD5::c4S34, 0XFDE5380C); // 36
    HH(c4A, c4B, c4C, c4D, ac4X[ 1], CIDCrypto_MD5::c4S31, 0XA4BEEA44); // 37
    HH(c4D, c4A, c4B, c4C, ac4X[ 4], CIDCrypto_MD5::c4S32, 0X4BDECFA9); // 38
    HH(c4C, c4D, c4A, c4B, ac4X[ 7], CIDCrypto_MD5::c4S33, 0XF6BB4B60); // 39
    HH(c4B, c4C, c4D, c4A, ac4X[10], CIDCrypto_MD5::c4S34, 0XBEBFBC70); // 40
    HH(c4A, c4B, c4C, c4D, ac4X[13], CIDCrypto_MD5::c4S31, 0X289B7EC6); // 41
    HH(c4D, c4A, c4B, c4C, ac4X[ 0], CIDCrypto_MD5::c4S32, 0XEAA127FA); // 42
    HH(c4C, c4D, c4A, c4B, ac4X[ 3], CIDCrypto_MD5::c4S33, 0XD4EF3085); // 43
    HH(c4B, c4C, c4D, c4A, ac4X[ 6], CIDCrypto_MD5::c4S34,  0X4881D05); // 44
    HH(c4A, c4B, c4C, c4D, ac4X[ 9], CIDCrypto_MD5::c4S31, 0XD9D4D039); // 45
    HH(c4D, c4A, c4B, c4C, ac4X[12], CIDCrypto_MD5::c4S32, 0XE6DB99E5); // 46
    HH(c4C, c4D, c4A, c4B, ac4X[15], CIDCrypto_MD5::c4S33, 0X1FA27CF8); // 47
    HH(c4B, c4C, c4D, c4A, ac4X[ 2], CIDCrypto_MD5::c4S34, 0XC4AC5665); // 48

    // Round #4
    II(c4A, c4B, c4C, c4D, ac4X[ 0], CIDCrypto_MD5::c4S41, 0XF4292244); // 49
    II(c4D, c4A, c4B, c4C, ac4X[ 7], CIDCrypto_MD5::c4S42, 0X432AFF97); // 50
    II(c4C, c4D, c4A, c4B, ac4X[14], CIDCrypto_MD5::c4S43, 0XAB9423A7); // 51
    II(c4B, c4C, c4D, c4A, ac4X[ 5], CIDCrypto_MD5::c4S44, 0XFC93A039); // 52
    II(c4A, c4B, c4C, c4D, ac4X[12], CIDCrypto_MD5::c4S41, 0X655B59C3); // 53
    II(c4D, c4A, c4B, c4C, ac4X[ 3], CIDCrypto_MD5::c4S42, 0X8F0CCC92); // 54
    II(c4C, c4D, c4A, c4B, ac4X[10], CIDCrypto_MD5::c4S43, 0XFFEFF47D); // 55
    II(c4B, c4C, c4D, c4A, ac4X[ 1], CIDCrypto_MD5::c4S44, 0X85845DD1); // 56
    II(c4A, c4B, c4C, c4D, ac4X[ 8], CIDCrypto_MD5::c4S41, 0X6FA87E4F); // 57
    II(c4D, c4A, c4B, c4C, ac4X[15], CIDCrypto_MD5::c4S42, 0XFE2CE6E0); // 58
    II(c4C, c4D, c4A, c4B, ac4X[ 6], CIDCrypto_MD5::c4S43, 0XA3014314); // 59
    II(c4B, c4C, c4D, c4A, ac4X[13], CIDCrypto_MD5::c4S44, 0X4E0811A1); // 60
    II(c4A, c4B, c4C, c4D, ac4X[ 4], CIDCrypto_MD5::c4S41, 0XF7537E82); // 61
    II(c4D, c4A, c4B, c4C, ac4X[11], CIDCrypto_MD5::c4S42, 0XBD3AF235); // 62
    II(c4C, c4D, c4A, c4B, ac4X[ 2], CIDCrypto_MD5::c4S43, 0X2AD7D2BB); // 63
    II(c4B, c4C, c4D, c4A, ac4X[ 9], CIDCrypto_MD5::c4S44, 0XEB86D391); // 64

    // Add the new results back into the m_ac4State flags
    m_ac4State[0] += c4A;
    m_ac4State[1] += c4B;
    m_ac4State[2] += c4C;
    m_ac4State[3] += c4D;

    // Zero out sensitive information.
    TRawMem::SetMemBuf(ac4X, tCIDLib::TCard4(0), TMessageDigest5::c4HashBytes);
}

