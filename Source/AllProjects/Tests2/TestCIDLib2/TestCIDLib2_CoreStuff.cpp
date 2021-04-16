//
// FILE NAME: TestCIDLib2_CoreStuff.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/23/2019
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
//  This file contains tests for some fundamental stuff that isn't worth having
//  their own files, like TRawBits, and some of the fundamental templated stuff
//  in tCIDLib and such.
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
RTTIDecls(TTest_RawBits,TTestFWTest)


// ---------------------------------------------------------------------------
//  CLASS: TTest_RawBits
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_RawBits: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_RawBits::TTest_RawBits() :

    TTestFWTest
    (
        L"RawBits", L"Tests the TRawBits namespace", 1
    )
{
}

TTest_RawBits::~TTest_RawBits()
{
}


// ---------------------------------------------------------------------------
//  TTest_RawBits: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_RawBits::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Test the rotation stuff. We just test the pathlogical scenarios plus a
    //  couple more, since if those work we are good.
    //
    if ((TRawBits::c1RotateLeft(0x80, 1) != 0x1)
    ||  (TRawBits::c1RotateLeft(0x81, 1) != 0x3)
    ||  (TRawBits::c1RotateLeft(0xFF, 1) != 0xFF)
    ||  (TRawBits::c1RotateLeft(0xFF, 4) != 0xFF)
    ||  (TRawBits::c1RotateLeft(0xDE, 8) != 0xDE)
    ||  (TRawBits::c1RotateLeft(0xFA, 0) != 0xFA))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"8 bit rotate left failed\n\n";
    }

    if ((TRawBits::c1RotateRight(0x1, 1) != 0x80)
    ||  (TRawBits::c1RotateRight(0x81, 1) != 0xC0)
    ||  (TRawBits::c1RotateRight(0xFF, 1) != 0xFF)
    ||  (TRawBits::c1RotateRight(0xFF, 7) != 0xFF)
    ||  (TRawBits::c1RotateRight(0xCF, 8) != 0xCF)
    ||  (TRawBits::c1RotateRight(0xAF, 0) != 0xAF))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"8 bit rotate right failed\n\n";
    }


    if ((TRawBits::c2RotateLeft(0x8000, 1) != 0x1)
    ||  (TRawBits::c2RotateLeft(0x8001, 1) != 0x3)
    ||  (TRawBits::c2RotateLeft(0xFFFF, 1) != 0xFFFF)
    ||  (TRawBits::c2RotateLeft(0xFFFF, 12) != 0xFFFF)
    ||  (TRawBits::c2RotateLeft(0xDEAD, 16) != 0xDEAD)
    ||  (TRawBits::c2RotateLeft(0xBEAD, 0) != 0xBEAD))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"16 bit rotate left failed\n\n";
    }

    if ((TRawBits::c2RotateRight(0x1, 1) != 0x8000)
    ||  (TRawBits::c2RotateRight(0x8001, 1) != 0xC000)
    ||  (TRawBits::c2RotateRight(0xFFFF, 1) != 0xFFFF)
    ||  (TRawBits::c2RotateRight(0xFFFF, 11) != 0xFFFF)
    ||  (TRawBits::c2RotateRight(0x10CF, 16) != 0x10CF)
    ||  (TRawBits::c2RotateRight(0x33AF, 0) != 0x33AF))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"16 bit rotate right failed\n\n";
    }


    if ((TRawBits::c4RotateLeft(0x80000000, 1) != 0x1)
    ||  (TRawBits::c4RotateLeft(0x80000001, 1) != 0x3)
    ||  (TRawBits::c4RotateLeft(0xFFFFFFFF, 1) != 0xFFFFFFFF)
    ||  (TRawBits::c4RotateLeft(0xFFFFFFFF, 5) != 0xFFFFFFFF)
    ||  (TRawBits::c4RotateLeft(0xFFFFFFFF, 19) != 0xFFFFFFFF)
    ||  (TRawBits::c4RotateLeft(0xA10DE, 32) != 0xA10DE)
    ||  (TRawBits::c4RotateLeft(0xE20FE, 0) != 0xE20FE))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"32 bit rotate left failed\n\n";
    }

    if ((TRawBits::c4RotateRight(0x1, 1) != 0x80000000)
    ||  (TRawBits::c4RotateRight(0x80000001, 1) != 0xC0000000)
    ||  (TRawBits::c4RotateRight(0xFFFFFFFF, 1) != 0xFFFFFFFF)
    ||  (TRawBits::c4RotateRight(0xFFFFFFFF, 7) != 0xFFFFFFFF)
    ||  (TRawBits::c4RotateRight(0xFFFFFFFF, 28) != 0xFFFFFFFF)
    ||  (TRawBits::c4RotateRight(0x0133ACF, 32) != 0x0133ACF)
    ||  (TRawBits::c4RotateRight(0xA19FF2A, 0) != 0xA19FF2A))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"32 bit rotate right failed\n\n";
    }

    if ((TRawBits::c8RotateLeft(0x8000000000000000, 1) != 0x1)
    ||  (TRawBits::c8RotateLeft(0x8000000000000001, 1) != 0x3)
    ||  (TRawBits::c8RotateLeft(0x8000000000000001, 16) != 0x18000)
    ||  (TRawBits::c8RotateLeft(0xFFFFFFFFFFFFFFFF, 1) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateLeft(0xFFFFFFFFFFFFFFFF, 5) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateLeft(0xFFFFFFFFFFFFFFFF, 19) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateLeft(0xFFFFFFFFFFFFFFFF, 48) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateLeft(0x2044A10DE, 64) != 0x2044A10DE)
    ||  (TRawBits::c8RotateLeft(0xE20FE, 0) != 0xE20FE))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"64 bit rotate left failed\n\n";
    }

    if ((TRawBits::c8RotateRight(0x1, 1) != 0x8000000000000000)
    ||  (TRawBits::c8RotateRight(0x8000000000000001, 1) != 0xC000000000000000)
    ||  (TRawBits::c8RotateRight(0xFFFFFFFFFFFFFFFF, 1) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateRight(0xFFFFFFFFFFFFFFFF, 7) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateRight(0xFFFFFFFFFFFFFFFF, 28) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateRight(0xFFFFFFFFFFFFFFFF, 56) != 0xFFFFFFFFFFFFFFFF)
    ||  (TRawBits::c8RotateRight(0x0133ACF, 64) != 0x0133ACF)
    ||  (TRawBits::c8RotateRight(0x0133ACF, 8) != 0xCF0000000000133A)
    ||  (TRawBits::c8RotateRight(0xA19FF2A, 0) != 0xA19FF2A))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"64 bit rotate right failed\n\n";
    }
    return eRes;
}

