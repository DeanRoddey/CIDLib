//
// FILE NAME: CIDCrypto_SHA1Hash.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/19/2009
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
//  This file implements the TSHA1Hash class.
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
RTTIDecls(TSHA1Hash,TObject)
RTTIDecls(TSHA1KeyOps,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TSHA1KeyOps
//  PREFIX: kops
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSHA1KeyOps: Constructors and destructor
// ---------------------------------------------------------------------------
TSHA1KeyOps::TSHA1KeyOps()
{
}

TSHA1KeyOps::~TSHA1KeyOps()
{
}


// ---------------------------------------------------------------------------
//  TSHA1KeyOps: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::THashVal
TSHA1KeyOps::hshKey(const   TSHA1Hash&       mhashToHash
                    , const tCIDLib::TCard4 c4Modulus) const
{
    // Just ask the hash object to hash itself
    return mhashToHash.hshCalcHash(c4Modulus);
}




// ---------------------------------------------------------------------------
//   CLASS: TSHA1Hash
//  PREFIX: mhash
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSHA1Hash: Constructors and Destructor
// ---------------------------------------------------------------------------
TSHA1Hash::TSHA1Hash() :

    TMsgHash(kCIDCrypto::c4SHA1HashBytes, kCIDCrypto::c4SHA1HashBytes)
{
}

TSHA1Hash::TSHA1Hash(const  tCIDLib::TCard1* const  pc1Bytes
                    , const tCIDLib::TCard4         c4Bytes) :

    TMsgHash(pc1Bytes, c4Bytes, kCIDCrypto::c4SHA1HashBytes)
{
}

TSHA1Hash::TSHA1Hash(const TMemBuf& mbufBytes) :

    TMsgHash(mbufBytes, kCIDCrypto::c4SHA1HashBytes)
{
}

TSHA1Hash::TSHA1Hash(TSHA1Hash&& mhashSrc) :

    TSHA1Hash()
{
    *this = tCIDLib::ForceMove(mhashSrc);
}

TSHA1Hash::~TSHA1Hash()
{
}


// ---------------------------------------------------------------------------
//  TSHA1Hash: Public operators
// ---------------------------------------------------------------------------
TSHA1Hash& TSHA1Hash::operator=(TSHA1Hash&& mhashSrc)
{
    if (this != &mhashSrc)
        TParent::operator=(tCIDLib::ForceMove(mhashSrc));
    return *this;
}


tCIDLib::TBoolean TSHA1Hash::operator==(const TSHA1Hash& mhashSrc) const
{
    //
    //  Just pass to our parents. We provide our own type in order to enforce
    //  type safety on callers.
    //
    return TParent::operator==(mhashSrc);
}

tCIDLib::TBoolean TSHA1Hash::operator!=(const TSHA1Hash& mhashSrc) const
{
    //
    //  Just pass to our parents. We provide our own type in order to enforce
    //  type safety on callers.
    //
    return !TParent::operator==(mhashSrc);
}


// ---------------------------------------------------------------------------
//  TSHA1Hash: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TSHA1Hash::FormatToStr(TString& strToFill, const tCIDLib::TBoolean bAppend) const
{
    // If not appending, clear the string first
    if (!bAppend)
        strToFill.Clear();

    //
    //  We format out the bytes in 5 digit blocks, each zero leading if needed,
    //  and with a space between each block.
    //
    CIDAssert(c4Bytes() == kCIDCrypto::c4SHA1HashBytes, L"Invalid SHA1 hash length");

    const tCIDLib::TCard4* pc4Cur
    (
        reinterpret_cast<const tCIDLib::TCard4*>(pc1Buffer())
    );

    TString strVal;
    TString strFld;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 5; c4Index++)
    {
        if (c4Index)
            strToFill.Append(kCIDLib::chSpace);

        //
        //  Deal with endianness. It's stored big endian, so we have to swap
        //  if we are on a little endian machine.
        //
        #if defined(CIDLIB_LITTLEENDIAN)
        strVal.SetFormatted(TRawBits::c4SwapBytes(*pc4Cur), tCIDLib::ERadices::Hex);
        #else
        strVal.SetFormatted(*pc4Cur, tCIDLib::ERadices::Hex);
        #endif

        strFld.FormatToFld(strVal, 8, tCIDLib::EHJustify::Right, kCIDLib::chDigit0);
        strToFill.Append(strFld);

        pc4Cur++;
    }
}


// ---------------------------------------------------------------------------
//  TSHA1Hash: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSHA1Hash::ParseFormatted(const TString& strFormatted)
{
    //
    //  We should get five chunks of 32 bit hex values, space separated, and
    //  left zero filled. So it should be a specific length with spaces at the
    //  expected positions.
    //
    if ((strFormatted.c4Length() != 44)
    ||  (strFormatted[8] != kCIDLib::chSpace)
    ||  (strFormatted[17] != kCIDLib::chSpace)
    ||  (strFormatted[26] != kCIDLib::chSpace)
    ||  (strFormatted[35] != kCIDLib::chSpace))
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcFmt_IdStrFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"SHA1")
        );
    }

    // Extract the hex card 4 values at the expected places
    try
    {
        TString strTmp;

        tCIDLib::TCard4 ac4Vals[5];
        strFormatted.CopyOutSubStr(strTmp, 0, 8);
        ac4Vals[0] = strTmp.c4Val(tCIDLib::ERadices::Hex);

        strFormatted.CopyOutSubStr(strTmp, 9, 8);
        ac4Vals[1] = strTmp.c4Val(tCIDLib::ERadices::Hex);

        strFormatted.CopyOutSubStr(strTmp, 18, 8);
        ac4Vals[2] = strTmp.c4Val(tCIDLib::ERadices::Hex);

        strFormatted.CopyOutSubStr(strTmp, 27, 8);
        ac4Vals[3] = strTmp.c4Val(tCIDLib::ERadices::Hex);

        strFormatted.CopyOutSubStr(strTmp, 36, 8);
        ac4Vals[4] = strTmp.c4Val(tCIDLib::ERadices::Hex);

        // Flip them if on a little endian machine
        #if defined(CIDLIB_LITTLEENDIAN)
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 5; c4Index++)
            ac4Vals[c4Index] = TRawBits::c4SwapBytes(ac4Vals[c4Index]);
        #endif

        // Now copy the whole thing as a byte array to our parent
        Set((tCIDLib::TCard1*)ac4Vals, kCIDCrypto::c4SHA1HashBytes);
    }

    catch(...)
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcFmt_IdStrFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"SHA1")
        );
    }
}


// ---------------------------------------------------------------------------
//  TSHA1Hash: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSHA1Hash::FormatTo(TTextOutStream& strmDest) const
{
    TString strTmp(45UL);
    FormatToStr(strTmp);
    strmDest << strTmp;
}


