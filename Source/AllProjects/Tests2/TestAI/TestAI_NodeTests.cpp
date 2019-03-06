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
#include    "TestAI_NodeTests.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_BTInverterTest1,TTestFWTest)
RTTIDecls(TTest_BTVarTest1,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_BTInverterTest1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BTInverterTest1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BTInverterTest1::TTest_BTInverterTest1() :

    TTestFWTest
    (
        L"BT Inverter Node ", L"Basic test of invter node", 3
    )
{
}

TTest_BTInverterTest1::~TTest_BTInverterTest1()
{
}


// ---------------------------------------------------------------------------
//  TTest_BTInverterTest1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BTInverterTest1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  This tree is just a single variable test node. So we set up that variable as
    //  an initial variable and run the tree. It should succeed.
    //
    TAIBehaviorTree btreeTest(L"Inverter Node Test 1");
    btreeTest.LoadXMLTree(L".\\BTFiles\\InverterTest1.xml");

    // It's a success node wrapped in an inverter node, so it should return ailure
    tCIDLib::TKValsList colInitVars;
    if (btreeTest.eRun(L"InverterTest1", colInitVars) != tCIDAI::ENodeStates::Failure)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Inverter node did not invert result"
                << kCIDLib::DNewLn;
    }

    return eRes;
}





// ---------------------------------------------------------------------------
//  CLASS: TTest_BTVarTest1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BTVarTest1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BTVarTest1::TTest_BTVarTest1() :

    TTestFWTest
    (
        L"BT Var Test Node ", L"Basic test of variable test node", 4
    )
{
}

TTest_BTVarTest1::~TTest_BTVarTest1()
{
}


// ---------------------------------------------------------------------------
//  TTest_BTVarTest1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BTVarTest1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  This tree is just a single variable test node. So we set up that variable as
    //  an initial variable and run the tree. It should succeed.
    //
    TAIBehaviorTree btreeTest(L"Var Test Node 1");
    btreeTest.LoadXMLTree(L".\\BTFiles\\VarTestNode1.xml");

    // Do a positive test
    {
        tCIDLib::TKValsList colInitVars;
        colInitVars.objAdd(TKeyValues(L"VarTest1Var", L"TestVal", L"Value To Test"));

        if (btreeTest.eRun(L"VarTest1", colInitVars) != tCIDAI::ENodeStates::Success)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First variable test node test failed" << kCIDLib::DNewLn;
        }
    }

    // And do a negative version of it
    {
        tCIDLib::TKValsList colInitVars;
        colInitVars.objAdd(TKeyValues(L"VarTest1Var", L"TestVal", L"Bad Value To Test"));

        if (btreeTest.eRun(L"VarTest1", colInitVars) != tCIDAI::ENodeStates::Failure)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Variable test node negative test didn't fail"
                    << kCIDLib::DNewLn;
        }
    }

    return eRes;
}

