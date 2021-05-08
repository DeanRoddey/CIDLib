//
// FILE NAME: TestRegX.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
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
#include    "TestRegX.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TRegXTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TRegXTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TRegXTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TRegXTestApp::TRegXTestApp()
{
}

TRegXTestApp::~TRegXTestApp()
{
}


// ----------------------------------------------------------------------------
//  TRegXTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TRegXTestApp::bInitialize(TString& strErr)
{
    // For testing an error return
    #if 0
    strErr.LoadMsg
    (
        facTestRegX()
        , kCIDErrs::errcGen_BadEnumValue
        , TString(L"Ting")
        , TString(L"Tang")
    );
    return kCIDLib::False;
    #endif

    return kCIDLib::True;
}

tCIDLib::TVoid TRegXTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_Parse);
    AddTest(new TTest_Match);
    AddTest(new TTest_RepAll);
    AddTest(new TTest_CpMv);
    AddTest(new TTest_Misc);
}

tCIDLib::TVoid TRegXTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TRegXTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TRegXTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TRegXTestApp   tfwappRegX;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TRegXTestApp>(&tfwappRegX, &TRegXTestApp::eTestThread)
    )
)

