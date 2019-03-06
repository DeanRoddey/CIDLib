//
// FILE NAME: CIDKernel_Time.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/20/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TKrnlTimeStamp class. It represents a time
//  in 100 nano-second intervals from the base time of midnight 01/01/1970.
//
//  This file contains the platform independent portions of the implementation
//  with the rest being provided in the per-platform subdirectory.
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



namespace CIDKernel_Time
{
    // -----------------------------------------------------------------------
    //  Local constant values
    //
    //  c4MonDays
    //      The number of days in each month, in a non-leap year.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 c4MonDays[tCIDLib::c4EnumOrd(tCIDLib::EMonths::Count)] =
    {
           31,  28,  31,  30,  31,  30
        ,  31,  31,  30,  31,  30,  31
    };
};



// ---------------------------------------------------------------------------
//   CLASS: TKrnlTimeStamp
//  PREFIX: ktms
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlTimeStamp::bIsLeapYear(const tCIDLib::TCard4 c4Year)
{
    //
    //  If it's an even multiple of 400, always a leap year. Otherwise, if
    //  it's a multiple of 4 but not a multiple of 100, then it's a leap year.
    //
    if (!(c4Year % 400) || (!(c4Year % 4) && (c4Year % 100)))
        return kCIDLib::True;
    return kCIDLib::False;

}


tCIDLib::TBoolean
TKrnlTimeStamp::bIsValidDate(   const   tCIDLib::TCard4     c4Day
                                , const tCIDLib::EMonths    eMonth
                                , const tCIDLib::TCard4     c4Year)
{
    if ((c4Year < kCIDLib::c4MinYear) || (c4Year > kCIDLib::c4MaxYear))
        return kCIDLib::False;

    if ((eMonth < tCIDLib::EMonths::Min) || (eMonth > tCIDLib::EMonths::Max))
        return kCIDLib::False;

    if ((c4Day < 1) || (c4Day > c4MaxDayForMonth(eMonth, c4Year)))
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bIsValidTime(   const   tCIDLib::TCard4 c4Hours
                                , const tCIDLib::TCard4 c4Minutes
                                , const tCIDLib::TCard4 c4Seconds
                                , const tCIDLib::TCard4 c4Hundredths)
{
    if (c4Hours> 23)
        return kCIDLib::False;

    if (c4Minutes> 59)
        return kCIDLib::False;

    if (c4Seconds> 59)
        return kCIDLib::False;

    if (c4Hundredths > 99)
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TCard4
TKrnlTimeStamp::c4MaxDayForMonth(   const   tCIDLib::EMonths    eMonth
                                    , const tCIDLib::TCard4     c4Year)
{
    if (bIsLeapYear(c4Year) && (eMonth == tCIDLib::EMonths::February))
            return 29;
    return CIDKernel_Time::c4MonDays[tCIDLib::c4EnumOrd(eMonth)];
}



// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlTimeStamp::TKrnlTimeStamp() :

    m_enctTime(0)
{
}

TKrnlTimeStamp::TKrnlTimeStamp(const TKrnlTimeStamp& ktmsToCopy) :

    m_enctTime(ktmsToCopy.m_enctTime)
{
}

TKrnlTimeStamp::~TKrnlTimeStamp()
{
}



// ---------------------------------------------------------------------------
//  TKrnlTimeStamp: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlTimeStamp::bAdd(const tCIDLib::TEncodedTime& enctToAdd)
{
    m_enctTime += enctToAdd;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTimeStamp::bSubtract(const tCIDLib::TEncodedTime& enctToSubtract)
{
    m_enctTime -= enctToSubtract;

    if (m_enctTime < 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NegativeResult);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TEncodedTime
TKrnlTimeStamp::enctDifference(const TKrnlTimeStamp& ktmsToSubtract) const
{
    return m_enctTime - ktmsToSubtract.m_enctTime;
}

tCIDLib::TEncodedTime TKrnlTimeStamp::enctTime() const
{
    return m_enctTime;
}


tCIDLib::TEncodedTime TKrnlTimeStamp::enctMilliSeconds() const
{
    if (m_enctTime < 10000)
        return 0;

    return m_enctTime / 10000;
}

