//
// FILE NAME: TestCIDLib2_ChunkedStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/2009
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains tests related to the binary and text chunked stream classes.
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
RTTIDecls(TTest_ChunkedBinInStream,TTestFWTest)
RTTIDecls(TTest_ChunkedBinOutStream,TTestFWTest)
RTTIDecls(TTest_ChunkedTextInStream,TTestFWTest)
RTTIDecls(TTest_ChunkedTextOutStream,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedBinInStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedBinIntStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedBinInStream::TTest_ChunkedBinInStream() :

    TTestFWTest
    (
        L"Bin In chunked stream", L"Basic tests of the TChunkedBinIntStream class", 3
    )
{
}

TTest_ChunkedBinInStream::~TTest_ChunkedBinInStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedBinIntStream: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedBinInStream::eRunTest( TTextStringOutStream&   strmOut
                                    , tCIDLib::TBoolean&    bWarning)
{
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

        // The input and output streams should have the same size now
        if (strmTestIn.c4CurSize() != strmTestOut.c4CurSize())
        {
            strmOut << TFWCurLn << L"In/out disagree on current end of data\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

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

        // The output stream should now be at zero pos and zero size
        if (strmTestOut.c4CurPos() || strmTestOut.c4CurSize())
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
    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch.strErrText() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedBinOutStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedBinOutStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedBinOutStream::TTest_ChunkedBinOutStream() :

    TTestFWTest
    (
        L"Bin out chunked stream", L"Basic tests of the TChunkedBinOutStream class", 3
    )
{
}

TTest_ChunkedBinOutStream::~TTest_ChunkedBinOutStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedBinOutStream: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedBinOutStream::eRunTest(TTextStringOutStream&   strmOut
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
//  CLASS: TTest_ChunkedTextInStream
// PREFIX: tfwt
//
//  Note that we aren't really worried too much about writing large amounts
//  of data here, since we've tested that in the output stream test. It's more
//  just making sure we see the data written.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedTextInStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedTextInStream::TTest_ChunkedTextInStream() :

    TTestFWTest
    (
        L"Text In chunked stream", L"Basic tests of the TChunkedTextInStream class", 3
    )
{
}

TTest_ChunkedTextInStream::~TTest_ChunkedTextInStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedTextInStream: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedTextInStream::eRunTest(TTextStringOutStream&   strmOut
                                    , tCIDLib::TBoolean&    bWarning)
{
    try
    {

    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}






// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedTextOutStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedTextOutStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedTextOutStream::TTest_ChunkedTextOutStream() :

    TTestFWTest
    (
        L"Text out chunked stream", L"Basic tests of the TChunkedTextOutStream class", 3
    )
{
}

TTest_ChunkedTextOutStream::~TTest_ChunkedTextOutStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedTextOutStream: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedTextOutStream::eRunTest(TTextStringOutStream&  strmOut
                                    , tCIDLib::TBoolean&    bWarning)
{
    try
    {
        const tCIDLib::TCard4 c4MaxBufSz = 3 * (1024 * 1024);
        TChunkedTextOutStream strmTest(c4MaxBufSz);

        //
        //  Create a string and just write it out enough times to cause at least
        //  one buffer allocation.
        //
        const TString strTest
        (
            L"Version 4.4 includes some fairly significant changes. We are setting the stage for "
            L"for the next steps in the product, and some of the changes may require some small "
            L"adjustments for existing users. You should also be aware of new features that you "
            L"might make use of. This document lays out all the significant changes."
        );

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 1000; c4Index++)
            strmTest << strTest << kCIDLib::NewLn;

        strmTest.Flush();

        //
        //  Copy out the data, then set up a regular text input stream on it, and let's
        //  read back out the strings and make sure we get back what we put in.
        //
        const tCIDLib::TCard4 c4DataSz = strmTest.c4CurSize();
        THeapBuf mbufText(c4DataSz, c4DataSz);
        const tCIDLib::TCard4 c4CopiedSz = strmTest.c4CopyOutTo(mbufText, 0);

        if (c4CopiedSz != c4DataSz)
        {
            strmOut << TFWCurLn << L"Copied out " << c4CopiedSz << L" but "
                    << c4DataSz << L" bytes were available\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        TTextMBufInStream strmSrc(&mbufText, c4DataSz);
        TString strRead;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 1000; c4Index++)
        {
            strmSrc >> strRead;
            if (strRead != strTest)
            {
                strmOut << TFWCurLn << L"Read in different text thatn written\n\n";
                return tTestFWLib::ETestRes::Failed;
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


