//
// FILE NAME: CIDMacroEng_TimeClass.cpp
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
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the fundamental MEng.Time macro class.
//
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngTimeVal,TMEngClassVal)
RTTIDecls(TMEngTimeInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_TimeClasses
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  The names for the types that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strTime(L"Time");
        const TString   strTimeClassPath(L"MEng.Time");
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngTimeVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTimeVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngTimeVal::TMEngTimeVal( const   TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Time), eConst)
    , m_chDateSep(TLocale::chDateSeparator())
    , m_chTimeSep(TLocale::chTimeSeparator())
    , m_chTZSep(kCIDLib::chSpace)
    , m_pfcolPattern(nullptr)
{
}

TMEngTimeVal::~TMEngTimeVal()
{
    delete m_pfcolPattern;
}


// ---------------------------------------------------------------------------
//  TMEngTimeVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngTimeVal::bDbgFormat(       TTextOutStream&     strmTarget
                        , const TMEngClassInfo&
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices
                        , const TCIDMacroEngine&    ) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
        strmTarget << L"Raw Time=";

     try
    {
        strmTarget << m_tmValue.enctTime();
    }

    catch(...)
    {
        // It could be an invalid time stamp, so catch that
        strmTarget << L"????";
    }

    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget  << L"\nFormatted=" << m_tmValue
                    << L"\nDef Format=" << m_tmValue.strDefaultFormat();

        tCIDLib::TEncodedTime enctVal = m_tmValue.enctTime();
        const tCIDLib::TCard8 c8Hours(enctVal / kCIDLib::enctOneHour);
        enctVal -= (c8Hours * kCIDLib::enctOneHour);
        const tCIDLib::TCard8 c8Minutes(enctVal / kCIDLib::enctOneMinute);
        enctVal -= (c8Minutes * kCIDLib::enctOneMinute);
        const tCIDLib::TCard8 c8Seconds(enctVal / kCIDLib::enctOneSecond);

        strmTarget  << L"\nElapsed Hours/Mins/Secs="
                    << c8Hours << kCIDLib::chForwardSlash
                    << c8Minutes << kCIDLib::chForwardSlash
                    << c8Seconds;
    }
    return kCIDLib::True;
}


tCIDLib::TVoid
TMEngTimeVal::CopyFrom( const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_tmValue = static_cast<const TMEngTimeVal&>(mecvToCopy).m_tmValue;
}


// ---------------------------------------------------------------------------
//  TMEngTimeVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngTimeVal::bParseFromPattern(const TString& strToParse, TString& strErr)
{
    if (!m_pfcolPattern)
    {
        strErr = L"The parsing pattern has not been set";
        return kCIDLib::False;
    }

    const tCIDLib::TBoolean bRes = m_tmValue.bParseFromText
    (
        strToParse, *m_pfcolPattern, m_chDateSep, m_chTimeSep, m_chTZSep
    );
    if (!bRes)
    {
        strErr = L"The passed time string did not match the parsing pattern";
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TEncodedTime TMEngTimeVal::enctValue() const
{
    return m_tmValue.enctTime();
}


const TTime& TMEngTimeVal::tmValue() const
{
    return m_tmValue;
}

TTime& TMEngTimeVal::tmValue()
{
    return m_tmValue;
}

TTime& TMEngTimeVal::tmValue(const TTime& tmToSet)
{
    m_tmValue = tmToSet;
    return m_tmValue;
}


tCIDLib::TVoid
TMEngTimeVal::SetPattern(const  TTime::TCompList&   fcolToSet
                        , const tCIDLib::TCh        chDateSep
                        , const tCIDLib::TCh        chTimeSep
                        , const tCIDLib::TCh        chTZSep)
{
    if (!m_pfcolPattern)
        m_pfcolPattern = new TTime::TCompList(fcolToSet);
    else
        *m_pfcolPattern = fcolToSet;

    m_chDateSep = chDateSep;
    m_chTimeSep = chTimeSep;
    m_chTZSep = chTZSep;
}

tCIDLib::TVoid TMEngTimeVal::SetPattern(const TTime::TCompList& fcolToSet)
{
    if (!m_pfcolPattern)
        m_pfcolPattern = new TTime::TCompList(fcolToSet);
    else
        *m_pfcolPattern = fcolToSet;
}


tCIDLib::TVoid TMEngTimeVal::SetTime(const tCIDLib::TEncodedTime enctToSet)
{
    m_tmValue.enctTime(enctToSet);
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngTimeInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngTimeInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngTimeInfo::strPath()
{
    return CIDMacroEng_TimeClasses::strTimeClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngTimeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngTimeInfo::TMEngTimeInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_TimeClasses::strTime
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2EnumId_Months(kCIDMacroEng::c2BadId)
    , m_c2EnumId_TimeComps(kCIDMacroEng::c2BadId)
    , m_c2EnumId_WeekDays(kCIDMacroEng::c2BadId)
    , m_c2TypeId_TimeCompsVec(kCIDMacroEng::c2BadId)
    , m_c2MethId_AddStamp(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Equal(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCurMillis(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCurMillis64(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCurStamp(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCurStampPlusMSs(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCurStampPlusSecs(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetDateFlds(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetJulianDate(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetStamp(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetTimeFlds(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetTZOffset(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetWeekday(kCIDMacroEng::c2BadId)
    , m_c2MethId_IsDiffGreater(kCIDMacroEng::c2BadId)
    , m_c2MethId_IsLeapYear(kCIDMacroEng::c2BadId)
    , m_c2MethId_LocalToUTC(kCIDMacroEng::c2BadId)
    , m_c2MethId_Parse8601(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseFromPattern(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetDate(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetDateTime(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetDefFmt(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetFromJulian(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetFromStamp(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetParsePattern(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetParsePattern2(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetSpecial(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetToDifference(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetToSunrise(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetToSunset(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetTime(kCIDMacroEng::c2BadId)
    , m_c2MethId_Sleep(kCIDMacroEng::c2BadId)
    , m_c2MethId_SpecialCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_SubStamp(kCIDMacroEng::c2BadId)
    , m_c2MethId_UTCToLocal(kCIDMacroEng::c2BadId)
    , m_c4ErrCvtFailed(kCIDLib::c4MaxCard)
    , m_c4ErrFmtFailed(kCIDLib::c4MaxCard)
    , m_c4ErrNegTime(kCIDLib::c4MaxCard)
    , m_c4ErrSetFailed(kCIDLib::c4MaxCard)
    , m_pmeciErrors(nullptr)
    , m_pmeciMonths(nullptr)
    , m_pmeciTimeComps(nullptr)
    , m_pmeciWeekDays(nullptr)
{
    // We need to import the vector class
    bAddClassImport(TMEngVectorInfo::strPath());
}

TMEngTimeInfo::~TMEngTimeInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngTimeInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngTimeInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  We need to add some nested enum types. First we do one for the days
    //  of the week, months, and then for errors.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"TimeErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 6
        );
        m_c4ErrCvtFailed = m_pmeciErrors->c4AddEnumItem(L"CvtFailed", TString::strEmpty());
        m_c4ErrFmtFailed = m_pmeciErrors->c4AddEnumItem(L"FmtFailed", TString::strEmpty());
        m_c4ErrNegTime = m_pmeciErrors->c4AddEnumItem(L"NegTime", L"This operation would create a negative time");
        m_c4ErrParse = m_pmeciErrors->c4AddEnumItem(L"Parse", TString::strEmpty());
        m_c4ErrPatternFailed = m_pmeciErrors->c4AddEnumItem(L"PatternFailed", TString::strEmpty());
        m_c4ErrSetFailed = m_pmeciErrors->c4AddEnumItem(L"SetFailed", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Enum for months of the year
    {
        m_pmeciMonths = new TMEngEnumInfo
        (
            meOwner
            , L"Months"
            , strClassPath()
            , L"MEng.Enum"
            , 12
        );
        m_pmeciMonths->c4AddEnumItem(L"January", L"Jan");
        m_pmeciMonths->c4AddEnumItem(L"February", L"Feb");
        m_pmeciMonths->c4AddEnumItem(L"March", L"Mar");
        m_pmeciMonths->c4AddEnumItem(L"April", L"Apr");
        m_pmeciMonths->c4AddEnumItem(L"May", L"May");
        m_pmeciMonths->c4AddEnumItem(L"June", L"Jun");
        m_pmeciMonths->c4AddEnumItem(L"July", L"Jul");
        m_pmeciMonths->c4AddEnumItem(L"August", L"Aug");
        m_pmeciMonths->c4AddEnumItem(L"September", L"Sep");
        m_pmeciMonths->c4AddEnumItem(L"October", L"Oct");
        m_pmeciMonths->c4AddEnumItem(L"November", L"Nov");
        m_pmeciMonths->c4AddEnumItem(L"December", L"Dec");
        m_pmeciMonths->BaseClassInit(meOwner);
        m_c2EnumId_Months = meOwner.c2AddClass(m_pmeciMonths);
        bAddNestedType(m_pmeciMonths->strClassPath());
    }

    // Enum for special times that can be set
    {
        m_pmeciSpecial = new TMEngEnumInfo
        (
            meOwner
            , L"SpecialTimes"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_pmeciSpecial->c4AddEnumItem(L"Base", L"Time base", tCIDLib::ESpecialTimes::Base);
        m_pmeciSpecial->c4AddEnumItem(L"CurDate", L"Midnight today", tCIDLib::ESpecialTimes::CurrentDate);
        m_pmeciSpecial->c4AddEnumItem(L"CurTime", L"Current local time", tCIDLib::ESpecialTimes::CurrentTime);
        m_pmeciSpecial->c4AddEnumItem(L"Noon", L"Noon today", tCIDLib::ESpecialTimes::NoonToday);
        m_pmeciSpecial->c4AddEnumItem(L"UTC", L"Current UTC time", tCIDLib::ESpecialTimes::CurrentUTC);
        m_pmeciSpecial->BaseClassInit(meOwner);
        m_c2EnumId_Special = meOwner.c2AddClass(m_pmeciSpecial);
        bAddNestedType(m_pmeciSpecial->strClassPath());
    }

    // Enum for the time components used in parsing. These must map to the C++ values
    {
        m_pmeciTimeComps = new TMEngEnumInfo
        (
            meOwner
            , L"TimeComps"
            , strClassPath()
            , L"MEng.Enum"
            , 22
        );
        m_pmeciTimeComps->c4AddEnumItem(L"ISO8601TZ", L"ISO-8601TZ", tCIDLib::ETimeComps::ISO8601TZ);
        m_pmeciTimeComps->c4AddEnumItem(L"APPM", L"AM/PM", tCIDLib::ETimeComps::AMPM);
        m_pmeciTimeComps->c4AddEnumItem(L"Colon", L"Colon", tCIDLib::ETimeComps::Colon);
        m_pmeciTimeComps->c4AddEnumItem(L"DateSep", L"Date Separator", tCIDLib::ETimeComps::DateSep);
        m_pmeciTimeComps->c4AddEnumItem(L"DayName", L"Day Name", tCIDLib::ETimeComps::DayName);
        m_pmeciTimeComps->c4AddEnumItem(L"DayNum", L"Day Num", tCIDLib::ETimeComps::DayNum);
        m_pmeciTimeComps->c4AddEnumItem(L"Hour12", L"Hour 12 Form", tCIDLib::ETimeComps::Hour12);
        m_pmeciTimeComps->c4AddEnumItem(L"Hour24", L"Hour 24 Form", tCIDLib::ETimeComps::Hour24);
        m_pmeciTimeComps->c4AddEnumItem(L"Hyphen", L"Hyphen", tCIDLib::ETimeComps::Hyphen);
        m_pmeciTimeComps->c4AddEnumItem(L"Minute", L"Minute", tCIDLib::ETimeComps::Minute);
        m_pmeciTimeComps->c4AddEnumItem(L"MonthName", L"Month Name", tCIDLib::ETimeComps::MonthName);
        m_pmeciTimeComps->c4AddEnumItem(L"MonthNum", L"Month Num", tCIDLib::ETimeComps::MonthNum);
        m_pmeciTimeComps->c4AddEnumItem(L"Punct", L"Punct", tCIDLib::ETimeComps::Punct);
        m_pmeciTimeComps->c4AddEnumItem(L"Second", L"Second", tCIDLib::ETimeComps::Second);
        m_pmeciTimeComps->c4AddEnumItem(L"Space", L"Space", tCIDLib::ETimeComps::Space);
        m_pmeciTimeComps->c4AddEnumItem(L"T", L"T Character", tCIDLib::ETimeComps::T);
        m_pmeciTimeComps->c4AddEnumItem(L"TimeSep", L"Time Separator", tCIDLib::ETimeComps::TimeSep);
        m_pmeciTimeComps->c4AddEnumItem(L"TZ", L"Time Zone", tCIDLib::ETimeComps::TZ);
        m_pmeciTimeComps->c4AddEnumItem(L"TZName", L"Time Zone Name", tCIDLib::ETimeComps::TZName);
        m_pmeciTimeComps->c4AddEnumItem(L"TZSep", L"Time Separator", tCIDLib::ETimeComps::TZSep);
        m_pmeciTimeComps->c4AddEnumItem(L"Year2", L"2 Digit Year", tCIDLib::ETimeComps::Year2);
        m_pmeciTimeComps->c4AddEnumItem(L"Year4", L"4 Digit Year", tCIDLib::ETimeComps::Year4);
        m_pmeciTimeComps->BaseClassInit(meOwner);
        m_c2EnumId_TimeComps = meOwner.c2AddClass(m_pmeciTimeComps);
        bAddNestedType(m_pmeciTimeComps->strClassPath());
    }

    // Enum for days of the week
    {
        m_pmeciWeekDays = new TMEngEnumInfo
        (
            meOwner
            , L"WeekDays"
            , strClassPath()
            , L"MEng.Enum"
            , 7
        );
        m_pmeciWeekDays->c4AddEnumItem(L"Sunday", L"Sun");
        m_pmeciWeekDays->c4AddEnumItem(L"Monday", L"Mon");
        m_pmeciWeekDays->c4AddEnumItem(L"Tuesday", L"Tues");
        m_pmeciWeekDays->c4AddEnumItem(L"Wednesday", L"Wed");
        m_pmeciWeekDays->c4AddEnumItem(L"Thursday", L"Thu");
        m_pmeciWeekDays->c4AddEnumItem(L"Friday", L"Fri");
        m_pmeciWeekDays->c4AddEnumItem(L"Saturday", L"Sat");
        m_pmeciWeekDays->BaseClassInit(meOwner);
        m_c2EnumId_WeekDays = meOwner.c2AddClass(m_pmeciWeekDays);
        bAddNestedType(m_pmeciWeekDays->strClassPath());
    }


    // We need a vector of time component enums
    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"TimeCompList"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , m_c2EnumId_TimeComps
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_TimeCompsVec = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }


    // Define some literal values. We provide some pre-fab format strings
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFmt24HHMM"
            , tCIDMacroEng::EIntrinsics::String
            , new TMEngStringVal(L"kFmt24HHMM", tCIDMacroEng::EConstTypes::Const, TTime::str24HHMM())
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFmtCTime"
            , tCIDMacroEng::EIntrinsics::String
            , new TMEngStringVal(L"kFmtCTime", tCIDMacroEng::EConstTypes::Const, TTime::strCTime())
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFmtFullDate"
            , tCIDMacroEng::EIntrinsics::String
            , new TMEngStringVal(L"kFmtFullDate", tCIDMacroEng::EConstTypes::Const, TTime::strFullDate())
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFmtHHMMSS"
            , tCIDMacroEng::EIntrinsics::String
            , new TMEngStringVal(L"kFmtHHMMSS", tCIDMacroEng::EConstTypes::Const, TTime::strHHMMSS())
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFmtMMDDYYYY"
            , tCIDMacroEng::EIntrinsics::String
            , new TMEngStringVal(L"kFmtMMDDYYYY", tCIDMacroEng::EConstTypes::Const, TTime::strMMDDYYYY())
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFmtMMDDYY"
            , tCIDMacroEng::EIntrinsics::String
            , new TMEngStringVal(L"kFmtMMDDYY", tCIDMacroEng::EConstTypes::Const, TTime::strMMDDYY())
        )
    );


    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kOneMilliSec"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val(L"kOneMilliSec", tCIDMacroEng::EConstTypes::Const, kCIDLib::enctOneMilliSec)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kOneSecond"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val(L"kOneSecond", tCIDMacroEng::EConstTypes::Const, kCIDLib::enctOneSecond)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kOneMinute"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val(L"kOneMinute", tCIDMacroEng::EConstTypes::Const, kCIDLib::enctOneMinute)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kTwoMinutes"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val
              (
                L"kTwoMinutes", tCIDMacroEng::EConstTypes::Const, kCIDLib::enctOneMinute * 2
              )
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFiveMinutes"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val
              (
                L"kFiveMinutes", tCIDMacroEng::EConstTypes::Const, kCIDLib::enctOneMinute * 5
              )
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kOneHour"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val(L"kOneHour", tCIDMacroEng::EConstTypes::Const, kCIDLib::enctOneHour)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kOneDay"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val(L"kOneDay", tCIDMacroEng::EConstTypes::Const, kCIDLib::enctOneDay)
        )
    );

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Time"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add a stamp amount to our time
    {
        TMEngMethodInfo methiNew
        (
            L"AddStamp"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_AddStamp = c2AddMethodInfo(methiNew);
    }

    // Add the equal method (which supports the equality operator as well)
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", c2Id());
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get the current running millisecond counter, 32 and 64 bit versions
    {
        TMEngMethodInfo methiNew
        (
            L"GetCurMillis"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetCurMillis = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCurMillis64"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetCurMillis64 = c2AddMethodInfo(methiNew);
    }

    // Get the current running 100-ns counter
    {
        TMEngMethodInfo methiNew
        (
            L"GetCurStamp"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetCurStamp = c2AddMethodInfo(methiNew);
    }

    // Get the current running 100-ns counter plus the indicated millisecs
    {
        TMEngMethodInfo methiNew
        (
            L"GetCurStampPlusMSs"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Millis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCurStampPlusMSs = c2AddMethodInfo(methiNew);
    }

    // Get the current running 100-ns counter plus the indicated seconds
    {
        TMEngMethodInfo methiNew
        (
            L"GetCurStampPlusSecs"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Seconds", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCurStampPlusSecs = c2AddMethodInfo(methiNew);
    }

    // Get the date values out
    {
        TMEngMethodInfo methiNew
        (
            L"GetDateFlds"
            , m_c2EnumId_WeekDays
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"Year", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Month", m_c2EnumId_Months);
        methiNew.c2AddOutParm(L"Day", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetDateFlds = c2AddMethodInfo(methiNew);
    }

    // Get the julian date of the current stamp
    {
        TMEngMethodInfo methiNew
        (
            L"GetJulianDate"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetJulianDate = c2AddMethodInfo(methiNew);
    }

    // Get the stamp value of this object
    {
        TMEngMethodInfo methiNew
        (
            L"GetStamp"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetStamp = c2AddMethodInfo(methiNew);
    }

    // Get the time values out
    {
        TMEngMethodInfo methiNew
        (
            L"GetTimeFlds"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"Hours", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Minutes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Seconds", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetTimeFlds = c2AddMethodInfo(methiNew);
    }

    // Get the time zone offset in minutes
    {
        TMEngMethodInfo methiNew
        (
            L"GetTZOffset"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetTZOffset = c2AddMethodInfo(methiNew);
    }

    // Get the weekday value
    {
        TMEngMethodInfo methiNew
        (
            L"GetWeekday"
            , m_c2EnumId_WeekDays
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetWeekday = c2AddMethodInfo(methiNew);
    }

    //
    //  Returns true if the difference between this time and the (earlier) one
    //  passed is greater than a number of seconds.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"IsDiffGreater"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"EarlierTime", tCIDMacroEng::EIntrinsics::Card8);
        methiNew.c2AddInParm(L"TestSecs", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_IsDiffGreater = c2AddMethodInfo(methiNew);
    }

    // Returns true if the passed year is a leap year
    {
        TMEngMethodInfo methiNew
        (
            L"IsLeapYear"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"YearToCheck", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_IsLeapYear = c2AddMethodInfo(methiNew);
    }

    // Convert our stamp from Local to UTC
    {
        TMEngMethodInfo methiNew
        (
            L"LocalToUTC"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_LocalToUTC = c2AddMethodInfo(methiNew);
    }


    // Parse from an ISO-8601 formatted time stamp
    {
        TMEngMethodInfo methiNew
        (
            L"Parse8601"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToParse", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"AsLocal", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_Parse8601 = c2AddMethodInfo(methiNew);
    }


    // A generic type parser driven by a format description that has been previously set
    {
        TMEngMethodInfo methiNew
        (
            L"ParseFromPattern"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToParse", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ParseFromPattern = c2AddMethodInfo(methiNew);
    }


    // Set the date from individual values
    {
        TMEngMethodInfo methiNew
        (
            L"SetDate"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Year", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Month", m_c2EnumId_Months);
        methiNew.c2AddInParm(L"Day", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetDate = c2AddMethodInfo(methiNew);
    }

    // Set the date and time time from individual time values
    {
        TMEngMethodInfo methiNew
        (
            L"SetDateTime"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Year", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Month", m_c2EnumId_Months);
        methiNew.c2AddInParm(L"Day", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Hours", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Minutes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Seconds", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetDateTime = c2AddMethodInfo(methiNew);
    }

    // Set the default format string for this object
    {
        TMEngMethodInfo methiNew
        (
            L"SetDefFmt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"FormatStr", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetDefFmt = c2AddMethodInfo(methiNew);
    }

    // Set from a Julian date
    {
        TMEngMethodInfo methiNew
        (
            L"SetFromJulian"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"JulianDate", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetFromJulian = c2AddMethodInfo(methiNew);
    }

    // Set from a stamp value
    {
        TMEngMethodInfo methiNew
        (
            L"SetFromStamp"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"StampVal", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_SetFromStamp = c2AddMethodInfo(methiNew);
    }

    // Set the parse pattern
    {
        TMEngMethodInfo methiNew
        (
            L"SetParsePattern"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_TimeCompsVec);
        m_c2MethId_SetParsePattern = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetParsePattern2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_TimeCompsVec);
        methiNew.c2AddInParm(L"DateSep", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddInParm(L"TimeSep", tCIDMacroEng::EIntrinsics::Char);
        methiNew.c2AddInParm(L"TZSep", tCIDMacroEng::EIntrinsics::Char);
        m_c2MethId_SetParsePattern2 = c2AddMethodInfo(methiNew);
    }

    // Set one of the special times
    {
        TMEngMethodInfo methiNew
        (
            L"SetSpecial"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2EnumId_Special);
        m_c2MethId_SetSpecial = c2AddMethodInfo(methiNew);
    }

    // Set the time from individual time values
    {
        TMEngMethodInfo methiNew
        (
            L"SetTime"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Hours", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Minutes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Seconds", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Millis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetTime = c2AddMethodInfo(methiNew);
    }

    //
    //  Set to the difference between the passed stamp and our stamp.
    //  The passed stamp must be beyond our time or an exception is thrown.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"SetToDifference"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SubFrom", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_SetToDifference = c2AddMethodInfo(methiNew);
    }

    //
    //  Set to the sunrise or sunset of the data represented by this
    //  object. So the date parts won't change, but the time parts will.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"SetToSunrise"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Lat", tCIDMacroEng::EIntrinsics::Float8);
        methiNew.c2AddInParm(L"Long", tCIDMacroEng::EIntrinsics::Float8);
        methiNew.c2AddInParm(L"TZOffset", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_SetToSunrise = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetToSunset"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Lat", tCIDMacroEng::EIntrinsics::Float8);
        methiNew.c2AddInParm(L"Long", tCIDMacroEng::EIntrinsics::Float8);
        methiNew.c2AddInParm(L"TZOffset", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_SetToSunset = c2AddMethodInfo(methiNew);
    }

    // Sleep for the indicated time
    {
        TMEngMethodInfo methiNew
        (
            L"Sleep"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Millis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Sleep = c2AddMethodInfo(methiNew);
    }

    // Subtract a stamp from our time
    {
        TMEngMethodInfo methiNew
        (
            L"SubStamp"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_SubStamp = c2AddMethodInfo(methiNew);
    }

    // A constructor that takes a special time
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Time"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"InitVal", m_c2EnumId_Special);
        m_c2MethId_SpecialCtor = c2AddMethodInfo(methiNew);
    }

    // Convert our stamp from UTC to local
    {
        TMEngMethodInfo methiNew
        (
            L"UTCToLocal"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_UTCToLocal = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngTimeInfo::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&        meOwner
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngTimeVal(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngTimeInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngTimeInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngTimeVal& mecvActual = static_cast<TMEngTimeVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just sets it to now
        mecvActual.tmValue().SetTo(tCIDLib::ESpecialTimes::CurrentTime);
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly. It could fail if the stamp
        //  is not valid, so catch that.
        //
        try
        {
            TMEngTextOutStreamVal& mecvTarget
                    = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
            mecvTarget.strmTarget(meOwner) << mecvActual.tmValue();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrFmtFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_AddStamp)
    {
        const tCIDLib::TCard8 c8Add = meOwner.c8StackValAt(c4FirstInd);
        mecvActual.tmValue() += c8Add;
    }
     else if (c2MethId == m_c2MethId_Equal)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TMEngTimeVal& mecvComp
        (
            meOwner.mecvStackAtAs<const TMEngTimeVal>(c4FirstInd)
        );
        mecvRet.bValue(mecvActual.tmValue() == mecvComp.tmValue());
    }
     else if (c2MethId == m_c2MethId_GetCurMillis)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(TTime::c4Millis());
    }
     else if (c2MethId == m_c2MethId_GetCurMillis64)
    {
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(TTime::c8Millis());
    }
     else if (c2MethId == m_c2MethId_GetCurStamp)
    {
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(TTime::enctNow());
    }
     else if (c2MethId == m_c2MethId_GetCurStampPlusMSs)
    {
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(TTime::enctNowPlusMSs(meOwner.c4StackValAt(c4FirstInd)));
    }
     else if (c2MethId == m_c2MethId_GetCurStampPlusSecs)
    {
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(TTime::enctNowPlusSecs(meOwner.c4StackValAt(c4FirstInd)));
    }
     else if (c2MethId == m_c2MethId_GetDateFlds)
    {
        // Get the values out for conversion
        tCIDLib::TCard4 c4Year = 0;
        tCIDLib::EMonths eMonth = tCIDLib::EMonths::Count;
        tCIDLib::TCard4 c4Day = 0;
        tCIDLib::EWeekDays eDay = tCIDLib::EWeekDays::Count;
        try
        {
            eDay = mecvActual.tmValue().eAsDateInfo(c4Year, eMonth, c4Day);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrCvtFailed, errToCatch);
        }

        // Get all the out parms we need to fill
        TMEngCard4Val& mecvYear = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd);
        TMEngEnumVal& mecvMonth = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
        TMEngCard4Val& mecvDay = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 2);

        // And load them into the passed parms
        mecvYear.c4Value(c4Year);
        mecvMonth.c4Ordinal(tCIDLib::c4EnumOrd(eMonth));
        mecvDay.c4Value(c4Day);

        // Do the return value
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(tCIDLib::c4EnumOrd(eDay));
    }
     else if (c2MethId == m_c2MethId_GetJulianDate)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.tmValue().c4ToJulian());
    }
     else if (c2MethId == m_c2MethId_GetStamp)
    {
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.tmValue().enctTime());
    }
     else if (c2MethId == m_c2MethId_GetTimeFlds)
    {
        tCIDLib::TCard4 c4Hour = 0;
        tCIDLib::TCard4 c4Minute = 0;
        tCIDLib::TCard4 c4Second = 0;
        try
        {
            // Get the values out for conversion
            mecvActual.tmValue().c4AsTimeInfo(c4Hour, c4Minute, c4Second);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrCvtFailed, errToCatch);
        }

        // Get all the out parms we need to fill
        TMEngCard4Val& mecvHour = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd);
        TMEngCard4Val& mecvMinute = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);
        TMEngCard4Val& mecvSecond = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 2);

        // And update them
        mecvHour.c4Value(c4Hour);
        mecvMinute.c4Value(c4Minute);
        mecvSecond.c4Value(c4Second);
    }
     else if (c2MethId == m_c2MethId_GetTZOffset)
    {
        TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(TTime::i4Offset());
    }
     else if (c2MethId == m_c2MethId_GetWeekday)
    {
        tCIDLib::EWeekDays eDay = tCIDLib::EWeekDays::Count;
        try
        {
            eDay = mecvActual.tmValue().eDayOfWeek();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrCvtFailed, errToCatch);
        }

        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(tCIDLib::c4EnumOrd(eDay));
    }
     else if (c2MethId == m_c2MethId_IsDiffGreater)
    {
        const tCIDLib::TCard4 c4DiffLim = meOwner.c4StackValAt(c4FirstInd + 1);

        const tCIDLib::TEncodedTime enctOther = meOwner.c8StackValAt(c4FirstInd);
        const tCIDLib::TEncodedTime enctThis = mecvActual.tmValue().enctTime();

        tCIDLib::TCard4 c4SecsDiff;
        if (enctOther >= enctThis)
            c4SecsDiff = 0;
        else
            c4SecsDiff = tCIDLib::TCard4((enctThis - enctOther) / kCIDLib::enctOneSecond);

        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(c4SecsDiff >= c4DiffLim);
    }
     else if (c2MethId == m_c2MethId_IsLeapYear)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(TTime::bIsLeapYear(meOwner.c4StackValAt(c4FirstInd)));
    }
     else if (c2MethId == m_c2MethId_LocalToUTC)
    {
        try
        {
            mecvActual.tmValue().LocalToUTC();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Parse8601)
    {
        try
        {
            const tCIDLib::TBoolean bRes = mecvActual.tmValue().bParseFrom8601
            (
                meOwner.strStackValAt(c4FirstInd)
                , meOwner.bStackValAt(c4FirstInd + 1)
            );
            meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1).bValue(bRes);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrParse, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ParseFromPattern)
    {
        try
        {
            TString strErr;
            const tCIDLib::TBoolean bRes = mecvActual.bParseFromPattern
            (
                meOwner.strStackValAt(c4FirstInd), strErr
            );

            if (!bRes)
            {
                meOwner.SetException
                (
                    m_c2EnumId_Errors
                    , strClassPath()
                    , m_c4ErrPatternFailed
                    , m_pmeciErrors->strPartialName(m_c4ErrPatternFailed)
                    , strErr
                    , meOwner.c4CurLine()
                );

                // And throw the excpetion that represents a macro level exception
                throw TExceptException();
            }
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrParse, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetDate)
    {
        // The month one we have to do separately, since it's an enum
        TMEngEnumVal& mecvMonth = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);

        try
        {
            mecvActual.tmValue().FromDetails
            (
                meOwner.c4StackValAt(c4FirstInd)
                , tCIDLib::EMonths(mecvMonth.c4Ordinal())
                , meOwner.c4StackValAt(c4FirstInd + 2)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetDateTime)
    {
        // The month one we have to do separately, since it's an enum
        TMEngEnumVal& mecvMonth = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);

        try
        {
            mecvActual.tmValue().FromDetails
            (
                meOwner.c4StackValAt(c4FirstInd)
                , tCIDLib::EMonths(mecvMonth.c4Ordinal())
                , meOwner.c4StackValAt(c4FirstInd + 2)
                , meOwner.c4StackValAt(c4FirstInd + 3)
                , meOwner.c4StackValAt(c4FirstInd + 4)
                , meOwner.c4StackValAt(c4FirstInd + 5)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetDefFmt)
    {
        mecvActual.tmValue().strDefaultFormat(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SetFromJulian)
    {
        try
        {
            mecvActual.tmValue().FromJulian(meOwner.c4StackValAt(c4FirstInd));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetFromStamp)
    {
        try
        {
            mecvActual.tmValue() = meOwner.c8StackValAt(c4FirstInd);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_SetParsePattern)
          ||  (c2MethId == m_c2MethId_SetParsePattern2))
    {
        try
        {
            // We have to convert the CML vector of values to a C++ version
            TTime::TCompList fcolComps;
            const TMEngVectorVal& mecvComps = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd);
            const tCIDLib::TCard4 c4Count = mecvComps .c4ElemCount();

            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                // Get the element and cast to its real type
                const TMEngEnumVal& mecvCur
                (
                    static_cast<const TMEngEnumVal&>(mecvComps.mecvAt(meOwner, c4Index))
                );
                fcolComps.c4AddElement(tCIDLib::ETimeComps(mecvCur.c4Ordinal()));
            }

            // Now we can set it on the value
            if (c2MethId == m_c2MethId_SetParsePattern)
            {
                mecvActual.SetPattern(fcolComps);
            }
             else
            {
                mecvActual.SetPattern
                (
                    fcolComps
                    , meOwner.chStackValAt(c4FirstInd + 1)
                    , meOwner.chStackValAt(c4FirstInd + 2)
                    , meOwner.chStackValAt(c4FirstInd + 3)
                );
            }
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetSpecial)
    {
        TMEngEnumVal& mecvMonth = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        mecvActual.tmValue().SetTo(tCIDLib::ESpecialTimes(mecvMonth.c4Ordinal()));
    }
     else if (c2MethId == m_c2MethId_SetToDifference)
    {
        TTime& tmThis = mecvActual.tmValue();
        const tCIDLib::TEncodedTime enctDiff = meOwner.c8StackValAt(c4FirstInd);

        // If our stamp is beyond the passed stamp, that's an error
        if (tmThis.enctTime() > enctDiff)
            ThrowAnErr(meOwner, m_c4ErrNegTime);

        tmThis -= enctDiff;
    }
     else if (c2MethId == m_c2MethId_SetToSunrise)
    {
        try
        {
            // We get the long/lat as float values
            mecvActual.tmValue().SetToSunriseAt
            (
                meOwner.f8StackValAt(c4FirstInd)
                , meOwner.f8StackValAt(c4FirstInd + 1)
                , meOwner.i4StackValAt(c4FirstInd + 2)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetToSunset)
    {
        try
        {
            // We get the long/lat as float values
            mecvActual.tmValue().SetToSunsetAt
            (
                meOwner.f8StackValAt(c4FirstInd)
                , meOwner.f8StackValAt(c4FirstInd + 1)
                , meOwner.i4StackValAt(c4FirstInd + 2)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetTime)
    {
        try
        {
            mecvActual.tmValue().FromTimeDetails
            (
                meOwner.c4StackValAt(c4FirstInd)
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , meOwner.c4StackValAt(c4FirstInd + 2)
                , meOwner.c4StackValAt(c4FirstInd + 3)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Sleep)
    {
        meOwner.Sleep(meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SpecialCtor)
    {
        TMEngEnumVal& mecvSpec = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        mecvActual.tmValue
        (
            TTime(tCIDLib::ESpecialTimes(mecvSpec.c4Ordinal()))
        );
    }
     else if (c2MethId == m_c2MethId_SubStamp)
    {
        const tCIDLib::TCard8 c8Sub = meOwner.c8StackValAt(c4FirstInd);

        // Don't allow a negative time
        if (c8Sub > mecvActual.tmValue().enctTime())
            ThrowAnErr(meOwner, m_c4ErrNegTime);
        mecvActual.tmValue() -= c8Sub;
    }
     else if (c2MethId == m_c2MethId_UTCToLocal)
    {
        try
        {
            mecvActual.tmValue().UTCToLocal();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSetFailed, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngTimeInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngTimeInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngTimeInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errCaught)
{
    // If in verbose mode we'll log the C++ error
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


