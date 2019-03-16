//
// FILE NAME: CIDLib_Locale.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This is the header for the CIDLib_Locale.Cpp file. This file implements
//  the TLocale class, which provides access to locale information needed to
//  adapt programs to local conventions. Only a single locale is supported
//  per-process, so its implemented as a namespace of static methods.
//
//  For the methods that return fundamental values, these are just inlined
//  passthroughs to the underlying kernel locale driver. For the ones that
//  return strings, this namespace's out of line implementation does some
//  fancy work to fault in those strings as they are used.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  NAMESPACE: TLocale
// ---------------------------------------------------------------------------
namespace TLocale
{
    inline tCIDLib::TBoolean b24Hour()
    {
        return TKrnlLocale::b24Hour();
    }

    inline tCIDLib::TCard4 c4DecimalDigits()
    {
        return TKrnlLocale::c4DecimalDigits();
    }

    inline tCIDLib::TCard4 c4GroupSize()
    {
        return TKrnlLocale::c4GroupSize();
    }

    inline tCIDLib::TCard4 c4MonetaryDecimalDigits()
    {
        return TKrnlLocale::c4MonetaryDecimalDigits();
    }

    inline tCIDLib::TCard4 c4MonetaryGroupSize()
    {
        return TKrnlLocale::c4MonetaryGroupSize();
    }

    inline tCIDLib::TCh chCurrencySymbol()
    {
        return TKrnlLocale::chCurrencySymbol();
    }

    inline tCIDLib::TCh chDateSeparator()
    {
        return TKrnlLocale::chDateSeparator();
    }

    inline tCIDLib::TCh chDecimalSymbol()
    {
        return TKrnlLocale::chDecimalSymbol();
    }

    inline tCIDLib::TCh chGroupSeparator()
    {
        return TKrnlLocale::chGroupSeparator();
    }

    inline tCIDLib::TCh chPositiveSign()
    {
        return TKrnlLocale::chPositiveSign();
    }

    inline tCIDLib::TCh chMonetaryDecimalSymbol()
    {
        return TKrnlLocale::chMonetaryDecimalSymbol();
    }

    inline tCIDLib::TCh chMonetaryGroupSeparator()
    {
        return TKrnlLocale::chMonetaryGroupSeparator();
    }

    inline tCIDLib::TCh chNegativeSign()
    {
        return TKrnlLocale::chNegativeSign();
    }

    inline tCIDLib::TCh chTimeSeparator()
    {
        return TKrnlLocale::chTimeSeparator();
    }

    inline tCIDLib::ELanguages eDefLanguage()
    {
        return TKrnlLocale::eDefLanguage();
    }

    inline tCIDLib::ELanguages eLanguage()
    {
        return TKrnlLocale::eLanguage();
    }

    inline tCIDLib::EMeasures eMeasure()
    {
        return TKrnlLocale::eMeasure();
    }

    inline tCIDLib::ELanguages eXlatLangSuffix(const TString& strSuffix)
    {
        return TKrnlLocale::eXlatLangSuffix(strSuffix.pszBuffer());
    }

    CIDLIBEXP tCIDLib::EWeekDays eXlatLocDay
    (
        const   TString&                    strToXlat
    );

    CIDLIBEXP tCIDLib::EMonths eXlatLocMonth
    (
        const   TString&                    strToXlat
    );

    CIDLIBEXP const TString& strAMString();

    CIDLIBEXP const TString& strCountry();

    CIDLIBEXP const TString& strDateFormat();

    CIDLIBEXP const TString& strDay
    (
        const   tCIDLib::EWeekDays          eDay
    );

    CIDLIBEXP const TString& strDayAbbrev
    (
        const   tCIDLib::EWeekDays          eDay
    );

    CIDLIBEXP const TString& strDefLanguageSuffix();

    CIDLIBEXP const TString& strEnglishCountry();

    CIDLIBEXP const TString& strEnglishLanguage();

    CIDLIBEXP const TString& strISOCountry();

    CIDLIBEXP const TString& strISOLanguage();

    CIDLIBEXP const TString& strLanguage();

    CIDLIBEXP const TString& strLanguageSuffix();

    CIDLIBEXP TString strLanguageSuffixFor
    (
        const   tCIDLib::ELanguages         eLangFor
    );

    CIDLIBEXP const TString& strMonth
    (
        const   tCIDLib::EMonths            eMonth
    );

    CIDLIBEXP const TString& strMonthAbbrev
    (
        const   tCIDLib::EMonths            eMonth
    );

    CIDLIBEXP const TString& strNegMonFormat();

    CIDLIBEXP const TString& strPosMonFormat();

    CIDLIBEXP const TString& strPMString();

    CIDLIBEXP const TString& strTimeFormat();
}

#pragma CIDLIB_POPPACK

