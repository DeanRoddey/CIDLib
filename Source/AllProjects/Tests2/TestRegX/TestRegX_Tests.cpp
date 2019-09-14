//
// FILE NAME: TestRegX_Tests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
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
#include    "TestRegX.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_Parse,TTestFWTest)
RTTIDecls(TTest_Match,TTestFWTest)
RTTIDecls(TTest_RepAll,TTestFWTest)



// ----------------------------------------------------------------------------
//  Local types
//
//  This is a structure used to set up lists of tests to run. It lets us
//  set up the pattern, the test string, whether it should or should not
//  match, where it should match, etc...
// ----------------------------------------------------------------------------
enum EMatchTypes { EPart, EFull };
struct TTestEntry
{
    tCIDLib::TCard1         c1ShouldMatch;
    tCIDLib::TCard1         c1OnlyAtStart;
    EMatchTypes             eType;
    tCIDLib::TCard4         c4StartAt;
    tCIDLib::TCard4         c4ShouldMatchAt;
    tCIDLib::TCard4         c4ExpectedLen;
    const tCIDLib::TCh*     pszPattern;
    const tCIDLib::TCh*     pszToSearch;
};


// ----------------------------------------------------------------------------
//  Local data
//
//  aTests
//      A list of test structures that drives the testing of matches.
// ----------------------------------------------------------------------------
static const TTestEntry aTests[] =
{
    //
    //  Start with simple single character patterns, which should indicate
    //  whether the algorithm is fully coherent. Do both full and partial
    //  match tests since they should both pass.
    //
    { 1 , 1, EFull  , 0 , 0 , 0, L"A"               , L"A" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A"               , L"B" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A"               , L"" }

  , { 1 , 1, EPart  , 0 , 0 , 1, L"A"               , L"A" }
  , { 0 , 1, EPart  , 0 , 0 , 1, L"A"               , L"B" }
  , { 1 , 1, EPart  , 0 , 0 , 1, L"A"               , L"AB" }

    //
    //  Ok, lets do some simple two character concatenation tests. Do the
    //  same tests again with them, though there are more combinations now.
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"AB"              , L"AB" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A"               , L"AB" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"B"               , L"AB" }

  , { 1 , 1, EPart  , 0 , 0 , 2, L"AB"              , L"AB" }
  , { 0 , 1, EPart  , 0 , 0 , 0, L"AB"              , L"A" }
  , { 0 , 1, EPart  , 0 , 0 , 0, L"AB"              , L"B" }
  , { 1 , 1, EPart  , 0 , 0 , 2, L"AB"              , L"ABC" }

    //
    //  And now lets do some simple two character OR tests.
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A|B"             , L"A" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A|B"             , L"B" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A|B"             , L"AB" }
  , { 1 , 1, EPart  , 0 , 0 , 1, L"A|B"             , L"AB" }
  , { 1 , 1, EPart  , 0 , 0 , 1, L"A|B"             , L"BD" }
  , { 0 , 1, EPart  , 0 , 0 , 0, L"A|B"             , L"Z" }

    //
    //  And now do some three character OR tests. Start testing some matches
    //  that start other than at the 0th offset.
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A|B|C"           , L"A" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A|B|C"           , L"B" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A|B|C"           , L"C" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A|B|C"           , L"AC" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A|B|C"           , L"BC" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A|B|C"           , L"D" }
  , { 1 , 1, EPart  , 0 , 0 , 1, L"A|B|C"           , L"AC" }
  , { 1 , 1, EPart  , 0 , 0 , 1, L"A|B|C"           , L"BC" }
  , { 1 , 0, EPart  , 0 , 1 , 1, L"A|B|C"           , L" A" }
  , { 0 , 1, EPart  , 0 , 0 , 0, L"A|B|C"           , L" A" }
  , { 1 , 0, EPart  , 0 , 3 , 1, L"A|B|C"           , L"123ADE" }
  , { 0 , 0, EFull  , 0 , 3 , 0, L"A|B|C"           , L"123ADE" }

    //
    //  Now lets do some tests for the zero or more repetition operation
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A*"              , L"" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A*"              , L"AAA" }
  , { 1 , 1, EPart  , 0 , 0 , 3, L"ABC*"            , L"ABC" }
  , { 1 , 1, EPart  , 0 , 0 , 5, L"ABC*"            , L"ABCCC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"ABC*"            , L"ABC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"ABC*"            , L"ABCCC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"ABC*"            , L"ABCCC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"ABC*\\$"         , L"ABCCC$" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"ABC*$"           , L"ABCCCD" }
  , { 1 , 1, EPart  , 0 , 0 ,10, L"AB(CD)*EF"       , L"ABCDCDCDEFZZZ" }
  , { 0 , 1, EPart  , 0 , 0 , 0, L"AB(CD)*EF$"      , L"ABCDCDCDEFZZZ" }
  , { 1 , 1, EFull  , 0 , 0 ,10, L"AB(CD)*EF"       , L"ABCDCDCDEF" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"[A-D]*"          , L"ABDCBADBA" }

    //
    //  And lets do some tests for the one or more repetition operation
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A+"              , L"AAA" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A+"              , L"" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"AB+"             , L"ABBB" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A+B+"            , L"AAABBB" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A+B+"            , L"AB" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"(AB)+"           , L"ABABABAB" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"(AB|CD)+"        , L"ABABCDABCD" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"(AB|CD)+"        , L"ABABCDBACD" }
  , { 1 , 0, EPart  , 0 , 3 , 6, L"(AB|CD)+"        , L"XYZABCDCDXYZ" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([URE])+"        , L"U" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([URE])+"        , L"R" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([URE])+"        , L"E" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"([URE])+"        , L"A" }

    //
    //  And lets do some tests for the one or zero repetition operation
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A?"              , L"A" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A?"              , L"" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"ABC?"            , L"ABC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"ABC?"            , L"AB" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"AB?C"            , L"AC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"AB?C"            , L"ABC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A(BCD)?E"        , L"AE" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A(BCD)?E"        , L"ABCDE" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A(BCD)?E"        , L"ABBDE" }
  , { 1 , 0, EPart  , 0 , 1 , 5, L"A(BCD)?E"        , L"XABCDEZ" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A([TRC])?E"      , L"ATE" }


    //
    //  Do some tests for the '.' character which matches any single
    //  character when not escaped. Do some where it is escaped as well
    //  to insure that the escapement works.
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A.C"             , L"AZC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A.C"             , L"A C" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A.C"             , L"A12C" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A..C"            , L"A12C" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"A\\.C"           , L"A1C" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"A\\.C"           , L"A.C" }

    //
    //  Now lets test some full matches with complex repetitions at the end
    //  of the pattern.
    //
  , { 1 , 1, EPart  , 0 , 0 , 3, L"AB(C|Z)*"        , L"ABC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"AB(C|Z)*"        , L"ABC" }
  , { 1 , 1, EPart  , 0 , 0 , 8, L"AB(C|Z)*"        , L"ABCZZZCC" }
  , { 1 , 0, EPart  , 0 , 4 , 8, L"AB(C|Z)*"        , L"0123ABCZZZCC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"AB(C|Z)*$"       , L"ABC" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"AB(C|Z)*"        , L"ABCZZZCC" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"AB(C|Z)*\\$"     , L"ABCZZZCC" }

    //
    //  And lets do some tests on character ranges and make sure that they
    //  act as expected.
    //
  , { 0 , 1, EPart  , 0 , 0 , 0, L"[0-9]"           , L"A" }
  , { 0 , 1, EPart  , 0 , 0 , 0, L"[1234567890]"    , L"A" }
  , { 1 , 1, EPart  , 0 , 0 , 1, L"[0-9]*"          , L"1" }
  , { 1 , 1, EPart  , 0 , 0 , 4, L"[0-9]*"          , L"1234" }
  , { 1 , 1, EPart  , 0 , 0 , 5, L"[0-9]*$"         , L"99999" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"[0-9]*"          , L"99%99" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"[^0-9]"          , L"A" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"[^0-9]"          , L"9" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"[0-2A-CM-P]*"    , L"1BN" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"[0-2A-CM-P]*"    , L"1DN" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"[1-9]|[1-6][0-9]", L"62" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"[1-9]|[1-6][0-9]", L"0" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"[1-6][0-9]"      , L"62" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([A-Za-z0-9][A-Za-z0-9][A-Za-z0-9][A-Za-z0-9] *)+[A-Za-z0-9][A-Za-z0-9][A-Za-z0-9][A-Za-z0-9]", L"0000 006E  0010 0010" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"sendir,%\\(m\\):%\\(c\\),%\\(i\\),[0-9]+,%\\(C\\),([0-9]+,)+[0-9]+", L"sendir,%(m):%(c),%(i),38000,%(C),7,7,35,3,77,3,1840,10,30,10,70,10,30,10,30,10,30,10,70,10,30,10,70,10,30,10,30,10,30,10,70,10,70,10,30,10,30,10,1740,10,30,10,70,10,30,10,30,10,30,10,30,10,70,10,30,10,70,10,70,10,70,10,30,10,30,10,70,10,70,10,1840" }


    //
    //  Try some multiply nested expressions, with ORs and whatnot, to see
    //  if it deals with them correctly.
    //
  , { 1 , 1, EFull  , 0 , 0 , 0, L"a((123|345)|(678|890))z", L"a345z" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"a((123|345)|(678|890))z", L"a678z" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"a((123|345)|(678|890))z", L"a123678z" }
  , { 1 , 1, EPart  , 0 , 0 , 4, L"a((123|345)|(678|890))", L"a123678z" }
  , { 1 , 1, EPart  , 0 , 0 , 5, L"a((123|345)|(678|890))z", L"a123z678" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"a((123|345)\\|(678|890))z", L"a123|890z" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"(1|2)3((4|5)|[^A-B])6", L"2356" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"(1|2)3((4|5)|[^A-B])6", L"13X6" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"(1|2)3((4|5)|[^A-B])6", L"23M6" }
  , { 0 , 1, EFull  , 0 , 0 , 0, L"(1|2)3((4|5)|[^A-B])6", L"234M6" }
  , { 1 , 0, EPart  , 0 , 2 , 4, L"(1|2)3((4|5)|[^A-B])6", L"MM23M699" }


  , { 1 , 1, EFull  , 0 , 0,  0, L"([1-9]+[ ]*,[ ]*)*[1-9]*[ ]*", L"123, 456, 789,111" }
  , { 1 , 1, EFull  , 0 , 0,  0, L"([1-9]+[ ]*,[ ]*)*[1-9]*[ ]*", L"123,456,789,111 " }
  , { 1 , 1, EFull  , 0 , 0,  0, L"([1-9]+[ ]*,[ ]*)*[1-9]*[ ]*", L"123" }
  , { 1 , 1, EFull  , 0 , 0,  0, L"([1-9]+[ ]*,[ ]*)*[1-9]*[ ]*", L"" }


    //  Do some special cases
  , { 1 , 1, EFull  , 0 , 0 , 5, L".*"              , L"12345" }
  , { 1 , 1, EFull  , 0 , 0 , 1, L"[ ]"             , L"A" }
  , { 1 , 1, EFull  , 0 , 0 , 6, L"[ ]*"            , L"ABCDE1" }
  , { 1 , 1, EPart  , 0 , 0 , 6, L"[ ]*"            , L"ABCDE1" }
  , { 1 , 1, EPart  , 0 , 0 , 3, L"A[ ]*C"          , L"ABC" }
  , { 0 , 1, EPart  , 0 , 0 , 7, L"@[ ]*="          , L"@BuildMode" }
//  , { 1 , 0, EFull  , 0 , 0 , 0, L"^A"              , L"A" }
//  , { 0 , 0, EPart  , 0 , 0 , 0, L"^A"              , L" A" }
  , { 1 , 0, EPart  , 0 , 1 , 1, L"A$"              , L" A" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L".*XYX.*"         , L"The XYX Boys" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L".*XYX"           , L"The XYX" }

  , { 0 , 0, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"1" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"12" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"123" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"1.9" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"12.8" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"123.7" }
  , { 1 , 0, EPart  , 0 , 1 , 5, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])?", L"A123.7B" }

  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])*", L"1" }
  , { 1 , 1, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])*", L"1.2" }
  , { 0 , 0, EFull  , 0 , 0 , 0, L"([0-9]|[0-9][0-9]|[0-9][0-9][0-9])(\\.[0-9])*", L"1.23" }


  , { 1 , 1, EFull  , 0 , 0 , 0, L"ARMED \\*+[A-Z]+\\*+ .*", L"ARMED ***AWAY***** ALL SECURE **" }
};
static const tCIDLib::TCard4 c4TestCount = tCIDLib::c4ArrayElems(aTests);




// ---------------------------------------------------------------------------
//  CLASS: TTest_Parse
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Parse: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Parse::TTest_Parse() :

    TTestFWTest
    (
        L"Expression Parsing", L"Tests the parsing of regular expressions", 2
    )
{
}

TTest_Parse::~TTest_Parse()
{
}


// ---------------------------------------------------------------------------
//  TTest_Parse: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Parse::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tCIDLib::TBoolean bCaughtIt;
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    TRegEx regxTest;

    //
    //  Make sure that we catch an attempt to match without having set a
    //  pattern yet.
    //
    bCaughtIt = kCIDLib::False;
    try
    {
        regxTest.bFullyMatches(L"Duh");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDRegX().strName(), kRegXErrs::errcRegEx_NoPattern))
        {
            bCaughtIt = kCIDLib::True;
        }
         else
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got the wrong exception" << kCIDLib::NewLn
                    << errToCatch << kCIDLib::DNewLn;
        }
    }

    if (!bCaughtIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch match before pattern set"
                << kCIDLib::NewLn;
    }

    // And make sure we catch setting an empty pattern
    bCaughtIt = kCIDLib::False;
    try
    {
        regxTest.SetExpression(L"");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDRegX().strName(), kRegXErrs::errcRegEx_EmptyPattern))
        {
            bCaughtIt = kCIDLib::True;
        }
         else
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got the wrong exception" << kCIDLib::NewLn
                    << errToCatch << kCIDLib::DNewLn;
        }
    }

    if (!bCaughtIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch match set of empty pattern"
                << kCIDLib::NewLn;
    }

    //
    //  Just set a bunch of different patterns and make sure that he can
    //  parse them all correctly.
    //
    regxTest.SetExpression(L"A");
    regxTest.SetExpression(L"1?");
    regxTest.SetExpression(L".*");
    regxTest.SetExpression(L"(AB)*");
    regxTest.SetExpression(L"AB");
    regxTest.SetExpression(L"A|B|C");
    regxTest.SetExpression(L"(A*B+AC)D");
    regxTest.SetExpression(L"[A-Z]");
    regxTest.SetExpression(L"1[2-5\\?\\|X-Z]B");
    regxTest.SetExpression(L"A*|(BCD|EF)*(Y|Z)");
    regxTest.SetExpression(L"([a-z0-9]|[A-Z])|C*");
    regxTest.SetExpression(L"a((123|345)|(678|890))z");

    // Test missing close parens
    bCaughtIt = kCIDLib::False;
    try
    {
        regxTest.SetExpression(L"((1|2)34");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDRegX().strName(), kRegXErrs::errcRegEx_CloseParen))
        {
            bCaughtIt = kCIDLib::True;
        }
         else
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got the wrong exception" << kCIDLib::NewLn
                    << errToCatch << kCIDLib::DNewLn;
        }
    }

    if (!bCaughtIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch missing paren exception"
                << kCIDLib::NewLn;
    }

    // Test mismatched close parens
    bCaughtIt = kCIDLib::False;
    try
    {
        regxTest.SetExpression(L"((1|2)34))");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDRegX().strName(), kRegXErrs::errcRegEx_ExpectedExpr))
        {
            bCaughtIt = kCIDLib::True;
        }
         else
        {
            strmOut << TFWCurLn << L"Got the wrong exception" << kCIDLib::NewLn
                    << errToCatch << kCIDLib::DNewLn;
        }
    }

    if (!bCaughtIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch mismatched paren exception"
                << kCIDLib::NewLn;
    }

    // Test escape at end of pattern
    bCaughtIt = kCIDLib::False;
    try
    {
        regxTest.SetExpression(L"((1|2)34)\\");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDRegX().strName(), kRegXErrs::errcRegEx_EscapeAtEOP))
        {
            bCaughtIt = kCIDLib::True;
        }
         else
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got the wrong exception" << kCIDLib::NewLn
                    << errToCatch << kCIDLib::DNewLn;
        }
    }

    if (!bCaughtIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch escape at end of pattern"
                << kCIDLib::NewLn;
    }

    // Test an empty character range
    bCaughtIt = kCIDLib::False;
    try
    {
        regxTest.SetExpression(L"[]");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDRegX().strName(), kRegXErrs::errcRegEx_EmptyRange))
        {
            bCaughtIt = kCIDLib::True;
        }
         else
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got the wrong exception" << kCIDLib::NewLn
                    << errToCatch << kCIDLib::DNewLn;
        }
    }

    if (!bCaughtIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch empty char range"
                << kCIDLib::NewLn;
    }

    // Test an invalid character range
    bCaughtIt = kCIDLib::False;
    try
    {
        regxTest.SetExpression(L"[2-1]");
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDRegX().strName(), kRegXErrs::errcRegEx_InvalidRange))
        {
            bCaughtIt = kCIDLib::True;
        }
         else
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Got the wrong exception" << kCIDLib::NewLn
                    << errToCatch << kCIDLib::DNewLn;
        }
    }

    if (!bCaughtIt)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Failed to catch invalid char range"
                << kCIDLib::NewLn;
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Match
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Match: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Match::TTest_Match() :

    TTestFWTest
    (
        L"Expression Matching", L"Runs numerous pattern matching tests", 2
    )
{
}

TTest_Match::~TTest_Match()
{
}


// ---------------------------------------------------------------------------
//  TTest_Match: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Match::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Run through each of the tests in the test list and do each one
    //  in turn. Actually we have three layers. The outer one runs all of
    //  the tests once in case sensitive mode and then once in case
    //  insensitive mode. The next one runs each test, and the final one
    //  decides what type of test each one is.
    //
    TRegEx regxTest;
    tCIDLib::TBoolean bCase = kCIDLib::True;
    for (tCIDLib::TCard4 c4CaseInd = 0; c4CaseInd < 2; c4CaseInd++)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            const TTestEntry& testCur = aTests[c4Index];

            regxTest.SetExpression(testCur.pszPattern);
            tCIDLib::TBoolean   bRes;
            tCIDLib::TCard4     c4Ofs;
            tCIDLib::TCard4     c4Len;
            if (testCur.eType == EPart)
            {
                //
                //  If there is a start index, then we are obviously doing a
                //  'match at', otherwise just do a 'match'.
                //
                c4Ofs = testCur.c4StartAt;
                if (testCur.c4StartAt)
                {
                    bRes = regxTest.bFindMatchAt
                    (
                        testCur.pszToSearch
                        , c4Ofs
                        , c4Len
                        , (testCur.c1OnlyAtStart == 1)
                        , bCase
                    );
                }
                 else
                {
                    bRes = regxTest.bFindMatch
                    (
                        testCur.pszToSearch
                        , c4Ofs
                        , c4Len
                        , (testCur.c1OnlyAtStart == 1)
                        , bCase
                    );
                }

                // Make sure it matched at the right offset
                if ((bRes == (testCur.c1ShouldMatch == 1))
                &&  (testCur.c4ShouldMatchAt != c4Ofs))
                {
                    eRes = tTestFWLib::ETestRes::Failed;
                    strmOut << L"Test (" << c4Index
                            << L" matched at wrong offset. Pattern: '"
                            << testCur.pszPattern << L"', Search: '"
                            << testCur.pszToSearch << L"'\n\n";
                }

                if (bRes
                &&  (testCur.c1ShouldMatch == 1)
                &&  (testCur.c4ExpectedLen != c4Len))
                {
                    eRes = tTestFWLib::ETestRes::Failed;
                    strmOut << L"Test (" << c4Index
                            << L" Partial match was wrong length. Pattern: '"
                            << testCur.pszPattern << L"', Search: '"
                            << testCur.pszToSearch << L"'\n\n";
                }
            }
             else if (testCur.eType == EFull)
            {
                //
                //  Its a full match so we need to call the method that
                //  checks for a full match of the whole target string.
                //
                bRes = regxTest.bFullyMatches(testCur.pszToSearch, bCase);
            }

            // Make sure it matched if it should have or not if not
            if (bRes != (testCur.c1ShouldMatch == 1))
            {
                eRes = tTestFWLib::ETestRes::Failed;

                if (!testCur.c1ShouldMatch)
                    strmOut << L"Not-";
                if (testCur.eType == EPart)
                    strmOut << L"Part-";
                else if (testCur.eType == EFull)
                    strmOut << L"Full-";
                else
                    strmOut << L"????-";

                strmOut << L"Test (" << c4Index << L") failed. Pattern: '"
                        << testCur.pszPattern << L"', Search: '"
                        << testCur.pszToSearch << L"'\n"
                        << kCIDLib::NewLn << L"NFA is:\n" << regxTest
                        << kCIDLib::DNewLn;
            }
        }

        // Flip the case flag
        bCase = kCIDLib::False;
    }
    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_RepAll
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_RepAll: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_RepAll::TTest_RepAll() :

    TTestFWTest
    (
        L"Replace All", L"Tests replacing all matches", 3
    )
{
}

TTest_RepAll::~TTest_RepAll()
{
}


// ---------------------------------------------------------------------------
//  TTest_RepAll: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_RepAll::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;


    TRegEx regxTest(L"A.");
    TString strTest("ABCABDABE");
    if (regxTest.bReplaceAll(strTest, L"X", kCIDLib::True))
    {
        if (strTest != L"XCXDXE")
        {
            strmOut << L"Replace all created wrong output"
                    << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }
     else
    {
        strmOut << L"Failed to change the string in a replace all"
                << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    regxTest.SetExpression(L"AB");
    strTest = L"ABABAB";
    if (regxTest.bReplaceAll(strTest, L"X", kCIDLib::True))
    {
        if (strTest != L"XXX")
        {
            strmOut << L"Replace all created wrong output"
                    << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }
     else
    {
        strmOut << L"Failed to change the string in a replace all"
                << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}


