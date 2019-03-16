//
// FILE NAME: CIDCrypto_SHA256Hash.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/09/2017
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
//  This file implements the TSHA256Hash class.
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
RTTIDecls(TSHA256Hash,TObject)
RTTIDecls(TSHA256KeyOps,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TSHA256KeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSHA256KeyOps: Constructors and destructor
// ---------------------------------------------------------------------------
TSHA256KeyOps::TSHA256KeyOps()
{
}

TSHA256KeyOps::~TSHA256KeyOps()
{
}


// ---------------------------------------------------------------------------
//  TSHA256KeyOps: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::THashVal
TSHA256KeyOps::hshKey(const TSHA256Hash&    mhashToHash
                    , const tCIDLib::TCard4 c4Modulus) const
{
    // Just ask the hash object to hash itself
    return mhashToHash.hshCalcHash(c4Modulus);
}




// ---------------------------------------------------------------------------
//   CLASS: TSHA256Hash
//  PREFIX: mhash
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSHA256Hash: Constructors and Destructor
// ---------------------------------------------------------------------------
TSHA256Hash::TSHA256Hash() :

    TMsgHash(kCIDCrypto::c4SHA256HashBytes, kCIDCrypto::c4SHA256HashBytes)
{
}

TSHA256Hash::TSHA256Hash(const  tCIDLib::TCard1* const  pc1Bytes
                        , const tCIDLib::TCard4         c4Bytes) :

    TMsgHash(pc1Bytes, c4Bytes, kCIDCrypto::c4SHA256HashBytes)
{
}

TSHA256Hash::TSHA256Hash(const TMemBuf& mbufBytes) :

    TMsgHash(mbufBytes, kCIDCrypto::c4SHA256HashBytes)
{
}

TSHA256Hash::TSHA256Hash(TSHA256Hash&& mhashSrc) :

    TSHA256Hash()
{
    *this = tCIDLib::ForceMove(mhashSrc);
}

TSHA256Hash::~TSHA256Hash()
{
}


// ---------------------------------------------------------------------------
//  TSHA256Hash: Public operators
// ---------------------------------------------------------------------------

TSHA256Hash& TSHA256Hash::operator=(TSHA256Hash&& mhashSrc)
{
    if (this != &mhashSrc)
        TParent::operator=(tCIDLib::ForceMove(mhashSrc));
    return *this;
}


tCIDLib::TBoolean TSHA256Hash::operator==(const TSHA256Hash& mhashSrc) const
{
    //  Just pass to our parent
    return TParent::operator==(mhashSrc);
}

tCIDLib::TBoolean TSHA256Hash::operator!=(const TSHA256Hash& mhashSrc) const
{
    // Just pass to our parent
    return !TParent::operator==(mhashSrc);
}


// ---------------------------------------------------------------------------
//  TSHA256Hash: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TSHA256Hash::FormatToStr(TString& strToFill, const tCIDLib::TBoolean bAppend) const
{
    // If not appending, clear the string first
    if (!bAppend)
        strToFill.Clear();

    //
    //  We just format out the bytes in hex
    //
    CIDAssert(c4Bytes() == kCIDCrypto::c4SHA256HashBytes, L"Invalid SHA256 hash length");

    const tCIDLib::TCard1* pc1Cur = pc1Buffer();
    TString strVal;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDCrypto::c4SHA256HashBytes; c4Index++)
    {
        const tCIDLib::TCard1 c1Cur = *pc1Cur++;
        if (c1Cur < 0x10)
            strToFill.Append(kCIDLib::chDigit0);
        strToFill.AppendFormatted(c1Cur, tCIDLib::ERadices::Hex);
    }
}


// ---------------------------------------------------------------------------
//  TSHA256Hash: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSHA256Hash::ParseFormatted(const TString& strFormatted)
{
    // We just get a string of 64 characters, each two being a hex byte
    if (strFormatted.c4Length() != kCIDCrypto::c4SHA256HashBytes * 2)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcFmt_IdStrFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"SHA256")
        );
    }

    // Make sure the raw buffer we are going to fill in has the right byte count
    CIDAssert
    (
        c4Bytes() == kCIDCrypto::c4SHA256HashBytes
        , L"The SHA256 has byte size is not correct"
    );

    // Get the raw buffer to work with
    tCIDLib::TCard1* pc1This = pc1Writeable();

    tCIDLib::TCard1 c1Cur;
    tCIDLib::TCard4 c4SrcInd = 0;
    tCIDLib::TCard4 c4OutInd = 0;
    while (c4SrcInd < kCIDCrypto::c4SHA256HashBytes * 2)
    {
        // Get the next two chars out into temps and upper case them
        const tCIDLib::TCh ch1 = TRawStr::chUpper(strFormatted[c4SrcInd]);
        const tCIDLib::TCh ch2 = TRawStr::chUpper(strFormatted[c4SrcInd + 1]);

        // Do the first nibble
        if ((ch1 >= kCIDLib::chDigit0) && (ch1 <= kCIDLib::chDigit9))
            c1Cur = tCIDLib::TCard1(ch1 - kCIDLib::chDigit0);
        else if ((ch1 >= kCIDLib::chLatin_A) && (ch1 <= kCIDLib::chLatin_Z))
            c1Cur = tCIDLib::TCard1(10 + (ch1 - kCIDLib::chLatin_A));
        else
        {
            facCIDCrypto().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCryptoErrs::errcFmt_IdStrFmt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"SHA5")
            );
        }

        // Shift up and do the low nibble
        c1Cur <<= 4;
        if ((ch2 >= kCIDLib::chDigit0) && (ch2 <= kCIDLib::chDigit9))
            c1Cur |= tCIDLib::TCard1(ch2 - kCIDLib::chDigit0);
        else if ((ch2 >= kCIDLib::chLatin_A) && (ch2 <= kCIDLib::chLatin_Z))
            c1Cur |= tCIDLib::TCard1(10 + (ch2 - kCIDLib::chLatin_A));
        else
        {
            facCIDCrypto().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCryptoErrs::errcFmt_IdStrFmt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"SHA5")
            );
        }

        // Store the new byte and bump the output index
        pc1This[c4OutInd++] = c1Cur;

        // Move up in the source
        c4SrcInd += 2;
    }

    CIDAssert
    (
        c4OutInd == kCIDCrypto::c4SHA256HashBytes
        , L"The parsed SHA256 hash came out the wrong size"
    );
}


// ---------------------------------------------------------------------------
//  TSHA256Hash: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSHA256Hash::FormatTo(TTextOutStream& strmDest) const
{
    TString strTmp(45UL);
    FormatToStr(strTmp);
    strmDest << strTmp;
}


