//
// FILE NAME: TestKernel_RawMemory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/12/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module tests the raw memory manipulation APIs.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestKernel.hpp"
#include    <memory.h>



// ----------------------------------------------------------------------------
//  Local static functions
// ----------------------------------------------------------------------------

//
//  Test some of the fundamental raw bits stuff, which might be used by anything
//  down the line.
//
static tCIDLib::TVoid TestSwaps()
{
    if (TRawBits::c2SwapBytes(0x71A4) != 0xA471)
        strmOut << CUR_LN << L"Card2 byte swap failed\n";

    if (TRawBits::c4SwapBytes(0xDEADBEEF) != 0xEFBEADDE)
        strmOut << CUR_LN << L"Card4 byte swap failed\n";

    if (TRawBits::c8SwapBytes(0x0102030405060708) != 0x0807060504030201)
        strmOut << CUR_LN << L"Card8 byte swap failed\n";

    if (TRawBits::i2SwapBytes(0x71A4) != tCIDLib::TInt2(0xA471))
        strmOut << CUR_LN << L"Card2 byte swap failed\n";

    if (TRawBits::i4SwapBytes(0xDEADBEEF) != tCIDLib::TInt4(0xEFBEADDE))
        strmOut << CUR_LN << L"Card4 byte swap failed\n";

    if (TRawBits::i8SwapBytes(0x0102030405060708) != 0x0807060504030201)
        strmOut << CUR_LN << L"Card8 byte swap failed\n";


    // Test array level swaps
    tCIDLib::TCard4 ac4List[4] = { 0x01020304, 0x02030405, 0x03040506, 0x04050607 };
    TRawBits::SwapCard4Array(ac4List, 4);
    if ((ac4List[0] != 0x04030201)
    ||  (ac4List[1] != 0x05040302)
    ||  (ac4List[2] != 0x06050403)
    ||  (ac4List[3] != 0x07060504))
    {
        strmOut << CUR_LN << L"Card4 byte array swap failed\n";
    }

    tCIDLib::TInt4 ai4List[4] = { 0x01020304, 0x02030405, 0x03040506, 0x04050607 };
    TRawBits::SwapInt4Array(ai4List, 4);
    if ((ai4List[0] != 0x04030201)
    ||  (ai4List[1] != 0x05040302)
    ||  (ai4List[2] != 0x06050403)
    ||  (ai4List[3] != 0x07060504))
    {
        strmOut << CUR_LN << L"Int4 byte array swap failed\n";
    }
}


static tCIDLib::TVoid TestFills()
{
    //
    //  Create a memory buffer and fill it using each fill method and test
    //  its results. Allocate it so that any overruns are caught by the
    //  heap checking tools.
    //
    const tCIDLib::TCard4   c4Size = 512;
    tCIDLib::TCard4         c4Index;
    {
        tCIDLib::TCard1* pc1Test = new tCIDLib::TCard1[c4Size];

        // Set by raw RTL to all zeros, then set to test pattern
        memset(pc1Test, 0, c4Size);
        TRawMem::SetMemBuf(pc1Test, tCIDLib::TCard1(0xAC), c4Size);

        // Test it for the pattern
        for (c4Index = 0; c4Index < c4Size; c4Index++)
        {
            if (pc1Test[c4Index] != 0xAC)
            {
                strmOut << CUR_LN << L"TCard1 buffer fill failed\n";
                break;
            }
        }
        delete [] pc1Test;
    }

    {
        tCIDLib::TCard2* pc2Test = new tCIDLib::TCard2[c4Size];

        // Set by raw RTL to all zeros, then set to test pattern
        memset(pc2Test, 0, c4Size * 2);
        TRawMem::SetMemBuf(pc2Test, tCIDLib::TCard2(0xFACE), c4Size);

        // Test it for the pattern
        for (c4Index = 0; c4Index < c4Size; c4Index++)
        {
            if (pc2Test[c4Index] != 0xFACE)
            {
                strmOut << CUR_LN << L"TCard2 buffer fill failed\n";
                break;
            }
        }

        //
        //  Set by raw RTL to all zeros, then set using test pattern with
        //  same upper and lower byte. This will use an optimized fill.
        //
        memset(pc2Test, 0, c4Size * 2);
        TRawMem::SetMemBuf(pc2Test, tCIDLib::TCard2(0xCACA), c4Size);

        // Test it for the pattern
        for (c4Index = 0; c4Index < c4Size; c4Index++)
        {
            if (pc2Test[c4Index] != 0xCACA)
            {
                strmOut << CUR_LN << L"TCard2 buffer fill failed\n";
                break;
            }
        }
        delete [] pc2Test;
    }

    {
        tCIDLib::TCard4* pc4Test = new tCIDLib::TCard4[c4Size];

        // Set by raw RTL to all zeros, then set to test pattern
        memset(pc4Test, 0, c4Size * 4);
        TRawMem::SetMemBuf(pc4Test, tCIDLib::TCard4(0x90ABCDEF), c4Size);

        // Test it for the pattern
        for (c4Index = 0; c4Index < c4Size; c4Index++)
        {
            if (pc4Test[c4Index] != 0x90ABCDEF)
            {
                strmOut << CUR_LN << L"TCard4 buffer fill failed\n";
                break;
            }
        }
        delete [] pc4Test;
    }
}

static tCIDLib::TVoid TestHash()
{
    // This one is just for use in the debugger mainly
    const tCIDLib::TCard4 c4Size = 16;
    const tCIDLib::TCard4 c4Modulus = 23;

    tCIDLib::TCard1 c1Test[c4Size];
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Size; c4Index++)
        c1Test[c4Index] = tCIDLib::TCard1(c4Index)+4;

    tCIDLib::THashVal hshRet = TRawMem::hshHashBuffer(c1Test, c4Modulus, c4Size);
    if (hshRet > c4Modulus)
    {
        strmOut << CUR_LN << L"Hash was larger than modulus\n";
        return;
    }
}


static tCIDLib::TVoid TestAtomics()
{
    //
    //  This method does not attempt to beat on these methods from multiple
    //  threads to insure that they are really atomic. It just checks that
    //  they move the data around as they are supposed to.
    //

    // Test the basic exchange method
    CID_CACHEALIGN tCIDLib::TCard4 c4Val1 = 10;
    CID_CACHEALIGN tCIDLib::TCard4 c4Val2 = 20;
    CID_CACHEALIGN tCIDLib::TCard4 c4Val3 = 30;

    // This should return the old first value of 10
    if (TRawMem::c4Exchange(c4Val1, c4Val2) != 10)
        strmOut << CUR_LN << L"c4Exchange did not return old value\n";

    // The exchange should have put 20 into the first value
    if (c4Val1 != 20)
        strmOut << CUR_LN << L"c4Exchange did not put new value in\n";

    // Now test the compare and exchange
    c4Val1 = 10;
    c4Val2 = 20;

    //
    //  In this case it should return the old value and do nothing, since
    //  the compare value is not equal to the first value.
    //
    if (TRawMem::c4CompareAndExchange(c4Val1, c4Val2, c4Val3) != 10)
        strmOut << CUR_LN << L"c4CompareAndExchange returned wrong value\n";

    // It should not have changed the first value either
    if (c4Val1 != 10)
        strmOut << CUR_LN << L"c4CompareAndExchange modified first value\n";

    // Now set compare value to first value and do it again
    c4Val1 = c4Val3;

    if (TRawMem::c4CompareAndExchange(c4Val1, c4Val2, c4Val3) != 30)
        strmOut << CUR_LN << L"c4CompareAndExchange returned wrong value\n";

    // It should have changed the first value also
    if (c4Val1 != 20)
        strmOut << CUR_LN << L"c4CompareAndExchange did not update first value\n";


    //
    //  Now test the template versions, which deal with pointers to things.
    //  We just set up pointers to our existing values.
    //
    c4Val1 = 10;
    c4Val2 = 20;
    c4Val3 = 30;

    CID_CACHEALIGN tCIDLib::TCard4* pc4Val1 = &c4Val1;
    CID_CACHEALIGN tCIDLib::TCard4* pc4Val2 = &c4Val2;
    CID_CACHEALIGN tCIDLib::TCard4* pc4Val3 = &c4Val3;

    if (TRawMem::pExchangePtr<tCIDLib::TCard4>(&pc4Val1, pc4Val2) != &c4Val1)
        strmOut << CUR_LN << L"pExchangePtr did not return old value\n";

    if (pc4Val1 != pc4Val2)
        strmOut << CUR_LN << L"pExchangePtr did not put new value in\n";

    // And now test the compare and exchange with unequal values
    pc4Val1 = &c4Val1;
    pc4Val2 = &c4Val2;
    pc4Val3 = &c4Val3;

    if (TRawMem::pCompareAndExchangePtr<tCIDLib::TCard4>(&pc4Val1, pc4Val2, pc4Val3) != &c4Val1)
        strmOut << CUR_LN << L"pCompareAndExchangePtr returned wrong value\n";

    // It should not have updated the first value either
    if (pc4Val1 != &c4Val1)
        strmOut << CUR_LN << L"pCompareAndExchangePtr should not have changed first value\n";

    // Set the first and compare values equal and try again
    pc4Val1 = &c4Val1;
    pc4Val2 = &c4Val2;
    pc4Val3 = &c4Val1;

    if (TRawMem::pCompareAndExchangePtr<tCIDLib::TCard4>(&pc4Val1, pc4Val2, pc4Val3) != &c4Val1)
        strmOut << CUR_LN << L"pCompareAndExchangePtr returned wrong value\n";

    // It should have updated the first value also
    if (pc4Val1 != pc4Val2)
        strmOut << CUR_LN << L"pCompareAndExchangePtr did not change first value\n";
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------

tCIDLib::TVoid TestRawMemory(const tCIDLib::TCard4)
{
    TestSwaps();
    TestFills();
    TestHash();
    TestAtomics();
}

