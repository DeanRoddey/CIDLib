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
//  1)  The __enctTime field in the Win32 implementation is always a local
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
TCIDKrnlModule::__bInitTermTime(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        ::tzset();

        TKrnlTimeStamp::__bDST = daylight ? kCIDLib::True : kCIDLib::False;
        TKrnlTimeStamp::__i4Offset = timezone / 60 / -60;

        tCIDLib::TCard4 c4Index = TKrnlTimeStamp::__bDST ? 1 : 0;
        TRawStr::pszConvert(tzname[c4Index], TKrnlTimeStamp::__szTZName, 32);
    }

    return kCIDLib::True;
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
    TKrnlLinux::CIDFileTimeToLinuxFileTime(__enctTime, LinuxTime);

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
    TKrnlLinux::CIDFileTimeToLinuxFileTime(__enctTime, LinuxTime);

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
    TKrnlLinux::CIDFileTimeToLinuxFileTime(__enctTime, LinuxTime);

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
    TimeStruct.tm_mon = eMonth;
    TimeStruct.tm_year = c4Year - 1900;

    time_t LinuxTime = ::mktime(&TimeStruct);

    if (LinuxTime == -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    TKrnlLinux::LinuxFileTimeToCIDFileTime(LinuxTime - timezone, __enctTime);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlTimeStamp::bJulianDate(tCIDLib::TCard4& c4ToFill) const
{
    time_t LinuxTime;
    TKrnlLinux::CIDFileTimeToLinuxFileTime(__enctTime, LinuxTime);

    struct tm TimeStruct;
    if (!::gmtime_r(&LinuxTime, &TimeStruct))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidValue);
        return kCIDLib::False;
    }

    c4ToFill = TimeStruct.tm_yday;

    return kCIDLib::True;
}



tCIDLib::TBoolean TKrnlTimeStamp::bSetTo(const tCIDLib::TEncodedTime& enctNew)
{
    __enctTime = enctNew;
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlTimeStamp::bSetTo(const tCIDLib::ESpecialTimes eSpecial)
{
    time_t LinuxTime;

    if (eSpecial == tCIDLib::ESpecialTimes::Base)
    {
        LinuxTime = 0;
        TKrnlLinux::LinuxFileTimeToCIDFileTime(LinuxTime, __enctTime);
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

    TKrnlLinux::LinuxFileTimeToCIDFileTime(LinuxTime, __enctTime);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlTimeStamp::bValidate() const
{
    return __enctTime >= 0;
}
