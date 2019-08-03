//
// FILE NAME: TestCIDLib2_BinStreams.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/2009
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
//  This file contains tests related to the binary streams classes. There are
//  various variations, most of which can share common functionality since that's
//  the whole purpose of the streaming system. So we define a base binary stream
//  test class and derive the rest from that. Each variation just calls the base
//  class for core testing, then does any derivation specific bits.
//
//  The output tests are added to the list first (in the main Cpp file) so that
//  they are tested first. The inputs only support inked mode, i.e. they can onyl
//  read from output chunked streams. So we want to test the outputs first.
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
RTTIDecls(TTest_BaseBinStream,TTestFWTest)
RTTIDecls(TTest_ChunkedBinStream,TTest_BaseBinStream)
RTTIDecls(TTest_MBufBinStream,TTest_BaseBinStream)


// ---------------------------------------------------------------------------
//  CLASS: TTest_BaseBinStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedBinIntStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BaseBinStream::~TTest_BaseBinStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedBinIntStream: Hidden constructors
// ---------------------------------------------------------------------------
TTest_BaseBinStream::
TTest_BaseBinStream(const TString& strName, const TString& strDesc) :

    TTestFWTest(strName, strDesc, 3)
{
}



// ---------------------------------------------------------------------------
//  TTest_BaseBinStream: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BaseBinStream::eRunBaseTests( TBinInStream&           strmTestIn
                                    , TBinOutStream&        strmTestOut
                                    , TTextStringOutStream& strmOut
                                    , tCIDLib::TBoolean&    bWarning)
{
    try
    {
        // The input stream should be at 0 position
        if (strmTestIn.c4CurPos() != 0)
        {
            strmOut << TFWCurLn << L"Init size on in stream != 0\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  Write some data to the output and flush it. To avoid any rounding
        //  issues, don't use immediate values for floats.
        //
        const tCIDLib::TFloat4 f4Test(8.13F);
        const tCIDLib::TFloat8 f8Test(913.33);

        const TString strTest(L"This is a test");

        strmTestOut << tCIDLib::TCard1(0x54)
                    << tCIDLib::TCard2(0x8491)
                    << tCIDLib::TCard4(0x48741931)
                    << f4Test
                    << f8Test
                    << strTest
                    << kCIDLib::FlushIt;

        // We should not be at end of stream now
        if (strmTestIn.bEndOfStream())
        {
            strmOut << TFWCurLn << L"Should not be at end of stream\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Read the values back out
        tCIDLib::TCard1     c1Val;
        tCIDLib::TCard2     c2Val;
        tCIDLib::TCard4     c4Val;
        tCIDLib::TFloat4    f4Val;
        tCIDLib::TFloat8    f8Val;
        TString             strVal;

        strmTestIn  >>  c1Val
                    >>  c2Val
                    >>  c4Val
                    >>  f4Val
                    >>  f8Val
                    >>  strVal;

        // Make sure we got it all back
        if ((c1Val != 0x54)
        ||  (c2Val != 0x8491)
        ||  (c4Val != 0x48741931)
        ||  (f4Val != f4Test)
        ||  (f8Val != f8Test)
        ||  (strVal != strTest))
        {
            strmOut << TFWCurLn << L"Did not get out data written to stream\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // We should be at end of stream now
        if (!strmTestIn.bEndOfStream())
        {
            strmOut << TFWCurLn << L"Should have been at end of stream\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Reset it and we should be able to read them again
        strmTestIn.Reset();

        strmTestIn  >>  c1Val
                    >>  c2Val
                    >>  c4Val
                    >>  f4Val
                    >>  f8Val
                    >>  strVal;

        if ((c1Val != 0x54)
        ||  (c2Val != 0x8491)
        ||  (c4Val != 0x48741931)
        ||  (f4Val != f4Test)
        ||  (f8Val != f8Test)
        ||  (strVal != strTest))
        {
            strmOut << TFWCurLn << L"Did not get out data after reset\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Reset the output back to zero
        strmTestOut.Reset();

        // The output stream should now be at zero pos
        if (strmTestOut.c4CurPos())
        {
            strmOut << TFWCurLn << L"Out stream reset didn't reset size/pos\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // It won't have affected the input stream position
        if (!strmTestIn.c4CurPos())
        {
            strmOut << TFWCurLn << L"In stream pos affected by output reset\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Reset the input now which should take it back to zero
        strmTestIn.Reset();
        if (strmTestIn.c4CurPos())
        {
            strmOut << TFWCurLn << L"In stream pos not reset\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // We should get an exception if we try to read something onw
        try
        {
            strmTestIn >> c2Val;
            strmOut << TFWCurLn << L"No exception when reading at end\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        catch(...)
        {
        }


        // Write out some values
        strmTestOut << tCIDLib::TCard1(0x11)
                    << tCIDLib::TCard2(0x22)
                    << tCIDLib::TCard4(0x44)
                    << strTest
                    << kCIDLib::FlushIt;

        // Skip forward by 3 bytes and we should then be able to read the Card4 value
        strmTestIn.SkipForwardBy(3);
        if (strmTestIn.c4CurPos() != 3)
        {
            strmOut << TFWCurLn << L"Skip forward did not change input pos\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        strmTestIn >> c4Val;
        if (c4Val != 0x44)
        {
            strmOut << TFWCurLn << L"Skip did not get to expected value\n\n";
            return tTestFWLib::ETestRes::Failed;
        }


        // Run some pushback tests
        tTestFWLib::ETestRes eRes = eRunPushbackTests
        (
            strmTestIn, strmTestOut, strmOut, bWarning
        );
        if (eRes != tTestFWLib::ETestRes::Success)
            return eRes;
    }

    catch(TError& errToCatch)
    {
        strmOut << errToCatch << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}


// ---------------------------------------------------------------------------
//  TTest_BaseBinStream: Private, non-virtual methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BaseBinStream::eRunPushbackTests( TBinInStream&           strmTestIn
                                        , TBinOutStream&        strmTestOut
                                        , TTextStringOutStream& strmOut
                                        , tCIDLib::TBoolean&    bWarning)
{
    // Reset both streams for some pushback tests
    strmTestIn.Reset();
    strmTestOut.Reset();

    // Don't allow pushback before the start
    try
    {
        strmTestIn.c4Pushback(1);
        strmOut << TFWCurLn << L"Failed to catch pushback underflow\n\n";
        return tTestFWLib::ETestRes::Failed;
    }
    catch(...) {}

    for (tCIDLib::TCard1 c1Index = 0; c1Index < 8; c1Index++)
        strmTestOut << c1Index;
    strmTestOut.Flush();

    // Read a byte and the count should drop to 7
    tCIDLib::TCard1 c1Tmp;
    strmTestIn >> c1Tmp;

    if (strmTestIn.c4CurPos() != 1)
    {
        strmOut << TFWCurLn << L"Current position should have been 1\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Push the byte back and it should go back to 0
    strmTestIn.c4Pushback(c1Tmp);
    if (strmTestIn.c4CurPos() != 0)
    {
        strmOut << TFWCurLn
                << L"Current position should have been 0 after pushback\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Read the byte again and it should be the same
    tCIDLib::TCard1 c1Tmp2;
    strmTestIn >> c1Tmp2;
    if (c1Tmp != c1Tmp2)
    {
        strmOut << TFWCurLn << L"Pushed back byte should have been read next\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // And the current position should be 1 again
    if (strmTestIn.c4CurPos() != 1)
    {
        strmOut << TFWCurLn
                << L"Current position should have been 1 after reading pushback\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Read out the remaining 7 bytes, then do a pushback of all of the values
    for (tCIDLib::TCard1 c1Index = 0; c1Index < 7; c1Index++)
        strmTestIn >> c1Tmp;

    if (!strmTestIn.bEndOfStream())
    {
        strmOut << TFWCurLn << L"Stream should be at end after reading all bytes\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (strmTestIn.c4CurPos() != 8)
    {
        strmOut << TFWCurLn
                << L"Position should have been 8 after reading all bytes\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    for (tCIDLib::TCard1 c1Index = 8; c1Index > 0; c1Index--)
        strmTestIn.c4Pushback(c1Index - 1);

    if (strmTestIn.c4CurPos() != 0)
    {
        strmOut << TFWCurLn
                << L"Position should have been 0 after pushing back all bytes\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // And again, we shouldn't be able to push back now
    try
    {
        strmTestIn.c4Pushback(1);
        strmOut << TFWCurLn << L"Failed to catch pushback underflow\n\n";
        return tTestFWLib::ETestRes::Failed;
    }
    catch(...) {}

    // Make sure they come back out in the right order
    for (tCIDLib::TCard1 c1Index = 0; c1Index < 8; c1Index++)
    {
        strmTestIn >> c1Tmp;
        if (c1Tmp != c1Index)
        {
            strmOut << TFWCurLn
                    << L"Pushed back bytes came out in the wrong order\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    if (!strmTestIn.bEndOfStream())
    {
        strmOut << TFWCurLn
                << L"Stream should be at end after reading all pushed back bytes\n\n";
        return tTestFWLib::ETestRes::Failed;
    }
    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedBinStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedBinIntStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedBinStream::TTest_ChunkedBinStream() :

    TTest_BaseBinStream
    (
        L"Bin chunked stream", L"Basic tests of the chunked binary streams"
    )
{
}

TTest_ChunkedBinStream::~TTest_ChunkedBinStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedBinStream: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedBinStream::eRunTest( TTextStringOutStream&   strmOut
                                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRet = tTestFWLib::ETestRes::Success;
    try
    {
        // Create a chunked output stream and a linked input stream
        TChunkedBinOutStream strmTestOut(8192);
        TChunkedBinInStream  strmTestIn(strmTestOut);

        // The input stream should show zero current size
        if (strmTestIn.c4CurSize() != 0)
        {
            strmOut << TFWCurLn << L"Init size on in stream != 0\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Run the common tests
        eRet = eRunBaseTests(strmTestIn, strmTestOut, strmOut, bWarning);

        // If that worked, run some chunked stream specific tests
        if (eRet == tTestFWLib::ETestRes::Success)
            eRet = eRunOutTests(strmOut, bWarning);
    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch.strErrText() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    return eRet;
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedBinStream: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  These are derivative specific tests of chunked output streams. We want to test
//  these specifically, to make sure the chunking is working right.
//
tTestFWLib::ETestRes
TTest_ChunkedBinStream::eRunOutTests(   TTextStringOutStream&   strmOut
                                        , tCIDLib::TBoolean&    bWarning)
{
    try
    {
        // Create a chunked binary output stream with a max storage of 3MB.
        const tCIDLib::TCard4 c4MaxBufSz = 3 * (1024 * 1024);
        TChunkedBinOutStream strmTest(c4MaxBufSz);

        //
        //  Build up a few buffers of prime sizes so that we can test writing
        //  different sized chunks to it.
        //
        THeapBuf mbuf1(109, 109);
        THeapBuf mbuf2(1597, 1597);
        THeapBuf mbuf3(12391, 12391);

        // Set known data into these buffers
        mbuf1.Set(0xCE);
        mbuf2.Set(0x38);
        mbuf3.Set(0xF1);

        // The current size of the data should be zero
        if (strmTest.c4CurSize() != 0)
        {
            strmOut << TFWCurLn << L"Initial stream size was not zero\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // The max size should be what we set
        if (strmTest.c4MaxSize() != c4MaxBufSz)
        {
            strmOut << TFWCurLn << L"Max stream size was not as set\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Current position should be zero
        if (strmTest.c4CurPos() || strmTest.c8CurPos())
        {
            strmOut << TFWCurLn << L"Position should be zero here\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Now write a first buffer to it and flush it and check the size
        if (strmTest.c4WriteBuffer(mbuf1, 109) != 109)
        {
            strmOut << TFWCurLn << L"Did not write all bytes\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
        strmTest.Flush();
        if (strmTest.c4CurSize() != 109)
        {
            strmOut << TFWCurLn << L"Stream size invalid after write\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Write a copy of the other two and flush and check again
        if (strmTest.c4WriteBuffer(mbuf2, 1597) != 1597)
        {
            strmOut << TFWCurLn << L"Did not write all bytes\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
        if (strmTest.c4WriteBuffer(mbuf3, 12391) != 12391)
        {
            strmOut << TFWCurLn << L"Did not write all bytes\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
        strmTest.Flush();
        if (strmTest.c4CurSize() != 109 + 1597 + 12391)
        {
            strmOut << TFWCurLn << L"Stream size invalid after write\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  Write out the big buffer enough times that we know it will force
        //  the stream to allocate a second buffer.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 100; c4Index++)
        {
            if (strmTest.c4WriteBuffer(mbuf3, 12391) != 12391)
            {
                strmOut << TFWCurLn << L"Did not write all bytes\n\n";
                return tTestFWLib::ETestRes::Failed;
            }
        }
        strmTest.Flush();
        const tCIDLib::TCard4 c4FullSz = 109 + 1597 + 12391 + 1239100;
        if (strmTest.c4CurSize() != c4FullSz)
        {
            strmOut << TFWCurLn << L"Stream size invalid after big write\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  Copy out the data and let's go through and make sure that all of the
        //  bytes are correct.
        //
        THeapBuf mbufFull(c4FullSz, c4FullSz);
        const tCIDLib::TCard4 c4Copied = strmTest.c4CopyOutTo(mbufFull, 0);

        if (c4Copied != c4FullSz)
        {
            strmOut << TFWCurLn << L"Not all bytes were copied out\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        tCIDLib::TCard4 c4CurOfs = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 109; c4Index++)
        {
            const tCIDLib::TCard1 c1Cur = mbufFull[c4CurOfs];
            if (c1Cur != 0xCE)
            {
                strmOut << TFWCurLn << L"Got byte 0x"
                        << TCardinal(c1Cur, tCIDLib::ERadices::Hex)
                        << L" at offset " << c4CurOfs
                        << L" but expected 0xCE\n\n";
                return tTestFWLib::ETestRes::Failed;
            }
            c4CurOfs++;
        }

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 1597; c4Index++)
        {
            const tCIDLib::TCard1 c1Cur = mbufFull[c4CurOfs];
            if (c1Cur != 0x38)
            {
                strmOut << TFWCurLn << L"Got byte 0x"
                        << TCardinal(c1Cur, tCIDLib::ERadices::Hex)
                        << L" at offset " << c4CurOfs
                        << L" but expected 0x38\n\n";
                return tTestFWLib::ETestRes::Failed;
            }
            c4CurOfs++;
        }

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 12391; c4Index++)
        {
            const tCIDLib::TCard1 c1Cur = mbufFull[c4CurOfs];
            if (c1Cur != 0xF1)
            {
                strmOut << TFWCurLn << L"Got byte 0x"
                        << TCardinal(c1Cur, tCIDLib::ERadices::Hex)
                        << L" at offset " << c4CurOfs
                        << L" but expected 0xF1\n\n";
                return tTestFWLib::ETestRes::Failed;
            }
            c4CurOfs++;
        }


        // Reset the stream which should se us back to zero size
        strmTest.Reset();
        if (strmTest.c4CurSize() != 0)
        {
            strmOut << TFWCurLn << L"Reset did not clear current size\n\n";
            return tTestFWLib::ETestRes::Failed;
        }


        // Write up to the point where one more would not fit
        const tCIDLib::TCard4 c4MaxWrites = c4MaxBufSz / 12391;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MaxWrites; c4Index++)
        {
            if (strmTest.c4WriteBuffer(mbuf3, 12391) != 12391)
            {
                strmOut << TFWCurLn << L"Data should have fit\n\n";
                return tTestFWLib::ETestRes::Failed;
            }
        }
        strmTest.Flush();
        if (strmTest.c4CurSize() >= c4MaxBufSz)
        {
            strmOut << TFWCurLn << L"Stream should not be full yet\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Now write the last one and it should absorb the remaining space
        const tCIDLib::TCard4 c4LastSpace = c4MaxBufSz - strmTest.c4CurSize();
        if (strmTest.c4WriteBuffer(mbuf3, 12391, tCIDLib::EAllData::OkIfNotAll) != c4LastSpace)
        {
            strmOut << TFWCurLn << L"Final buffer did not absorb correct bytes\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // The size should be max now after a flush
        strmTest.Flush();
        if (strmTest.c4CurSize() != c4MaxBufSz)
        {
            strmOut << TFWCurLn << L"Stream should be full now\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Another write should write nothing
        if (strmTest.c4WriteBuffer(mbuf3, 12391, tCIDLib::EAllData::OkIfNotAll) != 0)
        {
            strmOut << TFWCurLn << L"Was able to write bytes when stream is full\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // And an attempt to write with no ok if not all, we hsould get an exception
        try
        {
            // Just one byte hsould be enough
            strmTest.c4WriteBuffer(mbuf3, 1);
            strmTest.Flush();

            strmOut << TFWCurLn << L"Write when full did not cause exception\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        catch(...)
        {
        }

        // Truncate it back to 110 bytes
        strmTest.TruncateAt(110);
        if (strmTest.c4CurSize() != 110)
        {
            strmOut << TFWCurLn << L"Stream size should be 110 after truncation\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // The current position should have been reset
        if (strmTest.c8CurPos() != 0)
        {
            strmOut << TFWCurLn << L"Truncate did not reset write position\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }


    try
    {
        //
        //  Let's test the creation of a fairly large buffer and insure that all
        //  of the bytes come through exactly correct. We'll set up a buffer of
        //  sequential bytes, of a size that is guaranteed to not to fall on even
        //  buffer boundaries and ramp it up to a few MB of data.
        //
        const tCIDLib::TCard4 c4MaxBufSz = 16 * (1024 * 1024);
        TChunkedBinOutStream strmTestOut(c4MaxBufSz);

        tCIDLib::TCard1 ac1Data1[177];
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 177; c4Index++)
            ac1Data1[c4Index] = tCIDLib::TCard1(c4Index);

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 20000; c4Index++)
            strmTestOut.c4WriteRawBuffer(ac1Data1, 177);

        // Flush it, and now go back and make sure we those bytes back
        strmTestOut.Flush();

        TChunkedBinInStream strmTestIn(strmTestOut);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 20000; c4Index++)
        {
            strmTestIn.c4ReadRawBuffer(ac1Data1, 177);
            for (tCIDLib::TCard4 c4Ind2 = 0; c4Ind2 < 177; c4Ind2++)
            {
                if (ac1Data1[c4Ind2] != c4Ind2)
                {
                    strmOut << TFWCurLn << L"Read back bad data in block index "
                            << c4Index
                            << L"\n\n";
                    return tTestFWLib::ETestRes::Failed;
                }
            }
        }
    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }


    try
    {
        //
        //  Let's do teh same thing now, but with a larger buffer that will always
        //  fall on even buoffer boundaries.
        //
        const tCIDLib::TCard4 c4MaxBufSz = 16 * (1024 * 1024);
        TChunkedBinOutStream strmTestOut(c4MaxBufSz);

        tCIDLib::TCard1 ac1Data1[0x10000];
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 0x10000; c4Index++)
            ac1Data1[c4Index] = tCIDLib::TCard1(c4Index & 0xFF);

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 50; c4Index++)
            strmTestOut.c4WriteRawBuffer(ac1Data1, 0x10000);

        // Flush it, and now go back and make sure we those bytes back
        strmTestOut.Flush();

        TChunkedBinInStream strmTestIn(strmTestOut);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 50; c4Index++)
        {
            strmTestIn.c4ReadRawBuffer(ac1Data1, 0x10000);
            for (tCIDLib::TCard4 c4Ind2 = 0; c4Ind2 < 0x10000; c4Ind2++)
            {
                if (ac1Data1[c4Ind2] != tCIDLib::TCard1(c4Ind2 & 0xFF))
                {
                    strmOut << TFWCurLn << L"Read back bad data in big block index "
                            << c4Index
                            << L"\n\n";
                    return tTestFWLib::ETestRes::Failed;
                }
            }
        }

        //
        //  Create a standard memory buffer based stream and copy the data from
        //  the output stream to the new one.
        //
        TBinMBufOutStream strmCopyOut(strmTestOut.c4CurSize(), strmTestOut.c4CurSize());
        strmTestOut.c4CopyOutToStream(strmCopyOut, strmTestOut.c4CurSize());

        // Now create a linked input stream for that, and test again
        TBinMBufInStream strmCopyIn(strmCopyOut);

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 50; c4Index++)
        {
            strmCopyIn.c4ReadRawBuffer(ac1Data1, 0x10000);
            for (tCIDLib::TCard4 c4Ind2 = 0; c4Ind2 < 0x10000; c4Ind2++)
            {
                if (ac1Data1[c4Ind2] != tCIDLib::TCard1(c4Ind2 & 0xFF))
                {
                    strmOut << TFWCurLn << L"Read back bad copied data in big block index "
                            << c4Index
                            << L"\n\n";
                    return tTestFWLib::ETestRes::Failed;
                }
            }
        }
    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }
    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_MBufBinStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_MBufBinIntStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_MBufBinStream::TTest_MBufBinStream() :

    TTest_BaseBinStream
    (
        L"Bin membuf stream", L"Basic tests of memory buffer binary streams"
    )
{
}

TTest_MBufBinStream::~TTest_MBufBinStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_MBufBinStream: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_MBufBinStream::eRunTest( TTextStringOutStream&   strmOut
                                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRet = tTestFWLib::ETestRes::Success;
    try
    {
        TBinMBufOutStream strmTestOut(128UL);
        TBinMBufInStream strmTestIn(strmTestOut);

        // Do common tests
        eRet = eRunBaseTests(strmTestIn, strmTestOut, strmOut, bWarning);
    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }
    return tTestFWLib::ETestRes::Success;
}
