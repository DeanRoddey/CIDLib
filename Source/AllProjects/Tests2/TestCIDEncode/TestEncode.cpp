//
// FILE NAME: TestEncode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/15/2008
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
#include    "TestEncode.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TEncodeTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TEncodeTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TEncodeTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TEncodeTestApp::TEncodeTestApp()
{
}

TEncodeTestApp::~TEncodeTestApp()
{
}


// ----------------------------------------------------------------------------
//  TEncodeTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TEncodeTestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TEncodeTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_ByName);
    AddTest(new TTest_Convert);
    AddTest(new TTest_ErrModes);
    AddTest(new TTest_RoundTrip1);
}

tCIDLib::TVoid TEncodeTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TEncodeTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TEncodeTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TEncodeTestApp   tfwappRegX;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TEncodeTestApp>(&tfwappRegX, &TEncodeTestApp::eTestThread)
    )
)

