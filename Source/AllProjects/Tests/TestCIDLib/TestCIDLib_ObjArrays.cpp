//
// FILE NAME: TestCIDLib_ObjArrays.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/1993
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  object array collection class.
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
//  Force some instantiations
// -----------------------------------------------------------------------------
typedef TObjArray<TFloat>       TObjArrayOfTFloat;
typedef TObjArray<TCardinal>    TObjArrayOfTCardinal;


// -----------------------------------------------------------------------------
//  Local static functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestObjArrayBasics(TTextOutStream& strmOut)
{
    TObjArrayOfTFloat  objaTest(16);

    tCIDLib::TCard4 c4Ind;
    for (c4Ind = 0; c4Ind < 16; c4Ind++)
        objaTest[c4Ind] = TFloat(c4Ind);

    for (c4Ind = 0; c4Ind < 16; c4Ind++)
    {
        if (objaTest[c4Ind] != TFloat(c4Ind))
        {
            strmOut << CUR_LN
                    << L"Value in object array was not equal to set value" << kCIDLib::EndLn;
            break;
        }
    }

    // Exchange the first and last element
    objaTest.ExchangeElems(0, 15);

    if (objaTest[0] != TFloat(15))
        strmOut << CUR_LN
                << L"Exchanged element of object array was incorrect" << kCIDLib::EndLn;

    if (objaTest[15] != TFloat(0))
        strmOut << CUR_LN
                << L"Exchanged element of object array was incorrect" << kCIDLib::EndLn;

    // Make sure it catches an index error
    tCIDLib::TBoolean  bCaught(kCIDLib::False);
    try
    {
        if (objaTest[16] != TFloat(0))
            objaTest[16] = TFloat(0);
    }

    catch(...)
    {
        bCaught = kCIDLib::True;
    }

    if (!bCaught)
        strmOut << CUR_LN << L"Index error in object array was not caught" << kCIDLib::EndLn;

    // Overwrite element 4
    objaTest[4] = TFloat(255);

    if (objaTest[4] != TFloat(255))
        strmOut << CUR_LN
                << L"Element put into object array did overwrite element" << kCIDLib::EndLn;

    // Over another one via the index operator
    objaTest[5] = TFloat(128);

    if (objaTest[5] != TFloat(128))
        strmOut << CUR_LN
                << L"Element put into object array did overwrite element" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestObjArrayCopyDup(TTextOutStream& strmOut)
{
    static const tCIDLib::TCard4 c4Count = 64;

    // Create and fill a test array
    tCIDLib::TCard4         c4Ind;
    TObjArrayOfTCardinal    objaTest(c4Count);
    for (c4Ind = 0; c4Ind < c4Count; c4Ind++)
        objaTest[c4Ind] = c4Ind;

    // Create another array by copy constructor
    TObjArrayOfTCardinal objaCopy(objaTest);

    // Compare the elements
    for (c4Ind = 0; c4Ind < c4Count; c4Ind++)
    {
        if (objaTest[c4Ind] != objaCopy[c4Ind])
        {
            strmOut << CUR_LN << L"Copy of array elements failed" << kCIDLib::EndLn;
            break;
        }
    }

    // Do a comparison of the arrays
    if (objaTest != objaCopy)
    {
        strmOut << CUR_LN << L"Standard array comparision function failed"
                << kCIDLib::EndLn;
    }

    //
    //  Dup the array into another dynamically allocated array and do the
    //  same test.
    //
    TObjArrayOfTCardinal* pobjaNew;
    pobjaNew = ::pDupObject<TObjArrayOfTCardinal>(objaTest);
    TJanitor<TObjArrayOfTCardinal> janNewArray(pobjaNew);

    for (c4Ind = 0; c4Ind < c4Count; c4Ind++)
    {
        if (objaTest[c4Ind] != (*pobjaNew)[c4Ind])
        {
            strmOut << CUR_LN << L"Dup of array failed" << kCIDLib::EndLn;
            break;
        }
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the object array collections.
//
tCIDLib::TVoid TFacTestCIDLib::TestObjArrays()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        TestObjArrayBasics(strmOut());

        pszCurTest = L"copy/dup";
        TestObjArrayCopyDup(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" object array test" << kCIDLib::EndLn;
        throw;
    }
}
