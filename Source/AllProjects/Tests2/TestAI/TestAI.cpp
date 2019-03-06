//
// FILE NAME: TestAI.cpp
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
//  This is the main implementation file of the test program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "TestAI.hpp"
#include    "TestAI_NodeTests.hpp"
#include    "TestAI_ParseTests.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TCIDAITestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TCIDAITestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TCIDAITestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TCIDAITestApp::TCIDAITestApp()
{
}

TCIDAITestApp::~TCIDAITestApp()
{
}


// ----------------------------------------------------------------------------
//  TCIDAITestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TCIDAITestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TCIDAITestApp::LoadTests()
{
    // Load up our tests on our parent class

    // Tree parsing tests
    AddTest(new TTest_BTParse1);

    // Basic node tests
    AddTest(new TTest_BTInverterTest1);
    AddTest(new TTest_BTVarTest1);
}

tCIDLib::TVoid TCIDAITestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TCIDAITestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TCIDAITestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TCIDAITestApp   tfwappAI;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TCIDAITestApp>(&tfwappAI, &TCIDAITestApp::eTestThread)
    )
)
