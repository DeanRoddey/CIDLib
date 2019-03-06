//
// FILE NAME: TestNet.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/02/2007
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
#include    "TestNet.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TNetTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TNetTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TNetTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TNetTestApp::TNetTestApp()
{
}

TNetTestApp::~TNetTestApp()
{
}


// ----------------------------------------------------------------------------
//  TNetTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TNetTestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TNetTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_EndPoints);
    AddTest(new TTest_HTTPAuthParse);
    AddTest(new TTest_HTTPGET);
    AddTest(new TTest_JSON1);
    AddTest(new TTest_JSON2);
    AddTest(new TTest_JSON3);
    AddTest(new TTest_JSON4);
    AddTest(new TTest_JSON5);
    AddTest(new TTest_URLParse);
    AddTest(new TTest_URLBuild);

    AddTest(new TTest_MPMIMEDecode1);
}

tCIDLib::TVoid TNetTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TNetTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TNetTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TNetTestApp   tfwappNet;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TNetTestApp>(&tfwappNet, &TNetTestApp::eTestThread)
    )
)

