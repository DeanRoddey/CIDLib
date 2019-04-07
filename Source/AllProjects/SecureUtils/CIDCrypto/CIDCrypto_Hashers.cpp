//
// FILE NAME: CIDCrypto_Hashers.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/24/2001
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
//  This file contains the little bit of concrete implemention of the
//  abstract hasher base class.
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
RTTIDecls(THashDigest,TObject)



// ---------------------------------------------------------------------------
//   CLASS: THashDisgest
//  PREFIX: mdig
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THashDigest: Constructors and Destructor
// ---------------------------------------------------------------------------
THashDigest::~THashDigest()
{
}


// ---------------------------------------------------------------------------
//  THashDigest: Public, non-virtual methods
// ---------------------------------------------------------------------------

// The derived class tells us his block size and we make that available
tCIDLib::TCard4 THashDigest::c4BlockSize() const
{
    return m_c4BlockSize;
}


//
//  All of these are just conveniences which work in terms of the two virtual
//  methods
//
tCIDLib::TVoid
THashDigest::DigestText(const   tCIDLib::TCh* const     pszToDigest
                        , const tCIDLib::TCard4         c4Length)
{
    //
    //  If the passed length is zero, then get the length of the passed string.
    //  Otherwise, take the passed length.
    //
    const tCIDLib::TCard4 c4Len = c4Length ? c4Length
                                           : TRawStr::c4StrLen(pszToDigest);

    //
    //  We have to insure that this gets the same result on any platform,
    //  regardless of endianness or size of TCh. So we transcode to UTF-8
    //  format, because it is byte order and char size independent.
    //
    TUTF8Converter  tcvtKey;
    TExpByteBuf     expbTmp(c4Len * 6);
    tcvtKey.c4ConvertTo(pszToDigest, c4Len, expbTmp);

    // And digest the bytes
    DigestRaw(expbTmp.pc1Buffer(), expbTmp.c4Bytes());
}

tCIDLib::TVoid THashDigest::DigestStr(const TString& strToDigest)
{
    //
    //  We have to insure that this gets the same result on any platform,
    //  regardless of endianness or size of TCh. So we transcode to UTF-8
    //  format, because it is byte order and char size independent.
    //
    TUTF8Converter  tcvtKey;
    TExpByteBuf     expbTmp(strToDigest.c4Length() * 6);
    tcvtKey.c4ConvertTo
    (
        strToDigest.pszBuffer(), strToDigest.c4Length(), expbTmp
    );

    DigestRaw(expbTmp.pc1Buffer(), expbTmp.c4Bytes());
}


tCIDLib::TVoid THashDigest::DigestBuf(const TMemBuf& mbufToDigest)
{
    // Just do the whole buffer
    DigestRaw(mbufToDigest.pc1Data(), mbufToDigest.c4Size());
}

tCIDLib::TVoid
THashDigest::DigestBuf( const   TMemBuf&        mbufToDigest
                        , const tCIDLib::TCard4 c4Bytes)
{
    if (c4Bytes > mbufToDigest.c4Size())
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcHash_InvalidByteCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufToDigest.c4Size())
            , TCardinal(c4Bytes)
        );
    }

    DigestRaw(mbufToDigest.pc1Data(), c4Bytes);
}


// ---------------------------------------------------------------------------
//  THashDigest: Hidden constructors
// ---------------------------------------------------------------------------
THashDigest::THashDigest(const tCIDLib::TCard4 c4BlockSize) :

    m_c4BlockSize(c4BlockSize)
{
}
