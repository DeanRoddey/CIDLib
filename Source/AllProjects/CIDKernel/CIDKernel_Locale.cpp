//
// FILE NAME: CIDKernel_Locale.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/1996
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
//  This file implements the platform independent parts of the locale
//  support class, TKrnlLocale.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static const tCIDLib::TCh* apszMapLanguageId(const tCIDLib::ELanguages eLangToMap)
{
    //
    //  If debugging, make sure that our list has the same number of elements
    //  as the language enum.
    //
    #if CID_DEBUG_ON
    tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(kCIDLib::apszLangSuffixes);
    if (c4Count != tCIDLib::c4EnumOrd(tCIDLib::ELanguages::Count))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcLoc_BadSuffixList);
        return nullptr;
    }
    #endif

    return kCIDLib::apszLangSuffixes[tCIDLib::c4EnumOrd(eLangToMap)];
}


// ---------------------------------------------------------------------------
//  TKrnlLocale: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlLocale::bDefLangSuffix(const tCIDLib::TCh*& pszToFill)
{
    pszToFill = apszMapLanguageId(s_eDefLanguage);
    return (pszToFill != nullptr);
}


tCIDLib::TBoolean
TKrnlLocale::bLangSuffix(const tCIDLib::TCh*& pszToFill)
{
    pszToFill = apszMapLanguageId(s_eDefLanguage);
    return (pszToFill != nullptr);
}

tCIDLib::TBoolean
TKrnlLocale::bLangSuffixFor(const   tCIDLib::ELanguages eLangFor
                            , const tCIDLib::TCh*&      pszToFill)
{
    pszToFill = apszMapLanguageId(eLangFor);
    return (pszToFill != nullptr);
}

tCIDLib::ELanguages
TKrnlLocale::eXlatLangSuffix(const tCIDLib::TCh* const pszSuffix)
{
    // Search the map for an entry with this
    for (tCIDLib::TCard4 c4Index = 0;
            c4Index < tCIDLib::c4EnumOrd(tCIDLib::ELanguages::Count); c4Index++)
    {
        if (TRawStr::bCompareStr(pszSuffix, kCIDLib::apszLangSuffixes[c4Index]))
            return tCIDLib::ELanguages(c4Index);
    }
    return tCIDLib::ELanguages::Count;
}



// ---------------------------------------------------------------------------
//  TKrnlLocale: Static Data Members
// ---------------------------------------------------------------------------
tCIDLib::TBoolean    TKrnlLocale::s_b24Hour;
tCIDLib::ELanguages  TKrnlLocale::s_eDefLanguage;
tCIDLib::ELanguages  TKrnlLocale::s_eLanguage;
tCIDLib::EMeasures   TKrnlLocale::s_eMeasure;
tCIDLib::TCard4      TKrnlLocale::s_c4DecDigits;
tCIDLib::TCard4      TKrnlLocale::s_c4GroupSize;
tCIDLib::TCh         TKrnlLocale::s_chDecSymbol;
tCIDLib::TCh         TKrnlLocale::s_chGroupSep;
tCIDLib::TCh         TKrnlLocale::s_chPosSign;
tCIDLib::TCh         TKrnlLocale::s_chNegSign;
tCIDLib::TCard4      TKrnlLocale::s_c4MonetaryDecDigits;
tCIDLib::TCard4      TKrnlLocale::s_c4MonetaryGrpSize;
tCIDLib::TCh         TKrnlLocale::s_chCurrencySymbol;
tCIDLib::TCh         TKrnlLocale::s_chMonetaryDecSymbol;
tCIDLib::TCh         TKrnlLocale::s_chMonetaryGrpSep;
tCIDLib::TZStr32     TKrnlLocale::s_szNegMonFmt;
tCIDLib::TZStr32     TKrnlLocale::s_szPosMonFmt;
tCIDLib::TCh         TKrnlLocale::s_chDateSep;
tCIDLib::TCh         TKrnlLocale::s_chTimeSep;
tCIDLib::TZStr32     TKrnlLocale::s_szDateFmt;
tCIDLib::TZStr32     TKrnlLocale::s_szTimeFmt;
tCIDLib::TZStr32     TKrnlLocale::s_szAMString;
tCIDLib::TZStr32     TKrnlLocale::s_szPMString;


// ---------------------------------------------------------------------------
//  TKrnlLocale: Hidden default constructor and destructor
// ---------------------------------------------------------------------------
TKrnlLocale::TKrnlLocale()
{
}

TKrnlLocale::~TKrnlLocale()
{
}


// ---------------------------------------------------------------------------
//  TKrnlLocale:: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlLocale::b24Hour()
{
    return s_b24Hour;
}


tCIDLib::TCard4 TKrnlLocale::c4DecimalDigits()
{
    return s_c4DecDigits;
}

tCIDLib::TCard4 TKrnlLocale::c4GroupSize()
{
    return s_c4GroupSize;
}

tCIDLib::TCard4 TKrnlLocale::c4MonetaryDecimalDigits()
{
    return s_c4MonetaryDecDigits;
}

tCIDLib::TCard4 TKrnlLocale::c4MonetaryGroupSize()
{
    return s_c4MonetaryGrpSize;
}

tCIDLib::TCh TKrnlLocale::chCurrencySymbol()
{
    return s_chCurrencySymbol;
}

tCIDLib::TCh TKrnlLocale::chDateSeparator()
{
    return s_chDateSep;
}

tCIDLib::TCh TKrnlLocale::chDecimalSymbol()
{
    return s_chDecSymbol;
}

tCIDLib::TCh TKrnlLocale::chGroupSeparator()
{
    return s_chGroupSep;
}

tCIDLib::TCh TKrnlLocale::chPositiveSign()
{
    return s_chPosSign;
}

tCIDLib::TCh TKrnlLocale::chMonetaryDecimalSymbol()
{
    return s_chMonetaryDecSymbol;
}

tCIDLib::TCh TKrnlLocale::chMonetaryGroupSeparator()
{
    return s_chMonetaryGrpSep;
}

tCIDLib::TCh TKrnlLocale::chNegativeSign()
{
    return s_chNegSign;
}

tCIDLib::TCh TKrnlLocale::chTimeSeparator()
{
    return s_chTimeSep;
}

tCIDLib::ELanguages TKrnlLocale::eDefLanguage()
{
    return s_eDefLanguage;
}

tCIDLib::ELanguages TKrnlLocale::eLanguage()
{
    return s_eLanguage;
}

tCIDLib::EMeasures TKrnlLocale::eMeasure()
{
    return s_eMeasure;
}


