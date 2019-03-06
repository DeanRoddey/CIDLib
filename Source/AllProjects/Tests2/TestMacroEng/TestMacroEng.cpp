//
// FILE NAME: TestMacroEng.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2007
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
#include    "TestMacroEng.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TMacroEngTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TMacroEngTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TMacroEngTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TMacroEngTestApp::TMacroEngTestApp()
{
}

TMacroEngTestApp::~TMacroEngTestApp()
{
}


// ----------------------------------------------------------------------------
//  TMacroEngTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TMacroEngTestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}

tCIDLib::TVoid TMacroEngTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_PathValidation);
    AddTest(new TTest_NumConstProbe);
    AddTest(new TTest_CMLRuntime);
}

tCIDLib::TVoid TMacroEngTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TMacroEngTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TMacroEngTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TMacroEngTestApp   tfwappMacroEng;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TMacroEngTestApp>(&tfwappMacroEng, &TMacroEngTestApp::eTestThread)
    )
)

