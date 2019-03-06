//
// FILE NAME: TestKernel_FlagJanitors.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This module tests the flag janitor classes, TBoolJanitor, TCardJanitor,
//  TIntJanitor, which provide a scope based mechanism for setting and
//  restoring fundamental flag types.
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
//  A template to create the common tests, which are all the same except for
//  the class type and the fundamental flag types they control.
// ----------------------------------------------------------------------------
template <class TF, class TC>
tCIDLib::TVoid TestFlagJan(const TF NewValue, const TF OldValue)
{
    TF  Flag = OldValue;

    // Do a basic test that it sets and restores the flag
    {
        TC  Janitor(&Flag, NewValue);
        if (Flag != NewValue)
            strmOut << CUR_LN << L"Janitor failed to set new value\n";
    }

    if (Flag != OldValue)
        strmOut << CUR_LN << L"Janitor failed to reset old value\n";


    // And now do one that releases the flag
    {
        TC  Janitor(&Flag, NewValue);
        if (Flag != NewValue)
            strmOut << CUR_LN << L"Janitor failed to set new value\n";

        Janitor.Release();
    }

    if (Flag == OldValue)
        strmOut << CUR_LN << L"Janitor failed to release, and reset old value\n";
}



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::TVoid BasicTests()
{
    TestFlagJan<tCIDLib::TBoolean,TBoolJanitor>(kCIDLib::False, kCIDLib::True);
    TestFlagJan<tCIDLib::TCard4,TCardJanitor>(16, 32);
    TestFlagJan<tCIDLib::TInt4,TIntJanitor>(-1, 260);
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestFlagJanitors(const tCIDLib::TCard4)
{
    BasicTests();
}
