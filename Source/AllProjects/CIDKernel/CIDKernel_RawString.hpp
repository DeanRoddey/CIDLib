//
// FILE NAME: CIDKernel_RawStrings.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
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
//  This is the header for the CIDKernel_RawStrings.Cpp file, which provides
//  APIs for manipulating raw strings. Almost all of these methods are
//  designed to work on the the default wide character type. There are a
//  couple though designed to convert between the wide character type and
//  the local code page encoding.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TRawStr
{
    KRNLEXPORT tCIDLib::TBoolean bFormatStr
    (
        const   tCIDLib::TCh* const     pszVal
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4FldWidth
        , const tCIDLib::TCh            chFill
        , const tCIDLib::EHJustify      eJustify
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard1         c1Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard2         c2Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard4         c4Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard8&        c8Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt1          i1Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt2          i2Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt4          i4Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt8&         i8Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TUInt          uVal
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TFloat4&       f4Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4Precision
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ETrailFmts     eTrail = tCIDLib::ETrailFmts::Ignore
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCh            chDecimal = kCIDLib::chPeriod
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TFloat8&       f8Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4Precision
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ETrailFmts     eTrail = tCIDLib::ETrailFmts::Ignore
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCh            chDecimal = kCIDLib::chPeriod
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    );

    KRNLEXPORT tCIDLib::TBoolean bIsAlpha
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bIsAlphaNum
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bIsDigit
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bIsLower
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bIsPunct
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bIsSpace
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bIsUpper
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bIsHexDigit
    (
        const   tCIDLib::TCh            chToTest
    );

    KRNLEXPORT tCIDLib::TBoolean bMakeHexByte
    (
        const   tCIDLib::TCh            ch1
        , const tCIDLib::TCh            ch2
        ,       tCIDLib::TCard1&        c1ToFill
    );

    KRNLEXPORT tCIDLib::TCard4 c4AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        , COP   tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT tCIDLib::TCard4 c4ToHex
    (
        const   tCIDLib::TCh            chConvert
    );

    KRNLEXPORT tCIDLib::TCard8 c8AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        , COP   tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT tCIDLib::TCh chLower
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT tCIDLib::TCh chUpper
    (
        const   tCIDLib::TCh            cchToTest
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid CatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCh* const     pszSrc
        , const tCIDLib::TCard4         c4MaxChars = 0
    );

    KRNLEXPORT tCIDLib::TVoid CopyCatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::TCh* const     pszCopy
        , const tCIDLib::TCh* const     pszCat
    );

    KRNLEXPORT tCIDLib::TVoid CopyCatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::TCh* const     pszCopy
        , const tCIDLib::TCh* const     pszCat1
        , const tCIDLib::TCh* const     pszCat2
    );

    KRNLEXPORT tCIDLib::TVoid CopyCatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::TCh* const     pszCopy
        , const tCIDLib::TCh* const     pszCat1
        , const tCIDLib::TCh* const     pszCat2
        , const tCIDLib::TCh* const     pszCat3
    );

    KRNLEXPORT tCIDLib::TVoid CopyStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCh* const     pszSrc
        , const tCIDLib::TCard4         c4MaxChars = 0
    );

    KRNLEXPORT tCIDLib::TVoid CopyStr
    (
                tCIDLib::TSCh* const    pszTarget
        , const tCIDLib::TSCh* const    pszSrc
        , const tCIDLib::TCard4         c4MaxChars = 0
    );

    KRNLEXPORT tCIDLib::ESortComps eCompareStr
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
    )   noexcept;

    KRNLEXPORT tCIDLib::ESortComps eCompareStrI
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
    )   noexcept;

    KRNLEXPORT tCIDLib::ESortComps eCompareStrN
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT tCIDLib::ESortComps eCompareStrNI
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT tCIDLib::ESortComps eCompareStr
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
    )   noexcept;

    KRNLEXPORT tCIDLib::ESortComps eCompareStrN
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT tCIDLib::ESortComps eCompareStrI
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
    )   noexcept;

    KRNLEXPORT tCIDLib::ESortComps eCompareStrNI
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT tCIDLib::TFloat8 f8AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TBoolean&      bValid
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid FillString
    (
                tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCh            chFill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::THashVal hshHashStr
    (
        const   tCIDLib::TCh* const     pszStr
        , const tCIDLib::TCard4         c4Modulus
    );

    KRNLEXPORT tCIDLib::TInt4 i4AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT tCIDLib::TInt8 i8AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszConvert
    (
        const   tCIDLib::TSCh* const    pszToConvert
    );

    KRNLEXPORT tCIDLib::TCh* pszConvert
    (
        const   tCIDLib::TSCh* const    pszToConvert
        ,       tCIDLib::TCh*           pszTarget
        , const tCIDLib::TCard4         c4MaxChars
    );

    KRNLEXPORT [[nodiscard]] tCIDLib::TSCh* pszConvert
    (
        const   tCIDLib::TCh* const  pszToConvert
    );

    KRNLEXPORT tCIDLib::TSCh* pszConvert
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TSCh*          pszTarget
        , const tCIDLib::TCard4         c4MaxChars
    );

    KRNLEXPORT tCIDLib::TCh* pszFindChar
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindChar
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszFindChars
    (
                tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindChars
    (
        const   tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszFindLastChar
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindLastChar
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszFindLastChars
    (
                tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindLastChars
    (
        const   tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszFindPrevChar
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindPrevChar
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszFindPrevChars
    (
                tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindPrevChars
    (
        const   tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszFindPrevSubStr
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindPrevSubStr
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszFindSubStr
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT const tCIDLib::TCh* pszFindSubStr
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    );

    KRNLEXPORT tCIDLib::TCh* pszLowerCase
    (
                tCIDLib::TCh*           pszToLower
    )   noexcept;

    KRNLEXPORT tCIDLib::TCh* pszStrTokenize
    (
                tCIDLib::TCh* const     pszSource
        , const tCIDLib::TCh* const     pszWhitespace
        ,       tCIDLib::TCh**          pszContext
    );

    KRNLEXPORT tCIDLib::TCh* pszUpperCase
    (
                tCIDLib::TCh*           pszToUpper
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid StripStr
    (
                tCIDLib::TCh* const     pszStripBuf
        , const tCIDLib::TCh* const     pszStripChars
        , const tCIDLib::EStripModes    eMode
        , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
    );



    // Some simpler versions of the string compare with relative mag is needed
    inline tCIDLib::TBoolean
    bCompareStr(const tCIDLib::TCh* const psz1, const tCIDLib::TCh* const psz2)
    {
        return eCompareStr(psz1, psz2) == tCIDLib::ESortComps::Equal;
    }

    inline tCIDLib::TBoolean
    bCompareStr(const tCIDLib::TSCh* const psz1, const tCIDLib::TSCh* const psz2)
    {
        return eCompareStr(psz1, psz2) == tCIDLib::ESortComps::Equal;
    }

    inline tCIDLib::TBoolean
    bCompareStrI(const tCIDLib::TCh* const psz1, const tCIDLib::TCh* const psz2)
    {
        return eCompareStrI(psz1, psz2) == tCIDLib::ESortComps::Equal;
    }

    inline tCIDLib::TBoolean
    bCompareStrN(const  tCIDLib::TCh* const     psz1
                , const tCIDLib::TCh* const     psz2
                , const tCIDLib::TCard4         c4Count)
    {
        return eCompareStrN(psz1, psz2, c4Count) == tCIDLib::ESortComps::Equal;
    }

    inline tCIDLib::TBoolean
    bCompareStrNI(  const   tCIDLib::TCh* const psz1
                    , const tCIDLib::TCh* const psz2
                    , const tCIDLib::TCard4     c4Count)
    {
        return eCompareStrNI(psz1, psz2, c4Count) == tCIDLib::ESortComps::Equal;
    }


    //
    //  A null pointer safe version. If both are null, they are equal, if one is and
    //  one isn't, they aren't. Else it does the actual compare.
    //
    inline tCIDLib::TBoolean
    bCompareStrSafe(const tCIDLib::TCh* const psz1, const tCIDLib::TCh* const psz2)
    {
        const tCIDLib::TBoolean bN1 = psz1 == nullptr;
        const tCIDLib::TBoolean bN2 = psz2 == nullptr;
        if (!bN1 && !bN2)
            return kCIDLib::True;
        if (bN1 != bN2)
            return kCIDLib::False;
        return eCompareStr(psz1, psz2) == tCIDLib::ESortComps::Equal;
    }

    //
    //  These we can do ourself as inlines, to avoid the overhead of an RTL
    //  call or out of line code for such trivial ops.
    //
    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TCard1     c1Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep)
    {
        // Just call the TCard4 version
        return bFormatVal
        (
            tCIDLib::TCard4(c1Val)
            , pszBuf
            , c4MaxChars
            , eRadix
            , c4GroupSize
            , chGroupSep
        );
    }

    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TCard2     c2Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep)
    {
        // Just call the TCard4 version
        return bFormatVal
        (
            tCIDLib::TCard4(c2Val)
            , pszBuf
            , c4MaxChars
            , eRadix
            , c4GroupSize
            , chGroupSep
        );
    }

    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TInt1      i1Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCh        chNegSign
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep)
    {
        // Just call the Int4 version
        return bFormatVal
        (
            tCIDLib::TInt4(i1Val)
            , pszBuf
            , c4MaxChars
            , eRadix
            , chNegSign
            , c4GroupSize
            , chGroupSep
        );
    }

    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TInt2      i2Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCh        chNegSign
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep)
    {
        // Just call the Int4 version
        return bFormatVal
        (
            tCIDLib::TInt4(i2Val)
            , pszBuf
            , c4MaxChars
            , eRadix
            , chNegSign
            , c4GroupSize
            , chGroupSep
        );
    }

    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TFloat4&       f4Val
                ,       tCIDLib::TCh* const     pszBuf
                , const tCIDLib::TCard4         c4Precision
                , const tCIDLib::TCard4         c4MaxChars
                , const tCIDLib::ETrailFmts     eTrail
                , const tCIDLib::TCh            chNegSign
                , const tCIDLib::TCh            chDecimal
                , const tCIDLib::TCard4         c4GroupSize
                , const tCIDLib::TCh            chGroupSep)
    {
        // Just call the TFloat8 version
        return bFormatVal
        (
            tCIDLib::TFloat8(f4Val)
            , pszBuf
            , c4Precision
            , c4MaxChars
            , eTrail
            , chNegSign
            , chDecimal
            , c4GroupSize
            , chGroupSep
        );
    }

    inline tCIDLib::TCard4 c4StrLen(const tCIDLib::TCh* const pszSrc)
    {
        tCIDLib::TCard4 c4Count = 0;
        if (pszSrc)
        {
            // We just tested pszSrc above, so tmp cannot be null
            CIDLib_Suppress(26429)
            const tCIDLib::TCh* pszTmp = pszSrc;
            while (*pszTmp)
                pszTmp++;
            c4Count = pszTmp - pszSrc;
        }
        return c4Count;
    }

    inline tCIDLib::TCard4 c4StrLen(const tCIDLib::TSCh* const pszSrc)
    {
        tCIDLib::TCard4 c4Count = 0;
        if (pszSrc)
        {
            // We just tested pszSrc above, so tmp cannot be null
            CIDLib_Suppress(26429)
            const tCIDLib::TSCh* pszTmp = pszSrc;
            while (*pszTmp)
                pszTmp++;
            c4Count = pszTmp - pszSrc;
        }
        return c4Count;
    }

    inline tCIDLib::TCh*
    pszFindLastSubStr(          tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszSubStr
                        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True)
    {
        //
        //  Just call the previous sub string version with the last index as
        //  the start index.
        //
        return pszFindPrevSubStr(pszToSearch, pszSubStr, c4StrLen(pszToSearch) - 1, bCaseSensitive);
    }

    inline const tCIDLib::TCh*
    pszFindLastSubStr(  const   tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszSubStr
                        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True)
    {
        //
        //  Just call the previous sub string version with the last index as
        //  the start index.
        //
        return pszFindPrevSubStr
        (
            pszToSearch
            , pszSubStr
            , c4StrLen(pszToSearch) - 1
            , bCaseSensitive
        );
    }

    inline tCIDLib::TSCh* pszReplicate(const tCIDLib::TSCh* const pszToReplicate)
    {
        const tCIDLib::TCard4 c4SrcLen = c4StrLen(pszToReplicate);
        tCIDLib::TSCh* pszNew = new tCIDLib::TSCh[c4SrcLen + 1];
        CopyStr(pszNew, pszToReplicate);
        return pszNew;
    }

    inline tCIDLib::TCh* pszReplicate(const tCIDLib::TCh* const pszToReplicate)
    {
        const tCIDLib::TCard4 c4SrcLen = c4StrLen(pszToReplicate);
        tCIDLib::TCh* pszNew = new tCIDLib::TCh[c4SrcLen + 1];
        CopyStr(pszNew, pszToReplicate);
        return pszNew;
    }
}


