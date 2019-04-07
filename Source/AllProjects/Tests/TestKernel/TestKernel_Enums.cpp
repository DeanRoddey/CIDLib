//
// FILE NAME: TestKernel_Enums.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/29/2018
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//  For low level code that can't use the IDL compiler, and for those enums that
//  aren't done via the IDL compiler, we have some macros that generate some of
//  the magic functionality for enumerations. This just tests those, since they
//  will likely be used in the test framework that runs the higher level tests.
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


enum ETests
{
    Value1
    , Value2
    , Value3

    , Count
    , Min       = Value1
    , Max       = Value3
};
StdEnumTricks(ETests);



// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestEnums(const tCIDLib::TCard4)
{
    ETests eTest = ETests::Value1;

    // Check prefix inc and dec
    if ((++eTest != ETests::Value2) || (eTest != ETests::Value2))
    {
        strmOut << CUR_LN
                << L"Safe enum prefix increment failed\n";
    }

    if ((eTest++ != ETests::Value2) || (eTest != ETests::Value3))
    {
        strmOut << CUR_LN
                << L"Safe enum post-fix increment failed\n";
    }

    if ((--eTest != ETests::Value2) || (eTest != ETests::Value2))
    {
        strmOut << CUR_LN
                << L"Safe enum prefix decrement failed\n";
    }

    if ((eTest-- != ETests::Value2) || (eTest != ETests::Value1))
    {
        strmOut << CUR_LN
                << L"Safe enum post-fix decrement failed\n";
    }
}
