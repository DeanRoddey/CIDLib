//
// FILE NAME: CIDLib_Time.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/05/1993
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
//  This file implements the basic time stamp class, TTime.
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


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TTime,TObject)


namespace
{
    namespace CIDLib_Time
    {
        // -----------------------------------------------------------------------
        //  Local data
        //
        //  atomInitDone
        //      This is a flag used to trigger all of the lazy evaluation that is
        //      required for this class. Since there are multiple lazy statics,
        //      this is easier.
        //
        //  pfcolXXX
        //      Some prefab time component parsing format lists which will be
        //      lazily faulted in as required.
        //
        //  pstrXXX
        //      Some prefab format strings which will be lazily faulted in as
        //      required.
        // -----------------------------------------------------------------------
        TAtomicFlag         atomInitDone;
        TTimeCompList*      pfcol12HHMM = nullptr;
        TTimeCompList*      pfcol24HHMMSS = nullptr;
        TTimeCompList*      pfcol24HHMM = nullptr;
        TTimeCompList*      pfcolCTime = nullptr;
        TTimeCompList*      pfcolDDMMYY = nullptr;
        TTimeCompList*      pfcolDDMMYYYY = nullptr;
        TTimeCompList*      pfcolDTStamp = nullptr;
        TTimeCompList*      pfcolFullDate = nullptr;
        TTimeCompList*      pfcolISO8601NTZ = nullptr;
        TTimeCompList*      pfcolMMDDYY = nullptr;
        TTimeCompList*      pfcolMMDDYYYY = nullptr;
        TTimeCompList*      pfcolRFC822 = nullptr;
        TTimeCompList*      pfcolYYMMDD = nullptr;
        TTimeCompList*      pfcolYYYYMMDD = nullptr;
        TTimeCompList*      pfcolYYYYMMDD24HHMM = nullptr;
        TString*            pstr24HHMM = nullptr;
        TString*            pstr24HHMMSS = nullptr;
        TString*            pstr24HM = nullptr;
        TString*            pstrCTime = nullptr;
        TString*            pstrDTStamp = nullptr;
        TString*            pstrDDMMYY = nullptr;
        TString*            pstrDDMMYYYY = nullptr;
        TString*            pstrFullDate = nullptr;
        TString*            pstrHHMM = nullptr;
        TString*            pstrHHMMap = nullptr;
        TString*            pstrHHMMSS = nullptr;
        TString*            pstrHM = nullptr;
        TString*            pstrMediaTime = nullptr;
        TString*            pstrMMSS = nullptr;
        TString*            pstrMMDD_24HHMM = nullptr;
        TString*            pstrMMDD_24HHMMSS = nullptr;
        TString*            pstrMMDDYY = nullptr;
        TString*            pstrMMDDYYYY = nullptr;
        TString*            pstrMMDD_HHMM = nullptr;
        TString*            pstrMMDD_HHMMSS = nullptr;
        TString*            pstrISO8601Basic = nullptr;
        TString*            pstrYYMMDD = nullptr;
        TString*            pstrYYYYMMDD = nullptr;
        TString*            pstrYYYYMMDD_NoSep = nullptr;


        // -----------------------------------------------------------------------
        //  Some info we cache about time zone settings, to optimize some things
        //  a bit.
        // -----------------------------------------------------------------------
        TAtomicInt          atomLastTZ;
        TString*            pstrTZName = nullptr;


        // -----------------------------------------------------------------------
        //  Used in the sunrise/sunset calculations below
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TFloat8 f8Rads = kCIDLib::f8PI / 180.0;
        constexpr tCIDLib::TFloat8 f8Degs = 180.0 / kCIDLib::f8PI;
    }
}



// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static tCIDLib::TFloat8 f8FNDay(const   tCIDLib::TInt4  i4Year
                                , const tCIDLib::TInt4  i4Month
                                , const tCIDLib::TInt4  i4Day)
{
    tCIDLib::TInt4 i4Tmp = -7 * (i4Year + (i4Month + 9) / 12) / 4
                            + 275 * i4Month / 9 + i4Day;
    i4Tmp += i4Year * 367;
    return tCIDLib::TFloat8(i4Tmp) - 730531.5 + 0.5;
}


// Returns an angle between zero and 2*PI
static tCIDLib::TFloat8 f8CalcAngle(const tCIDLib::TFloat8 f8X)
{
    const tCIDLib::TFloat8 f8B = f8X / kCIDLib::f8PIx2;
    tCIDLib::TFloat8 f8A = kCIDLib::f8PIx2 * (f8B - tCIDLib::TInt4(f8B));
    if (f8A < 0)
        f8A = kCIDLib::f8PIx2 + f8A;
    return f8A;
}


// Calc the sun angle for the lat/declination
static tCIDLib::TFloat8
f8CalcHourAngle(const tCIDLib::TFloat8 f8Lat, const tCIDLib::TFloat8 f8Decl)
{
    const tCIDLib::TFloat8 f8SunDeg = 0.53;
    const tCIDLib::TFloat8 f8Refract = 34.0 / 60.0;

    tCIDLib::TFloat8 f8DFo = CIDLib_Time::f8Rads * (f8SunDeg + f8Refract);
    if (f8Lat < 0.0)
        f8DFo = -f8DFo;

    tCIDLib::TFloat8 f8Fo = TMathLib::f8Tangent(f8Decl + f8DFo)
                            * TMathLib::f8Tangent(f8Lat * CIDLib_Time::f8Rads);
    if (f8Fo > 0.99999)
        f8Fo = 1.0;
    f8Fo = TMathLib::f8ArcSine(f8Fo) + kCIDLib::f8PI / 2.0;

    return f8Fo;
}


// Calc the sun angle for the twilight times
static tCIDLib::TFloat8
f8CalcTwilightAngle(const tCIDLib::TFloat8 f8Lat, const tCIDLib::TFloat8 f8Decl)
{
    tCIDLib::TFloat8 f8DF1 = CIDLib_Time::f8Rads * 6.0;
    if (f8Lat < 0.0)
        f8DF1 = -f8DF1;

    tCIDLib::TFloat8 f8Fi = TMathLib::f8Tangent(f8Decl + f8DF1)
                            * TMathLib::f8Tangent(f8Lat * CIDLib_Time::f8Rads);
    if (f8Fi > 0.99999)
        f8Fi = 1.0;

    f8Fi = TMathLib::f8ArcSine(f8Fi) + kCIDLib::f8PI / 2.0;
    return f8Fi;
}


// Calc ecliptic longitude of the sun for the day calculated by f8FNDay above
static tCIDLib::TFloat8
f8SunLong(  const   tCIDLib::TFloat8    f8Day
            ,       tCIDLib::TFloat8&   f8Long
            ,       tCIDLib::TFloat8&   f8Anom)
{
    // Mean longitude
    f8Long = f8CalcAngle
    (
        280.461 * CIDLib_Time::f8Rads + 0.9856474 * CIDLib_Time::f8Rads * f8Day
    );

    // Mean anomoly
    f8Anom = f8CalcAngle
    (
        357.528 * CIDLib_Time::f8Rads + 0.9856003 * CIDLib_Time::f8Rads * f8Day
    );

    return f8CalcAngle
    (
        f8Long + 1.915 * CIDLib_Time::f8Rads * TMathLib::f8Sine(f8Anom)
        + 0.02 * CIDLib_Time::f8Rads * TMathLib::f8Sine(2 * f8Anom)
    );
}


// Complete the type we had to forward reference
using TTimeCompList = TFundVector<tCIDLib::ETimeComps, tCIDLib::TCard4>;



// ---------------------------------------------------------------------------
//   CLASS: TTime
//  PREFIX: tm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTime: Public, static methods
//
//  NOTE:   Some of these are just returning already stored data, or they are
//          failsafe, so they will not throw any kernel exceptions, or just
//          don't have any error states. So we don't have to wrap them and
//          translate kernel exceptions to CIDLib level exceptions.
// ---------------------------------------------------------------------------

// Return the daylight savings time status
tCIDLib::TBoolean TTime::bDST()
{
    tCIDLib::TBoolean bDSTState;
    if (!TKrnlTimeStamp::bIsDST(bDSTState))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_GetTZInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return bDSTState;
}


tCIDLib::TBoolean TTime::bIsLeapYear(const tCIDLib::TCard4 c4ToCheck)
{
    return (!(c4ToCheck % 4) && (c4ToCheck % 100)) || !(c4ToCheck % 400);
}


// Converts from the passed Y/M/D to a Julian day value
tCIDLib::TCard4 TTime::c4CvtToJulian(const  tCIDLib::TCard4     c4Year
                                    , const tCIDLib::EMonths    eMonth
                                    , const tCIDLib::TCard4     c4Day)
{
    CIDAssert(c4Day != 0, L"Days are 1 based");

    tCIDLib::TCard4 c4D = c4Day;
    tCIDLib::TCard4 c4M = tCIDLib::TCard4(eMonth) + 1;
    tCIDLib::TCard4 c4Y = c4Year;

    if (c4M <= 2)
    {
        c4Y--;
        c4M += 12;
    }

    tCIDLib::TCard4 c4B = 0;
    if ((c4Y < 1582)
    ||  ((c4Y == 1582) && ((c4M < 10) || ((c4M == 10) && (c4D <= 4)))))
    {
        c4B = 0;
    }
     else
    {
        const tCIDLib::TCard4 c4A = c4Y / 100;
        c4B = (2 - c4A) + (c4A / 4);
    }

    // Calculate the full floating point date
    const tCIDLib::TFloat8 f8Res
    (
        tCIDLib::TCard4(365.25 * (c4Y + 4716))
        + tCIDLib::TCard4(30.6001 * (c4M + 1))
        + c4D + c4B - 1524.5
    );

    // Get the integral part out
    tCIDLib::TFloat8 f8Int;
    TMathLib::f8Split(f8Res, f8Int);

    //
    //  Convert it to a card, adding .5 in case it's some kind of .999999...
    //  type of value in binary, and add 1. The one is because the Julian
    //  day actually starts at noon, but we aren't adding any hour component,
    //  making it the same as hour zero. That makes it come out for the
    //  previous day.
    //
    return tCIDLib::TCard4(f8Int + 0.5) + 1;
}


tCIDLib::TCard4
TTime::c4MaxDaysForMonth(const  tCIDLib::EMonths    eMonth
                        , const tCIDLib::TCard4     c4Year)
{
    return TKrnlTimeStamp::c4MaxDayForMonth(eMonth, c4Year);
}


// Return current millis in 32 and 64 bit form
tCIDLib::TCard4 TTime::c4Millis()
{
    return TKrnlTimeStamp::c4Millis();
}

tCIDLib::TCard8 TTime::c8Millis()
{
    return TKrnlTimeStamp::c8Millis();
}


// This returns the higher performance timer, which is micro seconds
tCIDLib::TCard8 TTime::c8HPTimerUS()
{
    return TKrnlHPTimer::c8GetTicks();
}


// Just a passthrough to the kernel level
tCIDLib::TVoid
TTime::CurNTPTime(  COP     tCIDLib::TCard4&    c4Secs
                    , COP   tCIDLib::TCard4&    c4Fract
                    , const tCIDLib::TBoolean   bNWOrder)
{
    TKrnlTimeStamp::CurNTPTime(c4Secs, c4Fract, bNWOrder);
}


// Return the current 100-ns time stamp
tCIDLib::TEncodedTime TTime::enctNow()
{
    return TKrnlTimeStamp::enctNow();
}


//
//  Return the current 100-ns time stamp plus a number of millis, secs,
//  minutes, days, hours, etc...
//
tCIDLib::TEncodedTime TTime::enctNowPlusDays(const tCIDLib::TCard4 c4Days)
{
    return TKrnlTimeStamp::enctNow() + (c4Days * kCIDLib::enctOneDay);
}

tCIDLib::TEncodedTime TTime::enctNowPlusHours(const tCIDLib::TCard4 c4Hours)
{
    return TKrnlTimeStamp::enctNow() + (c4Hours * kCIDLib::enctOneHour);
}

tCIDLib::TEncodedTime TTime::enctNowPlusMSs(const tCIDLib::TCard4 c4Millis)
{
    return TKrnlTimeStamp::enctNow() + (c4Millis * kCIDLib::enctOneMilliSec);
}

tCIDLib::TEncodedTime TTime::enctNowPlusMins(const tCIDLib::TCard4 c4Mins)
{
    return TKrnlTimeStamp::enctNow() + (c4Mins * kCIDLib::enctOneMinute);
}

tCIDLib::TEncodedTime TTime::enctNowPlusSecs(const tCIDLib::TCard4 c4Secs)
{
    return TKrnlTimeStamp::enctNow() + (c4Secs * kCIDLib::enctOneSecond);
}


// Return the current 100-ns time stamp in UTC time
tCIDLib::TEncodedTime TTime::enctNowUTC()
{
    return TKrnlTimeStamp::enctNowUTC();
}


// Converts from a passed julian date to a y/m/d format
tCIDLib::TVoid
TTime::CvtFromJulian(const  tCIDLib::TCard4     c4Julian
                    ,       tCIDLib::TCard4&    c4Year
                    ,       tCIDLib::EMonths&   eMonth
                    ,       tCIDLib::TCard4&    c4Day)
{
    tCIDLib::TCard4 c4A = c4Julian;
    if (c4A >= 2299161)
    {
        tCIDLib::TCard4 c4Tmp = tCIDLib::TCard4((c4Julian - 1867216.25) / 36524.25);
        c4A = c4Julian + 1 + c4Tmp - (c4Tmp >> 2);
    }
    tCIDLib::TCard4 c4B = c4A + 1524;
    tCIDLib::TCard4 c4C = tCIDLib::TCard4((c4B - 122.1) / 365.25);
    tCIDLib::TCard4 c4D = tCIDLib::TCard4(c4C * 365.25);
    tCIDLib::TCard4 c4E = tCIDLib::TCard4((c4B - c4D) / 30.6001);

    // Calc the day (1 based)
    c4Day = tCIDLib::TCard4((c4B - c4D) - (c4E * 30.6001)) + 1;

    // Calc the month (our months are zero based)
    if (c4E > 13)
        eMonth = tCIDLib::EMonths(c4E - 14);
    else
        eMonth = tCIDLib::EMonths(c4E - 2);

    if ((eMonth == tCIDLib::EMonths::February) && (c4Day > 29))
        c4Day = 29;
    if ((eMonth == tCIDLib::EMonths::February) && (c4Day == 29) && (c4E == 3))
        c4Year = c4C - 4716;
    else if (eMonth > tCIDLib::EMonths::February)
        c4Year = c4C - 4716;
    else
        c4Year = c4C - 4715;
}


// Convert from a Julian date to a year, month, and day
tCIDLib::TVoid
TTime::CvtFromJulian(const  tCIDLib::TFloat8    f8Julian
                    ,       tCIDLib::TCard4&    c4Year
                    ,       tCIDLib::EMonths&   eMonth
                    ,       tCIDLib::TCard4&    c4Day)
{
    //
    //  Just call the float version, insuring that anything from x.5 and up
    //  rounds up.
    //
    CvtFromJulian(tCIDLib::TCard4(f8Julian + 0.5), c4Year, eMonth, c4Day);
}


// Provide access the the pre-fab time parsing component lists
const TTimeCompList& TTime::fcol12HHMM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcol12HHMM;
}

const TTimeCompList& TTime::fcol24HHMM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcol24HHMM;
}

const TTimeCompList& TTime::fcol24HHMMSS()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcol24HHMMSS;
}

const TTimeCompList& TTime::fcolCTime()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolCTime;
}

const TTimeCompList& TTime::fcolDDMMYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolDDMMYY;
}

const TTimeCompList& TTime::fcolDDMMYYYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolDDMMYYYY;
}

const TTimeCompList& TTime::fcolDTStamp()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolDTStamp;
}

const TTimeCompList& TTime::fcolFullDate()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolFullDate;
}

const TTimeCompList& TTime::fcolISO8601NTZ()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolISO8601NTZ;
}

const TTimeCompList& TTime::fcolMMDDYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolMMDDYY;
}

const TTimeCompList& TTime::fcolMMDDYYYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolMMDDYYYY;
}

const TTimeCompList& TTime::fcolRFC822()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolRFC822;
}

const TTimeCompList& TTime::fcolYYMMDD()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolYYMMDD;
}

const TTimeCompList& TTime::fcolYYYYMMDD()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolYYYYMMDD;
}

const TTimeCompList& TTime::fcolYYYYMMDD24HHMM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pfcolYYYYMMDD24HHMM;
}


// Return the local system time zone offset
tCIDLib::TInt4 TTime::i4Offset()
{
    tCIDLib::TInt4 i4Ret;
    if (!TKrnlTimeStamp::bTZOffset(i4Ret))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_GetTZInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return i4Ret;
}


// Convert from local time to native UTC
tCIDLib::TVoid
TTime::LocalToNative(const  tCIDLib::TEncodedTime&  enctLocal
                    ,       tCIDLib::TEncodedTime&  enctNative)
{
    if (!TKrnlTimeStamp::bLocalToNative(enctLocal, enctNative))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_LocalToUTC
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// Convert from local time to UTC
tCIDLib::TVoid
TTime::LocalToUTC(  const   tCIDLib::TEncodedTime&  enctLocal
                    ,       tCIDLib::TEncodedTime&  enctUTC)
{
    if (!TKrnlTimeStamp::bLocalToUTC(enctLocal, enctUTC))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_LocalToUTC
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// Accesses the various pre-fab format strings
const TString& TTime::strCTime()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrCTime;
}

const TString& TTime::str24HHMM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstr24HHMM;
}

const TString& TTime::str24HHMMSS()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstr24HHMMSS;
}

const TString& TTime::str24HM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstr24HM;
}

const TString& TTime::strDDMMYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrDDMMYY;
}

const TString& TTime::strDDMMYYYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrDDMMYYYY;
}

const TString& TTime::strDTStamp()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrDTStamp;
}

const TString& TTime::strFullDate()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrFullDate;
}

const TString& TTime::strHHMM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrHHMM;
}

const TString& TTime::strHHMMap()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrHHMMap;
}

const TString& TTime::strHHMMSS()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrHHMMSS;
}

const TString& TTime::strHM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrHM;
}

const TString& TTime::strMediaTime()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMediaTime;
}

const TString& TTime::strMMSS()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMMSS;
}

const TString& TTime::strMMDD_24HHMM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMMDD_24HHMM;
}

const TString& TTime::strMMDD_24HHMMSS()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMMDD_24HHMMSS;
}

const TString& TTime::strMMDD_HHMM()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMMDD_HHMM;
}

const TString& TTime::strMMDD_HHMMSS()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMMDD_HHMMSS;
}

const TString& TTime::strMMDDYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMMDDYY;
}

const TString& TTime::strMMDDYYYY()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrMMDDYYYY;
}

const TString& TTime::strISO8601Basic()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrISO8601Basic;
}

const TString& TTime::strTimeZoneName()
{
    //
    //  We optimize here so that we don't have to return a copy of a
    //  string. We get the time zone offset and name and store them.
    //  As long as the offset doesn't change, we don't have to update
    //  the name.
    //
    static TAtomicFlag atomInitDone;
    if (!atomInitDone)
    {
        TBaseLock lockInit;
        if (!atomInitDone)
        {
            tCIDLib::TInt4 i4TZOfs = 0;
            tCIDLib::TZStr64 szTZName = L"";
            if (!TKrnlTimeStamp::bTZOffset(i4TZOfs)
            ||  !TKrnlTimeStamp::bTZName(szTZName, tCIDLib::c4MaxBufChars(szTZName)))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcTime_GetTZInfo
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            CIDLib_Time::atomLastTZ.SetValue(i4TZOfs);
            CIDLib_Time::pstrTZName = new TString(szTZName);
            atomInitDone.Set();
        }
    }

    // See if the offset has changed. If so, update the name
    const tCIDLib::TInt4 i4NewOfs = i4Offset();
    if (i4NewOfs != CIDLib_Time::atomLastTZ.i4Value())
    {
        TBaseLock lockInit;
        if (i4NewOfs != CIDLib_Time::atomLastTZ.i4Value())
        {
            tCIDLib::TZStr64 szTZName;
            if (!TKrnlTimeStamp::bTZName(szTZName, tCIDLib::c4MaxBufChars(szTZName)))
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcTime_GetTZInfo
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            *CIDLib_Time::pstrTZName = szTZName;
            CIDLib_Time::atomLastTZ.SetValue(i4NewOfs);
        }
    }
    return *CIDLib_Time::pstrTZName;
}

const TString& TTime::strYYMMDD()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrYYMMDD;
}

const TString& TTime::strYYYYMMDD()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrYYYYMMDD;
}

const TString& TTime::strYYYYMMDD_NoSep()
{
    if (!CIDLib_Time::atomInitDone)
        LazyInit();
    return *CIDLib_Time::pstrYYYYMMDD_NoSep;
}

//
//  Calculates the sunrise and sunset time for the passed year, month and
//  day. It returns them as the hours and minutes into the day that they will
//  occur (i.e. from midnight.)
//
tCIDLib::TVoid
TTime::CalcRiseSetTimes(const   TTime&              tmOn
                        , const tCIDLib::TFloat8    f8Latitude
                        , const tCIDLib::TFloat8    f8Longitude
                        ,       tCIDLib::TCard4&    c4RiseHour
                        ,       tCIDLib::TCard4&    c4RiseMin
                        ,       tCIDLib::TCard4&    c4SetHour
                        ,       tCIDLib::TCard4&    c4SetMin)
{
    // Just call the other version breaking out the day values
    tCIDLib::TCard4     c4Year;
    tCIDLib::EMonths    eMonth;
    tCIDLib::TCard4     c4Day;
    tmOn.eAsDateInfo(c4Year, eMonth, c4Day);

    CalcRiseSetTimes
    (
        c4Year
        , eMonth
        , c4Day
        , i4Offset() / 60.0
        , f8Latitude
        , f8Longitude
        , c4RiseHour
        , c4RiseMin
        , c4SetHour
        , c4SetMin
    );
}


tCIDLib::TVoid
TTime::CalcRiseSetTimes(const   TTime&              tmOn
                        , const tCIDLib::TFloat8    f8TZOffset
                        , const tCIDLib::TFloat8    f8Latitude
                        , const tCIDLib::TFloat8    f8Longitude
                        ,       tCIDLib::TCard4&    c4RiseHour
                        ,       tCIDLib::TCard4&    c4RiseMin
                        ,       tCIDLib::TCard4&    c4SetHour
                        ,       tCIDLib::TCard4&    c4SetMin)
{
    // Just call the other version breaking out the day values
    tCIDLib::TCard4     c4Year;
    tCIDLib::EMonths    eMonth;
    tCIDLib::TCard4     c4Day;
    tmOn.eAsDateInfo(c4Year, eMonth, c4Day);

    CalcRiseSetTimes
    (
        c4Year
        , eMonth
        , c4Day
        , f8TZOffset
        , f8Latitude
        , f8Longitude
        , c4RiseHour
        , c4RiseMin
        , c4SetHour
        , c4SetMin
    );
}


tCIDLib::TVoid
TTime::CalcRiseSetTimes(const   tCIDLib::TCard4     c4Year
                        , const tCIDLib::EMonths    eMonth
                        , const tCIDLib::TCard4     c4Day
                        , const tCIDLib::TFloat8    f8TZOffset
                        , const tCIDLib::TFloat8    f8Latitude
                        , const tCIDLib::TFloat8    f8Longitude
                        ,       tCIDLib::TCard4&    c4RiseHour
                        ,       tCIDLib::TCard4&    c4RiseMin
                        ,       tCIDLib::TCard4&    c4SetHour
                        ,       tCIDLib::TCard4&    c4SetMin)
{
    tCIDLib::TFloat8 f8Day = f8FNDay(c4Year, tCIDLib::c4EnumOrd(eMonth) + 1, c4Day);

    tCIDLib::TFloat8 f8MeanLong;
    tCIDLib::TFloat8 f8MeanAnom;
    const tCIDLib::TFloat8 f8Lambda = f8SunLong(f8Day, f8MeanLong, f8MeanAnom);

    const tCIDLib::TFloat8 f8Obliq = 23.439 * CIDLib_Time::f8Rads
                                    - 0.0000004 * CIDLib_Time::f8Rads * f8Day;

    const tCIDLib::TFloat8 f8Alpha = TMathLib::f8ArcTangentXY
    (
        TMathLib::f8Cosine(f8Lambda)
        , TMathLib::f8Cosine(f8Obliq) * TMathLib::f8Sine(f8Lambda)
    );
    const tCIDLib::TFloat8 f8Delta = TMathLib::f8ArcSine
    (
        TMathLib::f8Sine(f8Obliq) * TMathLib::f8Sine(f8Lambda)
    );

    tCIDLib::TFloat8 f8LL = f8MeanLong - f8Alpha;
    if (f8MeanLong < kCIDLib::f8PI)
        f8LL += kCIDLib::f8PIx2;

    const tCIDLib::TFloat8 f8Eq
    (
        1440.0 * (1.0 - f8LL / kCIDLib::f8PIx2)
    );

    const tCIDLib::TFloat8 f8HA = f8CalcHourAngle(f8Latitude, f8Delta);

    // Don't need it now, but twilight hour
//    const tCIDLib::TFloat8 f8HB = f8CalcTwilightAngle(f8Latitude, f8Delta);
//    tCIDLib::TFloat8 f8TWX = f8HB - f8HA;
//    f8TWX = 12.0 * f8TWX / kCIDLib::f8PI;

    tCIDLib::TFloat8 f8DayLen(CIDLib_Time::f8Degs * f8HA / 7.5);
    if (f8DayLen < 0.0001)
        f8DayLen = 0.0;

    // Calc the basic rise/set times
    tCIDLib::TFloat8 f8RiseTm
    (
        12.0 - 12.0 * f8HA / kCIDLib::f8PI + f8TZOffset
        - f8Longitude / 15.0 + f8Eq / 60.0
    );

    tCIDLib::TFloat8 f8SetTm
    (
        12.0 + 12.0 * f8HA / kCIDLib::f8PI + f8TZOffset
        - f8Longitude / 15.0 + f8Eq / 60.0
    );

    if (f8RiseTm >= 24.0)
        f8RiseTm -= 24.0;

    if (f8SetTm >= 24.0)
        f8SetTm -= 24.0;

    // Don't need it now, but highest sun altitude
//    tCIDLib::TFloat8 f8AltMax = 90.0 * f8Delta * CIDLib_Time::f8Degs - f8Latitude;
//    if (f8Latitude < f8Delta * CIDLib_Time::f8Degs)
//        f8AltMax = 180 - f8AltMax;


    // Break out the hours/minus from the floats to the Card4 return values
    c4RiseHour = tCIDLib::TCard4(f8RiseTm);
    c4RiseMin = tCIDLib::TCard4((f8RiseTm - tCIDLib::TFloat8(c4RiseHour)) * 60);

    c4SetHour = tCIDLib::TCard4(f8SetTm);
    c4SetMin = tCIDLib::TCard4((f8SetTm - tCIDLib::TFloat8(c4SetHour)) * 60);

    // Test them for validity
    if (((c4RiseHour > 23) || (c4SetHour > 23))
    ||  ((c4RiseMin > 59) || (c4SetMin > 59)))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_BadRiseSetCalc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Range
        );
    }
}


// Convert from UTC to local time
tCIDLib::TVoid
TTime::UTCToLocal(  const   tCIDLib::TEncodedTime&  enctUTC
                    ,       tCIDLib::TEncodedTime&  enctLocal)
{
    if (!TKrnlTimeStamp::bUTCToLocal(enctUTC, enctLocal))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_UTCToLocal
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}



// ---------------------------------------------------------------------------
//  TTime: Constructors and Destructor
// ---------------------------------------------------------------------------
TTime::TTime() :

    m_pstrDefFormat(nullptr)
{
}

TTime::TTime(const TStringView& strvDefFormat) :

    m_pstrDefFormat(new TString(strvDefFormat))
{
}

TTime::TTime(const tCIDLib::ESpecialTimes eSpecial) :

    m_pstrDefFormat(nullptr)
{
    if (!m_ktmsThis.bSetTo(eSpecial))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_UnknownSpecialTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TInteger(tCIDLib::i4EnumOrd(eSpecial))
        );
    }

    switch(eSpecial)
    {
        case tCIDLib::ESpecialTimes::Base :
            strDefaultFormat(strCTime());
            break;

        case tCIDLib::ESpecialTimes::CurrentDate :
            strDefaultFormat(TLocale::strDateFormat());
            break;

        case tCIDLib::ESpecialTimes::CurrentTime :
            strDefaultFormat(TLocale::strTimeFormat());
            break;

        case tCIDLib::ESpecialTimes::NoonToday :
            strDefaultFormat(TLocale::strTimeFormat());
            break;

        case tCIDLib::ESpecialTimes::CurrentUTC :
            strDefaultFormat(strCTime());
            break;

        default :
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcTime_UnknownSpecialTime
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TInteger(tCIDLib::i4EnumOrd(eSpecial))
            );
            break;
    }
}

TTime::TTime(const TTime& tmToCopy) :

    m_ktmsThis(tmToCopy.m_ktmsThis)
    , m_pstrDefFormat(nullptr)
{
    // If the source had a default format we have to do ours
    if (tmToCopy.m_pstrDefFormat)
        m_pstrDefFormat = new TString(*tmToCopy.m_pstrDefFormat);
}

TTime::TTime(   const   tCIDLib::TCard4     c4Year
                , const tCIDLib::EMonths    eMonth
                , const tCIDLib::TCard4     c4Day
                , const tCIDLib::TCard4     c4Hours
                , const tCIDLib::TCard4     c4Minutes
                , const tCIDLib::TCard4     c4Seconds
                , const tCIDLib::TCard4     c4Millis) :

    m_pstrDefFormat(nullptr)
{
    if (!m_ktmsThis.bFromDetails
    (
        c4Year
        , eMonth
        , c4Day
        , c4Hours
        , c4Minutes
        , c4Seconds
        , c4Millis))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_BadDetails
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
}

TTime::TTime(const tCIDLib::TEncodedTime enctTime) :

    m_pstrDefFormat(nullptr)
{
    StoreValue(enctTime);
}

TTime::~TTime()
{
    delete m_pstrDefFormat;
    m_pstrDefFormat = nullptr;
}



// ---------------------------------------------------------------------------
//  TTime: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTime::operator!=(const TTime& tmToTest) const
{
    return !operator==(tmToTest);
}

tCIDLib::TBoolean operator<(const TTime& tmLHS, const TTime& tmRHS)
{
    return tmLHS.m_ktmsThis.enctTime() < tmRHS.m_ktmsThis.enctTime();
}

tCIDLib::TBoolean operator<=(const TTime& tmLHS, const TTime& tmRHS)
{
    return tmLHS.m_ktmsThis.enctTime() <= tmRHS.m_ktmsThis.enctTime();
}

tCIDLib::TBoolean operator>(const TTime& tmLHS, const TTime& tmRHS)
{
    return tmLHS.m_ktmsThis.enctTime() > tmRHS.m_ktmsThis.enctTime();
}

tCIDLib::TBoolean operator>=(const TTime& tmLHS, const TTime& tmRHS)
{
    return tmLHS.m_ktmsThis.enctTime() >= tmRHS.m_ktmsThis.enctTime();
}

tCIDLib::TBoolean TTime::operator==(const TTime& tmToTest) const
{
    if (this == &tmToTest)
        return kCIDLib::True;

    if (m_ktmsThis.enctTime() != tmToTest.m_ktmsThis.enctTime())
        return kCIDLib::False;

    //
    //  If the two default format pointers are the same, then they have to
    //  both be null (they couldn't be the same object but that would also
    //  mean the same thing, i.e. they would be equal.)
    //
    //  So we test that they are not equal. If not, then we see if either
    //  one is null, in which case they cannot be equal. Else we compare the
    //  text of the strings.
    //
    if (m_pstrDefFormat != tmToTest.m_pstrDefFormat)
    {
        if (!m_pstrDefFormat || !tmToTest.m_pstrDefFormat)
            return kCIDLib::False;

        if (*m_pstrDefFormat != *tmToTest.m_pstrDefFormat)
            return kCIDLib::False;
    }

    // Everything matched, so return true
    return kCIDLib::True;
}


TTime& TTime::operator=(const TTime& tmToAssign)
{
    if (&tmToAssign == this)
        return *this;

    m_ktmsThis   = tmToAssign.m_ktmsThis;

    // Clean up any existing format string
    delete m_pstrDefFormat;
    m_pstrDefFormat = nullptr;

    //
    //  If the source has a default string, then we need to make our own
    //  copy of it.
    //
    if (tmToAssign.m_pstrDefFormat)
        m_pstrDefFormat = new TString(*tmToAssign.m_pstrDefFormat);

    return *this;
}


TTime& TTime::operator=(const tCIDLib::TEncodedTime& enctToSet)
{
    StoreValue(enctToSet);
    return *this;
}


tCIDLib::TVoid TTime::operator+=(const TTime& tmToAdd)
{
    const tCIDLib::TEncodedTime enctRes = m_ktmsThis.enctTime()
                                          + tmToAdd.m_ktmsThis.enctTime();
    StoreValue(enctRes);
}

tCIDLib::TVoid TTime::operator+=(const tCIDLib::TEncodedTime& enctToAdd)
{
    const tCIDLib::TEncodedTime enctRes = m_ktmsThis.enctTime() + enctToAdd;
    StoreValue(enctRes);
}


tCIDLib::TVoid TTime::operator-=(const TTime& tmToSubtract)
{
    if (tmToSubtract.m_ktmsThis.enctTime() > m_ktmsThis.enctTime())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_Underflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    StoreValue(m_ktmsThis.enctTime() - tmToSubtract.m_ktmsThis.enctTime());
}

tCIDLib::TVoid TTime::operator-=(const tCIDLib::TEncodedTime& enctToSub)
{
    if (enctToSub > m_ktmsThis.enctTime())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_Underflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    StoreValue(m_ktmsThis.enctTime() - enctToSub);
}


TTime operator+(const TTime& tmLHS, const TTime& tmRHS)
{
    const tCIDLib::TEncodedTime enctRes = tmLHS.m_ktmsThis.enctTime()
                                          + tmRHS.m_ktmsThis.enctTime();
    return TTime(enctRes);
}

TTime operator-(const TTime& tmLHS, const TTime& tmRHS)
{
    if (tmRHS.m_ktmsThis.enctTime() > tmLHS.m_ktmsThis.enctTime())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_Underflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    return TTime(tmLHS.m_ktmsThis.enctTime() - tmRHS.m_ktmsThis.enctTime());
}


// ---------------------------------------------------------------------------
//  TTime: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Indicate if this time is in the PM, and obviously by inference in the AM
tCIDLib::TBoolean TTime::bInPM() const
{
    tCIDLib::TCard4 c4Hour;
    tCIDLib::TCard4 c4Minute;
    tCIDLib::TCard4 c4Second;
    tCIDLib::TCard4 c4Millis;
    if (!m_ktmsThis.bAsTimeInfo(c4Hour, c4Minute, c4Second, c4Millis))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_AsTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return (!c4Hour || (c4Hour > 12));
}


//
//  A special case for parsing 8601 time stamps, which are retardedly overly complex
//  for what they are intended to achieve. Time values can have optional decimal
//  places, so we have a helper to get a time value. We throw out
//
//  Since it can contain a time zone, the caller can ask us to convert to Zulu time
//  or local time (which we do by first converting to Zulu and then to local.) The
//  parameter defaults to false, so that anything without an explicit time zone just
//  comes back as is, since that's what they'd typically want.
//
static tCIDLib::TBoolean
bGetTimeVal(const   TStringView&        strvToParse
            , const tCIDLib::TCard4     c4Count
            ,       tCIDLib::TCard4&    c4Index
            ,       TString&            strTmp
            ,       tCIDLib::TCard4&    c4Val
            ,       tCIDLib::TFloat8&   f8DecVal)
{
    tCIDLib::TBoolean bGotDec = kCIDLib::False;
    c4Val = 0;
    f8DecVal = 0.0;

    // Has to be at two digits
    strvToParse.CopyOutSubStr(strTmp, c4Index, 2);
    c4Val = strTmp.c4Val(tCIDLib::ERadices::Dec);
    c4Index += 2;

    // If the next char is a decimal or comma, we have to deal with that
    if (c4Index < c4Count)
    {
        tCIDLib::TCh chCur = strvToParse[c4Index];
        bGotDec = (chCur == kCIDLib::chPeriod) || (chCur == kCIDLib::chComma);
        if (bGotDec)
        {
            c4Index++;
            strTmp.Clear();

            // Pull out decimals until we hit a non-digit
            while (c4Index < c4Count)
            {
                chCur = strvToParse[c4Index];
                if (!TRawStr::bIsDigit(chCur))
                    break;

                strTmp.Append(chCur);
                c4Index++;
            }

            // Store the decimal value
            f8DecVal = tCIDLib::TFloat8(strTmp.c4Val(tCIDLib::ERadices::Dec))
                       / TMathLib::f8Power(10.0, tCIDLib::TFloat8(strTmp.c4Length()));
        }
    }
    return bGotDec;
}

tCIDLib::TBoolean
TTime::bParseFrom8601(const TStringView& strvToParse, const tCIDLib::TBoolean bAsLocal)
{
    enum class EStates
    {
        StartWS
        , Year
        , YearSep
        , Month
        , MonthSep
        , Day
        , Divider
        , Hour
        , HourSep
        , Min
        , MinSep
        , Sec
        , TZSep
        , TZHour
        , TZHourSep
        , TZMin
        , TZEnd
        , EndWS
    };

    EStates eState = EStates::StartWS;

    // Default ones that don't have to be provided
    tCIDLib::TCard4 c4Year = 0;
    tCIDLib::TCard4 c4Month = 0;
    tCIDLib::TCard4 c4Day = 1;
    tCIDLib::TCard4 c4Hour = 0;
    tCIDLib::TCard4 c4Min = 0;
    tCIDLib::TCard4 c4Sec = 0;
    tCIDLib::TCard4 c4Millis = 0;
    tCIDLib::TInt4  i4TZOfs = 0;

    try
    {
        tCIDLib::TBoolean   bExtraHour = kCIDLib::False;
        tCIDLib::TBoolean   bGotDivider = kCIDLib::False;
        tCIDLib::TBoolean   bGotHourSep = kCIDLib::False;
        tCIDLib::TBoolean   bGotYearSep = kCIDLib::False;
        tCIDLib::TBoolean   bNegTZ = kCIDLib::False;
        tCIDLib::TBoolean   bSkippedDay = kCIDLib::True;
        tCIDLib::TCard4     c4Count = strvToParse.c4Length();
        tCIDLib::TCard4     c4Index = 0;
        tCIDLib::TFloat8    f8DecVal = 0;
        TString             strTmp;

        while (c4Index < c4Count)
        {
            const tCIDLib::TCh chCur = strvToParse[c4Index];

            switch(eState)
            {
                case EStates::StartWS :
                    // Break out on a non-whitespace
                    if (TRawStr::bIsSpace(chCur))
                        c4Index++;
                    else
                        eState = EStates::Year;
                    break;

                case EStates::Year :
                    // Has to be four digits
                    strvToParse.CopyOutSubStr(strTmp, c4Index, 4);
                    c4Year = strTmp.c4Val(tCIDLib::ERadices::Dec);
                    c4Index += 4;

                    eState = EStates::YearSep;
                    break;

                case EStates::YearSep :
                    // If a separator, then skip it
                    bGotYearSep = (chCur == kCIDLib::chHyphenMinus);
                    if (bGotYearSep)
                        c4Index++;

                    // Have to get the month now
                    eState = EStates::Month;
                    break;

                case EStates::Month :
                    // Has to be two digits
                    strvToParse.CopyOutSubStr(strTmp, c4Index, 2);
                    c4Month = strTmp.c4Val(tCIDLib::ERadices::Dec);
                    c4Index += 2;

                    // Make sure it's a valid month
                    if (!c4Month || (c4Month > 12))
                        return kCIDLib::False;

                    //
                    //  If we get a T, then we are skipping the day. In that case
                    //  we have to have gotten the year separator. Else, we are
                    //  obviously doing the day so check for the month separator.
                    //
                    if (chCur == L'T')
                    {
                        if (!bGotYearSep)
                            return kCIDLib::False;
                        eState = EStates::Divider;
                    }
                     else
                    {
                        eState = EStates::MonthSep;
                    }
                    break;

                case EStates::MonthSep :
                    //
                    //  If a separator, eat it, else move to the day. If not, and
                    //  we got a year separator, then that's a failure.
                    //
                    if (chCur == kCIDLib::chHyphenMinus)
                        c4Index++;
                    else if (bGotYearSep)
                        return kCIDLib::False;

                    // We have to be moving to the day
                    eState = EStates::Day;
                    break;

                case EStates::Day :
                    // Has to be two digits, since we don't support the ordinal or week types
                    strvToParse.CopyOutSubStr(strTmp, c4Index, 2);
                    c4Day = strTmp.c4Val(tCIDLib::ERadices::Dec);
                    c4Index += 2;

                    // Make sure it'sa valid day
                    if (!c4Day || (c4Day > 31))
                        return kCIDLib::False;

                    // Indicate we didn't skip the day
                    bSkippedDay = kCIDLib::False;

                    // Next is the divider if anything left
                    eState = EStates::Divider;
                    break;

                case EStates::Divider :
                    // If a divider, skip it. Either way, move to the hour now
                    if (chCur == L'T')
                    {
                        bGotDivider = kCIDLib::True;
                        c4Index++;
                    }
                    eState = EStates::Hour;
                    break;

                case EStates::Hour :
                    if (bGetTimeVal(strvToParse, c4Count, c4Index, strTmp, c4Hour, f8DecVal))
                    {
                        //
                        //  We won't do any more time, but set the minutes based on
                        //  this.
                        //
                        c4Min = tCIDLib::TCard4(60.0 * f8DecVal);
                        eState = EStates::TZSep;
                    }
                     else
                    {
                        eState = EStates::HourSep;
                    }

                    //
                    //  Make sure it's a valid hour. Adjuts for the special case of
                    //  24.
                    //
                    if (c4Hour == 24)
                    {
                        c4Hour--;
                        bExtraHour = kCIDLib::True;
                    }

                    if (c4Hour > 23)
                        return kCIDLib::False;
                    break;

                case EStates::HourSep :
                    //
                    //  Skip the hour separator if we have one. If we hit the TZ sep
                    //  then move to TZ mode. The only thing required is the hour.
                    //
                    if ((chCur == kCIDLib::chHyphenMinus)
                    ||  (chCur == kCIDLib::chPlusSign))
                    {
                        eState = EStates::TZSep;
                    }
                     else
                    {
                        // Skip the separator if there
                        if (chCur == kCIDLib::chColon)
                            c4Index++;
                        eState = EStates::Min;
                    }
                    break;

                case EStates::Min :
                    // Get the minute value
                    if (bGetTimeVal(strvToParse, c4Count, c4Index, strTmp, c4Min, f8DecVal))
                    {
                        //
                        //  We won't do any more time, but set the seconds based on
                        //  this.
                        //
                        c4Sec = tCIDLib::TCard4(60.0 * f8DecVal);
                        eState = EStates::TZSep;
                    }
                     else if (c4Index < c4Count)
                    {
                        // Move up to the seconds
                        eState = EStates::MinSep;
                    }

                    // Make sure it's a valid minute
                    if (c4Min > 59)
                        return kCIDLib::False;
                    break;

                case EStates::MinSep :
                    //
                    //  If we hit the time zone separator, then move to that. Else
                    //  skip the separator and move to the second.
                    //
                    if ((chCur == kCIDLib::chHyphenMinus)
                    ||  (chCur == kCIDLib::chPlusSign))
                    {
                        eState = EStates::TZSep;
                    }
                     else
                    {
                        //
                        //  Skip the separator if there. If not, but we got an hour
                        //  separator, then it's bad.
                        //
                        if (chCur == kCIDLib::chColon)
                            c4Index++;
                        else if (bGotHourSep)
                            return kCIDLib::False;

                        eState = EStates::Sec;
                    }
                    break;

                case EStates::Sec :
                {
                    // We'll set up millis if we get a decimal value
                    if (bGetTimeVal(strvToParse, c4Count, c4Index, strTmp, c4Sec, f8DecVal))
                        c4Millis = tCIDLib::TCard4(1000.0 * f8DecVal);

                    // Make sure it's a valid second
                    if (c4Sec > 59)
                        return kCIDLib::False;

                    // Check for time zone next
                    eState = EStates::TZSep;
                    break;
                }

                case EStates::TZSep :
                    //
                    //  If just a Z, then we are done. If a + or -, then move to the
                    //
                    if ((chCur == kCIDLib::chHyphenMinus)
                    ||  (chCur == kCIDLib::chPlusSign))
                    {
                        c4Index++;
                        eState = EStates::TZHour;
                        bNegTZ = (chCur == kCIDLib::chHyphenMinus);
                    }
                     else
                    {
                        eState = EStates::TZEnd;
                    }
                    break;

                case EStates::TZHour :
                    // Get the hours part of the offset
                    strvToParse.CopyOutSubStr(strTmp, c4Index, 2);
                    i4TZOfs = strTmp.i4Val(tCIDLib::ERadices::Dec) * 60;
                    c4Index += 2;

                    eState = EStates::TZHourSep;
                    break;

                case EStates::TZHourSep :
                    if (chCur == kCIDLib::chColon)
                        c4Index++;
                    eState = EStates::TZMin;
                    break;


                case EStates::TZMin :
                    strvToParse.CopyOutSubStr(strTmp, c4Index, 2);
                    i4TZOfs += strTmp.i4Val(tCIDLib::ERadices::Dec);
                    c4Index += 2;
                    eState = EStates::TZEnd;
                    break;

                case EStates::TZEnd :
                    if (chCur != L'Z')
                        return kCIDLib::False;

                    c4Index++;
                    eState = EStates::EndWS;
                    break;

                case EStates::EndWS :
                    // If not whitespace, we failed
                    if (!TRawStr::bIsSpace(chCur))
                        return kCIDLib::False;

                    // Else move up to eat more till the end
                    c4Index++;
                    break;

                default :
                    CIDAssert2(L"Unknown ISO-8601 parse state");
                    return kCIDLib::False;
                    break;
            };
        }

        //
        //  If we didn't get at least to the month, then it can't be a valid stamp.
        //  The minimum would be a YYYYMM or YYYY-MM. Otherwise, we have already
        //  caught any other errors.
        //
        if (eState < EStates::Month)
            return kCIDLib::False;

        // Set us from the collected values
        FromDetails
        (
            c4Year
            , tCIDLib::EMonths(c4Month - 1)
            , c4Day
            , c4Hour
            , c4Min
            , c4Sec
            , c4Millis
        );

        //
        //  If we have an extra hour because of the 24:00 thing, then we have to
        //  convert to Julian, bump the count,and convert back. This gets us midnight
        //  the next day, which is what we want.
        //
        if (bExtraHour)
        {
            tCIDLib::TCard4 c4Julian = c4ToJulian();
            FromJulian(c4Julian + 1);
        }

        //
        //  If the time offset just happens to match our own local time offset, and
        //  they want local time, we don't have to do anything. Else we have to
        //  convert it.
        //
        if (((i4TZOfs * (bNegTZ ? -1 : 1)) == TTime::i4Offset()) && bAsLocal)
        {
            // Special case, do nothing, since it's already our offset
        }
         else
        {
            //
            //  Adjust our time to zulu time, if non-zero offset. We basically do
            //  the opposit of the sign of the TZ offset, to bring it back to zero.
            //  Even if they want local time, we have to do this first anyway, for
            //  sanity's sake, to make the conversion easier.
            //
            tCIDLib::TEncodedTime enctAdj = enctTime();
            if (i4TZOfs)
            {
                if (bNegTZ)
                    enctAdj += (tCIDLib::TEncodedTime(i4TZOfs) * kCIDLib::enctOneMinute);
                else
                    enctAdj -= (tCIDLib::TEncodedTime(i4TZOfs) * kCIDLib::enctOneMinute);

                // If they want zulu store it back, else we'll do that below
                if (!bAsLocal)
                    enctTime(enctAdj);
            }

            // Now, if they want local, then we have to convert from the Zulu above
            if (bAsLocal)
            {
                tCIDLib::TEncodedTime enctLocal;
                UTCToLocal(enctAdj, enctLocal);
                enctTime(enctLocal);
            }
        }
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  A convenience which just calls the other version, catches any exceptions
//  and just returns a true/false result, for folks who don't care why it
//  didn't work, just whether it did or not.
//
tCIDLib::TBoolean
TTime::bParseFromText(  const   TStringView&    strvSrc
                        , const TTimeCompList&  fcolCompList
                        , const tCIDLib::TCh    chDateSep
                        , const tCIDLib::TCh    chTimeSep
                        , const tCIDLib::TCh    chTZSep)
{
    try
    {
        ParseFromText(strvSrc, fcolCompList, chDateSep, chTimeSep, chTZSep);
    }

    catch(...)
    {
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


// Break out the time values
tCIDLib::TCard4
TTime::c4AsTimeInfo(tCIDLib::TCard4&    c4Hour
                    , tCIDLib::TCard4&  c4Minute
                    , tCIDLib::TCard4&  c4Second) const
{
    tCIDLib::TCard4 c4Millis;
    if (!m_ktmsThis.bAsTimeInfo(c4Hour, c4Minute, c4Second, c4Millis))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_AsTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return c4Millis;
}


tCIDLib::TCard4 TTime::c4ToJulian() const
{
    // Get the date and time values out
    tCIDLib::TCard4   c4Year;
    tCIDLib::EMonths  eMonth;
    tCIDLib::TCard4   c4Day;
    eAsDateInfo(c4Year, eMonth, c4Day);

    // And use that to calc the Julian
    return c4CvtToJulian(c4Year, eMonth, c4Day);
}


tCIDLib::TVoid TTime::ClipToHour()
{
    tCIDLib::TEncodedTime enctTmp = m_ktmsThis.enctTime() / kCIDLib::enctOneHour;
    StoreValue(enctTmp * kCIDLib::enctOneHour);
}


tCIDLib::TVoid TTime::ClipToMinute()
{
    tCIDLib::TEncodedTime enctTmp = m_ktmsThis.enctTime() / kCIDLib::enctOneMinute;
    StoreValue(enctTmp * kCIDLib::enctOneMinute);
}


tCIDLib::TVoid TTime::ClipToSecond()
{
    tCIDLib::TEncodedTime enctTmp = m_ktmsThis.enctTime() / kCIDLib::enctOneSecond;
    StoreValue(enctTmp * kCIDLib::enctOneSecond);
}


tCIDLib::EWeekDays
TTime::eAsDateInfo( tCIDLib::TCard4&    c4Year
                    , tCIDLib::EMonths& eMonth
                    , tCIDLib::TCard4&  c4Day) const
{
    tCIDLib::EWeekDays eRet;
    if (!m_ktmsThis.bAsDateInfo(c4Year, eMonth, c4Day, eRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_AsDate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return eRet;
}


//
//  Expand all the details out in one call. This is necesary to get them
//  all atomically and insure they are consistent.
//
tCIDLib::EWeekDays
TTime::eExpandDetails(  tCIDLib::TCard4&            c4Year
                        , tCIDLib::EMonths&         eMonth
                        , tCIDLib::TCard4&          c4Day
                        , tCIDLib::TCard4&          c4Hour
                        , tCIDLib::TCard4&          c4Mins
                        , tCIDLib::TCard4&          c4Secs
                        , tCIDLib::TCard4&          c4Millis
                        , tCIDLib::TEncodedTime&    enctStamp)
{
    tCIDLib::EWeekDays eRet;
    if (!m_ktmsThis.bExpandDetails( c4Year, eMonth, c4Day, eRet
                                    , c4Hour, c4Mins, c4Secs, c4Millis, enctStamp))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_Expand
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return eRet;
}


// Return the difference between this time and the passed one, as a raw stamp
tCIDLib::TEncodedTime
TTime::enctDifference(const TTime& tmToSubtract) const
{
    return m_ktmsThis.enctDifference(tmToSubtract.m_ktmsThis);
}


//
//  Return the number of elappsed milliseconds this time represents, from
//  the base date.
//
tCIDLib::TEncodedTime TTime::enctMilliSeconds() const
{
    return m_ktmsThis.enctMilliSeconds();
}


// Get or set the raw encoded time stamp
tCIDLib::TEncodedTime TTime::enctTime() const
{
    return m_ktmsThis.enctTime();
}

tCIDLib::TEncodedTime TTime::enctTime(const tCIDLib::TEncodedTime& enctToSet)
{
    StoreValue(enctToSet);
    return enctToSet;
}


// Return the date of the week for our time stamp
tCIDLib::EWeekDays TTime::eDayOfWeek() const
{
    tCIDLib::EWeekDays eRet;
    if (!m_ktmsThis.bDayOfWeek(eRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_GetWeekDay
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return eRet;
}


//
//  We format out the time as elapsed days, hours, and minutes. Each value
//  is a two digit, leading zero value, and each section is separated using
//  the indicated sep char. The days are only done if the bShowDays flag
//  is true, else we just do hours and minutes (with any days just not shown,
//  i.e. still never more than 23 hours.)
//
tCIDLib::TVoid
TTime::FormatAsElapsed(         TString&            strToWriteTo
                        , const tCIDLib::TCh        chSepChar
                        , const tCIDLib::TBoolean   bShowDays) const
{
    tCIDLib::TCard4 c4Days = 0;
    tCIDLib::TCard4 c4Hours = 0;
    tCIDLib::TCard4 c4Minutes = 0;

    tCIDLib::TEncodedTime enctAccum(m_ktmsThis.enctTime());

    // If any days, then calc those and reduce the accumulator
    if (enctAccum >= kCIDLib::enctOneDay)
    {
        c4Days = tCIDLib::TCard4(enctAccum / kCIDLib::enctOneDay);
        enctAccum -= c4Days * kCIDLib::enctOneDay;
    }

    // If any hours, then calc those and reduce the accumulator
    if (enctAccum >= kCIDLib::enctOneHour)
    {
        c4Hours = tCIDLib::TCard4(enctAccum / kCIDLib::enctOneHour);
        enctAccum -= c4Hours * kCIDLib::enctOneHour;
    }

    // If any minutes, then calc those
    if (enctAccum >= kCIDLib::enctOneMinute)
        c4Minutes = tCIDLib::TCard4(enctAccum / kCIDLib::enctOneMinute);

    // And now format them out
    strToWriteTo.Clear();

    //
    //  If show days, we always show days. Else, we only show them if
    //  the day value is non-zero.
    //
    if (bShowDays || (c4Days > 0))
    {
        if (c4Days < 10)
            strToWriteTo.Append(kCIDLib::chDigit0);
        strToWriteTo.AppendFormatted(c4Days);
        strToWriteTo.Append(chSepChar);
    }

    if (c4Hours < 10)
        strToWriteTo.Append(kCIDLib::chDigit0);
    strToWriteTo.AppendFormatted(c4Hours);
    strToWriteTo.Append(chSepChar);

    if (c4Minutes < 10)
        strToWriteTo.Append(kCIDLib::chDigit0);
    strToWriteTo.AppendFormatted(c4Minutes);
}


//
//  Same as above but in this case shows hours, minutes, and seconds instead
//  of days, hours and minutes. leading zero hours can be optionally shown
//  or not.
//
tCIDLib::TVoid
TTime::FormatAsElapsed2(        TString&            strToWriteTo
                        , const tCIDLib::TCh        chSepChar
                        , const tCIDLib::TBoolean   bShowHours) const
{
    tCIDLib::TCard4 c4Hours = 0;
    tCIDLib::TCard4 c4Minutes = 0;
    tCIDLib::TCard4 c4Seconds = 0;

    tCIDLib::TEncodedTime enctAccum(m_ktmsThis.enctTime());

    // If any hours, then calc those and reduce the accumulator
    if (enctAccum >= kCIDLib::enctOneHour)
    {
        c4Hours = tCIDLib::TCard4(enctAccum / kCIDLib::enctOneHour);
        enctAccum -= c4Hours * kCIDLib::enctOneHour;
    }

    // If any minutes, then calc those and reduce the accumulator
    if (enctAccum >= kCIDLib::enctOneMinute)
    {
        c4Minutes = tCIDLib::TCard4(enctAccum / kCIDLib::enctOneMinute);
        enctAccum -= c4Minutes * kCIDLib::enctOneMinute;
    }

    // If any seconds, then calc those
    if (enctAccum >= kCIDLib::enctOneSecond)
        c4Seconds = tCIDLib::TCard4(enctAccum / kCIDLib::enctOneSecond);

    // And now format them out
    strToWriteTo.Clear();

    //
    //  If show hours, we always show hours. Else, we only show them if
    //  the hour value is non-zero.
    //
    if (bShowHours || (c4Hours > 0))
    {
        if (c4Hours < 10)
            strToWriteTo.Append(kCIDLib::chDigit0);
        strToWriteTo.AppendFormatted(c4Hours);
        strToWriteTo.Append(chSepChar);
    }

    if (c4Minutes < 10)
        strToWriteTo.Append(kCIDLib::chDigit0);
    strToWriteTo.AppendFormatted(c4Minutes);
    strToWriteTo.Append(chSepChar);

    if (c4Seconds < 10)
        strToWriteTo.Append(kCIDLib::chDigit0);
    strToWriteTo.AppendFormatted(c4Seconds);
}


//
//  Provides the fairly extensive amount of grunt work code required to
//  format out a time value using a format string. We have one that takes
//  a format string, and another that calls the first using the default
//  format string.
//
tCIDLib::TVoid
TTime::FormatToStr(         TString&                strToWriteTo
                    , const tCIDLib::EAppendOver    eAppend) const
{
    // Just call the other version with the default format
    FormatToStr(strToWriteTo, strDefaultFormat(), eAppend);
}

tCIDLib::TVoid
TTime::FormatToStr(         TString&                strToWriteTo
                    , const TString&                strFormat
                    , const tCIDLib::EAppendOver    eAppend) const
{
    //
    //  First get a list of all of the tokens that are in the format
    //  string. If there aren't any, that's pathological but legal and
    //  we just copy over the format string to the output and we are done.
    //
    TString strTokList;
    if (!strFormat.bFindTokenList(strTokList))
    {
        if (eAppend == tCIDLib::EAppendOver::Append)
            strToWriteTo.Append(strFormat);
        else
            strToWriteTo = strFormat;
        return;
    }

    //
    //  We have tokens, so let's replace them. Start by moving the
    //  format string to a temp string that we can build the resulting
    //  output into.
    //
    TString strBuild(strFormat);

    // Convert the time to the detail info
    tCIDLib::TCard4         c4Year;
    tCIDLib::EMonths        eMonth;
    tCIDLib::TCard4         c4Day;
    tCIDLib::EWeekDays      eWeekDay;
    tCIDLib::TCard4         c4Hour;
    tCIDLib::TCard4         c4Minute;
    tCIDLib::TCard4         c4Second;
    tCIDLib::TCard4         c4Millis;
    tCIDLib::TEncodedTime   enctRaw;

    if (!m_ktmsThis.bExpandDetails( c4Year, eMonth, c4Day, eWeekDay, c4Hour
                                    , c4Minute, c4Second, c4Millis, enctRaw))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_Expand
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    tCIDLib::TCard4 c4Tmp = 0;
    const tCIDLib::TCard4 c4TokCnt = strTokList.c4Length();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TokCnt; c4Index++)
    {
        const tCIDLib::TCh chCurTok = strTokList[c4Index];

        switch(chCurTok)
        {
            case kCIDLib::chColon :
                // The time separator from the locale
                strBuild.eReplaceToken
                (
                    TLocale::chTimeSeparator(), kCIDLib::chColon
                );
                break;

            case kCIDLib::chForwardSlash :
                // The date separate from the locale
                strBuild.eReplaceToken
                (
                    TLocale::chDateSeparator(), kCIDLib::chForwardSlash
                );
                break;

            case kCIDLib::chLatin_a :
                // The short weekday name
                strBuild.eReplaceToken
                (
                    TLocale::strDayAbbrev(eWeekDay), kCIDLib::chLatin_a
                );
                break;

            case kCIDLib::chLatin_A :
                // The long weekday name
                strBuild.eReplaceToken(TLocale::strDay(eWeekDay), kCIDLib::chLatin_A);
                break;

            case kCIDLib::chLatin_D :
                // The day as a number
                strBuild.eReplaceToken(c4Day, kCIDLib::chLatin_D);
                break;

            case kCIDLib::chLatin_d :
                // The full weekday name
                strBuild.eReplaceToken(TLocale::strDay(eWeekDay), kCIDLib::chLatin_d);
                break;

            case kCIDLib::chLatin_e :
                // This one is the elapsed seconds
                strBuild.eReplaceToken(enctRaw / kCIDLib::enctOneSecond, kCIDLib::chLatin_e);
                break;

            case kCIDLib::chLatin_E :
                // This one is the elapsed minutes
                strBuild.eReplaceToken(enctRaw / kCIDLib::enctOneMinute, kCIDLib::chLatin_E);
                break;

            case kCIDLib::chLatin_H :
                // The hours a number, 24 hour style
                strBuild.eReplaceToken(c4Hour, kCIDLib::chLatin_H);
                break;

            case kCIDLib::chLatin_h :
                // The hour in 12 hour style
                c4Tmp = c4Hour;
                if (!c4Tmp)
                    c4Tmp = 12;
                else if (c4Tmp > 12)
                    c4Tmp -= 12;
                strBuild.eReplaceToken(c4Tmp, kCIDLib::chLatin_h);
                break;

            case kCIDLib::chLatin_l :
                // The milliseconds as a number
                strBuild.eReplaceToken(c4Millis, kCIDLib::chLatin_l);
                break;

            case kCIDLib::chLatin_M :
                // The month as a number
                strBuild.eReplaceToken(tCIDLib::TCard4(eMonth) + 1UL, kCIDLib::chLatin_M);
                break;

            case kCIDLib::chLatin_m :
                // The short month name
                strBuild.eReplaceToken(TLocale::strMonthAbbrev(eMonth), kCIDLib::chLatin_m);
                break;

            case kCIDLib::chLatin_n :
                // Full localized month name
                strBuild.eReplaceToken(TLocale::strMonth(eMonth), kCIDLib::chLatin_n);
                break;

            case kCIDLib::chLatin_P :
                // The AM/PM designation
                if (c4Hour >= 12)
                    strBuild.eReplaceToken(TLocale::strPMString(), kCIDLib::chLatin_P);
                else
                    strBuild.eReplaceToken(TLocale::strAMString(), kCIDLib::chLatin_P);
                break;

            case kCIDLib::chLatin_p :
                // A single letter A/P designation (lower cased)
                if (c4Hour >= 12)
                {
                    TString strTmp(TLocale::strPMString());
                    strTmp.ToLower();
                    strTmp.Cut(1);
                    strBuild.eReplaceToken(strTmp, kCIDLib::chLatin_p);
                }
                 else
                {
                    TString strTmp(TLocale::strAMString());
                    strTmp.ToLower();
                    strTmp.Cut(1);
                    strBuild.eReplaceToken(strTmp, kCIDLib::chLatin_p);
                }
                break;

            case kCIDLib::chLatin_s :
                // Display the seconds
                strBuild.eReplaceToken(c4Second, kCIDLib::chLatin_s);
                break;

            // Display the raw time stamp value
            case kCIDLib::chLatin_S :
                strBuild.eReplaceToken(enctRaw, kCIDLib::chLatin_S);
                break;

            case kCIDLib::chLatin_T :
            {
                // The local time zone designation
                tCIDLib::TZStr64 szTZName;
                TKrnlTimeStamp::bTZName(szTZName, tCIDLib::c4MaxBufChars(szTZName));
                strBuild.eReplaceToken(szTZName, kCIDLib::chLatin_T);
                break;
            }

            case kCIDLib::chLatin_t :
            {
                //
                //  This one is the time zone offset from UTC. It a signed
                //  numeric value in hours as stored, but we have to display
                //  in a kind of pseudo hours, like -0600 or 0500 or 1000.
                //  So the hours must be displayed in a two digit, zero lead
                //  field, then the left over minutes are similarly formatted
                //  and appended.
                //
                tCIDLib::TInt4 i4Ofs = TTime::i4Offset();
                TString strTmp(8UL);
                if (i4Ofs >= 0)
                {
                    strTmp.Append(kCIDLib::chPlusSign);
                }
                 else
                {
                    strTmp.Append(kCIDLib::chHyphenMinus);
                    i4Ofs *= -1;
                }

                const tCIDLib::TInt4 i4Hours = i4Ofs / 60;
                const tCIDLib::TInt4 i4Mins = i4Ofs % 60;

                if (i4Hours < 10)
                    strTmp.Append(kCIDLib::chDigit0);
                strTmp.AppendFormatted(i4Hours);

                if (i4Mins < 10)
                    strTmp.Append(kCIDLib::chDigit0);
                strTmp.AppendFormatted(i4Mins);
                strBuild.eReplaceToken(strTmp, L't');
                break;
            }

            case kCIDLib::chLatin_u :
                // The minutes as a number
                strBuild.eReplaceToken(c4Minute, kCIDLib::chLatin_u);
                break;

            case kCIDLib::chLatin_x :
            case kCIDLib::chLatin_X :
            {
                //
                //  We calculate the elapsed days, hours, and minutes and
                //  format them out in 2 digit, leading zero form, with
                //  locale separators. Lower case shows leading zero days,
                //  and upper case does not.
                //
                TString strTmp;
                FormatAsElapsed
                (
                    strTmp, TLocale::chTimeSeparator(), (chCurTok == kCIDLib::chLatin_x)
                );
                if (chCurTok == kCIDLib::chLatin_x)
                    strBuild.eReplaceToken(strTmp, L'x');
                else
                    strBuild.eReplaceToken(strTmp, L'X');
                break;
            }

            case kCIDLib::chLatin_Y :
                // The full year as a number
                strBuild.eReplaceToken(c4Year, kCIDLib::chLatin_Y);
                break;

            case kCIDLib::chLatin_y :
                // The year as a two digit number
                c4Tmp = c4Year;
                if (c4Year > 2000)
                    c4Tmp -= 2000;
                else
                    c4Tmp -= 1900;
                strBuild.eReplaceToken(c4Tmp, kCIDLib::chLatin_y);
                break;

            case kCIDLib::chLatin_z :
            case kCIDLib::chLatin_Z :
            {
                //
                //  We calculate the elapsed hours, minutes, and seconds and
                //  format them out in 2 digit, leading zero form, with
                //  locale separators. Lower case shows leading zero hours,
                //  and upper case does not.
                //
                TString strTmp;
                FormatAsElapsed2
                (
                    strTmp, TLocale::chTimeSeparator(), (chCurTok == kCIDLib::chLatin_z)
                );

                if (chCurTok == L'z')
                    strBuild.eReplaceToken(strTmp, L'z');
                else
                    strBuild.eReplaceToken(strTmp, L'Z');
                break;
            }

            default :
                // Not one we care about, so ignore
                break;
        };
    }

    // And now either set or append tot he target string
    if (eAppend == tCIDLib::EAppendOver::Append)
        strToWriteTo.Append(strBuild);
    else
        strToWriteTo = strBuild;
}


// Set this time from the raw details
tCIDLib::TVoid
TTime::FromDetails( const   tCIDLib::TCard4     c4Year
                    , const tCIDLib::EMonths    eMonth
                    , const tCIDLib::TCard4     c4Day
                    , const tCIDLib::TCard4     c4Hour
                    , const tCIDLib::TCard4     c4Minute
                    , const tCIDLib::TCard4     c4Second
                    , const tCIDLib::TCard4     c4MilliSecs)
{
    if (!m_ktmsThis.bFromDetails
    (
        c4Year
        , eMonth
        , c4Day
        , c4Hour
        , c4Minute
        , c4Second
        , c4MilliSecs))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_BadDetails
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
}


//
//  Set us to passed julian date, in either card4 or floating point
//  format (the latter being able to represent a time into that day,
//  whereas the first one only gets the day itself.)
//
tCIDLib::TVoid TTime::FromJulian(const tCIDLib::TCard4 c4Julian)
{
    // Convert the Julian to raw values
    tCIDLib::TCard4     c4Day;
    tCIDLib::TCard4     c4Year;
    tCIDLib::EMonths    eMonth;
    CvtFromJulian(c4Julian, c4Year, eMonth, c4Day);

    // And set ourselves using that
    FromDetails(c4Year, eMonth, c4Day);
}

tCIDLib::TVoid TTime::FromJulian(const tCIDLib::TFloat8 f8Julian)
{
    //
    //  Convert the Julian to raw values. Make sure the passed value gets
    //  rounded up if 0.5 or greater, since that's in the next integral day
    //  in Julian world.
    //
    tCIDLib::TCard4     c4Day;
    tCIDLib::TCard4     c4Year;
    tCIDLib::EMonths    eMonth;
    CvtFromJulian(tCIDLib::TCard4(f8Julian + 0.5), c4Year, eMonth, c4Day);

    // And set ourselves using that
    FromDetails(c4Year, eMonth, c4Day);
}


//
//  Set this time from the passed time details. It'll be on the date of the
//  base date.
//
tCIDLib::TVoid
TTime::FromTimeDetails( const   tCIDLib::TCard4     c4Hour
                        , const tCIDLib::TCard4     c4Minute
                        , const tCIDLib::TCard4     c4Second
                        , const tCIDLib::TCard4     c4MilliSecs)
{
    if (!m_ktmsThis.bFromTimeDetails(c4Hour, c4Minute, c4Second, c4MilliSecs))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_BadDetails
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
}


//
//  These smethod will move us forward by some number of particular units
//  of time, dealing with DST, leap years, etc...
//
tCIDLib::TVoid TTime::ForwardByDays(const tCIDLib::TCard4 c4Count)
{
    if (!m_ktmsThis.bForwardByDays(c4Count))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_ForwardFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Count)
            , TString(L"day(s)")
        );
    }
}

tCIDLib::TVoid TTime::ForwardByMonths(const tCIDLib::TCard4 c4Count)
{
    if (!m_ktmsThis.bForwardByMonths(c4Count))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_ForwardFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Count)
            , TString(L"month(s)")
        );
    }
}

tCIDLib::TVoid TTime::ForwardByYears(const tCIDLib::TCard4 c4Count)
{
    if (!m_ktmsThis.bForwardByYears(c4Count))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_ForwardFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Count)
            , TString(L"year(s)")
        );
    }
}


//
//  Assume we contain a local time, and convert it to the equiv UTC
//  time.
//
tCIDLib::TVoid TTime::LocalToUTC()
{
    // Convert the value from UTC to local time and set it on us
    tCIDLib::TEncodedTime enctUTC;
    LocalToUTC(m_ktmsThis.enctTime(), enctUTC);
    StoreValue(enctUTC);
}


//
//  Parses a string based on information about the format and sets us up
//  to represent that parsed time. We get a list of the components that we
//  should expect to see and info about separator chars, and we go through
//  that list and pull those components out one by one.
//
tCIDLib::TVoid
TTime::ParseFromText(const  TStringView&    strvSrc
                    , const TTimeCompList&  fcolCompList
                    , const tCIDLib::TCh    chDateSep
                    , const tCIDLib::TCh    chTimeSep
                    , const tCIDLib::TCh    chTZSep)
{
    enum class EHourTypes
    {
        Hour24
        , HourAM
        , HourPM
    };

    //
    //  Set up defaults for anything we don't get. The weekday is ignored
    //  since it will come out of the actual date that we set.
    //
    tCIDLib::TCard4     c4Year = 1970;
    tCIDLib::TCard4     c4Day = 1;
    tCIDLib::TCard4     c4Hour = 0;
    tCIDLib::TCard4     c4Minute = 0;
    tCIDLib::TCard4     c4Second = 0;
    tCIDLib::TCard4     c4MilliSecs = 0;
    tCIDLib::EWeekDays  eDay = tCIDLib::EWeekDays::Count;
    EHourTypes          eHourType = EHourTypes::Hour24;
    tCIDLib::EMonths    eMonth = tCIDLib::EMonths::January;
    tCIDLib::TInt4      i4TZOfs = i4Offset();

    //
    //  Figure out the actual separators, since they may have passed nulls
    //  in which case we use the locale versions.
    //
    const tCIDLib::TCh chActDateSep = chDateSep ? chDateSep : TLocale::chDateSeparator();
    const tCIDLib::TCh chActTimeSep = chTimeSep ? chTimeSep : TLocale::chTimeSeparator();
    const tCIDLib::TCh chActTZSep = chTZSep ? chTZSep : kCIDLib::chSpace;

    // We need to move forward through the format list
    const tCIDLib::TCard4   c4FmtLen = fcolCompList.c4ElemCount();
    tCIDLib::TCard4         c4FmtIndex = 0;

    // Build up a whitespace string
    TString strSpace(kCIDLib::szWhitespace);
    if (chActDateSep != kCIDLib::chSpace)
        strSpace.Append(chActDateSep);
    if (chActTimeSep != kCIDLib::chSpace)
        strSpace.Append(chActTimeSep);
    if (chActTZSep != kCIDLib::chSpace)
        strSpace.Append(chActTZSep);

    //
    //  Set up an input stream on the src string. If it's a raw string, this will make a copy to
    //  be adopted by the stream. If a string object, the stream will use it directly.
    //
    TTextStringInStream strmSrc(strvSrc);

    tCIDLib::TCh    chCur = kCIDLib::chNull;
    tCIDLib::TCard4 c4Count = 0;
    tCIDLib::TInt4  i4Val = 0;
    TString         strTok;
    while (c4FmtIndex < c4FmtLen)
    {
        tCIDLib::TBoolean bGotIt = kCIDLib::False;

        // Get the next component we should expect
        const tCIDLib::ETimeComps eComp = fcolCompList[c4FmtIndex++];

        //
        //  If end of stream, then we have to have failed because there are still
        //  more components to process.
        //
        if (strmSrc.bEndOfStream())
            break;

        //
        //  It's going to be a number of some sort, whitespace, one of the
        //  separators, or a name. So based on that, we can do the basic
        //  parsing of the next token.
        //
        try
        {
            switch(eComp)
            {
                case tCIDLib::ETimeComps::Colon :
                    strmSrc >> chCur;
                    bGotIt = (chCur == kCIDLib::chColon);
                    break;

                case tCIDLib::ETimeComps::DateSep :
                    strmSrc >> chCur;
                    bGotIt = chCur == chActDateSep;
                    break;

                case tCIDLib::ETimeComps::Hyphen :
                    strmSrc >> chCur;
                    bGotIt = (chCur == kCIDLib::chHyphenMinus);
                    break;

                case tCIDLib::ETimeComps::Punct :
                    // It has to be a single character of punctuation
                    strmSrc >> chCur;
                    bGotIt = TRawStr::bIsPunct(chCur);
                    break;

                case tCIDLib::ETimeComps::Space :
                    //
                    //  Eat chars while it's whitespace. We have to get
                    //  at least one.
                    //
                    c4Count = 0;
                    while (kCIDLib::True)
                    {
                        chCur = strmSrc.chPeek();
                        if (!chCur || !strSpace.bContainsChar(chCur))
                            break;
                        strmSrc.chRead();
                        c4Count++;
                    }
                    bGotIt = (c4Count != 0);
                    break;

                case tCIDLib::ETimeComps::T :
                    strmSrc >> chCur;
                    bGotIt = (chCur == L'T');
                    break;

                case tCIDLib::ETimeComps::TimeSep :
                    strmSrc >> chCur;
                    bGotIt = chCur == chActTimeSep;
                    break;

                case tCIDLib::ETimeComps::TZSep :
                    strmSrc >> chCur;
                    bGotIt = chCur == chActTZSep;
                    break;

                case tCIDLib::ETimeComps::AMPM :
                case tCIDLib::ETimeComps::DayName :
                case tCIDLib::ETimeComps::MonthName :
                case tCIDLib::ETimeComps::TZName :
                    //
                    //  Parse until we get a non-alpha character, and then
                    //  look that up to make sure it's valid.
                    //
                    strTok.Clear();
                    while (kCIDLib::True)
                    {
                        chCur = strmSrc.chPeek();
                        if (!chCur || !TRawStr::bIsAlpha(chCur))
                            break;
                        strmSrc.chRead();
                        strTok.Append(chCur);
                    }

                    // We ignore the time zone name.
                    bGotIt = kCIDLib::True;
                    if (eComp == tCIDLib::ETimeComps::AMPM)
                    {
                        if (strTok == TLocale::strAMString())
                            eHourType = EHourTypes::HourAM;
                        else if (strTok == TLocale::strPMString())
                            eHourType = EHourTypes::HourPM;
                        else
                            bGotIt = kCIDLib::False;
                    }
                     else if (eComp == tCIDLib::ETimeComps::DayName)
                    {
                        eDay = TLocale::eXlatLocDay(strTok);
                        if (eDay == tCIDLib::EWeekDays::Count)
                            bGotIt = kCIDLib::False;
                    }
                     else if (eComp == tCIDLib::ETimeComps::MonthName)
                    {
                        eMonth = TLocale::eXlatLocMonth(strTok);
                        if (eMonth == tCIDLib::EMonths::Count)
                            bGotIt = kCIDLib::False;
                    }
                    break;

                case tCIDLib::ETimeComps::DayNum :
                case tCIDLib::ETimeComps::Hour12 :
                case tCIDLib::ETimeComps::Hour24 :
                case tCIDLib::ETimeComps::Minute :
                case tCIDLib::ETimeComps::MonthNum :
                case tCIDLib::ETimeComps::Second :
                case tCIDLib::ETimeComps::Year2 :
                case tCIDLib::ETimeComps::Year4 :
                    //
                    //  These will all be unsigned numbers, no sign
                    //
                    strTok.Clear();
                    while (kCIDLib::True)
                    {
                        chCur = strmSrc.chPeek();
                        if (!chCur || !TRawStr::bIsDigit(chCur))
                            break;
                        strmSrc.chRead();
                        strTok.Append(chCur);
                    }

                    if (!strTok.bIsEmpty())
                    {
                        i4Val = strTok.i4Val(tCIDLib::ERadices::Dec);

                        // Store it in the appropriate field
                        switch(eComp)
                        {
                            case tCIDLib::ETimeComps::DayNum :
                                if ((i4Val >= 1) && (i4Val <= 31))
                                {
                                    // This one is one based
                                    c4Day = tCIDLib::TCard4(i4Val);
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            case tCIDLib::ETimeComps::Hour12 :
                                if ((i4Val >= 1) && (i4Val <= 12))
                                {
                                    c4Hour = tCIDLib::TCard4(i4Val - 1);
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            case tCIDLib::ETimeComps::Hour24 :
                                if ((i4Val >= 0) && (i4Val <= 23))
                                {
                                    c4Hour = tCIDLib::TCard4(i4Val);
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            case tCIDLib::ETimeComps::Minute :
                                if ((i4Val >= 0) && (i4Val <= 59))
                                {
                                    c4Minute = tCIDLib::TCard4(i4Val);
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            case tCIDLib::ETimeComps::MonthNum :
                                if ((i4Val >= 1) && (i4Val <= 12))
                                {
                                    eMonth = tCIDLib::EMonths(i4Val - 1);
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            case tCIDLib::ETimeComps::Second :
                                if ((i4Val >= 0) && (i4Val <= 59))
                                {
                                    c4Second = tCIDLib::TCard4(i4Val);
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            case tCIDLib::ETimeComps::Year2 :
                                if ((i4Val >= 1) && (i4Val <= 99))
                                {
                                    c4Year = tCIDLib::TCard4(i4Val);
                                    if (c4Year > 50)
                                        c4Year += 1999;
                                    else
                                        c4Year += 2000;
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            case tCIDLib::ETimeComps::Year4 :
                                if (i4Val >= 1970)
                                {
                                    c4Year = tCIDLib::TCard4(i4Val);
                                    bGotIt = kCIDLib::True;
                                }
                                break;

                            default :
                                break;
                        }
                    }
                    break;

                case tCIDLib::ETimeComps::TZ :
                    //
                    //  This one is complicated. It can be either a signed
                    //  offset in hours/minutes from UTC, or it can be one
                    //  of several symbolic names.
                    //
                    strTok.Clear();
                    chCur = strmSrc.chRead();
                    if ((chCur == kCIDLib::chHyphenMinus)
                    ||  (chCur == kCIDLib::chPlusSign))
                    {
                        //
                        //  It has to be a numeric offset. Don't store the
                        //  sign, just remember it. It makes the below easier.
                        //
                        const tCIDLib::TCh chSign = chCur;

                        // Now we eat digits
                        while (kCIDLib::True)
                        {
                            chCur = strmSrc.chPeek();
                            if (!chCur || !TRawStr::bIsDigit(chCur))
                            {
                                //
                                //  We can see a colon after the second digit.
                                //  If so, just ignore it. Else we are done.
                                //
                                if ((chCur == kCIDLib::chColon) && (strTok.c4Length() == 2))
                                {
                                    strmSrc.chRead();
                                    strTok.Append(chCur);
                                }
                                 else
                                {
                                    break;
                                }
                            }
                             else
                            {
                                strmSrc.chRead();
                                strTok.Append(chCur);
                            }
                        }

                        //
                        //  If it is two digits, it's an hour offset. If it's
                        //  four digits, it's hours and minutes. Else, it's
                        //  bad.
                        //
                        if (strTok.c4Length() == 2)
                        {
                            // Convert to a number of hours and multiply
                            i4TZOfs = strTok.i4Val(tCIDLib::ERadices::Dec) * 60;
                        }
                         else if (strTok.c4Length() == 4)
                        {
                            // Get the two separate values out
                            TString strTmp;
                            strTok.CopyOutSubStr(strTmp, 2);
                            strTok.CapAt(2);

                            i4TZOfs = strTok.i4Val(tCIDLib::ERadices::Dec) * 60;
                            i4TZOfs += strTmp.i4Val(tCIDLib::ERadices::Dec);
                        }
                         else
                        {
                            break;
                        }

                        // Make it negative if we need to
                        if (chSign == kCIDLib::chHyphenMinus)
                            i4TZOfs *= -1;

                        bGotIt = kCIDLib::True;
                    }
                     else
                    {
                        // Some sort of symbolic name
                        strTok.Append(chCur);
                        while (kCIDLib::True)
                        {
                            chCur = strmSrc.chPeek();
                            if (!chCur || !TRawStr::bIsAlpha(chCur))
                                break;
                            strmSrc.chRead();
                            strTok.Append(chCur);
                        }

                        //
                        //  Just say we got it. We'll check for a couple of
                        //  ones that are unambiguous, but otherwise we just
                        //  leave the default TZ offset, which is our offset
                        //  and therefore we won't do any offset adjustment.
                        //
                        if (strTok.bCompareI(L"GMT")
                        ||  strTok.bCompareI(L"UTC")
                        ||  strTok.bCompareI(L"ZULU"))
                        {
                            i4TZOfs = 0;
                        }
                         else if (strTok.c4Length() == 1)
                        {
                            chCur = strTok[0];
                            TRawStr::chUpper(chCur);

                            if ((chCur >= L'A') && (chCur <= L'Z'))
                            {
                                // It's an army designator

                            }
                        }
                        bGotIt = kCIDLib::True;
                    }
                    break;

                default :
                    break;
            };
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcTime_BadTimeFmt
                , errToCatch.strErrText()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4FmtIndex)
            );
        }

        // If we didn't get it, then that's bad
        if (!bGotIt)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcTime_MissingToken
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4FmtIndex)
            );
        }
    }

    //
    //  Ok, we have the info, so let's set ourself up. We first have to
    //  adjust the hour if we got a 12 hour format, based on the AM/PM
    //  designation.
    //
    if (eHourType == EHourTypes::HourPM)
        c4Hour += 12;

    if (!m_ktmsThis.bFromDetails(c4Year, eMonth, c4Day, c4Hour, c4Minute, c4Second, 0))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_BadDetails
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }


    //
    //  If the time zone offset is not the same as ours, then adjust it and
    //  put it back.
    //
    if (i4TZOfs != i4Offset())
    {
        // <TBD> Need to address this sticky issue...
    }
}


// Set us to one of the special times
tCIDLib::TVoid TTime::SetTo(const tCIDLib::ESpecialTimes eSpecial)
{
    if (!m_ktmsThis.bSetTo(eSpecial))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_UnknownSpecialTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TInteger(tCIDLib::i4EnumOrd(eSpecial))
        );
    }
}


//
//  Set us to now plus a number of milliseconds, seconds, and minutes, hours,
//  days, etc...
//
tCIDLib::TVoid TTime::SetToNowPlusDays(const tCIDLib::TCard4 c4Days)
{
    StoreValue(TKrnlTimeStamp::enctNow() + (c4Days * kCIDLib::enctOneDay));
}

tCIDLib::TVoid TTime::SetToNowPlusHours(const tCIDLib::TCard4 c4Hours)
{
    StoreValue(TKrnlTimeStamp::enctNow() + (c4Hours * kCIDLib::enctOneHour));
}

tCIDLib::TVoid TTime::SetToNowPlusMSs(const tCIDLib::TCard4 c4Millis)
{
    StoreValue(TKrnlTimeStamp::enctNow() + (c4Millis * kCIDLib::enctOneMilliSec));
}

tCIDLib::TVoid TTime::SetToNowPlusMins(const tCIDLib::TCard4 c4Mins)
{
    StoreValue(TKrnlTimeStamp::enctNow() + (c4Mins * kCIDLib::enctOneMinute));
}

tCIDLib::TVoid TTime::SetToNowPlusSecs(const tCIDLib::TCard4 c4Secs)
{
    StoreValue(TKrnlTimeStamp::enctNow() + (c4Secs * kCIDLib::enctOneSecond));
}


//
//  These will set us to either local sunrise or sunset time, or for a
//  particular location.
//
tCIDLib::TVoid
TTime::SetToSunriseAt(  const   tCIDLib::TFloat8    f8Latitude
                        , const tCIDLib::TFloat8    f8Longitude
                        , const tCIDLib::TInt4      i4TZOffset)
{
    tCIDLib::TCard4 c4RiseHour;
    tCIDLib::TCard4 c4RiseMin;
    tCIDLib::TCard4 c4SetHour;
    tCIDLib::TCard4 c4SetMin;

    // Get the date details out first, because we want to put them back
    tCIDLib::TCard4 c4Day;
    tCIDLib::TCard4 c4Year;
    tCIDLib::EMonths eMonth;
    eAsDateInfo(c4Year, eMonth, c4Day);

    CalcRiseSetTimes
    (
        *this
        , tCIDLib::TFloat8(i4TZOffset) / 60.0
        , f8Latitude
        , f8Longitude
        , c4RiseHour
        , c4RiseMin
        , c4SetHour
        , c4SetMin
    );

    // And set us back again using the two parts
    FromDetails(c4Year, eMonth, c4Day, c4RiseHour, c4RiseMin, 0, 0);
}

tCIDLib::TVoid
TTime::SetToSunrise(const   tCIDLib::TFloat8    f8Latitude
                    , const tCIDLib::TFloat8    f8Longitude)
{
    SetToSunriseAt(f8Latitude, f8Longitude, i4Offset());
}


tCIDLib::TVoid
TTime::SetToSunsetAt(const  tCIDLib::TFloat8    f8Latitude
                    , const tCIDLib::TFloat8    f8Longitude
                    , const tCIDLib::TInt4      i4TZOffset)
{
    tCIDLib::TCard4 c4RiseHour;
    tCIDLib::TCard4 c4RiseMin;
    tCIDLib::TCard4 c4SetHour;
    tCIDLib::TCard4 c4SetMin;

    // Get the date details out first, because we want to put them back
    tCIDLib::TCard4 c4Day;
    tCIDLib::TCard4 c4Year;
    tCIDLib::EMonths eMonth;
    eAsDateInfo(c4Year, eMonth, c4Day);

    CalcRiseSetTimes
    (
        *this
        , tCIDLib::TFloat8(i4TZOffset) / 60.0
        , f8Latitude
        , f8Longitude
        , c4RiseHour
        , c4RiseMin
        , c4SetHour
        , c4SetMin
    );

    // And set us back again using the two parts
    FromDetails(c4Year, eMonth, c4Day, c4SetHour, c4SetMin, 0, 0);
}

tCIDLib::TVoid
TTime::SetToSunset( const   tCIDLib::TFloat8    f8Latitude
                    , const tCIDLib::TFloat8    f8Longitude)
{
    SetToSunsetAt(f8Latitude, f8Longitude, i4Offset());
}


//
//  Query or set the default format string for this time object
//
const TString& TTime::strDefaultFormat() const
{
    if (!m_pstrDefFormat)
        m_pstrDefFormat = new TString(TLocale::strTimeFormat());

    return *m_pstrDefFormat;
}

const TString& TTime::strDefaultFormat(const TString& strNewDefault)
{
    if (!m_pstrDefFormat)
        m_pstrDefFormat = new TString(strNewDefault);
    else
        *m_pstrDefFormat = strNewDefault;

    return *m_pstrDefFormat;
}


//
//  Assume we contain a UTC time, and convert it to the equiv local
//  time.
//
tCIDLib::TVoid TTime::UTCToLocal()
{
    tCIDLib::TEncodedTime enctLocal;
    UTCToLocal(m_ktmsThis.enctTime(), enctLocal);
    StoreValue(enctLocal);
}


// ---------------------------------------------------------------------------
//  TTime: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTime::FormatTo(TTextOutStream& strmToWriteTo) const
{
    TString strTmp(128UL);

    if (m_pstrDefFormat)
        FormatToStr(strTmp, *m_pstrDefFormat);
    else
        FormatToStr(strTmp, TLocale::strTimeFormat());
    strmToWriteTo << strTmp;
}


tCIDLib::TVoid TTime::StreamFrom(TBinInStream& strmToReadFrom)
{
    tCIDLib::TEncodedTime enctTmp;
    strmToReadFrom >> enctTmp;

    if (!m_ktmsThis.bSetTo(enctTmp))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_SetTo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal64(enctTmp)
        );
    }

    //
    //  Stream in a byte and see if we had a default format. If not, then
    //  get rid of any string we already have. If we did, then stream into
    //  it, creating a new string if there is not one now.
    //
    tCIDLib::TCard1 c1HadFormat;
    strmToReadFrom >> c1HadFormat;

    if (c1HadFormat)
    {
        if (!m_pstrDefFormat)
            m_pstrDefFormat = new TString();
        strmToReadFrom >> *m_pstrDefFormat;
    }
     else
    {
        delete m_pstrDefFormat;
        m_pstrDefFormat = nullptr;
    }
}

tCIDLib::TVoid TTime::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream out the raw time offset value
    strmToWriteTo << m_ktmsThis.enctTime();

    // Stream out a byte to indicate whether we have a format string.
    tCIDLib::TCard1 c1GotFormat = 0;
    if (m_pstrDefFormat)
        c1GotFormat = 1;
    strmToWriteTo << c1GotFormat;

    if (c1GotFormat)
        strmToWriteTo << *m_pstrDefFormat;
}


// ---------------------------------------------------------------------------
//  TTime: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTime::LazyInit()
{
    //
    //  The caller already checked once, so just assume we need to lock. Here, though
    //  normally you shouldn't do this, we use the base lock since this is pretty
    //  fundamental stuff.
    //
    TBaseLock lockInit;
    if (!CIDLib_Time::atomInitDone)
    {
        // Get some quicky references to current local date/time stuff
        const tCIDLib::TCh chDateSep = TLocale::chDateSeparator();
        const tCIDLib::TCh chTimeSep = TLocale::chTimeSeparator();

        // Set up the time formatting strings
        CIDLib_Time::pstrCTime = new TString(L"%(a), %(m) %(D,2,0) ", 18UL);
        CIDLib_Time::pstrCTime->Append(L"%(H,2,0)");
        CIDLib_Time::pstrCTime->Append(chTimeSep);
        CIDLib_Time::pstrCTime->Append(L"%(u,2,0)");
        CIDLib_Time::pstrCTime->Append(chTimeSep);
        CIDLib_Time::pstrCTime->Append(L"%(s,2,0) %(Y,4,0) %(t)");

        CIDLib_Time::pstrDTStamp = new TString(L"%(a) %(n) %(D,2,0), %(Y,4,0) - ", 8UL);
        CIDLib_Time::pstrDTStamp->Append(L"%(h,2,0)");
        CIDLib_Time::pstrDTStamp->Append(chTimeSep);
        CIDLib_Time::pstrDTStamp->Append(L"%(u,2,0)%(P)");

        CIDLib_Time::pstrDDMMYY = new TString(L"%(D,2,0)", 8UL);
        CIDLib_Time::pstrDDMMYY->Append(chDateSep);
        CIDLib_Time::pstrDDMMYY->Append(L"%(M,2,0)");
        CIDLib_Time::pstrDDMMYY->Append(chDateSep);
        CIDLib_Time::pstrDDMMYY->Append(L"%(y,2,0)");

        CIDLib_Time::pstrDDMMYYYY = new TString(L"%(D,2,0)", 10UL);
        CIDLib_Time::pstrDDMMYYYY->Append(chDateSep);
        CIDLib_Time::pstrDDMMYYYY->Append(L"%(M,2,0)");
        CIDLib_Time::pstrDDMMYYYY->Append(chDateSep);
        CIDLib_Time::pstrDDMMYYYY->Append(L"%(Y,4,0)");

        CIDLib_Time::pstrFullDate = new TString(L"%(a) %(n) %(D,2,0), %(Y,4,0)");

        CIDLib_Time::pstr24HHMM = new TString(L"%(H,2,0)", 4UL);
        CIDLib_Time::pstr24HHMM->Append(chTimeSep);
        CIDLib_Time::pstr24HHMM->Append(L"%(u,2,0)");

        CIDLib_Time::pstr24HHMMSS = new TString(L"%(H,2,0)", 8UL);
        CIDLib_Time::pstr24HHMMSS->Append(chTimeSep);
        CIDLib_Time::pstr24HHMMSS->Append(L"%(u,2,0)");
        CIDLib_Time::pstr24HHMMSS->Append(chTimeSep);
        CIDLib_Time::pstr24HHMMSS->Append(L"%(s,2,0)");

        CIDLib_Time::pstr24HM = new TString(L"%(H)", 4UL);
        CIDLib_Time::pstr24HM->Append(chTimeSep);
        CIDLib_Time::pstr24HM->Append(L"%(u)");

        CIDLib_Time::pstrHM = new TString(L"%(h)", 4UL);
        CIDLib_Time::pstrHM->Append(chTimeSep);
        CIDLib_Time::pstrHM->Append(L"%(u)");

        CIDLib_Time::pstrHHMM = new TString(L"%(h,2,0)", 4UL);
        CIDLib_Time::pstrHHMM->Append(chTimeSep);
        CIDLib_Time::pstrHHMM->Append(L"%(u,2,0)");

        CIDLib_Time::pstrHHMMap = new TString(L"%(h,2,0)", 8UL);
        CIDLib_Time::pstrHHMMap->Append(chTimeSep);
        CIDLib_Time::pstrHHMMap->Append(L"%(u,2,0)");
        CIDLib_Time::pstrHHMMap->Append(kCIDLib::chSpace);
        CIDLib_Time::pstrHHMMap->Append(L"%(P)");

        CIDLib_Time::pstrHHMMSS = new TString(L"%(h,2,0)", 8UL);
        CIDLib_Time::pstrHHMMSS->Append(chTimeSep);
        CIDLib_Time::pstrHHMMSS->Append(L"%(u,2,0)");
        CIDLib_Time::pstrHHMMSS->Append(chTimeSep);
        CIDLib_Time::pstrHHMMSS->Append(L"%(s,2,0)");

        CIDLib_Time::pstrMediaTime = new TString(L"%(u)m, %(s)s");

        CIDLib_Time::pstrMMSS = new TString(L"%(u,2,0)", 4UL);
        CIDLib_Time::pstrMMSS->Append(chTimeSep);
        CIDLib_Time::pstrMMSS->Append(L"%(s,2,0)");

        CIDLib_Time::pstrMMDD_24HHMM = new TString(L"%(M,2,0)", 8UL);
        CIDLib_Time::pstrMMDD_24HHMM->Append(chDateSep);
        CIDLib_Time::pstrMMDD_24HHMM->Append(L"%(D,2,0)  %(H,2,0)");
        CIDLib_Time::pstrMMDD_24HHMM->Append(chTimeSep);
        CIDLib_Time::pstrMMDD_24HHMM->Append(L"%(u,2,0)");

        CIDLib_Time::pstrMMDD_HHMM = new TString(L"%(M,2,0)", 12UL);
        CIDLib_Time::pstrMMDD_HHMM->Append(chDateSep);
        CIDLib_Time::pstrMMDD_HHMM->Append(L"%(D,2,0)  %(h,2,0)");
        CIDLib_Time::pstrMMDD_HHMM->Append(chTimeSep);
        CIDLib_Time::pstrMMDD_HHMM->Append(L"%(u,2,0)");

        CIDLib_Time::pstrMMDD_24HHMMSS = new TString(L"%(M,2,0)", 16UL);
        CIDLib_Time::pstrMMDD_24HHMMSS->Append(chDateSep);
        CIDLib_Time::pstrMMDD_24HHMMSS->Append(L"%(D,2,0)  %(H,2,0)");
        CIDLib_Time::pstrMMDD_24HHMMSS->Append(chTimeSep);
        CIDLib_Time::pstrMMDD_24HHMMSS->Append(L"%(u,2,0)");
        CIDLib_Time::pstrMMDD_24HHMMSS->Append(chTimeSep);
        CIDLib_Time::pstrMMDD_24HHMMSS->Append(L"%(s,2,0)");

        CIDLib_Time::pstrMMDD_HHMMSS = new TString(L"%(M,2,0)", 16UL);
        CIDLib_Time::pstrMMDD_HHMMSS->Append(chDateSep);
        CIDLib_Time::pstrMMDD_HHMMSS->Append(L"%(D,2,0)  %(h,2,0)");
        CIDLib_Time::pstrMMDD_HHMMSS->Append(chTimeSep);
        CIDLib_Time::pstrMMDD_HHMMSS->Append(L"%(u,2,0)");
        CIDLib_Time::pstrMMDD_HHMMSS->Append(chTimeSep);
        CIDLib_Time::pstrMMDD_HHMMSS->Append(L"%(s,2,0)");

        CIDLib_Time::pstrMMDDYY = new TString(L"%(M,2,0)", 12UL);
        CIDLib_Time::pstrMMDDYY->Append(chDateSep);
        CIDLib_Time::pstrMMDDYY->Append(L"%(D,2,0)");
        CIDLib_Time::pstrMMDDYY->Append(chDateSep);
        CIDLib_Time::pstrMMDDYY->Append(L"%(y,2,0)");

        CIDLib_Time::pstrMMDDYYYY = new TString(L"%(M,2,0)", 12UL);
        CIDLib_Time::pstrMMDDYYYY->Append(chDateSep);
        CIDLib_Time::pstrMMDDYYYY->Append(L"%(D,2,0)");
        CIDLib_Time::pstrMMDDYYYY->Append(chDateSep);
        CIDLib_Time::pstrMMDDYYYY->Append(L"%(Y,4,0)");

        CIDLib_Time::pstrISO8601Basic = new TString
        (
            L"%(Y,4,0)%(M,2,0)%(D,2,0)T%(H,2,0)%(u,2,0)%(s,2,0)Z"
        );

        CIDLib_Time::pstrYYMMDD = new TString(L"%(y,2,0)", 12UL);
        CIDLib_Time::pstrYYMMDD->Append(chDateSep);
        CIDLib_Time::pstrYYMMDD->Append(L"%(M,2,0)");
        CIDLib_Time::pstrYYMMDD->Append(chDateSep);
        CIDLib_Time::pstrYYMMDD->Append(L"%(D,2,0)");

        CIDLib_Time::pstrYYYYMMDD = new TString(L"%(Y,4,0)", 12UL);
        CIDLib_Time::pstrYYYYMMDD->Append(chDateSep);
        CIDLib_Time::pstrYYYYMMDD->Append(L"%(M,2,0)");
        CIDLib_Time::pstrYYYYMMDD->Append(chDateSep);
        CIDLib_Time::pstrYYYYMMDD->Append(L"%(D,2,0)");

        // Same as above but no separators
        CIDLib_Time::pstrYYYYMMDD_NoSep = new TString(L"%(Y,4,0)%(M,2,0)%(D,2,0)");

        // Set up the time parsing components lists
        CIDLib_Time::pfcol12HHMM = new TTimeCompList;
        CIDLib_Time::pfcol12HHMM->c4AddElement(tCIDLib::ETimeComps::Hour12);
        CIDLib_Time::pfcol12HHMM->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcol12HHMM->c4AddElement(tCIDLib::ETimeComps::Minute);
        CIDLib_Time::pfcol12HHMM->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcol12HHMM->c4AddElement(tCIDLib::ETimeComps::AMPM);

        CIDLib_Time::pfcol24HHMM = new TTimeCompList;
        CIDLib_Time::pfcol24HHMM->c4AddElement(tCIDLib::ETimeComps::Hour24);
        CIDLib_Time::pfcol24HHMM->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcol24HHMM->c4AddElement(tCIDLib::ETimeComps::Minute);

        CIDLib_Time::pfcol24HHMMSS = new TTimeCompList;
        CIDLib_Time::pfcol24HHMMSS->c4AddElement(tCIDLib::ETimeComps::Hour24);
        CIDLib_Time::pfcol24HHMMSS->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcol24HHMMSS->c4AddElement(tCIDLib::ETimeComps::Minute);
        CIDLib_Time::pfcol24HHMMSS->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcol24HHMMSS->c4AddElement(tCIDLib::ETimeComps::Second);

        CIDLib_Time::pfcolCTime = new TTimeCompList;
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::DayName);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::MonthName);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Punct);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Hour24);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Minute);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Second);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolCTime->c4AddElement(tCIDLib::ETimeComps::Year4);

        CIDLib_Time::pfcolDDMMYY = new TTimeCompList;
        CIDLib_Time::pfcolDDMMYY->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolDDMMYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolDDMMYY->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolDDMMYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolDDMMYY->c4AddElement(tCIDLib::ETimeComps::Year2);

        CIDLib_Time::pfcolDDMMYYYY = new TTimeCompList;
        CIDLib_Time::pfcolDDMMYYYY->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolDDMMYYYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolDDMMYYYY->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolDDMMYYYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolDDMMYYYY->c4AddElement(tCIDLib::ETimeComps::Year4);

        CIDLib_Time::pfcolDTStamp = new TTimeCompList;
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::DayName);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::MonthName);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Year4);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Punct);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Hour24);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::Minute);
        CIDLib_Time::pfcolDTStamp->c4AddElement(tCIDLib::ETimeComps::AMPM);

        CIDLib_Time::pfcolFullDate = new TTimeCompList;
        CIDLib_Time::pfcolFullDate->c4AddElement(tCIDLib::ETimeComps::DayName);
        CIDLib_Time::pfcolFullDate->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolFullDate->c4AddElement(tCIDLib::ETimeComps::MonthName);
        CIDLib_Time::pfcolFullDate->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolFullDate->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolFullDate->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolFullDate->c4AddElement(tCIDLib::ETimeComps::Year4);

        CIDLib_Time::pfcolISO8601NTZ = new TTimeCompList;
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Year4);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Hyphen);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Hyphen);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::T);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Hour24);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Colon);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Minute);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Second);
        CIDLib_Time::pfcolISO8601NTZ->c4AddElement(tCIDLib::ETimeComps::Minute);

        CIDLib_Time::pfcolMMDDYY = new TTimeCompList;
        CIDLib_Time::pfcolMMDDYY->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolMMDDYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolMMDDYY->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolMMDDYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolMMDDYY->c4AddElement(tCIDLib::ETimeComps::Year2);

        CIDLib_Time::pfcolMMDDYYYY = new TTimeCompList;
        CIDLib_Time::pfcolMMDDYYYY->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolMMDDYYYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolMMDDYYYY->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolMMDDYYYY->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolMMDDYYYY->c4AddElement(tCIDLib::ETimeComps::Year4);

        CIDLib_Time::pfcolRFC822 = new TTimeCompList;
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::DayName);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Punct);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::MonthName);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Year4);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Hour24);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Minute);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Second);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolRFC822->c4AddElement(tCIDLib::ETimeComps::TZ);

        CIDLib_Time::pfcolYYMMDD = new TTimeCompList;
        CIDLib_Time::pfcolYYMMDD->c4AddElement(tCIDLib::ETimeComps::Year2);
        CIDLib_Time::pfcolYYMMDD->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolYYMMDD->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolYYMMDD->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolYYMMDD->c4AddElement(tCIDLib::ETimeComps::DayNum);

        CIDLib_Time::pfcolYYYYMMDD = new TTimeCompList;
        CIDLib_Time::pfcolYYYYMMDD->c4AddElement(tCIDLib::ETimeComps::Year4);
        CIDLib_Time::pfcolYYYYMMDD->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolYYYYMMDD->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolYYYYMMDD->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolYYYYMMDD->c4AddElement(tCIDLib::ETimeComps::DayNum);

        CIDLib_Time::pfcolYYYYMMDD24HHMM = new TTimeCompList;
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::Year4);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::MonthNum);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::DateSep);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::DayNum);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::Space);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::Hour24);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::TimeSep);
        CIDLib_Time::pfcolYYYYMMDD24HHMM->c4AddElement(tCIDLib::ETimeComps::Minute);

        // And set the flag LAST!
        CIDLib_Time::atomInitDone.Set();
    }
}


tCIDLib::TVoid TTime::StoreValue(const tCIDLib::TEncodedTime& enctToStore)
{
    if (!m_ktmsThis.bSetTo(enctToStore))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTime_SetTo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal64(enctToStore)
        );
    }
}
