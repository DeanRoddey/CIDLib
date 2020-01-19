//
// FILE NAME: CIDCrypto_MD5Hash.cpp
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
//  This file implements the TMD5Hash class.
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
RTTIDecls(TMD5Hash,TObject)


namespace CIDCrypto_MD5Hash
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  Local constants
        //
        //  c4HyphenOfs
        //      The offset of the hypen in the formatted unique id string.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4HyphenOfs  = 16;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TMD5Hash
//  PREFIX: mhash
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMD5Hash: Constructors and Destructor
// ---------------------------------------------------------------------------
TMD5Hash::TMD5Hash() :

    TMsgHash(kCIDCrypto::c4MD5HashBytes, kCIDCrypto::c4MD5HashBytes)
{
}

TMD5Hash::TMD5Hash( const   tCIDLib::TCard1* const  pc1Bytes
                    , const tCIDLib::TCard4         c4Bytes) :

    TMsgHash(pc1Bytes, c4Bytes, kCIDCrypto::c4MD5HashBytes)
{
}

TMD5Hash::TMD5Hash(const TMemBuf& mbufBytes) :

    TMsgHash(mbufBytes, kCIDCrypto::c4MD5HashBytes)
{
}

TMD5Hash::TMD5Hash(TMD5Hash&& mhashSrc) :

    TMD5Hash()
{
    *this = tCIDLib::ForceMove(mhashSrc);
}

TMD5Hash::~TMD5Hash()
{
}


// ---------------------------------------------------------------------------
//  TMD5Hash: Public operators
// ---------------------------------------------------------------------------
TMD5Hash& TMD5Hash::operator=(TMD5Hash&& mhashSrc)
{
    if (this != &mhashSrc)
        TParent::operator=(tCIDLib::ForceMove(mhashSrc));
    return *this;
}


tCIDLib::TBoolean TMD5Hash::operator==(const TMD5Hash& mhashSrc) const
{
    //
    //  Just pass to our parents. We provide our own type in order to enforce
    //  type safety on callers.
    //
    return TParent::operator==(mhashSrc);
}

tCIDLib::TBoolean TMD5Hash::operator!=(const TMD5Hash& mhashSrc) const
{
    //
    //  Just pass to our parents. We provide our own type in order to enforce
    //  type safety on callers.
    //
    return !TParent::operator==(mhashSrc);
}


// ---------------------------------------------------------------------------
//  TMD5Hash: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMD5Hash::FormatToStr(TString& strToFill, const tCIDLib::TBoolean bAppend) const
{
    //
    //  Do the raw format into the caller's string, then insert the hyphen. We
    //  have to account for possibly appending to the caller's string. If not,
    //  we can set the original length to 0, else we remember the current
    //  length so that we can insert the hyphen at the right place into the
    //  appended new content.
    //
    const tCIDLib::TCard4 c4OrgLen = bAppend ? strToFill.c4Length() : 0;
    RawFormat(strToFill, bAppend);
    strToFill.Insert
    (
        kCIDLib::chHyphenMinus, CIDCrypto_MD5Hash::c4HyphenOfs + c4OrgLen
    );
}



// ---------------------------------------------------------------------------
//  TMD5Hash: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMD5Hash::ParseFormatted(const TString& strFormatted)
{
    // Get a raw buffer to work with
    tCIDLib::TCard1* pc1This = pc1HashW();

    // Check the length and that it has a hypen at the correct place
    if ((strFormatted.c4Length() != kCIDCrypto::c4UniqueIdStrLen)
    ||  (strFormatted[CIDCrypto_MD5Hash::c4HyphenOfs] != kCIDLib::chHyphenMinus))
    {
        facCIDCrypto().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCryptoErrs::errcFmt_IdStrFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"MD5")
        );
    }

    //
    //  Ok, lets loop through. Each two characters will be converted into a
    //  single hex digit.
    //
    tCIDLib::TCard1 c1Cur = 0;
    tCIDLib::TCard4 c4SrcInd = 0;
    tCIDLib::TCard4 c4OutInd = 0;
    while (c4SrcInd < kCIDCrypto::c4UniqueIdStrLen)
    {
        // Skip the index where the hyphen is
        if (c4SrcInd == CIDCrypto_MD5Hash::c4HyphenOfs)
        {
            c4SrcInd++;
            continue;
        }

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
                , TString(L"MD5")
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
                , TString(L"MD5")
            );
        }

        // Store the new byte and bump the output index
        pc1This[c4OutInd++] = c1Cur;

        // Move up in the source
        c4SrcInd += 2;
    }
}


// ---------------------------------------------------------------------------
//  TMD5Hash: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMD5Hash::FormatTo(TTextOutStream& strmDest) const
{
    // Do the raw format into a temp string
    TString strTmp(kCIDCrypto::c4UniqueIdStrLen);
    RawFormat(strTmp);
    strTmp.Insert(kCIDLib::chHyphenMinus, CIDCrypto_MD5Hash::c4HyphenOfs);

    // And then format that into the stream
    strmDest << strTmp;
}

