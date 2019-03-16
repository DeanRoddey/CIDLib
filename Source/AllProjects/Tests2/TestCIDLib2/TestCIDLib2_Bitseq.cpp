//
// FILE NAME: TestCIDLib2_Bitseq.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This file contains tests related to the bit sequence calsses
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
RTTIDecls(TTest_BitSeq1,TTestFWTest)
RTTIDecls(TTest_BitSeq2,TTestFWTest)
RTTIDecls(TTest_BitSeq3,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_BitSeq1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BitSeq11: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BitSeq1::TTest_BitSeq1() :

    TTestFWTest
    (
        L"Bit Sequence Basic", L"Basic tests of the TBitSeq class", 3
    )
{
}

TTest_BitSeq1::~TTest_BitSeq1()
{
}


// ---------------------------------------------------------------------------
//  TTest_BitSeq1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BitSeq1::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    TBitSeq bsqTmp;

    // Make sure the offsets are both zero after ctor
    if (bsqTmp.c4ReadOfs() || bsqTmp.c4WriteOfs())
    {
        strmOut << TFWCurLn << L"Initial read or write offset was not zero\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Write a one bit out and check the offets
    bsqTmp.AppendBits(1, 1);
    if (bsqTmp.c4ReadOfs())
    {
        strmOut << TFWCurLn << L"Read ofs was affected by a write\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (bsqTmp.c4WriteOfs() != 1)
    {
        strmOut << TFWCurLn << L"Write offset != 1 after write of 1 bit\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Make sure the bit was written correctly. Check it all three ways
    if (!bsqTmp[0] || !bsqTmp.bAt(0) || (bsqTmp.c1At(0) != 0x80))
    {
        strmOut << TFWCurLn << L"Wrote 1 bit but got zero bit in the buffer"
                << kCIDLib::EndLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Write a couple more and check again
    bsqTmp.AppendBits(3, 2);

    // Check the write offset
    if (bsqTmp.c4WriteOfs() != 3)
    {
        strmOut << TFWCurLn << L"Write offset != 3 after write of 2 more bits"
                << kCIDLib::EndLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Check the value as a byte
    if (bsqTmp.c1At(0) != 0xE0)
    {
        strmOut << TFWCurLn
                << L"0th byte should have a value of 0xE0 now, but it was "
                << TCardinal(bsqTmp.c1At(0), tCIDLib::ERadices::Hex)
                << kCIDLib::EndLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Check it as an extracted bit field
    if (bsqTmp.c4ExtractBits(3) != 7)
    {
        strmOut << TFWCurLn << L"First 3 bits should have a value of 7 now"
                << kCIDLib::EndLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Check the read offset, which should have moved up
    if (bsqTmp.c4ReadOfs() != 3)
    {
        strmOut << TFWCurLn << L"Read offset should be 3 now, but it is "
                << bsqTmp.c4ReadOfs() << kCIDLib::EndLn;
        return tTestFWLib::ETestRes::Failed;
    }


    // Append a zero bit and check again
    bsqTmp.AppendBits(0, 1);

    // Check the write offset
    if (bsqTmp.c4WriteOfs() != 4)
    {
        strmOut << TFWCurLn << L"Write offset != 4 after write of 1 more bit"
                << kCIDLib::EndLn;
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Check the value as a byte, which shouldn't have changed since we
    //  appended a zero bit.
    //
    if (bsqTmp.c1At(0) != 0xE0)
    {
        strmOut << TFWCurLn << L"0th byte should have a value of 0xE0 now"
                << kCIDLib::EndLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // Check the new bit, which is where our read offset should be now
    if (bsqTmp.c4ExtractBits(1) != 0)
    {
        strmOut << TFWCurLn << L"Didn't read newly written bit back out\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Check the read offset, which should have moved up by one
    if (bsqTmp.c4ReadOfs() != 4)
    {
        strmOut << TFWCurLn << L"Read offset should be 4 now\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Reset the read indices and read them as two 2 bit fields
    bsqTmp.ReadReset();
    if (bsqTmp.c4ReadOfs())
    {
        strmOut << TFWCurLn << L"Read offset was not reset\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (bsqTmp.c4ExtractBits(2) != 0x3)
    {
        strmOut << TFWCurLn << L"First two bits != 0x3\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (bsqTmp.c4ExtractBits(2) != 0x2)
    {
        strmOut << TFWCurLn << L"Second two bits != 0x2\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Reset for writing and check the indices again
    bsqTmp.WriteReset();
    if (bsqTmp.c4ReadOfs() || bsqTmp.c4WriteOfs())
    {
        strmOut << TFWCurLn << L"Write reset didn't reset indices\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Do a copy and make sure they are equal
    TBitSeq bsqTmp2(bsqTmp);

    if (bsqTmp2 != bsqTmp)
    {
        strmOut << TFWCurLn << L"Copy ctor created unequal objects\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // And do an assignment and compare to the original also
    TBitSeq bsqTmp3;
    bsqTmp3 = bsqTmp2;

    if (bsqTmp3 != bsqTmp)
    {
        strmOut << TFWCurLn << L"Assignment created unequal objects\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_BitSeq2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BitSeq21: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BitSeq2::TTest_BitSeq2() :

    TTestFWTest
    (
        L"Bit Sequence Adv 1", L"Advanced tests of the TBitSeq class", 3
    )
{
}

TTest_BitSeq2::~TTest_BitSeq2()
{
}


// ---------------------------------------------------------------------------
//  TTest_BitSeq2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BitSeq2::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    TBitSeq bsqTmp;

    //
    // Write out a few different sized chunks from the following sequence
    //
    //  1101 0011-0101 1010-1111 0000-0110 1001-01
    //  D3        5A        F0        69
    //
    bsqTmp.AppendBits(0x0D, 4);
    bsqTmp.AppendBits(0x06, 5);
    bsqTmp.AppendBits(0x05, 3);
    bsqTmp.AppendBits(0x0A, 4);
    bsqTmp.AppendBits(0xF0, 8);
    bsqTmp.AppendBits(0x01, 2);
    bsqTmp.AppendBits(0x29, 6);
    bsqTmp.AppendBits(0x01, 2);

    if ((bsqTmp.c1At(0) != 0xD3)
    ||  (bsqTmp.c1At(1) != 0x5A)
    ||  (bsqTmp.c1At(2) != 0xF0)
    ||  (bsqTmp.c1At(3) != 0x69)
    ||  (bsqTmp.c1At(4) != 0x40))
    {
        strmOut << TFWCurLn << L"Byte sequence was not correctly written\n\n";
        return tTestFWLib::ETestRes::Failed;;
    }

    if (bsqTmp.c4WriteOfs() != 34)
    {
        strmOut << TFWCurLn << L"Write offset should have been 34 but was "
                << bsqTmp.c4WriteOfs() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // And read them out in different sized chunks
    if ((bsqTmp.c4ExtractBits(5) != 0x1A)
    ||  (bsqTmp.c4ExtractBits(7) != 0x35)
    ||  (bsqTmp.c4ExtractBits(12) != 0xAF0)
    ||  (bsqTmp.c4ExtractBits(3) != 0x3)
    ||  (bsqTmp.c4ExtractBits(5) != 0x9)
    ||  (bsqTmp.c4ExtractBits(2) != 0x1))
    {
        strmOut << TFWCurLn << L"Bit read of sequence failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (bsqTmp.c4ReadOfs() != 34)
    {
        strmOut << TFWCurLn << L"Read offset should have been 34 but was "
                << bsqTmp.c4ReadOfs() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Make sure that we get an exception if we try to read another bit,
    //  which would put us over the end of the currently written bits.
    //
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        bsqTmp.c4ExtractBits(1);
    }

    catch(const TError& errToCatch)
    {
        // Make sure its the expected error
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcBitSeq_NoMoreBits))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            strmOut << TFWCurLn << L"Got the wrong exception\n\n";
        }
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Failed to catch read past end of valid data\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Make sure we catch any attempt to reset the read offset too far
    bGotIt = kCIDLib::False;
    try
    {
        bsqTmp.ReadReset(34);
    }

    catch(const TError& errToCatch)
    {
        // Make sure its the expected error
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcBitSeq_BadReadReset))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            strmOut << TFWCurLn << L"Got the wrong exception\n\n";
        }
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Failed to catch bad read reset\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_BitSeq3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BitSeq31: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BitSeq3::TTest_BitSeq3() :

    TTestFWTest
    (
        L"Bit Sequence Adv 2", L"Advanced tests of the TBitSeq class", 3
    )
{
}

TTest_BitSeq3::~TTest_BitSeq3()
{
}


// ---------------------------------------------------------------------------
//  TTest_BitSeq3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BitSeq3::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    //
    //  Test out the non-normal bit modes. Do one using the duplicate
    //  complementary mode. This should create a second trailing copy that
    //  is completed.
    //
    {
        TBitSeq bsqTmp;

        //
        //  Test out the non-normal bit modes. Do one using the duplicate
        //  complementary mode. This should create a second trailing copy
        //  that is completed.
        //
        bsqTmp.AppendBits(0xF0, 8, tCIDLib::EBitSeqs::DupCompl);

        if (bsqTmp.c4WriteOfs() != 16)
        {
            strmOut << TFWCurLn << L"Write offset should have been 16 but was "
                    << bsqTmp.c4WriteOfs() << kCIDLib::EndLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // Read the results and make sure its correct
        if ((bsqTmp.c1At(0) != 0xF0) || (bsqTmp.c1At(1) != 0x0F))
        {
            strmOut << TFWCurLn << L"Dup compl write created wrong sequence\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Now read it back out and make sure it eats bits correctly
        if (bsqTmp.c4ExtractBits(8, tCIDLib::EBitSeqs::DupCompl) != 0xF0)
        {
            strmOut << TFWCurLn << L"Read wrong value from compl dup write\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // This should have eaten the complementary bits as well
        if (bsqTmp.c4ReadOfs() != 16)
        {
            strmOut << TFWCurLn << L"Read offset should have been 16 but was "
                    << bsqTmp.c4ReadOfs() << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // Reset and try the interleaved complementary mode
        bsqTmp.WriteReset();

        // Write out 01101, which should create 01 10 10 01 01
        bsqTmp.AppendBits(0xD, 5, tCIDLib::EBitSeqs::InterleavedCompl);
        if (bsqTmp.c1At(0) != 0x69)
        {
            strmOut << TFWCurLn
                    << L"Interleaved compl write created wrong sequence\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        if (bsqTmp.c4ExtractBits(5, tCIDLib::EBitSeqs::InterleavedCompl) != 0x0D)
        {
            strmOut << TFWCurLn
                    << L"Interleaved compl write created wrong sequence\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        if (bsqTmp.c4ReadOfs() != 10)
        {
            strmOut << TFWCurLn << L"Read offset should have been 10 but was "
                    << bsqTmp.c4ReadOfs() << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // Test out some appends of one sequence to another
    {
        TBitSeq bsqSrc;
        TBitSeq bsqDest;

        bsqSrc.AppendBits(1, 1);
        bsqDest.AppendBits(1, 1);
        bsqDest.AppendBits(bsqSrc);

        // This should leave the 0th byte at 0xC
        if (bsqDest.c1At(0) != 0xC0)
        {
            strmOut << TFWCurLn << L"Sequence append was incorrect\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  Put 4 more bits into the source, and append from it, starting
        //  from offset 3, which should append the final two bits "10".
        //
        bsqSrc.AppendBits(0xA, 4);
        bsqDest.AppendBits(bsqSrc, 3);

        // We had two and just should have added two more
        if (bsqDest.c4WriteOfs() != 4)
        {
            strmOut << TFWCurLn << L"Appended wrong number of bits\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // And it should have created the pattern "1110"
        if (bsqDest.c1At(0) != 0xE0)
        {
            strmOut << TFWCurLn << L"Sequence append was incorrect\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  Read one bit from the src, and then do an append from it, using
        //  the c4MaxCard value to cause it to start at the source's read
        //  position. So it should append the trailing 4 bits (0xA) that we
        //  added to the source above.
        //
        bsqSrc.c4ExtractBits(1);
        bsqDest.AppendBits(bsqSrc, kCIDLib::c4MaxCard);

        if (bsqDest.c1At(0) != 0xEA)
        {
            strmOut << TFWCurLn << L"Read position append was incorrect\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    return tTestFWLib::ETestRes::Success;
}

