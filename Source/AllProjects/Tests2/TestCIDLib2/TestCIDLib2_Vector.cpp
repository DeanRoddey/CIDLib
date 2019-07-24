//
// FILE NAME: TestCIDLib2_Vector.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2007
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
//  This file contains tests related to the by value vector collection.
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
#include    "TestCIDLib2.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_VectorBasic, TTestFWTest)
RTTIDecls(TTest_VectorPlace, TTestFWTest)
RTTIDecls(TTest_VectorLambda, TTestFWTest)
RTTIDecls(TTest_VectorMoveSem, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_VectorBasic: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_VectorBasic::TTest_VectorBasic() :

    TTestFWTest
    (
        L"Vector Basic", L"Vector basic tests", 3
    )
{
}

TTest_VectorBasic::~TTest_VectorBasic()
{
}


// ---------------------------------------------------------------------------
//  TTest_VectorBasic: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_VectorBasic::eRunTest(TTextStringOutStream&  strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    tCIDLib::TKVPList colTest(8UL);
    if (colTest.c4ElemCount())
    {
        strmOut << TFWCurLn << L"Non-zero element count after ctor\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    colTest.objAdd(TKeyValuePair(L"Key 1", L"Value 1"));
    colTest.objPlace(L"Key 2", L"Value 2");
    colTest.objPlace(L"Key 3", L"Value 3");

    if (colTest.c4ElemCount() != 3)
    {
        strmOut << TFWCurLn << L"Wrong element count after additions\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (!colTest.bRemoveIfMember(TKeyValuePair(L"Key 1", L"Value 1")))
    {
        strmOut << TFWCurLn << L"Failed to remove member element\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest.c4ElemCount() != 2)
    {
        strmOut << TFWCurLn << L"Wrong element count after removal\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Add that one back if not new, which should add it
    if (!colTest.bAddIfNew(TKeyValuePair(L"Key 1", L"Value 1")))
    {
        strmOut << TFWCurLn << L"Could not add if new on removed element\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest.c4ElemCount() != 3)
    {
        strmOut << TFWCurLn << L"Wrong element count after addif new\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

   // And it should be rejected this time
    if (colTest.bAddIfNew(TKeyValuePair(L"Key 1", L"Value 1")))
    {
        strmOut << TFWCurLn << L"Add if new added an element already in the list\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Remove all elements and we should have zero elements
    colTest.RemoveAll();
    if (colTest.c4ElemCount())
    {
        strmOut << TFWCurLn << L"Non-zero element count after RemoveAll\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Make sure we catch a bad index access
    try
    {
        colTest[0].strValue(L"Bad");
        strmOut << TFWCurLn << L"Failed to catch bad index 0 access\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }
    catch(...) {}

    // Add an element and test past the end again
    colTest.objPlace(L"Key A", L"Value A");
    try
    {
        colTest[1].strValue(L"Bad");
        strmOut << TFWCurLn << L"Failed to catch bad index 1 access\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }
    catch(...) {}

    // Load up some and test removal at various places
    TString strKey(L"Key ");
    TString strValue(L"Value ");
    colTest.RemoveAll();
    for (tCIDLib::TCard4 c4Index = 1; c4Index < 9; c4Index++)
    {
        strKey.CapAt(4);
        strValue.CapAt(6);
        strKey.AppendFormatted(c4Index);
        strValue.AppendFormatted(c4Index);
        colTest.objPlace(strKey, strValue);
    }

    // Removing past the end should fail
    try
    {
        colTest.RemoveAt(8);
        strmOut << TFWCurLn << L"Failed to catch remove past last element\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }
    catch(...) {}

    // Remove the last and first elements
    colTest.RemoveAt(7);
    colTest.RemoveAt(0);

    if ((colTest[0].strKey() != L"Key 2")
    ||  (colTest[5].strKey() != L"Key 7"))
    {
        strmOut << TFWCurLn << L"First/last element removal failed\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And delete the rest individually, which should leave us empty
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 6; c4Index++)
        colTest.RemoveAt(0);

    if (colTest.c4ElemCount())
    {
        strmOut << TFWCurLn << L"Non-zero element count after removing all elements\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Test inserting
    colTest.objPlace(L"Key 3", L"Value 3");
    colTest.InsertAt(TKeyValuePair(L"Key 2", L"Value 2"), 0);
    colTest.InsertAt(TKeyValuePair(L"Key 1", L"Value 1"), 0);

    if ((colTest[0].strKey() != L"Key 1")
    ||  (colTest[1].strKey() != L"Key 2")
    ||  (colTest[2].strKey() != L"Key 3"))
    {
        strmOut << TFWCurLn << L"Element insertion failed\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_VectorLambda: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_VectorLambda::TTest_VectorLambda() :

    TTestFWTest
    (
        L"Vector Lambda", L"Vector lambda callback tests", 3
    )
{
}

TTest_VectorLambda::~TTest_VectorLambda()
{
}


// ---------------------------------------------------------------------------
//  TTest_VectorLambda: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_VectorLambda::eRunTest(TTextStringOutStream&  strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a ref fector with some objects
    TVector<TString> colTest(16);
    TString strVal(32UL);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        colTest.objAdd(strVal);
    }

    // Iterate the values and check them
    tCIDLib::TCard4 c4LoopVal = 0;
    colTest.bForEach
    (
        [&c4LoopVal, &strVal, &eRes, &strmOut](const TString& strCur)
        {
            strVal = L"Value ";
            strVal.AppendFormatted(c4LoopVal++);
            if (strVal != strCur)
            {
                strmOut << TFWCurLn << L"ForEach callback got out of sequence value\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
            return kCIDLib::True;
        }
    );

    // Do a find test
    strVal = L"Value ";
    strVal.AppendFormatted(2UL);
    if (colTest.tFind([&strVal](const TString& strCur) { return strVal == strCur; }) != 2)
    {
        strmOut << TFWCurLn << L"tFind did not find the test value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorMoveSem
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_VectorMoveSem: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_VectorMoveSem::TTest_VectorMoveSem() :

    TTestFWTest
    (
        L"Vector Move Semantics", L"Vector move ctor and assignment", 3
    )
{
}

TTest_VectorMoveSem::~TTest_VectorMoveSem()
{
}


// ---------------------------------------------------------------------------
//  TTest_VectorMoveSem: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_VectorMoveSem::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a vector with some objects
    tCIDLib::TStrList colTest(16);
    TString strVal(32UL);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        colTest.objAdd(strVal);
    }

    //
    //  Force a move ctor. The new guy should have the elements and the original
    //  should have none.
    //
    tCIDLib::TStrList colMoveCtor(tCIDLib::ForceMove(colTest));
    if (colMoveCtor.c4ElemCount() != 16)
    {
        strmOut << TFWCurLn << L"Move ctor did not move elements to new collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And the original should be empty now
    if (colTest.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Move ctor did not empty source collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And make sure that we got the correct values over
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        if (colMoveCtor[c4Index] != strVal)
        {
            strmOut << TFWCurLn << L"Move ctor copy is out of sequence\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
            break;
        }
    }

    //
    //  Create a thread safe vector and do a forced assign. We should still be
    //  thread safe afterwards since only element content is moved.
    //
    tCIDLib::TStrList colSafe(tCIDLib::EMTStates::Safe);
    colSafe = tCIDLib::ForceMove(colMoveCtor);
    if (!colSafe.bIsMTSafe())
    {
        strmOut << TFWCurLn << L"Move operator overwrote thread safety\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // The move one should now be empty
    if (colMoveCtor.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Move operator did not empty source collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And make sure that we got the correct values over
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        if (colSafe[c4Index] != strVal)
        {
            strmOut << TFWCurLn << L"Move ctor copy is out of sequence\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
            break;
        }
    }

    // Move construct one from a thread safe src, which should also be thread safe
    tCIDLib::TStrList colNewSafe(tCIDLib::ForceMove(colSafe));
    if (colNewSafe.eMTState() != tCIDLib::EMTStates::Safe)
    {
        strmOut << TFWCurLn << L"Move ctor did not copy over thread safety\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorPlace
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_VectorPlace: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_VectorPlace::TTest_VectorPlace() :

    TTestFWTest
    (
        L"Vector Placement", L"Vector element placement tests", 3
    )
{
}

TTest_VectorPlace::~TTest_VectorPlace()
{
}


// ---------------------------------------------------------------------------
//  TTest_VectorPlace: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_VectorPlace::eRunTest(TTextStringOutStream&  strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Place some areas directly and test that we get back what we expect
    TVector<TArea> colTest(16);
    const TArea& areaRet1 = colTest.objPlace(1, 2, 3, 4);
    const TArea& areaRet2 = colTest.objPlace(TPoint::pntOrigin, TSize(4, 4));
    const TArea& areaRet3 = colTest.objPlace(TArea(4, 5, 6, 7));

    if ((colTest[0] != TArea(1, 2, 3, 4))
    ||  (colTest[1] != TArea(0, 0, 4, 4))
    ||  (colTest[2] != TArea(4, 5, 6, 7)))
    {
        strmOut << TFWCurLn << L"In place vector elements are wrong\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((areaRet1 != TArea(1, 2, 3, 4))
    ||  (areaRet2 != TArea(0, 0, 4, 4))
    ||  (areaRet3 != TArea(4, 5, 6, 7)))
    {
        strmOut << TFWCurLn << L"In place vector element returns are wrong\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}
