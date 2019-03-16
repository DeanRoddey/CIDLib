//
// FILE NAME: TestCIDLib_RTTI.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/04/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This module implements one of the test methods of the facility class.
//  This particular method excercises the RTTI and dynamic typing systems.
//  These are fundamental to so much stuff that they need to be very well
//  tested automatically.
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
//  Local functions
// -----------------------------------------------------------------------------

//
//  This method will test out the dynamic typing system, by trying to create
//  some objects of various type by name or via their class objects.
//
static tCIDLib::TVoid TestDynamicTyping(TTextOutStream& strmOut)
{
    TPathStr*   ppathTest;
    TError*     perrTest;
    TTime*      ptmTest;
    TString*    pstrTest;

    // Create a color object using the low level TClass method
    perrTest = (TError*)TClass::pobjMakeNewOfClass(L"TError");
    if (!perrTest->bIsA(TError::clsThis()))
    {
        strmOut << CUR_LN
                << L"Dynamically created TError object is not of type"
                   L" TError (via low level method)" << kCIDLib::EndLn;
    }
    delete perrTest;
    perrTest = 0;

    //
    //  Now create one via the factory template method, which is more
    //  typesafe and checks for validity.
    //
    perrTest = ::pobjMakeNewOfClass<TError>(L"TError");
    if (!perrTest->bIsA(TError::clsThis()))
    {
        strmOut << CUR_LN
                << L"Dynamically created TError object is not of type"
                   L" TError (via template factory method)" << kCIDLib::EndLn;
    }
    delete perrTest;
    perrTest = 0;

    //
    //  Create a TTime object via its class object, which is yet another
    //  way to do it.
    //
    ptmTest = static_cast<TTime*>(TTime::clsThis().pobjMakeNew());
    if (!ptmTest->bIsA(TTime::clsThis()))
    {
        strmOut << CUR_LN
                << L"Dynamically created TTime object is not of type"
                   L" TTime (via class object.)" << kCIDLib::EndLn;
    }
    delete ptmTest;

    //
    //  And create another type of object via the factory template
    //  method.
    //
    ppathTest = ::pobjMakeNewOfClass<TPathStr>(L"TPathStr");
    if (!ppathTest->bIsA(TPathStr::clsThis()))
    {
        strmOut << CUR_LN
                << L"Dynamically created TPathStr object is not of type"
                   L" TPathStr (via template factory method)" << kCIDLib::EndLn;
    }
    delete ppathTest;
    ppathTest = 0;


    //
    //  Now purposefully try to dynamically create an error object into the
    //  pointer to the path string. This should cause an exception, so make
    //  sure it happens.
    //
    tCIDLib::TBoolean bCaughtIt = kCIDLib::False;
    try
    {
        pstrTest = ::pobjMakeNewOfClass<TString>(TString(L"TError"));
    }

    catch(...)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut <<  CUR_LN
                <<  L"Did not catch a dynamic object creation into wrong"
                    L" type of pointer" << kCIDLib::EndLn;
    }
}


//
//  This function will test the runtime type identification system out to
//  make sure that it is basically working ok.
//
static tCIDLib::TVoid TestCoreRTTI(TTextOutStream& strmOut)
{
    //
    //  Make sure two class objects constructed with the same class name
    //  compare ok.
    //
    {
        TClass clsTest(L"TestClassName");
        TClass clsTest2(L"TestClassName");
        if (clsTest != clsTest2)
        {
            strmOut << CUR_LN
                    << L"Class objects of same class did not compare" << kCIDLib::EndLn;
        }
    }

    //
    //  Make sure two class objects constructed with different class names
    //  don't compare ok.
    //
    {
        TClass   clsTest(L"TestClassName");
        TClass   clsTest2(L"AnotherClassName");
        if (clsTest == clsTest2)
        {
            strmOut << CUR_LN
                    << L"Class objects of different classes were equal"
                    << kCIDLib::EndLn;
        }
    }

    //
    //  Test out some known relationships in the class hierarchy.
    //
    TPathStr    pathTest(L"A test");
    if (!pathTest.bIsDescendantOf(TString::clsThis()))
    {
        strmOut << CUR_LN
                <<  L"TPathStr->TString descendence test failed" << kCIDLib::EndLn;
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the RTTI and Dynamic Typing systems.
//
tCIDLib::TVoid TFacTestCIDLib::TestRTTI()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"Base RTTI";
        TestCoreRTTI(strmOut());

        pszCurTest = L"Dynamic Typing";
        TestDynamicTyping(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" test" << kCIDLib::EndLn;
        throw;
    }
}

