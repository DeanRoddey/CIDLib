//
// FILE NAME: TestMathLib.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2007
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
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
#include    "TestMathLib.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TMathLibTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TMathLibTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TMathLibTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TMathLibTestApp::TMathLibTestApp()
{
}

TMathLibTestApp::~TMathLibTestApp()
{
}


// ----------------------------------------------------------------------------
//  TMathLibTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TMathLibTestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TMathLibTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_Ctor);
    AddTest(new TTest_Basic);
    AddTest(new TTest_Geo);
    AddTest(new TTest_GraphData);
    AddTest(new TTest_Range);
}

tCIDLib::TVoid TMathLibTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TMathLibTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TMathLibTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TMathLibTestApp   tfwappMathLib;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TMathLibTestApp>(&tfwappMathLib, &TMathLibTestApp::eTestThread)
    )
)

