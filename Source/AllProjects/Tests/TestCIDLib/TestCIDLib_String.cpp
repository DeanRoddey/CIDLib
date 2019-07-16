//
// FILE NAME: TestCIDLib_String.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/1992
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
//  This module is part of the TestCIDLib.Exe program. This module is called
//  from the program's main function. The functions in this module test the
//  string classes to make sure that they are functioning correctly.
//
// CAVEATS/GOTCHAS:
//
//  1>  Since the string class, as of relesae 1.2.0, now maintains the string
//      length internally (instead of just depending on it being null
//      terminated), we try to extensively test that the length always stays
//      in sync with the null terminated length. A macro is used to do this
//      check (as well as test some other invariants) after almost every op.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Facility specific includes
// ----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"


// ----------------------------------------------------------------------------
//  A macro for insuring that the length information in the string stays
//  correct.
// ----------------------------------------------------------------------------
#define CheckStrInvariants(str) \
{ \
    if (TRawStr::c4StrLen(str.pszBuffer()) != str.c4Length()) \
    { \
        strmOut << CUR_LN << L"Null len is " << TRawStr::c4StrLen(str.pszBuffer()) \
                << L" but internal len is " << str.c4Length() << kCIDLib::EndLn; \
    } \
    if (str.c4BufChars() < str.c4Length()) \
        strmOut << L"Buf chars is < current length" << kCIDLib::EndLn; \
}


// ----------------------------------------------------------------------------
//  Functions for local use
// ----------------------------------------------------------------------------

static tCIDLib::TVoid TestBufferGrowth(TTextOutStream& strmOut)
{
    TString strTmp(16UL);
    CheckStrInvariants(strTmp)

    if (strTmp.c4BufChars() != 16)
    {
        strmOut << CUR_LN << L"Buffer size not initially 16" << kCIDLib::EndLn;
        return;
    }

    // Append more than the initial allocation
    strTmp.Append(L"1234567890123456789012345678901234");
    CheckStrInvariants(strTmp)

    // Make sure it expanded correctly
    if (strTmp != L"1234567890123456789012345678901234")
    {
        strmOut << CUR_LN
                << L"Appended to string not equal to expected value" << kCIDLib::EndLn;
        return;
    }

    // Make sure the current length is 34
    if (strTmp.c4Length() != 34)
    {
        strmOut << CUR_LN << L"String buffer length was not 34 bytes << kCIDLib::EndLn";
        return;
    }
    CheckStrInvariants(strTmp);
}


static tCIDLib::TVoid TestConstructors(TTextOutStream& strmOut)
{
    TString strDefault;
    CheckStrInvariants(strDefault);
    if (strDefault.c4Length())
        strmOut << CUR_LN << L"Default ctor got wrong len" << kCIDLib::EndLn;

    TString strAdopt(TRawStr::pszReplicate(L"123456789012"), tCIDLib::EAdoptOpts::Adopt);
    CheckStrInvariants(strAdopt)

    TString strAdopt2(L"123456789012");
    CheckStrInvariants(strAdopt2)

    TString strCopyTest(L"12345678");
    CheckStrInvariants(strCopyTest)

    // Make sure the original got the right length
    if (strCopyTest.c4Length() != 8)
        strmOut << CUR_LN << L"Init from raw string got wrong len" << kCIDLib::EndLn;

    TString strTest(strCopyTest);
    CheckStrInvariants(strTest)

    // Make sure that the buffers are not the same address
    if (strCopyTest.pszBuffer() == strTest.pszBuffer())
    {
        strmOut << CUR_LN
                << L"Copy constructor from TString reused buffer" << kCIDLib::EndLn;
    }

    if (strTest != strCopyTest)
        strmOut << CUR_LN << L"Copy constructor from TString failed" << kCIDLib::EndLn;

    if (strTest.c4Length() != 8)
        strmOut << CUR_LN << L"Copy from TString set wrong length" << kCIDLib::EndLn;

    //
    //  Load up a couple of msg file based strings. In this case there are
    //  no replacement parameters, so we just provide the resource id and
    //  take the default values for the other parameters.
    //
    TString  strTest1(kTestMsgs::midTest1, facTestCIDLib);
    CheckStrInvariants(strTest1)
    if (strTest1 != L"This is test string 1")
        strmOut << CUR_LN << L"Msg file string 1 was not correct" << kCIDLib::EndLn;

    TString  strTest2(kTestMsgs::midTest2, facTestCIDLib);
    if (strTest2 != L"This is test string 2")
        strmOut << CUR_LN << L"Msg file string 2 was not correct" << kCIDLib::EndLn;

    //
    //  Load up a resource based string with replacement parameters and
    //  replace them. We have to tell it how much extra room we need,
    //  so give exactly what we need, as a test. If there's a problem,
    //  the string should clip and fail the comparision.
    //
    TString  strTest3
    (
        kTestMsgs::midTest3
        , facTestCIDLib
        , TString(L"test")
        , TCardinal(3)
    );
    CheckStrInvariants(strTest3)
    if (strTest3 != L"This is test string 3")
        strmOut << CUR_LN << L"Msg file string 3 was not correct" << kCIDLib::EndLn;

    //
    //  Test the constructor that supports the concatenation operator. It
    //  takes two raw strings and creates a string that holds the two
    //  strings concatenated together.
    //
    TString strConcat(TStrCat(L"This is ", L"a test string"));
    CheckStrInvariants(strConcat)
    if (strConcat != L"This is a test string")
    {
        strmOut << CUR_LN << L"Concatenation constructor gave incorrect value"
                << kCIDLib::EndLn;
    }

    // Test the constructor from a formattable object
    TString strFormatted(TArea(10,10,100,100));
    CheckStrInvariants(strFormatted)
    if (strFormatted != L"10,10,100,100")
    {
        strmOut << CUR_LN << L"Construct from formattable object failed"
                << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestCutNPaste(TTextOutStream& strmOut)
{
    TString   strCutPaste(L"0123456789ABCDEF");

    strCutPaste.Cut(0, 1);
    CheckStrInvariants(strCutPaste)
    strCutPaste.Cut(14, 1);
    CheckStrInvariants(strCutPaste)
    if (strCutPaste != L"123456789ABCDE")
        strmOut << CUR_LN << L"Cut on end failed" << kCIDLib::EndLn;

    strCutPaste.Cut(4, 4);
    CheckStrInvariants(strCutPaste)
    if (strCutPaste != L"12349ABCDE")
        strmOut << CUR_LN << L"Cut in middle failed" << kCIDLib::EndLn;

    strCutPaste.Insert(L"5678", 4);
    CheckStrInvariants(strCutPaste)
    if (strCutPaste != L"123456789ABCDE")
        strmOut << CUR_LN << L"Insert into middle failed" << kCIDLib::EndLn;

    strCutPaste.Prepend(L"0");
    CheckStrInvariants(strCutPaste)
    if (strCutPaste != L"0123456789ABCDE")
        strmOut << CUR_LN << L"Prepend failed" << kCIDLib::EndLn;

    strCutPaste.Insert(L"F-", 15);
    CheckStrInvariants(strCutPaste)
    if (strCutPaste != L"0123456789ABCDEF-")
        strmOut << CUR_LN << L"Insert at end failed" << kCIDLib::EndLn;

    TString strSrc(L"ABCDE");
    TString strDest(L"123");
    strDest.Prepend(strSrc);
    if (strDest != L"ABCDE123")
        strmOut << CUR_LN << L"Prepend failed" << kCIDLib::EndLn;

    strCutPaste = L"1234";
    strCutPaste.Cut(0, 4);

    strCutPaste = L"ESomeEnum_Value";
    strCutPaste.CutUpTo(L'_');
    if (strCutPaste != L"Value")
        strmOut << CUR_LN << L"CutUpTo failed" << kCIDLib::EndLn;

    strCutPaste = L"SomeValue_";
    strCutPaste.CutUpTo(L'_');
    if (!strCutPaste.bIsEmpty())
        strmOut << CUR_LN << L"CutUpTo failed" << kCIDLib::EndLn;

    strCutPaste = L"_SomeValue";
    strCutPaste.CutUpTo(L'_');
    if (strCutPaste != L"SomeValue")
        strmOut << CUR_LN << L"CutUpTo failed" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestOperators(TTextOutStream& strmOut)
{
    //
    //  Create one with an initial size of 0, which will force it to
    //  use the intial value string to create the initial size.
    //
    TString strTest(L"1234567890", tCIDLib::TCard4(0));
    CheckStrInvariants(strTest)

    //
    //  The buffer must be at least 10 chars since that was our initial
    //  value we passed in.
    //
    if (strTest.c4BufChars() < 10)
    {
        strmOut << CUR_LN
                << L"The init string was 10 chars long but the buffer "
                   L"size is only " << strTest.c4BufChars() << kCIDLib::EndLn;
        return;
    }

    //
    //  Test the indexing operator and make sure it catches errors in
    //  indexing.
    //
    if (strTest[0] != L'1')
    {
        strmOut << CUR_LN << L"The character at position 0 was "
                << TString(strTest[0]) << L" not 1" << kCIDLib::EndLn;
    }

    if (strTest[5] != L'6')
    {
        strmOut << CUR_LN << L"The character at position 5 was "
                << TString(strTest[5]) << L" not 6" << kCIDLib::EndLn;
    }

    if (strTest != L"1234567890")
    {
        strmOut << CUR_LN
                << L"The == operator for string to raw string failed" << kCIDLib::EndLn;
    }

    //
    //  Test the concatenation operator out. It creates two strings from a
    //  single string.
    //
    TString strConcat(TString(L"This is a") + TString(L" test string value"));
    CheckStrInvariants(strConcat)
    if (strConcat != L"This is a test string value")
    {
        strmOut << CUR_LN
                << L"Concatenation operator failed to create correct result"
                << kCIDLib::EndLn;
    }

    //
    //  Test some relative magnitude operations.
    //
    strTest = L"1234";
    TString strTest2 = L"12345";
    if (strTest > strTest2)
        strmOut << CUR_LN << L"1234 was > than 12345" << kCIDLib::EndLn;
    if (!(strTest < strTest2))
        strmOut << CUR_LN << L"1234 was > than 12345" << kCIDLib::EndLn;

    if (strTest > L"12345")
        strmOut << CUR_LN << L"1234 was > than 12345" << kCIDLib::EndLn;
    if (!(strTest < L"12345"))
        strmOut << CUR_LN << L"1234 was > than 12345" << kCIDLib::EndLn;

    if (strTest > L"1234")
        strmOut << CUR_LN << L"1234 was > than 1234" << kCIDLib::EndLn;
    if (strTest < L"1234")
        strmOut << CUR_LN << L"1234 was < than 1234" << kCIDLib::EndLn;

    strTest2 = L"1234";
    if (strTest > strTest2)
        strmOut << CUR_LN << L"1234 was > than 1234" << kCIDLib::EndLn;
    if (strTest < strTest2)
        strmOut << CUR_LN << L"1234 was < than 1234" << kCIDLib::EndLn;

    //
    //  Make sure that we catch reference beyond the current length.
    //
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        if (strTest[strTest.c4Length()])
        {
        }
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcGen_IndexError))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << L"Failed to get index error exception" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestPatternSearch(TTextOutStream& strmOut)
{
    //
    //  Create some strings in which we can search for various things.
    //
    //                        01234567890123456789012345
    const TString   strTest(L"0123456789 ABC DEF ABC 123");
    const TString   strTest2(L"ABC DEF ABC XYZ");
    CheckStrInvariants(strTest)
    CheckStrInvariants(strTest2)
    tCIDLib::TCard4 c4Index;

    //
    //  Check some prefix/suffix testing stuff
    //
    if (!strTest2.bStartsWith(L"ABC"))
        strmOut << CUR_LN << L"bStartsWith failed'" << kCIDLib::EndLn;

    if (strTest2.bStartsWith(L"abc"))
        strmOut << CUR_LN << L"bStartsWith gave false positive'" << kCIDLib::EndLn;

    if (!strTest2.bStartsWithI(L"abc"))
        strmOut << CUR_LN << L"bStartsWith failed'" << kCIDLib::EndLn;

    if (strTest2.bStartsWithI(L"abcE"))
        strmOut << CUR_LN << L"bStartsWith gave false positive'" << kCIDLib::EndLn;

    if (!strTest2.bEndsWith(L"XYZ"))
        strmOut << CUR_LN << L"bEndsWith failed'" << kCIDLib::EndLn;

    if (!strTest2.bEndsWithI(L"xyz"))
        strmOut << CUR_LN << L"bEndsWith failed'" << kCIDLib::EndLn;

    if (strTest2.bEndsWith(L"abc"))
        strmOut << CUR_LN << L"bEndsWith gave false positive'" << kCIDLib::EndLn;

    if (strTest2.bStartsWith(L""))
        strmOut << CUR_LN << L"bStartsWith matched empty string'" << kCIDLib::EndLn;

    if (strTest2.bEndsWith(L""))
        strmOut << CUR_LN << L"bEndsWith matched empty string'" << kCIDLib::EndLn;

    if (strTest2.bStartsWithI(L""))
        strmOut << CUR_LN << L"bStartsWithI matched empty string'" << kCIDLib::EndLn;

    if (strTest2.bEndsWithI(L""))
        strmOut << CUR_LN << L"bEndsWithI matched empty string'" << kCIDLib::EndLn;

    if (strTest2.bStartsWith(L"ABC DEF ABC 123  "))
        strmOut << CUR_LN << L"bStartsWith didn't catch too long prefix'" << kCIDLib::EndLn;

    if (strTest2.bEndsWith(L"ABC DEF ABC 123  "))
        strmOut << CUR_LN << L"bEndsWith didn't catch too long suffix'" << kCIDLib::EndLn;

    if (strTest2.bStartsWithI(L"ABC DEF ABC 123  "))
        strmOut << CUR_LN << L"bStartsWithI didn't catch too long prefix'" << kCIDLib::EndLn;

    if (strTest2.bEndsWithI(L"ABC DEF ABC 123  "))
        strmOut << CUR_LN << L"bEndsWithI didn't catch too long suffix'" << kCIDLib::EndLn;

    //
    //  Do some character seaching first. Lets find all of the '1' chars
    //  in the string.
    //
    if (!strTest.bFirstOccurrence(kCIDLib::chDigit1, c4Index))
    {
        strmOut << CUR_LN << L"Did not find first occurange of '1'" << kCIDLib::EndLn;
        return;
    }

    // It should have been at index 1
    if (c4Index != 1)
    {
        strmOut << CUR_LN << L"First occurrence of '1' was not at 1st index"
                << kCIDLib::EndLn;
        return;
    }

    // Find the next occurrence
    if (!strTest.bNextOccurrence(kCIDLib::chDigit1, c4Index))
    {
        strmOut << CUR_LN << L"Did not find next occurange of '1'" << kCIDLib::EndLn;
        return;
    }

    // Should have been at index 23
    if (c4Index != 23)
    {
        strmOut << CUR_LN << L"Next occurrence of '1' was not at 23rd index"
                << kCIDLib::EndLn;
        return;
    }

    // Now find the last occurence of 2
    if (!strTest.bLastOccurrence(kCIDLib::chDigit2, c4Index))
    {
        strmOut << CUR_LN << L"Did not find first occurrence of '2'" << kCIDLib::EndLn;
        return;
    }

    // Should have been at index 24
    if (c4Index != 24)
    {
        strmOut << CUR_LN << L"Last occurrence of '2' was not at 24rd index"
                << kCIDLib::EndLn;
        return;
    }

    // And find the previous instance
    if (!strTest.bPrevOccurrence(kCIDLib::chDigit2, c4Index))
    {
        strmOut << CUR_LN << L"Did not find previous occurrence of '2'"
                << kCIDLib::EndLn;
        return;
    }

    // Should have been at index 2
    if (c4Index != 2)
    {
        strmOut << CUR_LN << L"Previous occurrence of '2' was not at 2nd index"
                << kCIDLib::EndLn;
        return;
    }


    //
    //  Now do some substring searchs like the character searchs above.
    //  Start by finding the first occurrence of "ABC".
    //
    if (!strTest.bFirstOccurrence(L"ABC", c4Index))
    {
        strmOut << CUR_LN << L"Did not find first occurrence of 'ABC'"
                << kCIDLib::EndLn;
        return;
    }

    // It should have been at 11
    if (c4Index != 11)
    {
        strmOut << CUR_LN << L"First occurrence of 'ABC' was not at 11th index"
                << kCIDLib::EndLn;
        return;
    }

    if (!strTest.bNextOccurrence(L"ABC", c4Index))
    {
        strmOut << CUR_LN << L"Did not find next occurrence of 'ABC'" << kCIDLib::EndLn;
        return;
    }

    // It should have been at 19
    if (c4Index != 19)
    {
        strmOut << CUR_LN << L"Next occurrence of 'ABC' was not at 29th index"
                << kCIDLib::EndLn;
        return;
    }

    if (!strTest.bLastOccurrence(L"123", c4Index))
    {
        strmOut << CUR_LN << L"Did not find last occurrence of '123'" << kCIDLib::EndLn;
        return;
    }

    // It should have been at 23
    if (c4Index != 23)
    {
        strmOut << CUR_LN << L"Last occurrence of '123' was not at 23rd index"
                << kCIDLib::EndLn;
        return;
    }

    if (!strTest.bPrevOccurrence(L"123", c4Index))
    {
        strmOut << CUR_LN << L"Did not find previous occurrence of '123'"
                << kCIDLib::EndLn;
        return;
    }

    // It should have been at 1
    if (c4Index != 1)
    {
        strmOut << CUR_LN << L"Previous occurrence of '123' was not at 1st index"
                << kCIDLib::EndLn;
        return;
    }


    //
    //  Now do the same, but this time using the 'any char' flag that says find
    //  any char in the substring, not the whole substring.
    //
    if (!strTest.bFirstOccurrence(L"ABC", c4Index, kCIDLib::True))
    {
        strmOut << CUR_LN << L"Did not find first occurrence of char from 'ABC'"
                << kCIDLib::EndLn;
        return;
    }

    // It should have been at 11
    if (c4Index != 11)
    {
        strmOut << CUR_LN << L"First occurrence of 'A' was not at 11th index"
                << kCIDLib::EndLn;
        return;
    }

    c4Index = 25;
    if (!strTest.bPrevOccurrence(L"123", c4Index))
    {
        strmOut << CUR_LN << L"Did not find previous occurrence of char from '123'"
                << kCIDLib::EndLn;
        return;
    }

    // It should have been at 23
    if (c4Index != 23)
    {
        strmOut << CUR_LN << L"Previous occurrence of char from '123' was not at index 23"
                << kCIDLib::EndLn;
        return;
    }


    //
    //  Now do some tests to insure that we catch invalid index attempts.
    //
    tCIDLib::TBoolean   bCaughtIt;

    try
    {
        bCaughtIt = kCIDLib::False;
        c4Index = 50;
        strTest.bNextOccurrence(kCIDLib::chDigit4, c4Index);
    }

    catch(const TError&)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << CUR_LN << L"Invalid next occurrence index not caught"
                << kCIDLib::EndLn;
        return;
    }

    try
    {
        bCaughtIt = kCIDLib::False;
        c4Index = 50;
        strTest.bPrevOccurrence(kCIDLib::chDigit4, c4Index);
    }

    catch(const TError&)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << CUR_LN << L"Invalid previous occurrence index not caught"
                << kCIDLib::EndLn;
        return;
    }
}


static tCIDLib::TVoid TestStripping(TTextOutStream& strmOut)
{
    const tCIDLib::TCh* pszWhtSpace  = L" 1  2  3  \t4  5  6  ";

    TString str1(pszWhtSpace);
    CheckStrInvariants(str1)

    // First do the white space strip
    str1.StripWhitespace();
    CheckStrInvariants(str1)

    if (str1 != L"1  2  3  \t4  5  6")
        strmOut << CUR_LN << L"Leading/Trailing whitespace strip failed" << kCIDLib::EndLn;

    //
    //  Strip out all middle white space, starting with the previous
    //  string which has no leading/trailing white space (to test the
    //  boundary conditions.)
    //
    str1.StripWhitespace
    (
        tCIDLib::eOREnumBits
        (
            tCIDLib::EStripModes::Total
            , tCIDLib::EStripModes::Leading
            , tCIDLib::EStripModes::Trailing
            , tCIDLib::EStripModes::Middle
        )
    );
    CheckStrInvariants(str1)

    if (str1 != L"123456")
        strmOut << CUR_LN << L"Total whitespace strip failed" << kCIDLib::EndLn;


    // Do it again, starting with the original string
    str1 = pszWhtSpace;
    CheckStrInvariants(str1)
    str1.StripWhitespace
    (
        tCIDLib::eOREnumBits
        (
            tCIDLib::EStripModes::Total
            , tCIDLib::EStripModes::Leading
            , tCIDLib::EStripModes::Trailing
            , tCIDLib::EStripModes::Middle
        )
    );
    CheckStrInvariants(str1)
    if (str1 != L"123456")
        strmOut << CUR_LN << L"Total whitespace strip failed" << kCIDLib::EndLn;

    // Test small strings, which are handled specially inside the stripper
    str1 = L" ";
    CheckStrInvariants(str1)
    str1.StripWhitespace(tCIDLib::EStripModes::Leading);
    CheckStrInvariants(str1)
    if (str1 != kCIDLib::pszEmptyZStr)
        strmOut << CUR_LN << L"Strip of single space failed" << kCIDLib::EndLn;

    str1 = L"  \n";
    CheckStrInvariants(str1)
    str1.StripWhitespace(tCIDLib::EStripModes::Trailing);
    CheckStrInvariants(str1)
    if (str1 != kCIDLib::pszEmptyZStr)
        strmOut << CUR_LN << L"Strip of single space failed" << kCIDLib::EndLn;

    str1 = L"1 ";
    CheckStrInvariants(str1)
    str1.StripWhitespace(tCIDLib::EStripModes::Leading);
    CheckStrInvariants(str1)
    if (str1 != L"1 ")
        strmOut << CUR_LN << L"Strip of single space failed" << kCIDLib::EndLn;

    //
    //  Set up a string, strip certain characters out of it, and test the result.
    //
    str1 = L"aabbaaccaaddaaeeaaffaagg";
    CheckStrInvariants(str1)
    str1.Strip
    (
        L"a"
        , tCIDLib::eOREnumBits
          (
            tCIDLib::EStripModes::Total
            , tCIDLib::EStripModes::Leading
            , tCIDLib::EStripModes::Trailing
            , tCIDLib::EStripModes::Middle
          )
    );
    CheckStrInvariants(str1)
    if (str1 != L"bbccddeeffgg")
        strmOut << CUR_LN << L"Total strip failed" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestTokens(TTextOutStream& strmOut)
{
    TCardinal   cTmp1(0UL), cTmp2(0UL);
    TInteger    iTmp1(0L), iTmp2(0L);
    TString     str1(64UL);
    TString     str2(128UL);

    // Check a replacement of a token with an asciiz string
    str1 = L"This is a test of the %(a) system";
    str1.eReplaceToken(L"token replacement", L'a');
    CheckStrInvariants(str1)
    if (str1 != L"This is a test of the token replacement system")
        strmOut << CUR_LN << L"Asciiz token replacement failed" << kCIDLib::EndLn;

    // Check a replacement of a token with some string objects
    str1 = L"This is a %(1) test of the %(2) system";
    str2 = L"darned";
    str1.eReplaceToken(str2, L'1');
    CheckStrInvariants(str1)
    str2 = L"emergency broadcasting";
    str1.eReplaceToken(str2, L'2');
    CheckStrInvariants(str1)
    if (str1 != L"This is a darned test of the emergency broadcasting system")
        strmOut << CUR_LN << L"String object token replacement failed" << kCIDLib::EndLn;

    // Check a replacement of tokens with some numeric objects
    cTmp1 = 128;
    iTmp1 = -1;
    str1 = L"The value %(A) is less than the value %(B)";
    str1.eReplaceToken(iTmp1, L'A');
    CheckStrInvariants(str1)
    str1.eReplaceToken(cTmp1, L'B');
    CheckStrInvariants(str1)
    if (str1 != L"The value -1 is less than the value 128")
        strmOut << CUR_LN << L"Numeric object token replacement failed" << kCIDLib::EndLn;

    str1 = L"The value %(A,-4) is less than the value 0x%(B,-4)";
    CheckStrInvariants(str1)
    str1.eReplaceToken(-1L, L'A');
    CheckStrInvariants(str1)
    str1.eReplaceToken(128UL, L'B', tCIDLib::ERadices::Hex);
    CheckStrInvariants(str1)
    if (str1 != L"The value -1   is less than the value 0x80  ")
        strmOut << CUR_LN << L"Numeric object token replacement failed" << kCIDLib::EndLn;

    str1 = L"The value %(A,4) is less than the value %(B,4)";
    CheckStrInvariants(str1)
    str1.eReplaceToken(-1L, L'A');
    CheckStrInvariants(str1)
    str1.eReplaceToken(128UL, L'B');
    CheckStrInvariants(str1)
    if (str1 != L"The value   -1 is less than the value  128")
        strmOut << CUR_LN << L"Numeric object token replacement failed" << kCIDLib::EndLn;

    // Test the return value that indicates the status
    str1 = L"No tokens in this string";
    if (str1.eReplaceToken(0UL, L'1') != tCIDLib::EFindRes::NoMore)
        strmOut << CUR_LN << L"Expected no more return result" << kCIDLib::EndLn;

    str1 = L"Wrong token in this string %(A)";
    if (str1.eReplaceToken(0UL, L'1') != tCIDLib::EFindRes::NotFound)
        strmOut << CUR_LN << L"Expected not found result" << kCIDLib::EndLn;

    str1 = L"Right token in this string %(1)";
    if (str1.eReplaceToken(0UL, L'1') != tCIDLib::EFindRes::Found)
        strmOut << CUR_LN << L"Expected found result" << kCIDLib::EndLn;

    // Test the token scan
    str1 = L"%(1)%(2)%(A)%(A)%(3)";
    if (str1.bFindTokenList(str2))
    {
        if (str2 != L"12AA3")
            strmOut << CUR_LN << L"Got incorrect token list" << kCIDLib::EndLn;
    }
     else
    {
        strmOut << CUR_LN << L"Token list scan failed" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestTokenizer(TTextOutStream& strmOut)
{
    //
    //  Create a larger string to use as the source string, and a string with
    //  whitespace characters.
    //
    const TString strTest
    (
        L"Token1\tToken2   Token3\n Token4\r   \vToken5\n  "
    );
    CheckStrInvariants(strTest)

    const TString strWhitespace(kCIDLib::szWhitespace);
    CheckStrInvariants(strWhitespace)

    //  Ok, create a tokenizer for this string and start looking for tokens
    TStringTokenizer stokTest(&strTest, strWhitespace);
    TString          strToken;
    TString          strTmp;

    for (tCIDLib::TCard4 c4Token = 0; c4Token < 5; c4Token++)
    {
        if (!stokTest.bMoreTokens())
        {
            strmOut << CUR_LN << L"Should be more tokens available" << kCIDLib::EndLn;
            return;
        }

        // The text of the token should be this
        TString strExpected(TString(L"Token") + TString(TCardinal(c4Token+1)));

        // Make sure its idea of the token index matches ours
        if (stokTest.c4CurToken() != c4Token)
        {
            strmOut << CUR_LN << L"Expected token index " << c4Token
                    << L" but got " << stokTest.c4CurToken() << kCIDLib::EndLn;
            return;
        }

        // Get the token out
        if (!stokTest.bGetNextToken(strToken))
        {
            strmOut << CUR_LN << L"Should be more tokens available" << kCIDLib::EndLn;
            return;
        }

        // And now make sure we get the tokens we expect
        if (strToken != strExpected)
        {
            strmOut << CUR_LN << L"Did not get expected token" << kCIDLib::EndLn;
            return;
        }
    }

    // There should not be any more tokens
    if (stokTest.bMoreTokens())
        strmOut << CUR_LN << L"Should not be more tokens available" << kCIDLib::EndLn;

    if (stokTest.bGetNextToken(strToken))
        strmOut << CUR_LN << L"Should not be more tokens available" << kCIDLib::EndLn;

    // Reset the tokenizer and see if we get the first token
    stokTest.Reset();

    if (!stokTest.bMoreTokens())
        strmOut << CUR_LN << L"Reset did not make tokens available" << kCIDLib::EndLn;

    if (stokTest.c4CurToken() != 0)
    {
        strmOut << CUR_LN << L"Should have reset to 0th index" << kCIDLib::EndLn;
        return;
    }

    if (!stokTest.bGetNextToken(strToken))
        strmOut << CUR_LN << L"Expected 0th token after a Reset" << kCIDLib::EndLn;

    if (strToken != L"Token1")
        strmOut << CUR_LN << L"Expected 0th token after a Reset" << kCIDLib::EndLn;

    //
    //  Do one where we change the whitespace in the middle of a pass
    //  through a string.
    //
    const TString strTest2(L"Enum: , Val2");
    TStringTokenizer stokTest2(&strTest2, L": ");

    if (!stokTest2.bGetNextToken(strToken))
    {
        strmOut << CUR_LN << L"Should be more tokens available" << kCIDLib::EndLn;
        return;
    }

    if (strToken != L"Enum")
    {
        strmOut << CUR_LN << L"Did not get expected token" << kCIDLib::EndLn;
        return;
    }

    // Change the whitespace
    stokTest2.strWhitespace(L",");

    if (!stokTest2.bGetNextToken(strToken))
    {
        strmOut << CUR_LN << L"Should be more tokens available" << kCIDLib::EndLn;
        return;
    }

    if (!strToken.bIsEmpty())
    {
        strmOut << CUR_LN << L"Did not get expected token" << kCIDLib::EndLn;
        return;
    }

    if (!stokTest2.bGetNextToken(strToken))
    {
        strmOut << CUR_LN << L"Should be more tokens available" << kCIDLib::EndLn;
        return;
    }

    if (strToken != L" Val2")
    {
        strmOut << CUR_LN << L"Did not get expected token" << kCIDLib::EndLn;
        return;
    }

    // Test the split method
    strToken = L"Key = The Value";
    if (!strToken.bSplit(strTmp, kCIDLib::chEquals))
    {
        strmOut << CUR_LN << L"Split failed" << kCIDLib::EndLn;
        return;
    }
     else if ((strToken != L"Key") && (strTmp != L"The Value"))
    {
        strmOut << CUR_LN << L"Split returned wrong values" << kCIDLib::EndLn;
        return;
    }

    // Test the built in token extracter
    const TString strTExtSrc(L"One,Two,Three , Four");

    if (!strTExtSrc.bExtractNthToken(0, kCIDLib::chComma, strTmp)
    ||  (strTmp != L"One"))
    {
        strmOut << CUR_LN << L"Did not get expected token" << kCIDLib::EndLn;
        return;
    }

    if (!strTExtSrc.bExtractNthToken(3, kCIDLib::chComma, strTmp)
    ||  (strTmp != L"Four"))
    {
        strmOut << CUR_LN << L"Did not get expected token" << kCIDLib::EndLn;
        return;
    }

    if (!strTExtSrc.bExtractNthToken(2, kCIDLib::chComma, strTmp, kCIDLib::False)
    ||  (strTmp != L"Three "))
    {
        strmOut << CUR_LN << L"Did not get expected token" << kCIDLib::EndLn;
        return;
    }
}


static tCIDLib::TVoid TestSubString(TTextOutStream& strmOut)
{
    TString str1(L"123456");
    TString str2;

    str1.CopyOutSubStr(str2, 0, 6);
    if (str1 != str2)
        strmOut << CUR_LN << L"Copy out of full substring failed" << kCIDLib::EndLn;


    // Do some substring replacement
    str1 = L"One Two Three One Four One Five";
    str2 = L"One";
    tCIDLib::TCard4 c4Index = 1;
    if (!str1.bReplaceSubStr(str2, L"Nine", c4Index, kCIDLib::False))
    {
        strmOut << CUR_LN << L"Did not replace 2nd substring" << kCIDLib::EndLn;
        return;
    }

    if (str1 != L"One Two Three Nine Four One Five")
    {
        strmOut << CUR_LN << L"Substring replace was incorrect" << kCIDLib::EndLn;
        return;
    }

    if (!str1.bReplaceSubStr(str2, L"Nine", c4Index, kCIDLib::False))
    {
        strmOut << CUR_LN << L"Did not replace 3rd substring" << kCIDLib::EndLn;
        return;
    }

    if (str1 != L"One Two Three Nine Four Nine Five")
    {
        strmOut << CUR_LN << L"Substring replace was incorrect" << kCIDLib::EndLn;
        return;
    }

    if (str1.bReplaceSubStr(str2, L"Nine", c4Index, kCIDLib::False))
    {
        strmOut << CUR_LN << L"Should not have found another match" << kCIDLib::EndLn;
        return;
    }

    c4Index = 0;
    if (!str1.bReplaceSubStr(str2, L"Nine", c4Index, kCIDLib::False))
    {
        strmOut << CUR_LN << L"Did not replace 1st substring" << kCIDLib::EndLn;
        return;
    }

    if (str1 != L"Nine Two Three Nine Four Nine Five")
    {
        strmOut << CUR_LN << L"Substring replace was incorrect" << kCIDLib::EndLn;
        return;
    }

    if (str1.bReplaceSubStr(str2, L"Nine", c4Index, kCIDLib::False))
    {
        strmOut << CUR_LN << L"Should not have found another match" << kCIDLib::EndLn;
        return;
    }

    // Do another pass where we replace them all
    str1 = L"ABCDEABCMNABCYZ";
    str2 = L"ABC";
    c4Index = 0;
    if (!str1.bReplaceSubStr(str2, L"123", c4Index, kCIDLib::True))
    {
        strmOut << CUR_LN << L"Did not replace substrings" << kCIDLib::EndLn;
        return;
    }

    if (str1 != L"123DE123MN123YZ")
    {
        strmOut << CUR_LN << L"Substring replace was incorrect" << kCIDLib::EndLn;
        return;
    }

    // And again, replacing with an empty string
    str1 = L"ABCDEABCMNABCYZ";
    str2 = L"ABC";
    c4Index = 0;
    if (!str1.bReplaceSubStr(str2, L"", c4Index, kCIDLib::True))
    {
        strmOut << CUR_LN << L"Did not replace substrings" << kCIDLib::EndLn;
        return;
    }

    if (str1 != L"DEMNYZ")
    {
        strmOut << CUR_LN << L"Substring replace was incorrect" << kCIDLib::EndLn;
        return;
    }


    // Make sure we can do a single character length replacement
    str1 = L"2010-01-22T22:06:17.000Z";
    str2 = L":";
    c4Index = 1;
    if (!str1.bReplaceSubStr(str2, L"%3A", c4Index, kCIDLib::False))
    {
        strmOut << CUR_LN << L"Did not replace 2nd substring" << kCIDLib::EndLn;
        return;
    }

    if (str1 != L"2010-01-22T22%3A06:17.000Z")
    {
        strmOut << CUR_LN << L"Substring replace was incorrect" << kCIDLib::EndLn;
        return;
    }

    // Make sure a replacement string longer than the original works
    str1 = L"ABC";
    str2 = L"B";
    c4Index = 0;
    if (!str1.bReplaceSubStr(str2, L"123456789", c4Index, kCIDLib::True))
    {
        strmOut << CUR_LN << L"Did not replace substrings" << kCIDLib::EndLn;
        return;
    }

    if (str1 != L"A123456789C")
    {
        strmOut << CUR_LN << L"Substring replace was incorrect" << kCIDLib::EndLn;
        return;
    }

    // Make sure we catch the use of an index beyond the end
    tCIDLib::TBoolean   bCaughtIt;
    try
    {
        bCaughtIt = kCIDLib::False;
        str1 = L"ABCD";
        str2 = L"AB";
        c4Index = 5;
        str1.bReplaceSubStr(str2, L"", c4Index, kCIDLib::True);
    }

    catch(const TError&)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << CUR_LN << L"Invalid start index not caught" << kCIDLib::EndLn;
        return;
    }

    // But starting at the end (on the null) is ok, it should just fail
    try
    {
        str1 = L"ABCD";
        str2 = L"AB";
        c4Index = 4;
        if (str1.bReplaceSubStr(str2, L"XY", c4Index, kCIDLib::True))
        {
            strmOut << CUR_LN << L"Start at end shouldn't have worked" << kCIDLib::EndLn;
            return;
        }
    }

    catch(...)
    {
        strmOut << CUR_LN << L"Start index at end caused exception" << kCIDLib::EndLn;
        return;
    }
}


//
//  Streaming of strings is fundamental to so many things, so we need to make
//  sure this guy works correctly without fail. It converts to UTF-8 for
//  streaming purposes, so we want to create strings of many sizes, to test
//  for boundary conditions.
//
static tCIDLib::TVoid TestBinStreaming(TTextOutStream& strmOut)
{
    // Try streaming in and out a bunch of different length strings
    {
        TBinMBufOutStream strmTestOut(16000);
        TBinMBufInStream strmTestIn(strmTestOut);

        TString strSrc;
        TString strRead;
        tCIDLib::TCh chCur = 0x20;
        for (; chCur < 8192; chCur++)
        {
            strSrc.Append(chCur);

            strmTestOut.Reset();
            strmTestOut << strSrc << kCIDLib::FlushIt;

            strmTestIn.Reset();
            strmTestIn >> strRead;

            if (strSrc != strRead)
            {
                strmOut << CUR_LN << L"Binary string stream failed on char "
                        << tCIDLib::TCard4(chCur) << kCIDLib::EndLn;
                break;
            }
        }
    }
}


// ----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This method is one of the testing methods of the facility class. It tests
//  out all of the various capabilities of the TString class. This class is
//  very fundamental so it needs to be tested well.
//
tCIDLib::TVoid TFacTestCIDLib::TestStrings()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"constructor";
        TestConstructors(strmOut());

        pszCurTest = L"buffer growth";
        TestBufferGrowth(strmOut());

        pszCurTest = L"pattern search";
        TestPatternSearch(strmOut());

         pszCurTest = L"test opeators";
        TestOperators(strmOut());

        pszCurTest = L"cut/paste";
        TestCutNPaste(strmOut());

        pszCurTest = L"token";
        TestTokens(strmOut());

        pszCurTest = L"tokenizer";
        TestTokenizer(strmOut());

        pszCurTest = L"strip";
        TestStripping(strmOut());

        pszCurTest = L"substring";
        TestSubString(strmOut());

        pszCurTest = L"bin streaming";
        TestBinStreaming(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"An exception occured during the "
                    << pszCurTest
                    << L" test" << kCIDLib::EndLn;
        throw;
    }
}
