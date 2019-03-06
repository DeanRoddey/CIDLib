//
// FILE NAME: TestCIDLib2_Base64.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains tests related to the Base64 oriented classes.
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
RTTIDecls(TTest_Base641,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Base6411
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Base6411: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Base641::TTest_Base641() :

    TTestFWTest
    (
        L"Base64 Round Trip", L"Basic round trip tests of the TBase64 class", 3
    )
{
}

TTest_Base641::~TTest_Base641()
{
}


// ---------------------------------------------------------------------------
//  TTest_Base641: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Base641::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test a zero length input, which should create zero output
    {
        tCIDLib::TCard1 c1Dummy = 0;
        if (!bDoExpTest(strmOut, L"Empty input test", &c1Dummy, 0, &c1Dummy, 0, 0))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Test a one character input
    {
        if (!bDoExpTest(strmOut, L"One char test", "A", 1, "QQ==", 4, 0))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Test two character input
    {
        if (!bDoExpTest(strmOut, L"Two char test", "AB", 2, "QUI=", 4, 0))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Three  character input
    {
        if (!bDoExpTest(strmOut, L"Three char test", "ABC", 3, "QUJD", 4, 0))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Test four character input
    {
        if (!bDoExpTest(strmOut, L"Four char test", "ABCD", 4, "QUJDRA==", 8, 0))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // All the bytes from 0 to FF
    {
        static const tCIDLib::TSCh* pszOut =
        {
            "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIj\r\n"
            "JCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZH\r\n"
            "SElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWpr\r\n"
            "bG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P\r\n"
            "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKz\r\n"
            "tLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX\r\n"
            "2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7\r\n"
            "/P3+/w=="
        };
        static const tCIDLib::TCard4 c4OutBytes(TRawStr::c4StrLen(pszOut));

        tCIDLib::TSCh achIn[256];
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 256; c4Index++)
            achIn[c4Index] = tCIDLib::TSCh(c4Index);

        if (!bDoExpTest(strmOut, L"All byte vals test", achIn, 256, pszOut, c4OutBytes, 48))
            eRes = tTestFWLib::ETestRes::Failed;
    }
    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_Base641: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TTest_Base641::bDoExpTest(      TTextOutStream&         strmOut
                        , const tCIDLib::TCh* const     pszTestName
                        , const tCIDLib::TCard1* const  pc1Input
                        , const tCIDLib::TCard4         c4InCount
                        , const tCIDLib::TCard1* const  pc1Expected
                        , const tCIDLib::TCard4         c4ExpCount
                        , const tCIDLib::TCard4         c4LineWidth)
{
    // Create an object to test with
    TBase64 b64Test;

    // Set the line width if one was passed
    if (c4LineWidth)
        b64Test.c4LineWidth(c4LineWidth);

    // Set up an input stream for the test data
    TBinMBufInStream strmIn(pc1Input, c4InCount);

    // Set up an output stream to take the output
    TBinMBufOutStream strmEnc(1024UL);

    // Do the encode
    const tCIDLib::TCard4 c4EncBytes = b64Test.c4Encode(strmIn, strmEnc);

    const TMemBuf& mbufOut = strmEnc.mbufData();
    const tCIDLib::TCard1* pc1Out = mbufOut.pc1Data();

    // See if the out bytes matches the expected
    if (c4ExpCount != c4EncBytes)
    {
        strmOut << TFWCurLn << pszTestName
                << L" failed. Expected byte count != encoded byte count\n";
        return kCIDLib::False;
    }

    // See if the actual bytes are the same, if any bytes
    if (c4EncBytes)
    {
        for(tCIDLib::TCard4 c4Index = 0; c4Index < c4EncBytes; c4Index++)
        {
            if(pc1Out[c4Index] != pc1Expected[c4Index])
            {
                strmOut << TFWCurLn << pszTestName
                        << L" failed. Expected content != encoded content at offset "
                        << c4Index
                        << L"\n";
                return kCIDLib::False;
            }
        }
    }

    //
    //  Round trip it back now. We need another output stream for that, and
    //  we need to create a linked stream to get to the data in the encoded
    //  output stream.
    //
    TBinMBufOutStream strmRound(1024UL);
    TBinMBufInStream strmEncIn(strmEnc);
    const tCIDLib::TCard4 c4RawBytes = b64Test.c4Decode(strmEncIn, strmRound);

    if (c4RawBytes != c4InCount)
    {
        strmOut << TFWCurLn << pszTestName
                << L" failed. Round trip bytes != incoming byte count\n";
        return kCIDLib::False;
    }

    if (c4RawBytes)
    {
        const TMemBuf& mbufOut = strmRound.mbufData();
        if (!TRawMem::bCompareMemBuf(mbufOut.pc1Data(), pc1Input, c4InCount))
        {
            strmOut << TFWCurLn << pszTestName
                    << L" failed. Round content != input content\n";
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TTest_Base641::bDoExpTest(       TTextOutStream&         strmOut
                        , const tCIDLib::TCh* const     pszTestName
                        , const tCIDLib::TSCh* const    pchInput
                        , const tCIDLib::TCard4         c4InCount
                        , const tCIDLib::TSCh* const    pchExpected
                        , const tCIDLib::TCard4         c4ExpCount
                        , const tCIDLib::TCard4         c4LineWidth)
{
    return bDoExpTest
    (
        strmOut
        , pszTestName
        , (tCIDLib::TCard1*)pchInput
        , c4InCount
        , (tCIDLib::TCard1*)pchExpected
        , c4ExpCount
        , c4LineWidth
    );
}


