//
// FILE NAME: TestCIDLib2_Time.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains tests related to the TTime class, which provides a
//  fair amount of functionality that is pretty widely used.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestCIDLib2.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_Time1,TTestFWTest)
RTTIDecls(TTest_Time2,TTestFWTest)
RTTIDecls(TTest_Time3,TTestFWTest)
RTTIDecls(TTest_Time4,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Time1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Time1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Time1::TTest_Time1() :

    TTestFWTest
    (
        L"Time Tests 1", L"Basic tests of the TTime class", 2
    )
{
}

TTest_Time1::~TTest_Time1()
{
}


// ---------------------------------------------------------------------------
//  TTest_Time1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Time1::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test the various constructors and setup methods
    {
        tCIDLib::TCard4     c4Hour;
        tCIDLib::TCard4     c4Minute;
        tCIDLib::TCard4     c4Second;
        tCIDLib::TCard4     c4MilliSecs;

        tCIDLib::TCard4     c4Year;
        tCIDLib::TCard4     c4Day;
        tCIDLib::EMonths    eMonth;
        tCIDLib::EWeekDays  eWeekDay;

        //
        //  First test the constructor that uses an absolute number of
        //  100 nano-second intervals from 01/01/1970. We create one that
        //  is one hour from the base time.
        //
        {
            TTime tmTest(kCIDLib::enctOneHour);

            // Make sure that the raw time interval comes back correct
            if (tmTest.enctTime() != kCIDLib::enctOneHour)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Raw time was not what was used in constructor\n\n";
            }

            // Get the individual time values
            c4MilliSecs = tmTest.c4AsTimeInfo(c4Hour, c4Minute, c4Second);

            // Make sure that the values are what they should be
            if (c4Hour != 1)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Hour was not equal to 1\n\n";
            }

            if (c4Minute || c4Second || c4MilliSecs)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Minutes, seconds, and milliseconds should have been 0\n\n";
            }

            // And test the individual date values
            eWeekDay = tmTest.eAsDateInfo(c4Year, eMonth, c4Day);

            if ((c4Year != 1970)
            ||  (eMonth != tCIDLib::EMonths::January)
            ||  (c4Day != 1)
            ||  (eWeekDay != tCIDLib::EWeekDays::Thursday))
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Date values were not correct after construction\n\n";
            }

            // Copy construct and compare
            TTime tmCopy(tmTest);

            if (tmCopy.enctTime() != tmTest.enctTime())
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Copied time != to original\n\n";
            }
        }

        //
        //  Now test the other way around. We build one up from all of the
        //  individual values, for the base time of midnight on 01/01/1970.
        //  Then we get back out the 100-nanosecond raw value and it should
        //  be 0.
        //
        {
            TTime tmTest
            (
                1970
                , tCIDLib::EMonths::January
                , 1
                , 0, 0, 0, 0
            );

            if (tmTest.enctTime() != 0)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Raw time was not correct for component construction\n\n";
            }
        }

        //
        //  Now lets do a particular date that we know is a certain day of
        //  the week and check it out.
        //
        {
            TTime tmTest(1997, tCIDLib::EMonths::March, 1);
            if (tmTest.eDayOfWeek() != tCIDLib::EWeekDays::Saturday)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Day of the week was incorrect\n\n";
            }
        }
    }

    // Do a local to UTC back to local round trip
    {
        TTime tmTest(tCIDLib::ESpecialTimes::CurrentTime);
        TTime tmOriginal(tmTest);

        tmTest.LocalToUTC();
        tmTest.UTCToLocal();
        if (tmTest != tmOriginal)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"UTC to local conversion failed\n\n";
        }
    }

    // Test the equality and relative magnitude operators
    {
        // Create some time objects that are equal and inequal
        TTime   tmSmall(kCIDLib::enctOneHour);
        TTime   tmLarge;
        tmLarge.SetToNow();
        TTime   tmSameAsLarge(tmLarge);

        // Make sure that small is less than large
        if (!(tmSmall < tmLarge))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Small time >= large time\n\n";
        }

        // Make sure that large is greater than small
        if (!(tmLarge > tmSmall))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Large time <= small time\n\n";
        }

        // Make sure that large and same as large are equal
        if (tmLarge != tmSameAsLarge)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Copy constructed times not equal\n\n";
        }

        // Check <= and >= versions
        if (!(tmSmall <= tmLarge))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Small time > large time\n\n";
        }

        if (!(tmLarge >= tmSmall))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Large time < small time\n\n";
        }

        if (!(tmLarge >= tmSameAsLarge))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Copy constructed times not equal\n\n";
        }

        if (!(tmLarge <= tmSameAsLarge))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Copy constructed times not equal\n\n";
        }

        // Make sure non-equal objects are not equal
        if (tmLarge == tmSmall)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Inequal objects showed up equal\n\n";
        }
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_Time2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Time2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Time2::TTest_Time2() :

    TTestFWTest
    (
        L"Time Tests 2", L"Time class detail extraction tests", 2
    )
{
}

TTest_Time2::~TTest_Time2()
{
}


// ---------------------------------------------------------------------------
//  TTest_Time2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Time2::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Get the current time, then extract the details via the various
    //  methods, and make sure that they all agree.
    //
    tCIDLib::TCard4         c4Hour1;
    tCIDLib::TCard4         c4Minute1;
    tCIDLib::TCard4         c4Second1;
    tCIDLib::TCard4         c4MilliSecs1;
    tCIDLib::TCard4         c4Year1;
    tCIDLib::TCard4         c4Day1;
    tCIDLib::TEncodedTime   enctStamp1;
    tCIDLib::EMonths        eMonth1;
    tCIDLib::EWeekDays      eWeekDay1;
    TTime tmNow(tCIDLib::ESpecialTimes::CurrentTime);
    eWeekDay1 = tmNow.eExpandDetails
    (
        c4Year1
        , eMonth1
        , c4Day1
        , c4Hour1
        , c4Minute1
        , c4Second1
        , c4MilliSecs1
        , enctStamp1
    );

    if (tmNow.eDayOfWeek() != eWeekDay1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Inconsistent weekday details\n\n";
    }

    if (tmNow.enctTime() != enctStamp1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Inconsistent time stamp\n\n";
    }

    // Get the separate time and date details and compare them
    tCIDLib::TCard4     c4Hour2;
    tCIDLib::TCard4     c4Minute2;
    tCIDLib::TCard4     c4Second2;
    tCIDLib::TCard4     c4MilliSecs2;
    tCIDLib::TCard4     c4Year2;
    tCIDLib::TCard4     c4Day2;
    tCIDLib::EMonths    eMonth2;
    tCIDLib::EWeekDays  eWeekDay2;
    eWeekDay2 = tmNow.eAsDateInfo(c4Year2, eMonth2, c4Day2);
    c4MilliSecs2 = tmNow.c4AsTimeInfo(c4Hour2, c4Minute2, c4Second2);

    if ((c4Hour1 != c4Hour2)
    ||  (c4Minute1 != c4Minute2)
    ||  (c4Second1 != c4Second2)
    ||  (c4MilliSecs1 != c4MilliSecs2)
    ||  (c4Year1 != c4Year2)
    ||  (eMonth1 != eMonth2)
    ||  (eWeekDay1 != eWeekDay2))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Inconsistent time/date details\n\n";
    }

    return eRes;
}





// ---------------------------------------------------------------------------
//  CLASS: TTest_Time3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Time3: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Time3::TTest_Time3() :

    TTestFWTest
    (
        L"Time Tests 3", L"More advanced tests of the TTime class", 3
    )
{
}

TTest_Time3::~TTest_Time3()
{
}


// ---------------------------------------------------------------------------
//  TTest_Time1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Time3::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test the mathematical operators
    {
        // Create a timestamp for 1 hour and one for two hours
        TTime   tmTest1(kCIDLib::enctOneHour);
        TTime   tmTest2(kCIDLib::enctOneHour * 2);

        //
        //  First do it via the i8Difference() method, which is basically like
        //  the subtraction operator, but for folks who don't like operators.
        //
        if (tmTest2.enctDifference(tmTest1) != kCIDLib::enctOneHour)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Difference between times was not 1 hour\n\n";
        }

        // Add them together via the friend operator
        if (tmTest1 + tmTest2 != TTime(kCIDLib::enctOneHour * 3))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Friend addition of times was incorrect\n\n";
        }

        // Add one to the other via the member operator
        tmTest1 += tmTest2;
        if (tmTest1 != kCIDLib::enctOneHour * 3)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Member addition of times was incorrect\n\n";
        }

        // Subtract it back out and make sure its back to the original
        if (tmTest1 - tmTest2 != TTime(kCIDLib::enctOneHour))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Friend subtraction of times was incorrect\n\n";
        }

        // And via the member operator
        tmTest1 -= tmTest2;
        if (tmTest1 != kCIDLib::enctOneHour)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Member subtraction of times was incorrect\n\n";
        }

        // Make sure that the 100-nano second and millisecond views agree
        if (tmTest1.enctTime() / 10000 != tmTest1.enctMilliSeconds())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Different time views don't agree\n";
        }
    }


    // Test the sunrise/sunset support
    {
        //
        //  Set up a couple of known sunrise/sunset scenarios and run them
        //  and compare them against what we already calculated to be correct
        //  via other sources.
        //
        tCIDLib::TCard4 c4RiseHour;
        tCIDLib::TCard4 c4RiseMin;
        tCIDLib::TCard4 c4SetHour;
        tCIDLib::TCard4 c4SetMin;

        // Do one for Melbourne AU, for Feb/04/2005
        TTime tmCur(2005, tCIDLib::EMonths::February, 4);
        TTime::CalcRiseSetTimes
        (
            tmCur
            , 10
            , -37.5
            , 145.0
            , c4RiseHour
            , c4RiseMin
            , c4SetHour
            , c4SetMin
        );

        if ((c4RiseHour != 5) || (c4RiseMin != 38))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Bad rise time for Melbourne\n\n";
        }

        if ((c4SetHour != 19) || (c4SetMin != 28))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Bad set time for Melbourne\n\n";
        }

        // Do one for San Francisco, for Feb/04/2005
        tmCur.FromDetails(2005, tCIDLib::EMonths::February, 4);
        TTime::CalcRiseSetTimes
        (
            tmCur
            , -8
            , 37.48
            , -122.24
            , c4RiseHour
            , c4RiseMin
            , c4SetHour
            , c4SetMin
        );

        if ((c4RiseHour != 7) || (c4RiseMin != 10))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Bad rise time for San Francisco\n\n";
        }

        if ((c4SetHour != 17) || (c4SetMin != 35))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Bad set time for San Francisco\n\n";
        }

        // Do one for Montreal, for April/13/2006
        tmCur.FromDetails(2006, tCIDLib::EMonths::April, 13);
        TTime::CalcRiseSetTimes
        (
            tmCur
            , -5
            , 45.30
            , -73.36
            , c4RiseHour
            , c4RiseMin
            , c4SetHour
            , c4SetMin
        );

        if ((c4RiseHour != 5) || (c4RiseMin != 12))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Bad rise time for Montreal\n\n";
        }

        if ((c4SetHour != 18) || (c4SetMin != 35))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn
                    << L"Bad set time for Montreal\n\n";
        }
    }


    // Test the Julian date support
    {
        tCIDLib::TCard4     c4Year;
        tCIDLib::TCard4     c4Day;
        tCIDLib::EMonths    eMonth;
        tCIDLib::TCard4     c4Julian;
        TTime               tmTest;


        //
        //  Round trip some Julian dates. We convert a m/y/d to Julian,
        //  then back the ohter way and see if we get back to where we
        //  started.
        //
        c4Julian = TTime::c4CvtToJulian
        (
            1995, tCIDLib::EMonths::January, 1
        );
        TTime::CvtFromJulian(c4Julian, c4Year, eMonth, c4Day);
        if ((c4Year != 1995) || (c4Day != 1) || (eMonth != tCIDLib::EMonths::January))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Bad Julian date round trip\n\n";
        }

        c4Julian = TTime::c4CvtToJulian
        (
            1900, tCIDLib::EMonths::January, 1
        );
        TTime::CvtFromJulian(c4Julian, c4Year, eMonth, c4Day);
        if ((c4Year != 1900) || (c4Day != 1) || (eMonth != tCIDLib::EMonths::January))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Bad Julian date round trip\n\n";
        }

        c4Julian = TTime::c4CvtToJulian
        (
            2005, tCIDLib::EMonths::December, 30
        );
        TTime::CvtFromJulian(c4Julian, c4Year, eMonth, c4Day);
        if ((c4Year != 2005) || (c4Day != 30) || (eMonth != tCIDLib::EMonths::December))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Bad Julian date round trip\n\n";
        }

        c4Julian = TTime::c4CvtToJulian
        (
            2000, tCIDLib::EMonths::February, 28
        );
        TTime::CvtFromJulian(c4Julian, c4Year, eMonth, c4Day);
        if ((c4Year != 2000) || (c4Day != 28) || (eMonth != tCIDLib::EMonths::February))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Bad Julian date round trip\n\n";
        }

        //
        //  Start off on a leap year Feb 29th, then add 366 days. We should
        //  get to March 1st.
        //
        c4Julian = TTime::c4CvtToJulian
        (
            2008, tCIDLib::EMonths::February, 29
        );
        c4Julian += 366;
        TTime::CvtFromJulian(c4Julian, c4Year, eMonth, c4Day);
        if ((c4Year != 2009) || (c4Day != 1) || (eMonth != tCIDLib::EMonths::March))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Failed to handle leap day when moving forward\n\n";
        }

        // Do the same but add 365 days. We should get to Feb 28th
        c4Julian = TTime::c4CvtToJulian
        (
            2008, tCIDLib::EMonths::February, 29
        );
        c4Julian += 365;
        TTime::CvtFromJulian(c4Julian, c4Year, eMonth, c4Day);
        if ((c4Year != 2009) || (c4Day != 28) || (eMonth != tCIDLib::EMonths::February))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Failed to handle leap day when moving forward\n\n";
        }

        //
        //  Note that these types of conversions are adjusted so that we
        //  work on a normal day, not the noon changeover that Julian dates
        //  really use. So this creates a time of midnight, but we count it
        //  as falling into this day. Otherwise the Julian date would be
        //  one less than this, technically.
        //
        tmTest.FromDetails(2004, tCIDLib::EMonths::December, 16);
        if (tmTest.c4ToJulian() != 2453356)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got unexpected Julian date\n\n";
        }

        tmTest.FromJulian(2453356UL);
        tmTest.eAsDateInfo(c4Year, eMonth, c4Day);

        if ((eMonth != tCIDLib::EMonths::December)
        ||  (c4Day != 16)
        ||  (c4Year != 2004))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Bad Julian date round trip\n\n";
        }
    }

    //
    //  Test the time parsing support.
    //
    {
        {
            TTime tmExp(2011, tCIDLib::EMonths::September, 21, 9, 46, 6);
            TestParse
            (
                strmOut
                , eRes
                , TFWCurLn
                , L"Wed, 21 Sep 2011 9:46:06 -0700"
                , TTime::fcolRFC822()
                , 0
                , 0
                , 0
                , tmExp
                , kCIDLib::True
            );
        }


        {
            TTime tmExp(2007, tCIDLib::EMonths::May, 1);
            TestParse
            (
                strmOut
                , eRes
                , TFWCurLn
                , L"2007/05/01"
                , TTime::fcolYYYYMMDD()
                , 0
                , 0
                , 0
                , tmExp
                , kCIDLib::True
            );
        }

        {
            //
            //  Formatted as 24 hour the hour will be 10, not 9 since they
            //  are not zero based.
            //
            TTime tmExp(1970, tCIDLib::EMonths::January, 1, 21, 58);
            TestParse
            (
                strmOut
                , eRes
                , TFWCurLn
                , L"10:58 PM"
                , TTime::fcol12HHMM()
                , 0
                , 0
                , 0
                , tmExp
                , kCIDLib::True
            );
        }

        {
            TTime tmExp(1970, tCIDLib::EMonths::January, 1, 22, 58, 59);
            TestParse
            (
                strmOut
                , eRes
                , TFWCurLn
                , L"22:58:59"
                , TTime::fcol24HHMMSS()
                , 0
                , 0
                , 0
                , tmExp
                , kCIDLib::True
            );
        }

        {
            TTime tmExp(2007, tCIDLib::EMonths::December, 8);
            TestParse
            (
                strmOut
                , eRes
                , TFWCurLn
                , L"12/8/07"
                , TTime::fcolMMDDYY()
                , 0
                , 0
                , 0
                , tmExp
                , kCIDLib::True
            );
        }

        {
            TTime tmExp(2007, tCIDLib::EMonths::December, 8, 0, 0, 0);
            TestParse
            (
                strmOut
                , eRes
                , TFWCurLn
                , L"Sat Dec 8, 0:0:0 2007"
                , TTime::fcolCTime()
                , 0
                , 0
                , 0
                , tmExp
                , kCIDLib::True
            );
        }

        // These should fail
        {
            // Invalid minute
            TTime tmExp(2007, tCIDLib::EMonths::December, 8, 0, 0, 0);
            TestParse
            (
                strmOut
                , eRes
                , TFWCurLn
                , L"Sat Dec 8, 1:60:1 2007"
                , TTime::fcolCTime()
                , 0
                , 0
                , 0
                , tmExp
                , kCIDLib::False
            );
        }

    }
    return eRes;
}


//
//  A helper to do parsing tests. Note that we only set the result if it
//  fails, since we don't want to undo a previous failure status.
//
tCIDLib::TVoid
TTest_Time3::TestParse(         TTextStringOutStream&   strmOut
                        ,       tTestFWLib::ETestRes&   eRes
                        , const TTFWCurLn&              twclAt
                        , const tCIDLib::TCh* const     pszSrc
                        , const TTime::TCompList&       fcolComps
                        , const tCIDLib::TCh            chDateSep
                        , const tCIDLib::TCh            chTimeSep
                        , const tCIDLib::TCh            chTZSep
                        , const TTime&                  tmExpected
                        , const tCIDLib::TBoolean       bShouldPass)
{
    tCIDLib::TBoolean bStatus = kCIDLib::True;
    TTime tmTest;
    try
    {
        tmTest.ParseFromText(pszSrc, fcolComps, chDateSep, chTimeSep, chTZSep);
        if (tmTest != tmExpected)
        {
            strmOut << twclAt << L"Parsed time is incorrect\n\n";
            if (bShouldPass)
                eRes = tTestFWLib::ETestRes::Failed;
            bStatus = kCIDLib::False;
        }
    }

    catch(const TError& errToCatch)
    {
        if (bShouldPass)
        {
            strmOut << twclAt << errToCatch.strErrText() << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
        bStatus = kCIDLib::False;
    }
    if (bStatus && !bShouldPass)
        eRes = tTestFWLib::ETestRes::Failed;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_Time4
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Time4: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Time4::TTest_Time4() :

    TTestFWTest
    (
        L"Time Tests 4", L"Time class formatting tests", 3
    )
{
}

TTest_Time4::~TTest_Time4()
{
}


// ---------------------------------------------------------------------------
//  TTest_Time4: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Time4::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  We create a list of raw time stamps, format strings, and the resulting
    //  formatted time that we'd expect. We pre-calculate some elapsed time
    //  values to use in the tests.
    //
    struct TTestInfo
    {
        tCIDLib::TEncodedTime   enctStamp;
        const tCIDLib::TCh*     pszFmtStr;
        const tCIDLib::TCh*     pszResult;
    };

    const tCIDLib::TEncodedTime enct1D1H1M
    (
        kCIDLib::enctOneDay + kCIDLib::enctOneHour + kCIDLib::enctOneMinute
    );
    const tCIDLib::TEncodedTime enct10D11H12M
    (
        (kCIDLib::enctOneDay * 10)
        + (kCIDLib::enctOneHour * 11)
        + (kCIDLib::enctOneMinute * 12)
    );
    const tCIDLib::TEncodedTime enctLongElap
    (
        (kCIDLib::enctOneDay * 20)
        + (kCIDLib::enctOneHour * 23)
        + (kCIDLib::enctOneMinute * 59)
    );
    const tCIDLib::TEncodedTime enct9H8M7S
    (
        (kCIDLib::enctOneHour * 9)
        + (kCIDLib::enctOneMinute * 8)
        + (kCIDLib::enctOneSecond * 7)
    );
    const tCIDLib::TEncodedTime enct5M6S
    (
        (kCIDLib::enctOneMinute * 5)
        + (kCIDLib::enctOneSecond * 6)
    );

    const TTestInfo aTests[] =
    {
        {               0, L"%(e)"              , L"0"                  }
      , {               0, L"%(x)"              , L"00:00:00"           }
      , {      enct1D1H1M, L"%(x)"              , L"01:01:01"           }
      , {   enct10D11H12M, L"%(x)"              , L"10:11:12"           }
      , {    enctLongElap, L"%(x)"              , L"20:23:59"           }
      , {    enctLongElap, L"Time: %(x)"        , L"Time: 20:23:59"     }
      , {      enct9H8M7S, L"Time: %(z)"        , L"Time: 09:08:07"     }
      , {        enct5M6S, L"Time: %(Z)"        , L"Time: 05:06"        }
      , {        enct5M6S, L"Time: %(z)"        , L"Time: 00:05:06"     }

    };
    const tCIDLib::TCard4 c4TestCnt = tCIDLib::c4ArrayElems(aTests);

    TTime   tmFmt;
    TString strFmt;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
    {
        const TTestInfo& CurTest = aTests[c4Index];
        tmFmt.enctTime(CurTest.enctStamp);
        tmFmt.FormatToStr(strFmt, CurTest.pszFmtStr, tCIDLib::EAppendOver::Overwrite);
        if (strFmt != CurTest.pszResult)
        {
            strmOut << TFWCurLn << L"Formatted time is wrong for test index "
                    << TCardinal(c4Index)
                    << L". Got='" << strFmt << L"'\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    return eRes;
}

