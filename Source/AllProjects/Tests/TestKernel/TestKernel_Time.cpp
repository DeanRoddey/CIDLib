//
// FILE NAME: TestKernel_Time.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/22/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module tests the TKrnlTimeStamp class, which provides access to
//  core system time services.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestKernel.hpp"



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  These methods are all called from TestTime() below to test out various
//  aspects of the time services classes.
//
static tCIDLib::TVoid TestConversions()
{
    TKrnlTimeStamp ktmsTest;

    if (!ktmsTest.bFromDetails(1996, tCIDLib::EMonths::December, 5))
    {
        strmOut << CUR_LN << L"Error setting time from details. "
                << TKrnlError::kerrLast() << EndLn;
        return;
    }

    tCIDLib::EWeekDays eDay;
    if (!ktmsTest.bDayOfWeek(eDay))
        throw TKrnlError::kerrLast();

    if (eDay != tCIDLib::EWeekDays::Thursday)
    {
        strmOut << CUR_LN << L"Time from details not == to thursday\n";
        return;
    }

    //
    //  Do a local to UTC to local round trip and make sure we get back
    //  to where we started.
    //
    tCIDLib::TEncodedTime enctTest1 = TKrnlTimeStamp::enctNow();
    tCIDLib::TEncodedTime enctTest2;
    tCIDLib::TEncodedTime enctTest3;
    if (!TKrnlTimeStamp::bLocalToUTC(enctTest1, enctTest2)
    ||  !TKrnlTimeStamp::bUTCToLocal(enctTest2, enctTest3))
    {
        strmOut << CUR_LN << L"Local/UTC conversion failed\n";
        return;
    }

    if (enctTest1 != enctTest3)
    {
        strmOut << CUR_LN << L"Local/UTC conversion failed\n";
        return;
    }
}

static tCIDLib::TVoid TestDefaultConstruction()
{
    // This should create a time at the start of our time universe
    TKrnlTimeStamp  ktmsDefault;

    // Query out the date info and see if its what whe think it is
    tCIDLib::TCard4     c4Year;
    tCIDLib::EMonths    eMonth;
    tCIDLib::TCard4     c4Day;
    tCIDLib::EWeekDays  eWeekDay;

    if (!ktmsDefault.bAsDateInfo(c4Year, eMonth, c4Day, eWeekDay))
    {
        strmOut << CUR_LN << L"Error getting as date info. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c4Year != 1970)
    {
        strmOut << CUR_LN
                << L"Default time stamp not == to base year (Actual="
                << c4Year << L")\n";
    }

    if (c4Day != 1)
    {
        strmOut << CUR_LN
                << L"Default time stamp not set to 1st day (Actual="
                << c4Day << L"\n";
    }

    if (eMonth != tCIDLib::EMonths::January)
        strmOut << CUR_LN << L"Default time stamp not set to January\n";

    if (eWeekDay != tCIDLib::EWeekDays::Thursday)
        strmOut << CUR_LN << L"Default weekday was not thursday\n";


    // Now make sure its time stamp is at midnight on that day
    tCIDLib::TCard4 c4Hour;
    tCIDLib::TCard4 c4Minute;
    tCIDLib::TCard4 c4Second;
    tCIDLib::TCard4 c4MilliSecs;

    if (!ktmsDefault.bAsTimeInfo(c4Hour, c4Minute, c4Second, c4MilliSecs))
    {
        strmOut << CUR_LN << L"Error getting as time info. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c4Hour || c4Minute || c4Second || c4MilliSecs)
    {
        strmOut << CUR_LN << L"Default time stamp not at midnight (Actual="
                << c4Hour << L"." << c4Minute << L"."
                << c4Second << L"." << c4MilliSecs << EndLn;
    }
}

static tCIDLib::TVoid TestExplicitConstruction()
{
    TKrnlTimeStamp  ktmsTest;
    if (!ktmsTest.bSetTo(kCIDLib::enctOneHour))
        throw TKrnlError::kerrLast();

    tCIDLib::TCard4 c4Hour;
    tCIDLib::TCard4 c4Minute;
    tCIDLib::TCard4 c4Second;
    tCIDLib::TCard4 c4MilliSecs;

    if (!ktmsTest.bAsTimeInfo(c4Hour, c4Minute, c4Second, c4MilliSecs))
    {
        strmOut << CUR_LN << L"Error getting as time info. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c4Hour != 1)
    {
        strmOut << CUR_LN << L"Construct to 1 hour failed. Actual="
                << c4Hour << L"." << c4Minute << L"."
                << c4Second << L"." << c4MilliSecs << EndLn;
    }
}

static tCIDLib::TVoid TestFromDetails()
{
    TKrnlTimeStamp  ktmsTest;

    // Set it to what we know should create a 0 value
    if (!ktmsTest.bFromDetails(1970, tCIDLib::EMonths::January, 1))
    {
        strmOut << CUR_LN << L"Error setting from details. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (ktmsTest.enctTime())
        strmOut << CUR_LN << L"Setting from details was not 0\n";
}

static tCIDLib::TVoid TestMathAndOperators()
{
    TKrnlTimeStamp  ktmsTest1;
    if (!ktmsTest1.bSetTo(kCIDLib::enctOneHour))
        throw TKrnlError::kerrLast();
    TKrnlTimeStamp  ktmsTest2;
    if (!ktmsTest2.bSetTo(kCIDLib::enctOneDay))
        throw TKrnlError::kerrLast();

    if (ktmsTest1.enctTime() > ktmsTest2.enctTime())
        strmOut << CUR_LN << L"Operator > reported incorrect order\n";

    // Now make it equal and test again by adding 23 hours
    if (!ktmsTest1.bAdd(kCIDLib::enctOneHour * 23))
        throw TKrnlError::kerrLast();
    if (ktmsTest1.enctTime() != ktmsTest2.enctTime())
        strmOut << CUR_LN << L"Objects not equal.\n";

    // Copy construct one and make sure its equal
    TKrnlTimeStamp  ktmsCopy(ktmsTest1);
    if (ktmsCopy.enctTime() != ktmsTest1.enctTime())
        strmOut << CUR_LN << L"Copy constructed time != original\n";

    // Subtract an hour from the second value and make sure they aren't equal
    if (!ktmsTest2.bSubtract(kCIDLib::enctOneDay))
        throw TKrnlError::kerrLast();
    if (ktmsTest2.enctTime() >= ktmsTest1.enctTime())
        strmOut << CUR_LN << L"Relative magnitude bad after subtraction\n";
}

static tCIDLib::TVoid TestFileTimes()
{

}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------

tCIDLib::TVoid TestTime(const tCIDLib::TCard4)
{
    TestDefaultConstruction();
    TestExplicitConstruction();
    TestFromDetails();
    TestMathAndOperators();
    TestConversions();
    TestFileTimes();
}
