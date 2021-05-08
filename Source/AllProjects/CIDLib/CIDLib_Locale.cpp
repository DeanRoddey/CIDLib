//
// FILE NAME: CIDLib_Locale.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This file implements TLocale namespace.
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
#include    "CIDLib_.hpp"


namespace CIDLib_Locale
{
    namespace
    {
        // ---------------------------------------------------------------------------
        //  Local types
        //
        //  ECacheStrs
        //      This is the list of fields that are in the cache of strings. These
        //      are used to index the astrCache array below.
        // ---------------------------------------------------------------------------
        enum class ECacheStrs
        {
            AMString
            , Country
            , DateFormat
            , Sunday
            , Monday
            , Tuesday
            , Wednesday
            , Thursday
            , Friday
            , Saturday
            , ASunday
            , AMonday
            , ATuesday
            , AWednesday
            , AThursday
            , AFriday
            , ASaturday
            , DefLanguageSuffix
            , EnglishCountry
            , EnglishLanguage
            , ISOCountry
            , ISOLanguage
            , Language
            , LanguageSuffix
            , January
            , February
            , March
            , April
            , May
            , June
            , July
            , August
            , September
            , October
            , November
            , December
            , AJanuary
            , AFebruary
            , AMarch
            , AApril
            , AMay
            , AJune
            , AJuly
            , AAugust
            , ASeptember
            , AOctober
            , ANovember
            , ADecember
            , NegMonFormat
            , PosMonFormat
            , PMString
            , TimeFormat

            , Count
        };

        // -----------------------------------------------------------------------
        //  Local data
        //
        //  astrCache
        //      This is an array of string object pointers which serves as a cache
        //      for locale info. The TKrnlLocale class provides raw strings, but
        //      we provide string objects. And we don't want to create a string
        //      object every time they call. So we fault in a string object when
        //      a field is requested and store the results here.
        // -----------------------------------------------------------------------
        TEArray<TString*, ECacheStrs, CIDLib_Locale::ECacheStrs::Count> apstrCache
        (
            static_cast<TString*>(nullptr)
        );
    }
}



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method just makes it more convenient to load a string field from
//  the kernel locale class.
//
static tCIDLib::TVoid LoadStr(          tCIDLib::TCh* const     pszToFill
                                , const tCIDLib::TCard4         c4MaxChars
                                , const TKrnlLocale::EStrFlds   eField)
{
    if (!TKrnlLocale::bQueryStrField(eField, pszToFill, c4MaxChars))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_CantQuery
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


//
//  This method handles the lazy creation of the critical section that is
//  used to syncrhonize access to the locale cache.
//
static TCriticalSection* pcrsSync()
{
    static TCriticalSection crsLocSync;
    return &crsLocSync;
}



// ---------------------------------------------------------------------------
//   CLASS: TLocale
//  PREFIX: loc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLocale: Public, static methods
// ---------------------------------------------------------------------------

//
//  Translate a localized day name into a day of the week enum. We check for
//  the full and abrreviated versions.
//
tCIDLib::EWeekDays TLocale::eXlatLocDay(const TString& strToXlat)
{
    tCIDLib::EWeekDays eDay = tCIDLib::EWeekDays::Min;
    while (eDay < tCIDLib::EWeekDays::Count)
    {
        if (strDay(eDay).bCompareI(strToXlat))
            break;
        if (strDayAbbrev(eDay).bCompareI(strToXlat))
            break;
        eDay++;
    }
    return eDay;
}


//
//  Translate a localized month name to a month enum. We check for the full
//  and abbreviated versions.
//
tCIDLib::EMonths TLocale::eXlatLocMonth(const TString& strToXlat)
{
    tCIDLib::EMonths eMonth = tCIDLib::EMonths::Min;
    while (eMonth < tCIDLib::EMonths::Count)
    {
        if (strMonth(eMonth).bCompareI(strToXlat))
            break;
        if (strMonthAbbrev(eMonth).bCompareI(strToXlat))
            break;
        eMonth++;
    }
    return eMonth;
}


// Return the various local strings, faulting in as required
const TString& TLocale::strAMString()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::AMString])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::AMString])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::AMString);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::AMString] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::AMString];
}

const TString& TLocale::strCountry()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::Country])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        tCIDLib::TZStr128   szTmp;
        LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::Country);
        CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::Country] = new TString(szTmp);
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::Country];
}


const TString& TLocale::strDateFormat()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DateFormat])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DateFormat])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::DateFormat);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DateFormat] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DateFormat];
}


const TString& TLocale::strDay(const tCIDLib::EWeekDays eDay)
{
    // Have we cached this day yet? If not, then do so
    const CIDLib_Locale::ECacheStrs eStr = CIDLib_Locale::ECacheStrs
    (
        tCIDLib::c4EnumOrd(CIDLib_Locale::ECacheStrs::Sunday) + tCIDLib::c4EnumOrd(eDay)
    );

    if (!CIDLib_Locale::apstrCache[eStr])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[eStr])
        {
            tCIDLib::TZStr128 szTmp;
            if (!TKrnlLocale::bQueryLongDay(eDay, szTmp, tCIDLib::c4MaxBufChars(szTmp)))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcLoc_CantQuery
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            CIDLib_Locale::apstrCache[eStr] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[eStr];
}

const TString& TLocale::strDayAbbrev(const tCIDLib::EWeekDays eDay)
{
    // Have we cached this day yet? If not, then do so
    const CIDLib_Locale::ECacheStrs eStr = CIDLib_Locale::ECacheStrs
    (
        tCIDLib::c4EnumOrd(CIDLib_Locale::ECacheStrs::ASunday) + tCIDLib::c4EnumOrd(eDay)
    );

    if (!CIDLib_Locale::apstrCache[eStr])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[eStr])
        {
            tCIDLib::TZStr128 szTmp;
            if (!TKrnlLocale::bQueryShortDay(eDay, szTmp, tCIDLib::c4MaxBufChars(szTmp)))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcLoc_CantQuery
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            CIDLib_Locale::apstrCache[eStr] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[eStr];
}


const TString& TLocale::strDefLanguageSuffix()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DefLanguageSuffix])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DefLanguageSuffix])
        {
            const tCIDLib::TCh* pszSuffix;
            if (!TKrnlLocale::bDefLangSuffix(pszSuffix))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcLoc_CantQuery
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DefLanguageSuffix] = new TString(pszSuffix);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::DefLanguageSuffix];
}


const TString& TLocale::strEnglishCountry()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishCountry])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishCountry])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::EnglishCountry);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishCountry] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishCountry];
}


const TString& TLocale::strEnglishLanguage()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishLanguage])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishLanguage])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::EnglishLanguage);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishLanguage] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::EnglishLanguage];
}


const TString& TLocale::strLanguage()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::Language])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::Language])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::Language);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::Language] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::Language];
}


const TString& TLocale::strLanguageSuffix()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::LanguageSuffix])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::LanguageSuffix])
        {
            const tCIDLib::TCh* pszSuffix;
            if (!TKrnlLocale::bLangSuffix(pszSuffix))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcLoc_CantQuery
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::LanguageSuffix] = new TString(pszSuffix);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::LanguageSuffix];
}


TString TLocale::strLanguageSuffixFor(const tCIDLib::ELanguages eLangFor)
{
    const tCIDLib::TCh* pszSuffix;
    if (!TKrnlLocale::bLangSuffixFor(eLangFor, pszSuffix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_CantQuery
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return TString(pszSuffix);
}

const TString& TLocale::strISOCountry()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOCountry])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOCountry])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::ISOCountry);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOCountry] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOCountry];
}


const TString& TLocale::strISOLanguage()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOLanguage])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOLanguage])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::ISOLanguage);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOLanguage] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::ISOLanguage];
}


const TString& TLocale::strNegMonFormat()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::NegMonFormat])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::NegMonFormat])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::NegMonFormat);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::NegMonFormat] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::NegMonFormat];
}


const TString& TLocale::strPosMonFormat()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PosMonFormat])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PosMonFormat])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::PosMonFormat);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PosMonFormat] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PosMonFormat];
}


const TString& TLocale::strPMString()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PMString])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PMString])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::PMString);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PMString] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::PMString];
}


const TString& TLocale::strMonth(const tCIDLib::EMonths eMonth)
{
    // Have we cached this month yet? If not, then do so
    const CIDLib_Locale::ECacheStrs eStr = CIDLib_Locale::ECacheStrs
    (
        tCIDLib::c4EnumOrd(CIDLib_Locale::ECacheStrs::January) + tCIDLib::c4EnumOrd(eMonth)
    );

    if (!CIDLib_Locale::apstrCache[eStr])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[eStr])
        {
            tCIDLib::TZStr128 szTmp;
            if (!TKrnlLocale::bQueryLongMonth(eMonth, szTmp, tCIDLib::c4MaxBufChars(szTmp)))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcLoc_CantQuery
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            CIDLib_Locale::apstrCache[eStr] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[eStr];
}

const TString& TLocale::strMonthAbbrev(const tCIDLib::EMonths eMonth)
{
    // Have we cached this month yet? If not, then do so
    const CIDLib_Locale::ECacheStrs eStr = CIDLib_Locale::ECacheStrs
    (
        tCIDLib::c4EnumOrd(CIDLib_Locale::ECacheStrs::AJanuary) + tCIDLib::c4EnumOrd(eMonth)
    );

    if (!CIDLib_Locale::apstrCache[eStr])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[eStr])
        {
            tCIDLib::TZStr128 szTmp;
            if (!TKrnlLocale::bQueryShortMonth(eMonth, szTmp, tCIDLib::c4MaxBufChars(szTmp)))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcLoc_CantQuery
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            CIDLib_Locale::apstrCache[eStr] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[eStr];
}


const TString& TLocale::strTimeFormat()
{
    // Is this string cached up already? If not, then cache it
    if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::TimeFormat])
    {
        // Protected the cache while doing this
        TCritSecLocker lockCache(pcrsSync());

        if (!CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::TimeFormat])
        {
            tCIDLib::TZStr128   szTmp;
            LoadStr(szTmp, tCIDLib::c4MaxBufChars(szTmp), TKrnlLocale::EStrFlds::TimeFormat);
            CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::TimeFormat] = new TString(szTmp);
        }
    }
    return *CIDLib_Locale::apstrCache[CIDLib_Locale::ECacheStrs::TimeFormat];
}
