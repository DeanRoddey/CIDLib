//
// FILE NAME: TestCIDMData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
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
//  Include our public header and all of the test file headers so that we can add
//  them to our list of tests to run.
// -----------------------------------------------------------------------------
#include    "TestCIDMData.hpp"
#include    "TestCIDMData_AttrData.hpp"
#include    "TestCIDMData_ChunkedFile.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TMDataTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TMDataTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TMDataTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TMDataTestApp::TMDataTestApp()
{
}

TMDataTestApp::~TMDataTestApp()
{
}


// ----------------------------------------------------------------------------
//  TMDataTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TMDataTestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TMDataTestApp::LoadTests()
{
    // Set our attribute data tests
    AddTest(new TTest_AttrDataBasic);

    // Set our chunked file tests
    AddTest(new TTest_ChunkedBasic);
    AddTest(new TTest_ChunkedMeta);
    AddTest(new TTest_ChunkedExt);
}

tCIDLib::TVoid TMDataTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TMDataTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TMDataTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TMDataTestApp   tfwappMData;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread entry point
//  of the base test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TMDataTestApp>(&tfwappMData, &TMDataTestApp::eTestThread)
    )
)

