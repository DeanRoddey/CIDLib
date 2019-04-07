//
// FILE NAME: CIDCrypto_MsgHash.cpp
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
//  This file implements the TMsgHash class.
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
RTTIDecls(TMsgHash,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDCrypto_MsgHash
{
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//   CLASS: TMsgHash
//  PREFIX: mhash
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMsgHash: Destructor
// ---------------------------------------------------------------------------
TMsgHash::~TMsgHash()
{
    delete [] m_pc1Hash;
    m_pc1Hash = nullptr;
}


// ---------------------------------------------------------------------------
//  TMsgHash: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TCard1& TMsgHash::operator[](const tCIDLib::TCard4 c4ByteIndex)
{
    if (c4ByteIndex >= m_c4Bytes)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4ByteIndex)
            , clsIsA()
            , TCardinal(m_c4Bytes)
        );
    }
    return m_pc1Hash[c4ByteIndex];
}


tCIDLib::TCard1 TMsgHash::operator[](const tCIDLib::TCard4 c4ByteIndex) const
{
    if (c4ByteIndex >= m_c4Bytes)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4ByteIndex)
            , clsIsA()
            , TCardinal(m_c4Bytes)
        );
    }
    return m_pc1Hash[c4ByteIndex];
}


tCIDLib::TBoolean TMsgHash::operator==(const TMsgHash& mhashSrc) const
{
    // Do the fastest test, for the number of bytes in the two hashes
    if (m_c4Bytes != mhashSrc.m_c4Bytes)
        return kCIDLib::False;

    // And then check for self comparison
    if (&mhashSrc == this)
        return kCIDLib::True;

    if (!TRawMem::bCompareMemBuf(m_pc1Hash, mhashSrc.m_pc1Hash, m_c4Bytes))
        return kCIDLib::False;

    return kCIDLib::True;
}


// Performs logical ops on each byte of this hash with the passed one
TMsgHash& TMsgHash::operator&=(const TMsgHash& mhashToAnd)
{
    if (m_c4Bytes != mhashToAnd.m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4Bytes)
            , TCardinal(mhashToAnd.m_c4Bytes)
        );
    }

    tCIDLib::TCard1* pc1This = m_pc1Hash;
    const tCIDLib::TCard1* pc1Src = mhashToAnd.m_pc1Hash;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        *pc1This &= *pc1Src;
        pc1This++;
        pc1Src++;
    }
    return *this;
}

TMsgHash& TMsgHash::operator|=(const TMsgHash& mhashToAnd)
{
    if (m_c4Bytes != mhashToAnd.m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4Bytes)
            , TCardinal(mhashToAnd.m_c4Bytes)
        );
    }

    tCIDLib::TCard1* pc1This = m_pc1Hash;
    const tCIDLib::TCard1* pc1Src = mhashToAnd.m_pc1Hash;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        *pc1This |= *pc1Src;
        pc1This++;
        pc1Src++;
    }
    return *this;
}

TMsgHash& TMsgHash::operator^=(const TMsgHash& mhashToAnd)
{
    if (m_c4Bytes != mhashToAnd.m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4Bytes)
            , TCardinal(mhashToAnd.m_c4Bytes)
        );
    }

    tCIDLib::TCard1* pc1This = m_pc1Hash;
    const tCIDLib::TCard1* pc1Src = mhashToAnd.m_pc1Hash;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        *pc1This ^= *pc1Src;
        pc1This++;
        pc1Src++;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMsgHash: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TMsgHash::c4Bytes() const
{
    return m_c4Bytes;
}


// Copy our bytes to the caller's buffer
tCIDLib::TCard4 TMsgHash::c4ToBuffer(TMemBuf& mbufToFill) const
{
    // Reallocate if needed
    if (m_c4Bytes > mbufToFill.c4Size())
        mbufToFill.Reallocate(m_c4Bytes, kCIDLib::False);

    mbufToFill.CopyIn(m_pc1Hash, m_c4Bytes);
    return m_c4Bytes;
}


// Copy out as many of our bytes as will fit to the caller's raw buffer
tCIDLib::TCard4
TMsgHash::c4ToRawArray(         tCIDLib::TCard1* const  pc1ToFill
                        , const tCIDLib::TCard4         c4MaxBytes) const
{
    // Make sure the caller's buffer can hold this
    if (c4MaxBytes < m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4MaxBytes)
            , TCardinal(m_c4Bytes)
        );
    }
    TRawMem::CopyMemBuf(pc1ToFill, m_pc1Hash, m_c4Bytes);
    return m_c4Bytes;
}


const tCIDLib::TCard1* TMsgHash::pc1Buffer() const
{
    return m_pc1Hash;
}


tCIDLib::TVoid
TMsgHash::RawFormat(TString& strToFill, const tCIDLib::TBoolean bAppend) const
{
    // If not appending, clear the string first
    if (!bAppend)
        strToFill.Clear();

    //
    //  Loop through the bytes of the hash and format each one as a two digit
    //  hex value. There is no space between them since as hash is supposed to
    //  be seen as a a long number.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        // Get out the current byte
        const tCIDLib::TCard1 c1Cur = m_pc1Hash[c4Index];

        // Get the first nibble
        tCIDLib::TCard1 c1Fmt = c1Cur >> 4;
        if (c1Fmt < 0xA)
            strToFill.Append(tCIDLib::TCh(kCIDLib::chDigit0 + c1Fmt));
        else
            strToFill.Append(tCIDLib::TCh(kCIDLib::chLatin_A + (c1Fmt - 10)));

        // And the second nibble
        c1Fmt = c1Cur & 0xF;
        if (c1Fmt < 0xA)
            strToFill.Append(tCIDLib::TCh(kCIDLib::chDigit0 + c1Fmt));
        else
            strToFill.Append(tCIDLib::TCh(kCIDLib::chLatin_A + (c1Fmt - 10)));
    }
}


tCIDLib::TVoid TMsgHash::Set(const  tCIDLib::TCard1* const  pc1Bytes
                            , const tCIDLib::TCard4         c4ProvidedBytes)
{
    // Make sure the data is the right size. If so, copy the data in
    if (c4ProvidedBytes != m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_NewBytesBadSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(m_c4Bytes)
            , TCardinal(c4ProvidedBytes)
        );
    }
    TRawMem::CopyMemBuf(m_pc1Hash, pc1Bytes, m_c4Bytes);
}

tCIDLib::TVoid TMsgHash::Set(const  TMemBuf&        mbufBytes
                            , const tCIDLib::TCard4 c4ProvidedBytes)

{
    // Make sure that the buffer has at least the indicated bytes
    if (mbufBytes.c4Size() < c4ProvidedBytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_InvalidByteCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(mbufBytes.c4Size())
            , TCardinal(c4ProvidedBytes)
        );
    }

    // Make sure that the provided bytes is the right size for this hash
    if (c4ProvidedBytes != m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_NewBytesBadSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(m_c4Bytes)
            , TCardinal(c4ProvidedBytes)
        );
    }

    // Looks ok, so copy out from the src buffer to our buffer
    mbufBytes.CopyOut(m_pc1Hash, m_c4Bytes);
}


tCIDLib::TVoid TMsgHash::Zero()
{
    // Just zero out all of the hash bytes
    TRawMem::SetMemBuf(m_pc1Hash, tCIDLib::TCard1(0), m_c4Bytes);
}


// ---------------------------------------------------------------------------
//  TMsgHash: Hidden constructors and operators
// ---------------------------------------------------------------------------
TMsgHash::TMsgHash( const   tCIDLib::TCard4 c4Bytes
                    , const tCIDLib::TCard4 c4Expected) :

    m_c4Bytes(c4Bytes)
    , m_pc1Hash(nullptr)
{
    // Make sure the indicated size is that expected
    if (c4Bytes != c4Expected)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_BadSrcBytes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Bytes)
            , TCardinal(c4Expected)
        );
    }

    // Allocate the buffer and zero it
    m_pc1Hash = new tCIDLib::TCard1[c4Bytes];
    TRawMem::SetMemBuf(m_pc1Hash, tCIDLib::TCard1(0), c4Bytes);
}

TMsgHash::TMsgHash( const   tCIDLib::TCard1* const  pc1Bytes
                    , const tCIDLib::TCard4         c4Bytes
                    , const tCIDLib::TCard4         c4Expected) :

    m_c4Bytes(c4Bytes)
    , m_pc1Hash(nullptr)
{
    // Make sure the provided bytes is that expected
    if (c4Bytes != c4Expected)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_BadSrcBytes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Bytes)
            , TCardinal(c4Expected)
        );
    }
    m_pc1Hash = new tCIDLib::TCard1[m_c4Bytes];
    TRawMem::CopyMemBuf(m_pc1Hash, pc1Bytes, m_c4Bytes);
}

TMsgHash::TMsgHash( const   TMemBuf&        mbufBytes
                    , const tCIDLib::TCard4 c4Expected) :
    m_c4Bytes(c4Expected)
    , m_pc1Hash(nullptr)
{
    // Make sure the buffer has at least the expected bytes
    if (mbufBytes.c4Size() < c4Expected)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_BadSrcBytes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(mbufBytes.c4Size())
            , TCardinal(c4Expected)
        );
    }

    // Allocate the buffer and copy out of the passed buffer
    m_pc1Hash = new tCIDLib::TCard1[m_c4Bytes];
    mbufBytes.CopyOut(m_pc1Hash, m_c4Bytes);
}

TMsgHash::TMsgHash(const TMsgHash& mhashSrc) :

    m_c4Bytes(mhashSrc.m_c4Bytes)
    , m_pc1Hash(nullptr)
{
    // Allocate the buffer and copy in its contents
    m_pc1Hash = new tCIDLib::TCard1[m_c4Bytes];
    TRawMem::CopyMemBuf(m_pc1Hash, mhashSrc.m_pc1Hash, m_c4Bytes);
}

TMsgHash::TMsgHash(TMsgHash&& mhashSrc) :

    TMsgHash()
{
    *this = tCIDLib::ForceMove(mhashSrc);
}

TMsgHash& TMsgHash::operator=(const TMsgHash& mhashSrc)
{
    if (&mhashSrc != this)
    {
        //
        //  If our byte sizes are the same, then just copy over the contents.
        //  Otherwise, we have to free our buffer and allocate another one.
        //
        if (m_c4Bytes != mhashSrc.m_c4Bytes)
        {
            delete [] m_pc1Hash;
            m_pc1Hash = nullptr;

            m_c4Bytes = mhashSrc.m_c4Bytes;
            m_pc1Hash = new tCIDLib::TCard1[m_c4Bytes];
        }
        TRawMem::CopyMemBuf(m_pc1Hash, mhashSrc.m_pc1Hash, m_c4Bytes);
    }
    return *this;
}

TMsgHash& TMsgHash::operator=(TMsgHash&& mhashSrc)
{
    if (&mhashSrc != this)
    {
        tCIDLib::Swap(m_c4Bytes, mhashSrc.m_c4Bytes);
        tCIDLib::Swap(m_pc1Hash, mhashSrc.m_pc1Hash);
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TMsgHash: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::THashVal TMsgHash::hshCalcHash(const tCIDLib::TCard4 c4Modulus) const
{
    return TRawMem::hshHashBuffer(m_pc1Hash, c4Modulus, m_c4Bytes);
}


tCIDLib::TCard1* TMsgHash::pc1Writeable()
{
    return m_pc1Hash;
}


// ---------------------------------------------------------------------------
//  TMsgHash: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMsgHash::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Check that we get a start of object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDCrypto_MsgHash::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Pull in the number of bytes
    tCIDLib::TCard4 c4NewBytes;
    strmToReadFrom >> c4NewBytes;

    // If its different from our current size, then resize the buffer
    if (c4NewBytes != m_c4Bytes)
    {
        delete [] m_pc1Hash;
        m_pc1Hash = nullptr;

        m_c4Bytes = c4NewBytes;
        m_pc1Hash = new tCIDLib::TCard1[m_c4Bytes];
    }

    // Stream in the hash bytes now
    strmToReadFrom.ReadArray(m_pc1Hash, m_c4Bytes);

    // And make sure it ends with the end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TMsgHash::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  Stream out our start object marker, and then our byte count. We also
    //  stream out our format version, to support later version update.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDCrypto_MsgHash::c2FmtVersion
                    << m_c4Bytes;

    // Then stream out the raw buffer
    strmToWriteTo.WriteArray(m_pc1Hash, m_c4Bytes);

    // And end with and end object marker
    strmToWriteTo   << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TMsgHash: Hidden constructor
// ---------------------------------------------------------------------------

// Just for use in the move constructor
TMsgHash::TMsgHash() :

    m_c4Bytes(1)
    , m_pc1Hash(new tCIDLib::TCard1[1])
{
    *m_pc1Hash = 0;
}
