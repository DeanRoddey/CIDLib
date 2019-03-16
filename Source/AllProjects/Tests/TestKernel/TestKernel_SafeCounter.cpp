//
// FILE NAME: TestKernel_SafeCounter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/12/1996
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
//  This module tests the 'safe counter' classes. Note that these classes
//  are also tested out in a multi-threaded way in the treading tests.
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
#include    "TestKernel.hpp"


// ---------------------------------------------------------------------------
//  Local testing functions
// ---------------------------------------------------------------------------

//
//  Do simple, non-threaded tests first just to see that they work correctly
//  outside of any race conditions.
//
static tCIDLib::TVoid SimpleTests()
{
    TKrnlSafeCard4Counter    kscntTest1;
    TKrnlSafeCard4Counter    kscntTest2(15);
    TKrnlSafeInt4Counter     kscntTest3;
    TKrnlSafeInt4Counter     kscntTest4(-15);

    //
    //  Make sure the initial values are correct
    //
    if (kscntTest1.c4Value() != 0)
        strmOut << CUR_LN << L"Default Card4 counter ctor failed\n";

    if (kscntTest3.i4Value() != 0)
        strmOut << CUR_LN << L"Default Int4 counter ctor failed\n";

    if (kscntTest2.c4Value() != 15)
        strmOut << CUR_LN << L"Card4 counter ctor failed\n";

    if (kscntTest4.i4Value() != -15)
        strmOut << CUR_LN << L"Int4 counter ctor failed\n";

    //
    //  Do the basic inc/dec operations
    //
    if ((kscntTest1.c4Inc() != 1)
    ||  (kscntTest1.c4Value() != 1))
    {
        strmOut << CUR_LN << L"Card4 increment failed\n";
    }

    if ((kscntTest3.i4Inc() != 1)
    ||  (kscntTest3.i4Value() != 1))
    {
        strmOut << CUR_LN << L"Int4 increment failed\n";
    }

    if ((kscntTest1.c4Dec() != 0)
    ||  (kscntTest1.c4Value() != 0))
    {
        strmOut << CUR_LN << L"Card4 decrement failed\n";
    }

    if ((kscntTest3.i4Dec() != 0)
    ||  (kscntTest3.i4Value() != 0))
    {
        strmOut << CUR_LN << L"Int4 decrement failed\n";
    }

    //
    //  Test out the exchange methods
    //
    if (kscntTest2.c4Exchange(100) != 15)
        strmOut << CUR_LN << L"Exchange returned incorrect previous value\n";

    if (kscntTest4.i4Exchange(-99) != -15)
        strmOut << CUR_LN << L"Exchange returned incorrect previous value\n";

    if (kscntTest2.c4Value() != 100)
        strmOut << CUR_LN << L"Exchange did not put in new value\n";

    if (kscntTest4.i4Value() != -99)
        strmOut << CUR_LN << L"Exchange did not put in new value\n";

    //
    //  And now test the compare and exchange methods
    //
    if (kscntTest1.c4CompareAndExchange(2000, 0) != 0)
        strmOut << CUR_LN << L"Comp/Exchg failed\n";

    if (kscntTest3.i4CompareAndExchange(1000, 0) != 0)
        strmOut << CUR_LN << L"Comp/Exchg failed\n";

    if (kscntTest1.c4CompareAndExchange(2000, 0) == 0)
        strmOut << CUR_LN << L"Comp/Exchg should have failed\n";

    if (kscntTest3.i4CompareAndExchange(1000, 0) == 0)
        strmOut << CUR_LN << L"Comp/Exchg should have failed\n";

    //
    //  Test add and subtract methods. They should return the known previous
    //  value.
    //
    if (kscntTest2.c4AddTo(10) != 100)
        strmOut << CUR_LN << L"Add failed to return previous value\n";

    if (kscntTest4.i4AddTo(10) != -99)
        strmOut << CUR_LN << L"Add failed to return previous value\n";

    if (kscntTest2.c4Value() != 110)
        strmOut << CUR_LN << L"Add did not create correct new value\n";

    if (kscntTest4.i4Value() != -89)
        strmOut << CUR_LN << L"Add did not create correct new value\n";

    if (kscntTest2.c4SubFrom(50) != 110)
        strmOut << CUR_LN << L"Sub failed to return previous value\n";

    if (kscntTest4.i4SubFrom(20) != -89)
        strmOut << CUR_LN << L"Sub failed to return previous value\n";

    if (kscntTest2.c4Value() != 60)
        strmOut << CUR_LN << L"Sub did not create correct new value\n";

    if (kscntTest4.i4Value() != -109)
        strmOut << CUR_LN << L"Sub did not create correct new value\n";
}


// ---------------------------------------------------------------------------
//  Intrafacility functions
// ---------------------------------------------------------------------------

tCIDLib::TVoid TestSafeCounters(const tCIDLib::TCard4)
{
    SimpleTests();
}
