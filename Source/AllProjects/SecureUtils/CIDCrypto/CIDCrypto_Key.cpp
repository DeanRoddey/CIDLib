//
// FILE NAME: CIDCrypto_Key.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/24/1997
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
//  This file implements the TCryptoKey class, which is a simple class to
//  abstract the concept of encryption keys.
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
AdvRTTIDecls(TCryptoKey, TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDCrypto_Key
{
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//   CLASS: TCryptoKey
//  PREFIX: ckey
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCryptoKey: Constructors and Destructor
// ---------------------------------------------------------------------------
TCryptoKey::TCryptoKey(const tCIDLib::TCard4 c4KeyBytes) :

    m_c4Bytes(c4KeyBytes)
    , m_pc1Buffer(nullptr)
{
    m_pc1Buffer = new tCIDLib::TCard1[c4KeyBytes];
    TRawMem::SetMemBuf(m_pc1Buffer, tCIDLib::TCard1(0), c4KeyBytes);
}

TCryptoKey::TCryptoKey(const TString& strKey) :

    m_c4Bytes(0)
    , m_pc1Buffer(nullptr)
{
    if (strKey.bIsEmpty())
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcKey_EmptyKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  We have to make this platform independent. So we will transcode the
    //  text to UTF-8 format, which is byte order independent. So we format
    //  into a temp memory buffer object first, then allocate our buffer when
    //  we know how much we need.
    //
    TUTF8Converter  tcvtKey;
    TExpByteBuf     expbTmp(strKey.c4Length() * 6);
    m_c4Bytes = tcvtKey.c4ConvertTo(strKey.pszBuffer(), strKey.c4Length(), expbTmp);

    // Now we can allocate the buffer and store it
    m_pc1Buffer = new tCIDLib::TCard1[expbTmp.c4Bytes()];
    TRawMem::CopyMemBuf(m_pc1Buffer, expbTmp.pc1Buffer(), expbTmp.c4Bytes());
}

TCryptoKey::TCryptoKey(const TMemBuf& mbufKey, const tCIDLib::TCard4 c4KeyBytes) :

    m_c4Bytes(0)
    , m_pc1Buffer(nullptr)
{
    Set(mbufKey, c4KeyBytes);
}

TCryptoKey::TCryptoKey(const TMsgHash& mhashBytes) :

    m_c4Bytes(mhashBytes.c4Bytes())
    , m_pc1Buffer(nullptr)
{
    m_pc1Buffer = new tCIDLib::TCard1[m_c4Bytes];
    mhashBytes.c4ToRawArray(m_pc1Buffer, m_c4Bytes);
}

TCryptoKey::TCryptoKey( const   tCIDLib::TCard1* const  apc1KeyBytes
                        , const tCIDLib::TCard4         c4KeyBytes) :
    m_c4Bytes(c4KeyBytes)
    , m_pc1Buffer(nullptr)
{
    m_pc1Buffer = new tCIDLib::TCard1[c4KeyBytes];
    TRawMem::CopyMemBuf(m_pc1Buffer, apc1KeyBytes, c4KeyBytes);
}

TCryptoKey::TCryptoKey(const TCryptoKey& ckeySrc) :

    m_c4Bytes(ckeySrc.m_c4Bytes)
    , m_pc1Buffer(nullptr)
{
    m_pc1Buffer = new tCIDLib::TCard1[m_c4Bytes];
    TRawMem::CopyMemBuf(m_pc1Buffer, ckeySrc.m_pc1Buffer, m_c4Bytes);
}

TCryptoKey::TCryptoKey(TCryptoKey&& ckeySrc) :

    TCryptoKey(2)
{
    *this = tCIDLib::ForceMove(ckeySrc);
}

TCryptoKey::~TCryptoKey()
{
    //
    //  For security, make sure that we zero out the memory where the key
    //  was, so that its not hanging around in memory.
    //
    if (m_pc1Buffer)
    {
        TRawMem::SetMemBuf(m_pc1Buffer, tCIDLib::TCard1(0), m_c4Bytes);
        delete [] m_pc1Buffer;
        m_pc1Buffer = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TCryptoKey: Public operators
// ---------------------------------------------------------------------------
TCryptoKey& TCryptoKey::operator=(const TCryptoKey& ckeySrc)
{
    if (this != &ckeySrc)
    {
        // Reallocate the buffer if its not the same size
        if (m_c4Bytes != ckeySrc.m_c4Bytes)
        {
            delete [] m_pc1Buffer;
            m_pc1Buffer = nullptr;
            m_c4Bytes = ckeySrc.m_c4Bytes;
            m_pc1Buffer = new tCIDLib::TCard1[m_c4Bytes];
        }
        TRawMem::CopyMemBuf(m_pc1Buffer, ckeySrc.m_pc1Buffer, m_c4Bytes);
    }
    return *this;
}

TCryptoKey& TCryptoKey::operator=(TCryptoKey&& ckeySrc)
{
    if (this != &ckeySrc)
    {
        tCIDLib::Swap(m_c4Bytes, ckeySrc.m_c4Bytes);
        tCIDLib::Swap(m_pc1Buffer, ckeySrc.m_pc1Buffer);
    }
    return *this;
}


tCIDLib::TBoolean TCryptoKey::operator==(const TCryptoKey& ckeySrc) const
{
    if (this != &ckeySrc)
    {
        if ((m_c4Bytes != ckeySrc.m_c4Bytes)
        ||  !TRawMem::bCompareMemBuf(m_pc1Buffer, ckeySrc.m_pc1Buffer, m_c4Bytes))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TCryptoKey::operator!=(const TCryptoKey& ckeySrc) const
{
    return !operator==(ckeySrc);
}


tCIDLib::TCard1 TCryptoKey::operator[](const tCIDLib::TCard4 c4Index) const
{
    if (c4Index >= m_c4Bytes)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , clsIsA()
            , TCardinal(m_c4Bytes)
        );
    }
    return m_pc1Buffer[c4Index];
}

tCIDLib::TCard1& TCryptoKey::operator[](const tCIDLib::TCard4 c4Index)
{
    if (c4Index >= m_c4Bytes)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , clsIsA()
            , TCardinal(m_c4Bytes)
        );
    }
    return m_pc1Buffer[c4Index];
}


// Performs logical ops on each byte of this hash with the passed one
TCryptoKey& TCryptoKey::operator&=(const TCryptoKey& ckeyToAnd)
{
    if (m_c4Bytes != ckeyToAnd.m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcKey_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4Bytes)
            , TCardinal(ckeyToAnd.m_c4Bytes)
        );
    }

    tCIDLib::TCard1* pc1This = m_pc1Buffer;
    const tCIDLib::TCard1* pc1Src = ckeyToAnd.m_pc1Buffer;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        *pc1This &= *pc1Src;
        pc1This++;
        pc1Src++;
    }
    return *this;
}

TCryptoKey& TCryptoKey::operator|=(const TCryptoKey& ckeyToAnd)
{
    if (m_c4Bytes != ckeyToAnd.m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcKey_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4Bytes)
            , TCardinal(ckeyToAnd.m_c4Bytes)
        );
    }

    tCIDLib::TCard1* pc1This = m_pc1Buffer;
    const tCIDLib::TCard1* pc1Src = ckeyToAnd.m_pc1Buffer;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        *pc1This |= *pc1Src;
        pc1This++;
        pc1Src++;
    }
    return *this;
}

TCryptoKey& TCryptoKey::operator^=(const TCryptoKey& ckeyToAnd)
{
    if (m_c4Bytes != ckeyToAnd.m_c4Bytes)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcKey_DifferentSizes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4Bytes)
            , TCardinal(ckeyToAnd.m_c4Bytes)
        );
    }

    tCIDLib::TCard1* pc1This = m_pc1Buffer;
    const tCIDLib::TCard1* pc1Src = ckeyToAnd.m_pc1Buffer;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        *pc1This ^= *pc1Src;
        pc1This++;
        pc1Src++;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TCryptoKey: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TCryptoKey::c4Bytes() const
{
    return m_c4Bytes;
}


tCIDLib::TVoid
TCryptoKey::FormatKey(          TString&            strToFill
                        , const tCIDLib::TCh        chSepChar
                        , const tCIDLib::TBoolean   bAppend)
{
    if (!bAppend)
        strToFill.Clear();

    const tCIDLib::TCard1* pc1Src = m_pc1Buffer;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Bytes; c4Index++)
    {
        if (c4Index && chSepChar)
            strToFill.Append(chSepChar);

        const tCIDLib::TCard1 c1Byte = *pc1Src++;
        if (c1Byte < 0x10)
            strToFill.Append(kCIDLib::chDigit0);
        strToFill.AppendFormatted(c1Byte, tCIDLib::ERadices::Hex);
    }
}


const tCIDLib::TCard1* TCryptoKey::pc1Buffer() const
{
    return m_pc1Buffer;
}


tCIDLib::TVoid TCryptoKey::Reset()
{
    m_c4Bytes = 0;
}


tCIDLib::TVoid TCryptoKey::Set( const   TMemBuf&        mbufKey
                                , const tCIDLib::TCard4 c4KeyBytes)
{
    tCIDLib::TCard4 c4ActualBytes = c4KeyBytes;

    // If zero, then use the current bytes in buffer
    if (!c4ActualBytes)
    {
        c4ActualBytes = mbufKey.c4Size();

        // If that's zero too, then an error
        if (!c4ActualBytes)
        {
            facCIDCrypto().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCryptoErrs::errcKey_EmptyKey
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
            );
        }
    }
    else if (c4ActualBytes > mbufKey.c4Size())
    {
        // The bytes indicated is larger than the data supplied
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcKey_BadKeyBytes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4ActualBytes)
            , TCardinal(mbufKey.c4Size())
        );
    }

    //
    //  If our buffer isn't the same size, then reallocate. If the ctor
    //  called this, then m_c4Bytes will be zero so we'll allocate.
    //
    if (m_c4Bytes != c4ActualBytes)
    {
        delete [] m_pc1Buffer;
        m_c4Bytes = c4ActualBytes;
        m_pc1Buffer = new tCIDLib::TCard1[c4KeyBytes];
    }
    TRawMem::CopyMemBuf(m_pc1Buffer, mbufKey.pc1Data(), m_c4Bytes);
}

tCIDLib::TVoid TCryptoKey::Set( const   tCIDLib::TCard1* const  apc1KeyBytes
                                , const tCIDLib::TCard4         c4KeyBytes)
{
    // If our buffer isn't the same size, then reallocate
    if (m_c4Bytes != c4KeyBytes)
    {
        delete [] m_pc1Buffer;
        m_c4Bytes = c4KeyBytes;
        m_pc1Buffer = new tCIDLib::TCard1[m_c4Bytes];
    }
    TRawMem::CopyMemBuf(m_pc1Buffer, apc1KeyBytes, c4KeyBytes);
}

tCIDLib::TVoid TCryptoKey::Set(const TMsgHash& mhashBytes)
{
    // If our buffer isn't the same size, then reallocate
    if (m_c4Bytes != mhashBytes.c4Bytes())
    {
        delete [] m_pc1Buffer;
        m_c4Bytes = mhashBytes.c4Bytes();
        m_pc1Buffer = new tCIDLib::TCard1[m_c4Bytes];
    }
    mhashBytes.c4ToRawArray(m_pc1Buffer, m_c4Bytes);
}

tCIDLib::TVoid TCryptoKey::Set(const TString& strKey)
{
    if (strKey.bIsEmpty())
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcKey_EmptyKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  We have to make this platform independent. So we will transcode the
    //  text to UTF-8 format, which is byte order independent. So we format
    //  into a temp memory buffer object first, then allocate our buffer when
    //  we know how much we need.
    //
    TUTF8Converter  tcvtKey;
    TExpByteBuf     expbTmp(strKey.c4Length() * 6);
    tcvtKey.c4ConvertTo
    (
        strKey.pszBuffer()
        , strKey.c4Length()
        , expbTmp
    );

    if (m_c4Bytes != expbTmp.c4Bytes())
    {
        delete [] m_pc1Buffer;
        m_c4Bytes = expbTmp.c4Bytes();
        m_pc1Buffer = new tCIDLib::TCard1[m_c4Bytes];
    }

    TRawMem::CopyMemBuf(m_pc1Buffer, expbTmp.pc1Buffer(), m_c4Bytes);
}


// ---------------------------------------------------------------------------
//  TCryptoKey: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCryptoKey::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Check that we get a start of object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDCrypto_Key::c2FmtVersion)
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

    // Stream in the key bytes to a temp
    tCIDLib::TCard4 c4TmpBytes;
    strmToReadFrom >> c4TmpBytes;

    // If the bytes are different, then reallocate our buffer
    if (c4TmpBytes != m_c4Bytes)
    {
        delete [] m_pc1Buffer;
        m_pc1Buffer = 0;
        m_pc1Buffer = new tCIDLib::TCard1[c4TmpBytes];
        m_c4Bytes = c4TmpBytes;
    }

    // And now read in the new data
    strmToReadFrom.ReadArray(m_pc1Buffer, m_c4Bytes);

    // And it should end with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TCryptoKey::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream out a start of object stream marker
    strmToWriteTo << tCIDLib::EStreamMarkers::StartObject;

    //
    //  Stream out the count of key bytes and then the bytes of data. We also
    //  stream out or format version to support later automatic data upgrading.
    //
    strmToWriteTo   << CIDCrypto_Key::c2FmtVersion
                    << m_c4Bytes;
    strmToWriteTo.WriteArray(m_pc1Buffer, m_c4Bytes);

    // And end up with an end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}

