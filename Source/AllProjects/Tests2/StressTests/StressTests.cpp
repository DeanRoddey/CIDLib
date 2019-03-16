//
// FILE NAME: StressTests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
#include    "StressTests.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TStressTestsApp,TTestFWApp)



// ---------------------------------------------------------------------------
//  CLASS: TStressTestsApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TStressTestsApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TStressTestsApp::TStressTestsApp()
{
}

TStressTestsApp::~TStressTestsApp()
{
}


// ----------------------------------------------------------------------------
//  TStressTestsApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TStressTestsApp::bInitialize(TString&)
{

    //
    //  We need to clean up any old object store's in case they got left
    //  around from the last time. We do this in the termination as well,
    //  but just in case that didn't work, we try it here again before the
    //  test is run.
    //
    TDirIter iterOld;
    TFindBuf fndbCur;
    if (iterOld.bFindFirst(L"*.CIDObjStore", fndbCur))
    {
        do
        {
            TFileSys::DeleteFile(fndbCur.pathFileName());
        }   while (iterOld.bFindNext(fndbCur));
    }

    return kCIDLib::True;
}

tCIDLib::TVoid TStressTestsApp::LoadTests()
{
    // Load our object store related tests
    AddTest(new TTest_ObjStore1);
    AddTest(new TTest_ObjStore2);
    AddTest(new TTest_ObjStore3);
    AddTest(new TTest_ObjStore4);
}

tCIDLib::TVoid TStressTestsApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TStressTestsApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TStressTestsApp::Terminate()
{
    //
    //  We try to clean up any of the object store files we created, so as
    //  not to leave them lying around.
    //
    TDirIter iterOld;
    TFindBuf fndbCur;
    if (iterOld.bFindFirst(L"*.CIDObjStore", fndbCur))
    {
        do
        {
            TFileSys::DeleteFile(fndbCur.pathFileName());
        }   while (iterOld.bFindNext(fndbCur));
    }
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TStressTestsApp   tfwappObjSt;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TStressTestsApp>(&tfwappObjSt, &TStressTestsApp::eTestThread)
    )
)

