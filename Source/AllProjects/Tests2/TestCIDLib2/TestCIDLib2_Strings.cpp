//
// FILE NAME: TestCIDLib2_Strings.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/18/2008
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
//  This file contains tests related to the TString class. Note that the
//  fundamentals of strings are tested in the lower level (non-framework
//  based) tests, because this class is used in the test framework itself
//  and so we want to make sure it works before running the test framework.
//
//  So what we are doing here is higher level string functionality that the
//  test framework is not dependent on.
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
RTTIDecls(TTest_String1,TTestFWTest)
RTTIDecls(TTest_String2,TTestFWTest)
RTTIDecls(TTest_String3,TTestFWTest)
RTTIDecls(TTest_StringMove,TTestFWTest)
RTTIDecls(TTest_StringCat,TTestFWTest)


// A macro to repeatedly test certain fundamental things
#define CheckStrInvariants(str) \
{ \
    if (TRawStr::c4StrLen(str.pszBuffer()) != str.c4Length()) \
    { \
        strmOut << TFWCurLn << L"Null len is " << TRawStr::c4StrLen(str.pszBuffer()) \
                << L" but internal len is " << str.c4Length() << L"\n\n"; \
        eRes = tTestFWLib::ETestRes::Failed; \
    } \
    if (str.c4BufChars() < str.c4Length()) \
    { \
        strmOut << L"Buf chars is < current length\n\n"; \
        eRes = tTestFWLib::ETestRes::Failed; \
    } \
}


// ---------------------------------------------------------------------------
//  CLASS: TTest_String1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_String1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_String1::TTest_String1() :

    TTestFWTest
    (
        L"String Formatting", L"Tests formatting values to strings", 2
    )
{
}

TTest_String1::~TTest_String1()
{
}


// ---------------------------------------------------------------------------
//  TTest_String1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_String1::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TString str1;
    TString str2;

    // Format some fundamental data types into strings
    str1 = L"This is a test ";
    CheckStrInvariants(str1)
    str2 = L"of the emergency";
    CheckStrInvariants(str2)

    str1.Append(str2);
    CheckStrInvariants(str1)

    str1.Append(L" broadcasting system");
    CheckStrInvariants(str1)

    if (str1 != L"This is a test of the emergency broadcasting system")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"String into string insert failed\n\n";
    }

    str1.Clear();
    CheckStrInvariants(str1)
    str2 = L"234567";
    CheckStrInvariants(str2)

    str1.FormatToFld(str2, 8, tCIDLib::EHJustify::Center);
    CheckStrInvariants(str1)
    if (str1 != L" 234567 ")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format to field failed\n\n";
    }

    //
    //  Do some setting of formatted numerics into strings
    //
    str1.SetFormatted(tCIDLib::TCard1(1));
    if (str1 != L"1")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Card1 failed\n\n";
    }

    str1.SetFormatted(tCIDLib::TCard2(2));
    if (str1 != L"2")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Card2 failed\n\n";
    }

    str1.SetFormatted(tCIDLib::TCard4(4));
    if (str1 != L"4")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Card4 failed\n\n";
    }

    str1.SetFormatted(tCIDLib::TCard8(8));
    if (str1 != L"8")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Card8 failed\n\n";
    }

    // Default is 2 decimal places for floats
    str1.SetFormatted(4.4F);
    if (str1 != L"4.40")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Float4 failed\n\n";
    }

    str1.SetFormatted(4.4F, 1);
    if (str1 != L"4.4")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Float4 failed\n\n";
    }

    str1.SetFormatted(tCIDLib::TFloat4(4));
    if (str1 != L"4.00")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Float4 failed\n\n";
    }

    str1.SetFormatted(tCIDLib::TFloat8(8.8));
    if (str1 != L"8.80")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Float8 failed\n\n";
    }

    str1.SetFormatted(tCIDLib::TFloat4(8.8), 1);
    if (str1 != L"8.8")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Float8 failed\n\n";
    }

    str1.SetFormatted(tCIDLib::TFloat8(8));
    if (str1 != L"8.00")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Format of Float8 failed\n\n";
    }


    //
    //  Create a string stream to do some formatting via a stream
    //
    TString strToStream(64UL);
    CheckStrInvariants(strToStream)
    TTextStringOutStream   strmTest(&strToStream);

    strmTest << 123456UL << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"123456")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Raw 32 bit value format failed\n\n";
    }

    strmTest.Reset();
    CheckStrInvariants(strToStream)
    strmTest << TCardinal(0xFACE, tCIDLib::ERadices::Hex) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"FACE")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Hex TCardinal object format failed\n\n";
    }

    strmTest.Reset();
    strmTest << TInteger(-1024, tCIDLib::ERadices::Dec) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"-1024")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Negative integer format failed\n\n";
    }

    strmTest.Reset();
    strmTest << TCardinal(0xFF, tCIDLib::ERadices::Bin) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"11111111")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Binary cardinal format failed\n\n";
    }

    strmTest.Reset();
    strmTest << 999.999 << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"999.99")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Raw float format failed\n\n";
    }

    strmTest.Reset();
    strmTest << -999.99 << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"-999.99")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Raw negative float format failed\n\n";
    }

    //
    //  This one will just be '0' since the default trailing format for
    //  raw floats is to ignore trailing zeros.
    //
    strmTest.Reset();
    strmTest << 0.000001 << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"0")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Raw very small float format failed\n\n";
    }

    strmTest.Reset();
    strmTest << TFloat(11111.11111, 4) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"11111.1111")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Float object format failed" << kCIDLib::EndLn;
    }

    strmTest.Reset();
    strmTest << TFloat(10.0, 0) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"10")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Float object format failed: "
                << strToStream << L"\n\n";
    }

    strmTest.Reset();
    strmTest << TFloat(10.0) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"10.00")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Float object format failed\n\n";
    }

    strmTest.Reset();
    strmTest << TFloat(0.0) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"0.00")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Float object format failed\n\n";
    }

    strmTest.Reset();
    strmTest << TFloat(0.0, 6) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"0.000000")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Float object format failed\n\n";
    }

    strmTest.Reset();
    strmTest << TFloat(12345.01234567, 7) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"12345.0123456")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Float object format failed\n\n";
    }

    strmTest.Reset();
    strmTest << TFloat(1.1, 7, tCIDLib::ETrailFmts::Spaces) << kCIDLib::FlushIt;
    CheckStrInvariants(strToStream)
    if (strToStream != L"1.1      ")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Float object format failed\n\n";
    }

    // Test the lower/upper casing
    strmTest.Reset();
    strmTest << L"this is a lower case string" << kCIDLib::FlushIt;
    strToStream.ToUpper();
    CheckStrInvariants(strToStream)
    if (!TRawStr::bCompareStr(strToStream.pszBuffer(), L"THIS IS A LOWER CASE STRING"))
    {
        strmOut << TFWCurLn << L"ToUpper() failed\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    strmTest.Reset();
    strmTest << L"NOW DO THE OPPOSITE OPERATION AND LOWER CASE IT" << kCIDLib::FlushIt;
    strToStream.ToLower();
    CheckStrInvariants(strToStream)
    if (!TRawStr::bCompareStr(strToStream.pszBuffer()
                              , L"now do the opposite operation and lower case it"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"ToLower() failed\n\n";
    }

    //
    //  For variety we want to create a string that is full and do some
    //  stuff on it, to check for going over the end.
    //
    TString strCase(L"abcdefghijklmnop", 16);
    strCase.ToUpper(2,1);
    CheckStrInvariants(strCase)
    if (!TRawStr::bCompareStr(strCase.pszBuffer(), L"abCdefghijklmnop"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"ToUpper() failed on char 2 of string\n\n";
    }

    strCase.ToUpper(15,1);
    CheckStrInvariants(strCase)
    if (!TRawStr::bCompareStr(strCase.pszBuffer(), L"abCdefghijklmnoP"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"ToUpper() failed on last char\n\n";
    }

    strCase.ToLower();
    CheckStrInvariants(strCase)
    if (!TRawStr::bCompareStr(strCase.pszBuffer(), L"abcdefghijklmnop"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"ToLower() failed on full string\n\n";
    }

    str1 = (tCIDLib::TCh*)0;
    CheckStrInvariants(str1)
    if (str1 != kCIDLib::pszNullStr)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Assignment of null pointer failed\n\n";
    }

    str1.Clear();
    str1.Append((tCIDLib::TCh*)0);
    CheckStrInvariants(str1)
    if (str1 != kCIDLib::pszNullStr)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Append of null pointer failed\n\n";
    }

    str1.Clear();
    str1.FromZStr((tCIDLib::TCh*)0);
    CheckStrInvariants(str1)
    if (str1 != kCIDLib::pszNullStr)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"FromZStr of null pointer failed\n\n";
    }

    str1 = L"12";
    CheckStrInvariants(str1)
    str1.Insert((tCIDLib::TCh*)0, 1);
    CheckStrInvariants(str1)
    if (str1 != L"1<Null>2")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Insert of null pointer failed\n\n";
    }
    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_String2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_String2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_String2::TTest_String2() :

    TTestFWTest
    (
        L"String Tokenizer", L"Tests string tokenizer functions", 2
    )
{
}

TTest_String2::~TTest_String2()
{
}


// ---------------------------------------------------------------------------
//  TTest_String2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_String2::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test the parsing of comma separated quoted strings
    {
        const tCIDLib::TCh* pszErrText = L"Quoted comma list parse failed\n\n";
        const tCIDLib::TCh* pszShouldErr = L"Quoted comma list parse should have failed\n\n";
        tCIDLib::TBoolean   bRes;
        tCIDLib::TCard4     c4ErrIndex;
        TVector<TString>    colList;


        //
        //  Do some posistive tests
        //
        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"Value1\", \"Value2\", \"Value3\"", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 3))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErrText;
        }

        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"  \"Value1\"  ", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 1))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErrText;
        }

        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"Value1\", \"\", \"Value3\"", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 3))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErrText;
        }

        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\r\n\t\"Value1\",\r\n\t\"\", \"Value3\"\r\n\t", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 3))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErrText;
        }

        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            kCIDLib::pszEmptyZStr, colList, c4ErrIndex
        );

        if (!bRes || colList.c4ElemCount())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErrText;
        }


        // Do some special case, edge type scenarios

        // This one tests escaping of quotes inside a quoted value
        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"\\\"\"", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 1) || (colList[0] != L"\""))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErrText;
        }

        // And more embedded quotes
        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"\\\"\", \"\\\"\\\"\"", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 2)
        ||  (colList[0] != L"\"") || (colList[1] != L"\"\""))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErrText;
        }


        //
        //  Do some negative tests
        //
        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"Value1", colList, c4ErrIndex
        );

        if (bRes || (c4ErrIndex != 7))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszShouldErr;
        }

        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"Value1, ", colList, c4ErrIndex
        );

        if (bRes || (c4ErrIndex != 9))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszShouldErr;
        }

        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"Value1\", ", colList, c4ErrIndex
        );

        if (bRes || (c4ErrIndex != 10))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszShouldErr;
        }

        //
        //  It has a trailing slash, which makes the trailing quote
        //  a non-terminating quote, so it hits the end of the string
        //  without being closed.
        //
        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"\\\"", colList, c4ErrIndex
        );

        if (bRes || (c4ErrIndex != 3))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszShouldErr;
        }
    }


    //
    //  Test the parsing of the standard CSV format, which is comma separated
    //  but only things that have commas have to be quoted, other's don't
    //
    {
        tCIDLib::TBoolean   bRes;
        tCIDLib::TCard4     c4ErrIndex;
        TVector<TString>    colList;

        // Do some posistive tests
        bRes = TStringTokenizer::bParseCSVLine
        (
            L"\"Value1\", \"Value2\", \"Value3\"", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 3))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV line parse failed\n\n";
        }

        bRes = TStringTokenizer::bParseCSVLine
        (
            L"Value1, Value2, Value3, Value 4", colList, c4ErrIndex
        );

        if (!bRes
        ||  (colList.c4ElemCount() != 4)
        ||  (colList[0] != L"Value1")
        ||  (colList[1] != L"Value2")
        ||  (colList[2] != L"Value3")
        ||  (colList[3] != L"Value 4"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV line parse failed\n\n";
        }

        bRes = TStringTokenizer::bParseCSVLine
        (
            L"\"A,Value\", Value2, \"Another,Value\"", colList, c4ErrIndex
        );

        if (!bRes || (colList.c4ElemCount() != 3))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV line parse failed\n\n";
        }

        bRes = TStringTokenizer::bParseCSVLine
        (
            kCIDLib::pszEmptyZStr, colList, c4ErrIndex
        );

        if (!bRes || colList.c4ElemCount())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV line parse failed\n\n";
        }

        bRes = TStringTokenizer::bParseCSVLine
        (
            L"\t\n\r\"Another,Value\", \t\n\rValue2", colList, c4ErrIndex
        );

        if (!bRes
        ||  (colList.c4ElemCount() != 2)
        ||  (colList[0] != L"Another,Value")
        ||  (colList[1] != L"Value2"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV line parse failed\n\n";
        }

        bRes = TStringTokenizer::bParseCSVLine
        (
            L"1,,3,,5,", colList, c4ErrIndex
        );

        if (!bRes
        ||  (colList.c4ElemCount() != 6)
        ||  !colList[1].bIsEmpty()
        ||  !colList[3].bIsEmpty()
        ||  !colList[5].bIsEmpty())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV line parse failed\n\n";
        }


        // Do some negative tests
        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"Value1, ", colList, c4ErrIndex
        );

        if (bRes || (c4ErrIndex != 9))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV parse should have failed\n\n";
        }


        bRes = TStringTokenizer::bParseQuotedCommaList
        (
            L"\"Value1, ", colList, c4ErrIndex
        );

        if (bRes || (c4ErrIndex != 9))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"CSV parse should have failed\n\n";
        }
    }


    //
    //  Test the helper that builds up comma separated quoted string
    //  type content.
    //
    {
        const tCIDLib::TCh* const pszErr = L"Quoted comma list format failed\n\n";
        tCIDLib::TBoolean   bRes;
        tCIDLib::TCard4     c4ErrIndex;
        TVector<TString>    colList;
        TString             strTest;

        TStringTokenizer::BuildQuotedCommaList(L"Testing", strTest);
        TStringTokenizer::BuildQuotedCommaList(L"Testing 2", strTest);
        TStringTokenizer::BuildQuotedCommaList(L"Testing 3", strTest);

        bRes = TStringTokenizer::bParseQuotedCommaList(strTest, colList, c4ErrIndex);
        if (!bRes || (colList.c4ElemCount() != 3) || (colList[2] != L"Testing 3"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErr;
        }

        TStringTokenizer::BuildQuotedCommaList(L"\"", strTest);
        bRes = TStringTokenizer::bParseQuotedCommaList(strTest, colList, c4ErrIndex);
        if (!bRes || (colList.c4ElemCount() != 4) || (colList[3] != L"\""))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErr;
        }

        TStringTokenizer::BuildQuotedCommaList(L"\"\"\"", strTest);
        bRes = TStringTokenizer::bParseQuotedCommaList(strTest, colList, c4ErrIndex);
        if (!bRes || (colList.c4ElemCount() != 5) || (colList[4] != L"\"\"\""))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << pszErr;
        }
    }
    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_String3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_String3: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_String3::TTest_String3() :

    TTestFWTest
    (
        L"String Stripping/Replacing", L"Tests string stripping/replacing functions", 2
    )
{
}

TTest_String3::~TTest_String3()
{
}


// ---------------------------------------------------------------------------
//  TTest_String2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_String3::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    tCIDLib::TCard4 c4At;
    TString strTest;

    // A very basic test of the common leading/trailing whitespace strip
    strTest = L" 11233 ";
    strTest.StripWhitespace();
    if (strTest != L"11233")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Basic lead/trail whitespace strip failed\n";
    }


    // This should half the number of slashes
    strTest = L"//////";
    c4At = 0;
    if (!strTest.bReplaceSubStr(L"//", L"/", c4At, kCIDLib::True))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Substring replace found no matches\n";
    }

    if (strTest != L"///")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Substring replace did not half count\n";
    }

    // This should reduce all middle consequtive slashes to a single one
    strTest = L"///Test////Test2/Test3";
    strTest.Strip(L"/", tCIDLib::EStripModes::Full, kCIDLib::chForwardSlash);
    if (strTest != "Test/Test2/Test3")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Complete strip failed\n";
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_StringMove
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_StringMove: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_StringMove::TTest_StringMove() :

    TTestFWTest
    (
        L"String Move", L"Tests string move semantics", 2
    )
{
}

TTest_StringMove::~TTest_StringMove()
{
}


// ---------------------------------------------------------------------------
//  TTest_StringMove: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_StringMove::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    const tCIDLib::TCh* const pszTestStr1 = L"This is a test";

    TString strStart(pszTestStr1);
    TString strCopy(tCIDLib::ForceMove(strStart));

    if (strCopy != pszTestStr1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"String move cotr didn't set the new object\n";
    }

    if (strStart == pszTestStr1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"String move cotr didn't clear the old object\n";
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_StringCat
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_StringCat: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_StringCat::TTest_StringCat() :

    TTestFWTest
    (
        L"String Cat", L"Tests concatenator helper class", 2
    )
{
}

TTest_StringCat::~TTest_StringCat()
{
}


// ---------------------------------------------------------------------------
//  TTest_StringCat: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_StringCat::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    const TString strSrc1(L"ABC");
    const TString strSrc2(L"DEF");
    const TString strSrc3(L"GHI");
    const TString strSrc4(L"JKL");

    const tCIDLib::TCh* const pszSrc1(L"abc");
    const tCIDLib::TCh* const pszSrc2(L"def");

    const TString strTest1(TStrCat(strSrc1, strSrc2));
    if (strTest1 != L"ABCDEF")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"2 String cat helper failed\n";
    }

    const TString strTest2(TStrCat(strSrc1, strSrc2, strSrc3));
    if (strTest2 != L"ABCDEFGHI")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"3 String cat helper failed\n";
    }

    const TString strTest3(TStrCat(strSrc1, strSrc2, strSrc3, strSrc4));
    if (strTest3 != L"ABCDEFGHIJKL")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"4 String cat helper failed\n";
    }

    const TString strTest4(TStrCat(strSrc1, kCIDLib::chDollarSign));
    if (strTest4 != L"ABC$")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"String/TCh cat helper failed\n";
    }

    const TString strTest5(TStrCat(strSrc1, kCIDLib::chPeriod, strSrc2));
    if (strTest5 != L"ABC.DEF")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TString/TCh/TString cat helper failed\n";
    }

    const TString strTest6(TStrCat(strSrc1, pszSrc2));
    if (strTest6 != L"ABCdef")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"String/TCh* cat helper failed\n";
    }

    const TString strTest7(TStrCat(pszSrc1, strSrc2));
    if (strTest7 != L"abcDEF")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TCh*/String cat helper failed\n";
    }

    const TString strTest8(TStrCat(pszSrc1, pszSrc2));
    if (strTest8 != L"abcdef")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TCh*/TCh* cat helper failed\n";
    }

    const TString strTest9(TStrCat(pszSrc1, kCIDLib::chAmpersand, pszSrc2));
    if (strTest9 != L"abc&def")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TCh*/TCh/TCh* cat helper failed\n";
    }



    // Should handle nulls
    const TString strTest15(TStrCat(nullptr, strSrc1));
    if (strTest15 != L"ABC")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"null/TString cat helper failed\n";
    }

    const TString strTest16(TStrCat(strSrc2, nullptr));
    if (strTest16 != L"DEF")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TString/null cat helper failed\n";
    }

    const TString strTest17(TStrCat(nullptr, nullptr));
    if (!strTest17.bIsEmpty())
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"null/null cat helper failed\n";
    }


    // Should handle empty strings
    const TString strTest20(TStrCat(L"", pszSrc1));
    if (strTest20 != L"abc")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Empty/TCh* cat helper failed\n";
    }

    const TString strTest21(TStrCat(pszSrc2, L""));
    if (strTest21 != L"def")
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TCh*/Empty cat helper failed\n";
    }

    const TString strTest22(TStrCat(TString::strEmpty(), TString::strEmpty()));
    if (!strTest22.bIsEmpty())
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Empty/Empty cat helper failed\n";
    }

    return eRes;
}

