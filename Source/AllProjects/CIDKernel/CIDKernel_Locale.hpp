//
// FILE NAME: CIDKernel_Locale.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_Locale.Cpp file. This file
//  implements the TKrnlLocale class, which supports the querying of locale
//  information. Its all static, because it only supports a single locale
//  per-process. So its just used via the class name. Its not a namespace
//  because it needs to have data members shared with the platform and
//  non-platform dependent implementation files.
//
// CAVEATS/GOTCHAS:
//
//  1)  The monetary format and date/time format members are strings that
//      encode the fields in the order that they should be formatted. See
//      the TTime and TMoney classes in CIDLib which define the format
//      of these strings.
//
//  2)  Some of the fields are cached in these objects, in order to very
//      much speed up the access of fields that often accessed. Others are
//      loaded from the system support as required.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlLocale
//  PREFIX: kloc
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlLocale
{
    public :
        // -------------------------------------------------------------------
        //  The available string locale fields
        // -------------------------------------------------------------------
        enum class EStrFlds
        {
            AMString
            , Country
            , DateFormat
            , EnglishLanguage
            , EnglishCountry
            , ISOCountry
            , ISOLanguage
            , Language
            , NegMonFormat
            , PMString
            , PosMonFormat
            , TimeFormat
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean b24Hour();

        static tCIDLib::TBoolean bDefLangSuffix
        (
            const   tCIDLib::TCh*&          pszToFill
        );

        static tCIDLib::TBoolean bLangSuffix
        (
            const   tCIDLib::TCh*&          pszToFill
        );

        static tCIDLib::TBoolean bLangSuffixFor
        (
            const   tCIDLib::ELanguages     eLangFor
            , const tCIDLib::TCh*&          pszToFill
        );

        static tCIDLib::TCard4 c4DecimalDigits();

        static tCIDLib::TCard4 c4GroupSize();

        static tCIDLib::TCard4 c4MonetaryDecimalDigits();

        static tCIDLib::TCard4 c4MonetaryGroupSize();

        static tCIDLib::TCh chCurrencySymbol();

        static tCIDLib::TCh chDateSeparator();

        static tCIDLib::TCh chDecimalSymbol();

        static tCIDLib::TCh chGroupSeparator();

        static tCIDLib::TCh chPositiveSign();

        static tCIDLib::TCh chMonetaryDecimalSymbol();

        static tCIDLib::TCh chMonetaryGroupSeparator();

        static tCIDLib::TCh chNegativeSign();

        static tCIDLib::TCh chTimeSeparator();

        static tCIDLib::ELanguages eDefLanguage();

        static tCIDLib::ELanguages eLanguage();

        static tCIDLib::EMeasures eMeasure();

        static tCIDLib::ELanguages eXlatLangSuffix
        (
            const   tCIDLib::TCh* const     pszSuffix
        );

        static tCIDLib::TBoolean bQueryLongDay
        (
            const   tCIDLib::EWeekDays      eDay
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bQueryLongMonth
        (
            const   tCIDLib::EMonths        eMonth
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bQueryShortDay
        (
            const   tCIDLib::EWeekDays      eDay
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bQueryShortMonth
        (
            const   tCIDLib::EMonths        eMonth
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bQueryStrField
        (
            const   EStrFlds                eField
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TKrnlLocale(const TKrnlLocale&) = delete;

        ~TKrnlLocale();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlLocale& operator=(const TKrnlLocale&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends. This is the per-platform init method, and
        //  allows it to fill in the cached data members of this class.
        // -------------------------------------------------------------------
        friend class TCIDKrnlModule;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TKrnlLocale();


    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bLoadLocaleInfo();


        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_eDefLanguage
        //      The default language used by this locale.
        //
        //  s_eLanguage
        //      The language id for the language used by this locale.
        //
        //  s_eMeasure
        //      The measuring system used by this locale.
        //
        //  Non-monetary values
        //  -------------------
        //  s_c4DecDigits
        //      The default number of decimal digits to use for floating
        //      point values.
        //
        //  s_c4GroupSize
        //      Indicates the grouping of characters into thousands or
        //      whatever, for numeric and monetary values. 0 if none for
        //      this locale.
        //
        //  s_chDecSymbol
        //      The character for use in floating point numeric values.
        //      0 if none.
        //
        //  s_chGroupSep
        //      The separator value for numeric values. 0 if none.
        //
        //  s_chPosSign
        //  s_chNegSign
        //      The +/- signs for numeric values.
        //
        //
        //  Monetary values
        //  ---------------
        //  s_c4MonetaryDecDigits
        //      The number of decimal digits to the right of the decimal
        //      in monetary values.
        //
        //  s_c4MonetaryGrpSize
        //      The size of digit groups in monetary values
        //
        //  s_chCurrencySymbol
        //      The symbol that represents a monetary value. 0 if none.
        //
        //  s_chMonetaryDecSymbol
        //      The symbol used as the decimal place in monetary values.
        //
        //  s_chMonetaryGrpSep
        //      The separator used between groups in monetary values.
        //
        //  s_szMonNegFmt
        //  s_szMonPosFmt
        //      These represent the format for negative and positive monetary
        //      values in this locale.
        //
        //
        //  Date/Time values
        //  ----------------
        //  s_b24Hour
        //      Indicates if the local time format is 24 or 12 hour type
        //
        //  s_chDateSep
        //  s_chTimeSep
        //      These are the separators used in dates and times, between the
        //      components of the formatted value.
        //
        //  s_szDateFmt
        //  s_szTimeFmt
        //      The default formatting strings for dates and times in this
        //      locale.
        //
        //  s_szAMString
        //  s_szPMString
        //      The locale strings for the AM/PM strings used in 12 hour
        //      times.
        // -------------------------------------------------------------------
        static tCIDLib::ELanguages  s_eDefLanguage;
        static tCIDLib::ELanguages  s_eLanguage;
        static tCIDLib::EMeasures   s_eMeasure;

        // These are for non-monetary values
        static tCIDLib::TCard4      s_c4DecDigits;
        static tCIDLib::TCard4      s_c4GroupSize;
        static tCIDLib::TCh         s_chDecSymbol;
        static tCIDLib::TCh         s_chGroupSep;
        static tCIDLib::TCh         s_chPosSign;
        static tCIDLib::TCh         s_chNegSign;

        // These are for monetary values
        static tCIDLib::TCard4      s_c4MonetaryDecDigits;
        static tCIDLib::TCard4      s_c4MonetaryGrpSize;
        static tCIDLib::TCh         s_chCurrencySymbol;
        static tCIDLib::TCh         s_chMonetaryDecSymbol;
        static tCIDLib::TCh         s_chMonetaryGrpSep;
        static tCIDLib::TZStr32     s_szNegMonFmt;
        static tCIDLib::TZStr32     s_szPosMonFmt;

        // These are for time/date values
        static tCIDLib::TBoolean    s_b24Hour;
        static tCIDLib::TCh         s_chDateSep;
        static tCIDLib::TCh         s_chTimeSep;
        static tCIDLib::TZStr32     s_szDateFmt;
        static tCIDLib::TZStr32     s_szTimeFmt;
        static tCIDLib::TZStr32     s_szAMString;
        static tCIDLib::TZStr32     s_szPMString;
};

#pragma CIDLIB_POPPACK


