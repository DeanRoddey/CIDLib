//
// FILE NAME: TestCIDLib2_Types.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2019
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file contains the most basic tests, and the ones run first, mostly
//  to make sure that stuff set on a per-platform basis is not obviously incorrect.
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
RTTIDecls(TTest_Types,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Types
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Types: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Types::TTest_Types() :

    TTestFWTest
    (
        L"Test Types", L"Validate per-platform type definitions", 1
    )
{
}

TTest_Types::~TTest_Types()
{
}


// ---------------------------------------------------------------------------
//  TTest_Types: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Types::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    if ((sizeof(tCIDLib::TCard1)    != 1)
    ||  (sizeof(tCIDLib::TCard2)    != 2)
    ||  (sizeof(tCIDLib::TCard4)    != 4)
    ||  (sizeof(tCIDLib::TCard8)    != 8)
    ||  (sizeof(tCIDLib::TInt1)     != 1)
    ||  (sizeof(tCIDLib::TInt2)     != 2)
    ||  (sizeof(tCIDLib::TInt4)     != 4)
    ||  (sizeof(tCIDLib::TInt8)     != 8)
    ||  (sizeof(tCIDLib::TFloat4)   != 4)
    ||  (sizeof(tCIDLib::TFloat8)   != 8)
    ||  (sizeof(tCIDLib::TCh)       != kCIDLib::c4CharBytes)
    ||  (sizeof(tCIDLib::TSCh)      != kCIDLib::c4SCharBytes))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"One or more per-platform types was incorrectly defined\n\n";
    }

    return eRes;
}

