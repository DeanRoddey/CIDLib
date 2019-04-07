//
// FILE NAME: TestORB.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/26/2009
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
#include    "TestORB.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TORBTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TORBTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TORBTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TORBTestApp::TORBTestApp()
{
}

TORBTestApp::~TORBTestApp()
{
}


// ----------------------------------------------------------------------------
//  TORBTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TORBTestApp::bInitialize(TString& )
{
    //
    //  We need to start up the client and server sides of the ORB. We just
    //  use a very unlikely to be used port. We only accept one client max.
    //
    facCIDOrb().InitServer(9876, 1);
    facCIDOrb().InitClient();

    return kCIDLib::True;
}

tCIDLib::TVoid TORBTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_ORBBasic);
    AddTest(new TTest_ORBLoopback);
}

tCIDLib::TVoid TORBTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TORBTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TORBTestApp::Terminate()
{
    // We need to stop the ORB
    facCIDOrb().Terminate();
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TORBTestApp   tfwappNet;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TORBTestApp>(&tfwappNet, &TORBTestApp::eTestThread)
    )
)

