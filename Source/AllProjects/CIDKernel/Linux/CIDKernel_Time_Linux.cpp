//
// FILE NAME: CIDKernel_Time_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 09/12/1998
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
//  This file implements the platform specific aspects of the TKrnlTimeStamp
//  class.
//
// CAVEATS/GOTCHAS:
//
//  1)  The m_enctTime field in the Win32 implementation is always a local
//      time, i.e. we adjust any UTC times we get from the system to be local
//      times.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermTime(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        ::tzset();
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Public, static methods
// ---------------------------------------------------------------------------

// <TBD> Do we have this on Linux?
tCIDLib::TBoolean TKrnlTimeStamp::bHighResTimerAvailable()
{
    return kCIDLib::False;
}


// <TBD> Do we have this on Linux?
tCIDLib::TBoolean
TKrnlTimeStamp::bHighResDelay(const tCIDLib::TCard4 c4MicroSecs)
{
    if (c4MicroSecs < 1000)
        TKrnlThread::Sleep(1);
    else
        TKrnlThread::Sleep(c4MicroSecs / 1000);    
}


tCIDLib::TBoolean TKrnlTimeStamp::bIsDST(tCIDLib::TBoolean& bDSTState)
{
    time_t tmCur = ::time(0);
    if (tmCur == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                
    }

    tm brokenTime;
    if (::localtime_r(&tmCur, &brokenTime) == 0)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;        
    }
    bDSTState = brokenTime.tm_isdst != 0;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlTimeStamp::bTZOffset(tCIDLib::TInt4& i4Ofs)
{
    timeval tv;
    struct timezone tz;

    if (::gettimeofday(&tv, &tz))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                
    }

    i4Ofs = tz.tz_minuteswest;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bTZName(tCIDLib::TCh* pszToFill, const tCIDLib::TCard4 c4MaxChars)
{
    timeval tv;
    struct timezone tz;

    if (::gettimeofday(&tv, &tz))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                        
    }

    return TRawStr::bFormatVal(tCIDLib::TInt4(tz.tz_minuteswest), pszToFill, c4MaxChars);
    return kCIDLib::True;
}


// For us this is the same as local to UTC
tCIDLib::TBoolean
TKrnlTimeStamp::bLocalToNative( const   tCIDLib::TEncodedTime& enctLocal
                                ,       tCIDLib::TEncodedTime& enctNative)
{
    return bLocalToUTC(enctLocal, enctNative);
}


tCIDLib::TBoolean
TKrnlTimeStamp::bLocalToUTC(const   tCIDLib::TEncodedTime& enctLocal
                            ,       tCIDLib::TEncodedTime& enctNative)
{
    // Conver the local to the Linux type stame format
    time_t LocalTime = time_t(enctLocal / kCIDLib::enctOneSecond);

    struct tm GMTime;
    if (::gmtime_r(&LocalTime, &GMTime) == 0)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                        
    }

    // And now multiply this back up
    const time_t EpochTime = ::mktime(&GMTime);
    enctNative = tCIDLib::TEncodedTime(EpochTime) * kCIDLib::enctOneSecond;
    return kCIDLib::True;    
}


tCIDLib::TBoolean
TKrnlTimeStamp::bUTCToLocal(const   tCIDLib::TEncodedTime& enctUTC
                            ,       tCIDLib::TEncodedTime& enctLocal)
{
    // Conver the UTC to the Linux format
    time_t UTCTime = time_t(enctUTC / kCIDLib::enctOneSecond);

    struct tm LocalTime;
    if (::localtime_r(&UTCTime, &LocalTime) == 0)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                        
    }

    // And now multiply this back up to the Epoch type style
    const time_t EpochTime = ::mktime(&LocalTime);
    enctLocal = tCIDLib::TEncodedTime(EpochTime) * kCIDLib::enctOneSecond;
    return kCIDLib::True;    
}


// Return the curent millisecond tick count, 32 and 64 bit versions
tCIDLib::TCard4 TKrnlTimeStamp::c4Millis()
{
    time_t tmCur = ::time(0);
    if (tmCur == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                
    }

    // Just return seconds time a thousand
    return tCIDLib::TCard4(tmCur) * 1000;
}

tCIDLib::TCard8 TKrnlTimeStamp::c8Millis()
{
    time_t tmCur = ::time(0);
    if (tmCur == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                
    }

    // Just return seconds time a thousand
    return tCIDLib::TCard8(tmCur) * 1000;
}


tCIDLib::TVoid
TKrnlTimeStamp::CurNTPTime(         tCIDLib::TCard4&    c4Secs
                            ,       tCIDLib::TCard4&    c4Fract
                            , const tCIDLib::TBoolean   bNWOrder)
{
    time_t tmCur = ::time(0);
    c4Secs = tCIDLib::TCard4(tmCur);
    c4Fract = 0;    
}


tCIDLib::TEncodedTime TKrnlTimeStamp::enctNow()
{
    time_t tmCur = ::time(0);
    return tCIDLib::TEncodedTime(tmCur) * kCIDLib::enctOneSecond;
}


tCIDLib::TEncodedTime TKrnlTimeStamp::enctNowUTC()
{
    time_t tmCur = ::time(0);

    struct tm GMTime;
    if (::gmtime_r(&tmCur, &GMTime) == 0)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;                        
    }

    // And now multiply this back up
    const time_t EpochTime = ::mktime(&GMTime);
}


// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlTimeStamp::bAsDateInfo(tCIDLib::TCard4&        c4Year
                            , tCIDLib::EMonths&     eMonth
                            , tCIDLib::TCard4&      c4Day
                            , tCIDLib::EWeekDays&   eWeekDay) const
{
    time_t LinuxTime;
    TKrnlLinux::CIDFileTimeToLinuxFileTime(m_enctTime, LinuxTime);

    struct tm TimeStruct;
    if (!::gmtime_r(&LinuxTime, &TimeStruct))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4Year = TimeStruct.tm_year + 1900;
    eMonth = tCIDLib::EMonths(TimeStruct.tm_mon);
    c4Day = TimeStruct.tm_mday;
    eWeekDay = tCIDLib::EWeekDays(TimeStruct.tm_wday);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bAsTimeInfo(tCIDLib::TCard4&    c4Hour
                            , tCIDLib::TCard4&  c4Minute
                            , tCIDLib::TCard4&  c4Second
                            , tCIDLib::TCard4&  c4Millis) const
{
    time_t LinuxTime;
    TKrnlLinux::CIDFileTimeToLinuxFileTime(m_enctTime, LinuxTime);

    struct tm TimeStruct;
    if (!::gmtime_r(&LinuxTime, &TimeStruct))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4Hour = TimeStruct.tm_hour;
    c4Minute = TimeStruct.tm_min;
    c4Second = TimeStruct.tm_sec;
    c4Millis = 0;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bDayOfWeek(tCIDLib::EWeekDays& eToFill) const
{
    time_t LinuxTime;
    TKrnlLinux::CIDFileTimeToLinuxFileTime(m_enctTime, LinuxTime);

    struct tm TimeStruct;
    if (!::gmtime_r(&LinuxTime, &TimeStruct))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidValue);
        return kCIDLib::False;
    }

    eToFill = tCIDLib::EWeekDays(TimeStruct.tm_wday);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bFromDetails(const  tCIDLib::TCard4     c4Year
                            , const tCIDLib::EMonths    eMonth
                            , const tCIDLib::TCard4     c4Day
                            , const tCIDLib::TCard4     c4Hour
                            , const tCIDLib::TCard4     c4Minute
                            , const tCIDLib::TCard4     c4Second
                            , const tCIDLib::TCard4     c4MilliSecs)
{
    struct tm TimeStruct;

    ::memset(&TimeStruct, 0, sizeof(TimeStruct));

    TimeStruct.tm_sec = c4Second;
    TimeStruct.tm_min = c4Minute;
    TimeStruct.tm_hour = c4Hour;
    TimeStruct.tm_mday = c4Day;
    TimeStruct.tm_mon = int(eMonth);
    TimeStruct.tm_year = c4Year - 1900;

    time_t LinuxTime = ::mktime(&TimeStruct);

    if (LinuxTime == -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    TKrnlLinux::LinuxFileTimeToCIDFileTime(LinuxTime - timezone, m_enctTime);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlTimeStamp::bSetTo(const tCIDLib::TEncodedTime& enctNew)
{
    m_enctTime = enctNew;
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlTimeStamp::bSetTo(const tCIDLib::ESpecialTimes eSpecial)
{
    time_t LinuxTime;

    if (eSpecial == tCIDLib::ESpecialTimes::Base)
    {
        LinuxTime = 0;
        TKrnlLinux::LinuxFileTimeToCIDFileTime(LinuxTime, m_enctTime);
        return kCIDLib::True;
    }

    LinuxTime = ::time(0);
    if (eSpecial == tCIDLib::ESpecialTimes::CurrentTime)
    {
        LinuxTime -= timezone;
        LinuxTime += daylight ? 3600 : 0;
    }
    else if (eSpecial == tCIDLib::ESpecialTimes::CurrentUTC)
    {
        // nothing to do
    }
    else
    {
        struct tm TimeStruct;
        if (::localtime_r(&LinuxTime, &TimeStruct) == 0)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcHostErr);
            return kCIDLib::False;
        }

        // And jiggle it as required
        if (eSpecial == tCIDLib::ESpecialTimes::CurrentDate)
        {
            TimeStruct.tm_sec = 0;
            TimeStruct.tm_min = 0;
            TimeStruct.tm_hour = 0;
        }
        else if (eSpecial == tCIDLib::ESpecialTimes::NoonToday)
        {
            TimeStruct.tm_sec = 0;
            TimeStruct.tm_min = 0;
            TimeStruct.tm_hour = 11;
        }

        LinuxTime = ::mktime(&TimeStruct);

        if (LinuxTime == -1)
        {
            TKrnlError::SetLastHostError(kKrnlErrs::errcHostErr);
            return kCIDLib::False;
        }
    }

    TKrnlLinux::LinuxFileTimeToCIDFileTime(LinuxTime, m_enctTime);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlTimeStamp::bValidate() const
{
    return m_enctTime >= 0;
}
