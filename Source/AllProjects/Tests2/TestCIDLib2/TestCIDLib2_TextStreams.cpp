//
// FILE NAME: TestCIDLib2_TextStreams.cpp
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
//  This file contains tests related to the text streams classes. There are
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
RTTIDecls(TTest_BaseTextStream,TTestFWTest)
RTTIDecls(TTest_ChunkedTextStream,TTest_BaseTextStream)


// ---------------------------------------------------------------------------
//  CLASS: TTest_BaseTextStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BaseTextStream: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BaseTextStream::TTest_BaseTextStream(const TString& strName, const TString& strDescr) :

    TTestFWTest(strName, strDescr, 3)
{
}

TTest_BaseTextStream::~TTest_BaseTextStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_BaseTextStream: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BaseTextStream::eRunBaseTests(TTextInStream&          strmTestIn
                                    , TTextOutStream&       strmTestOut
                                    , TTextStringOutStream& strmOutput
                                    , tCIDLib::TBoolean&    bWarning)
{

    return tTestFWLib::ETestRes::Success;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedTextStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedTextInStream1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedTextStream::TTest_ChunkedTextStream() :

    TTest_BaseTextStream
    (
        L"Text In chunked stream", L"Basic tests of the TChunkedTextInStream class"
    )
{
}

TTest_ChunkedTextStream::~TTest_ChunkedTextStream()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedTextStream: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedTextStream::eRunTest(  TTextStringOutStream&   strmOut
                                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRet = tTestFWLib::ETestRes::Success;
    try
    {
        const tCIDLib::TCard4 c4MaxBufSz = 3 * (1024 * 1024);
        TChunkedTextOutStream strmTestOut(c4MaxBufSz);
        TChunkedTextInStream strmTestIn(strmTestOut);

        // Run the base tests
        eRet = TParent::eRunBaseTests(strmTestIn, strmTestOut, strmOut, bWarning);
    }

    catch(TError& errToCatch)
    {
        strmOut << TFWCurLn << errToCatch << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedTextStream: Private, non-virtual methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedTextStream::eRunOutTests(TTextStringOutStream&  strmOut
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
            L"Version 4.4 includes some fairly significant changes. We are setting "
            L"the stage for for the next steps in the product, and some of the changes "
            L"may require some small adjustments for existing users. You should also be "
            L"aware of new features that you might make use of. This document lays out "
            L"all the significant changes."
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

        TTextMBufInStream strmSrc(tCIDLib::ForceMove(mbufText), c4DataSz);
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

