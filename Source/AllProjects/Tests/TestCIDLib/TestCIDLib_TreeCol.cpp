//
// FILE NAME: TestCIDLib_TreeCol.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/13/2001
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
//  Tree collection classes.
//
// CAVEATS/GOTCHAS:
//
//  1)  We don't call the common collection tests for this one, because the
//      generic objAdd() method isn't supported for the tree collection. It
//      just doesn't make any sense because the tree collection is completely
//      location oriented, and there is no way to indicate where to insert
//      a new node in the generic API.
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
//  Typedef some instantiations
// -----------------------------------------------------------------------------
using TTreeOfTString = TBasicTreeCol<TString>;


// -----------------------------------------------------------------------------
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class TBasicTreeCol<TString>;


// -----------------------------------------------------------------------------
//  Local test methods
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestTreeBasic(TTextOutStream& strmOut)
{
    TTreeOfTString  colTest;

    // Make sure its initially empty
    if (!colTest.bIsEmpty())
    {
        strmOut << CUR_LN << L"Tree was not initially empty" << kCIDLib::EndLn;
        return;
    }

    // And make sure the element count is zero
    if (colTest.c4ElemCount() != 0)
    {
        strmOut << CUR_LN << L"Initial tree element count != 0" << kCIDLib::EndLn;
        return;
    }

    //
    //  Add a few nodes under the root. Add them in non-sorted order, to
    //  make sure that the sorting of nodes is done right.
    //
    //  We create this tree (after its sorted during insertion)
    //
    //      /NT1
    //      /NT1/NT2
    //      /NT1/NT2/NT3
    //      /NT1/NT2/NT3/T6
    //      /NT1/T4
    //      /NT1/T5
    //      /T1
    //      /T2
    //      /T3
    //
    if (colTest.objAddTerminal(L"/", L"T1", TString(L"T1")) != L"T1")
    {
        strmOut << CUR_LN << L"Added non-terminal had wrong value on return"
                << kCIDLib::EndLn;
    }

    if (colTest.objAddTerminal(L"/", L"T3", TString(L"T3")) != L"T3")
    {
        strmOut << CUR_LN << L"Added non-terminal had wrong value on return"
                << kCIDLib::EndLn;
    }

    if (colTest.objAddTerminal(L"/", L"T2", TString(L"T2")) != L"T2")
    {
        strmOut << CUR_LN << L"Added non-terminal had wrong value on return"
                << kCIDLib::EndLn;
    }

    colTest.pnodeAddNonTerminal(L"/", L"NT1");

    // Check the node counts to make sure that they are correct
    if (colTest.c4TerminalCount() != 3)
    {
        strmOut << CUR_LN << L"Terminal count was incorrct" << kCIDLib::EndLn;
        return;
    }

    if (colTest.c4NonTerminalCount() != 1)
    {
        strmOut << CUR_LN << L"Non-terminal count was incorrct" << kCIDLib::EndLn;
        return;
    }

    // Ok, now lets start adding deeper levels
    colTest.objAddTerminal(L"/NT1", L"T4", TString(L"T4"));
    colTest.objAddTerminal(L"/NT1", L"T5", TString(L"T5"));
    colTest.pnodeAddNonTerminal(L"/NT1", L"NT2");
    colTest.pnodeAddNonTerminal(L"/NT1/NT2", L"NT3");
    colTest.objAddTerminal(L"/NT1/NT2/NT3", L"T6", TString(L"T6"));

    // Make sure our counts are right
    if (colTest.c4NonTerminalCount() != 3)
    {
        strmOut << CUR_LN << L"Non-terminal count was incorrct" << kCIDLib::EndLn;
        return;
    }

    if (colTest.c4TerminalCount() != 6)
    {
        strmOut << CUR_LN << L"Terminal count was incorrct" << kCIDLib::EndLn;
        return;
    }

    //
    //  Now check each of the nodes we just added by looking them up by
    //  their path, and see if they have the right values.
    //
    if (colTest.objAt(L"/T1") != L"T1")
        strmOut << CUR_LN << L"T1 had wrong node on lookup" << kCIDLib::EndLn;

    if (colTest.objAt(L"/T2") != L"T2")
        strmOut << CUR_LN << L"T2 had wrong node on lookup" << kCIDLib::EndLn;

    if (colTest.objAt(L"/T3") != L"T3")
        strmOut << CUR_LN << L"T3 had wrong node on lookup" << kCIDLib::EndLn;

    if (colTest.objAt(L"/NT1/T4") != L"T4")
        strmOut << CUR_LN << L"T4 had wrong node on lookup" << kCIDLib::EndLn;

    if (colTest.objAt(L"/NT1/NT2/NT3/T6") != L"T6")
        strmOut << CUR_LN << L"T6 had wrong node on lookup" << kCIDLib::EndLn;

    const tCIDLib::TCh* apszNames[] =
    {
        L"NT1", L"NT2", L"NT3", L"T6", L"T4", L"T5", L"T1", L"T2", L"T3"
    };
    const tCIDLib::TCard4 c4NameCount = tCIDLib::c4ArrayElems(apszNames);

    const tCIDLib::TCard4 ac4Depths [] =
    {
        0, 1, 2, 3, 1, 1, 0, 0, 0
    };
    TTreeOfTString::TCursor cursTest(&colTest);
    if (!cursTest.bReset())
    {
        strmOut << CUR_LN << L"Cursor reset failed" << kCIDLib::EndLn;
        return;
    }

    tCIDLib::TCard4     c4Index = 0;
    do
    {
        if (c4Index == c4NameCount)
        {
            strmOut << CUR_LN << L"Got more names than expected" << kCIDLib::EndLn;
            break;
        }

        if (cursTest.strCurName() != apszNames[c4Index])
        {
            strmOut << CUR_LN << L"Failed to get expected iterated node"
                    << kCIDLib::EndLn;
            break;
        }

        if (cursTest.c4Depth() != ac4Depths[c4Index])
        {
            strmOut << CUR_LN << L"Failed to get expected node depth"
                    << kCIDLib::EndLn;
            break;
        }

        c4Index++;
    }   while (cursTest.bNext());


    // Now lets remove a non-terminal and make sure children gets cleaned up
    colTest.RemoveNode(L"/NT1");

    // There should be 3 terminals left and no non-terminals
    if (colTest.c4NonTerminalCount() != 0)
    {
        strmOut << CUR_LN << L"Non-terminal count was incorrct" << kCIDLib::EndLn;
        return;
    }

    if (colTest.c4TerminalCount() != 3)
    {
        strmOut << CUR_LN << L"Terminal count was incorrct" << kCIDLib::EndLn;
        return;
    }

    // And make sure they are really gone by testing a few
    if (colTest.bNodeExists(L"/NT1/NT2/NT3/T6")
    ||  colTest.bNodeExists(L"/NT1/NT2/NT3"))
    {
        strmOut << CUR_LN << L"Flushed nodes still exist" << kCIDLib::EndLn;
        return;
    }
}


static tCIDLib::TVoid TestTreeBasic2(TTextOutStream& strmOut)
{
    TTreeOfTString  colTest;

    // Create a non-terminal path via the key/value pair version
    tCIDLib::TKVPList colPairs;
    colPairs.objAdd(TKeyValuePair(L"Scope1", L"Description for Scope1"));
    colPairs.objAdd(TKeyValuePair(L"Scope2", L"Description for Scope2"));
    colPairs.objAdd(TKeyValuePair(L"Scope3", L"Description for Scope3"));
    colPairs.objAdd(TKeyValuePair(L"Scope4", L"Description for Scope4"));
    colTest.pnodeCreateNTPath(colPairs);

    // Make sure the path exists
    tCIDLib::ETreeNodes eType;
    if (!colTest.bNodeExists(L"/Scope1/Scope2/Scope3/Scope4", eType))
    {
        strmOut << CUR_LN << L"Failed to create k/v scope path" << kCIDLib::EndLn;
        return;
    }

    if (eType != tCIDLib::ETreeNodes::NonTerminal)
    {
        strmOut << CUR_LN << L"Created node was of wrong type" << kCIDLib::EndLn;
        return;
    }

    // Add in another non-terminal halfway down the path
    colTest.pnodeAddNonTerminal
    (
        L"/Scope1/Scope2"
        , L"AAA"
        , L"Description for AAA"
    );

    // And add a terminal under this new non-terminal
    colTest.objAddTerminal
    (
        L"/Scope1/Scope2/AAA"
        , L"Obj1"
        , L"Description of Obj1"
    );
}


static tCIDLib::TVoid TestTreeCase(TTextOutStream& strmOut)
{
    // Make one that uses case insensitive paths
    TTreeOfTString  colTest(tCIDLib::EMTStates::Unsafe, kCIDLib::False);

    // And some nodes with all upper case names
    colTest.pnodeAddNonTerminal(L"/", L"NT1");
    colTest.objAddTerminal(L"/NT1", L"T1", L"Value 1");
    colTest.objAddTerminal(L"/NT1", L"T2", L"Value 2");
    colTest.pnodeAddNonTerminal(L"/NT1", L"NT2");

    // Look one up by lower case
    if (!colTest.bNodeExists(L"/nt1/t2"))
    {
        strmOut << CUR_LN << L"Case insensitive path test failed" << kCIDLib::EndLn;
        return;
    }

    //
    //  Try to add one again by lower case, which should be rejected in
    //  this case, because when insensitive it'll be a dup.
    //
    tCIDLib::TBoolean bCaughtIt = kCIDLib::False;
    try
    {
        colTest.objAddTerminal(L"/nt1", L"t1", L"Value 3");
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_DupTreeNode))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got wrong exception" << kCIDLib::EndLn;
        }
         else
        {
            bCaughtIt = kCIDLib::True;
        }
    }

    if (!bCaughtIt)
    {
        strmOut << CUR_LN << L"Failed to catch insensitive dup" << kCIDLib::EndLn;
        return;
    }

    // And the same for a non-terminal
    bCaughtIt = kCIDLib::False;
    try
    {
        colTest.pnodeAddNonTerminal(L"/nt1", L"nt2");
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_DupTreeNode))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got wrong exception" << kCIDLib::EndLn;
        }
         else
        {
            bCaughtIt = kCIDLib::True;
        }
    }

    if (!bCaughtIt)
    {
        strmOut << CUR_LN << L"Failed to catch insensitive dup" << kCIDLib::EndLn;
        return;
    }
}


static tCIDLib::TVoid TestTreeNegative(TTextOutStream& strmOut)
{
    TTreeOfTString  colTest;
    colTest.pnodeAddNonTerminal(L"/", L"NT1");
    colTest.objAddTerminal(L"/", L"T1", TString(L"T1"));

    // Do some ops that can only be done on a terminal or non-terminal
    tCIDLib::TBoolean bCaughtIt = kCIDLib::False;
    try
    {
        colTest.pnodeAddNonTerminal(L"/T1", L"T2");
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_MustBeNonTerminal))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got wrong exception" << kCIDLib::EndLn;
        }
         else
        {
            bCaughtIt = kCIDLib::True;
        }
    }

    if (!bCaughtIt)
        strmOut << CUR_LN << L"Failed to catch exception" << kCIDLib::EndLn;


    // Try to add a duplicate
    bCaughtIt = kCIDLib::False;
    try
    {
        colTest.pnodeAddNonTerminal(L"/", L"NT1");
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_DupTreeNode))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got wrong exception" << kCIDLib::EndLn;
        }
         else
        {
            bCaughtIt = kCIDLib::True;
        }
    }

    if (!bCaughtIt)
        strmOut << CUR_LN << L"Failed to catch exception" << kCIDLib::EndLn;
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the Tree collections.
//
tCIDLib::TVoid TFacTestCIDLib::TestTreeCols()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        // Invoke the basic tests
        pszCurTest = L"basic";
        TestTreeBasic(strmOut());

        pszCurTest = L"basic2";
        TestTreeBasic2(strmOut());

        pszCurTest = L"case";
        TestTreeCase(strmOut());

        // Do some negative tests
        pszCurTest = L"negative";
        TestTreeNegative(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" tree collection test" << kCIDLib::EndLn;
        throw;
    }
}

