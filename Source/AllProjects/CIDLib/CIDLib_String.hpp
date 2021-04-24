//
// FILE NAME: CIDLib_String.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/16/1992
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
//  This file implements the TString class, which (suprise) implements a wrapper
//  for an array of (wide) characters strings. The string is maintained null
//  terminated at least any time you look at the raw buffer, so that it can be
//  passed to system APIs, but the length is also  maintained at all times. So
//  appends or other ops that require access to the end of the string are very fast.
//
//  String capacity is not considered part of equality between strings, only the
//  current length and actual character content. So the capacity can be changed
//  at any time as required.
//
//  Buffer Issues.
//
//  We take the complexity hit here to provide for greater performance, in that we
//  allow our buffer to be null. This is both so that default ctor'd strings that
//  never get actually used don't allocate anything. And so that we can move
//  strings without having to pay the cost to allocate a buffer that in most case
//  will just get discarded. This makes the common scenario where we create some
//  string contents locally and return it much cheaper since we can just move the
//  contents to the caller.
//
//  So we almost never directly access the buffer member, we call methods that will
//  get the buffer for const or non-const access and the same at a specific index
//  within the buffer. This will fault in the buffer if it doesn't exist, so that
//  we never are caught with a null buffer.
//
//  In some rare scenarios where we have already called the wrapper and know we have
//  a buffer we'll use the buffer directly (mostly to play tricks where the buffer
//  has been extended but the new length isn't stored yet, which would make the
//  wrappers throw an index exception.)
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TFacility;
class   TStreamFmt;
class   TTextConverter;
class   TString;


#pragma CIDLIB_PACK(CIDLIBPACK)



// ---------------------------------------------------------------------------
//  CLASS: TString
// PREFIX: str
// ---------------------------------------------------------------------------
class CIDLIBEXP TString :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------

        // Used in the low level token processing method, pszFindToken()
        enum class ETokenFind
        {
            End
            , Token
            , TextRun
            , NewLine
            , BadFormat
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        [[nodiscard]] static tCIDLib::TBoolean bComp(const TString& str1, const TString& str2)
        {
            return str1.bCompare(str2);
        }

        [[nodiscard]] static tCIDLib::TBoolean bCompI(const TString& str1, const TString& str2)
        {
            return str1.bCompareI(str2);
        }

        [[nodiscard]] static tCIDLib::TCard1 c1ToHex(const tCIDLib::TSCh chToXlat)
        {
            tCIDLib::TCard1 c1Ret =(0);
            if ((chToXlat >= 0x41) && (chToXlat <= 0x46))
                c1Ret = 10 + (chToXlat - 0x41);
            else if ((chToXlat >= 0x30) && (chToXlat <= 0x39))
                c1Ret = chToXlat - 0x30;
            else
                BadHexChar(tCIDLib::TCh(chToXlat));
            return c1Ret;
        }

        [[nodiscard]] static tCIDLib::TCard1 c1ToHex(const tCIDLib::TCh chToXlat)
        {
            tCIDLib::TCard1 c1Ret(0);
            if ((chToXlat >= kCIDLib::chLatin_A) && (chToXlat <= kCIDLib::chLatin_F))
                c1Ret = static_cast<tCIDLib::TCard1>(10 + (chToXlat - kCIDLib::chLatin_A));
            else if ((chToXlat >= kCIDLib::chDigit0) && (chToXlat <= kCIDLib::chDigit9))
                c1Ret = static_cast<tCIDLib::TCard1>(chToXlat - kCIDLib::chDigit0);
            else
                BadHexChar(chToXlat);
            return c1Ret;
        }

        [[nodiscard]] static tCIDLib::ESortComps eComp(const TString& str1, const TString& str2)
        {
            return str1.eCompare(str2);
        }

        [[nodiscard]] static tCIDLib::ESortComps eCompI(const TString& str1, const TString& str2)
        {
            return str1.eCompareI(str2);
        }

        [[nodiscard]] static ETokenFind eFindToken
        (
            const   tCIDLib::TCh*&          pszStart
            , COP    tCIDLib::TCh&          chToken
            , COP    tCIDLib::EHJustify&    eJustify
            , COP    tCIDLib::TCard4&       c4FldWidth
            , COP    tCIDLib::TCh&          chFill
            , COP    tCIDLib::TCard4&       c4Precision
            , COP const tCIDLib::TCh*&      pszEnd
        );

        static tCIDLib::TVoid FromHex
        (
            const   tCIDLib::TCard1         c1ToXlat
            , COP   tCIDLib::TSCh&          chOne
            , COP   tCIDLib::TSCh&          chTwo
        );

        static tCIDLib::TVoid FromHex
        (
            const   tCIDLib::TCard1         c1ToXlat
            , COP   tCIDLib::TCh&           chOne
            , COP   tCIDLib::TCh&           chTwo
        );

        [[nodiscard]] static const tCIDLib::TCh* pszFindToken
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCh            chToFind
            , COP   tCIDLib::EHJustify&     eJustify
            , COP   tCIDLib::TCard4&        c4FldWidth
            , COP   tCIDLib::TCh&           chFill
            , COP   tCIDLib::TCard4&        c4Precision
            , COP   tCIDLib::TCard4&        c4TokenChars
            , COP   tCIDLib::TCard4&        c4TokenCnt
        );

        static const TString& strEmpty();

        static TString strConcat
        (
            const   TString&                str1
            , const TString&                str2
        );

        static TString strConcat
        (
            const   TString&                str1
            , const TString&                str2
            , const TString&                str3
        );

        static TString strConcat
        (
            const   TString&                str1
            , const TString&                str2
            , const TString&                str3
            , const TString&                str4
        );

        static TString strConcat
        (
            const   TString&                str1
            , const tCIDLib::TCh            chSep
        );

        static TString strConcat
        (
            const   TString&                str1
            , const tCIDLib::TCh            chSep
            , const TString&                str2
        );

        static TString strConcat
        (
            const   tCIDLib::TCh* const     psz1
            , const TString&                str2
        );

        static TString strConcat
        (
            const   TString&                str1
            , const tCIDLib::TCh* const     psz2
        );

        static TString strConcat
        (
            const   tCIDLib::TCh* const     psz1
            , const tCIDLib::TCh* const     psz2
        );

        static TString strConcat
        (
            const   tCIDLib::TCh* const     psz1
            , const tCIDLib::TCh            chSep
            , const tCIDLib::TCh* const     psz2
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TString();

        explicit TString
        (
            const   tCIDLib::TCard4         c4BufSize
        );

        explicit TString
        (
            const   tCIDLib::TCh            chInit
        );

        explicit TString
        (
            const   tCIDLib::TSCh* const    pszInitValue
        );

        TString
        (
            const   tCIDLib::TCh* const     pszInitValue
            , const tCIDLib::TCard4         c4ExtraChars = 0
        );

        TString
        (
            const   TString&                strInitValue
            , const tCIDLib::TCard4         c4ExtraChars
        );

        TString
        (
                    tCIDLib::TCh*           pszInit
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TString
        (
            const   MFormattable&           fmtblInitValue
        );

        TString
        (
            const   MFormattable&           fmtblInitValue
            , const TStreamFmt&             strmfToUse
        );

        TString
        (
            const   tCIDLib::TMsgId         midToLoad
            , const TFacility&              facSource
        );

        TString
        (
            const   tCIDLib::TMsgId         midToLoad
            , const TFacility&              facSource
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        );

        TString
        (
            const   TString&                strPattern
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        );

        TString
        (
            const   tCIDLib::TCh* const     pszPattern
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        );

        TString
        (
            const   TString&                strSrc
        );

        TString
        (
                    TString&&               strSrc
        );

        TString
        (
            const   TKrnlString&            kstrSrc
        );

        TString
        (
                    TKrnlString&&           kstrSrc
        );

        ~TString();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TString& operator=
        (
            const   tCIDLib::TCh* const     pszSrc
        );

        TString& operator=
        (
            const   TString&                strSrc
        );

        TString& operator=
        (
                    TString&&               strSrc
        );

        TString& operator=
        (
                    TKrnlString&&           kstrSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TVoid operator+=(const TString& strSrc)
        {
            Append(strSrc);
        }

        tCIDLib::TVoid operator+=(const tCIDLib::TCh* const pszSrc)
        {
            Append(pszSrc);
        }

        tCIDLib::TVoid operator+=(const tCIDLib::TCh chSrc)
        {
            Append(chSrc);
        }

        tCIDLib::TBoolean operator<
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TCh operator[]
        (
            const   tCIDLib::TCard4         c4Ind
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TSCh* const    pszSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   tCIDLib::TSCh* const    pszSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Append
        (
            const   TString&                strSrc
        );

        tCIDLib::TVoid Append
        (
            const   TString&                strToAppend1
            , const TString&                strToAppend2
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCh* const     pszSrc2 = nullptr
            , const tCIDLib::TCh* const     pszSrc3 = nullptr
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh            chSrc
            , const tCIDLib::TCard4         c4Count = 1
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TCard1         c1ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TCard2         c2ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TCard4         c4ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TCard8         c8ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TFloat8        f8ToFmt
            , const tCIDLib::TCard4         c4DecPlaces = 2
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TFloat8        f8ToFmt
            , const tCIDLib::TCard4         c4DecPlaces
            , const tCIDLib::TCh            chNegSign
            , const tCIDLib::TCh            chDecimal
            , const tCIDLib::TCard4         c4GroupSize
            , const tCIDLib::TCh            chGroupSep
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TInt1          i1ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TInt2          i2ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TInt4          i4ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TInt8          i8ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid AppendFormatted
        (
            const   tCIDLib::TSInt          iToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        //
        //  The precision parameter is a dummy parameter that is not used. This
        //  exists purely to deal with an otherwise really ugly issue with
        //  the variadice Format() method.
        //
        tCIDLib::TVoid AppendFormatted
        (
            const   MFormattable&           fmtblSrc
            , const tCIDLib::TCard4         c4Prec = 0
        );

        tCIDLib::TVoid AppendSubStr
        (
            const   TString&                strSrc
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid AppendSubStr
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TBoolean bCapAtChar
        (
            const   tCIDLib::TCh            chAt
        );

        [[nodiscard]] tCIDLib::TBoolean bCompare
        (
            const   TString&                strSrc
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCompare
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCompareI
        (
            const   TString&                strSrc
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCompareI
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCompareN
        (
            const   TString&                strSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCompareN
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCompareNI
        (
            const   TString&                strSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCompareNI
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bContainsChar
        (
            const   tCIDLib::TCh            chToFind
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bContainsSubStr
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bEndsWith(const tCIDLib::TCh* const pszToCheck) const
        {
            return bCheckSuffix(pszToCheck, kCIDLib::True);
        }

        [[nodiscard]] tCIDLib::TBoolean bEndsWith(const TString& strToCheck) const
        {
            return bCheckSuffix(strToCheck, kCIDLib::True);
        }

        [[nodiscard]] tCIDLib::TBoolean bEndsWithI(const tCIDLib::TCh* const pszToCheck) const
        {
            return bCheckSuffix(pszToCheck, kCIDLib::False);
        }

        [[nodiscard]] tCIDLib::TBoolean bEndsWithI(const TString& strToCheck) const
        {
            return bCheckSuffix(strToCheck, kCIDLib::False);
        }

        [[nodiscard]] tCIDLib::TBoolean bExtractNthToken
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCh            chSepChar
            , COP   TString&                strToFill
            , const tCIDLib::TBoolean       bStripWS = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bFindTokenList
        (
            COP     TString&                strToFill
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bFirstOccurrence
        (
            const   tCIDLib::TCh            chToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bFirstOccurrence
        (
            const   TString&                strSubStrToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bIsAlpha() const;

        [[nodiscard]] tCIDLib::TBoolean bIsAlphaNum() const;

        [[nodiscard]] tCIDLib::TBoolean bIsEmpty() const
        {
            return(m_c4CurEnd == 0);
        }

        [[nodiscard]] tCIDLib::TBoolean bLastOccurrence
        (
            const   tCIDLib::TCh            chToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bLastOccurrence
        (
            const   TString&                strSubStrToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bNextOccurrence
        (
            const   tCIDLib::TCh            chToFind
            , CIOP   tCIDLib::TCard4&        c4StartPos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bNextOccurrence
        (
            const   TString&                strSubStrToFind
            , CIOP  tCIDLib::TCard4&        c4StartPos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bPrevOccurrence
        (
            const   tCIDLib::TCh            chToFind
            , CIOP  tCIDLib::TCard4&        c4StartPos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bPrevOccurrence
        (
            const   TString&                strSubStrToFind
            , CIOP  tCIDLib::TCard4&        c4StartPos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bReplaceChar
        (
            const   tCIDLib::TCh            chToReplace
            , const tCIDLib::TCh            chReplaceWith
        );

        tCIDLib::TBoolean bReplaceSubStr
        (
            const   TString&                strToReplace
            , const TString&                strReplaceWith
            , CIOP  tCIDLib::TCard4&        c4StartAt
            , const tCIDLib::TBoolean       bReplaceAll
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        );

        tCIDLib::TBoolean bSplit
        (
            COP     TString&                strSecondHalf
            , const tCIDLib::TCh            chDivider
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        );

        [[nodiscard]] tCIDLib::TBoolean bStartsWith(const tCIDLib::TCh* const pszToCheck) const
        {
            return bCheckPrefix(pszToCheck, kCIDLib::True);
        }

        [[nodiscard]] tCIDLib::TBoolean bStartsWith(const TString& strToCheck) const
        {
            return bCheckPrefix(strToCheck, kCIDLib::True);
        }

        [[nodiscard]] tCIDLib::TBoolean bStartsWithI(const tCIDLib::TCh* const pszToCheck) const
        {
            return bCheckPrefix(pszToCheck, kCIDLib::False);
        }

        [[nodiscard]] tCIDLib::TBoolean bStartsWithI(const TString& strToCheck) const
        {
            return bCheckPrefix(strToCheck, kCIDLib::False);
        }

        tCIDLib::TBoolean bToBoolean
        (
            COP    tCIDLib::TBoolean&      bToFill
        )   const noexcept;

        tCIDLib::TBoolean bToCard1
        (
            COP     tCIDLib::TCard1&        c1ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToCard2
        (
            COP     tCIDLib::TCard2&        c2ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToCard4
        (
            COP     tCIDLib::TCard4&        c4ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToCard8
        (
            COP     tCIDLib::TCard8&        c8ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToFloat4
        (
            COP   tCIDLib::TFloat4&       f4ToFill
        )   const noexcept;

        tCIDLib::TBoolean bToFloat8
        (
            COP   tCIDLib::TFloat8&       f8ToFill
        )   const noexcept;

        tCIDLib::TBoolean bToInt1
        (
            COP     tCIDLib::TInt1&         i1ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToInt2
        (
            COP     tCIDLib::TInt2&         i2ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToInt4
        (
            COP     tCIDLib::TInt4&         i4ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToInt8
        (
            COP     tCIDLib::TInt8&         i8ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bTokenExists
        (
            const   tCIDLib::TCh            chToken
        )   const;

        [[nodiscard]] tCIDLib::TCard4 c4BufChars() const;

        [[nodiscard]] tCIDLib::TCard4 c4Length() const;

        tCIDLib::TCard4 c4Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TCard8 c8Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        [[nodiscard]] tCIDLib::TCh chAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        [[nodiscard]] tCIDLib::TCh chFirst() const;

        [[nodiscard]] tCIDLib::TCh chLast() const;

        tCIDLib::TVoid CapAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid CopyInSubStr
        (
            const   TString&                strSource
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid CopyOutSubStr
        (
            COP     TString&               strTarget
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        )   const;

        tCIDLib::TVoid Cut
        (
            const   tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid CutUpTo
        (
            const   tCIDLib::TCh            chFind
        );

        tCIDLib::TVoid DeleteLast();

        [[nodiscard]] tCIDLib::ESortComps eCompare
        (
            const   tCIDLib::TCh* const     pszToCompare
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompare
        (
            const   TString&                strToCompare
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareN
        (
            const   tCIDLib::TCh* const     pszToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareN
        (
            const   TString&                strToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareI
        (
            const   tCIDLib::TCh* const     pszToCompare
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareI
        (
            const   TString&                strToCompare
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareNI
        (
            const   tCIDLib::TCh* const     pszToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareNI
        (
            const   TString&                strToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TCh            chVal
            , const tCIDLib::TCh            chToken
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   MFormattable&           fmtblVal
            , const tCIDLib::TCh            chToken
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TBoolean       bVal
            , const tCIDLib::TCh            chToken
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TCard1         c1Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TCard2         c2Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TCard4         c4Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TCard8         c8Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TFloat8&       f8Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::TCard1         c1Precision = 2
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TInt1          i1Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TInt2          i2Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TInt4          i4Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
            const   tCIDLib::TInt8          i8Val
            , const tCIDLib::TCh            chToken
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::EFindRes eReplaceToken
        (
             const   tCIDLib::TCh* const    pszVal
            , const tCIDLib::TCh            chToken
        );

        [[nodiscard]] tCIDLib::EFindRes eTokenExists
        (
            const   tCIDLib::TCh            chToken
        )   const;

        tCIDLib::TVoid ExportChars
        (
                    tCIDLib::TCh* const     pszTarget
            , const tCIDLib::TCard4         c4MaxChars
            , const tCIDLib::TCard4         c4StartInd = 0
        )   const;

        tCIDLib::TVoid FormatToFld
        (
            const   TString&                strToFormat
            , const tCIDLib::TCard4         c4FldWidth  = 0
            , const tCIDLib::EHJustify      eJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::TCh            chFill = kCIDLib::chSpace
            , const tCIDLib::TCard4         c4TrailingSp = 0
        );

        tCIDLib::TVoid FromShortZStr
        (
            const    tCIDLib::TSCh* const   pszNewValue
        );

        tCIDLib::TVoid FromZStr
        (
            const   tCIDLib::TCh* const     pszSource
            , const tCIDLib::TCard4         c4Count = 0
        );

        //
        //  This will append the formatted content to us. We have another that will
        //  clear us first and then call appending version. To avoid having to
        //  replicate a lot of code, we pass a temp string which will be used to
        //  format the token values out.
        //
        template <typename... TArgs>
        tCIDLib::TVoid Format(const TString& strFmt, const TArgs... Args)
        {
            Clear();
            FormatAppend(strFmt, Args...);
        }

        template <typename... TArgs>
        tCIDLib::TVoid FormatAppend(const TString& strFmt, const TArgs... Args)
        {
            TString strTmp;
            TString strTmp2;

            // If we get back the buffer, then no parms, just write out fmt text
            if (pszFmtHelper(strFmt.pszBuffer(), strTmp, strTmp2, Args...) == strFmt.pszBuffer())
                Append(strFmt);
        }

        tCIDLib::TFloat8 f8Val() const;

        [[nodiscard]] tCIDLib::THashVal hshCalcHash
        (
            const   tCIDLib::TCard4         c4Modulus
        )   const;

        [[nodiscard]] tCIDLib::TInt4 i4Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        [[nodiscard]] tCIDLib::TInt8 i8Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TVoid ImportChars
        (
            const   tCIDLib::TCh* const     pszSource
            , const tCIDLib::TCard4         c4SrcCount
        );

        tCIDLib::TVoid Insert
        (
            const   TString&                strInsert
            , const tCIDLib::TCard4         c4Ind
        );

        tCIDLib::TVoid Insert
        (
            const   tCIDLib::TCh* const     pszInsert
            , const tCIDLib::TCard4         c4Ind
        );

        tCIDLib::TVoid Insert
        (
            const   tCIDLib::TCh            chToInsert
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid LoadFromMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            , const TFacility&              facSource
        );

        tCIDLib::TVoid LoadFromMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            , const TFacility&              facSource
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        );

        [[nodiscard]] const tCIDLib::TCh* pszBuffer() const;

        [[nodiscard]] const tCIDLib::TCh* pszBufferAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        [[nodiscard]] tCIDLib::TCh* pszDupBuffer() const;

        const tCIDLib::TCh* pszEnd() const;

        tCIDLib::TVoid Prepend
        (
            const   tCIDLib::TCh            chToPrepend
        );

        tCIDLib::TVoid Prepend
        (
            const   TString&                strPrepend
        );

        tCIDLib::TVoid Prepend
        (
            const   tCIDLib::TCh* const     pszPrepend
        );

        tCIDLib::TVoid PutAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCh            chToPut
        );

        tCIDLib::TVoid Reallocate
        (
            const   tCIDLib::TCard4         c4NewSize
            , const tCIDLib::TBoolean       bPreserveContent = kCIDLib::True
        );

        //
        //  The precision parameter is a dummy parameter that is not used. This
        //  exists purely to deal with an otherwise really ugly issue with
        //  the variadice Format() method.
        //
        tCIDLib::TVoid SetFormatted
        (
            const   MFormattable&           fmtblSrc
            , const tCIDLib::TCard4         c4Prec = 0
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TCard1         c1ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TCard2         c2ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TCard4         c4ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TCard8         c8ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TFloat8        f8ToFmt
            , const tCIDLib::TCard4         c4DecPlaces = 2
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TFloat8        f8ToFmt
            , const tCIDLib::TCard4         c4DecPlaces
            , const tCIDLib::TCh            chNegSign
            , const tCIDLib::TCh            chDecimal
            , const tCIDLib::TCard4         c4GroupSize
            , const tCIDLib::TCh            chGroupSep
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TInt1          i1ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TInt2          i2ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TInt4          i4ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TInt8          i8ToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetFormatted
        (
            const   tCIDLib::TSInt          iToFmt
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TCh            chNegSign = kCIDLib::chHyphenMinus
            , const tCIDLib::TCard4         c4GroupSize = 0
            , const tCIDLib::TCh            chGroupSep = kCIDLib::chNull
        );

        tCIDLib::TVoid SetLast
        (
            const   tCIDLib::TCh            chNew
        );

        tCIDLib::TVoid Strip
        (
            const   tCIDLib::TCh* const     pszChars
            , const tCIDLib::EStripModes    eStripMode = tCIDLib::EStripModes::LeadTrail
            , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
        );

        tCIDLib::TVoid Strip
        (
            const   TString&                strChars
            , const tCIDLib::EStripModes    eStripMode = tCIDLib::EStripModes::LeadTrail
            , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
        );

        tCIDLib::TVoid StripWhitespace
        (
            const   tCIDLib::EStripModes    eStripMode = tCIDLib::EStripModes::LeadTrail
            , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
        );

        tCIDLib::TVoid ToLower
        (
            const   tCIDLib::TCard4         c4StartInd = 0
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid ToUpper
        (
            const   tCIDLib::TCard4         c4StartInd = 0
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid ToZStr
        (
                    tCIDLib::TCh* const     pszTarget
            , const tCIDLib::TCard4         c4MaxChars
            , const tCIDLib::TCard4         c4StartInd = 0
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TRawBufAcc;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
            CIOP    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
            CIOP    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid BadHexChar
        (
            const   tCIDLib::TCh            chToXlat
        );


        // -------------------------------------------------------------------
        //  Private constructor, for use by the strConcat static methods. We add a
        //  bogus parameter to make sure it doesn't clash with any public ones. The
        //  buffer is adopted and is allocated to the indicated length, and will
        //  be null terminated.
        // -------------------------------------------------------------------
        enum EBogusCtorParm { x };
        TString(        tCIDLib::TCh* const     pszToAdopt
                , const tCIDLib::TCard4         c4BufLen
                , const EBogusCtorParm          eBogus) :

            m_c4BufChars(c4BufLen)
            , m_c4CurEnd(c4BufLen)
            , m_pszBuffer(pszToAdopt)
        {
        }


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::TBoolean bCheckPrefix
        (
            const   tCIDLib::TCh* const     pszToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCheckPrefix
        (
            const   TString&                strToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCheckSuffix
        (
            const   tCIDLib::TCh* const     pszToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCheckSuffix
        (
            const   TString&                strToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;

        [[nodiscard]] tCIDLib::TCard4 c4CalcBufDiff
        (
            const   tCIDLib::TCh* const     pszUpper
            , const tCIDLib::TCh* const     pszLower
        )   const;

        [[nodiscard]] tCIDLib::TCard4 c4CalcBufDiff(const tCIDLib::TCh* const pszUpper) const
        {
            return c4CalcBufDiff(pszUpper, m_pszBuffer);
        }

        //
        //  Helpers for Format() which needs to process a list of variadic
        //  parameters.
        //
        template <typename TOne, typename... TOthers>
        const tCIDLib::TCh* pszFmtHelper(const  tCIDLib::TCh* const pszFmt
                                        ,       TString&            strTmp
                                        ,       TString&            strTmp2
                                        , const TOne                tOne
                                        , const TOthers...          tOthers)
        {
            tCIDLib::TCh chFill = kCIDLib::chNull;
            tCIDLib::TCh chToken = kCIDLib::chNull;
            tCIDLib::TCard4 c4Width = 0;
            tCIDLib::TCard4 c4Precision = 0;
            tCIDLib::EHJustify eJustify = tCIDLib::EHJustify::Count;

            //
            //  We have to loop until we get a token, since we could have text bits
            //  which we just copy straight out as text.
            //
            const tCIDLib::TCh* pszStart = pszFmt;
            const tCIDLib::TCh* pszEnd = nullptr;
            while (kCIDLib::True)
            {
                const TString::ETokenFind eFindRes = TString::eFindToken
                (
                    pszStart, chToken, eJustify, c4Width, chFill, c4Precision, pszEnd
                );

                // If it fails, then give up and stop recursing
                if ((eFindRes == TString::ETokenFind::BadFormat)
                ||  (eFindRes == TString::ETokenFind::End))
                {
                    return nullptr;
                }
                 else if (eFindRes == TString::ETokenFind::Token)
                {
                    //
                    //  If no width, then just append it formatted, else we need
                    //  to use a temp string and formatted into a field, then append
                    //  that.
                    //
                    if (tCIDLib::IsTFloatX<TOne>::bState)
                    {
                        if (c4Width)
                        {
                            strTmp.SetFormatted(tOne, c4Precision);
                            strTmp2.FormatToFld(strTmp, c4Width, eJustify, chFill);
                            Append(strTmp2);
                        }
                         else
                        {
                            AppendFormatted(tOne, c4Precision);
                        }
                    }
                     else
                    {
                        if (c4Width)
                        {
                            strTmp.SetFormatted(tOne);
                            strTmp2.FormatToFld(strTmp, c4Width, eJustify, chFill);
                            Append(strTmp2);
                        }
                         else
                        {
                            AppendFormatted(tOne);
                        }
                    }

                    // Now we need to recurse to move to the next parameter
                    pszEnd = pszFmtHelper(pszEnd, strTmp, strTmp2, tOthers...);
                }
                 else if (eFindRes == TString::ETokenFind::TextRun)
                {
                    // It's a text run, so just copy those as is
                    AppendSubStr(pszStart, 0, pszEnd - pszStart);
                }

                // Move forward
                pszStart = pszEnd;
            }
            return pszEnd;
        }

        [[nodiscard]] tCIDLib::TCh* pszBufferW();

        [[nodiscard]] tCIDLib::TCh* pszBufferWAt
        (
            const   tCIDLib::TCard4         c4At
        );

        [[nodiscard]] const tCIDLib::TCh* pszFmtHelper(const tCIDLib::TCh* pszFmt, TString&, TString&)
        {
            return pszFmt;
        }

        tCIDLib::TVoid Replace
        (
            const   tCIDLib::TCard4         c4RepStart
            , const tCIDLib::TCard4         c4RepEnd
            , const tCIDLib::TCh* const     pszRepText
            , const tCIDLib::TCard4         c4RepLen
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4Extra = 0
        );

        tCIDLib::TVoid SetFromShort
        (
            const   tCIDLib::TSCh* const    pszSrc
        );

        tCIDLib::TVoid Set
        (
            const   TString&                strSrc
            , const tCIDLib::TCard4         c4Extra = 0
        );

        tCIDLib::TVoid ToZStr
        (
                    tCIDLib::TCh* const     pszTarget
            , const tCIDLib::TCard4         c4MaxChars
            , const tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TBoolean       bAddNull
        )   const;


        // -------------------------------------------------------------------
        //  Private data members.
        //
        //  It's all mutable because we need to be super-efficient wrt to move
        //  semantics for strings. We want to move a null into the source, but be
        //  able to fault the buffer back in if need be.
        //
        //  m_c4BufChars
        //      This is the current size of the buffer, minus one. I.e. the buffer is
        //      one bigger than this (so that there is room for the null terminator
        //      even if there are this many chars in it.) It will grow as required.
        //
        //  m_c4CurEnd
        //      This is the index of the null terminator, i.e. this is where you'd
        //      end up appending (and its the number of chars actually in the buffer
        //      now.)
        //
        //  m_pszBuffer
        //      This is the buffer for this string.
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard4 m_c4BufChars;
        mutable tCIDLib::TCard4 m_c4CurEnd;
        mutable tCIDLib::TCh*   m_pszBuffer;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TString,TObject)
        DefPolyDup(TString)
        NulObject(TString)
};


#pragma CIDLIB_POPPACK



// ---------------------------------------------------------------------------
//  Global operators
// ---------------------------------------------------------------------------
inline TString operator+(const TString& str1, const TString& str2)
{
    TString strRet(str1, str2.c4Length());
    strRet.Append(str2);
    return tCIDLib::ForceMove(strRet);
}
