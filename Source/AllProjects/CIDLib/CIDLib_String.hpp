//
// FILE NAME: CIDLib_String.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/16/1992
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TString class, which (suprise) implements a
//  wrapper for XMLCh based strings. The string is maintained null terminated,
//  so that it can be passed to system APIs, but the length is also maintained
//  at all times. So appends or other ops that require access to the end of
//  the string are very fast.
//
//  TStrBuf is the object that actually contains the string buffer. It provides
//  a simple set of fundamental operations via which all string manipulations
//  can be done. Each TString object has one of these objects, via which it
//  manages the actual string data. At this time, these buffer objects are not
//  reference counted, but it might be in the future. Using this buffer object
//  will allow us to implement copy on write, ref counted buffers later if
//  we decide to.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  TStrBuf doesn't implement TObject, since its purely for use by the
//      TString class.
//
//  2)  Only const access is provided to the raw buffer, for passing to system
//      APIs internally (or in your code if you must wrap some third party
//      code.) Because we may move to a ref counted buffer at some point in
//      the future, direct access cannot be allowed.
//
//  3)  Basically almost all of the TString class is made up of inlined
//      methods which just delegate to the string buffer object.
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
//  CLASS: TStrCat
// PREFIX: scat
//
//  Exists purely to be passed to the ctor of a string, in order to concatenate
//  multiple string together. We build up the raw string buffer, and the string
//  object steals it, leaving us with a null.
//
//  This is necessary because multiple strings conflicts with the token replacement
//  scheme (because TString implements MFormattable which all of the token
//  replacement parametres are.)
// ---------------------------------------------------------------------------
class CIDLIBEXP TStrCat
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TStrCat() = delete;

        TStrCat(const TStrCat&) = delete;

        TStrCat
        (
            const   TString&                str1
            , const TString&                str2
        );

        TStrCat
        (
            const   TString&                str1
            , const TString&                str2
            , const TString&                str3
        );

        TStrCat
        (
            const   TString&                str1
            , const TString&                str2
            , const TString&                str3
            , const TString&                str4
        );

        TStrCat
        (
            const   TString&                str1
            , const tCIDLib::TCh            chSep
        );

        TStrCat
        (
            const   TString&                str1
            , const tCIDLib::TCh            chSep
            , const TString&                str2
        );

        TStrCat
        (
            const   tCIDLib::TCh* const     psz1
            , const TString&                str2
        );

        TStrCat
        (
            const   TString&                str1
            , const tCIDLib::TCh* const     psz2
        );

        TStrCat
        (
            const   tCIDLib::TCh* const     psz1
            , const tCIDLib::TCh* const     psz2
        );

        TStrCat
        (
            const   tCIDLib::TCh* const     psz1
            , const tCIDLib::TCh            chSep
            , const tCIDLib::TCh* const     psz2
        );

        ~TStrCat();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCh* pszStealBuf
        (
                    tCIDLib::TCard4&        c4Len
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        mutable tCIDLib::TCard4 m_c4Len;
        mutable tCIDLib::TCh*   m_pszBuf;
};


// ---------------------------------------------------------------------------
//  CLASS: TString
// PREFIX: str
// ---------------------------------------------------------------------------
class CIDLIBEXP TString :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strEmpty();


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean
        bComp(const TString& str1, const TString& str2)
        {
            return str1.bCompare(str2);
        }

        static tCIDLib::TBoolean
        bCompI(const TString& str1, const TString& str2)
        {
            return str1.bCompareI(str2);
        }

        static tCIDLib::TCard1 c1ToHex
        (
            const   tCIDLib::TSCh           chToXlat
        );

        static tCIDLib::TCard1 c1ToHex
        (
            const   tCIDLib::TCh            chToXlat
        );

        static tCIDLib::ESortComps eComp(const TString& str1, const TString& str2)
        {
            return str1.eCompare(str2);
        }

        static tCIDLib::ESortComps eCompI(const TString& str1, const TString& str2)
        {
            return str1.eCompareI(str2);
        }

        static tCIDLib::TVoid FromHex
        (
            const   tCIDLib::TCard1         c1ToXlat
            ,       tCIDLib::TSCh&          chOne
            ,       tCIDLib::TSCh&          chTwo
        );

        static tCIDLib::TVoid FromHex
        (
            const   tCIDLib::TCard1         c1ToXlat
            ,       tCIDLib::TCh&           chOne
            ,       tCIDLib::TCh&           chTwo
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

        TString
        (
            const   tCIDLib::TCh*           pszInitValue
        );

        TString
        (
            const   tCIDLib::TCh* const     pszInitValue
            , const tCIDLib::TCard4         c4ExtraChars
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
            const   tCIDLib::TMsgId         midToLoad
            , const TFacility&              facSource
        );

        TString
        (
            const   MFormattable&           fmtblInitValue
            , const TStreamFmt&             strmfToUse
        );

        TString
        (
            const   MFormattable&           fmtblInitValue
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

        explicit TString
        (
            const   tCIDLib::TSCh* const    pszInit
        );

        TString
        (
            const   TStrCat&                scatSrc
        );

        TString
        (
            const   TString&                strSrc
        );

        TString
        (
                    TString&&               strSrc
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

        tCIDLib::TCh& operator[]
        (
            const   tCIDLib::TCard4         c4Ind
        );

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
            const   TString&                strToAppend
        );

        tCIDLib::TVoid Append
        (
            const   TString&                strToAppend1
            , const TString&                strToAppend2
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh* const     pszToAppend
            , const tCIDLib::TCh* const     pszToAppend2 = 0
            , const tCIDLib::TCh* const     pszToAppend3 = 0
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh            chToAppend
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
            const   MFormattable&           fmtblSrc
        );

        tCIDLib::TVoid AppendSubStr
        (
            const   TString&                strToCopyFrom
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid AppendSubStr
        (
            const   tCIDLib::TCh* const     pszToCopyFrom
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TBoolean bCapAtChar
        (
            const   tCIDLib::TCh            chAt
        );

        tCIDLib::TBoolean bCompare
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean bCompare
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TBoolean bCompareI
        (
            const   TString&                strSrc
        )   const;

        tCIDLib::TBoolean bCompareI
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const;

        tCIDLib::TBoolean bCompareN
        (
            const   TString&                strSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TBoolean bCompareN
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TBoolean bCompareNI
        (
            const   TString&                strSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TBoolean bCompareNI
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TBoolean bContainsChar
        (
            const   tCIDLib::TCh            chToFind
        )   const noexcept;

        tCIDLib::TBoolean bContainsSubStr
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bEndsWith
        (
            const   tCIDLib::TCh* const     pszToCheck
        )   const;

        tCIDLib::TBoolean bEndsWith
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bEndsWithI
        (
            const   tCIDLib::TCh* const     pszToCheck
        )   const;

        tCIDLib::TBoolean bEndsWithI
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bExtractNthToken
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCh            chSepChar
            ,       TString&                strToFill
            , const tCIDLib::TBoolean       bStripWS = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bFindTokenList
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bFirstOccurrence
        (
            const   tCIDLib::TCh            chToFind
            ,       tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bFirstOccurrence
        (
            const   TString&                strSubStrToFind
            ,       tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bIsAlpha() const;

        tCIDLib::TBoolean bIsAlphaNum() const;

        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TBoolean bLastOccurrence
        (
            const   tCIDLib::TCh            chToFind
            ,       tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bLastOccurrence
        (
            const   TString&                strSubStrToFind
            ,       tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bNextOccurrence
        (
            const   tCIDLib::TCh            chToFind
            ,       tCIDLib::TCard4&        c4StartPos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bNextOccurrence
        (
            const   TString&                strSubStrToFind
            ,       tCIDLib::TCard4&        c4StartPos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bPrevOccurrence
        (
            const   tCIDLib::TCh            chToFind
            ,       tCIDLib::TCard4&        c4StartPos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        tCIDLib::TBoolean bPrevOccurrence
        (
            const   TString&                strSubStrToFind
            ,       tCIDLib::TCard4&        c4StartPos
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
            ,       tCIDLib::TCard4&        c4StartAt
            , const tCIDLib::TBoolean       bReplaceAll
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        );

        tCIDLib::TBoolean bSplit
        (
                    TString&                strSecondHalf
            , const tCIDLib::TCh            chDivider
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        );

        tCIDLib::TBoolean bStartsWith
        (
            const   tCIDLib::TCh* const     pszToCheck
        )   const;

        tCIDLib::TBoolean bStartsWith
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bStartsWithI
        (
            const   tCIDLib::TCh* const     pszToCheck
        )   const;

        tCIDLib::TBoolean bStartsWithI
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bToBoolean
        (
                    tCIDLib::TBoolean&      bToFill
        )   const noexcept;

        tCIDLib::TBoolean bToCard1
        (
                    tCIDLib::TCard1&        c1ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToCard2
        (
                    tCIDLib::TCard2&        c2ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToCard4
        (
                    tCIDLib::TCard4&        c4ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToCard8
        (
                    tCIDLib::TCard8&        c8ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToFloat4
        (
                    tCIDLib::TFloat4&       f4ToFill
        )   const noexcept;

        tCIDLib::TBoolean bToFloat8
        (
                    tCIDLib::TFloat8&       f8ToFill
        )   const noexcept;

        tCIDLib::TBoolean bToInt1
        (
                    tCIDLib::TInt1&         i1ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToInt2
        (
                    tCIDLib::TInt2&         i2ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToInt4
        (
                    tCIDLib::TInt4&         i4ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bToInt8
        (
                    tCIDLib::TInt8&         i8ToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const noexcept;

        tCIDLib::TBoolean bTokenExists
        (
            const   tCIDLib::TCh            chToken
        )   const;

        tCIDLib::TCard4 c4BufChars() const;

        tCIDLib::TCard4 c4Length() const;

        tCIDLib::TCard4 c4Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TCard8 c8Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TCh chAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCh chFirst() const;

        tCIDLib::TCh chLast() const;

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
                    TString&                strTarget
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

        tCIDLib::ESortComps eCompare
        (
            const   tCIDLib::TCh* const     pszToCompare
        )   const;

        tCIDLib::ESortComps eCompare
        (
            const   TString&                strToCompare
        )   const;

        tCIDLib::ESortComps eCompareN
        (
            const   tCIDLib::TCh* const     pszToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        tCIDLib::ESortComps eCompareN
        (
            const   TString&                strToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        tCIDLib::ESortComps eCompareI
        (
            const   tCIDLib::TCh* const     pszToCompare
        )   const;

        tCIDLib::ESortComps eCompareI
        (
            const   TString&                strToCompare
        )   const;

        tCIDLib::ESortComps eCompareNI
        (
            const   tCIDLib::TCh* const     pszToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        tCIDLib::ESortComps eCompareNI
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

        tCIDLib::EFindRes eTokenExists
        (
            const   tCIDLib::TCh            chToken
        )   const;

        tCIDLib::TVoid ExportChars
        (
                    tCIDLib::TCh* const     pszTarget
            , const tCIDLib::TCard4         c4MaxCount
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

        tCIDLib::TVoid FromZStr
        (
            const   tCIDLib::TCh* const     pszSource
            , const tCIDLib::TCard4         c4Count = 0
        );

        tCIDLib::TVoid FromShortZStr
        (
            const   tCIDLib::TSCh* const    pszNewValue
        );

        tCIDLib::TFloat8 f8Val() const;

        tCIDLib::THashVal hshCalcHash
        (
            const   tCIDLib::TCard4         c4Modulus
        )   const;

        tCIDLib::TInt4 i4Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TInt8 i8Val
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
            const   TString&                strToInsert
            , const tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid Insert
        (
            const   tCIDLib::TCh* const     pszToInsert
            , const tCIDLib::TCard4         c4Index
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

        const tCIDLib::TCh* pszBuffer() const;

        const tCIDLib::TCh* pszBufferAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCh* pszDupBuffer() const;

        const tCIDLib::TCh* pszEnd() const;

        tCIDLib::TVoid Prepend
        (
            const   TString&                strToPrepend
        );

        tCIDLib::TVoid Prepend
        (
            const   tCIDLib::TCh* const     pszToPrepend
        );

        tCIDLib::TVoid Prepend
        (
            const   tCIDLib::TCh            chToPrepend
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

        tCIDLib::TVoid SetFormatted
        (
            const   MFormattable&           fmtblSrc
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

        tCIDLib::TVoid SetLast
        (
            const   tCIDLib::TCh            chNew
        );

        tCIDLib::TVoid StreamInConverted
        (
                    TBinInStream&           strmSrc
            ,       TTextConverter&         tcvtToUse
        );

        tCIDLib::TVoid StreamOutConverted
        (
                    TBinOutStream&          strmTarget
            ,       TTextConverter&         tcvtToUse
        );

        tCIDLib::TVoid Strip
        (
            const   tCIDLib::TCh* const     pszChars
            , const tCIDLib::EStripModes    eStripmode = tCIDLib::EStripModes::LeadTrail
            , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
        );

        tCIDLib::TVoid Strip
        (
            const   TString&                strChars
            , const tCIDLib::EStripModes    eStripmode = tCIDLib::EStripModes::LeadTrail
            , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
        );

        tCIDLib::TVoid StripWhitespace
        (
            const   tCIDLib::EStripModes    eStripMode = tCIDLib::EStripModes::LeadTrail
            , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
        );

        tCIDLib::TVoid ToLower
        (
            const   tCIDLib::TCard4         c4StartInd  = 0
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
            , const tCIDLib::TCard4         c4StartInd  = 0
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
                    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  CLASS: TStrBuf
        // PREFIX: strb
        // -------------------------------------------------------------------
        class CIDLIBEXP TStrBuf
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TStrBuf();

                TStrBuf
                (
                    const   tCIDLib::TCard4         c4BufSize
                );

                TStrBuf
                (
                    const   tCIDLib::TCh            chInit
                );

                TStrBuf
                (
                    const   tCIDLib::TCh* const     pszInit
                    , const tCIDLib::TCard4         c4ExtraChars
                );

                TStrBuf
                (
                    const   TStrBuf&                strbInit
                    , const tCIDLib::TCard4         c4ExtraChars
                );

                TStrBuf
                (
                            tCIDLib::TCh* const     pszToAdopt
                    , const tCIDLib::EAdoptOpts     eAdopt
                );

                TStrBuf
                (
                    const   tCIDLib::TCh* const     pszInit1
                    , const tCIDLib::TCh* const     pszInit2
                );

                TStrBuf
                (
                    const   TStrBuf&                strInit1
                    , const TStrBuf&                strInit2
                );

                TStrBuf
                (
                    const   TStrBuf&                strInit1
                    , const tCIDLib::TCh* const     pszInit2
                );

                TStrBuf
                (
                    const   tCIDLib::TCh* const     pszInit1
                    , const TStrBuf&                strInit2
                );

                TStrBuf
                (
                    const   TStrCat&                scatSrc
                );

                TStrBuf
                (
                    const   TStrBuf&                strbSrc
                );

                TStrBuf
                (
                            TStrBuf&&               strbSrc
                );

                ~TStrBuf();


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TStrBuf& operator=
                (
                    const   TStrBuf&                strbSrc
                );

                TStrBuf& operator=
                (
                            TStrBuf&&               strbSrc
                );


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TVoid Append
                (
                    const   tCIDLib::TCh            chToAppend
                    , const tCIDLib::TCard4         c4Count
                );

                tCIDLib::TVoid Append
                (
                    const   tCIDLib::TCh* const     pszToAppend
                    , const tCIDLib::TCh* const     pszToAppend2
                    , const tCIDLib::TCh* const     pszToAppend3
                );

                tCIDLib::TVoid Append
                (
                    const   TStrBuf&                strToAppend
                );

                tCIDLib::TVoid AppendSubStr
                (
                    const   TStrBuf&                strbCopyFrom
                    , const tCIDLib::TCard4         c4Start
                    , const tCIDLib::TCard4         c4Len
                );

                tCIDLib::TVoid AppendSubStr
                (
                    const   tCIDLib::TCh* const     pszCopyFrom
                    , const tCIDLib::TCard4         c4Start
                    , const tCIDLib::TCard4         c4Len
                );

                tCIDLib::TBoolean bCapAtChar
                (
                    const   tCIDLib::TCh            chAt
                );

                tCIDLib::TBoolean bCheckPrefix
                (
                    const   tCIDLib::TCh* const     pszToCheck
                    , const tCIDLib::TBoolean       bCaseSensitive
                )   const;

                tCIDLib::TBoolean bCheckPrefix
                (
                    const   TStrBuf&                strbToCheck
                    , const tCIDLib::TBoolean       bCaseSensitive
                )   const;

                tCIDLib::TBoolean bCheckSuffix
                (
                    const   tCIDLib::TCh* const     pszToCheck
                    , const tCIDLib::TBoolean       bCaseSensitive
                )   const;

                tCIDLib::TBoolean bCheckSuffix
                (
                    const   TStrBuf&                strbToCheck
                    , const tCIDLib::TBoolean       bCaseSensitive
                )   const;

                tCIDLib::TBoolean bIsAlpha() const;

                tCIDLib::TBoolean bIsAlphaNum() const;

                tCIDLib::TBoolean bEqual
                (
                    const   TStrBuf&                strbToComp
                )   const;

                tCIDLib::TBoolean bReplaceChar
                (
                    const   tCIDLib::TCh            chToReplace
                    , const tCIDLib::TCh            chReplaceWith
                );

                tCIDLib::TCard4 c4BufChars() const;

                tCIDLib::TCard4 c4Length() const;

                tCIDLib::TCh& chAt
                (
                    const   tCIDLib::TCard4         c4Index
                );

                tCIDLib::TCh chAt
                (
                    const   tCIDLib::TCard4         c4Index
                )   const;

                tCIDLib::TCh chLast() const;

                tCIDLib::TVoid CapAt
                (
                    const   tCIDLib::TCard4         c4Index
                );

                tCIDLib::TVoid Clear();

                tCIDLib::TVoid CopyInSubStr
                (
                    const   TString&                strSource
                    , const tCIDLib::TCard4         c4Start
                    , const tCIDLib::TCard4         c4Len
                );

                tCIDLib::TVoid CopyOutSubStr
                (
                            TString&                strTarget
                    , const tCIDLib::TCard4         c4Start
                    , const tCIDLib::TCard4         c4Len
                )   const;

                tCIDLib::TVoid Cut
                (
                    const   tCIDLib::TCard4         c4Start
                    , const tCIDLib::TCard4         c4Len
                );

                tCIDLib::TVoid CutUpTo
                (
                    const   tCIDLib::TCh            chFind
                );

                tCIDLib::TVoid DeleteLast();

                tCIDLib::TVoid FormatToFld
                (
                    const   TStrBuf&                strbToFormat
                    , const tCIDLib::TCard4         c4FldWidth
                    , const tCIDLib::EHJustify      eJustify
                    , const tCIDLib::TCh            chFill
                    , const tCIDLib::TCard4         c4TrailingSp
                );

                tCIDLib::TVoid FromZStr
                (
                    const   tCIDLib::TCh* const     pszSource
                    , const tCIDLib::TCard4         c4Count
                );

                tCIDLib::THashVal hshCalcHash
                (
                    const   tCIDLib::TCard4         c4Modulus
                )   const;

                tCIDLib::TVoid ImportChars
                (
                    const   tCIDLib::TCh* const     pszSource
                    , const tCIDLib::TCard4         c4SrcCount
                );

                tCIDLib::TVoid Insert
                (
                    const   TStrBuf&                strbInsert
                    , const tCIDLib::TCard4         c4Ind
                );

                tCIDLib::TVoid Insert
                (
                    const   tCIDLib::TCh* const     pszInsert
                    , const tCIDLib::TCard4         c4Ind
                );

                const tCIDLib::TCh* pszBuffer() const;

                const tCIDLib::TCh* pszBufferAt
                (
                    const   tCIDLib::TCard4         c4At
                )   const;

                tCIDLib::TCh* pszDupBuffer() const;

                tCIDLib::TVoid PutAt
                (
                    const   tCIDLib::TCard4         c4Index
                    , const tCIDLib::TCh            chToPut
                );

                tCIDLib::TVoid Reallocate
                (
                    const   tCIDLib::TCard4         c4NewSize
                    , const tCIDLib::TBoolean       bPreserveContent
                );

                tCIDLib::TVoid Replace
                (
                    const   tCIDLib::TCard4         c4RepStart
                    , const tCIDLib::TCard4         c4RepEnd
                    , const tCIDLib::TCh* const     pszRepText
                    , const tCIDLib::TCard4         c4RepLen
                );

                tCIDLib::TVoid Set
                (
                    const   tCIDLib::TCh* const     pszNewValue
                );

                tCIDLib::TVoid Set
                (
                    const   TStrBuf&                strbNewValue
                );

                tCIDLib::TVoid SetFromShort
                (
                    const   tCIDLib::TSCh* const    pszNewValue
                );

                tCIDLib::TVoid SetLast
                (
                    const   tCIDLib::TCh            chNew
                );

                tCIDLib::TVoid StreamFrom
                (
                            TBinInStream&           strmToReadFrom
                );

                tCIDLib::TVoid StreamTo
                (
                            TBinOutStream&          strmToWriteTo
                )   const;

                tCIDLib::TVoid Strip
                (
                    const   tCIDLib::TCh* const     pszStripChars
                    , const tCIDLib::EStripModes    eStripMode
                    , const tCIDLib::TCh            chRepChar = kCIDLib::chSpace
                );

                tCIDLib::TVoid TestIndex
                (
                    const   tCIDLib::TCard4         c4ToTest
                )   const;

                tCIDLib::TVoid ToLower
                (
                    const   tCIDLib::TCard4         c4StartInd
                    , const tCIDLib::TCard4         c4Len
                );

                tCIDLib::TVoid ToUpper
                (
                    const   tCIDLib::TCard4         c4StartInd
                    , const tCIDLib::TCard4         c4Len
                );

                tCIDLib::TVoid ToZStr
                (
                            tCIDLib::TCh* const     pszTarget
                    , const tCIDLib::TCard4         c4MaxChars
                    , const tCIDLib::TCard4         c4StartInd
                    , const tCIDLib::TBoolean       bAddNull
                )   const;


            private :
                // -----------------------------------------------------------
                //  Private data members.
                //
                //  m_c4CurEnd
                //      This is the index of the null terminator, i.e. this is
                //      where you'd end up appending (and its the number of
                //      chars actually in the buffer now.)
                //
                //  m_c4BufChars
                //      This is the current size of the buffer, minus one.
                //      I.e. the buffer is one bigger than this (so that there
                //      is room for the null terminator even if there are this
                //      many chars in it.) It will grow as required.
                //
                //  m_pszBuffer
                //      This is the buffer for this string.
                // -----------------------------------------------------------
                tCIDLib::TCard4 m_c4CurEnd;
                tCIDLib::TCard4 m_c4BufChars;
                tCIDLib::TCh*   m_pszBuffer;
        };


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid BadHexChar
        (
            const   tCIDLib::TCh            chToXlat
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strbData
        //      This is the string buffer object that holds the actual string
        //      data. We do all our work via it, in order to remain very
        //      flexible in how it is actually managed. We might at some point
        //      decide to reference count these, and do a copy on write type
        //      of deal.
        // -------------------------------------------------------------------
        TStrBuf m_strbData;


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
    return strRet;
}


// ---------------------------------------------------------------------------
//  TStrBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard4 TString::TStrBuf::c4BufChars() const
{
    return m_c4BufChars;
}

inline tCIDLib::TCard4 TString::TStrBuf::c4Length() const
{
    return m_c4CurEnd;
}

//
//  The writable version of this is out of line and always checks the index,
//  but this one just returns a character, so it is inlined for speed and only
//  checks the index in debug mode. This is an extremely commonly method and
//  often in a loop over large strings, so the benefits are large to inline
//  it.
//
inline tCIDLib::TCh TString::TStrBuf::chAt(const tCIDLib::TCard4 c4Ind) const
{
    #if CID_DEBUG_ON
    TestIndex(c4Ind);
    #endif
    return m_pszBuffer[c4Ind];
}


// ---------------------------------------------------------------------------
//  TString: Public, static methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard1 TString::c1ToHex(const tCIDLib::TSCh chToXlat)
{
    tCIDLib::TCard1 c1Ret(0);
    if ((chToXlat >= 0x41) && (chToXlat <= 0x46))
        c1Ret = tCIDLib::TCard1(10) + (chToXlat - 0x41);
    else if ((chToXlat >= 0x30) && (chToXlat <= 0x39))
        c1Ret = chToXlat - 0x30;
    else
        BadHexChar(tCIDLib::TCh(chToXlat));
    return c1Ret;
}

inline tCIDLib::TCard1 TString::c1ToHex(const tCIDLib::TCh chToXlat)
{
    tCIDLib::TCard1 c1Ret(0);
    if ((chToXlat >= kCIDLib::chLatin_A) && (chToXlat <= kCIDLib::chLatin_F))
        c1Ret = tCIDLib::TCard1(10 + (chToXlat - kCIDLib::chLatin_A));
    else if ((chToXlat >= kCIDLib::chDigit0) && (chToXlat <= kCIDLib::chDigit9))
        c1Ret = tCIDLib::TCard1(chToXlat - kCIDLib::chDigit0);
    else
        BadHexChar(chToXlat);
    return c1Ret;
}


// THESE PUT OUT UPPER CASE and some folks depend on that. So don't change the case
inline tCIDLib::TVoid TString::FromHex( const   tCIDLib::TCard1 c1ToXlat
                                        ,       tCIDLib::TSCh&  chOne
                                        ,       tCIDLib::TSCh&  chTwo)
{
    tCIDLib::TCard1 c1Tmp = c1ToXlat & 0xF;
    if (c1Tmp)
    {
        chTwo = (c1Tmp > 9) ? tCIDLib::TSCh(kCIDLib::chLatin_A + (c1Tmp - 10))
                            : tCIDLib::TSCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chTwo = tCIDLib::TSCh(kCIDLib::chDigit0);
    }

    c1Tmp = c1ToXlat >> 4;
    if (c1Tmp & 0xF)
    {
        chOne = (c1Tmp > 9) ? tCIDLib::TSCh(kCIDLib::chLatin_A + (c1Tmp - 10))
                            : tCIDLib::TSCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chOne = tCIDLib::TSCh(kCIDLib::chDigit0);
    }
}

inline tCIDLib::TVoid TString::FromHex( const   tCIDLib::TCard1 c1ToXlat
                                        ,       tCIDLib::TCh&   chOne
                                        ,       tCIDLib::TCh&   chTwo)
{
    tCIDLib::TCard1 c1Tmp = c1ToXlat & 0xF;
    if (c1Tmp)
    {
        chTwo = (c1Tmp > 9) ? tCIDLib::TCh(kCIDLib::chLatin_A+ (c1Tmp - 10))
                            : tCIDLib::TCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chTwo = kCIDLib::chDigit0;
    }

    c1Tmp = c1ToXlat >> 4;
    if (c1Tmp & 0xF)
    {
        chOne = (c1Tmp > 9) ? tCIDLib::TCh(kCIDLib::chLatin_A + (c1Tmp - 10))
                            : tCIDLib::TCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chOne = kCIDLib::chDigit0;
    }
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TVoid
TString::Append(const tCIDLib::TCh chSrc, const tCIDLib::TCard4 c4Count)
{
    m_strbData.Append(chSrc, c4Count);
}

inline tCIDLib::TVoid
TString::Append(const   tCIDLib::TCh* const pszSrc
                , const tCIDLib::TCh* const pszSrc2
                , const tCIDLib::TCh* const pszSrc3)
{
    m_strbData.Append(pszSrc, pszSrc2, pszSrc3);
}

inline tCIDLib::TVoid TString::Append(const TString& strSrc)
{
    m_strbData.Append(strSrc.m_strbData);
}

inline tCIDLib::TVoid
TString::AppendSubStr(  const   TString&        strToCopyFrom
                        , const tCIDLib::TCard4 c4Start
                        , const tCIDLib::TCard4 c4Len)
{
    m_strbData.AppendSubStr(strToCopyFrom.m_strbData, c4Start, c4Len);
}

inline tCIDLib::TVoid
TString::AppendSubStr(  const   tCIDLib::TCh* const pszToCopyFrom
                        , const tCIDLib::TCard4     c4Start
                        , const tCIDLib::TCard4     c4Len)
{
    m_strbData.AppendSubStr(pszToCopyFrom, c4Start, c4Len);
}

inline tCIDLib::TBoolean TString::bIsEmpty() const
{
    return !m_strbData.c4Length();
}

inline tCIDLib::TBoolean
TString::bEndsWith(const tCIDLib::TCh* const pszToCheck) const
{
    return m_strbData.bCheckSuffix(pszToCheck, kCIDLib::True);
}

inline tCIDLib::TBoolean TString::bEndsWith(const TString& strToCheck) const
{
    return m_strbData.bCheckSuffix(strToCheck.m_strbData, kCIDLib::True);
}

inline tCIDLib::TBoolean
TString::bEndsWithI(const tCIDLib::TCh* const pszToCheck) const
{
    return m_strbData.bCheckSuffix(pszToCheck, kCIDLib::False);
}

inline tCIDLib::TBoolean TString::bEndsWithI(const TString& strToCheck) const
{
    return m_strbData.bCheckSuffix(strToCheck.m_strbData, kCIDLib::False);
}

inline tCIDLib::TBoolean TString::bIsAlpha() const
{
    return m_strbData.bIsAlpha();
}

inline tCIDLib::TBoolean TString::bIsAlphaNum() const
{
    return m_strbData.bIsAlphaNum();
}

inline tCIDLib::TBoolean
TString::bStartsWith(const tCIDLib::TCh* const pszToCheck) const
{
    return m_strbData.bCheckPrefix(pszToCheck, kCIDLib::True);
}

inline tCIDLib::TBoolean TString::bStartsWith(const TString& strToCheck) const
{
    return m_strbData.bCheckPrefix(strToCheck.m_strbData, kCIDLib::True);
}

inline tCIDLib::TBoolean
TString::bStartsWithI(const tCIDLib::TCh* const pszToCheck) const
{
    return m_strbData.bCheckPrefix(pszToCheck, kCIDLib::False);
}

inline tCIDLib::TBoolean TString::bStartsWithI(const TString& strToCheck) const
{
    return m_strbData.bCheckPrefix(strToCheck.m_strbData, kCIDLib::False);
}

inline tCIDLib::TCard4 TString::c4BufChars() const
{
    return m_strbData.c4BufChars();
}

inline tCIDLib::TCard4 TString::c4Length() const
{
    return m_strbData.c4Length();
}

inline tCIDLib::TCh TString::chAt(const tCIDLib::TCard4 c4Index) const
{
    return m_strbData.chAt(c4Index);
}

inline tCIDLib::TCh TString::chFirst() const
{
    return m_strbData.chAt(0);
}

inline tCIDLib::TCh TString::chLast() const
{
    return m_strbData.chLast();
}

inline tCIDLib::TVoid TString::CapAt(const tCIDLib::TCard4 c4Index)
{
    m_strbData.CapAt(c4Index);
}

inline tCIDLib::TVoid TString::Clear()
{
    m_strbData.Clear();
}

inline tCIDLib::TVoid
TString::CopyInSubStr(  const   TString&        strSource
                        , const tCIDLib::TCard4 c4Start
                        , const tCIDLib::TCard4 c4Len)
{
    m_strbData.Clear();
    m_strbData.AppendSubStr(strSource.m_strbData, c4Start, c4Len);
}

inline tCIDLib::TVoid
TString::CopyOutSubStr(         TString&        strTarget
                        , const tCIDLib::TCard4 c4Start
                        , const tCIDLib::TCard4 c4Len) const
{
    strTarget.m_strbData.Clear();
    strTarget.m_strbData.AppendSubStr(m_strbData, c4Start, c4Len);
}

inline tCIDLib::TVoid TString::Cut( const   tCIDLib::TCard4 c4Start
                                    , const tCIDLib::TCard4 c4Len)
{
    m_strbData.Cut(c4Start, c4Len);
}

inline tCIDLib::TVoid TString::CutUpTo(const tCIDLib::TCh chFind)
{
    m_strbData.CutUpTo(chFind);
}

inline tCIDLib::TVoid TString::DeleteLast()
{
    m_strbData.DeleteLast();
}

inline tCIDLib::TVoid
TString::ExportChars(       tCIDLib::TCh* const pszTarget
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::TCard4     c4StartInd) const
{
    // Tell it not to add any null terminator
    m_strbData.ToZStr(pszTarget, c4MaxChars, c4StartInd, kCIDLib::False);
}

inline tCIDLib::TVoid
TString::FromZStr(  const   tCIDLib::TCh* const pszSource
                    , const tCIDLib::TCard4     c4Count)
{
    m_strbData.FromZStr(pszSource, c4Count);
}

inline tCIDLib::TVoid TString::FromShortZStr(const tCIDLib::TSCh* const pszNewValue)
{
    m_strbData.SetFromShort(pszNewValue);
}

inline tCIDLib::TVoid
TString::ImportChars(const  tCIDLib::TCh* const pszSource
                    , const tCIDLib::TCard4     c4SrcCount)
{
    m_strbData.ImportChars(pszSource, c4SrcCount);
}

inline tCIDLib::TVoid
TString::Insert(const TString& strInsert, const tCIDLib::TCard4 c4Ind)
{
    m_strbData.Insert(strInsert.m_strbData, c4Ind);
}

inline tCIDLib::TVoid
TString::Insert(const tCIDLib::TCh* const pszInsert, const tCIDLib::TCard4 c4Ind)
{
    m_strbData.Insert(pszInsert, c4Ind);
}

inline const tCIDLib::TCh* TString::pszBuffer() const
{
    return m_strbData.pszBuffer();
}

inline const tCIDLib::TCh* TString::pszBufferAt(const tCIDLib::TCard4 c4At) const
{
    return m_strbData.pszBufferAt(c4At);
}

inline tCIDLib::TCh* TString::pszDupBuffer() const
{
    return m_strbData.pszDupBuffer();
}

inline const tCIDLib::TCh* TString::pszEnd() const
{
    return m_strbData.pszBufferAt(m_strbData.c4Length());
}

inline tCIDLib::TVoid TString::Prepend(const TString& strPrepend)
{
    m_strbData.Insert(strPrepend.m_strbData, 0);
}

inline tCIDLib::TVoid TString::Prepend(const tCIDLib::TCh* const pszPrepend)
{
    m_strbData.Insert(pszPrepend, 0);
}

inline tCIDLib::TVoid
TString::PutAt( const   tCIDLib::TCard4 c4Index
                 , const tCIDLib::TCh   chToPut)
{
    m_strbData.PutAt(c4Index, chToPut);
}

inline tCIDLib::TVoid
TString::Reallocate(const   tCIDLib::TCard4     c4NewSize
                    , const tCIDLib::TBoolean   bPreserveContent)
{
    m_strbData.Reallocate(c4NewSize, bPreserveContent);
}

inline tCIDLib::TBoolean
TString::bReplaceChar(  const   tCIDLib::TCh    chToReplace
                        , const tCIDLib::TCh    chReplaceWith)
{
    return m_strbData.bReplaceChar(chToReplace, chReplaceWith);
}

inline tCIDLib::TVoid TString::SetLast(const tCIDLib::TCh chNew)
{
    m_strbData.SetLast(chNew);
}

inline tCIDLib::TVoid
TString::Strip( const   tCIDLib::TCh* const     pszChars
                , const tCIDLib::EStripModes    eStripMode
            , const tCIDLib::TCh                chRepChar)
{
    m_strbData.Strip(pszChars, eStripMode, chRepChar);
}

inline tCIDLib::TVoid
TString::Strip( const   TString&                strChars
                , const tCIDLib::EStripModes    eStripMode
            , const tCIDLib::TCh                chRepChar)
{
    m_strbData.Strip(strChars.m_strbData.pszBuffer(), eStripMode, chRepChar);
}

inline tCIDLib::TVoid
TString::ToLower(const  tCIDLib::TCard4 c4StartInd
                , const tCIDLib::TCard4 c4Len)
{
    m_strbData.ToLower(c4StartInd, c4Len);
}

inline tCIDLib::TVoid
TString::ToUpper(const  tCIDLib::TCard4 c4StartInd
                , const tCIDLib::TCard4 c4Len)
{
    m_strbData.ToUpper(c4StartInd, c4Len);
}

inline tCIDLib::TVoid
TString::ToZStr(        tCIDLib::TCh* const pszTarget
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::TCard4     c4StartInd) const
{
    m_strbData.ToZStr(pszTarget, c4MaxChars, c4StartInd, kCIDLib::True);
}

