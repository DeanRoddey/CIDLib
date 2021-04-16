//
// FILE NAME: CIDMacroEng_TimeClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/18/2003
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
//  This is the header file for the CIDMacroEng_TimeClass.cpp file, which
//  implements the info and value classes required to create a new macro
//  class, in this case a wrapper around the TTime C++ class.
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
//  CLASS: TMEngTimeVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngTimeVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTimeVal(const TMEngTimeVal&) = delete;

        TMEngTimeVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
        );

        ~TMEngTimeVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTimeVal& operator=(const TMEngTimeVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime enctValue() const;

        tCIDLib::TBoolean bParseFromPattern
        (
            const   TString&                strToParse
            ,       TString&                strErr
        );

        const TTime& tmValue() const;

        TTime& tmValue();

        TTime& tmValue
        (
            const   TTime&                  tmToSet
        );

        tCIDLib::TVoid SetPattern
        (
            const   TTimeCompList&          fcolToSet
            , const tCIDLib::TCh            chDateSep
            , const tCIDLib::TCh            chTimeSep
            , const tCIDLib::TCh            chTZSep
        );

        tCIDLib::TVoid SetPattern
        (
            const   TTimeCompList&          fcolToSet
        );

        tCIDLib::TVoid SetTime
        (
            const   tCIDLib::TEncodedTime   enctToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_chXXXSep
        //      The characters to use for any separate components in the parsing
        //      pattern.
        //
        //  m_pfcolPattern
        //      They can set a parse pattern on us and then use by calling the
        //      ParseFromPattern() method. If never set it's null.
        //
        //  m_tmValue
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TCh        m_chDateSep;
        tCIDLib::TCh        m_chTimeSep;
        tCIDLib::TCh        m_chTZSep;
        TTimeCompList*      m_pfcolPattern;
        TTime               m_tmValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTimeVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngTimeInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngTimeInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTimeInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngTimeInfo(const TMEngTimeInfo&) = delete;

        ~TMEngTimeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTimeInfo& operator=(const TMEngTimeInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errCaught
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2EnumId_XXX
        //      We create some enum nested types, so we store the ids here for quick
        //      access later.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that we set up
        //      during init.
        //
        //  m_c4ErrBadSet
        //      We have an error enum, so we store the ordinals so that we can throw
        //      macro level errors internally if needed.
        //
        //  m_pmeciXXX
        //      Various nested types
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2EnumId_Errors;
        tCIDLib::TCard2     m_c2EnumId_Months;
        tCIDLib::TCard2     m_c2EnumId_Special;
        tCIDLib::TCard2     m_c2EnumId_TimeComps;
        tCIDLib::TCard2     m_c2EnumId_WeekDays;
        tCIDLib::TCard2     m_c2TypeId_TimeCompsVec;
        tCIDLib::TCard2     m_c2MethId_AddStamp;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_Equal;
        tCIDLib::TCard2     m_c2MethId_FormatToStr;
        tCIDLib::TCard2     m_c2MethId_GetCurMillis;
        tCIDLib::TCard2     m_c2MethId_GetCurMillis64;
        tCIDLib::TCard2     m_c2MethId_GetCurStamp;
        tCIDLib::TCard2     m_c2MethId_GetCurStampPlusMSs;
        tCIDLib::TCard2     m_c2MethId_GetCurStampPlusSecs;
        tCIDLib::TCard2     m_c2MethId_GetDateFlds;
        tCIDLib::TCard2     m_c2MethId_GetJulianDate;
        tCIDLib::TCard2     m_c2MethId_GetStamp;
        tCIDLib::TCard2     m_c2MethId_GetTimeFlds;
        tCIDLib::TCard2     m_c2MethId_GetTZOffset;
        tCIDLib::TCard2     m_c2MethId_GetWeekday;
        tCIDLib::TCard2     m_c2MethId_IsDiffGreater;
        tCIDLib::TCard2     m_c2MethId_IsLeapYear;
        tCIDLib::TCard2     m_c2MethId_LocalToUTC;
        tCIDLib::TCard2     m_c2MethId_ParseFromPattern;
        tCIDLib::TCard2     m_c2MethId_Parse8601;
        tCIDLib::TCard2     m_c2MethId_SetDate;
        tCIDLib::TCard2     m_c2MethId_SetDateTime;
        tCIDLib::TCard2     m_c2MethId_SetDefFmt;
        tCIDLib::TCard2     m_c2MethId_SetFromJulian;
        tCIDLib::TCard2     m_c2MethId_SetFromStamp;
        tCIDLib::TCard2     m_c2MethId_SetParsePattern;
        tCIDLib::TCard2     m_c2MethId_SetParsePattern2;
        tCIDLib::TCard2     m_c2MethId_SetSpecial;
        tCIDLib::TCard2     m_c2MethId_SetToDifference;
        tCIDLib::TCard2     m_c2MethId_SetToSunrise;
        tCIDLib::TCard2     m_c2MethId_SetToSunset;
        tCIDLib::TCard2     m_c2MethId_SetTime;
        tCIDLib::TCard2     m_c2MethId_Sleep;
        tCIDLib::TCard2     m_c2MethId_SpecialCtor;
        tCIDLib::TCard2     m_c2MethId_SubStamp;
        tCIDLib::TCard2     m_c2MethId_UTCToLocal;
        tCIDLib::TCard4     m_c4ErrCvtFailed;
        tCIDLib::TCard4     m_c4ErrFmtFailed;
        tCIDLib::TCard4     m_c4ErrParse;
        tCIDLib::TCard4     m_c4ErrPatternFailed;
        tCIDLib::TCard4     m_c4ErrNegTime;
        tCIDLib::TCard4     m_c4ErrSetFailed;
        TMEngEnumInfo*      m_pmeciErrors;
        TMEngEnumInfo*      m_pmeciMonths;
        TMEngEnumInfo*      m_pmeciSpecial;
        TMEngEnumInfo*      m_pmeciTimeComps;
        TMEngEnumInfo*      m_pmeciWeekDays;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTimeInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK



