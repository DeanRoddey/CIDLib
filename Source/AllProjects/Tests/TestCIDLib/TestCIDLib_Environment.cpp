//
// FILE NAME: TestCIDLib_Environment.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/16/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module is part of the TestCIDLib.Exe program. This module is called
//  from the program's main function. The functions in this module test the
//  environment classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"



// -----------------------------------------------------------------------------
//  Local static data
//
//  c4Pass
//      We can be called multiple times so we have to be sure that we
//      don't try to add the same keys multiple times.
// -----------------------------------------------------------------------------
static tCIDLib::TCard4  c4Pass = 0;


// -----------------------------------------------------------------------------
//  Local static functions
// -----------------------------------------------------------------------------

static tCIDLib::TVoid BasicProcEnvTest(TTextOutStream& strmOut)
{
    //
    //  First we test out the process environment class, which works on the
    //  process wide environment.
    //
    //  We start by looking for some values that we know should be there.
    //
    tCIDLib::TCh* apszToFind[] =
    {
        L"Path"
        , L"CID_DRIVE"
        , L"COMPUTERNAME"
        , L"INCLUDE"
        , L"SystemRoot"
    };
    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(apszToFind);

    TString strValue;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (!TProcEnvironment::bFind(apszToFind[c4Index], strValue))
        {
            strmOut << CUR_LN << L"Could not find " << apszToFind[c4Index]
                    << L" in the process environment" << kCIDLib::EndLn;
        }
    }

    // And make sure that we don't find some that we don't exists.
    if (TProcEnvironment::bFind(L"DontExist", strValue))
        strmOut << CUR_LN << L"Seemed to find var that does not exist" << kCIDLib::EndLn;


    //
    //  Test for some operations that should fail if the variable does not
    //  already exists.
    //
    tCIDLib::TBoolean bCaughtIt = kCIDLib::False;
    try
    {
        TProcEnvironment::Update(L"DontExist", L"Dummy Value");
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcEnv_KeyNotFound))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Expected VarNotFound exception" << kCIDLib::EndLn;

            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
        strmOut << CUR_LN << L"Did not get exception on non-existent var" << kCIDLib::EndLn;


    //
    //  Now check a couple of test values that we know are in the environment
    //  and make sure that their values are correct.
    //
    if (!TProcEnvironment::bFind(L"TestVar1", strValue))
    {
        strmOut << CUR_LN << L"Did not find TestVar1 variable" << kCIDLib::EndLn;
    }
     else
    {
        if (strValue != L"The value of TestVar1")
            strmOut << CUR_LN << L"Value of TestVar1 was not correct" << kCIDLib::EndLn;
    }

    if (!TProcEnvironment::bFind(L"TestVar2", strValue))
    {
        strmOut << CUR_LN << L"Did not find TestVar2 variable" << kCIDLib::EndLn;
    }
     else
    {
        if (strValue != L"The value of TestVar2")
            strmOut << CUR_LN << L"Value of TestVar2 was not correct" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid ProcEnvTest2(TTextOutStream& strmOut)
{
    const tCIDLib::TCh* const pszDummyKey = L"ADummyKey1";

    const tCIDLib::TCh* const pszDummyVal = L"A Dummy Value for Dummy Key 1";

    TString strValue;

    //
    //  Add a value and make sure that we can read it back. Get the element
    //  count first, then check that it went up by one.
    //
    tCIDLib::TCard4 c4EnvCount = TProcEnvironment::c4Entries();
    TProcEnvironment::Add(pszDummyKey, pszDummyVal);
    if (TProcEnvironment::c4Entries() != c4EnvCount+1)
        strmOut << CUR_LN << L"Adding a key did not bump up entry count" << kCIDLib::EndLn;

    if (!TProcEnvironment::bFind(pszDummyKey, strValue))
        strmOut << CUR_LN << L"Did not find a key that was just added" << kCIDLib::EndLn;

    //
    //  Now try to add the same element again. This should cause an exception
    //  that the variable already exists.
    //
    tCIDLib::TBoolean bCaughtIt = kCIDLib::False;
    try
    {
        TProcEnvironment::Add(pszDummyKey, pszDummyVal);
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcEnv_KeyAlreadyExists))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got wrong exception when adding existing key"
                    << kCIDLib::EndLn;

            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
        strmOut << CUR_LN << L"Failed to catch addition of existing key" << kCIDLib::EndLn;

    //
    //  Now do an 'add or update' on that key, which should cause the value
    //  to be modified. Make sure that it returns false, which indicates that
    //  it did not add the element, just updated it.
    //
    if (TProcEnvironment::bAddOrUpdate
    (
        pszDummyKey, L"1 Dummy Value for Dummy Key"))
    {
        strmOut << CUR_LN << L"AddOrUpdate added when it should have updated"
                << kCIDLib::EndLn;
    }

    // And make sure that the new value is what it should be
    if (TProcEnvironment::strFind(pszDummyKey) != L"1 Dummy Value for Dummy Key")
    {
        strmOut << CUR_LN << L"AddOrUpdate did not update value" << kCIDLib::EndLn;
    }
}

static tCIDLib::TVoid BasicEnvTest(TTextOutStream& strmOut)
{
    static tCIDLib::TCh* apszKeys[] =
    {
        L"Jimi_Hendrix"
        , L"Janis_Joplin"
        , L"Jim_Morrison"
        , L"Mama_Cass"
        , L"John_Lennnon"
    };

    static tCIDLib::TCh* apszValues[] =
    {
        L"Greatest blues rock guitarist"
        , L"A white chick what can sang"
        , L"Prentitious but interesting singer for Doors"
        , L"One of the Mamas and Papas"
        , L"The soul of the beatles"
    };

    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(apszKeys);


    //
    //  Create an empty environment first and just do some basic testing
    //  on it.
    //
    TEnvironment envTest;

    //
    //  Add a couple of values and keys to it.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        envTest.Add(apszKeys[c4Index], apszValues[c4Index]);

    // Check that the entry count is value
    if (envTest.c4Entries() != c4Count)
        strmOut << CUR_LN << L"The entry count is not correct" << kCIDLib::EndLn;

    //
    //  Check that we can find each one and that its value is the same
    //  as what we put in there.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (envTest.strFind(apszKeys[c4Index]) != apszValues[c4Index])
        {
            strmOut << CUR_LN
                    << L"Value was not as originally set for key: "
                    << apszKeys[c4Index] << kCIDLib::EndLn;
        }
    }
}

static tCIDLib::TVoid AdvEnvTest(TTextOutStream& strmOut)
{
    // Create an environment that inherits the current process environment
    TEnvironment envTest(kCIDLib::True);

    // Make sure it has the same number of entries as the main environment
    if (envTest.c4Entries() != TProcEnvironment::c4Entries())
    {
        strmOut << CUR_LN
                << L"Test env did not inherit same number of entries as process has"
                << kCIDLib::EndLn;
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the environment classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestEnvironment()
{
    // Don't do the second pass for this one
    if (c4Pass)
        return;

    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        //
        //  Invoke the tests that we want to do memory checking on. These
        //  only do stuff that doesn't cause spurious leak reports.
        //
        {
            pszCurTest = L"basic process environment";
            BasicProcEnvTest(strmOut());

            pszCurTest = L"basic environment";
            BasicEnvTest(strmOut());

            pszCurTest = L"advanced environment";
            AdvEnvTest(strmOut());
        }

        //
        //  Now do the tests that interfere with the heap checking stuff.
        //
        pszCurTest = L"more process environment";
        ProcEnvTest2(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" environment test" << kCIDLib::EndLn;
        throw;
    }

    // Bump our pass counter
    c4Pass++;
}
