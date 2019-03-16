//
// FILE NAME: TestObjStore_BasicTests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This file implements a set of basic object store tests.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestObjStore.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_Basic1, TTestFWTest)
RTTIDecls(TTest_Basic2, TTestFWTest)
RTTIDecls(TTest_Basic3, TTestFWTest)
RTTIDecls(TTest_Basic4, TTestFWTest)
RTTIDecls(TTest_Basic5, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Basic1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Basic1::TTest_Basic1() :

    TTestFWTest
    (
        L"Basic Tests 1", L"Simple open, write, read, and close test", 4
    )
{
}

TTest_Basic1::~TTest_Basic1()
{
}



// ---------------------------------------------------------------------------
//  TTest_Basic1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Basic1::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    TCIDObjStore oseTest;

    TArea area1(1, 2, 3, 4);
    TArea area2(10, 11, 12, 13);
    tCIDLib::TCard4 c4Ver1 = 1;
    tCIDLib::TCard4 c4Ver2 = 1;
    tCIDLib::TCard4 c4VerSave;
    TArea areaTest;
    TString strTest;

    // Initialize the object store
    if (!oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        strmOut << TFWCurLn
                << L"Should have created the store, not opened it\n\n";
    }

    // Do an initial validation of the store
    oseTest.ValidateStore(strmOut);

    // We should currently have zero items in the store
    if (oseTest.c4ObjectsInStore() != 0)
    {
        strmOut << TFWCurLn
                << L"Expected to find 0 items in the store, but found "
                << oseTest.c4ObjectsInStore() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Write an object. This gives us back an id.
    oseTest.AddObject(TObjStTestApp::strKey1, area1);

    // We should now have one item in the store
    if (oseTest.c4ObjectsInStore() != 1)
    {
        strmOut << TFWCurLn
                << L"Expected to find 1 items in the store, but found "
                << oseTest.c4ObjectsInStore() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  And let's try to read it back. We shouldn't get any new data, since
    //  we've not modified it.
    //
    if (oseTest.bReadObject(TObjStTestApp::strKey1, c4Ver1, areaTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should not have gotten new data\n\n";
    }

    //
    //  Verify that we will get new data if we pass in a version of zero,
    //  since zero is never a valid version number.
    //
    tCIDLib::TCard4 c4FakeVer = 0;
    areaTest.ZeroAll();
    if (!oseTest.bReadObject(TObjStTestApp::strKey1, c4FakeVer, areaTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else
    {
        // Make sure we got the right data
        if (areaTest != area1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Read value != to original\n\n";
        }
    }

    //
    //  Now update the object value. We should get a new version back each
    //  time, one higher than before.
    //
    area1.Set(2, 3, 4, 5);
    c4VerSave = c4Ver1;
    c4Ver1 = oseTest.c4UpdateObject(TObjStTestApp::strKey1, area1);
    if (c4Ver1 != c4VerSave + 1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Expected version " << (c4VerSave + 1)
                << L" but got " << c4Ver1 << kCIDLib::DNewLn;
    }

    //
    //  Read it back and make sure we get the new value. We have to use
    //  the fake version again to force it to give us data.
    //
    c4FakeVer = 0;
    if (!oseTest.bReadObject(TObjStTestApp::strKey1, c4FakeVer, areaTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else
    {
        // Make sure we got the right data
        if (areaTest != area1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Read value != to updated value\n\n";
        }
    }

    //
    //  Now lets write out a second object, and do the most basic tests
    //  to make sure that we get back the right object via key and id.
    //
    oseTest.AddObject(TObjStTestApp::strKey2, area2);

    // We should now have two items in the store
    if (oseTest.c4ObjectsInStore() != 2)
    {
        strmOut << TFWCurLn
                << L"Expected to find 2 items in the store, but found "
                << oseTest.c4ObjectsInStore() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    if (oseTest.bReadObject(TObjStTestApp::strKey2, c4Ver2, areaTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should not have gotten new data\n\n";
    }

    // And do a forced read of the second object now
    c4FakeVer = 0;
    areaTest.ZeroAll();
    c4FakeVer = 0;
    if (!oseTest.bReadObject(TObjStTestApp::strKey2, c4FakeVer, areaTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else
    {
        // Make sure we got the right data
        if (areaTest != area2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Read value != to original\n\n";
        }
    }


    // And go back and read the first object again
    areaTest.ZeroAll();
    c4FakeVer = 0;
    if (!oseTest.bReadObject(TObjStTestApp::strKey1, c4FakeVer, areaTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else
    {
        // Make sure we got the right data
        if (areaTest != area1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Read value != to original\n\n";
        }
    }

    // Validate the store and close it
    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    // Reopen it again, revalidate, and close again
    if (oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Basic2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Basic2::TTest_Basic2() :

    TTestFWTest
    (
        L"Basic Tests 2", L"Re-opens store from previous test and verifies it", 4
    )
{
}

TTest_Basic2::~TTest_Basic2()
{
}


// ---------------------------------------------------------------------------
//  TTest_Basic2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Basic2::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TCIDObjStore oseTest;

    TArea area1;
    TArea area2;
    tCIDLib::TCard4 c4Ver1 = 0;
    tCIDLib::TCard4 c4Ver2 = 0;

    // Initialize the object store
    if (oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    // Do an initial validation of the store
    oseTest.ValidateStore(strmOut);

    // There should be two objects
    if (oseTest.c4ObjectsInStore() != 2)
    {
        strmOut << TFWCurLn
                << L"Expected to find 2 items in the store, but found "
                << oseTest.c4ObjectsInStore() << kCIDLib::NewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Read in the two objects we wrote above and check their values
    if (!oseTest.bReadObject(TObjStTestApp::strKey1, c4Ver1, area1))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else
    {
        // Make sure we got the right data
        if (area1 != TArea(2, 3, 4, 5))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Read value != to written\n\n";
        }
    }

    if (!oseTest.bReadObject(TObjStTestApp::strKey2, c4Ver2, area2))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else
    {
        // Make sure we got the right data
        if (area2 != TArea(10, 11, 12, 13))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Read value != to written\n\n";
        }
    }

    //
    //  Make sure that we get an error if we try to reinitialize the
    //  store when it already is.
    //
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        oseTest.bInitialize(L".\\", L"BasicTestStore");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent( facCIDObjStore().strName()
                                    , kObjSErrs::errcInit_AlreadyInitialized))
        {
            bGotIt = kCIDLib::True;
        }
    }

    if (!bGotIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Did not catch double init\n\n";
    }

    //
    //  And close it and make sure that a double close doesn't do damage.
    //  It doesn't throw or anything.
    //
    oseTest.Close();
    oseTest.Close();

    // And lets try re-initializing the existing store object
    if (oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    // Validate the store and close it
    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Basic3: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Basic3::TTest_Basic3() :

    TTestFWTest
    (
        L"Basic Tests 3", L"Forces forced expansion of store objects", 4
    )
{
}

TTest_Basic3::~TTest_Basic3()
{
}


// ---------------------------------------------------------------------------
//  TTest_Basic3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Basic3::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TCIDObjStore oseTest;

    TString strValue;
    TString strTest;
    tCIDLib::TCard4 c4Ver3 = 0;
    tCIDLib::TCard4 c4Ver4 = 0;

    // Initialize the object store
    if (oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    // Do an initial validation of the store
    oseTest.ValidateStore(strmOut);

    // Create a test string object, with a reserve of 16 chars
    strValue = L"Initial value";
    oseTest.AddObject(TObjStTestApp::strKey3, strValue, 16 * kCIDLib::c4CharBytes);

    // Update the value within the reserve range, and read it back
    strValue.Append(L" extended ");
    c4Ver3 = oseTest.c4UpdateObject(TObjStTestApp::strKey3, strValue);
    if (oseTest.bReadObject(TObjStTestApp::strKey3, c4Ver3, strTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should not have gotten new data\n\n";
    }

    c4Ver3 = 0;
    if (!oseTest.bReadObject(TObjStTestApp::strKey3, c4Ver3, strTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else if (strTest != L"Initial value extended ")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Value was not updated correctly\n\n";
    }

    // Now force it to go beyond its reserve space
    strValue.Append(L"beyond it's reserve capacity");
    c4Ver3 = oseTest.c4UpdateObject(TObjStTestApp::strKey3, strValue);
    if (oseTest.bReadObject(TObjStTestApp::strKey3, c4Ver3, strTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should not have gotten new data\n\n";
    }
    c4Ver3 = 0;
    if (!oseTest.bReadObject(TObjStTestApp::strKey3, c4Ver3, strTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else if (strTest != L"Initial value extended beyond it's reserve capacity")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Value was not updated correctly\n\n";
    }

    //
    //  Add another string now, which will be between the first string and
    //  the available free space.
    //
    strValue = L"Value 2";
    oseTest.AddObject(TObjStTestApp::strKey4, strValue, 16 * kCIDLib::c4CharBytes);

    //
    //  Now try to expand the first string again. It will not be able to
    //  expand into adjacent free space now, so it'll have to move the
    //  storage.
    //
    strValue = L"Initial value extended beyond it's reserve capacity, again";
    c4Ver3 = oseTest.c4UpdateObject(TObjStTestApp::strKey3, strValue);

    // Force a re-read of it and check the value
    c4Ver3 = 0;
    if (!oseTest.bReadObject(TObjStTestApp::strKey3, c4Ver3, strTest))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have gotten new data\n\n";
    }
     else if (strTest != L"Initial value extended beyond it's reserve capacity, again")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Value was not updated correctly\n\n";
    }

    // Validate the store and close it
    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    // Reopen it again, revalidate, and close again
    if (oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic4
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Basic4: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Basic4::TTest_Basic4() :

    TTestFWTest
    (
        L"Basic Tests 4", L"Tests store object deletion", 4
    )
{
}

TTest_Basic4::~TTest_Basic4()
{
}


// ---------------------------------------------------------------------------
//  TTest_Basic4: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Basic4::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    TCIDObjStore oseTest;

    if (oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    //
    //  And now let's delete all the objects from the store and then validate
    //  to insure that something didn't go awry.
    //
    oseTest.DeleteObject(TObjStTestApp::strKey1);
    oseTest.DeleteObject(TObjStTestApp::strKey2);
    oseTest.DeleteObject(TObjStTestApp::strKey3);
    oseTest.DeleteObject(TObjStTestApp::strKey4);

    // Validate the store and close it
    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    // Reopen it again, revalidate, and close again
    if (oseTest.bInitialize(L".\\", L"BasicTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic5
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Basic5: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Basic5::TTest_Basic5() :

    TTestFWTest
    (
        L"Basic Tests 5", L"Key finding tests", 4
    )
{
}

TTest_Basic5::~TTest_Basic5()
{
}



// ---------------------------------------------------------------------------
//  TTest_Basic5: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Basic5::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    TCIDObjStore oseTest;

    TArea area1(1, 2, 3, 4);
    tCIDLib::TCard4 c4Ver;
    TArea areaTest;
    TString strTest;

    // Initialize the object store
    if (!oseTest.bInitialize(L".\\", L"FindTestStore"))
    {
        strmOut << TFWCurLn
                << L"Should have created the store, not opened it\n\n";
    }

    // Do an initial validation of the store
    oseTest.ValidateStore(strmOut);

    // We should currently have zero items in the store
    if (oseTest.c4ObjectsInStore() != 0)
    {
        strmOut << TFWCurLn
                << L"Expected to find 0 items in the store, but found "
                << oseTest.c4ObjectsInStore() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Write an object
    oseTest.AddObject(TObjStTestApp::strKey1, area1);
    oseTest.AddObject(TObjStTestApp::strKey2, area1);
    oseTest.AddObject(TObjStTestApp::strKey3, area1);
    oseTest.AddObject(TObjStTestApp::strKey4, area1);
    oseTest.AddObject(TObjStTestApp::strKey5, area1);

    // Test the key exists method
    if (!oseTest.bKeyExists(TObjStTestApp::strKey1)
    ||  !oseTest.bKeyExists(TObjStTestApp::strKey2, c4Ver))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Added keys not found\n\n";
    }

    // Test the one that finds a given key under a given portion of the tree
    tCIDLib::TStrList colKeys;
    if (!oseTest.bFindNameUnder(L"AreaOne", L"/", colKeys))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Added keys not found in tree\n\n";
    }

    if (colKeys.c4ElemCount() != 2)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have found two matching keys\n\n";
    }

    // Validate the store and close it
    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    return eRes;
}

