//
// FILE NAME: CIDKernel_Locale_Win32.Cpp
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
//  This file provides the Win32 specific implementation the TKrnlLocale
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
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  Local types
//
//  EDTStates
//      The states for the little state machine that parses the date/time
//      format string.
//
//  TLangXlat
//      A small structure used to create a translation table to map between
//      native language ids and CIDLib ids.
// ---------------------------------------------------------------------------
enum EDTStates
{
    EDTState_Whitespace
    , EDTState_InQuote
    , EDTState_InToken
};

struct TLangXlat
{
    tCIDLib::ELanguages eIndependent;
    tCIDLib::TCard4     c4Native;
};


namespace CIDKernel_Locale_Win32
{
    namespace
    {
        // ---------------------------------------------------------------------------
        //  Local, static data
        //
        //  alxTable
        //  c4LangXlatCount
        //      A table to translate the CIDLib languag ids into local platform
        //      language ids, and vice versa, and its size.
        //
        //  lcidUser
        //      The locale id of the user locale that we load up info about. Its
        //      set up during module init.
        // ---------------------------------------------------------------------------
        static TLangXlat alxlTable[] =
        {
                { tCIDLib::ELanguages::Afrikaans  , 0x36 }
            ,   { tCIDLib::ELanguages::Albanian   , 0x1c }
            ,   { tCIDLib::ELanguages::Arabic     , 0x01 }
            ,   { tCIDLib::ELanguages::Basque     , 0x2d }
            ,   { tCIDLib::ELanguages::Belarusian , 0x23 }
            ,   { tCIDLib::ELanguages::Bulgarian  , 0x02 }
            ,   { tCIDLib::ELanguages::Catalan    , 0x03 }
            ,   { tCIDLib::ELanguages::Chinese    , 0x04 }
            ,   { tCIDLib::ELanguages::Croatian   , 0x1a }
            ,   { tCIDLib::ELanguages::Czech      , 0x05 }
            ,   { tCIDLib::ELanguages::Danish     , 0x06 }
            ,   { tCIDLib::ELanguages::Dutch      , 0x13 }
            ,   { tCIDLib::ELanguages::English    , 0x09 }
            ,   { tCIDLib::ELanguages::Farsi      , 0x29 }
            ,   { tCIDLib::ELanguages::Finnish    , 0x0b }
            ,   { tCIDLib::ELanguages::French     , 0x0c }
            ,   { tCIDLib::ELanguages::German     , 0x07 }
            ,   { tCIDLib::ELanguages::Greek      , 0x08 }
            ,   { tCIDLib::ELanguages::Hebrew     , 0x0d }
            ,   { tCIDLib::ELanguages::Hungarian  , 0x0e }
            ,   { tCIDLib::ELanguages::Icelandic  , 0x0f }
            ,   { tCIDLib::ELanguages::Indonesian , 0x21 }
            ,   { tCIDLib::ELanguages::Italian    , 0x10 }
            ,   { tCIDLib::ELanguages::Japanese   , 0x11 }
            ,   { tCIDLib::ELanguages::Korean     , 0x12 }
            ,   { tCIDLib::ELanguages::Latvian    , 0x26 }
            ,   { tCIDLib::ELanguages::Lithuanian , 0x27 }
            ,   { tCIDLib::ELanguages::Norwegian  , 0x14 }
            ,   { tCIDLib::ELanguages::Polish     , 0x15 }
            ,   { tCIDLib::ELanguages::Portuguese , 0x16 }
            ,   { tCIDLib::ELanguages::Romanian   , 0x18 }
            ,   { tCIDLib::ELanguages::Russian    , 0x19 }
            ,   { tCIDLib::ELanguages::Serbian    , 0x1a }
            ,   { tCIDLib::ELanguages::Slovak     , 0x1b }
            ,   { tCIDLib::ELanguages::Slovenian  , 0x24 }
            ,   { tCIDLib::ELanguages::Spanish    , 0x0a }
            ,   { tCIDLib::ELanguages::Swedish    , 0x1d }
            ,   { tCIDLib::ELanguages::Turkish    , 0x1f }
            ,   { tCIDLib::ELanguages::Ukrainian  , 0x22 }
            ,   { tCIDLib::ELanguages::Vietnamese , 0x2a }
        };
        static tCIDLib::TCard4  c4LangXlatCount = tCIDLib::c4ArrayElems(alxlTable);
        static LCID             lcidUser;
    }
}



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TCard4 c4GetDecValue(const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TBoolean           bValid;
    constexpr tCIDLib::TCard4   c4TmpChars = 128;
    tCIDLib::TCh                szTmp[c4TmpChars + 1];

    if (!::GetLocaleInfo(CIDKernel_Locale_Win32::lcidUser, c4Index, szTmp, c4TmpChars))
        TKrnlError::ThrowHostError(::GetLastError());

    tCIDLib::TCard4 c4Ret = TRawStr::c4AsBinary(szTmp, bValid, tCIDLib::ERadices::Dec);
    if (!bValid)
        TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_ConvertValue);

    return c4Ret;
}


static tCIDLib::TCard4 c4GetHexValue(const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TBoolean           bValid;
    constexpr tCIDLib::TCard4   c4TmpChars = 128;
    tCIDLib::TCh                szTmp[c4TmpChars + 1];

    if (!::GetLocaleInfo(CIDKernel_Locale_Win32::lcidUser, c4Index, szTmp, c4TmpChars))
        TKrnlError::ThrowHostError(::GetLastError());

    tCIDLib::TCard4 c4Ret = TRawStr::c4AsBinary(szTmp, bValid, tCIDLib::ERadices::Hex);
    if (!bValid)
        TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_ConvertValue);

    return c4Ret;
}


static tCIDLib::TCh chGetCharValue(const tCIDLib::TCard4 c4Index)
{
    constexpr tCIDLib::TCard4   c4TmpChars = 127;
    tCIDLib::TCh                szTmp[c4TmpChars + 1];

    if (!::GetLocaleInfo(CIDKernel_Locale_Win32::lcidUser, c4Index, szTmp, c4TmpChars))
        TKrnlError::ThrowHostError(::GetLastError());

    return szTmp[0];
}


static tCIDLib::TVoid
ConvertAToken(  const   tCIDLib::TCh* const pszToken
                ,       tCIDLib::TCh* const pszOutput
                , const tCIDLib::TCard4     c4MaxChars)
{
    //
    //  Convert this token to the CIDLib token that does the same
    //  thing. Then copy it into the output string.
    //
    if (TRawStr::bCompareStr(pszToken, L"dddd"))
        TRawStr::CatStr(pszOutput, L"%(d)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"dd"))
        TRawStr::CatStr(pszOutput, L"%(D,2,0)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"d"))
        TRawStr::CatStr(pszOutput, L"%(D)", c4MaxChars);

    else if (TRawStr::bCompareStr(pszToken, L"y"))
        TRawStr::CatStr(pszOutput, L"%(y)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"yy"))
        TRawStr::CatStr(pszOutput, L"%(y,2,0)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"yyyy"))
        TRawStr::CatStr(pszOutput, L"%(Y)", c4MaxChars);


    else if (TRawStr::bCompareStr(pszToken, L"M"))
        TRawStr::CatStr(pszOutput, L"%(M)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"MM"))
        TRawStr::CatStr(pszOutput, L"%(M,2,0)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"MMM"))
        TRawStr::CatStr(pszOutput, L"%(n)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"MMMM"))
        TRawStr::CatStr(pszOutput, L"%(m)", c4MaxChars);


    else if (TRawStr::bCompareStr(pszToken, L"h"))
        TRawStr::CatStr(pszOutput, L"%(h)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"hh"))
        TRawStr::CatStr(pszOutput, L"%(h,2,0)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"H"))
        TRawStr::CatStr(pszOutput, L"%(H)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"HH"))
        TRawStr::CatStr(pszOutput, L"%(H,2,0)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"m"))
        TRawStr::CatStr(pszOutput, L"%(u)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"mm"))
        TRawStr::CatStr(pszOutput, L"%(u,2,0)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"ss"))
        TRawStr::CatStr(pszOutput, L"%(s,2,0)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"s"))
        TRawStr::CatStr(pszOutput, L"%(s)", c4MaxChars);

    else if (TRawStr::bCompareStr(pszToken, L"tt"))
        TRawStr::CatStr(pszOutput, L"%(P)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"t"))
        TRawStr::CatStr(pszOutput, L"%(p)", c4MaxChars);
    else if (TRawStr::bCompareStr(pszToken, L"gg"))
    {
        // <TBD> Deal with gg
    }
     else
    {

        //
        //  Its not a token that we understand. If we are in debug
        //  mode, then throw an exception in order to let the tester
        //  know about it. Otherwise, just ignore it.
        //
        #if CID_DEBUG_ON
        TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_UnknownDTField);
        #endif
    }
}


static tCIDLib::TVoid
ConvertDateTimeFormat(          tCIDLib::TCh* const pszInOut
                        , const tCIDLib::TCard4     c4MaxChars
                        , const tCIDLib::TCh* const pszSeparators)
{
    // Strings to build up tokens and the output
    constexpr tCIDLib::TCard4 c4TmpChars = 127;
    tCIDLib::TCh szToken[c4TmpChars+1];
    tCIDLib::TCh szOutput[c4TmpChars+1];

    // Start off the output strings empty
    szToken[0] = kCIDLib::chNull;
    szOutput[0] = kCIDLib::chNull;

    //
    //  Enter the state machine. We start off the state as whitespace. The
    //  machine will self correct itself if this is not correct for the
    //  first char.
    //
    tCIDLib::TCard4 c4SrcIndex = 0;
    tCIDLib::TCard4 c4OutIndex = 0;
    tCIDLib::TCard4 c4TokenInd = 0;
    EDTStates eState = EDTState_Whitespace;

    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        if (eState == EDTState_Whitespace)
        {
            //
            //  If the current character is still a whitespace, then
            //  copy it over to the output string. Else, change the
            //  state appropriately.
            //
            if (TRawStr::pszFindChar(pszSeparators, pszInOut[c4SrcIndex]))
            {
                szOutput[c4OutIndex++] = pszInOut[c4SrcIndex++];
            }
             else
            {
                if (pszInOut[c4SrcIndex] == L'\'')
                {
                    eState = EDTState_InQuote;
                    c4SrcIndex++;
                }
                 else if (!pszInOut[c4SrcIndex])
                {
                    break;
                }
                 else
                {
                    eState = EDTState_InToken;
                    c4TokenInd = 0;
                }
            }
        }
         else if (eState == EDTState_InQuote)
        {
            if (!pszInOut[c4SrcIndex])
                break;

            //
            //  If the current character is a single quote, then we need
            //  to skip it and move the state up to whitespace again.
            //  Otherwise, just copy over the chars as is.
            //
            if (pszInOut[c4SrcIndex] == L'\'')
            {
                c4SrcIndex++;
                eState = EDTState_Whitespace;
            }
             else if (!pszInOut[c4SrcIndex])
            {
                break;
            }
             else
            {
                szOutput[c4OutIndex++] = pszInOut[c4SrcIndex++];
            }
        }
         else if (eState == EDTState_InToken)
        {
            //
            //  If the current input char is a separator or null, then
            //  cap off the token, convert it to one of our tokens, copy
            //  it to the output string, and move the state up to
            //  whitespace.
            //
            //  Otherwise, we are still in the token, so just copy over
            //  the char.
            //
            if (!pszInOut[c4SrcIndex]
            ||  TRawStr::pszFindChar(pszSeparators, pszInOut[c4SrcIndex]))
            {
                // Cap off the token and output strings
                szToken[c4TokenInd] = kCIDLib::chNull;
                szOutput[c4OutIndex] = kCIDLib::chNull;

                // Convert to our token and cat to the output string
                ConvertAToken(szToken, szOutput, c4TmpChars);

                // Resync the output index
                c4OutIndex = TRawStr::c4StrLen(szOutput);

                // And change the state or break out
                if (pszInOut[c4SrcIndex])
                    eState = EDTState_Whitespace;
                else
                    break;
            }
             else
            {
                szToken[c4TokenInd++] = pszInOut[c4SrcIndex++];
            }
        }
         else
        {
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_ParseState);
        }

        if (c4OutIndex >= c4TmpChars)
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcData_BufferOverflow);
    }

    // Copy the output string back to the caller's buffer
    szOutput[c4OutIndex] = kCIDLib::chNull;
    TRawStr::CopyStr(pszInOut, szOutput, c4MaxChars);
}


static tCIDLib::TVoid
GetStringValue( const   tCIDLib::TCard4     c4Index
                ,       tCIDLib::TCh* const pszTarget
                , const tCIDLib::TCard4     c4MaxChars)
{
    pszTarget[0] = kCIDLib::chNull;

    if (!::GetLocaleInfo(CIDKernel_Locale_Win32::lcidUser, c4Index, pszTarget, c4MaxChars))
    {
        // If no error, then its just an empty field so return empty string
        if (::GetLastError())
            TKrnlError::ThrowHostError(::GetLastError());
    }
}


static tCIDLib::ELanguages eXlatLanguage(const tCIDLib::TCard4 c4ToXlat)
{
    for (tCIDLib::TCard4 c4Index = 0;
                    c4Index < CIDKernel_Locale_Win32::c4LangXlatCount; c4Index++)
    {
        if (CIDKernel_Locale_Win32::alxlTable[c4Index].c4Native == c4ToXlat)
            return CIDKernel_Locale_Win32::alxlTable[c4Index].eIndependent;
    }

    // Return the default english
    return tCIDLib::ELanguages::English;
}



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDKrnlModule::InitTermLocale(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        //
        //  Query the user's default locale and then use that to load up
        //  the static data members of the TKrnlLocale class. Note. We need
        //  to store the locale id away locally also for later calls.
        //
        CIDKernel_Locale_Win32::lcidUser = ::GetUserDefaultLCID();

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
TKrnlLocale::bQueryLongDay( const   tCIDLib::EWeekDays  eDay
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    try
    {
        //
        //  Calculate the locale field to get. Unfortunately they put
        //  monday first, while we put sunday first. So, if its sunday,
        //  treat it specially, else just ofset by one.
        //
        //  Be careful of the order of +/- on the false scenario, else we
        //  could underflow and wrap.
        //
        const tCIDLib::TCard4 c4StrFld
        (
            (eDay == tCIDLib::EWeekDays::Sunday)
             ? LOCALE_SDAYNAME7
             : (LOCALE_SDAYNAME1 + (tCIDLib::c4EnumOrd(eDay))) - 1
        );
        GetStringValue(c4StrFld, pszToFill, c4MaxChars);
    }

    catch(const TKrnlError& kerrToCatch)
    {
        // Catch the error, set it as the last error, and return failure
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlLocale::bQueryLongMonth(const  tCIDLib::EMonths    eMonth
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    try
    {
        const tCIDLib::TCard4 c4StrFld = LOCALE_SMONTHNAME1 + tCIDLib::c4EnumOrd(eMonth);
        GetStringValue(c4StrFld, pszToFill, c4MaxChars);
    }

    catch(const TKrnlError& kerrToCatch)
    {
        // Catch the error, set it as the last error, and return failure
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlLocale::bQueryShortDay(const   tCIDLib::EWeekDays  eDay
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    try
    {
        //
        //  Calculate the locale field to get. Unfortunately they put
        //  monday first, while we put sunday first. So, if its sunday,
        //  treat it specially, else just ofset by one.
        //
        const tCIDLib::TCard4 c4StrFld
        (
            (eDay == tCIDLib::EWeekDays::Sunday)
            ? LOCALE_SABBREVDAYNAME7
            : (LOCALE_SABBREVDAYNAME1 + tCIDLib::c4EnumOrd(eDay)) - 1
        );
        GetStringValue(c4StrFld, pszToFill, c4MaxChars);
    }

    catch(const TKrnlError& kerrToCatch)
    {
        // Catch the error, set it as the last error, and return failure
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlLocale::bQueryShortMonth(  const   tCIDLib::EMonths    eMonth
                                ,       tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    try
    {
        const tCIDLib::TCard4 c4StrFld = LOCALE_SABBREVMONTHNAME1 + tCIDLib::c4EnumOrd(eMonth);
        GetStringValue(c4StrFld, pszToFill, c4MaxChars);
    }

    catch(const TKrnlError& kerrToCatch)
    {
        // Catch the error, set it as the last error, and return failure
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlLocale::bQueryStrField(const   EStrFlds            eField
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    try
    {
        switch(eField)
        {
            // These we query from the system upon demand
            case EStrFlds::Country :
                GetStringValue(LOCALE_SCOUNTRY, pszToFill, c4MaxChars);
                break;

            case EStrFlds::EnglishLanguage :
                GetStringValue(LOCALE_SENGLANGUAGE, pszToFill, c4MaxChars);
                break;

            case EStrFlds::Language :
                GetStringValue(LOCALE_SLANGUAGE, pszToFill, c4MaxChars);
                break;

            case EStrFlds::ISOCountry :
                GetStringValue(LOCALE_SABBREVCTRYNAME, pszToFill, c4MaxChars);
                break;

            case EStrFlds::ISOLanguage :
                GetStringValue(LOCALE_SABBREVLANGNAME, pszToFill, c4MaxChars);
                break;

            case EStrFlds::EnglishCountry :
                GetStringValue(LOCALE_SENGCOUNTRY, pszToFill, c4MaxChars);
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
    }

    catch(const TKrnlError& kerrToCatch)
    {
        // Catch the error, set it as the last error, and return failure
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlLocale: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlLocale::bLoadLocaleInfo()
{
    constexpr tCIDLib::TCard4   c4BufSize = 1023;
    tCIDLib::TBoolean           bValid;
    tCIDLib::TCard4             c4Tmp;
    tCIDLib::TCh                szTmp[c4BufSize + 1];
    tCIDLib::TCh*               pszTmp;

    try
    {
        // ------------------------------------------
        //  Do the general stuff
        // ------------------------------------------
        c4Tmp = c4GetDecValue(LOCALE_IMEASURE);
        if (c4Tmp == 0)
            s_eMeasure = tCIDLib::EMeasures::Metric;
        else if (c4Tmp == 1)
            s_eMeasure = tCIDLib::EMeasures::US;
        else
            s_eMeasure = tCIDLib::EMeasures::Unknown;

        c4Tmp = c4GetHexValue(LOCALE_IDEFAULTLANGUAGE);
        s_eDefLanguage = eXlatLanguage(c4Tmp & 0xFF);

        c4Tmp = c4GetHexValue(LOCALE_ILANGUAGE);
        s_eLanguage = eXlatLanguage(c4Tmp & 0xFF);


        // ------------------------------------------
        // Do date/time values
        // ------------------------------------------
        s_b24Hour = c4GetDecValue(LOCALE_ITIME) == 1;
        s_chDateSep = chGetCharValue(LOCALE_SDATE);
        s_chTimeSep = chGetCharValue(LOCALE_STIME);

        //
        //  Build up a separator string for parsing the format strings. This
        //  string includes the two separators plus the comma, space, and
        //  single quote chars.
        //
        tCIDLib::TCh szDTSeps[] = L"   ,'";
        szDTSeps[0] = s_chDateSep;
        szDTSeps[1] = s_chTimeSep;

        //
        //  Now query and convert the date/time formatting strings and
        //  store them.
        //
        constexpr tCIDLib::TCard4 c4DTBufChars = c4MaxBufChars(s_szTimeFmt);
        GetStringValue(LOCALE_STIMEFORMAT, s_szTimeFmt, c4DTBufChars);
        ConvertDateTimeFormat(s_szTimeFmt, c4DTBufChars, szDTSeps);

        GetStringValue(LOCALE_SSHORTDATE, s_szDateFmt, c4DTBufChars);
        ConvertDateTimeFormat(s_szDateFmt, c4DTBufChars, szDTSeps);

        // And get the AM/PM strings
        GetStringValue(LOCALE_S1159, s_szAMString, c4MaxBufChars(s_szAMString));
        GetStringValue(LOCALE_S2359, s_szPMString, c4MaxBufChars(s_szAMString));


        // ------------------------------------------
        //  Do non-monetary numeric values
        // ------------------------------------------
        s_c4DecDigits = c4GetDecValue(LOCALE_IDIGITS);
        s_chGroupSep = chGetCharValue(LOCALE_STHOUSAND);
        s_chPosSign = chGetCharValue(LOCALE_SPOSITIVESIGN);
        s_chNegSign = chGetCharValue(LOCALE_SNEGATIVESIGN);
        s_chDecSymbol = chGetCharValue(LOCALE_SDECIMAL);

        //
        //  This one is kind of wierd. It actually has the ability to give a
        //  separate size for each group. We don't support that so we just
        //  get the first one out.
        //
        GetStringValue(LOCALE_SGROUPING, szTmp, c4BufSize);
        pszTmp = TRawStr::pszFindChar(szTmp, ';');
        if (pszTmp)
            *pszTmp = kCIDLib::chNull;
        s_c4GroupSize = TRawStr::c4AsBinary(szTmp, bValid, tCIDLib::ERadices::Dec);
        if (!bValid)
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_ConvertValue);


        // ------------------------------------------
        // And now do monetary values
        // ------------------------------------------
        s_c4MonetaryDecDigits = c4GetDecValue(LOCALE_ICURRDIGITS);
        s_chMonetaryDecSymbol = chGetCharValue(LOCALE_SMONDECIMALSEP);
        s_chMonetaryGrpSep = chGetCharValue(LOCALE_SMONTHOUSANDSEP);
        s_chCurrencySymbol = chGetCharValue(LOCALE_SCURRENCY);

        //
        //  Deal with the format of positive monetary values. The value we get
        //  is just a number, which we map into an array of format strings.
        //
        c4Tmp = c4GetDecValue(LOCALE_ICURRENCY);

        //
        //  Local static array that maps the monetary format indices to our
        //  CIDLib type of format strings.
        //
        static const tCIDLib::TCh* apszPosMonFmt[] =
        {
            L"%(y)%(v)"
            , L"%(v)%(y)"
            , L"%(y) %(v)"
            , L"%(v) %(y)"
        };

        if (c4Tmp > tCIDLib::c4ArrayElems(apszPosMonFmt))
        {
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_UnknownCurrMode);
        }
         else
        {
            TRawStr::CopyStr
            (
                s_szPosMonFmt
                , apszPosMonFmt[c4Tmp]
                , c4MaxBufChars(s_szPosMonFmt)
            );
        }

        //
        //  Deal with the format of negative monetary values. The value we get
        //  is just a number, which we map into an array.
        //
        c4Tmp = c4GetDecValue(LOCALE_INEGCURR);

        //
        //  Local static array that maps the monetary format indices to our
        //  CIDLib type of format strings.
        //
        static const tCIDLib::TCh* apszNegMonFmt[] =
        {
            L"(%(y)%(v))"
            , L"%(s)%(y)%(v)"
            , L"%(y)%(s)%(v)"
            , L"%(y)%(v)%(s)"
            , L"%(v)%(y)"
            , L"%(s)%(v)%(y)"
            , L"%(v)%(s)%(y)"
            , L"%(v)%(y)%(s)"
            , L"%(s)%(v) %(y)"
            , L"%(s)%(y) %(v)"
            , L"%(v) %(y)%(s)"
            , L"%(y) %(v)%(s)"
            , L"%(y) %(s)%(v)"
            , L"%(v)%(s) %(y)"
            , L"(%(y) %(v))"
            , L"(%(v) %(y))"
        };

        if (c4Tmp > tCIDLib::c4ArrayElems(apszNegMonFmt))
        {
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_UnknownCurrMode);
        }
         else
        {
            TRawStr::CopyStr
            (
                s_szNegMonFmt
                , apszNegMonFmt[c4Tmp]
                , c4MaxBufChars(s_szNegMonFmt)
            );
        }

        //
        //  This one is kind of wierd. It actually has the ability to give a
        //  separate size for each group. We don't support that so we just
        //  get the first one out.
        //
        GetStringValue(LOCALE_SMONGROUPING, szTmp, c4BufSize);
        pszTmp = TRawStr::pszFindChar(szTmp, ';');
        if (pszTmp)
            *pszTmp = kCIDLib::chNull;
        s_c4MonetaryGrpSize = TRawStr::c4AsBinary(szTmp, bValid, tCIDLib::ERadices::Dec);
        if (!bValid)
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcLoc_ConvertValue);
    }

    catch(const TKrnlError& kerrToCatch)
    {
        // Catch the error, set it as the last error, and return failure
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}
