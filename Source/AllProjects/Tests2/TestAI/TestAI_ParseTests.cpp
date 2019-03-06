//
// FILE NAME: TestAI_ParseTests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/25/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This test program doesn't have a lot of tests, so we just implemennt them
//  all in this same file.
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
#include    "TestAI.hpp"
#include    "TestAI_ParseTests.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_BTParse1,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_BTParse1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BTParse1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BTParse1::TTest_BTParse1() :

    TTestFWTest
    (
        L"Behavior Tree Parse 1", L"Tests basic parsing of behavior trees", 3
    )
{
}

TTest_BTParse1::~TTest_BTParse1()
{
}


// ---------------------------------------------------------------------------
//  TTest_BTParse1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BTParse1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Either works or causes an exception
    TAIBehaviorTree btreeTest(L"Parse 1 Test");
    btreeTest.LoadXMLTree(L".\\BTFiles\\BasicTest.xml");

    return eRes;
}

