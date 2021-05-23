//
// FILE NAME: TestMacroEng_Tests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2007
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
//  This test program doesn't have a lot of tests, so we just implemennt them
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
#include    "TestMacroEng.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_PathValidation,TTestFWTest)
RTTIDecls(TTest_NumConstProbe,TTestFWTest)
RTTIDecls(TTest_CMLRuntime,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_PathValidation
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_PathValidation: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_PathValidation::TTest_PathValidation() :

    TTestFWTest(L"Path Validation", L"Tests the CML facility's path syntax checker", 4)
{
}

TTest_PathValidation::~TTest_PathValidation()
{
}


// ---------------------------------------------------------------------------
//  TTest_PathValidation: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_PathValidation::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Just set up a set of paths to test and whether they should pass or not
    //
    struct TestInfo
    {
        const tCIDLib::TCh* const   pszPath;
        tCIDLib::TBoolean           bShouldPass;
    };

    const TestInfo aTests[] =
    {
        // Some negative tests
        { L"MEng"           , kCIDLib::False }
      , { L"MEng."          , kCIDLib::False }
      , { L"MEng..Test"     , kCIDLib::False }
      , { L""               , kCIDLib::False }

      // Some positive tests
      , { L"MEng.Test"      , kCIDLib::True }
      , { L"MEng.T"         , kCIDLib::True }
      , { L"MEng.T.Test"    , kCIDLib::True }
      , { L"meng.test"      , kCIDLib::True }
    };
    const tCIDLib::TCard4 c4TestCnt = tCIDLib::c4ArrayElems(aTests);

    TString strErrText;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
    {
        const TestInfo& Info = aTests[c4Index];

        // For these say empty is not valid
        const tCIDLib::TBoolean bRes = facCIDMacroEng().bIsValidCMLPath
        (
            Info.pszPath, strErrText, kCIDLib::False
        );

        if (bRes && !Info.bShouldPass)
        {
            strmOut << L"Path '" << Info.pszPath << L"' should not have passed.\n    "
                    << L"   Error=" << strErrText << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
         else if (!bRes && Info.bShouldPass)
        {
            strmOut << L"Path '" << Info.pszPath << L"' should have passed.\n    "
                    << L"   Error=" << strErrText << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    // Do one where we say empty is OK and test an empty
    if (!facCIDMacroEng().bIsValidCMLPath(TString::strEmpty(), strErrText, kCIDLib::True))
    {
        strmOut << L"Empty path test failed\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_NumConstProbe
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_NumConstProbe: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_NumConstProbe::TTest_NumConstProbe() :

    TTestFWTest(L"Num Const Probe", L"Tests the CML parser's numeric constant parser", 5)
{
}

TTest_NumConstProbe::~TTest_NumConstProbe()
{
}


// ---------------------------------------------------------------------------
//  TTest_NumConstProbe: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_NumConstProbe::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Just set up a bunch of possible numeric constants and test that they pass
    //  the probe and return the correct type.
    //
    struct TestInfo
    {
        const tCIDLib::TCh* const   pszVal;
        tCIDMacroEng::ENumTypes     eExpType;
        tCIDLib::TInt4              i4Radix;
        tCIDLib::TBoolean           bExplicitSuff;
    };

    // These are positive tests and should pass
    {
        const TestInfo aPosTests[] =
        {
            { L"0."     , tCIDMacroEng::ENumTypes::Float8    , 10    , kCIDLib::False }
          , { L"0.1"    , tCIDMacroEng::ENumTypes::Float8    , 10    , kCIDLib::False }
          , { L"0"      , tCIDMacroEng::ENumTypes::Card4     , 10    , kCIDLib::False }
          , { L"0#C1"   , tCIDMacroEng::ENumTypes::Card1     , 10    , kCIDLib::True }
          , { L"1"      , tCIDMacroEng::ENumTypes::Card4     , 10    , kCIDLib::False }
          , { L"1.0"    , tCIDMacroEng::ENumTypes::Float8    , 10    , kCIDLib::False }
          , { L"1.0#F8" , tCIDMacroEng::ENumTypes::Float8    , 10    , kCIDLib::True }
          , { L"-4"     , tCIDMacroEng::ENumTypes::Int4      , 10    , kCIDLib::False }
          , { L"-4.4"   , tCIDMacroEng::ENumTypes::Float8    , 10    , kCIDLib::False }
          , { L"0xAE"   , tCIDMacroEng::ENumTypes::Card4     , 16    , kCIDLib::False }
          , { L"0xAE#C1", tCIDMacroEng::ENumTypes::Card1     , 16    , kCIDLib::True }
          , { L"0732"   , tCIDMacroEng::ENumTypes::Card4     , 8     , kCIDLib::False }
          , { L"-0"     , tCIDMacroEng::ENumTypes::Int4      , 10    , kCIDLib::False }
          , { L"+14912" , tCIDMacroEng::ENumTypes::Card4     , 10    , kCIDLib::False }

            //
            //  Suffix disagrees with actual type, but we are only checking syntax here!
            //  It may be rejected later or converted to the suffix type.
            //
          , { L"3.4#C1" , tCIDMacroEng::ENumTypes::Card1     , 10    , kCIDLib::True }
        };
        const tCIDLib::TCard4 c4PosTestCnt = tCIDLib::c4ArrayElems(aPosTests);

        TString strVal;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4PosTestCnt; c4Index++)
        {
            const TestInfo& Info = aPosTests[c4Index];

            tCIDLib::TBoolean   bWasExplicit;
            tCIDLib::ERadices   eGotRadix;

            // It can modify the value, removing any type suffix, so we have to put it into a temp
            strVal = Info.pszVal;
            const tCIDMacroEng::ENumTypes eType = TMacroEngParser::eCheckNumericLiteral
            (
                strVal, eGotRadix, bWasExplicit
            );

            if (eType == tCIDMacroEng::ENumTypes::None)
            {
                strmOut << L"Value '" << Info.pszVal << L"' was not a valid number\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (bWasExplicit != Info.bExplicitSuff)
            {
                strmOut << L"Value '" << Info.pszVal << L"' failed explicit suffic test\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (eGotRadix != tCIDLib::ERadices(Info.i4Radix))
            {
                strmOut << L"Value '" << Info.pszVal << L"' was the wrong radix\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }
    }


    // These are negative tests and should fail
    {
        const tCIDLib::TCh* apszTests[] =
        {
            L"x1A"          // Bad hex prefix
            , L"0x"         // No actual digits, just a hex prefix
            , L"4.5F"       // Bad suffix
            , L"4#F"        // Bad suffix
            , L"."          // Not a number at all
            , L".1"         // Must have leading digit for a float
            , L"048"        // 8 isn't a valid octal digit
            , L"A4F"        // Hex but no prefix
            , L"-"          // No digits
            , L"+"          // No digits
        };
        const tCIDLib::TCard4 c4NegTestCnt = tCIDLib::c4ArrayElems(apszTests);

        TString strVal;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4NegTestCnt; c4Index++)
        {
            tCIDLib::TBoolean   bWasExplicit;
            tCIDLib::ERadices   eGotRadix;

            // It can modify the value, removing any type suffix, so we have to put it into a temp
            strVal = apszTests[c4Index];
            const tCIDMacroEng::ENumTypes eType = TMacroEngParser::eCheckNumericLiteral
            (
                strVal, eGotRadix, bWasExplicit
            );

            if (eType != tCIDMacroEng::ENumTypes::None)
            {
                strmOut << L"Value '" << apszTests[c4Index] << L"' should have failed\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }
    }
    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_CMLRuntime
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_CMLRuntime: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_CMLRuntime::TTest_CMLRuntime() :

    TTestFWTest(L"CML Runtime", L"Tests the CML runtime classes", 6)
    , m_strmConsole(0x1000UL)
{
}

TTest_CMLRuntime::~TTest_CMLRuntime()
{
}


// ---------------------------------------------------------------------------
//  TTest_CMLRuntime: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_CMLRuntime::eRunTest( TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    tTestFWLib::ETestRes eOneRes;

    // Set up the path resolver we are going to use in our tests
    TPathStr pathLoad;
    TFileSys::QueryCurrentDir(pathLoad);
    pathLoad.AddLevel(L"Classes");
    m_mecmTest.strBasePath(pathLoad);

    // And set up the file resolver
    TFileSys::QueryCurrentDir(pathLoad);
    pathLoad.AddLevel(L"Files");
    m_mefrTest.strBasePath(pathLoad);

    //
    //  Set the approrpiate handlers on the macro engine. We set the parser
    //  and error handlers.
    //
    m_meehEngine.SetStream(&strmOut);
    m_meehParser.SetStream(&strmOut);
    m_meTest.SetErrHandler(&m_meehEngine);
    m_meTest.SetFileResolver(&m_mefrTest);

    //
    //  Set up an array of tests. We store the class path and any parameters
    //  that are to be passed to that test.
    //
    struct TTestInfo
    {
        const tCIDLib::TCh* pszPath;
        const tCIDLib::TCh* pszParms;
    };

    const TTestInfo aTests[] =
    {
        { L"MEng.User.Tests.Path-Name_Test", L"" }
      , { L"MEng.User.Tests.TestDerivedClass", L"" }
      , { L"MEng.User.Tests.TestOperators1", L"" }
      , { L"MEng.User.Tests.TestLocals1", L"" }
      , { L"MEng.User.Tests.TestBoolean1", L"" }
      , { L"MEng.User.Tests.TestCard1", L"" }
      , { L"MEng.User.Tests.TestCard2", L"" }
      , { L"MEng.User.Tests.TestCard4", L"" }
      , { L"MEng.User.Tests.TestCard8", L"" }
      , { L"MEng.User.Tests.TestFileSystem", L"" }
      , { L"MEng.User.Tests.TestInt1", L"" }
      , { L"MEng.User.Tests.TestInt2", L"" }
      , { L"MEng.User.Tests.TestInt4", L"" }
      , { L"MEng.User.Tests.TestFloat4", L"" }
      , { L"MEng.User.Tests.TestFloat8", L"" }
      , { L"MEng.User.Tests.TestEnum1", L"" }
      , { L"MEng.User.Tests.TestString1", L"" }
      , { L"MEng.User.Tests.TestString2", L"" }
      , { L"MEng.User.Tests.TestXCoder1", L"" }
      , { L"MEng.User.Tests.TestLiterals", L"" }
      , { L"MEng.User.Tests.TestFlow1", L"" }
      , { L"MEng.User.Tests.TestMemBuf", L"" }
      , { L"MEng.User.Tests.TestException1", L"" }
      , { L"MEng.User.Tests.TestTime", L"" }
      , { L"MEng.User.Tests.TestTextStreams", L"" }
      , { L"MEng.User.Tests.TestStringList", L"" }
      , { L"MEng.User.Tests.TestArray1", L"" }
      , { L"MEng.User.Tests.TestVector1", L"" }
      , { L"MEng.User.Tests.TestSysInfo1", L"" }
      , { L"MEng.User.Tests.TestTokenizer1", L"" }
      , { L"MEng.User.Tests.TestKVPair", L"" }
      , { L"MEng.User.Tests.TestNamedValMap", L"" }
      , { L"MEng.User.Tests.TestASCII", L"" }
      , { L"MEng.User.Tests.TestBase64", L"" }
      , { L"MEng.User.Tests.TestMD5Hash", L"" }
      , { L"MEng.User.Tests.TestRandomNum", L"" }
      , { L"MEng.User.Tests.TestDynType1", L"" }
//      , { L"MEng.User.Tests.TestSpeech", L"" }
      , { L"MEng.User.Tests.TestURL1", L"" }
      , { L"MEng.User.Tests.TestXML1", L"" }
      , { L"MEng.User.Tests.TestEPParms1", L"1" }
      , { L"MEng.User.Tests.TestEPParms2", L"1 2.3 -4 C 'Eat Me' ValP2" }
      , { L"MEng.User.Tests.TestJSONParser", L"" }
    };
    const tCIDLib::TCard4 c4TestCnt = tCIDLib::c4ArrayElems(aTests);

    // Turn on validation for these tests, to double check everything
    m_meTest.bValidation(kCIDLib::True);

    //
    //  We can't just set the passed output stream as the macro engine
    //  console, because he resets it before each run, to make sure no
    //  old stuff is left in it. That will trash anything that has been
    //  stored so far. So we have to use our own and then copy out the
    //  results to the caller's string on each round.
    //
    m_meTest.SetConsole(&m_strmConsole);

    // Ok run all the tests in the list
    TString strParms;
    TString strOutput;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
    {
        const TTestInfo& CurTest = aTests[c4Index];
        eOneRes = eRunTestMacro(strmOut, CurTest.pszPath, CurTest.pszParms);

        // Get out any console output
        m_strmConsole.Flush();
        strOutput = m_strmConsole.strData();
        strOutput.StripWhitespace();
        if (!strOutput.bIsEmpty())
            strmOut << strOutput << kCIDLib::EndLn;

        // If the result is worse than the current result, then return that
        if (eOneRes < eRes)
            eRes = eOneRes;
    }

    //
    //  Run a special one that tests conditional debug blocks. We run it once,
    //  and then turn on the debug mode on the engine and run it again.
    //
    strParms.Clear();
    eOneRes = eRunTestMacro(strmOut, L"MEng.User.Tests.TestCond1", strParms);
    if (eOneRes < eRes)
        eRes = eOneRes;

    m_meTest.bDebugMode(kCIDLib::True);
    eOneRes = eRunTestMacro(strmOut, L"MEng.User.Tests.TestCond1", strParms);
    if (eOneRes < eRes)
        eRes = eOneRes;

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_CMLRuntime: Private, non-virtual methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_CMLRuntime::eRunTestMacro(        TTextStringOutStream&   strmOut
                                , const TString&                strClassPath
                                , const TString&                strParms)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TMEngClassInfo*     pmeciMain;
    if (m_meprsTest.bParse( strClassPath
                            , pmeciMain
                            , &m_meTest
                            , &m_meehParser
                            , &m_mecmTest))
    {
        TMEngClassVal* pmecvTarget = pmeciMain->pmecvMakeStorage
        (
            L"$Main$", m_meTest, tCIDMacroEng::EConstTypes::NonConst
        );
        TJanitor<TMEngClassVal> janTarget(pmecvTarget);

        //
        //  Call its default constructor. If that worked, then run it.
        //  All of these tests return zero if no errors.
        //
        try
        {
            if (m_meTest.bInvokeDefCtor(*pmecvTarget, 0))
            {
                if (strParms.bIsEmpty())
                {
                    TCIDMacroEngine::TParmList colParms(tCIDLib::EAdoptOpts::Adopt);
                    if (m_meTest.i4Run(*pmecvTarget, colParms, 0) != 0)
                    {
                        strmOut << L"Macro '" << strClassPath << L"' returned non-zero\n";
                        eRes = tTestFWLib::ETestRes::Failed;
                    }
                }
                 else
                {
                    if (m_meTest.i4Run(*pmecvTarget, strParms, 0) != 0)
                    {
                        strmOut << L"Macro '" << strClassPath
                                << L"' returns non-zero\n\n";
                        eRes = tTestFWLib::ETestRes::Failed;
                    }
                }
            }
        }

        catch(const TExceptException&)
        {
            //
            //  This will already have been reported into the test output
            //  but we need to catch it separately to prevent it looking
            //  like a system exception to the framework.
            //
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }
     else
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << L"Macro '" << strClassPath << L"' failed to parse\n"
                << kCIDLib::EndLn;
    }
    return eRes;
}

