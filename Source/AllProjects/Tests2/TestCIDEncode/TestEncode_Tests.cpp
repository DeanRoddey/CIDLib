//
// FILE NAME: TestEncode_Tests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/15/2008
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
//  This test program doesn't have a lot of tests, so we just implement them
//  all in this same file.
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
#include    "TestEncode.hpp"


// ---------------------------------------------------------------------------
//  Normally the converter headers are not public and they are created using
//  calls on the facility object. But for testing purposes we bring in the ones
//  we are going to test.
// ---------------------------------------------------------------------------
#include    "CIDEncode_EBCDIC_037_.hpp"
#include    "CIDEncode_EBCDIC_1140_.hpp"
#include    "CIDEncode_ISO8859_1_.hpp"
#include    "CIDEncode_ISO8859_3_.hpp"
#include    "CIDEncode_ISO8859_4_.hpp"
#include    "CIDEncode_ISO8859_5_.hpp"
#include    "CIDEncode_ISO8859_6_.hpp"
#include    "CIDEncode_PC_437_.hpp"
#include    "CIDEncode_PC_850_.hpp"
#include    "CIDEncode_Win_1251_.hpp"
#include    "CIDEncode_Win_1252_.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_Convert,TTestFWTest)
RTTIDecls(TTest_ErrModes,TTestFWTest)
RTTIDecls(TTest_RoundTrip1,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_Convert
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Convert: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Convert::TTest_Convert() :

    TTestFWTest
    (
        L"Basic Conversion"
        , L"Tests basic text internal/externalization methods"
        , 2
    )
{
}

TTest_Convert::~TTest_Convert()
{
}


// ---------------------------------------------------------------------------
//  TTest_Convert: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Convert::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes    eRes = tTestFWLib::ETestRes::Success;
    THeapBuf                mbufTest(1024, 1024);

    // ASCII
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x61, 0x62, 0x20, 0x63, 0x64
        };
        const TString strData(L"ab cd");

        TUSASCIIConverter tcvtTest;
        if (!bTestSrc(strmOut, tcvtTest, ac1Data, 5, strData))
            eRes = tTestFWLib::ETestRes::Failed;

        if (!bTestTar(strmOut, tcvtTest, strData, 5, ac1Data))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // UTF-8
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x41, 0x42, 0x20, 0x43, 0x44
        };
        const TString strData(L"AB CD");

        TUTF8Converter tcvtTest;
        if (!bTestSrc(strmOut, tcvtTest, ac1Data, 5, strData))
            eRes = tTestFWLib::ETestRes::Failed;

        if (!bTestTar(strmOut, tcvtTest, strData, 5, ac1Data))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do a representative table based mode 1 converter
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x41, 0x42, 0x20, 0x43, 0x44
        };
        const TString strData(L"AB CD");

        TLatin3Converter tcvtTest;
        if (!bTestSrc(strmOut, tcvtTest, ac1Data, 5, strData))

            eRes = tTestFWLib::ETestRes::Failed;

        if (!bTestTar(strmOut, tcvtTest, strData, 5, ac1Data))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do Latin1 which is an odd man out
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x61, 0x62, 0x20, 0x63, 0x64
        };
        const TString strData(L"ab cd");

        TLatin1Converter tcvtTest;
        if (!bTestSrc(strmOut, tcvtTest, ac1Data, 5, strData))
            eRes = tTestFWLib::ETestRes::Failed;

        if (!bTestTar(strmOut, tcvtTest, strData, 5, ac1Data))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Test a Unicode to 1251 conversion
    {
        const tCIDLib::TCh achData[] =
        {
              42,   83,   49,   68,  73,  83,  80,  76,   73, 78
          ,   69,   49,   34,  115, 100, 102, 1089, 32, 1076, 1092
          , 1076, 1089, 1092, 1076,  34, 0
        };

        TWin1251Converter tcvtTest;
        THeapBuf mbufData(1024, 1024);
        tCIDLib::TCard4 c4ByteCnt;
        TString strSrc(achData);
        try
        {
            tcvtTest.c4ConvertTo(strSrc, mbufData, c4ByteCnt);
        }

        catch(...)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Failed in Cyrillic conversion"
                    << kCIDLib::DNewLn;
        }
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  TTest_Convert: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Tests a single internalization scenario. We expect to transcode all
//  available incoming bytes, and to end up with the indicated expected
//  number of characters, and the resulting string.
//
tCIDLib::TBoolean
TTest_Convert::bTestSrc(        TTextOutStream&         strmOut
                        ,       TTextConverter&         tcvtTest
                        , const tCIDLib::TCard1* const  pc1Data
                        , const tCIDLib::TCard4         c4InCount
                        , const TString&                strExpRes)
{
    tCIDLib::TCard4 c4BytesEaten;
    TString         strTest;
    try
    {
        c4BytesEaten = tcvtTest.c4ConvertFrom(pc1Data, c4InCount, strTest);
    }

    catch(...)
    {
        strmOut << TFWCurLn << L"In conversion failed for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (c4BytesEaten != c4InCount)
    {
        strmOut << TFWCurLn << L"Not all bytes were internalized for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (strTest != strExpRes)
    {
        strmOut << TFWCurLn << L"Got wrong resulting text for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  Tests a single externalization scenario. We expect to transcode all
//  srce chars and to end up with the indicated number of bytes, and the
//  indicated actual bytes content.
//
tCIDLib::TBoolean
TTest_Convert::bTestTar(        TTextOutStream&         strmOut
                        ,       TTextConverter&         tcvtTest
                        , const TString&                strData
                        , const tCIDLib::TCard4         c4ExpBytes
                        , const tCIDLib::TCard1* const  pc1ExpRes)
{
    tCIDLib::TCard4 c4OutBytes;
    tCIDLib::TCard4 c4CharsEaten;
    THeapBuf        mbufOut(1024UL, 2048UL);
    try
    {
        c4CharsEaten = tcvtTest.c4ConvertTo(strData, mbufOut, c4OutBytes);
    }

    catch(...)
    {
        strmOut << TFWCurLn << L"In conversion failed for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (c4CharsEaten != strData.c4Length())
    {
        strmOut << TFWCurLn << L"Not all bytes were externalized for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (c4OutBytes != c4ExpBytes)
    {
        strmOut << TFWCurLn << L"Wrong number of bytes externalized for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (!mbufOut.bCompare(pc1ExpRes, c4ExpBytes))
    {
        strmOut << TFWCurLn << L"Got incorrect output data for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  CLASS: TTest_ErrModes
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ErrModes: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ErrModes::TTest_ErrModes() :

    TTestFWTest
    (
        L"Error Action Mode Tests"
        , L"Tests the error modes of text internalization"
        , 3
    )
{
}

TTest_ErrModes::~TTest_ErrModes()
{
}


// ---------------------------------------------------------------------------
//  TTest_ErrModes: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ErrModes::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes    eRes = tTestFWLib::ETestRes::Success;
    THeapBuf                mbufTest(1024, 1024);

    //
    //  Let's test the internalization direction.
    //
    //  Note that some will not fail, since they can represent any text
    //  we can feed them. So we don't bother testing them here
    //
    //  Also note that a lot of them are done by way of a common base class
    //  that they provide lookup tables to. So we only need to test one of
    //  those types.
    //

    // ASCII
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x61, 0x62, 0xCD, 0x63, 0x64
        };
        TUSASCIIConverter tcvtTest;
        if (!bTestBadSrc(strmOut, tcvtTest, ac1Data, 5, 2, L"ab cd", L' ', 2))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // UTF-8
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x61, 0x62, 0xE0, 0x0, 0x0, 0x63, 0x64
        };

        TUTF8Converter tcvtTest;
        if (!bTestBadSrc(strmOut, tcvtTest, ac1Data, 7, 2, L"ab cd", L' ', 2))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    //
    //  Do a representative table based mode 1 converter that cannot
    //  represent our sample data value.
    //
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x61, 0x62, 0xC3, 0x63, 0x64
        };
        TLatin3Converter tcvtTest;
        if (!bTestBadSrc(strmOut, tcvtTest, ac1Data, 5, 2, L"ab cd", L' ', 2))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do the Latin1 which is an odd man out
    {
        const tCIDLib::TCard1 ac1Data[] =
        {
            0x61, 0x62, 0x95, 0x63, 0x64
        };

        TLatin1Converter tcvtTest;
        if (!bTestBadSrc(strmOut, tcvtTest, ac1Data, 5, 2, L"ab cd", L' ', 2))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    //
    //  And now lets's test the externalization mode.
    //
    {
        TString strSrc(L"AB CD");
        strSrc[2] = tCIDLib::TCh(0x91);
        TUSASCIIConverter tcvtTest;
        if (!bTestBadTar(strmOut, tcvtTest, strSrc, 2, 2))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    {
        TString strSrc(L"AB CD");
        strSrc[2] = tCIDLib::TCh(0xC3);
        TLatin3Converter tcvtTest;
        if (!bTestBadTar(strmOut, tcvtTest, strSrc, 2, 2))
            eRes = tTestFWLib::ETestRes::Failed;
    }

    {
        TString strSrc(L"AB CD");
        strSrc[2] = tCIDLib::TCh(0x91);
        TLatin1Converter tcvtTest;
        if (!bTestBadTar(strmOut, tcvtTest, strSrc, 2, 2))
            eRes = tTestFWLib::ETestRes::Failed;
    }
    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_ErrModes: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper to generically test various encoders to see if they correctly
//  deal with bad source bytes correctly during internalization. We test
//  all three modes and make sure that they work.
//
tCIDLib::TBoolean
TTest_ErrModes::bTestBadSrc(        TTextOutStream&         strmOut
                            ,       TTextConverter&         tcvtTest
                            , const tCIDLib::TCard1* const  pc1Data
                            , const tCIDLib::TCard4         c4Count
                            , const tCIDLib::TCard4         c4ErrIndex
                            , const TString&                strRepResult
                            , const tCIDLib::TCh            chRepChar
                            , const tCIDLib::TCard4         c4ErrBytes)
{
    CIDAssert(c4ErrIndex < c4Count, L"Error index is >= count");

    tCIDLib::TBoolean   bCaughtIt;
    tCIDLib::TCard4     c4BytesEaten;
    tCIDLib::TCard4     c4ChDone;
    TString             strTest;

    //
    //  First, set the mode to Throw mode, which sould fail without
    //  transcoding anything.
    //
    tcvtTest.eErrorAction(tCIDLib::ETCvtActions::Throw);
    bCaughtIt = kCIDLib::False;
    try
    {
        tcvtTest.c4ConvertFrom(pc1Data, c4Count, strTest);
    }

    catch(...)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << TFWCurLn
                << L"Failed to catch invalid data in Throw mode for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    //
    //  Now test the StopThenThrow mode. We should decode c4ErrIndex
    //  characters the first time, then throw the second time.
    //
    tcvtTest.eErrorAction(tCIDLib::ETCvtActions::StopThenThrow);
    try
    {
        c4BytesEaten = tcvtTest.c4ConvertFrom
        (
            pc1Data, c4Count, strTest
        );
    }

    catch(...)
    {
        strmOut << TFWCurLn << L"Stop/Throw mode failed for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (strTest.c4Length() != c4ErrIndex)
    {
        strmOut << TFWCurLn << L"Stop/Throw mode stopped at wrong index for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (c4BytesEaten != c4ErrBytes)
    {
        strmOut << TFWCurLn << L"Stop/Throw mode stopped at wrong src byte for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    // Now do the second one, starting at the error index which should fail
    bCaughtIt = kCIDLib::False;
    try
    {
        tcvtTest.c4ConvertFrom(&pc1Data[c4ErrIndex], c4Count - c4ErrIndex, strTest);
    }

    catch(...)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << TFWCurLn
                << L"Stop/throw mode didn't fail on second pass for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    // Now test the replacement character mode
    tcvtTest.eErrorAction(tCIDLib::ETCvtActions::Replace);
    tcvtTest.chRepChar(chRepChar);
    try
    {
        c4ChDone = tcvtTest.c4ConvertFrom(pc1Data, c4Count, strTest);
    }

    catch(...)
    {
        strmOut << TFWCurLn << L"Replace mode failed for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (strTest != strRepResult)
    {
        strmOut << TFWCurLn << L"Replace mode result was wrong for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



//
//  This helper method tests the error modes for externalization, of which
//  we really only throw and stop/throw.
//
tCIDLib::TBoolean
TTest_ErrModes::bTestBadTar(        TTextOutStream&         strmOut
                            ,       TTextConverter&         tcvtTest
                            , const TString&                strData
                            , const tCIDLib::TCard4         c4ErrIndex
                            , const tCIDLib::TCard4         c4BytesOut)
{
    const tCIDLib::TCard4 c4SrcLen = strData.c4Length();
    CIDAssert(c4ErrIndex < c4SrcLen, L"Error index is >= count");

    tCIDLib::TBoolean   bCaughtIt;
    tCIDLib::TCard4     c4ChDone;
    tCIDLib::TCard4     c4ByteCnt;
    THeapBuf            mbufData(1024);

    //
    //  First, set the mode to Throw mode, which sould fail without
    //  transcoding anything.
    //
    tcvtTest.eErrorAction(tCIDLib::ETCvtActions::Throw);
    bCaughtIt = kCIDLib::False;
    try
    {
        tcvtTest.c4ConvertTo(strData, mbufData, c4ByteCnt);
    }

    catch(...)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << TFWCurLn
                << L"Failed to catch invalid data in Throw mode for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }


    //
    //  Now test the StopThenThrow mode. We should decode c4ErrIndex
    //  characters and c4Bytes eaten the first time, then throw the
    //  second time.
    //
    tcvtTest.eErrorAction(tCIDLib::ETCvtActions::StopThenThrow);
    try
    {
        const tCIDLib::TCard4 c4Len = strData.c4Length();
        c4ChDone = tcvtTest.c4ConvertTo(strData, mbufData, c4ByteCnt);
    }

    catch(...)
    {
        strmOut << TFWCurLn << L"Stop/Throw mode failed for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (c4ChDone != c4ErrIndex)
    {
        strmOut << TFWCurLn << L"Stop/Throw mode stopped at wrong index for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    if (c4ByteCnt != c4BytesOut)
    {
        strmOut << TFWCurLn << L"Stop/Throw mode ate wrong src byte count for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    // Now do the second one, starting at the error index which should fail
    bCaughtIt = kCIDLib::False;
    try
    {
        c4ChDone = tcvtTest.c4ConvertTo
        (
            strData.pszBufferAt(c4ErrIndex)
            , c4SrcLen - c4ErrIndex
            , mbufData
            , c4ByteCnt
        );
    }

    catch(...)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << TFWCurLn
                << L"Stop/throw mode didn't fail on second pass for encoding: "
                << tcvtTest.strEncodingName()
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_RoundTrip1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_RoundTrip1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_RoundTrip1::TTest_RoundTrip1() :

    TTestFWTest
    (
        L"Round Trip 1"
        , L"Round trip conversion tests for mode 1 encodings"
        , 3
    )
{
}

TTest_RoundTrip1::~TTest_RoundTrip1()
{
}


// ---------------------------------------------------------------------------
//  TTest_RoundTrip1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_RoundTrip1::eRunTest( TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    TString strErr;

    // We run an internal test on each Mode1 derived encoding
    TMode1EncodingBase* atcvtEncodings[] =
    {
        new TEBCDIC037Converter
        , new TEBCDIC1140Converter
        , new TLatin3Converter
        , new TLatin4Converter
        , new TLatin5Converter
        , new TLatin6Converter
        , new TPC437Converter
        , new TPC850Converter
        , new TWin1251Converter
        , new TWin1252Converter

    };
    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(atcvtEncodings);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (!atcvtEncodings[c4Index]->bDoTests(strErr))
        {
            strmOut << TFWCurLn << strErr << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // Delete them as we go
        delete atcvtEncodings[c4Index];
    }
    return eRes;
}


