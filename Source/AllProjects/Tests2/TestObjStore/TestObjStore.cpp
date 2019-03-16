//
// FILE NAME: TestObjStore.cpp
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
#include    "TestObjStore.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TObjStTestApp,TTestFWApp)



// ---------------------------------------------------------------------------
//  CLASS: TObjStTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TObjStTestApp: Public, static data
// ----------------------------------------------------------------------------
const TString TObjStTestApp::strKey1(L"/TestData/AreaOne");
const TString TObjStTestApp::strKey2(L"/TestData/AreaTwo");
const TString TObjStTestApp::strKey3(L"/BasicTest3/Test1");
const TString TObjStTestApp::strKey4(L"/BasicTest3/Test2");
const TString TObjStTestApp::strKey5(L"/TestData2/AreaOne");


// ----------------------------------------------------------------------------
//  TObjStTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TObjStTestApp::TObjStTestApp()
{
}

TObjStTestApp::~TObjStTestApp()
{
}


// ----------------------------------------------------------------------------
//  TObjStTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TObjStTestApp::bInitialize(TString&)
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

tCIDLib::TVoid TObjStTestApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_Basic1);
    AddTest(new TTest_Basic2);
    AddTest(new TTest_Basic3);
    AddTest(new TTest_Basic4);
    AddTest(new TTest_Basic5);
}

tCIDLib::TVoid TObjStTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TObjStTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TObjStTestApp::Terminate()
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
TObjStTestApp   tfwappObjSt;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TObjStTestApp>(&tfwappObjSt, &TObjStTestApp::eTestThread)
    )
)

