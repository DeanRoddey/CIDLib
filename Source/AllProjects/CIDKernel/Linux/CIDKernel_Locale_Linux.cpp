//
// FILE NAME: CIDKernel_Locale_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/30/1999
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
//  This file provides the Linux specific implementation the TKrnlLocale
//  namespace's functions.
//
// CAVEATS/GOTCHAS:
//
//  1)  This class only caches some highly used information. So the private
//      method LoadLocaleInfo() only loads up the cached stuff. The others
//      are queried from the system when requested.
//
// LOG:
//
//  $Log: CIDKernel_Locale_Linux.cpp,v $
//  Revision 1.2  2000/06/13 17:36:11  will_mason
//  Added Log keyword to header
//
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    <locale.h>


namespace
{
    // ---------------------------------------------------------------------------
    //  Local types
    //
    //  TLangXlat
    //      A small structure used to create a translation table to map between
    //      native language ids and CIDLib ids.
    // ---------------------------------------------------------------------------
    struct TLangXlat
    {
        tCIDLib::ELanguages eIndependent;
        const tCIDLib::TCh  szISO639Code[3];
        const tCIDLib::TCh* pszISO639Name;
    };

    // ---------------------------------------------------------------------------
    //  Local, static data
    //
    //  alxlTable
    //  c4LangXlatCount
    //      A table to translate the CIDLib languag ids into local platform
    //      language ids, and vice versa, and its size.
    //
    //  szThisLocale
    //      The string identifier for the set locale returned by setlocale()
    // ---------------------------------------------------------------------------
    TLangXlat alxlTable[] =
    {
            { tCIDLib::ELanguages::Afrikaans  , L"af", L"Afrikaans" }
        ,   { tCIDLib::ELanguages::Albanian   , L"sg", L"Albanian" }
        ,   { tCIDLib::ELanguages::Arabic     , L"ar", L"Arabic" }
        ,   { tCIDLib::ELanguages::Basque     , L"eu", L"Basque" }
        ,   { tCIDLib::ELanguages::Belarusian , L"be", L"Byelorussian" }
        ,   { tCIDLib::ELanguages::Bulgarian  , L"bg", L"Bulgarian" }
        ,   { tCIDLib::ELanguages::Catalan    , L"ca", L"Catalan" }
        ,   { tCIDLib::ELanguages::Chinese    , L"zh", L"Chinese" }
        ,   { tCIDLib::ELanguages::Croatian   , L"hr", L"Croatian" }
        ,   { tCIDLib::ELanguages::Czech      , L"cs", L"Czech" }
        ,   { tCIDLib::ELanguages::Danish     , L"da", L"Danish" }
        ,   { tCIDLib::ELanguages::Dutch      , L"nl", L"Dutch" }
        ,   { tCIDLib::ELanguages::English    , L"en", L"English" }
        /* ,   { tCIDLib::ELanguages::Farsi      , L"", L"" } */
        ,   { tCIDLib::ELanguages::Finnish    , L"fi", L"Finnish" }
        ,   { tCIDLib::ELanguages::French     , L"fr", L"French" }
        ,   { tCIDLib::ELanguages::German     , L"de", L"German" }
        ,   { tCIDLib::ELanguages::Greek      , L"el", L"Greek" }
        ,   { tCIDLib::ELanguages::Hebrew     , L"he", L"Hebrew" }
        ,   { tCIDLib::ELanguages::Hungarian  , L"hu", L"Hungarian" }
        ,   { tCIDLib::ELanguages::Icelandic  , L"is", L"Icelandic" }
        ,   { tCIDLib::ELanguages::Indonesian , L"id", L"Indonesian" }
        ,   { tCIDLib::ELanguages::Italian    , L"it", L"Italian" }
        ,   { tCIDLib::ELanguages::Japanese   , L"ja", L"Japanese" }
        ,   { tCIDLib::ELanguages::Korean     , L"ko", L"Korean" }
        ,   { tCIDLib::ELanguages::Latvian    , L"lv", L"Latvian, Lettish" }
        ,   { tCIDLib::ELanguages::Lithuanian , L"lt", L"Lithuanian" }
        ,   { tCIDLib::ELanguages::Norwegian  , L"no", L"Norwegian" }
        ,   { tCIDLib::ELanguages::Polish     , L"pl", L"Polish" }
        ,   { tCIDLib::ELanguages::Portuguese , L"pt", L"Portugese" }
        ,   { tCIDLib::ELanguages::Romanian   , L"ro", L"Romanian" }
        ,   { tCIDLib::ELanguages::Russian    , L"ru", L"Russian" }
        ,   { tCIDLib::ELanguages::Serbian    , L"sr", L"Serbian" }
        ,   { tCIDLib::ELanguages::Slovak     , L"sk", L"Slovak" }
        ,   { tCIDLib::ELanguages::Slovenian  , L"sl", L"Slovenian" }
        ,   { tCIDLib::ELanguages::Spanish    , L"es", L"Spanish" }
        ,   { tCIDLib::ELanguages::Swedish    , L"sv", L"Swedish" }
        ,   { tCIDLib::ELanguages::Turkish    , L"tr", L"Turkish" }
        ,   { tCIDLib::ELanguages::Ukrainian  , L"uk", L"Ukrainian" }
        ,   { tCIDLib::ELanguages::Vietnamese , L"vi", L"Vietnamese" }
    };
    tCIDLib::TCard4  c4LangXlatCount = tCIDLib::c4ArrayElems(alxlTable);
    tCIDLib::TZStr32 szThisLocale;


    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    tCIDLib::TVoid BuildMonetaryFmt(        tCIDLib::TBoolean   bPositive
                                    ,       tCIDLib::TCh* const pszToFill
                                    , const tCIDLib::TCard4     c4MaxChars)
    {
        static const tCIDLib::TCh* apszMonFmt[] =
        {
            L"%(v)%(y)"
            , L"%(y)%(v)"
            , L"%(v) %(y)"
            , L"%(y) %(v)"
        };

        static const tCIDLib::TCh* apszPrecedeCurrFmt[] =
        {
            L"%(v)%(s)%(y)"
            , L"%(s)%(y)%(v)"
            , L"%(v) %(s)%(y)"
            , L"%(s)%(y) %(v)"
        };

        static const tCIDLib::TCh* apszFollowCurrFmt[] =
        {
            L"%(v)%(y)%(s)"
            , L"%(y)%(s)%(v)"
            , L"%(v) %(y)%(s)"
            , L"%(y)%(s) %(v)"
        };

        nl_item nlCSPrecedes, nlSepBySpace, nlSignPos;
        tCIDLib::TSCh chSign;

        if (bPositive)
        {
            nlCSPrecedes = P_CS_PRECEDES;
            nlSepBySpace = P_SEP_BY_SPACE;
            nlSignPos = P_SIGN_POSN;
            chSign = *::nl_langinfo(POSITIVE_SIGN);
        }
        else
        {
            nlCSPrecedes = N_CS_PRECEDES;
            nlSepBySpace = N_SEP_BY_SPACE;
            nlSignPos = N_SIGN_POSN;
            chSign = *::nl_langinfo(NEGATIVE_SIGN);
        }

        tCIDLib::TCard4 c4Idx = *::nl_langinfo(nlCSPrecedes) ? 1 : 0;
        c4Idx |= *::nl_langinfo(nlSepBySpace) ? 2 : 0;

        tCIDLib::TCard4 c4PosVal = chSign ? *::nl_langinfo(nlSignPos) : 5;

        switch (c4PosVal)
        {
            case 0:
                TRawStr::CopyCatStr(pszToFill, c4MaxChars, L"(", apszMonFmt[c4Idx], L")");
                break;

            case 1:
                TRawStr::CopyCatStr(pszToFill, c4MaxChars, L"%(s)", apszMonFmt[c4Idx]);
                break;

            case 2:
                TRawStr::CopyCatStr(pszToFill, c4MaxChars, apszMonFmt[c4Idx], L"%(s)");
                break;

            case 3:
                TRawStr::CopyStr(pszToFill, apszPrecedeCurrFmt[c4Idx], c4MaxChars);
                break;

            case 4:
                TRawStr::CopyStr(pszToFill, apszFollowCurrFmt[c4Idx], c4MaxChars);
                break;

            case 5:
                TRawStr::CopyStr(pszToFill, apszMonFmt[c4Idx], c4MaxChars);
        }
    }

    tCIDLib::TVoid BuildCIDToken(const  tCIDLib::TCh chToken
                                , const tCIDLib::TCh* const pszWidth
                                , const tCIDLib::TCh chPad
                                ,       tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4 c4MaxChars)
    {
        if (*pszWidth != kCIDLib::chNull)
        {
            TRawStr::CopyCatStr(pszToFill, c4MaxChars, L"%( ,", pszWidth, L", )");
            pszToFill[2] = chToken;
            tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToFill);
            pszToFill[c4Len - 2] = chPad ? chPad : kCIDLib::chUnderscore;
        }
        else
        {
            TRawStr::CopyStr(pszToFill, L"%( )", c4MaxChars);
            pszToFill[2] = chToken;
        }
    }

    tCIDLib::TVoid ConvertDateTimeString(tCIDLib::TCh* const pszTarget
                                           , const tCIDLib::TCh* const pszSource
                                           , const tCIDLib::TCard4 c4MaxChars);

    tCIDLib::TCard4 ConvertAToken(const tCIDLib::TSCh* const pszToken
                                    , tCIDLib::TCh* const     pszOutput
                                    , const tCIDLib::TCard4   c4MaxChars)
    {
        *pszOutput = kCIDLib::chNull;

        if (*pszToken != '%')
            return 0;

        tCIDLib::TCh chPad = 0;
        tCIDLib::TZStr16 szWidth;
        szWidth[0] = kCIDLib::chNull;
        tCIDLib::TSCh chModifier = 0;
        tCIDLib::TZStr128 szBuf;
        tCIDLib::TBoolean bFoundTerminator = kCIDLib::False;

        tCIDLib::TSCh chCur;
        tCIDLib::TCard4 c4Idx;
        for (chCur = pszToken[1], c4Idx = 1; chCur; chCur = pszToken[++c4Idx])
        {
            if (chCur == '_'
                || chCur == '-'
                || chCur == '0')
            {
                ::mbtowc(&chPad, &chCur, 1);
                continue;
            }
            else if (isdigit(chCur))
            {
                tCIDLib::TCard4 c4Len = 1;

                while (isdigit(*(pszToken + c4Idx + c4Len)))
                    c4Len++;

                TRawStr::pszConvert(pszToken + c4Idx, szWidth, c4Len);

                c4Idx += c4Len - 1;
                continue;
            }
            else if (chCur == 'E'
                     || chCur == 'O')
            {
                chModifier = chCur;
                continue;
            }

            bFoundTerminator = kCIDLib::True;

            switch (chCur)
            {
                case '%':
                    TRawStr::CopyStr(pszOutput, L"%%", c4MaxChars);
                    break;

                case 'a':
                    TRawStr::CopyStr(pszOutput, L"%(a)", c4MaxChars);
                    break;

                case 'A':
                    TRawStr::CopyStr(pszOutput, L"%(d)", c4MaxChars);
                    break;

                case 'b':
                case 'h':
                    TRawStr::CopyStr(pszOutput, L"%(m)", c4MaxChars);
                    break;

                case 'B':
                    TRawStr::CopyStr(pszOutput, L"%(n)", c4MaxChars);
                    break;

                case 'c':
                    if (chModifier == 'E' && *(::nl_langinfo(ERA_D_T_FMT)) != 0)
                        TRawStr::pszConvert(::nl_langinfo(ERA_D_T_FMT), szBuf, c4MaxBufChars(szBuf));
                    else
                        TRawStr::pszConvert(::nl_langinfo(D_T_FMT), szBuf, c4MaxBufChars(szBuf));
                    ConvertDateTimeString(pszOutput, szBuf, c4MaxChars);
                    break;

                case 'd':
                    TRawStr::CopyStr(pszOutput, L"%(D,2,0)", c4MaxChars);
                    break;

                case 'D':
                    TRawStr::CopyStr(pszOutput, L"%(M,2,0)/%(D,2,0)/%(Y)", c4MaxChars);
                    break;

                case 'e':
                    BuildCIDToken(kCIDLib::chLatin_D, szWidth, chPad, pszOutput, c4MaxChars);
                    break;

                case 'H':
                    TRawStr::CopyStr(pszOutput, L"%(H,2,0)", c4MaxChars);
                    break;

                case 'I':
                    TRawStr::CopyStr(pszOutput, L"%(h,2,0)", c4MaxChars);
                    break;

                case 'k':
                    BuildCIDToken(kCIDLib::chLatin_H, szWidth, chPad, pszOutput, c4MaxChars);
                    break;

                case 'l':
                    BuildCIDToken(kCIDLib::chLatin_h, szWidth, chPad, pszOutput, c4MaxChars);
                    break;

                case 'm':
                    TRawStr::CopyStr(pszOutput, L"%(M,2,0)", c4MaxChars);
                    break;

                case 'M':
                    TRawStr::CopyStr(pszOutput, L"%(u,2,0)", c4MaxChars);
                    break;

                case 'p':
                case 'P':
                    TRawStr::CopyStr(pszOutput, L"%(P)", c4MaxChars);
                    break;

                case 'r':
                    TRawStr::CopyStr(pszOutput, L"%(h,2,0):%(u,2,0):%(s,2,0) %(P)", c4MaxChars);
                    break;

                case 'R':
                    TRawStr::CopyStr(pszOutput, L"%(H,2,0):%(u,2,0)", c4MaxChars);
                    break;

                case 'S':
                    TRawStr::CopyStr(pszOutput, L"%(s,2,0)", c4MaxChars);
                    break;

                case 'T':
                    TRawStr::CopyStr(pszOutput, L"%(H,2,0):%(u,2,0):%(s,2,0)", c4MaxChars);
                    break;

                case 'x':
                    if (chModifier == 'E' && *(::nl_langinfo(ERA_D_FMT)) != 0)
                        TRawStr::pszConvert(::nl_langinfo(ERA_D_FMT), szBuf, c4MaxBufChars(szBuf));
                    else
                        TRawStr::pszConvert(::nl_langinfo(D_FMT), szBuf, c4MaxBufChars(szBuf));
                    ConvertDateTimeString(pszOutput, szBuf, c4MaxChars);
                    break;

                case'X':
                    if (chModifier == 'E' && *(::nl_langinfo(ERA_T_FMT)) != 0)
                        TRawStr::pszConvert(::nl_langinfo(ERA_T_FMT), szBuf, c4MaxBufChars(szBuf));
                    else
                        TRawStr::pszConvert(::nl_langinfo(T_FMT), szBuf, c4MaxBufChars(szBuf));
                    ConvertDateTimeString(pszOutput, szBuf, c4MaxChars);
                    break;

                case 'y':
                    TRawStr::CopyStr(pszOutput, L"%(y)", c4MaxChars);
                    break;

                case 'Y':
                    TRawStr::CopyStr(pszOutput, L"%(Y)", c4MaxChars);
                    break;

                case 'Z':
                    TRawStr::CopyStr(pszOutput, L"%(T)", c4MaxChars);
                    break;

                default:
                    bFoundTerminator = kCIDLib::False;
                    break;
                }

                if (bFoundTerminator)
                    break;
        }

        return c4Idx + 1;
    }

    tCIDLib::TCard4 ConvertAToken(  const   tCIDLib::TCh* const pszToken
                                    ,       tCIDLib::TCh* const pszOutput
                                    , const tCIDLib::TCard4     c4MaxChars)
    {
        tCIDLib::TSCh* pszLocToken = TRawStr::pszConvert(pszToken);
        TArrayJanitor<tCIDLib::TSCh> janToken(pszLocToken);
        return ConvertAToken(pszLocToken, pszOutput, c4MaxChars);
    }


    tCIDLib::TVoid ConvertDateTimeString(       tCIDLib::TCh* const pszTarget
                                        , const tCIDLib::TCh* const pszSource
                                        , const tCIDLib::TCard4     c4MaxChars)
    {
        tCIDLib::TZStr128 szBuf;
        tCIDLib::TCard4 c4Len;
        pszTarget[0] = kCIDLib::chNull;
        tCIDLib::TCard4 c4CharsLeft = c4MaxChars;
        tCIDLib::TCh szOneChar[2] = L" ";

        for (tCIDLib::TCh* pszCur = const_cast<tCIDLib::TCh*>(pszSource); *pszCur; pszCur++)
        {
            c4Len = ConvertAToken(pszCur, szBuf, 128);

            if (c4Len)
            {
                TRawStr::CatStr(pszTarget, szBuf, c4CharsLeft);
                c4CharsLeft -= TRawStr::c4StrLen(szBuf);
                pszCur += c4Len - 1;
            }
            else
            {
                szOneChar[0] = *pszCur;
                TRawStr::CatStr(pszTarget, szOneChar, c4CharsLeft);
                c4CharsLeft--;
            }
        }
    }

    tCIDLib::TCh chFindSeparator(nl_item nlItem)
    {
        tCIDLib::TZStr128 szBuf;
        tCIDLib::TCard4 c4Len;

        for (tCIDLib::TSCh* pszCur = ::nl_langinfo(nlItem); *pszCur; pszCur++)
        {
            c4Len = ConvertAToken(pszCur, szBuf, 128);

            if (c4Len)
            {
                if (!::strncmp(pszCur, "%r", c4Len)
                ||  !::strncmp(pszCur, "%R", c4Len)
                ||  !::strncmp(pszCur, "%T", c4Len))
                {
                    return kCIDLib::chColon;
                }

                pszCur += c4Len - 1;
            }
            else
            {
                tCIDLib::TCh chReturn;

                if (!isspace(*pszCur))
                {
                    ::mbtowc(&chReturn, pszCur, 1);
                    return chReturn;
                }
            }
        }

        return kCIDLib::chSpace;
    }
}



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDKrnlModule::InitTermLocale(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        TRawStr::pszConvert(::setlocale(LC_ALL, "")
                            , szThisLocale
                            , c4MaxBufChars(szThisLocale));

        if (!TKrnlLocale::bLoadLocaleInfo())
            throw TKrnlError::kerrLast();
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlLocale
//  PREFIX: kloc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlLocale: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlLocale::bQueryStrField(const   EStrFlds            eField
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    switch(eField)
    {
        case EStrFlds::Country:
        case EStrFlds::Language:
        case EStrFlds::ISOCountry:
        case EStrFlds::EnglishCountry:
            pszToFill[0] = L'\000';
            break;

        case EStrFlds::EnglishLanguage:
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4LangXlatCount; c4Index++)
            {
                if (alxlTable[c4Index].eIndependent == s_eLanguage)
                {
                    TRawStr::CopyStr(pszToFill, alxlTable[c4Index].pszISO639Name, c4MaxChars);
                    break;
                }
            }
            break;

        case EStrFlds::ISOLanguage:
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4LangXlatCount; c4Index++)
            {
                if (alxlTable[c4Index].eIndependent == s_eLanguage)
                {
                    TRawStr::CopyStr(pszToFill, alxlTable[c4Index].szISO639Code, c4MaxChars);
                    break;
                }
            }
            break;

        // These are already loaded and translated
        case EStrFlds::AMString :
            TRawStr::CopyStr(pszToFill, s_szAMString, c4MaxChars);
            break;

        case EStrFlds::PMString :
            TRawStr::CopyStr(pszToFill, s_szPMString, c4MaxChars);
            break;

        case EStrFlds::TimeFormat :
            TRawStr::CopyStr(pszToFill, s_szTimeFmt, c4MaxChars);
            break;

        case EStrFlds::DateFormat :
            TRawStr::CopyStr(pszToFill, s_szDateFmt, c4MaxChars);
            break;

        case EStrFlds::NegMonFormat :
            TRawStr::CopyStr(pszToFill, s_szNegMonFmt, c4MaxChars);
            break;

        case EStrFlds::PosMonFormat :
            TRawStr::CopyStr(pszToFill, s_szPosMonFmt, c4MaxChars);
            break;

        default :
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_UnknownStrField);
            return kCIDLib::False;
    };

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlLocale::bQueryShortDay(const   tCIDLib::EWeekDays  eDay
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    TRawStr::pszConvert
    (
        ::nl_langinfo(nl_item(ABDAY_1 + tCIDLib::c4EnumOrd(eDay))), pszToFill, c4MaxChars
    );
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlLocale::bQueryLongDay( const   tCIDLib::EWeekDays  eDay
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    TRawStr::pszConvert
    (
        ::nl_langinfo(nl_item(DAY_1 + tCIDLib::c4EnumOrd(eDay))), pszToFill, c4MaxChars
    );
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlLocale::bQueryShortMonth(  const   tCIDLib::EMonths    eMonth
                                ,       tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    TRawStr::pszConvert
    (
        ::nl_langinfo(nl_item(ABMON_1 + tCIDLib::c4EnumOrd(eMonth))), pszToFill, c4MaxChars
    );
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlLocale::bQueryLongMonth(const  tCIDLib::EMonths    eMonth
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    TRawStr::pszConvert
    (
        ::nl_langinfo(nl_item(MON_1 + tCIDLib::c4EnumOrd(eMonth))), pszToFill, c4MaxChars
    );
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlLocale: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlLocale::bLoadLocaleInfo()
{
    // We have to guess at the measure type, because Dean said so. :)
    if ((TRawStr::eCompareStr(szThisLocale, L"C") == tCIDLib::ESortComps::Equal)
    ||  TRawStr::pszFindChars(szThisLocale, L"US") != 0)
    {
        s_eMeasure = tCIDLib::EMeasures::US;
    }
    else
    {
        s_eMeasure = tCIDLib::EMeasures::Metric;
    }

    // Find the language from the locale identifier string
    s_eLanguage = tCIDLib::ELanguages::English;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4LangXlatCount; c4Index++)
    {
        if (TRawStr::eCompareStrN(alxlTable[c4Index].szISO639Code, szThisLocale, 2) == tCIDLib::ESortComps::Equal)
            s_eLanguage = alxlTable[c4Index].eIndependent;
    }
    s_eDefLanguage = s_eLanguage;

    // Now extract the date and time separator characters
    s_chDateSep = chFindSeparator(D_FMT);
    s_chTimeSep = chFindSeparator(T_FMT);

    // AM and PM are easy
    TRawStr::pszConvert(::nl_langinfo(AM_STR), s_szAMString, c4MaxBufChars(s_szAMString));
    TRawStr::pszConvert(::nl_langinfo(PM_STR), s_szPMString, c4MaxBufChars(s_szPMString));

    // Numeric
    tCIDLib::TCard4 c4Tmp = *::nl_langinfo(FRAC_DIGITS);
    s_c4DecDigits = c4Tmp == 127 ? 0 : c4Tmp;
    c4Tmp = *::nl_langinfo(GROUPING);
    s_c4GroupSize = c4Tmp == 127 ? 0 : c4Tmp;
    ::mbtowc(&s_chGroupSep, ::nl_langinfo(THOUSANDS_SEP), 1);
    ::mbtowc(&s_chPosSign, ::nl_langinfo(POSITIVE_SIGN), 1);
    ::mbtowc(&s_chNegSign, ::nl_langinfo(NEGATIVE_SIGN), 1);
    ::mbtowc(&s_chDecSymbol, ::nl_langinfo(DECIMAL_POINT), 1);

    // Monetary
    s_c4MonetaryDecDigits = s_c4DecDigits;
    c4Tmp = *::nl_langinfo(MON_GROUPING);
    s_c4MonetaryGrpSize = c4Tmp == 127 ? 0 : c4Tmp;
    ::mbtowc(&s_chMonetaryDecSymbol, ::nl_langinfo(MON_DECIMAL_POINT), 1);
    ::mbtowc(&s_chMonetaryGrpSep, ::nl_langinfo(MON_THOUSANDS_SEP), 1);
    ::mbtowc(&s_chCurrencySymbol, ::nl_langinfo(CURRENCY_SYMBOL), 1);

    BuildMonetaryFmt(kCIDLib::True, s_szPosMonFmt, c4MaxBufChars(s_szPosMonFmt));
    BuildMonetaryFmt(kCIDLib::False, s_szNegMonFmt, c4MaxBufChars(s_szNegMonFmt));

    tCIDLib::TZStr128 szBuf;
    TRawStr::pszConvert(::nl_langinfo(D_FMT), szBuf, c4MaxBufChars(szBuf));
    ConvertDateTimeString(s_szDateFmt, szBuf, c4MaxBufChars(s_szDateFmt));
    TRawStr::pszConvert(::nl_langinfo(T_FMT), szBuf, c4MaxBufChars(szBuf));
    ConvertDateTimeString(s_szTimeFmt, szBuf, c4MaxBufChars(s_szTimeFmt));

    return kCIDLib::True;
}
