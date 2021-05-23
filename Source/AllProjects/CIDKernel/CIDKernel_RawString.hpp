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
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard1         c1Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard2         c2Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard4         c4Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TCard8&        c8Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt1          i1Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt2          i2Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt4          i4Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TInt8&         i8Val
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

    KRNLEXPORT tCIDLib::TBoolean bFormatVal
    (
        const   tCIDLib::TUInt          uVal
        ,       tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        , const tCIDLib::TCard4         c4GroupSize = 0
        , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
    )   noexcept;

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
    )   noexcept;

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
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsAlpha
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsAlphaNum
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsDigit
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsLower
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsPunct
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsSpace
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsUpper
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bIsHexDigit
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TBoolean bMakeHexByte
    (
        const   tCIDLib::TCh            ch1
        , const tCIDLib::TCh            ch2
        ,       tCIDLib::TCard1&        c1ToFill
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCard4 c4AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        , COP   tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCard4 c4ToHex
    (
        const   tCIDLib::TCh            chConvert
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCard8 c8AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        , COP   tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh chLower
    (
        const   tCIDLib::TCh            chToTest
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh chUpper
    (
        const   tCIDLib::TCh            cchToTest
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid CatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCh* const     pszSrc
        , const tCIDLib::TCard4         c4MaxChars = 0
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid CopyCatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::TCh* const     pszCopy
        , const tCIDLib::TCh* const     pszCat
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid CopyCatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::TCh* const     pszCopy
        , const tCIDLib::TCh* const     pszCat1
        , const tCIDLib::TCh* const     pszCat2
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid CopyCatStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::TCh* const     pszCopy
        , const tCIDLib::TCh* const     pszCat1
        , const tCIDLib::TCh* const     pszCat2
        , const tCIDLib::TCh* const     pszCat3
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid CopyStr
    (
                tCIDLib::TCh* const     pszTarget
        , const tCIDLib::TCh* const     pszSrc
        , const tCIDLib::TCard4         c4MaxChars = 0
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid CopyStr
    (
                tCIDLib::TSCh* const    pszTarget
        , const tCIDLib::TSCh* const    pszSrc
        , const tCIDLib::TCard4         c4MaxChars = 0
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStr
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStrI
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStrN
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStrNI
    (
        const   tCIDLib::TCh* const     pszStr1
        , const tCIDLib::TCh* const     pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStr
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStrN
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStrI
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::ESortComps eCompareStrNI
    (
        const   tCIDLib::TSCh* const    pszStr1
        , const tCIDLib::TSCh* const    pszStr2
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TFloat8 f8AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TBoolean&      bValid
    )   noexcept;

    KRNLEXPORT tCIDLib::TVoid FillString
    (
                tCIDLib::TCh* const     pszBuf
        , const tCIDLib::TCh            chFill
        , const tCIDLib::TCard4         c4Count
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::THashVal hshHashStr
    (
        const   tCIDLib::TCh* const     pszStr
        , const tCIDLib::TCard4         c4Modulus
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TInt4 i4AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TInt8 i8AsBinary
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TBoolean&      bValid
        , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszConvert
    (
        const   tCIDLib::TSCh* const    pszToConvert
    )   noexcept;

    KRNLEXPORT  [[nodiscard]]tCIDLib::TCh* pszConvert
    (
        const   tCIDLib::TSCh* const    pszToConvert
        ,       tCIDLib::TCh*           pszTarget
        , const tCIDLib::TCard4         c4MaxChars
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TSCh* pszConvert
    (
        const   tCIDLib::TCh* const  pszToConvert
    )   noexcept;

    KRNLEXPORT  [[nodiscard]]tCIDLib::TSCh* pszConvert
    (
        const   tCIDLib::TCh* const     pszToConvert
        ,       tCIDLib::TSCh*          pszTarget
        , const tCIDLib::TCard4         c4MaxChars
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszFindChar
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] const tCIDLib::TCh* pszFindChar
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT tCIDLib::TCh* pszFindChars
    (
                tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT const tCIDLib::TCh* pszFindChars
    (
        const   tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszFindLastChar
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] const tCIDLib::TCh* pszFindLastChar
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszFindLastChars
    (
                tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] const tCIDLib::TCh* pszFindLastChars
    (
        const   tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszFindPrevChar
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] const tCIDLib::TCh* pszFindPrevChar
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh            chToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszFindPrevChars
    (
                tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] const tCIDLib::TCh* pszFindPrevChars
    (
        const   tCIDLib::TCh* const     pszSearch
        , const tCIDLib::TCh* const     pszChars
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszFindPrevSubStr
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] const tCIDLib::TCh* pszFindPrevSubStr
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] tCIDLib::TCh* pszFindSubStr
    (
                tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT [[nodiscard]] const tCIDLib::TCh* pszFindSubStr
    (
        const   tCIDLib::TCh* const     pszToSearch
        , const tCIDLib::TCh* const     pszToFind
        , const tCIDLib::TCard4         c4StartAt = 0
        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
    )   noexcept;

    KRNLEXPORT tCIDLib::TCh* pszLowerCase
    (
                tCIDLib::TCh*           pszToLower
    )   noexcept;

    KRNLEXPORT tCIDLib::TCh* pszStrTokenize
    (
                tCIDLib::TCh* const     pszSource
        , const tCIDLib::TCh* const     pszWhitespace
        ,       tCIDLib::TCh**          pszContext
    )   noexcept;

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
    )   noexcept;



    // Some simpler versions of the string compare with relative mag is needed
    [[nodiscard]] inline tCIDLib::TBoolean
    bCompareStr(const tCIDLib::TCh* const psz1, const tCIDLib::TCh* const psz2) noexcept
    {
        return eCompareStr(psz1, psz2) == tCIDLib::ESortComps::Equal;
    }

    [[nodiscard]] inline tCIDLib::TBoolean
    bCompareStr(const tCIDLib::TSCh* const psz1, const tCIDLib::TSCh* const psz2) noexcept
    {
        return eCompareStr(psz1, psz2) == tCIDLib::ESortComps::Equal;
    }

    [[nodiscard]] inline tCIDLib::TBoolean
    bCompareStrI(const tCIDLib::TCh* const psz1, const tCIDLib::TCh* const psz2) noexcept
    {
        return eCompareStrI(psz1, psz2) == tCIDLib::ESortComps::Equal;
    }

    [[nodiscard]] inline tCIDLib::TBoolean
    bCompareStrN(const  tCIDLib::TCh* const     psz1
                , const tCIDLib::TCh* const     psz2
                , const tCIDLib::TCard4         c4Count) noexcept
    {
        return eCompareStrN(psz1, psz2, c4Count) == tCIDLib::ESortComps::Equal;
    }

    [[nodiscard]] inline tCIDLib::TBoolean
    bCompareStrNI(  const   tCIDLib::TCh* const psz1
                    , const tCIDLib::TCh* const psz2
                    , const tCIDLib::TCard4     c4Count) noexcept
    {
        return eCompareStrNI(psz1, psz2, c4Count) == tCIDLib::ESortComps::Equal;
    }


    //
    //  These we can do ourself as inlines, to avoid the overhead of an RTL
    //  call or out of line code for such trivial ops.
    //
    [[nodiscard]] inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TCard1     c1Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep) noexcept
    {
        // Just call the TCard4 version
        return bFormatVal
        (
            tCIDLib::TCard4(c1Val), pszBuf, c4MaxChars, eRadix, c4GroupSize, chGroupSep
        );
    }

    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TCard2     c2Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep) noexcept
    {
        // Just call the TCard4 version
        return bFormatVal
        (
            tCIDLib::TCard4(c2Val), pszBuf, c4MaxChars, eRadix, c4GroupSize, chGroupSep
        );
    }

    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TInt1      i1Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCh        chNegSign
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep) noexcept
    {
        // Just call the Int4 version
        return bFormatVal
        (
            tCIDLib::TInt4(i1Val), pszBuf, c4MaxChars, eRadix, chNegSign, c4GroupSize, chGroupSep
        );
    }

    inline tCIDLib::TBoolean
    bFormatVal( const   tCIDLib::TInt2      i2Val
                ,       tCIDLib::TCh* const pszBuf
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::ERadices   eRadix
                , const tCIDLib::TCh        chNegSign
                , const tCIDLib::TCard4     c4GroupSize
                , const tCIDLib::TCh        chGroupSep) noexcept
    {
        // Just call the Int4 version
        return bFormatVal
        (
            tCIDLib::TInt4(i2Val), pszBuf, c4MaxChars, eRadix, chNegSign, c4GroupSize, chGroupSep
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
                , const tCIDLib::TCh            chGroupSep) noexcept
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

    [[nodiscard]] constexpr  tCIDLib::TCard4 c4StrLen(const tCIDLib::TCh* const pszSrc) noexcept
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

    [[nodiscard]] constexpr  tCIDLib::TCard4 c4StrLen(const tCIDLib::TSCh* const pszSrc) noexcept
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

    [[nodiscard]] inline tCIDLib::TCh*
    pszFindLastSubStr(          tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszSubStr
                        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True) noexcept
    {
        //
        //  Just call the previous sub string version with the last index as
        //  the start index.
        //
        return pszFindPrevSubStr(pszToSearch, pszSubStr, c4StrLen(pszToSearch) - 1, bCaseSensitive);
    }

    [[nodiscard]] inline const tCIDLib::TCh*
    pszFindLastSubStr(  const   tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszSubStr
                        , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True) noexcept
    {
        //
        //  Just call the previous sub string version with the last index as
        //  the start index.
        //
        return pszFindPrevSubStr
        (
            pszToSearch, pszSubStr, c4StrLen(pszToSearch) - 1, bCaseSensitive
        );
    }

    [[nodiscard]] inline tCIDLib::TSCh* pszReplicate(const tCIDLib::TSCh* const pszToReplicate) noexcept
    {
        const tCIDLib::TCard4 c4SrcLen = c4StrLen(pszToReplicate);
        tCIDLib::TSCh* pszNew = new tCIDLib::TSCh[c4SrcLen + 1];
        CopyStr(pszNew, pszToReplicate);
        return pszNew;
    }

    [[nodiscard]] inline tCIDLib::TCh* pszReplicate(const tCIDLib::TCh* const pszToReplicate) noexcept
    {
        const tCIDLib::TCard4 c4SrcLen = c4StrLen(pszToReplicate);
        tCIDLib::TCh* pszNew = new tCIDLib::TCh[c4SrcLen + 1];
        CopyStr(pszNew, pszToReplicate);
        return pszNew;
    }
}


