//
// FILE NAME: TestCIDLib2_MemBuf.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/02/2018
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
//  This file contains tests related to the memory buffer classes.
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
RTTIDecls(TTest_MemBufMove,TTestFWTest)
RTTIDecls(TTest_MemBufExp,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_MemBufMove
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_MemBufMove: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_MemBufMove::TTest_MemBufMove() :

    TTestFWTest
    (
        L"Memory Buffer Move", L"Memory buffer move semantics", 2
    )
{
}

TTest_MemBufMove::~TTest_MemBufMove()
{
}


// ---------------------------------------------------------------------------
//  TTest_Base641: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_MemBufMove::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test move semantics stuff
    THeapBuf mbufOld(64, 128);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 64; c4Index++)
        mbufOld[c4Index] = tCIDLib::TCard1(c4Index);
    THeapBuf mbufCopy(tCIDLib::ForceMove(mbufOld));

    // The new guy should have the data now
    if ((mbufCopy.c4Size() != 64)
    ||  (mbufCopy.c4MaxSize() != 128)
    ||  (mbufCopy[16] != 16)
    ||  (mbufCopy[54] != 54))
    {
        strmOut << TFWCurLn << L"Heap buffer move ctor failed on new buffer\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    //
    //  And the old one shouldn't have this info. Neither 64 nor 128 are the values
    //  set by the default ctor, whose contents would have been swapped into the old
    //  one.
    //
    if ((mbufOld.c4Size() == 64) ||  (mbufOld.c4MaxSize() == 128))
    {
        strmOut << TFWCurLn << L"Heap buffer move ctor failed on old buffer\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Now test the assignment
    THeapBuf mbufAssign = tCIDLib::ForceMove(mbufCopy);
    if ((mbufAssign.c4Size() != 64)
    ||  (mbufAssign.c4MaxSize() != 128)
    ||  (mbufAssign[16] != 16)
    ||  (mbufAssign[54] != 54))
    {
        strmOut << TFWCurLn << L"Heap buffer move operator failed on new buffer\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((mbufCopy.c4Size() == 64) ||  (mbufCopy.c4MaxSize() == 128))
    {
        strmOut << TFWCurLn << L"Heap buffer move operator failed on old buffer\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_MemBufExp
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_MemBufExp: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_MemBufExp::TTest_MemBufExp() :

    TTestFWTest
    (
        L"Memory Buffer Exp", L"Buffer expansion/contraction tests", 2
    )
{
}

TTest_MemBufExp::~TTest_MemBufExp()
{
}


// ---------------------------------------------------------------------------
//  TTest_Base641: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_MemBufExp::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Test the MakeSpace method, which moves up trailing bytes to make room
    //  in the middle of a buffer.
    //
    {
        //
        //  Create a buffer with the values
        //
        //  0, 1, 2, 3, 4, 10, 11, 12, 13, 14
        //
        THeapBuf mbufTest(10, 15);
        for (tCIDLib::TCard1 c1Val = 0; c1Val < 5; c1Val++)
            mbufTest[c1Val] = c1Val;
        for (tCIDLib::TCard1 c1Val = 5; c1Val < 10; c1Val++)
            mbufTest[c1Val] = c1Val + 5;

        //
        //  Make 5 bytes available at offset 5 and insert 5 through 9. Ths should
        //  give us a contiguously numbered buffer.
        //
        mbufTest.MakeSpace(5, 5, 10);
        for (tCIDLib::TCard1 c1Val = 5; c1Val < 10; c1Val++)
            mbufTest[c1Val] = c1Val;

        for (tCIDLib::TCard1 c1Val = 0; c1Val < 15; c1Val++)
        {
            if (mbufTest[c1Val] != c1Val)
            {
                strmOut << TFWCurLn << L"Mem buffer MakeSpace method failed\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }

        // If we tried to expand by another byte, it should fail
        tCIDLib::TBoolean bGotIt = kCIDLib::False;
        try
        {
            mbufTest.MakeSpace(5, 1, 15);
        }

        catch(...)
        {
            bGotIt = kCIDLib::True;
        }

        if (!bGotIt)
        {
                strmOut << TFWCurLn << L"Invalid MakeSpace did not cause exception\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    // Test the remove space method, which moves trailing bytes down
    {
        //
        //  Create a buffer with the values
        //
        //  0, 1, 2, 10, 11, 3, 4
        //
        THeapBuf mbufTest(7, 7);
        mbufTest[0] = 0;
        mbufTest[1] = 1;
        mbufTest[2] = 2;
        mbufTest[3] = 10;
        mbufTest[4] = 11;
        mbufTest[5] = 3;
        mbufTest[6] = 4;

        // Remove the 10/11 bytes to leave a contiguous buffer
        mbufTest.RemoveSpace(3, 2, 7);

        for (tCIDLib::TCard1 c1Val = 0; c1Val < 5; c1Val++)
        {
            if (mbufTest[c1Val] != c1Val)
            {
                strmOut << TFWCurLn << L"Mem buffer RemoveSpace method failed\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
                break;
            }
        }

        // Try to remove a byte too much
        tCIDLib::TBoolean bGotIt = kCIDLib::False;
        try
        {
            mbufTest.MakeSpace(4, 2, 5);
        }

        catch(...)
        {
            bGotIt = kCIDLib::True;
        }

        if (!bGotIt)
        {
            strmOut << TFWCurLn << L"Invalid RemoveSpace did not cause exception\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // Move the last two bytes to the start
        mbufTest.MoveToStart(3, 2);
        if ((mbufTest[0] != 3) && (mbufTest[1] != 4))
        {
            strmOut << TFWCurLn << L"MoveToStart failed\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // This should do nothing
        mbufTest.MoveToStart(2, 0);
        if ((mbufTest[0] != 3) && (mbufTest[1] != 4))
        {
            strmOut << TFWCurLn << L"MoveToStart should not have done anything\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // And this should do nothing
        mbufTest.RemoveSpace(2, 0, 2);
        if ((mbufTest[0] != 3) && (mbufTest[1] != 4))
        {
            strmOut << TFWCurLn << L"RemoveSpace should not have done anything\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }
    return eRes;
}

