//
// FILE NAME: CIDCrypto_Xor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/21/1997
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
//  This file implements the very simple Xor encrpytor scheme. There is a
//  TXorBlockEncrypter for block based Xor encryption, and a
//  TXorStreamEncrypter for stream oriented encryption.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TXorBlockEncrypter,TBlockEncrypter)
RTTIDecls(TXorStreamEncrypter,TStreamEncrypter)


namespace CIDCrypto_Xor
{
    // -----------------------------------------------------------------------
    //  Local, constant data
    //
    //  c4StreamBlockSize
    //      The size of the block of bytes that the TXorStreamEncypter class
    //      works on at a time (or up to that many at a time.)
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4StreamBlockSize = 512;
}


// ---------------------------------------------------------------------------
//   CLASS: TXorBlockEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXorBlockEncrypter: Constructors and destructors
// ---------------------------------------------------------------------------
TXorBlockEncrypter::TXorBlockEncrypter( const   TCryptoKey&             ckeyToUse
                                        , const tCIDCrypto::EBlockModes eMode) :

    TBlockEncrypter(1, eMode)
    , m_c4CurKeyByte(0)
    , m_ckeyThis(ckeyToUse)
{
}

TXorBlockEncrypter::~TXorBlockEncrypter()
{
}

// ---------------------------------------------------------------------------
//  TXorBlockEncrypter: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TCryptoKey& TXorBlockEncrypter::ckeyThis() const
{
    return m_ckeyThis;
}


// ---------------------------------------------------------------------------
//  TXorBlockEncrypter: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXorBlockEncrypter::DecryptImpl(const   tCIDLib::TCard1* const  pc1CypherBuf
                                ,       tCIDLib::TCard1* const  pc1PlainBuf)
{
    // Do the XOR operation
    *pc1PlainBuf = *pc1CypherBuf ^ m_ckeyThis[m_c4CurKeyByte];

    // And adjust the key byte index
    m_c4CurKeyByte++;
    if (m_c4CurKeyByte >= m_ckeyThis.c4Bytes())
        m_c4CurKeyByte = 0;
}


tCIDLib::TVoid
TXorBlockEncrypter::EncryptImpl(const   tCIDLib::TCard1* const  pc1PlainBuf
                                ,       tCIDLib::TCard1* const  pc1CypherBuf)
{
    // Do the XOR operation
    *pc1CypherBuf = *pc1PlainBuf ^ m_ckeyThis[m_c4CurKeyByte];

    // And adjust the key byte index
    m_c4CurKeyByte++;
    if (m_c4CurKeyByte >= m_ckeyThis.c4Bytes())
        m_c4CurKeyByte = 0;
}


tCIDLib::TVoid TXorBlockEncrypter::ResetImpl()
{
    // Just start the key back at its 0th byte
    m_c4CurKeyByte = 0;
}




// ---------------------------------------------------------------------------
//   CLASS: TXorStreamEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXorStreamEncrypter: Constructors and destructors
// ---------------------------------------------------------------------------
TXorStreamEncrypter::TXorStreamEncrypter(const TCryptoKey&           ckeyToUse
                                        ,       TBinInStream* const  pstrmIn
                                        ,       TBinOutStream* const pstrmOut
                                        , const tCIDLib::EAdoptOpts  eAdopt) :

    TStreamEncrypter(pstrmIn, pstrmOut, eAdopt)
    , m_c4CurKeyByte(0)
    , m_ckeyThis(ckeyToUse)
{
}

TXorStreamEncrypter::~TXorStreamEncrypter()
{
}


// ---------------------------------------------------------------------------
//  TXorStreamEncrypter: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TCryptoKey& TXorStreamEncrypter::ckeyThis() const
{
    return m_ckeyThis;
}


// ---------------------------------------------------------------------------
//  TXorStreamEncrypter: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXorStreamEncrypter::DecryptImpl()
{
    //
    //  Read data from the cypher stream and write it back out to the plain
    //  stream after xoring with the key bytes. To be more efficient, we do
    //  it blocks of up to m_c4StreamBlockSize bytes.
    //
    tCIDLib::TCard1 ac1TempBuf[CIDCrypto_Xor::c4StreamBlockSize];
    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        //
        //  Read in up to a block. The default is to be intolerant of not
        //  reading in the whole requested block size, which is not what we
        //  want here. So we tell it to just get as much as is available.
        //
        const tCIDLib::TCard4 c4Actual = strmIn().c4ReadRawBuffer
        (
            ac1TempBuf
            , CIDCrypto_Xor::c4StreamBlockSize
            , tCIDLib::EAllData::OkIfNotAll
        );

        // If no data available, just return
        if (!c4Actual)
            break;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Actual; c4Index++)
        {
            if (m_c4CurKeyByte >= m_ckeyThis.c4Bytes())
                m_c4CurKeyByte = 0;
            ac1TempBuf[c4Index] ^= m_ckeyThis[m_c4CurKeyByte];
        }

        //
        //  And write it back out. For this one, we have to fail if we don't
        //  write all the requested data, which is the default.
        //
        strmOut().c4WriteRawBuffer(ac1TempBuf, c4Actual);
    }
    strmOut().Flush();
}


tCIDLib::TVoid TXorStreamEncrypter::EncryptImpl()
{
    //
    //  Read data from the plain stream and write it back out to the cypher
    //  stream after xoring with the key bytes. To be more efficient, we do
    //  it blocks of up to m_c4StreamBlockSize bytes.
    //
    tCIDLib::TCard1 ac1TempBuf[CIDCrypto_Xor::c4StreamBlockSize];
    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        //
        //  Read in up to a block. The default is to be intolerant of not
        //  reading in the whole requested block size, which is not what we
        //  want here. So we tell it to just get as much as is available.
        //
        const tCIDLib::TCard4 c4Actual = strmIn().c4ReadRawBuffer
        (
            ac1TempBuf
            , CIDCrypto_Xor::c4StreamBlockSize
            , tCIDLib::EAllData::OkIfNotAll
        );

        // If no data available, just return
        if (!c4Actual)
            break;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Actual; c4Index++)
        {
            if (m_c4CurKeyByte >= m_ckeyThis.c4Bytes())
                m_c4CurKeyByte = 0;
            ac1TempBuf[c4Index] ^= m_ckeyThis[m_c4CurKeyByte];
        }

        //
        //  And write it back out. For this one, we have to fail if we don't
        //  write all the requested data, which is the default.
        //
        strmOut().c4WriteRawBuffer(ac1TempBuf, c4Actual);
    }
    strmOut().Flush();
}


tCIDLib::TVoid TXorStreamEncrypter::ResetImpl()
{
    // Start the key back at its 0th byte
    m_c4CurKeyByte = 0;
}

