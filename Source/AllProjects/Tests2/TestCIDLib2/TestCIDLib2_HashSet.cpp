//
// FILE NAME: TestCIDLib2_HashSet.cpp
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
//  This file contains tests related to the by value hash set collection.
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
RTTIDecls(TTest_HashSetMove, TTestFWTest)
RTTIDecls(TTest_HashSetPlace, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_HashSetMove
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_HashSetMove: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_HashSetMove::TTest_HashSetMove() :

    TTestFWTest
    (
        L"Hashset Move", L"Hash set move support", 3
    )
{
}

TTest_HashSetMove::~TTest_HashSetMove()
{
}


// ---------------------------------------------------------------------------
//  TTest_HashSetMove: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_HashSetMove::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    tCIDLib::TStrHashSet colTest(17, TStringKeyOps(kCIDLib::True));
    colTest.objAdd(L"Value 1");
    colTest.objAdd(L"Value 2");
    colTest.objAdd(L"Value 3");

    tCIDLib::TStrHashSet colTest2(tCIDLib::ForceMove(colTest));

    if (colTest.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Move ctor did not clear out source collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest2.c4ElemCount() != 3)
    {
        strmOut << TFWCurLn << L"Move ctor did not move elements to target\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest2.c4HashModulus() != 17)
    {
        strmOut << TFWCurLn << L"Move ctor did not move the hash modulus\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And make sure we can find them (which means the modulus and keyops were moved
    if (!colTest2.bHasElement(L"Value 1")
    ||  !colTest2.bHasElement(L"Value 2")
    ||  !colTest2.bHasElement(L"Value 3"))
    {
        strmOut << TFWCurLn << L"Could not find elements after move ctor\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    //
    //  Do an assign, this should also change the modulus and keyops to the source's
    //  so set them both different from the source.
    //
    tCIDLib::TStrHashSet colTest3(7, TStringKeyOps(kCIDLib::False));
    colTest3 = tCIDLib::ForceMove(colTest2);
    if (colTest2.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Move operator did not clear out source collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest3.c4ElemCount() != 3)
    {
        strmOut << TFWCurLn << L"Move operator did not move elements to target\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest3.c4HashModulus() != 17)
    {
        strmOut << TFWCurLn << L"Move operator did not move the hash modulus\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And if the modulus and key-ops got moved we should find the elements
    if (!colTest3.bHasElement(L"Value 1")
    ||  !colTest3.bHasElement(L"Value 2")
    ||  !colTest3.bHasElement(L"Value 3"))
    {
        strmOut << TFWCurLn << L"Could not find elements after move\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_HashSetPlace
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_HashSetPlace: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_HashSetPlace::TTest_HashSetPlace() :

    TTestFWTest
    (
        L"Hashset Placement", L"Hash set element placement tests", 3
    )
{
}

TTest_HashSetPlace::~TTest_HashSetPlace()
{
}


// ---------------------------------------------------------------------------
//  TTest_HashSetPlace: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_HashSetPlace::eRunTest(TTextStringOutStream&  strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Place some strings directly and test that we get back what we expect
    tCIDLib::TStrHashSet colTest(17, TStringKeyOps());

    const TString& strRet1 = colTest.objPlace(L"Value 1");
    const TString& strRet2 = colTest.objPlace(L"Value 2");
    const TString& strRet3 = colTest.objPlace(L"Value 3");

    if (!colTest.bHasElement(L"Value 1")
    ||  !colTest.bHasElement(L"Value 2")
    ||  !colTest.bHasElement(L"Value 3"))
    {
        strmOut << TFWCurLn << L"In place has set elements are wrong\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    if ((strRet1 != L"Value 1")
    ||  (strRet2 != L"Value 2")
    ||  (strRet3 != L"Value 3"))
    {
        strmOut << TFWCurLn << L"In place hash set returns are wrong\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}

