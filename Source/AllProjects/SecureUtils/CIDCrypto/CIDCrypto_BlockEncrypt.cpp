//
// FILE NAME: CIDCrypto_BlockEncrypt.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/21/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TBlockEncrypter class. This is an abstract base
//  class interface for all block oriented encryption schemes.
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
RTTIDecls(TBlockEncrypter,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDCrypto_BlockEncrypt
{
    const tCIDLib::TCard4   c4BufSz = 4096;
}



// ---------------------------------------------------------------------------
//   CLASS: TBlockEncrypter
//  PREFIX: cryp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBlockEncrypter: Constructors and destructor
// ---------------------------------------------------------------------------
TBlockEncrypter::TBlockEncrypter(const  tCIDLib::TCard4         c4BlockSize
                                , const tCIDCrypto::EBlockModes eMode) :

    m_c4BlockSize(c4BlockSize)
    , m_eMode(eMode)
{
}

TBlockEncrypter::~TBlockEncrypter()
{
}


// ---------------------------------------------------------------------------
//  TBlockEncrypter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TBlockEncrypter::c4Decrypt( const   TMemBuf&                mbufCypher
                            ,       TMemBuf&                mbufPlain
                            , const tCIDLib::TCard4         c4CypherBytes
                            , const tCIDLib::TCard1* const  pc1IV)
{
    const tCIDLib::TCard4 c4SrcSize = mbufCypher.c4Size();

    //
    //  On decryption the source cypher bytes buffer must be an equal
    //  multiple of the block size, since the encryption should have forced
    //  this to happen by padding any trailing partial block.
    //
    if (c4CypherBytes % m_c4BlockSize)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcBlock_NotBlockMultiple
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4CypherBytes)
            , TCardinal(m_c4BlockSize)
        );
    }

    //
    //  If the cypher byte parm is larger than the size of the cypher text
    //  memory buffer's size, that's an error.
    //
    if (c4CypherBytes > c4SrcSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcGen_BadCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4CypherBytes)
            , TCardinal(c4SrcSize)
        );
    }

    //
    //  If the cypher byte parm is 0, then use the allocated size of the
    //  source buffer.
    //
    tCIDLib::TCard4 c4ActualSize = c4CypherBytes;
    if (!c4ActualSize)
        c4ActualSize = c4SrcSize;

    // Call the other version
    return c4Decrypt(mbufCypher.pc1Data(), mbufPlain, c4ActualSize, pc1IV);
}

tCIDLib::TCard4
TBlockEncrypter::c4Decrypt( const   tCIDLib::TCard1* const  pc1Cypher
                            ,       TMemBuf&                mbufPlain
                            , const tCIDLib::TCard4         c4CypherBytes
                            , const tCIDLib::TCard1* const  pc1IV)
{
    //
    //  If an IV is required, and not provided, throw. In CBC mode without the random
    //  first block we have to have the original IV to get back the real first block.
    //
    if ((m_eMode == tCIDCrypto::EBlockModes::CBC) && !pc1IV)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcBlock_NoIV
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  If zero cypher bytes, short circuit and just say zero output bytes. That's just
    //  not a situation we care to support.
    //
    if (!c4CypherBytes)
        return 0;

    // Do the appropriate decrypt for our mode
    tCIDLib::TCard4 c4Ret = 0;
    switch(m_eMode)
    {
        case tCIDCrypto::EBlockModes::ECB :
            c4Ret = c4ECBDecrypt(pc1Cypher, c4CypherBytes, mbufPlain);
            break;

        case tCIDCrypto::EBlockModes::CBC :
        case tCIDCrypto::EBlockModes::CBC_IV :
            c4Ret = c4CBCDecrypt(pc1Cypher, c4CypherBytes, mbufPlain, pc1IV);
            break;

        case tCIDCrypto::EBlockModes::OFB :
            c4Ret = c4OFBProcess(pc1Cypher, c4CypherBytes, mbufPlain, pc1IV);
            break;

        default :
            CIDAssert2(L"Unknown block decryption mode");
            break;
    };

    return c4Ret;
}

tCIDLib::TCard4
TBlockEncrypter::c4Decrypt( const   TMemBuf&                mbufCypher
                            ,       TString&                strPlain
                            , const tCIDLib::TCard4         c4CypherBytes
                            , const tCIDLib::TCard1* const  pc1IV)
{
    //
    //  Get the number of allocated bytes in the source cypher buffer
    //  and the maximum bytes the target string could hold (which is the
    //  max char size times the bytes per char.
    //
    const tCIDLib::TCard4 c4SrcSize = mbufCypher.c4Size();

    //
    //  If the cypher byte parm is larger than the size of the cypher text
    //  memory buffer's size, that's an error.
    //
    if (c4CypherBytes > c4SrcSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcGen_BadCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4CypherBytes)
            , TCardinal(c4SrcSize)
        );
    }

    //
    //  If the cypher byte parm is 0, then use the allocated size of the
    //  source buffer.
    //
    tCIDLib::TCard4 c4ActualSize = c4CypherBytes;
    if (!c4ActualSize)
        c4ActualSize = c4SrcSize;


    // Decrypt into a temp buffer by calling the other version
    THeapBuf mbufPlain(c4ActualSize);
    const tCIDLib::TCard4 c4PlainCnt = c4Decrypt
    (
        mbufCypher.pc1Data(), mbufPlain, c4ActualSize, pc1IV
    );

    //
    //  Ok, if we got any bytes, then we need to transcode them. The
    //  encrypter method for strings converts to UTF-8 first, then does
    //  the encryption. This is so that this encrypted text can be put on
    //  the wire and decoded on other machines.
    //
    if (c4PlainCnt)
    {
        TUTF8Converter tcvtTmp;
        tcvtTmp.c4ConvertFrom(mbufPlain.pc1Data(), c4PlainCnt, strPlain);
    }
    return c4PlainCnt;
}

tCIDLib::TCard4
TBlockEncrypter::c4Decrypt( const   tCIDLib::TCard1* const  pc1Cypher
                            ,       tCIDLib::TCard1* const  pc1Plain
                            , const tCIDLib::TCard4         c4CypherBytes
                            , const tCIDLib::TCard4         c4MaxOutBytes
                            , const tCIDLib::TCard1* const  pc1IV)
{
    // Decrypt into a temp buffer using the other version
    THeapBuf mbufPlain(c4CypherBytes);
    const tCIDLib::TCard4 c4PlainCnt = c4Decrypt
    (
        pc1Cypher, mbufPlain, c4CypherBytes, pc1IV
    );

    // If the target isn't large enough, then give up
    if (c4MaxOutBytes < c4PlainCnt)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcBlock_TarTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
        );
    }

    // Copy over the data
    mbufPlain.CopyOut(pc1Plain, c4PlainCnt);
    return c4PlainCnt;
}


//
//  These methods handle encrypting to and from various types of source and
//  destination buffers.
//
tCIDLib::TCard4
TBlockEncrypter::c4Encrypt( const   tCIDLib::TCard1* const  pc1Plain
                            ,       TMemBuf&                mbufCypher
                            , const tCIDLib::TCard4         c4PlainBytes
                            , const tCIDLib::TCard1* const  pac1IV)
{
    // If an IV is required, and not provided, throw
    if (((m_eMode == tCIDCrypto::EBlockModes::CBC)
    ||   (m_eMode == tCIDCrypto::EBlockModes::CBC_IV)
    ||   (m_eMode == tCIDCrypto::EBlockModes::OFB))
    &&  !pac1IV)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcBlock_NoIV
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  If zero plain bytes, short circuit and just say zero output bytes. That's just
    //  not a situation we care to support.
    //
    if (!c4PlainBytes)
        return 0;

    //
    //  We can vastly simplify the private grunt work methods by checking to see if the
    //  incoming plain text is less than one block in size. If so, we will create a
    //  zero padded block and pass that in instead.
    //
    tCIDLib::TCard4 c4RealPlain = c4PlainBytes;
    const tCIDLib::TCard1* pc1RealPlain = pc1Plain;
    tCIDLib::TCard1* pc1Padded = nullptr;
    if (c4PlainBytes < m_c4BlockSize)
    {
        pc1Padded = new tCIDLib::TCard1[m_c4BlockSize];
        tCIDLib::TCard4 c4Index = 0;
        while (c4Index < c4PlainBytes)
        {
            pc1Padded[c4Index] = pc1Plain[c4Index];
            c4Index++;
        }

        while (c4Index < m_c4BlockSize)
        {
            pc1Padded[c4Index] = 0;
            c4Index++;
        }

        c4RealPlain = m_c4BlockSize;
        pc1RealPlain = pc1Padded;
    }
    TArrayJanitor<tCIDLib::TCard1> janPadded(pc1Padded);

    // Do the appropriate decrypt for our mode
    tCIDLib::TCard4 c4Ret = 0;
    switch(m_eMode)
    {
        case tCIDCrypto::EBlockModes::ECB :
            c4Ret = c4ECBEncrypt(pc1RealPlain, c4RealPlain, mbufCypher);
            break;

        case tCIDCrypto::EBlockModes::CBC :
        case tCIDCrypto::EBlockModes::CBC_IV :
            c4Ret = c4CBCEncrypt(pc1RealPlain, c4RealPlain, mbufCypher, pac1IV);
            break;

        case tCIDCrypto::EBlockModes::OFB :
            c4Ret = c4OFBProcess(pc1RealPlain, c4RealPlain, mbufCypher, pac1IV);
            break;

        default :
            CIDAssert2(L"Unknown block encryption mode");
            break;
    };

    return c4Ret;
}

tCIDLib::TCard4
TBlockEncrypter::c4Encrypt( const   TMemBuf&                mbufPlain
                            ,       TMemBuf&                mbufCypher
                            , const tCIDLib::TCard4         c4PlainBytes
                            , const tCIDLib::TCard1* const  pac1IV)
{
    const tCIDLib::TCard4 c4SrcSize = mbufPlain.c4Size();

    //
    //  If the plain bytes parm is larger than the size of the plain text
    //  memory buffer's size, that's an error.
    //
    if (c4PlainBytes > c4SrcSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcGen_BadCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4PlainBytes)
            , TCardinal(c4SrcSize)
        );
    }

    //
    //  If the plain byte parm is 0, then use the allocated size of the
    //  source buffer.
    //
    tCIDLib::TCard4 c4ActualSize = c4PlainBytes;
    if (!c4ActualSize)
        c4ActualSize = c4SrcSize;

    // Call the other version
    return c4Encrypt(mbufPlain.pc1Data(), mbufCypher, c4ActualSize, pac1IV);
}

tCIDLib::TCard4
TBlockEncrypter::c4Encrypt( const   TString&                strPlain
                            ,       TMemBuf&                mbufCypher
                            , const tCIDLib::TCard1* const  pac1IV)
{
    // Watch for an empty source string
    if (strPlain.bIsEmpty())
        return 0;

    //
    //  First we have to transcode the text. In order to avoid endian issues
    //  if this data gets put on the wire, we transcode it to UTF-8 and then
    //  encrypt that. So transcode to a temp buffer. Take a guess at an initial
    //  allocation that won't have to be expanded.
    //
    THeapBuf mbufPlain((strPlain.c4Length() * kCIDLib::c4CharBytes) + 64);
    TUTFConverter tcvtTmp;
    tCIDLib::TCard4 c4PlainBytes;
    tcvtTmp.c4ConvertTo(strPlain, mbufPlain, c4PlainBytes);

    // Call the other version
    return c4Encrypt(mbufPlain.pc1Data(), mbufCypher, c4PlainBytes, pac1IV);
}

tCIDLib::TCard4
TBlockEncrypter::c4Encrypt( const   tCIDLib::TCard1* const  pc1Plain
                            ,       tCIDLib::TCard1* const  pc1Cypher
                            , const tCIDLib::TCard4         c4PlainBytes
                            , const tCIDLib::TCard4         c4MaxOutBytes
                            , const tCIDLib::TCard1* const  pac1IV)
{
    // Encrypt to a temp buffer first
    THeapBuf mbufCypher(c4MaxOutBytes, c4MaxOutBytes);
    const tCIDLib::TCard4 c4CypherBytes = c4Encrypt
    (
        pc1Plain, mbufCypher, c4PlainBytes, pac1IV
    );

    // COpy it to the caller's buffer
    mbufCypher.CopyOut(pc1Cypher, c4CypherBytes);

    return c4CypherBytes;
}


tCIDCrypto::EBlockModes TBlockEncrypter::eMode() const
{
    return m_eMode;
}

tCIDCrypto::EBlockModes
TBlockEncrypter::eMode(const tCIDCrypto::EBlockModes eNewMode)
{
    m_eMode = eNewMode;
    return m_eMode;
}


tCIDLib::TVoid TBlockEncrypter::Reset()
{
    ResetImpl();
}



// ---------------------------------------------------------------------------
//  TBlockEncrypter: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Just does a simple ECB decrypt
tCIDLib::TCard4
TBlockEncrypter::c4ECBDecrypt(  const   tCIDLib::TCard1* const  pc1Cypher
                                , const tCIDLib::TCard4         c4CypherBytes
                                ,       TMemBuf&                mbufPlain)
{
    //
    //  Lets see now many full blocks we have. We know that there are only
    //  even blocks in the cypher text so we don't have to worry about
    //  extra blocks at the end.
    //
    const tCIDLib::TCard4 c4FullBlocks = c4CypherBytes / m_c4BlockSize;

    //
    //  And here is a current block index we use to run through the
    //  blocks and a pointer into each buffer that we bump up as we
    //  go through the data.
    //
    tCIDLib::TCard1* pc1Dest = new tCIDLib::TCard1[m_c4BlockSize];
    TArrayJanitor<tCIDLib::TCard1> janDest(pc1Dest);

    const tCIDLib::TCard1* pc1Src = pc1Cypher;
    tCIDLib::TCard4 c4TarInd = 0;
    for (tCIDLib::TCard4 c4BlockInd = 0; c4BlockInd < c4FullBlocks; c4BlockInd++)
    {
        // Decrypt a block from the source to the destination
        DecryptImpl(pc1Src, pc1Dest);

        // And copy to our target buffer
        mbufPlain.CopyIn(pc1Dest, m_c4BlockSize, c4TarInd);

        // And move to the next round
        pc1Src += m_c4BlockSize;
        c4TarInd += m_c4BlockSize;
    }

    // Return the actual bytes decrypted
    return c4TarInd;
}


//
//  Does a CBC/CBC_IV decrypt. We require an IV for decript on CBC since the first
//  block is real content and needs to be correctly decoded. For CBC_IV we don't use
//  it since the first block is random stuff and it is just tossed.
//
tCIDLib::TCard4
TBlockEncrypter::c4CBCDecrypt(  const   tCIDLib::TCard1* const  pc1Cypher
                                , const tCIDLib::TCard4         c4CypherBytes
                                ,       TMemBuf&                mbufCypher
                                , const tCIDLib::TCard1* const  pac1IV)
{
    //
    //  Start off assuming that we will output the same number of bytes
    //  as the input has available.
    //
    tCIDLib::TCard4 c4OutputBytes = c4CypherBytes;

    //
    //  If we are in CBC_IV mode, then we subtract room for an extra dummy
    //  block in the output buffer because the encryption put out an
    //  initialization vector block that is not part of the plain text
    //  that we are to return.
    //
    if (m_eMode == tCIDCrypto::EBlockModes::CBC_IV)
        c4OutputBytes -= m_c4BlockSize;

    //
    //  Lets see now many full blocks we have. We know that there are only
    //  even blocks in the cypher text so we don't have to worry about
    //  extra blocks at the end.
    //
    const tCIDLib::TCard4 c4FullBlocks = c4CypherBytes / m_c4BlockSize;

    //
    //  And here is a current block index we use to run through the blocks and
    //  a pointer into the source buffer that we bump up as we go through the data.
    //
    tCIDLib::TCard4 c4BlockInd = 0;
    const tCIDLib::TCard1* pc1Src = pc1Cypher;

    // Allocate a block size output buffer to use for each round
    tCIDLib::TCard1* pc1Dest = new tCIDLib::TCard1[m_c4BlockSize];
    TArrayJanitor<tCIDLib::TCard1> janDest(pc1Dest);

    //
    //  A pointer to the previous source block, which we use for the XORing
    //  stuff.
    //
    const tCIDLib::TCard1* pc1Prev = pc1Src;

    // Decrypt the first block and move the block index and source pointer forward
    DecryptImpl(pc1Src, pc1Dest);
    c4BlockInd++;
    pc1Src += m_c4BlockSize;

    //
    //  If we are in CBC_IV mode, then we can just discard the initial block because
    //  it is not used and in decrypt mode it's not needed as a previous block.
    //
    //  Else it's the first real block and we have to XOR it with the IV and output
    //  it.
    //
    //  Either way, the pump is primed now for the loop below.
    //
    tCIDLib::TCard4 c4TarInd = 0;
    if (m_eMode == tCIDCrypto::EBlockModes::CBC)
    {
        // Xor with the IV in this case
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4BlockSize; c4Ind++)
            pc1Dest[c4Ind] ^= pac1IV[c4Ind];

        mbufCypher.CopyIn(pc1Dest, m_c4BlockSize, c4TarInd);
        c4TarInd += m_c4BlockSize;
    }


    for (; c4BlockInd < c4FullBlocks; c4BlockInd++)
    {
        // Decrypt a block from the source to the destination
        DecryptImpl(pc1Src, pc1Dest);

        // Xor with the previous block
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4BlockSize; c4Ind++)
        {
            pc1Dest[c4Ind] ^= *pc1Prev;
            pc1Prev++;
        }

        mbufCypher.CopyIn(pc1Dest, m_c4BlockSize, c4TarInd);

        // And adjust for the next block
        pc1Src += m_c4BlockSize;
        c4TarInd += m_c4BlockSize;
    }

    CIDAssert(c4TarInd == c4OutputBytes, L"CBC decrypt output size != expected");

    // Return the actual bytes decrypted
    return c4OutputBytes;
}


//
//  Does a simple ECB encryption
//
tCIDLib::TCard4
TBlockEncrypter::c4ECBEncrypt(  const   tCIDLib::TCard1* const  pc1Plain
                                , const tCIDLib::TCard4         c4PlainBytes
                                ,       TMemBuf&                mbufCypher)
{
    //
    //  Lets see now many full blocks we can do. We can blast through these
    //  first then deal with any partial last block after that.
    //
    tCIDLib::TCard4 c4FullBlocks = c4PlainBytes / m_c4BlockSize;

    // Calc the number of user data bytes in the last buffer
    const tCIDLib::TCard4 c4ExtraBytes = c4PlainBytes
                                         - (c4FullBlocks * m_c4BlockSize);

    // We have to pad the output up to the next block size
    tCIDLib::TCard4 c4OutputBytes = c4FullBlocks * m_c4BlockSize;
    if (c4ExtraBytes)
        c4OutputBytes += m_c4BlockSize;

    // Now make sure the output buffer can handle it
    if (mbufCypher.c4MaxSize() < c4OutputBytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcGen_TargetTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4OutputBytes)
            , TCardinal(mbufCypher.c4MaxSize())
        );
    }

    // Allocate it up to handle this much if not already
    if (mbufCypher.c4Size() < c4OutputBytes)
        mbufCypher.Reallocate(c4OutputBytes, kCIDLib::False);

    // Allocate a block size output buffer to use for each round
    tCIDLib::TCard1* pc1Dest = new tCIDLib::TCard1[m_c4BlockSize];
    TArrayJanitor<tCIDLib::TCard1> janDest(pc1Dest);
    const tCIDLib::TCard1* pc1Src = pc1Plain;

    //
    //  Ok, so now we can just run through the full blocks and encrypt
    //  each one out to the plain text buffer. On each round we just pass
    //  the derived class' basic block encrypt/decrypt method a raw pointer
    //  into each buffer. We both agree on the block size so he knows how
    //  many bytes to do and we know how many bytes to move forward each
    //  time.
    //
    tCIDLib::TCard4 c4TarInd = 0;
    for (tCIDLib::TCard4 c4BlockInd = 0; c4BlockInd < c4FullBlocks; c4BlockInd++)
    {
        // Do another block
        EncryptImpl(pc1Src, pc1Dest);

        // Copy this block to the output buffer and adjust for the next round
        mbufCypher.CopyIn(pc1Dest, m_c4BlockSize, c4TarInd);

        pc1Src += m_c4BlockSize;
        c4TarInd += m_c4BlockSize;
    }

    if (c4ExtraBytes)
    {
        // Copy over the trailing bytes to a temp
        tCIDLib::TCard1* pc1Tmp = new tCIDLib::TCard1[m_c4BlockSize];
        TArrayJanitor<tCIDLib::TCard1> janTmp(pc1Tmp);

        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < c4ExtraBytes; c4Index++)
            pc1Tmp[c4Index] = *pc1Src++;

        // Zero pad the unused bytes
        for (; c4Index < m_c4BlockSize; c4Index++)
            pc1Tmp[c4Index] = 0;

        // And Encrypt it to the target
        EncryptImpl(pc1Tmp, pc1Dest);

        mbufCypher.CopyIn(pc1Dest, m_c4BlockSize, c4TarInd);
        c4TarInd += m_c4BlockSize;
    }

    CIDAssert(c4OutputBytes == c4TarInd, L"ECB encrypt bytes != calculated");

    // Return the actual bytes decrypted
    return c4OutputBytes;
}


//
//  Does a CBC/CBC_IV type encrypt. We need the passed IV for both modes. For IV
//  mode, we generate a random vector as the first block. For straight CBC mode
//  there isn't one, so the first block is XORd against the passed IV. That means
//  the IV must be provided for decryption in that case.
//
tCIDLib::TCard4
TBlockEncrypter::c4CBCEncrypt(  const   tCIDLib::TCard1* const  pc1Plain
                                , const tCIDLib::TCard4         c4PlainBytes
                                ,       TMemBuf&                mbufCypher
                                , const tCIDLib::TCard1* const  pac1IV)
{
    //
    //  Lets see now many full blocks we can do. We can blast through these
    //  first then deal with any partial last block after that.
    //
    tCIDLib::TCard4 c4FullBlocks = c4PlainBytes / m_c4BlockSize;

    // Calc the number of user data bytes in the last buffer
    const tCIDLib::TCard4 c4ExtraBytes(c4PlainBytes - (c4FullBlocks * m_c4BlockSize));

    // We have to pad the output up to the next block size
    tCIDLib::TCard4 c4OutputBytes = c4FullBlocks * m_c4BlockSize;
    if (c4ExtraBytes && c4FullBlocks)
        c4OutputBytes += m_c4BlockSize;

    //
    //  If we are in CBC_IV mode, then we have to have room for an extra
    //  dummy block in the output buffer because we have to put out an
    //  random block.
    //
    if (m_eMode == tCIDCrypto::EBlockModes::CBC_IV)
    {
        c4OutputBytes += m_c4BlockSize;
        c4FullBlocks++;
    }

    // Now make sure the output buffer can handle it
    if (mbufCypher.c4MaxSize() < c4OutputBytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcGen_TargetTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4OutputBytes)
            , TCardinal(mbufCypher.c4MaxSize())
        );
    }

    // Allocate it up to handle this much if not already
    if (mbufCypher.c4Size() < c4OutputBytes)
        mbufCypher.Reallocate(c4OutputBytes, kCIDLib::False);

    //
    //  And here is a current block index we use to run through the
    //  blocks and a pointer into the source buffer that we bump up as we
    //  go through the data.
    //
    tCIDLib::TCard4 c4BlockInd = 0;
    const tCIDLib::TCard1* pc1Src = pc1Plain;

    // Allocate a block size buffer to use for each round
    tCIDLib::TCard1* pc1Round = new tCIDLib::TCard1[m_c4BlockSize];
    TArrayJanitor<tCIDLib::TCard1> janRound(pc1Round);

    //
    //  And another that we need to decode to each output buffer to to act as
    //  previous round XOR material.
    //
    tCIDLib::TCard1* pc1Prev = new tCIDLib::TCard1[m_c4BlockSize];
    TArrayJanitor<tCIDLib::TCard1> janPrev(pc1Prev);

    //
    //  If we are in CBC_IV mode, then we have to fake in a random block at
    //  the start as though it was the first block. Else we have to start with
    //  the first actual block.
    //
    //  Either way it's XOR'd with the IV.
    //
    //  Either way we have primed the pump and the loop below has its previous
    //  block content to start working with.
    //
    tCIDLib::TCard4 c4TarInd = 0;
    if (m_eMode == tCIDCrypto::EBlockModes::CBC_IV)
    {
        facCIDCrypto().GetRandomBytes(pc1Round, m_c4BlockSize);
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4BlockSize; c4Ind++)
            pc1Round[c4Ind] ^= pac1IV[c4Ind];

        EncryptImpl(pc1Round, pc1Prev);
    }
     else
    {
        //
        //  Start with the first actual source block, copy to dummy and XOR with IV.
        //
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4BlockSize; c4Ind++)
        {
            pc1Round[c4Ind] = *pc1Src++;
            pc1Round[c4Ind] ^= pac1IV[c4Ind];
        }
        EncryptImpl(pc1Round, pc1Prev);
    }

    //
    //  Copy this first block to the target and bump the target index. The prevoius
    //  block has the right stuff for the next round.
    //
    mbufCypher.CopyIn(pc1Prev, m_c4BlockSize, c4TarInd);
    c4TarInd += m_c4BlockSize;

    // And bump up the block index since we've done one output block so far
    c4BlockInd++;

    for (; c4BlockInd < c4FullBlocks; c4BlockInd++)
    {
        //
        //  And now copy each byte of the next block of plain text to the round
        //  block, XORing against the previous block's bytes as we go.
        //
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4BlockSize; c4Ind++)
        {
            pc1Round[c4Ind] = *pc1Src++;
            pc1Round[c4Ind] ^= pc1Prev[c4Ind];
        }

        // Encrypt into previous, which leaves it where we want it for the next round
        EncryptImpl(pc1Round, pc1Prev);

        // Copy to the output buffer and move up
        mbufCypher.CopyIn(pc1Prev, m_c4BlockSize, c4TarInd);
        c4TarInd += m_c4BlockSize;
    }

    if (c4ExtraBytes)
    {
        // Copy over the trailing bytes to the temp block
        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < c4ExtraBytes; c4Index++)
            pc1Round[c4Index] = *pc1Src++;

        // Zero pad the unused bytes
        for (; c4Index < m_c4BlockSize; c4Index++)
            pc1Round[c4Index] = 0;

        // XOR against the previous block
        for (c4Index = 0; c4Index < m_c4BlockSize; c4Index++)
            pc1Round[c4Index] ^= pc1Prev[c4Index];

        // And Encrypt it to the target
        EncryptImpl(pc1Round, pc1Prev);

        mbufCypher.CopyIn(pc1Prev, m_c4BlockSize, c4TarInd);
        c4TarInd += m_c4BlockSize;
    }

    CIDAssert(c4TarInd == c4OutputBytes, L"CBC encrypt output size != expected");

    // Return the actual bytes decrypted
    return c4OutputBytes;
}


//
//  Does an OFB mode decrypt or encrypt. It is a symmetric algorithm, so we do
//  the same thing for both, they just pass the plain text in one case and the
//  cypher text in another.
//
//  On decrypt we just will never have a trailing partial block. The caller
//  verifies this so we don't have to worry about it. So we process any partial
//  block if it is present, else not.
//
//  The IV is required for both since there is no random block padding.
//
tCIDLib::TCard4
TBlockEncrypter::c4OFBProcess(  const   tCIDLib::TCard1* const  pc1Input
                                , const tCIDLib::TCard4         c4InBytes
                                ,       TMemBuf&                mbufOut
                                , const tCIDLib::TCard1* const  pac1IV)
{
    //
    //  Lets see now many full blocks we can do. We can blast through these
    //  first then deal with any partial last block after that.
    //
    tCIDLib::TCard4 c4FullBlocks = c4InBytes / m_c4BlockSize;

    // Calc the number of user data bytes in the last buffer
    const tCIDLib::TCard4 c4ExtraBytes = c4InBytes
                                         - (c4FullBlocks * m_c4BlockSize);

    // We have to pad the output up to the next block size
    tCIDLib::TCard4 c4OutputBytes = c4FullBlocks * m_c4BlockSize;
    if (c4ExtraBytes)
        c4OutputBytes += m_c4BlockSize;

    // Now make sure the output buffer can handle it
    if (mbufOut.c4MaxSize() < c4OutputBytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcGen_TargetTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4OutputBytes)
            , TCardinal(mbufOut.c4MaxSize())
        );
    }

    // Allocate two block size buffers, we'll rotate between them
    tCIDLib::TCard1* pc1First = new tCIDLib::TCard1[m_c4BlockSize];
    TArrayJanitor<tCIDLib::TCard1> janFirst(pc1First);
    tCIDLib::TCard1* pc1Sec = new tCIDLib::TCard1[m_c4BlockSize];
    TArrayJanitor<tCIDLib::TCard1> janSec(pc1Sec);


    // To kick start the process, process the IV
    const tCIDLib::TCard1* pc1Src = pc1Input;
    EncryptImpl(pac1IV, pc1First);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4BlockSize; c4Index++)
        pc1Sec[c4Index] = pc1First[c4Index] ^ *pc1Src++;

    // Store it away
    tCIDLib::TCard4 c4TarInd = 0;
    mbufOut.CopyIn(pc1Sec, m_c4BlockSize, c4TarInd);
    c4TarInd += m_c4BlockSize;

    // Now loop through the rest of the full blocks
    tCIDLib::TCard4 c4BlockInd = 1;
    tCIDLib::TCard1* pc1Prev = nullptr;
    tCIDLib::TCard1* pc1Cur = nullptr;
    for (; c4BlockInd < c4FullBlocks; c4BlockInd++)
    {
        // Just go back and forth from one buffer to another
        if (c4BlockInd & 1)
        {
            pc1Prev = pc1First;
            pc1Cur = pc1Sec;
        }
         else
        {
            pc1Prev = pc1Sec;
            pc1Cur = pc1First;
        }

        // Encrypt the previous block to the current block
        EncryptImpl(pc1Prev, pc1Cur);

        //
        //  Xor the current block with the source, into the previous block
        //  since we need the current block as the next round's previous.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4BlockSize; c4Index++)
            pc1Prev[c4Index] = *pc1Src++ ^ pc1Cur[c4Index];

        // And store this one
        mbufOut.CopyIn(pc1Prev, m_c4BlockSize, c4TarInd);
        c4TarInd += m_c4BlockSize;
    }

    if (c4ExtraBytes)
    {
        // One last flip of the rotating buffers and encrypt
        if (c4BlockInd & 1)
        {
            pc1Prev = pc1First;
            pc1Cur = pc1Sec;
        }
         else
        {
            pc1Prev = pc1Sec;
            pc1Cur = pc1First;
        }
        EncryptImpl(pc1Prev, pc1Cur);

        // Copy over the trailing bytes to the previous block
        tCIDLib::TCard4 c4Index = 0;
        for (; c4Index < c4ExtraBytes; c4Index++)
            pc1Prev[c4Index] = *pc1Src++;

        // Zero pad the unused bytes
        for (; c4Index < m_c4BlockSize; c4Index++)
            pc1Prev[c4Index] = 0;

        //
        //  XOR against the current block. We don't need the current block anymore
        //  so it's ok to overwrite it now.
        //
        for (c4Index = 0; c4Index < m_c4BlockSize; c4Index++)
            pc1Cur[c4Index] ^= pc1Prev[c4Index];

        mbufOut.CopyIn(pc1Cur, m_c4BlockSize, c4TarInd);
        c4TarInd += m_c4BlockSize;
    }

    CIDAssert(c4TarInd == c4OutputBytes, L"OFB operation output size != expected");

    // Return the actual bytes decrypted
    return c4OutputBytes;
}
