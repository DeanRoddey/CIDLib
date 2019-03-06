//
// FILE NAME: TestCrypto_RSA.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/14/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file tests the secure cannel class.
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
#include    "TestCrypto.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_SChan1,TTestFWTestTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_SChan1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SChan1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SChan1::TTest_SChan1() :

    TTestFWTest(L"SChan 1", L"Tests the secure channel classes", 4)
{
}

TTest_SChan1::~TTest_SChan1()
{
}


// ---------------------------------------------------------------------------
//  TTest_SChan1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_SChan1::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    return eRes;
}

