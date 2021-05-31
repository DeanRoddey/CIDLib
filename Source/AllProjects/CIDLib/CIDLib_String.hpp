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
//  This file implements the TString class, which (surprise) implements a wrapper
//  for an array of (wide) characters. The string is maintained null
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
//  In order to be efficient, we support small string optimzation. This provides
//  a number of benefits:
//
//      1. We don't have to allocate a buffer for default ctor
//      2. We don't have to allocate a buffer for moved from strings
//      3. Short strings may never have to allocate a buffer
//
//  To support this, we have a little buffer wrapper internal class. We can only
//  access the buffer via it. We have to tell it the size of buffer we need. It
//  will see if it is currently using the small string buffer and allocate and
//  fault in a buffer if needed (moving any current short string contents over to
//  it.) It supports move/copy to make life easier for us.
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
class   TStringView;


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
        [[nodiscard]] static tCIDLib::TBoolean bComp(const TString& str1, const TString& str2) noexcept;

        [[nodiscard]] static tCIDLib::TBoolean bCompI(const TString& str1, const TString& str2)  noexcept;

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

        [[nodiscard]] static tCIDLib::ESortComps eComp(const TString& str1, const TString& str2) noexcept;

        [[nodiscard]] static tCIDLib::ESortComps eCompI(const TString& str1, const TString& str2) noexcept;

        [[nodiscard]] static ETokenFind eFindToken
        (
            const    tCIDLib::TCh*&         pszStart
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

        static const TString& strEmpty() noexcept;
        static const TStringView& strvEmpty() noexcept;


        //
        //  These templates allow for arbitrary concatenation of lists of TString objects,
        //  raw strings, and single characters.
        //
        //  ConcatSz calculates the bytes required
        //  ConcatAccum accumulates the content in a string
        //  strConcat uses the above to create a string with the needed capacity, then
        //  load it up with values, and then just returns the string. The compiler should
        //  do a move of the return of the final string.
        //
        //  At the bottom of this file, some very common variations are pre-instantiated
        //  to avoid this goop being spit out in a lot of places.
        //
        //  * To avoid craziness we have separate variations for raw arrays, which need to
        //    match size. String literals aren't decayed. And of course it saves time since
        //    the size is compile time, we don't have to do a c4StrLen() on it.
        //
        template<typename T, tCIDLib::TCard4 c4Size> static tCIDLib::TCard4 c4ConcatSz(T(&tVal)[c4Size])
        {
            return c4Size;
        }

        template<typename T> static tCIDLib::TCard4 c4ConcatSz(T&& tVal)
        {
            tCIDLib::TCard4 c4Ret = 0;
            using TBase = tCIDLib::tRemoveCR<T>;
            if constexpr (tCIDLib::bIsSameType<TBase, tCIDLib::TCh>)
            {
                c4Ret = 1;
            }
            else if constexpr ((tCIDLib::bIsSameType<TBase, const tCIDLib::TCh*>)
                              || (tCIDLib::bIsSameType<TBase, tCIDLib::TCh*>))
            {
                if (tVal != nullptr)
                    c4Ret = TRawStr::c4StrLen(tVal);
            }
            else if constexpr (tCIDLib::bIsSameType<TBase, TString>)
            {
                c4Ret = tVal.c4Length();
            }
            else
            {
                static_assert(tCIDLib::FalseDependType<T>::bState);
            }
            return c4Ret;
        }

        template<typename T, typename... TArgs>
        static tCIDLib::TVoid ConcatSz(tCIDLib::TCard4& c4Accum, T&& tFirst)
        {
            c4Accum += c4ConcatSz(tCIDLib::Forward<T>(tFirst));
        }

        template<typename T, typename... TArgs>
        static tCIDLib::TVoid ConcatSz(tCIDLib::TCard4& c4Accum, T&& tFirst, TArgs&&... tArgs)
        {
            c4Accum += c4ConcatSz(tCIDLib::Forward<T>(tFirst));
            ConcatSz(c4Accum, tCIDLib::Forward<TArgs>(tArgs)...);
        }

        // Does the actual accumulation of the parameters into a string object
        template<typename T, tCIDLib::TCard4 c4Size>
        static tCIDLib::TVoid ConcatAppend(TString& strTar, T(&tVal)[c4Size])
        {
            strTar.Append(tVal);
        }

        template<typename T> tCIDLib::TVoid static ConcatAppend(TString& strTar, T&& tVal)
        {
            using TBase = tCIDLib::tRemoveCR<T>;
            if constexpr (tCIDLib::bIsSameType<TBase, tCIDLib::TCh>)
            {
                strTar.Append(tVal);
            }
            else if constexpr ((tCIDLib::bIsSameType<TBase, const tCIDLib::TCh*>)
                              || (tCIDLib::bIsSameType<TBase, tCIDLib::TCh*>))
            {
                if (tVal != nullptr)
                    strTar.Append(tVal);
            }
            else if constexpr (tCIDLib::bIsSameType<TBase, TString>)
            {
                strTar.Append(tVal);
            }
            else
            {
                static_assert(tCIDLib::FalseDependType<T>::bState);
            }
        }

        template<typename T, typename... TArgs>
        static tCIDLib::TVoid ConcatAccum(TString& strTar, T&& tFirst)
        {
            ConcatAppend(strTar, tCIDLib::Forward<T>(tFirst));
        }

        template<typename T, typename... TArgs>
        static tCIDLib::TVoid ConcatAccum(TString& strTar, T&& tFirst, TArgs&&... tArgs)
        {
            ConcatAppend(strTar, tFirst);
            ConcatAccum(strTar, tCIDLib::Forward<TArgs>(tArgs)...);
        }

        template<typename... TArgs> static TString strConcat(TArgs&&... tArgs)
        {
            // Calculate the size required
            tCIDLib::TCard4 c4Sz = 0;
            ConcatSz(c4Sz, tCIDLib::Forward<TArgs>(tArgs)...);

            // Allocate a string and accumulate
            TString strRet(c4Sz);
            ConcatAccum(strRet, tCIDLib::Forward<TArgs>(tArgs)...);

            return strRet;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TString() : m_strbData(0UL) {}

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

        explicit TString
        (
            const   TStringView&            strvSrc
            , const tCIDLib::TCard4         c4ExtraChars = 0
        );

        TString(const   tCIDLib::TCh* const pszInitValue
                , const tCIDLib::TCard4     c4ExtraChars = 0) :

            m_strbData(pszInitValue, 0, c4ExtraChars)
        {
        }

        TString(const TString& strInit, const tCIDLib::TCard4 c4ExtraChars) :

            m_strbData(strInit.pszBuffer(), strInit.c4Length(), c4ExtraChars)
        {
        }

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

        TString(const TString& strSrc) :

            m_strbData(strSrc.m_strbData)
        {
        }

        // We swap out an empty small string to the source
        TString(TString&& strSrc) :

            m_strbData(0UL)
        {
            *this = tCIDLib::ForceMove(strSrc);
        }

        // We just steal the kernel string's buffer
        TString(TKrnlString&& kstrSrc) :

            m_strbData(tCIDLib::ForceMove(kstrSrc))
        {
        }

        TString(const TKrnlString& kstrSrc) :

            m_strbData(kstrSrc)
        {
        }

        ~TString() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TString& operator=(const TStringView& strvSrc)
        {
            m_strbData = strvSrc;
            return *this;
        }

        TString& operator=(const TString& strSrc)
        {
            if (this != &strSrc)
                m_strbData = strSrc.m_strbData;
            return *this;
        }

        TString& operator=(TString&& strSrc)
        {
            if (this != &strSrc)
                m_strbData = tCIDLib::ForceMove(strSrc.m_strbData);
            return *this;
        }

        // We just steal the kernel string's buffer
        TString& operator=(TKrnlString&& kstrSrc)
        {
            m_strbData = tCIDLib::ForceMove(kstrSrc);
            return *this;
        }

        TString& operator=(const tCIDLib::TCh* const pszSrc)
        {
            m_strbData.Clear();
            m_strbData.Append(pszSrc, TRawStr::c4StrLen(pszSrc));
            return *this;
        }

        [[nodiscard]] tCIDLib::TBoolean operator==
        (
            const   TString&                strSrc
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean operator!=(const TString& strSrc) const noexcept
        {
            return !operator==(strSrc);
        }

        [[nodiscard]] tCIDLib::TBoolean operator==(const tCIDLib::TCh* const pszSrc) const noexcept
        {
            return (TRawStr::eCompareStr(pszBuffer(), pszSrc) == tCIDLib::ESortComps::Equal);
        }

        [[nodiscard]] tCIDLib::TBoolean operator!=(const tCIDLib::TCh* const pszSrc) const noexcept
        {
            return !operator==(pszSrc);
        }

        [[nodiscard]] tCIDLib::TBoolean operator==
        (
            const   TStringView&            strvSrc
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean operator!=(const TStringView& strvSrc) const noexcept
        {
            return !operator==(strvSrc);
        }

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

        [[nodiscard]] tCIDLib::TBoolean operator<(const TString& strSrc) const noexcept
        {
            return TRawStr::eCompareStr(pszBuffer(), strSrc.pszBuffer()) == tCIDLib::ESortComps::FirstLess;
        }

        [[nodiscard]] tCIDLib::TBoolean operator>(const TString& strSrc) const noexcept
        {
            return TRawStr::eCompareStr(pszBuffer(), strSrc.pszBuffer()) == tCIDLib::ESortComps::FirstGreater;
        }

        [[nodiscard]] tCIDLib::TBoolean operator<=(const TString& strSrc) const noexcept
        {
            return (TRawStr::eCompareStr(pszBuffer(), strSrc.pszBuffer()) != tCIDLib::ESortComps::FirstGreater);
        }

        [[nodiscard]] tCIDLib::TBoolean operator>=(const TString& strSrc) const noexcept
        {
            return (TRawStr::eCompareStr(pszBuffer(), strSrc.pszBuffer()) != tCIDLib::ESortComps::FirstLess);
        }

        tCIDLib::TBoolean operator<
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const noexcept;

        tCIDLib::TBoolean operator>
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const noexcept;

        tCIDLib::TBoolean operator<=
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const noexcept;

        tCIDLib::TBoolean operator>=
        (
            const   tCIDLib::TCh* const     pszSrc
        )   const noexcept;

        [[nodiscard]] tCIDLib::TCh operator[](const tCIDLib::TCard4 c4Ind) const
        {
            return chAt(c4Ind);
        }

        tCIDLib::TBoolean operator==
        (
            const   tCIDLib::TSCh* const    pszSrc
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean operator!=(const tCIDLib::TSCh* const pszSrc) const noexcept
        {
            return !operator==(pszSrc);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Append(const TString& strSrc)
        {
            m_strbData.Append(strSrc.pszBuffer(), strSrc.c4Length());
        }

        tCIDLib::TVoid Append(const TStringView& strvSrc);

        tCIDLib::TVoid Append(const TString& strSrc1, const TString& strSrc2)
        {
            Append(strSrc1);
            Append(strSrc2);
        }

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
            const   TStringView&            strvSrc
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TBoolean bCapAtChar
        (
            const   tCIDLib::TCh            chAt
        );

        [[nodiscard]] tCIDLib::TBoolean bCompare
        (
            const   TStringView&            strvSrc
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bCompareI
        (
            const   TStringView&            strvSrc
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bCompareN
        (
            const   TStringView&            strvSrc
            , const tCIDLib::TCard4         c4Count
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bCompareNI
        (
            const   TStringView&            strvSrc
            , const tCIDLib::TCard4         c4Count
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bContainsChar(const tCIDLib::TCh chToFind) const noexcept
        {
            return TRawStr::pszFindChar(pszBuffer(), chToFind) != nullptr;
        }

        // Just call the first occurrence check
        [[nodiscard]] tCIDLib::TBoolean
        bContainsSubStr(const   TString&            strToFind
                        , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
        {
            tCIDLib::TCard4 c4At = 0;
            return bFirstOccurrence(strToFind, c4At, kCIDLib::False, bCaseSensitive);
        }

        [[nodiscard]] tCIDLib::TBoolean bEndsWith(const tCIDLib::TCh* const pszToCheck) const noexcept
        {
            return bCheckSuffix(pszToCheck, kCIDLib::True);
        }

        [[nodiscard]] tCIDLib::TBoolean bEndsWith(const TString& strToCheck) const noexcept
        {
            return bCheckSuffix(strToCheck, kCIDLib::True);
        }

        [[nodiscard]] tCIDLib::TBoolean bEndsWithI(const tCIDLib::TCh* const pszToCheck) const noexcept
        {
            return bCheckSuffix(pszToCheck, kCIDLib::False);
        }

        [[nodiscard]] tCIDLib::TBoolean bEndsWithI(const TString& strToCheck) const noexcept
        {
            return bCheckSuffix(strToCheck, kCIDLib::False);
        }

        [[nodiscard]] tCIDLib::TBoolean bExtractNthToken
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCh            chSepChar
            , COP   TString&                strToFill
            , const tCIDLib::TBoolean       bStripWS = kCIDLib::True
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bFindTokenList
        (
            COP     TString&                strToFill
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bFirstOccurrence
        (
            const   tCIDLib::TCh            chToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bFirstOccurrence
        (
            const   TString&                strSubStrToFind
            , COP   tCIDLib::TCard4&        c4Pos
            , const tCIDLib::TBoolean       bAnyChar = kCIDLib::False
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::True
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bIsAlpha() const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bIsAlphaNum() const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bIsEmpty() const noexcept
        {
            return(m_strbData.c4CurEnd() == 0);
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

        [[nodiscard]] tCIDLib::TCard4 c4BufChars() const noexcept
        {
            return m_strbData.c4BufSz();
        }

        [[nodiscard]] tCIDLib::TCard4 c4Length() const noexcept
        {
            return m_strbData.c4CurEnd();
        }

        tCIDLib::TCard4 c4Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        tCIDLib::TCard8 c8Val
        (
            const   tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
        )   const;

        [[nodiscard]] tCIDLib::TCh chAt(const tCIDLib::TCard4 c4Ind) const
        {
            return m_strbData.chAt(c4Ind);
        }

        [[nodiscard]] tCIDLib::TCh chFirst() const noexcept
        {
            return chAt(0);
        }

        [[nodiscard]] tCIDLib::TCh chLast() const noexcept
        {
            return m_strbData.chLast();
        }

        tCIDLib::TVoid CapAt(const tCIDLib::TCard4 c4Index)
        {
            m_strbData.SetEnd(c4Index);
        }

        tCIDLib::TVoid Clear() noexcept
        {
            m_strbData.Clear();
        }

        tCIDLib::TVoid CopyInSubStr
        (
            const   TStringView&            strSource
            , const tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4Len = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid CopyOutSubStr
        (
            COP     TString&                strTarget
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

        tCIDLib::TVoid DeleteLast()
        {
            m_strbData.DeleteLast();
        }

        [[nodiscard]] tCIDLib::ESortComps eCompare
        (
            const   TStringView&           strvToCompare
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareN
        (
            const   TStringView&            strvToCompare
            , const tCIDLib::TCard4         c4MaxComp
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareI
        (
            const   TStringView&            strvToCompare
        )   const;

        [[nodiscard]] tCIDLib::ESortComps eCompareNI
        (
            const   TStringView&            strvToCompare
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
            const   TStringView&            strvToFormat
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

        tCIDLib::TVoid Insert(  const   TString&        strInsert
                                , const tCIDLib::TCard4 c4Ind)
        {
            Insert(strInsert.pszBuffer(), c4Ind, strInsert.c4Length());
        }

        tCIDLib::TVoid Insert
        (
            const   tCIDLib::TCh* const     pszInsert
            , const tCIDLib::TCard4         c4Ind
            , const tCIDLib::TCard4         c4SrcLen = 0
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

        [[nodiscard]] const tCIDLib::TCh* pszBuffer() const noexcept
        {
            return m_strbData.pszBuffer();
        }

        [[nodiscard]] const tCIDLib::TCh* pszBufferAt(const tCIDLib::TCard4 c4At) const
        {
            return m_strbData.pszBufferAt(c4At);
        }

        [[nodiscard]] tCIDLib::TCh* pszDupBuffer() const;

        const tCIDLib::TCh* pszEnd() const noexcept
        {
            return m_strbData.pszAtEnd();
        }

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

        tCIDLib::TVoid PutAt(const  tCIDLib::TCard4 c4Index
                            , const tCIDLib::TCh    chToPut)
        {
            m_strbData.PutAt(c4Index, chToPut);
        }

        tCIDLib::TVoid Reallocate(  const   tCIDLib::TCard4     c4NewSize
                                    , const tCIDLib::TBoolean   bPreserve = kCIDLib::True)
        {
            m_strbData.ExpandTo(c4NewSize, bPreserve);
        }

        tCIDLib::TVoid Set
        (
            const   TStringView&            strvSrc
            , const tCIDLib::TCard4         c4Extra = 0
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
        //  Private class constants
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4 c4SmallBufSz = 7;


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
        //   CLASS: TStrBuf
        //  PREFIX: strb
        //
        //  This is our internal buffer pointer. See file comments above.
        // -------------------------------------------------------------------
        class TStrBuf
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TStrBuf() = delete;

                explicit TStrBuf
                (
                    const   tCIDLib::TCard4         c4InitSz
                );

                TStrBuf
                (
                    const   tCIDLib::TCh* const     pszAdopt
                    , const tCIDLib::TCard4         c4Len
                    , const tCIDLib::TCard4         c4ExtraChars
                );

                TStrBuf
                (
                            tCIDLib::TCh* const     pszAdopt
                    , const tCIDLib::EAdoptOpts     eAdopt
                );

                TStrBuf
                (
                    const   TKrnlString&            kstrSrc
                );

                TStrBuf
                (
                            TKrnlString&&           kstrSrc
                );

                TStrBuf(const TStrBuf& strbSrc);
                TStrBuf(TStrBuf&& strbSrc);
                ~TStrBuf();

                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TStrBuf& operator=(const TStrBuf& strbSrc);
                TStrBuf& operator=(TStrBuf&& strbSrc);
                TStrBuf& operator=(const TStringView& strvSrc);
                TStrBuf& operator=(const TKrnlString& strvSrc);
                TStrBuf& operator=(TKrnlString&& strvSrc);

                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TVoid AdoptBuffer
                (
                            tCIDLib::TCh* const     pszToAdopt
                    , const tCIDLib::TCard4         c4Count
                );

                tCIDLib::TVoid Append
                (
                    const   tCIDLib::TCh* const     pszSrc
                    , const tCIDLib::TCard4         c4Count
                    , const tCIDLib::TCard4         c4Extra = 0
                );

                tCIDLib::TBoolean bIsEmpty() const noexcept
                {
                    return m_c4CurEnd == 0;
                }

                tCIDLib::TCard4 c4BufSz() const noexcept
                {
                    return m_c4BufSz;
                }

                tCIDLib::TCard4 c4CurEnd() const noexcept
                {
                    return m_c4CurEnd;
                }

                tCIDLib::TCard4 c4CurEnd(const tCIDLib::TCard4 c4ToSet)
                {
                    // We allow it to be on the current end
                    CheckIndex(CID_FILE, CID_LINE, c4ToSet, kCIDLib::True);
                    m_c4CurEnd = c4ToSet;
                    return m_c4CurEnd;
                }

                tCIDLib::TCh chAt(const tCIDLib::TCard4 c4At) const;

                tCIDLib::TCh chLast() const noexcept;

                [[nodiscard]] tCIDLib::TCard4 c4CalcBufDiff
                (
                    const   tCIDLib::TCh* const     pszUpper
                    , const tCIDLib::TCh* const     pszLower
                )   const;

                [[nodiscard]] tCIDLib::TCard4
                c4CalcBufDiff(const tCIDLib::TCh* const pszUpper) const noexcept
                {
                    return c4CalcBufDiff(pszUpper, m_pszBuffer);
                }

                tCIDLib::TVoid Clear() noexcept;

                tCIDLib::TVoid DecEnd
                (
                    const   tCIDLib::TCard4     c4DecBy
                );

                tCIDLib::TVoid DeleteLast() noexcept;

                //
                //  THIS MAY invalidate any previous value returned from pszBuffer()! This
                //  only expands, it never reduces the buffer size. It will optionally preserve
                //  any current contents.
                //
                tCIDLib::TVoid ExpandTo
                (
                    const   tCIDLib::TCard4     c4NewSize
                    , const tCIDLib::TBoolean   bPreserve
                );

                //
                //  If the CURRENT LENGTH plus the increment would exceed the buffer size, then
                //  expand to current end
                //
                tCIDLib::TVoid ExpandBy(const   tCIDLib::TCard4     c4Increment
                                        , const tCIDLib::TBoolean   bPreserve)
                {
                    ExpandTo(m_c4CurEnd + c4Increment, bPreserve);
                }

                tCIDLib::TVoid IncEnd
                (
                    const   tCIDLib::TCard4     c4IncBy
                );

                const tCIDLib::TCh* pszBuffer(const tCIDLib::TBoolean bNullTerm = kCIDLib::True) const noexcept
                {
                    if (bNullTerm)
                        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
                    return m_pszBuffer;
                }

                tCIDLib::TCh* pszBuffer(const tCIDLib::TBoolean bNullTerm = kCIDLib::True) noexcept
                {
                    if (bNullTerm)
                        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
                    return m_pszBuffer;
                }

                // For these two we allow them to get a pointer to the end, though we provide pszAtEnd below
                const tCIDLib::TCh* pszBufferAt(const   tCIDLib::TCard4     c4At
                                                , const tCIDLib::TBoolean   bNullTerm = kCIDLib::True) const
                {
                    // We allow it to be on the current end
                    CheckIndex(CID_FILE, CID_LINE, c4At, kCIDLib::True);
                    if (bNullTerm)
                        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
                    return &m_pszBuffer[c4At];
                }

                tCIDLib::TCh* pszBufferAt(  const   tCIDLib::TCard4     c4At
                                            , const tCIDLib::TBoolean   bNullTerm = kCIDLib::True)
                {
                    // We allow it to be on the current end
                    CheckIndex(CID_FILE, CID_LINE, c4At, kCIDLib::True);
                    if (bNullTerm)
                        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
                    return &m_pszBuffer[c4At];
                }

                const tCIDLib::TCh* pszAtEnd() const noexcept
                {
                    return &m_pszBuffer[m_c4CurEnd];
                }

                tCIDLib::TCh* pszAtEnd() noexcept
                {
                    return &m_pszBuffer[m_c4CurEnd];
                }

                tCIDLib::TVoid PutAt
                (
                    const   tCIDLib::TCard4     c4At
                    , const tCIDLib::TCh        chToPut
                );

                tCIDLib::TVoid ResetEnd() noexcept;

                tCIDLib::TVoid SetEnd
                (
                    const   tCIDLib::TCard4     c4At
                );

                tCIDLib::TVoid SetEnd
                (
                    const   tCIDLib::TCh* const pszAt
                );

                tCIDLib::TVoid Terminate();


            private :
                // -----------------------------------------------------------
                //  Private, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TVoid CheckIndex
                (
                    const   tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line
                    , const tCIDLib::TCard4     c4ToCheck
                    , const tCIDLib::TBoolean   bEndOk
                )   const;


                // -----------------------------------------------------------
                //  Private data members
                // -----------------------------------------------------------
                mutable tCIDLib::TCard4 m_c4BufSz = 0;
                tCIDLib::TCard4         m_c4CurEnd = 0;
                tCIDLib::TCh            m_szSmallBuf[c4SmallBufSz + 1];
                mutable tCIDLib::TCh*   m_pszBuffer = nullptr;
        };


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid BadHexChar
        (
            const   tCIDLib::TCh            chToXlat
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::TBoolean bCheckPrefix
        (
            const   tCIDLib::TCh* const     pszToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bCheckPrefix
        (
            const   TString&                strToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bCheckSuffix
        (
            const   tCIDLib::TCh* const     pszToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const noexcept;

        [[nodiscard]] tCIDLib::TBoolean bCheckSuffix
        (
            const   TString&                strToCheck
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const noexcept;

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

        tCIDLib::TVoid SetFromShort
        (
            const   tCIDLib::TSCh* const    pszSrc
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
        //  m_strbData
        //      Our internal string buffer.
        // -------------------------------------------------------------------
        TStrBuf     m_strbData;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TString,TObject)
        DefPolyDup(TString)
        NulObject(TString)
};


#pragma CIDLIB_POPPACK


// Pre-instantiate some really common variations on the concatenation method
extern template TString TString::strConcat(const TString&, const TString&);
extern template TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh*&);
extern template TString TString::strConcat(const TString&, const tCIDLib::TCh*&);
extern template TString TString::strConcat(const tCIDLib::TCh*&, const TString&);

extern template TString TString::strConcat(const TString&, const TString&, const TString&);
extern template TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh*&, const tCIDLib::TCh*&);

extern template TString TString::strConcat(const TString&, const tCIDLib::TCh&, const TString&);
extern template TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh&, const tCIDLib::TCh*&);
extern template TString TString::strConcat(const TString&, const tCIDLib::TCh&, const tCIDLib::TCh*&);
extern template TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh&, const TString&);

extern template TString TString::strConcat(const tCIDLib::TCh&, const tCIDLib::TCh*&);
extern template TString TString::strConcat(const tCIDLib::TCh&, const TString&);


// ---------------------------------------------------------------------------
//  Global operators
// ---------------------------------------------------------------------------
inline TString operator+(const TString& str1, const TString& str2)
{
    TString strRet(str1, str2.c4Length());
    strRet.Append(str2);
    return strRet;
}
