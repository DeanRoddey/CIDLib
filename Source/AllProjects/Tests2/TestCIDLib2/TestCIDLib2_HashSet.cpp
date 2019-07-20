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
//  This file contains tests related to the by value has set collection.
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
RTTIDecls(TTest_HashSetPlace, TTestFWTest)


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

