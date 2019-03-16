//
// FILE NAME: CIDKernel_Time_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//  This file implements the platform specific aspects of the TKrnlTimeStamp
//  class.
//
// CAVEATS/GOTCHAS:
//
//  1)  The enctTime field in the Win32 implementation is always a local
//      time, i.e. we adjust any UTC times we get from the system to be local
//      times, except where UTC is explicitly requested.
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
//  Local data
// ---------------------------------------------------------------------------
namespace CIDKernel_Time_Win32
{
    volatile tCIDLib::TBoolean  bHighResInit = kCIDLib::False;
    volatile tCIDLib::TBoolean  bHighResAvail = kCIDLib::False;
    tCIDLib::TCard4             c4MinUSecs;
    tCIDLib::TInt8              i8TicksPerUSec;
}



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDKrnlModule::bInitTermTime(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        #if 0
        // -------------------------------------------------------------------
        //  Since Win32 uses a base time of midnight 01/01/1601, and our
        //  portable semantics uses a time of midnight 01/01/1970, we have to
        //  adjust any times in the encoded format (since zero encoded time
        //  represents the 1970 base time.) This is set up during init of the
        //  time/date module, to hold the correct offset for our local time
        //  zone.
        //
        //  We pre-calculate this value and put it into a public static member
        //  but keep the code around in case we have to recalculate it.
        // -------------------------------------------------------------------
        SYSTEMTIME  SysTime;

        SysTime.wYear       = 1970;
        SysTime.wMonth      = 1;
        SysTime.wDay        = 1;
        SysTime.wHour       = 0;
        SysTime.wMinute     = 0;
        SysTime.wSecond     = 0;
        SysTime.wMilliseconds= 0;

        FILETIME FlTime;
        if (!::SystemTimeToFileTime(&SysTime, &FlTime))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }

        // Convert to 64 bit int and store the local base variable
        CIDKernel_Time_Win32::enctBaseTime = TRawBits::c8From32
        (
            FlTime.dwLowDateTime, FlTime.dwHighDateTime
        );
        #endif
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Public, static data
// ---------------------------------------------------------------------------
const tCIDLib::TEncodedTime TKrnlTimeStamp::enctBaseOfs = 116444736000000000;
const tCIDLib::TEncodedTime TKrnlTimeStamp::enctNTPOfs  = 116444738208988800;



// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Public, static methods
// ---------------------------------------------------------------------------

//
//  Indicates whether high resolution timers are available on this machine.
//  If not, then the high res delay methods below will not have any better
//  than system time slice granularity necessarily.
//
tCIDLib::TBoolean TKrnlTimeStamp::bHighResTimerAvailable()
{
    // If we've not initializd the high res timer stuff yet, then do so
    if (!CIDKernel_Time_Win32::bHighResInit)
    {
        TBaseLock lockInit;
        if (!CIDKernel_Time_Win32::bHighResInit)
        {
            LARGE_INTEGER Freq;
            if (::QueryPerformanceFrequency(&Freq))
            {
                // Make sure we actually have counter support also
                LARGE_INTEGER TestTicks;
                if (::QueryPerformanceCounter(&TestTicks))
                {
                    //
                    //  We allow sleeps for microsecond intervals, so calculate
                    //  the ticks per microsecond.
                    //
                    CIDKernel_Time_Win32::i8TicksPerUSec = Freq.QuadPart / 1000000;
                    CIDKernel_Time_Win32::bHighResAvail = kCIDLib::True;
                }
            }
             else
            {
                CIDKernel_Time_Win32::bHighResAvail = kCIDLib::False;
            }
            CIDKernel_Time_Win32::bHighResInit = kCIDLib::True;
        }
    }
    return CIDKernel_Time_Win32::bHighResAvail;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bHighResDelay(const tCIDLib::TCard4 c4MicroSecs)
{
    // If we've not initializd the high res timer stuff yet, then do so
    if (!CIDKernel_Time_Win32::bHighResInit)
    {
        TBaseLock lockInit;
        if (!CIDKernel_Time_Win32::bHighResInit)
            bHighResTimerAvailable();
    }

    // If no support, then just sleep for millis
    if (!CIDKernel_Time_Win32::bHighResAvail)
    {
        if (c4MicroSecs < 1000)
            ::Sleep(1);
        else
            Sleep(c4MicroSecs / 1000);
    }
     else
    {
        // Calculate the ticks we'll need to wait for
        const tCIDLib::TInt8 i8TicksToWait
        (
            c4MicroSecs * CIDKernel_Time_Win32::i8TicksPerUSec
        );

        // Get the current timer
        LARGE_INTEGER CurTicks;
        if (!::QueryPerformanceCounter(&CurTicks))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }

        // Calculate the end ticks
        const tCIDLib::TInt8 i8EndTicks = CurTicks.QuadPart + i8TicksToWait;

        // Now loop until it its the required value
        do
        {
            if (!::QueryPerformanceCounter(&CurTicks))
            {
                TKrnlError::SetLastHostError(::GetLastError());
                return kCIDLib::False;
            }
        }   while (CurTicks.QuadPart < i8EndTicks);
    }
    return kCIDLib::True;
}


// Return the daylist savings time state
tCIDLib::TBoolean TKrnlTimeStamp::bIsDST(tCIDLib::TBoolean& bDSTState)
{
    // Query time zone information
    TIME_ZONE_INFORMATION TZInfo;
    tCIDLib::TCard4 c4Res = ::GetTimeZoneInformation(&TZInfo);

    if ((c4Res == TIME_ZONE_ID_STANDARD)
    ||  (c4Res == TIME_ZONE_ID_UNKNOWN))
    {
        bDSTState = kCIDLib::False;
    }
     else if (c4Res == TIME_ZONE_ID_DAYLIGHT)
    {
        bDSTState = kCIDLib::True;
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_LoadTZInfo);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// Return the time zone offset
tCIDLib::TBoolean TKrnlTimeStamp::bTZOffset(tCIDLib::TInt4& i4Ofs)
{
    TIME_ZONE_INFORMATION TZInfo;
    tCIDLib::TCard4 c4Res = ::GetTimeZoneInformation(&TZInfo);

    if ((c4Res == TIME_ZONE_ID_STANDARD)
    ||  (c4Res == TIME_ZONE_ID_UNKNOWN))
    {
        i4Ofs = (TZInfo.Bias + TZInfo.StandardBias);
    }
     else if (c4Res == TIME_ZONE_ID_DAYLIGHT)
    {
        i4Ofs = (TZInfo.Bias + TZInfo.DaylightBias);
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_LoadTZInfo);
        return kCIDLib::False;
    }

    //
    //  The offset value we get from Windows is backwards from what we
    //  want. It gives us the amount to adjust local time to get UTC. We
    //  want to store the amount to adjust UTC to get local time.
    //
    i4Ofs *= -1;
    return kCIDLib::True;
}


// Return the time zone name
tCIDLib::TBoolean
TKrnlTimeStamp::bTZName(tCIDLib::TCh* pszToFill, const tCIDLib::TCard4 c4MaxChars)
{
    TIME_ZONE_INFORMATION TZInfo;
    tCIDLib::TCard4 c4Res = ::GetTimeZoneInformation(&TZInfo);

    if ((c4Res == TIME_ZONE_ID_STANDARD)
    ||  (c4Res == TIME_ZONE_ID_UNKNOWN))
    {
        TRawStr::CopyStr(pszToFill, TZInfo.StandardName, c4MaxChars);
    }
     else if (c4Res == TIME_ZONE_ID_DAYLIGHT)
    {
        TRawStr::CopyStr(pszToFill, TZInfo.DaylightName, c4MaxChars);
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_LoadTZInfo);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Convert from our local to native UTC. This is the same as local to UTC,
//  except we add back in the base offset that we normally subtract from
//  native time stamps.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bLocalToNative( const   tCIDLib::TEncodedTime& enctLocal
                                ,       tCIDLib::TEncodedTime& enctNative)
{
    FILETIME LocTime;
    LocTime.dwLowDateTime = TRawBits::c4Low32From64(enctLocal);
    LocTime.dwHighDateTime = TRawBits::c4High32From64(enctLocal);

    FILETIME UTCTime;
    if (!::LocalFileTimeToFileTime(&LocTime, &UTCTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    enctNative = TRawBits::c8From32(UTCTime.dwLowDateTime, UTCTime.dwHighDateTime);
    enctNative += TKrnlTimeStamp::enctBaseOfs;

    return kCIDLib::True;
}



//
//  Convert a time from local to UTC, or UTC to local. Note that we don't
//  have to deal with the base offset here. The relative time will be adjusted
//  by the correct amount, so the resulting time will still be relative to the
//  base offset.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bLocalToUTC(const   tCIDLib::TEncodedTime& enctLocal
                            ,       tCIDLib::TEncodedTime& enctUTC)
{
    FILETIME LocTime;
    LocTime.dwLowDateTime = TRawBits::c4Low32From64(enctLocal);
    LocTime.dwHighDateTime = TRawBits::c4High32From64(enctLocal);

    FILETIME UTCTime;
    if (!::LocalFileTimeToFileTime(&LocTime, &UTCTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    enctUTC = TRawBits::c8From32(UTCTime.dwLowDateTime, UTCTime.dwHighDateTime);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bUTCToLocal(const   tCIDLib::TEncodedTime& enctUTC
                            ,       tCIDLib::TEncodedTime& enctLocal)
{
    FILETIME UTCTime;
    UTCTime.dwLowDateTime = TRawBits::c4Low32From64(enctUTC);
    UTCTime.dwHighDateTime = TRawBits::c4High32From64(enctUTC);

    FILETIME LocTime;
    if (!::FileTimeToLocalFileTime(&UTCTime, &LocTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    enctLocal = TRawBits::c8From32(LocTime.dwLowDateTime, LocTime.dwHighDateTime);
    return kCIDLib::True;
}


// Return the curent millisecond tick count, 32 and 64 bit versions
tCIDLib::TCard4 TKrnlTimeStamp::c4Millis()
{
    return ::GetTickCount();
}

tCIDLib::TCard8 TKrnlTimeStamp::c8Millis()
{
    return ::GetTickCount64();
}


//
//  Get the current time as an NTP type stamp, of seconds and fractional seconds
//  since 1900, UTC.
//
tCIDLib::TVoid
TKrnlTimeStamp::CurNTPTime(         tCIDLib::TCard4&    c4Secs
                            ,       tCIDLib::TCard4&    c4Fract
                            , const tCIDLib::TBoolean   bNWOrder)
{
    // Get the UTC time
    FILETIME FlTime;
    ::GetSystemTimeAsFileTime(&FlTime);

    //
    //  Sub constant to it that represents the base NTP time. We have to get
    //  it to 8 byte format first.
    //
    tCIDLib::TCard8 c8Tmp = TRawBits::c8From32(FlTime.dwLowDateTime, FlTime.dwHighDateTime);

    // This is relative to 1601, so we have to subtrct the NTP base time
    c8Tmp -= TKrnlTimeStamp::enctNTPOfs;

    // Convert to seconds and fractional seconds
    c4Secs = tCIDLib::TCard4(c8Tmp / kCIDLib::enctOneSecond);

    // Get what's left as 100ns ticks
    const tCIDLib::TCard4 c4Left = tCIDLib::TCard4
    (
        c8Tmp - (c4Secs * kCIDLib::enctOneSecond)
    );

    //
    //  The fraction is the full range (32 bit value) divided by the units, which
    //  is 100ns in our case, times the number of 100ns ticks.
    //
    c4Fract = tCIDLib::TCard4(c4Left * ((2 ^ 32) / 1.0e-9));

    //
    //  If they want it in network order nd we are little endian, then flip the
    //  two values.
    //
    #if CID_LITTLE_ENDIAN
    if (bNWOrder)
    {
        c4Secs = TRawBits::c4SwapBytes(c4Secs);
        c4Fract = TRawBits::c4SwapBytes(c4Fract);
    }
    #endif
}


// Return the local 100ns tick count, in local and UTC formats
tCIDLib::TEncodedTime TKrnlTimeStamp::enctNow()
{
    // Get the local time and convert to a file time that we can use
    SYSTEMTIME  SysTime;
    ::GetLocalTime(&SysTime);

    FILETIME FlTime;
    if (!::SystemTimeToFileTime(&SysTime, &FlTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert it to a time stamp
    const tCIDLib::TEncodedTime enctRet = TRawBits::c8From32
    (
        FlTime.dwLowDateTime, FlTime.dwHighDateTime
    );

    // Return it, adjusted for our time base
    return enctRet - TKrnlTimeStamp::enctBaseOfs;
}

tCIDLib::TEncodedTime TKrnlTimeStamp::enctNowUTC()
{
    // Get the system time (which is UTC) in the file time format we need
    FILETIME UTCTime;
    ::GetSystemTimeAsFileTime(&UTCTime);

    // Convert to a 64 bit value
    const tCIDLib::TEncodedTime enctRet = TRawBits::c8From32
    (
        UTCTime.dwLowDateTime, UTCTime.dwHighDateTime
    );

    // And adjust for our offset
    return enctRet - TKrnlTimeStamp::enctBaseOfs;
}



// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Break out the date related fields from our current time stamp
tCIDLib::TBoolean
TKrnlTimeStamp::bAsDateInfo(tCIDLib::TCard4&        c4Year
                            , tCIDLib::EMonths&     eMonth
                            , tCIDLib::TCard4&      c4Day
                            , tCIDLib::EWeekDays&   eWeekDay) const
{
    // Get our time into the file time, adjusting for our portable base
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    //
    //  Convert to a system time. Since our file time is a locally adjusted
    //  file time, this will be a locally adjusted system time.
    //
    SYSTEMTIME  SysTime;
    if (!::FileTimeToSystemTime(&FlTime, &SysTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    c4Year      = SysTime.wYear;
    eMonth      = tCIDLib::EMonths(SysTime.wMonth - 1);
    c4Day       = SysTime.wDay;
    eWeekDay    = tCIDLib::EWeekDays(SysTime.wDayOfWeek);
    return kCIDLib::True;
}


// Break out the time related fields from our current time stamp
tCIDLib::TBoolean
TKrnlTimeStamp::bAsTimeInfo(tCIDLib::TCard4&    c4Hour
                            , tCIDLib::TCard4&  c4Minute
                            , tCIDLib::TCard4&  c4Second
                            , tCIDLib::TCard4&  c4Millis) const
{
    // Get our time into the file time, adjusting for our portable base
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    //
    //  Convert to a system time. Since our file time is a locally adjusted
    //  file time, this will be a locally adjusted system time.
    //
    SYSTEMTIME  SysTime;
    if (!::FileTimeToSystemTime(&FlTime, &SysTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    c4Hour      = SysTime.wHour;
    c4Minute    = SysTime.wMinute;
    c4Second    = SysTime.wSecond;
    c4Millis    = SysTime.wMilliseconds;

    return kCIDLib::True;
}


// Get the time of week for our current time stamp
tCIDLib::TBoolean
TKrnlTimeStamp::bDayOfWeek(tCIDLib::EWeekDays& eToFill) const
{
    // Get our time into the file time, adjusting for our portable base
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    //
    //  Convert to a system time. Since our file time is a locally adjusted
    //  file time, this will be a locally adjusted system time.
    //
    SYSTEMTIME  SysTime;
    ::FileTimeToSystemTime(&FlTime, &SysTime);
    if (SysTime.wDayOfWeek > tCIDLib::c4EnumOrd(tCIDLib::EWeekDays::Max))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_InvalidDayOfWeek);
        return kCIDLib::False;
    }

    eToFill = tCIDLib::EWeekDays(SysTime.wDayOfWeek);
    return kCIDLib::True;
}


//
//  Expand out all of the information from our current timestamp. Calling
//  the separate methods might create an inconsistency in some uses.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bExpandDetails( tCIDLib::TCard4&            c4Year
                                , tCIDLib::EMonths&         eMonth
                                , tCIDLib::TCard4&          c4Day
                                , tCIDLib::EWeekDays&       eWeekDay
                                , tCIDLib::TCard4&          c4Hour
                                , tCIDLib::TCard4&          c4Minute
                                , tCIDLib::TCard4&          c4Second
                                , tCIDLib::TCard4&          c4Millis
                                , tCIDLib::TEncodedTime&    enctStamp) const
{
    // Get our time into the file time, adjusting for our portable base
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    //
    //  Convert to a system time. Since our file time is a locally adjusted
    //  file time, this will be a locally adjusted system time.
    //
    SYSTEMTIME  SysTime;
    if (!::FileTimeToSystemTime(&FlTime, &SysTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // And now break out the values
    c4Hour      = SysTime.wHour;
    c4Minute    = SysTime.wMinute;
    c4Second    = SysTime.wSecond;
    c4Millis    = SysTime.wMilliseconds;
    c4Year      = SysTime.wYear;
    eMonth      = tCIDLib::EMonths(SysTime.wMonth - 1);
    c4Day       = SysTime.wDay;
    eWeekDay    = tCIDLib::EWeekDays(SysTime.wDayOfWeek);

    // And finally give back the raw stamp
    enctStamp = m_enctTime;
    return kCIDLib::True;
}



//
//  Moves the time forward a number of days dealing with the issues of DST,
//  leap years and so forth. For days, we convert to UTC where a day is
//  always 24 hours and then convert back to the local time zone, so that
//  one is easy enough.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bForwardByDays(const tCIDLib::TCard4 c4Count)
{
    // Get our time into the file time, adjusting for our portable base
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    FILETIME UTCTime;
    if (!::LocalFileTimeToFileTime(&FlTime, &UTCTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // OK, we have a UTC encoded version of the time, so add the offset
    tCIDLib::TEncodedTime enctNew = TRawBits::c8From32
    (
        UTCTime.dwLowDateTime, UTCTime.dwHighDateTime
    );
    enctNew += (kCIDLib::enctOneDay * c4Count);

    //
    //  And now convert back. This should now adjust for any time zone
    //  differences. If this created a bad time, then the conversion won't
    //  work and we will not store the new date.
    //
    UTCTime.dwLowDateTime = TRawBits::c4Low32From64(enctNew);
    UTCTime.dwHighDateTime = TRawBits::c4High32From64(enctNew);
    if (!::FileTimeToLocalFileTime(&UTCTime, &FlTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_enctTime = TRawBits::c8From32(FlTime.dwLowDateTime, FlTime.dwHighDateTime);

    // And adjust for the portable base time
    m_enctTime = c8Tmp  - TKrnlTimeStamp::enctBaseOfs;

    return kCIDLib::True;
}

//
//  For months we pull out the details, move the month forward, adding years
//  if they go more than 12 months, then we check the date to see if we hit
//  a leap day and adjust if so. Here, we don't have to deal with DST since
//  we are dealing in years, months, days, so when we convert back, DST will
//  have been taken care of.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bForwardByMonths(const tCIDLib::TCard4 c4Count)
{
    // Get our time into the file time, adjusting for our portable base
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    //
    //  Convert to a system time. Since our file time is a locally adjusted
    //  file time, this will be a locally adjusted system time.
    //
    SYSTEMTIME  SysTime;
    if (!::FileTimeToSystemTime(&FlTime, &SysTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  And now break out the values. Adjust the years and months to cover
    //  the number of passed months, then correct for landing on a leap
    //  day.
    //
    SysTime.wYear += WORD(c4Count / 12);
    SysTime.wMonth += WORD(c4Count % 12);
    if (!bIsValidDate(  SysTime.wYear
                        , tCIDLib::EMonths(SysTime.wMonth - 1)
                        , SysTime.wDay))
    {
        SysTime.wMonth++;
        SysTime.wDay = 1;
    }

    //
    //  Now convert back. To avoid replication we just call another method.
    //  If this fails, we never stored anything.
    //
    return bFromDetails
    (
        SysTime.wYear
        , tCIDLib::EMonths(SysTime.wMonth - 1)
        , SysTime.wDay
        , SysTime.wHour
        , SysTime.wMinute
        , SysTime.wSecond
        , SysTime.wMilliseconds
    );
}


//
//  For years, we do similarly to months above. WE break out the details and
//  move the year forward. We see if we hit a leap day and adjust forward if
//  so. Then we convert back and that deals with the DST issues for us.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bForwardByYears(const tCIDLib::TCard4 c4Count)
{
    // Get our time into the file time, adjusting for our portable base
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;
    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    //
    //  Convert to a system time. Since our file time is a locally adjusted
    //  file time, this will be a locally adjusted system time.
    //
    SYSTEMTIME  SysTime;
    if (!::FileTimeToSystemTime(&FlTime, &SysTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  And now break out the values. Adjust the years value and see if
    //  the month is valid for the resulting year. If not, then adjust
    //  for the leap year.
    //
    SysTime.wYear += WORD(c4Count);
    if (!bIsValidDate(  SysTime.wYear
                        , tCIDLib::EMonths(SysTime.wMonth - 1)
                        , SysTime.wDay))
    {
        SysTime.wMonth++;
        SysTime.wDay = 1;
    }

    //
    //  Now convert back. To avoid replication we just call another method.
    //  If this fails, we never stored anything.
    //
    return bFromDetails
    (
        SysTime.wYear
        , tCIDLib::EMonths(SysTime.wMonth - 1)
        , SysTime.wDay
        , SysTime.wHour
        , SysTime.wMinute
        , SysTime.wSecond
        , SysTime.wMilliseconds
    );
}


//
//  Creates a time stamp from the passed raw values. If the system says
//  they are wrong, we return an error.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bFromDetails(const  tCIDLib::TCard4     c4Year
                            , const tCIDLib::EMonths    eMonth
                            , const tCIDLib::TCard4     c4Day
                            , const tCIDLib::TCard4     c4Hour
                            , const tCIDLib::TCard4     c4Minute
                            , const tCIDLib::TCard4     c4Second
                            , const tCIDLib::TCard4     c4MilliSecs)
{
    SYSTEMTIME  SysTime = {0};

    SysTime.wYear       = WORD(c4Year);
    SysTime.wMonth      = WORD(eMonth) + 1;
    SysTime.wDay        = WORD(c4Day);
    SysTime.wHour       = WORD(c4Hour);
    SysTime.wMinute     = WORD(c4Minute);
    SysTime.wSecond     = WORD(c4Second);
    SysTime.wMilliseconds=WORD(c4MilliSecs);

    //
    //  Assume that the system time info we get is locally adjusted time,
    //  so the file time we create will be locally adjusted which is what
    //  we want.
    //
    FILETIME FlTime;
    if (!::SystemTimeToFileTime(&SysTime, &FlTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert it to a 64 bit int
    const tCIDLib::TCard8 c8Tmp = TRawBits::c8From32
    (
        FlTime.dwLowDateTime, FlTime.dwHighDateTime
    );

    // Make sure its not less than the portable base time
    if (c8Tmp < TKrnlTimeStamp::enctBaseOfs)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_BelowBaseTime);
        return kCIDLib::False;
    }

    // Looks ok, so ave it and adjust for local time base
    m_enctTime = c8Tmp  - TKrnlTimeStamp::enctBaseOfs;
    return kCIDLib::True;
}


//
//  Set our stamp from the passed time values. The date will be the base
//  time stamp date.
//
tCIDLib::TBoolean
TKrnlTimeStamp::bFromTimeDetails(const  tCIDLib::TCard4     c4Hour
                                , const tCIDLib::TCard4     c4Minute
                                , const tCIDLib::TCard4     c4Second
                                , const tCIDLib::TCard4     c4MilliSecs)
{
    // Get the local time, and override the time fields
    SYSTEMTIME  SysTime;
    ::GetLocalTime(&SysTime);
    SysTime.wHour       = WORD(c4Hour);
    SysTime.wMinute     = WORD(c4Minute);
    SysTime.wSecond     = WORD(c4Second);
    SysTime.wMilliseconds=WORD(c4MilliSecs);

    //
    //  Assume that the system time info we get is locally adjusted time,
    //  so the file time we create will be locally adjusted which is what
    //  we want.
    //
    FILETIME FlTime;
    if (!::SystemTimeToFileTime(&SysTime, &FlTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert it to a 64 bit int
    const tCIDLib::TCard8 c8Tmp = TRawBits::c8From32
    (
        FlTime.dwLowDateTime, FlTime.dwHighDateTime
    );

    // Make sure its not less than the portable base time
    if (c8Tmp < TKrnlTimeStamp::enctBaseOfs)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_BelowBaseTime);
        return kCIDLib::False;
    }

    // Looks ok, so ave it and adjust for local time base
    m_enctTime = c8Tmp  - TKrnlTimeStamp::enctBaseOfs;
    return kCIDLib::True;
}


//
//  Stores the passed encoded time stamp. We assume it's legal. If not,
//  it'll fail later when they try to use it. If debugging we validate
//  it just to make sure, else we just return true.
//
tCIDLib::TBoolean TKrnlTimeStamp::bSetTo(const tCIDLib::TEncodedTime& enctNew)
{
    m_enctTime = enctNew;

    #if CID_DEBUG_ON
    return bValidate();
    #else
    return kCIDLib::True;
    #endif
}


// Set our stamp to one of the special times
tCIDLib::TBoolean TKrnlTimeStamp::bSetTo(const tCIDLib::ESpecialTimes eSpecial)
{
    // Check for some special case ones first
    if (eSpecial == tCIDLib::ESpecialTimes::Base)
    {
        m_enctTime = 0;
        return kCIDLib::True;
    }

    //
    //  The rest of them are based to one degree or another on the current
    //  local or system time.
    //
    SYSTEMTIME  SysTime;

    if (eSpecial == tCIDLib::ESpecialTimes::CurrentUTC)
        ::GetSystemTime(&SysTime);
    else
        ::GetLocalTime(&SysTime);

    // And jiggle it as required
    switch(eSpecial)
    {
        case tCIDLib::ESpecialTimes::CurrentDate :
            // Zero out all of the time related fields
            SysTime.wHour           = 0;
            SysTime.wMinute         = 0;
            SysTime.wSecond         = 0;
            SysTime.wMilliseconds   = 0;
            break;

        case tCIDLib::ESpecialTimes::CurrentTime :
        case tCIDLib::ESpecialTimes::CurrentUTC :
            break;

        case tCIDLib::ESpecialTimes::NoonToday :
            SysTime.wHour           = 11;
            SysTime.wMinute         = 0;
            SysTime.wSecond         = 0;
            SysTime.wMilliseconds   = 0;
            break;

        default :
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_InvalidSpecialTime);
            return kCIDLib::False;
    }

    // Convert to the file time format that we can make use of
    FILETIME FlTime;
    if (!::SystemTimeToFileTime(&SysTime, &FlTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert it to a 64 bit int
    const tCIDLib::TCard8 c8Tmp = TRawBits::c8From32
    (
        FlTime.dwLowDateTime, FlTime.dwHighDateTime
    );

    // Make sure its not less than the portable base time
    if (c8Tmp < TKrnlTimeStamp::enctBaseOfs)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTm_BelowBaseTime);
        return kCIDLib::False;
    }

    // Looks ok, so ave it and adjust for local time base
    m_enctTime = c8Tmp  - TKrnlTimeStamp::enctBaseOfs;

    return kCIDLib::True;
}


// Validate our current time stamp by trying to convert it
tCIDLib::TBoolean TKrnlTimeStamp::bValidate() const
{
    FILETIME    FlTime;
    tCIDLib::TCard8 c8Tmp = m_enctTime + TKrnlTimeStamp::enctBaseOfs;

    FlTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
    FlTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);

    SYSTEMTIME  SysTime;
    if (!::FileTimeToSystemTime(&FlTime, &SysTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



