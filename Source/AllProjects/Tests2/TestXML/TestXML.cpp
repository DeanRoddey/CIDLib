//
// FILE NAME: TestRegX.cpp
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
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "TestXML.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TXMLTestApp,TTestFWApp)


// ---------------------------------------------------------------------------
//  CLASS: TXMLTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TXMLTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TXMLTestApp::TXMLTestApp()
{
}

TXMLTestApp::~TXMLTestApp()
{
}


// ----------------------------------------------------------------------------
//  TXMLTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TXMLTestApp::bInitialize(TString&)
{
    // Nothing to do
    return kCIDLib::True;
}


tCIDLib::TVoid TXMLTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_Attr);
}

tCIDLib::TVoid TXMLTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TXMLTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TXMLTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TXMLTestApp   tfwappRegX;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TXMLTestApp>(&tfwappRegX, &TXMLTestApp::eTestThread)
    )
)

