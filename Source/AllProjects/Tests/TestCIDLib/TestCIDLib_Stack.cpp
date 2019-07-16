//
// FILE NAME: TestCIDLib_Stacks.cpp
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
//  Stack collection classes.
//
//  This module brings in a common set of collection tests that are implemented
//  as templates themselves so that they can be invoked for all types of
//  collections without redundancy. But some of the testing is always specific
//  to the particular collection type.
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
#include    "TestCIDLib_CommonCollect.hpp"


// -----------------------------------------------------------------------------
//  Typedef some instantiations we want to test
// -----------------------------------------------------------------------------
typedef TStack<TInteger>    TStackOfTInteger;
typedef TStack<TPoint>      TStackOfTPoint;
typedef TRefStack<TFloat>   TRefStackOfTFloat;


// -----------------------------------------------------------------------------
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class TStack<TInteger>;
template class TRefStack<TFloat>;



// -----------------------------------------------------------------------------
//  Local static functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestStackBasics(TTextOutStream& strmOut)
{
    // Create a stack of points and push on some values
    TStackOfTPoint  colTest;

    // Push on some values that are easy to check the order of
    colTest.objPush(TPoint(0,0));
    colTest.objPush(TPoint(1,1));
    colTest.objPush(TPoint(2,2));
    colTest.objPush(TPoint(3,3));

    //
    //  Pop the values off and make sure that they are in the correct reverse
    //  order of pushing.
    //
    for (tCIDLib::TInt4 i4Index = 3; i4Index >= 0; i4Index--)
    {
        TPoint pntCur = colTest.objPop();

        if (pntCur != TPoint(i4Index, i4Index))
        {
            strmOut << CUR_LN
                    << L"Stack pops were not reverse of pushes. Expected: "
                    << TPoint(i4Index, i4Index)
                    << L" but got:" << pntCur
                    << kCIDLib::EndLn;
            break;
        }
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the Stack collections.
//
tCIDLib::TVoid TFacTestCIDLib::TestStacks()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        TestStackBasics(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" stack test" << kCIDLib::EndLn;
        throw;
    }
}

