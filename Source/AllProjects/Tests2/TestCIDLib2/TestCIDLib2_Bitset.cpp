//
// FILE NAME: TestCIDLib2_Bitset.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This file contains tests related to the bit set family of classes, which allow
//  you to have a set of bits that you can index like an array and manipulate in
//  various ways.
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
RTTIDecls(TTest_Bitset1,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Bitset1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Bitset11: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Bitset1::TTest_Bitset1() :

    TTestFWTest
    (
        L"Bit Set Basic", L"Basic tests of the bit set classes", 3
    )
{
}

TTest_Bitset1::~TTest_Bitset1()
{
}


// ---------------------------------------------------------------------------
//  TTest_Bitset1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Bitset1::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;


    // Make it an odd number (not a multiple of 8) to test any trailing bits problems
    tCIDLib::TBoolean bGotIt;
    TBitset btsTest(63);

    // Make sure we get the right bit count back
    if (btsTest.c4BitCount() != 63)
    {
        strmOut << TFWCurLn << L"Ctor did not set the correct bit count\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // All the bits should be off
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 63; c4Index++)
    {
        if (btsTest[c4Index])
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Bits set after non-initializing ctor" << kCIDLib::DNewLn;

            break;
        }
    }


    // Obviously two constructed ones with the same size must be equal
    {
        TBitset btsComp(63);
        if (btsComp != btsTest)
        {
            strmOut << TFWCurLn << L"Default constructed sets not equal\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // Obviously one of a different bit size shouldn't be
    {
        TBitset btsComp(62);
        if (btsComp == btsTest)
        {
            strmOut << TFWCurLn << L"Sets of different shows up equal\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // Check bad index via state getter
    bGotIt = kCIDLib::False;
    try
    {
        btsTest.bBitState(63);
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch bad state getter index" << kCIDLib::DNewLn;
    }

    // Test it via index operator
    bGotIt = kCIDLib::False;
    try
    {
        tCIDLib::TBoolean bTest = btsTest[63];
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }


    if (!bGotIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch bad operator index" << kCIDLib::DNewLn;
    }

    // Test it via bit state setter
    bGotIt = kCIDLib::False;
    try
    {
        btsTest.bSetBitState(63, kCIDLib::False);
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch bad state setter index" << kCIDLib::DNewLn;
    }


    // Round trip some bits
    btsTest.bSetBitState(7, kCIDLib::True);
    if (!btsTest[7])
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Set bit did not come back set" << kCIDLib::DNewLn;
    }

    btsTest.bSetBitState(62, kCIDLib::True);
    if (!btsTest[62])
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Set bit did not come back set" << kCIDLib::DNewLn;
    }

    //
    //  Test that we don't get bad trailing bits into the set if we set from an array
    //  that has those trailing bits on.
    //
    {
        tCIDLib::TCard1 ac1Src[8];
        TRawMem::SetMemBuf(ac1Src, tCIDLib::TCard1(0xFF), 8);

        // Note here we are indicating how many bits in the set
        TBitset btsAllBits(63, ac1Src);

        //
        //  Mask off the last byte ourself and set our other one from that. The mask
        //  will be 0x7F since we are leaving 7 unused bits at the end.
        //
        //  Note that here we are indicating how many bytes in the sourc array.
        //
        ac1Src[7] &= 0x7F;
        btsTest.FromRawArray(ac1Src, 8);

        // And now they should compare equal
        if (btsTest != btsAllBits)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Trailing bits removal test failed" << kCIDLib::DNewLn;
        }
    }

    // And now all bits should be set
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 63; c4Index++)
    {
        if (!btsTest[c4Index])
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Not all bits set after set from raw array"
                    << kCIDLib::DNewLn;

            break;
        }
    }


    // Change the set size
    btsTest.ChangeBitCount(16);
    if (btsTest.c4BitCount() != 16)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Changing bit count didn't work" << kCIDLib::DNewLn;
    }

    bGotIt = kCIDLib::False;
    try
    {
        btsTest.bSetBitState(16, kCIDLib::False);
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch index" << kCIDLib::DNewLn;
    }


    // Test the move constructor
    {
        TBitset btsMoved(tCIDLib::ForceMove(TBitset(43)));
    }


    return tTestFWLib::ETestRes::Success;
}

