//
// FILE NAME: TestKernel_RawStrings.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/12/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module tests the raw string manipulation APIs.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestKernel.hpp"



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  Some helper methods for TestBinConversion(), to avoid a lot of redundant
//  typing.
//
static tCIDLib::TVoid
TestCard4Conversion(const   TCurLn&             curLn
                    , const tCIDLib::TCh* const pszToConvert
                    , const tCIDLib::TCard4     c4TargetValue
                    , const tCIDLib::TCard4     c4Radix
                    , const tCIDLib::TBoolean   bShouldFail
                    , const tCIDLib::TBoolean   bShouldMatch)
{
    tCIDLib::TBoolean   bIsValid;
    tCIDLib::TCard4     c4Res;

    c4Res = TRawStr::c4AsBinary
    (
        pszToConvert
        , bIsValid
        , tCIDLib::ERadices(c4Radix)
    );

    if (!bShouldFail)
    {
        if (!bIsValid)
            strmOut << curLn << L"TCard4 conversion reported bogus failure\n";
        else if ((c4Res != c4TargetValue) && bShouldMatch)
            strmOut << curLn << L"TCard4 conversion caused wrong binary value\n";
    }

    if (bShouldFail && bIsValid)
        strmOut << curLn << L"TCard4 conversion reported bogus success\n";
}


static tCIDLib::TVoid
TestInt4Conversion( const   TCurLn&             curLn
                    , const tCIDLib::TCh* const pszToConvert
                    , const tCIDLib::TInt4      i4TargetValue
                    , const tCIDLib::TCard4     c4Radix
                    , const tCIDLib::TBoolean   bShouldFail
                    , const tCIDLib::TBoolean   bShouldMatch)
{
    tCIDLib::TBoolean   bIsValid;
    tCIDLib::TInt4      i4Res;

    i4Res = TRawStr::i4AsBinary
    (
        pszToConvert
        , bIsValid
        , tCIDLib::ERadices(c4Radix)
    );

    if (!bShouldFail)
    {
        if (!bIsValid)
            strmOut << curLn << L"TInt4 conversion reported bogus failure\n";
        else if ((i4Res != i4TargetValue) && bShouldMatch)
            strmOut << curLn << L"TInt4 conversion caused wrong binary value\n";
    }

    if (bShouldFail && bIsValid)
        strmOut << curLn << L"TInt4 conversion reported bogus success\n";
}


//
//  Tests the most basic thing's that the rest of the tests depend upon to
//  correctly test what they deal with.
//
static tCIDLib::TVoid TestBasics()
{
    // Do some very basic tests to make sure the core stuff is healthy
    if (TRawStr::c4StrLen(L"") != 0)
        strmOut << CUR_LN << L"Zero length string length query failed\n";

    if (TRawStr::c4StrLen(L"1") != 1)
        strmOut << CUR_LN << L"One char string length query failed\n";

    tCIDLib::TCard1 c1Byte;
    if (!TRawStr::bMakeHexByte(L'A', L'4', c1Byte)
    ||  (c1Byte != 0xA4))
    {
        strmOut << CUR_LN << L"Make hex byte failed\n";
    }

    if (!TRawStr::bMakeHexByte(L'0', L'0', c1Byte)
    ||  (c1Byte != 0))
    {
        strmOut << CUR_LN << L"Make hex byte failed\n";
    }

    if (!TRawStr::bMakeHexByte(L'F', L'A', c1Byte)
    ||  (c1Byte != 0xFA))
    {
        strmOut << CUR_LN << L"Make hex byte failed\n";
    }

    if (TRawStr::bMakeHexByte(L'G', L'A', c1Byte))
        strmOut << CUR_LN << L"Make hex byte failed to catch bad char\n";

    if (TRawStr::bMakeHexByte(L'0', L'M', c1Byte))
        strmOut << CUR_LN << L"Make hex byte failed to catch bad char\n";
}


//
//  Tests the methods that convert text into binary numeric values.
//
static tCIDLib::TVoid TestBinConversion()
{
    // This one should work
    TestCard4Conversion(CUR_LN, L"123", 123, 10, kCIDLib::False, kCIDLib::True);

    // This one should fail to match
    TestCard4Conversion(CUR_LN, L"123", 124, 10, kCIDLib::False, kCIDLib::False);

    // This one should fail because its a negative number
    TestCard4Conversion(CUR_LN, L"-123", 0, 10, kCIDLib::True, kCIDLib::True);

    // This one should work because it uses a positive sign
    TestCard4Conversion(CUR_LN, L"+123", 123, 10, kCIDLib::False, kCIDLib::True);

    // This one should fail because of a bad character
    TestCard4Conversion(CUR_LN, L"12A3", 0, 10, kCIDLib::True, kCIDLib::True);

    // This one should fail because of a bad character even after whitespace
    TestCard4Conversion(CUR_LN, L"123  A", 0, 10, kCIDLib::True, kCIDLib::True);

    // This one should work despite leading/trailing whitespace
    TestCard4Conversion(CUR_LN, L"  1  ", 1, 10, kCIDLib::False, kCIDLib::True);

    // This one should work, testing out hex radix
    TestCard4Conversion(CUR_LN, L"0xFACE", 0xFACE, 16, kCIDLib::False, kCIDLib::True);

    // This one should fail, due to a bad hex digit
    TestCard4Conversion(CUR_LN, L"0xFACG", 0, 16, kCIDLib::True, kCIDLib::True);

    // This one should fail due to overflow
    TestCard4Conversion(CUR_LN, L"0xFFFFFFFFF", 0, 16, kCIDLib::True, kCIDLib::True);

    // These should work, testing out figuring out the radix itself
    TestCard4Conversion(CUR_LN, L"0xFACE", 0xFACE, 0, kCIDLib::False, kCIDLib::True);
    TestCard4Conversion(CUR_LN, L"010", 8, 0, kCIDLib::False, kCIDLib::True);

    // Should fail, cause it will think its octal
    TestCard4Conversion(CUR_LN, L"090", 0, 0, kCIDLib::True, kCIDLib::True);

    // Should fail to match
    TestCard4Conversion(CUR_LN, L"0x90", 0x91, 0, kCIDLib::False, kCIDLib::False);


    //
    //  Do some TInt4 versions. It uses the same code as the TCard4 one, so just
    //  test sign issues.
    //
    TestInt4Conversion(CUR_LN, L"-123", -123, 10, kCIDLib::False, kCIDLib::True);
    TestInt4Conversion(CUR_LN, L" -1 ", -1, 10, kCIDLib::False, kCIDLib::True);
    TestInt4Conversion(CUR_LN, L" !1 ", 0, 10, kCIDLib::True, kCIDLib::True);
    TestInt4Conversion(CUR_LN, L"-256", 256, 10, kCIDLib::False, kCIDLib::False);
    TestInt4Conversion(CUR_LN, L"-09", -9, 10, kCIDLib::False, kCIDLib::True);
}


static tCIDLib::TVoid TestConversions()
{
    const tCIDLib::TCh* const pszUNIString = L"This is a string";
    const tCIDLib::TSCh* const pszANSIString  = "This is a string";

    //
    //  Convert the Unicode string to an ANSI string and do some comparison
    //  tests on them.
    //
    tCIDLib::TSCh* pszANSITest = TRawStr::pszConvert(pszUNIString);

    if (strcmp(pszANSITest, pszANSIString))
        strmOut << CUR_LN << "Converted ANSI string != to original\n";

    //
    //  Change a byte of the test string and then compare them again.
    //  It should not be equal anymore.
    //
    pszANSITest[0] = '1';

    if (!strcmp(pszANSITest, pszANSIString))
        strmOut << CUR_LN << "Modified ANSI string still == to original\n";

    //
    //  Convert the ANSI string to the Unicode string and do some comparison
    //  tests on them.
    //
    tCIDLib::TCh* pszUNITest = TRawStr::pszConvert(pszANSIString);

    if (!TRawStr::bCompareStr(pszUNITest, pszUNIString))
        strmOut << CUR_LN << L"Converted Unicode string != to original\n";

    //
    //  Change a byte of the test string and then compare them again.
    //  It should not be equal anymore.
    //
    pszUNITest[0] = L'1';

    if (TRawStr::bCompareStr(pszUNITest, pszUNIString))
        strmOut << CUR_LN << L"Modified Unicode string still == to original\n";


    //
    //  Now test the versions that convert into a buffer. We test that they
    //  treat the buffer length correctly.
    //
    //  Put marks beyond the nul to test for overwrites. The conversion methods
    //  assume the bufer is one larger than the actual count, so that they can
    //  always null termiante. So we pass 4 as the max, which should terminate in
    //  in the 5th byte (offset 4), not touching the marker bytes.
    //
    tCIDLib::TCh  szUNIString[6];
    tCIDLib::TSCh szANSIString[6];
    szUNIString[5] = 0xFFFE;
    szANSIString[5] = 0x7E;

    // Convert the UNI to ANSI and test the results
    TRawStr::pszConvert(pszUNIString, szANSIString, 4);

    if (szANSIString[5] != 0x7E)
        strmOut << CUR_LN << L"ANSI target string was overwritten\n";

    if (szANSIString[4] != 0)
        strmOut << CUR_LN << L"ANSI target string was not terminated\n";

    if (strncmp(szANSIString, pszANSIString, 4))
        strmOut << CUR_LN << L"Target ANSI string != to start of original\n";

    // Now go the other way, from ANSI to UNI
    TRawStr::pszConvert(pszANSIString, szUNIString, 4);

    if (szUNIString[5] != 0xFFFE)
        strmOut << CUR_LN << L"Unicode target string was overwritten\n";

    if (szUNIString[4] != 0)
        strmOut << CUR_LN << L"Unicode target string was not terminated\n";

    if (!TRawStr::bCompareStrN(szUNIString, pszUNIString, 4))
        strmOut << CUR_LN << L"Target Unicode string != to start of original\n";


    delete [] pszUNITest;
}


static tCIDLib::TVoid TestFormatting()
{
    //
    //  Try to format each of the possible fundamental data types into a
    //  raw string.
    //
    tCIDLib::TZStr128   szTmp;

    // Try to format a string into the string, left justified
    if (!TRawStr::bFormatStr
    (
        L"This is a test"
        , szTmp
        , 15
        , L' '
        , tCIDLib::EHJustify::Left))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"This is a test "))
        strmOut << CUR_LN << L"Left justified string format failed\n";

    // Try to format a string into the string, right justified
    if (!TRawStr::bFormatStr
    (
        L"This is a test"
        , szTmp
        , 15
        , L' '
        , tCIDLib::EHJustify::Right))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L" This is a test"))
        strmOut << CUR_LN << L"Right justified string format failed\n";


    // Format all the cardinal types
    if (!TRawStr::bFormatVal(tCIDLib::TCard1(0xAC), szTmp, 128, tCIDLib::ERadices::Hex))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"AC"))
        strmOut << CUR_LN << L"TCard1 format failed\n";

    if (!TRawStr::bFormatVal(tCIDLib::TCard2(0xFACE), szTmp, 128, tCIDLib::ERadices::Hex))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"FACE"))
        strmOut << CUR_LN << L"TCard2 format failed\n";

    if (!TRawStr::bFormatVal(tCIDLib::TCard4(0xACDB9875), szTmp, 128, tCIDLib::ERadices::Hex))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"ACDB9875"))
        strmOut << CUR_LN << L"TCard4 format failed\n";


    // Do some octal radix stuff
    if (!TRawStr::bFormatVal(tCIDLib::TCard1(8), szTmp, 128, tCIDLib::ERadices::Oct))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"10"))
        strmOut << CUR_LN << L"TCard1 format failed for octal radix\n";

    if (!TRawStr::bFormatVal(tCIDLib::TCard1(64), szTmp, 128, tCIDLib::ERadices::Oct))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"100"))
        strmOut << CUR_LN << L"TCard1 format failed for octal radix\n";


    // Format all the integer types
    if (!TRawStr::bFormatVal(tCIDLib::TInt1(-1), szTmp, 128))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"-1"))
        strmOut << CUR_LN << L"TInt1 format failed\n";

    if (!TRawStr::bFormatVal(tCIDLib::TInt2(16135), szTmp, 128))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"16135"))
        strmOut << CUR_LN << L"TInt2 format failed\n";

    if (!TRawStr::bFormatVal(tCIDLib::TInt4(1931831), szTmp, 128))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"1931831"))
        strmOut << CUR_LN << L"TInt4 format failed\n";


    // Format the unsigned int type
    if (!TRawStr::bFormatVal(tCIDLib::TUInt(12345), szTmp, 128))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"12345"))
        strmOut << CUR_LN << L"TUInt format failed\n";

    if (!TRawStr::bFormatVal(tCIDLib::TUInt(128), szTmp, 128, tCIDLib::ERadices::Oct))
        throw TKrnlError::kerrLast();
    if (!TRawStr::bCompareStr(szTmp, L"200"))
        strmOut << CUR_LN << L"TUInt format failed\n";


    //
    //  Format the floating point types. Use a couple of different scenarios
    //  with the last digit of the ingoing value, to test for bad rounding.
    //
    //  We only need to test the TFloat8 version, since the TFloat4 version
    //  calls it internallly.
    //
    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(12345.9876)
        , szTmp
        , 3
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Spaces))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"12345.987"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(12345.9878)
        , szTmp
        , 3
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Spaces))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"12345.987"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(12345.987)
        , szTmp
        , 3
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Spaces))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"12345.987"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(12345.98)
        , szTmp
        , 3
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Spaces))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"12345.98 "))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(12345.98)
        , szTmp
        , 0
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Spaces))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"12345"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(0)
        , szTmp
        , 0
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Spaces))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"0"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(0)
        , szTmp
        , 2
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Zeroes))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"0.00"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(0.001)
        , szTmp
        , 4
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Zeroes))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"0.0010"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(0.001)
        , szTmp
        , 2
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Zeroes))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"0.00"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(-0.01455)
        , szTmp
        , 6
        , c4MaxBufChars(szTmp)
        , tCIDLib::ETrailFmts::Ignore))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"-0.01455"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(-0.01455)
        , szTmp
        , 5
        , 8
        , tCIDLib::ETrailFmts::Ignore))
    {
        throw TKrnlError::kerrLast();
    }
    if (!TRawStr::bCompareStr(szTmp, L"-0.01455"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(1234.567)
        , szTmp
        , 3
        , 10
        , tCIDLib::ETrailFmts::Zeroes
        , kCIDLib::chHyphenMinus
        , kCIDLib::chPeriod
        , 3
        , kCIDLib::chComma))
    {
        throw TKrnlError::kerrLast();
    }

    if (!TRawStr::bCompareStr(szTmp, L"1,234.567"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";


    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(-1234.567)
        , szTmp
        , 3
        , 10
        , tCIDLib::ETrailFmts::Zeroes
        , kCIDLib::chHyphenMinus
        , kCIDLib::chPeriod
        , 3
        , kCIDLib::chComma))
    {
        throw TKrnlError::kerrLast();
    }

    if (!TRawStr::bCompareStr(szTmp, L"-1,234.567"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(-12345.6789)
        , szTmp
        , 4
        , 12
        , tCIDLib::ETrailFmts::Zeroes
        , kCIDLib::chHyphenMinus
        , kCIDLib::chPeriod
        , 3
        , kCIDLib::chComma))
    {
        throw TKrnlError::kerrLast();
    }

    if (!TRawStr::bCompareStr(szTmp, L"-12,345.6789"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";


    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(12345)
        , szTmp
        , 4
        , 12
        , tCIDLib::ETrailFmts::Zeroes
        , kCIDLib::chHyphenMinus
        , kCIDLib::chPeriod
        , 3
        , kCIDLib::chComma))
    {
        throw TKrnlError::kerrLast();
    }

    if (!TRawStr::bCompareStr(szTmp, L"12,345.0000"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";

    if (!TRawStr::bFormatVal
    (
        tCIDLib::TFloat8(123)
        , szTmp
        , 2
        , 12
        , tCIDLib::ETrailFmts::Zeroes
        , kCIDLib::chHyphenMinus
        , kCIDLib::chPeriod
        , 3
        , kCIDLib::chComma))
    {
        throw TKrnlError::kerrLast();
    }

    if (!TRawStr::bCompareStr(szTmp, L"123.00"))
        strmOut << CUR_LN << L"TFloat8 format failed\n";
}

//
//  In all of these tests the memory debug alloc/delete stuff will catch
//  overflows of the string buffer as long as we use allocated strings then
//  delete them immediately afterwards. It allocates extra bytes and puts a
//  guard pattern at the end.
//
static tCIDLib::TVoid TestOverflows()
{
    //
    //  Test out the basic copy and cat operations to make sure that they
    //  don't have any boundary conditions. Do both the Unicode and the
    //  short character versions.
    //
    {
        // Do a copy and cat of a string same size as target
        tCIDLib::TCh* pszTest = new tCIDLib::TCh[9];
        TRawStr::CopyStr(pszTest, L"12345678");
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Copy did not result in len of 8\n";
        delete [] pszTest;

        pszTest = new tCIDLib::TCh[9];
        pszTest[0] = kCIDLib::chNull;
        TRawStr::CatStr(pszTest, L"12345678");
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Cat did not result in len of 8\n";
        delete [] pszTest;
    }

    {
        // Do a copy of a short char string
        tCIDLib::TSCh* pszTest = new tCIDLib::TSCh[9];
        TRawStr::CopyStr(pszTest, "12345678");
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Short char copy did not result in len of 8\n";
        delete [] pszTest;
    }

    {
        // Do a string longer than the target to insure clipping
        tCIDLib::TCh* pszTest = new tCIDLib::TCh[9];
        TRawStr::CopyStr(pszTest, L"12345678901234", 8);
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Copy did not result in len of 8\n";
        delete [] pszTest;

        pszTest = new tCIDLib::TCh[9];
        pszTest[0] = kCIDLib::chNull;
        TRawStr::CatStr(pszTest, L"12345678901234", 8);
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Cat did not result in len of 8\n";
        delete [] pszTest;
    }

    {
        // Do a copy then cat a string that should exactly fit
        tCIDLib::TCh* pszTest = new tCIDLib::TCh[9];
        TRawStr::CopyStr(pszTest, L"1234");
        TRawStr::CatStr(pszTest, L"5678");
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Copy and Cat did not result in len of 8\n";
        delete [] pszTest;
    }

    {
        // Do a copy then cat a string that should be clipped
        tCIDLib::TCh* pszTest = new tCIDLib::TCh[9];
        TRawStr::CopyStr(pszTest, L"1234", 8);
        TRawStr::CatStr(pszTest, L"5678901234", 8);
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Copy and Cat did not result in len of 8\n";
        delete [] pszTest;

        pszTest = new tCIDLib::TCh[9];
        TRawStr::CopyCatStr(pszTest, 8, L"1234", L"5678901234");
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"CopyCat did not result in len of 8\n";
        delete [] pszTest;

        pszTest = new tCIDLib::TCh[9];
        TRawStr::CopyCatStr(pszTest, 8, L"123", L"456", L"789");
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"CopyCat did not result in len of 8\n";
        delete [] pszTest;
    }

    {
        // Test the character fill operation
        tCIDLib::TCh* pszTest = new tCIDLib::TCh[9];
        TRawStr::FillString(pszTest, L'1', 8);
        if (TRawStr::c4StrLen(pszTest) != 8)
            strmOut << CUR_LN << L"Fill did not result in len of 8\n";
        delete [] pszTest;
    }


    //
    //  Check out the string formatting function (which is at the core of
    //  all of the other type formatting functions, so we can get a lot of
    //  mileage by just checking it.
    //
    {
        // Test a string string shorter than the target, right justified
        tCIDLib::TCh* pszTest = new tCIDLib::TCh[17];
        if (!TRawStr::bFormatStr
        (
            L"This is a test"
            , pszTest
            , 16
            , L' '
            , tCIDLib::EHJustify::Right))
        {
            throw TKrnlError::kerrLast();
        }
        if (TRawStr::c4StrLen(pszTest) != 16)
            strmOut << CUR_LN << L"Format did not result in len of 16\n";
        delete [] pszTest;

        // Test a string string shorter than the target, centered
        pszTest = new tCIDLib::TCh[17];
        if (!TRawStr::bFormatStr
        (
            L"This is a test"
            , pszTest
            , 16
            , L' '
            , tCIDLib::EHJustify::Center))
        {
            throw TKrnlError::kerrLast();
        }
        if (TRawStr::c4StrLen(pszTest) != 16)
            strmOut << CUR_LN << L"Format did not result in len of 16\n";
        delete [] pszTest;
    }

    {
        // Test a string longer than the target, right justified
        tCIDLib::TCh* pszTest = new tCIDLib::TCh[17];
        if (!TRawStr::bFormatStr
        (
            L"This is a test of a string longer than the target"
            , pszTest
            , 16
            , L' '
            , tCIDLib::EHJustify::Right))
        {
            throw TKrnlError::kerrLast();
        }
        if (TRawStr::c4StrLen(pszTest) != 16)
            strmOut << CUR_LN << L"Format did not result in len of 16\n";
        delete [] pszTest;

        // Test a string longer than the target, centered
        pszTest = new tCIDLib::TCh[17];
        if (!TRawStr::bFormatStr
        (
            L"This is a test of a string longer than the target"
            , pszTest
            , 16
            , L' '
            , tCIDLib::EHJustify::Center))
        {
            throw TKrnlError::kerrLast();
        }
        if (TRawStr::c4StrLen(pszTest) != 16)
            strmOut << CUR_LN << L"Format did not result in len of 16\n";
        delete [] pszTest;
    }
}


static tCIDLib::TVoid TestFindCharForward()
{
    // Set up a test string to do searches in
    const tCIDLib::TCh* const pszTest = L"This is a test string, 1, 2, 3";

    tCIDLib::TCard4     c4At;
    const tCIDLib::TCh* pszFind;

    // Do a search that should fail
    pszFind = TRawStr::pszFindChar(pszTest, L'W');
    if (pszFind)
    {
        strmOut << CUR_LN << L"Invalid match on char search\n";
        return;
    }

    // Do a set of forward finds that should get matches
    c4At = 0;
    pszFind = TRawStr::pszFindChar(pszTest, L'i', c4At);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find first char\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 2)
    {
        strmOut << CUR_LN << L"Did not find first char at correct index\n";
        return;
    }

    // And find the next match
    pszFind = TRawStr::pszFindChar(pszTest, L'i', c4At+1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find second char char\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 5)
    {
        strmOut << CUR_LN << L"Did not find second char at correct index\n";
        return;
    }

    // And the next match
    pszFind = TRawStr::pszFindChar(pszTest, L'i', c4At+1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find third char\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 18)
    {
        strmOut << CUR_LN << L"Did not find third char at correct index\n";
        return;
    }

    // And make sure we don't find another
    pszFind = TRawStr::pszFindChar(pszTest, L'i', c4At+1);
    if (pszFind)
    {
        strmOut << CUR_LN << L"Should not have found another forward match\n";
        return;
    }
}

 static tCIDLib::TVoid TestFindCharReverse()
{
    // Set up a test string to do searches in
    const tCIDLib::TCh* const pszTest = L"012345678901234567890123456";

    tCIDLib::TCard4     c4At;
    const tCIDLib::TCh* pszFind;

    // Do a search that should fail
    pszFind = TRawStr::pszFindLastChar(pszTest, L'A');
    if (pszFind)
    {
        strmOut << CUR_LN << L"Invalid match on last char search\n";
        return;
    }

    // Do a series of reverse finds that should get a match
    pszFind = TRawStr::pszFindLastChar(pszTest, L'6');
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find last char\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 26)
    {
        strmOut << CUR_LN << L"Did not find last char at correct index\n";
        return;
    }

    // And now find the previous char
    pszFind = TRawStr::pszFindPrevChar(pszTest, L'6', c4At-1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find first previous char\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 16)
    {
        strmOut << CUR_LN << L"Did not find first previous char at correct index\n";
        return;
    }

    // And now find another previous char
    pszFind = TRawStr::pszFindPrevChar(pszTest, L'6', c4At-1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find second previous char\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 6)
    {
        strmOut << CUR_LN << L"Did not find second previous char at correct index\n";
        return;
    }

    // And make sure we don't find another one
    pszFind = TRawStr::pszFindPrevChar(pszTest, L'6', c4At-1);
    if (pszFind)
    {
        strmOut << CUR_LN << L"Should not have found another previous char\n";
        return;
    }

    //
    //  Do a special test to make sure that the reverse search does not go
    //  beyond the origin. We make the 1st index the base and search for a '0'
    //  starting at the string length-1 (a common operation.) Any boundary
    //  condition that counts beyond the base would get caught here.
    //
    const tCIDLib::TCh* const pszTest2 = L"0123456789";
    pszFind = TRawStr::pszFindPrevChar(&pszTest2[1], L'0', TRawStr::c4StrLen(&pszTest2[1])-1);
    if (pszFind)
    {
        strmOut << CUR_LN << L"Backwards search went beyond string base\n";
        return;
    }

    //
    //  And check for a boundary condition that would fail to find the first
    //  char by not searching back far enough.
    //
    pszFind = TRawStr::pszFindPrevChar(pszTest2, L'0', TRawStr::c4StrLen(pszTest2)-1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Backwards search failed on 0th char\n";
        return;
    }
}


static tCIDLib::TVoid TestFindSubStrForward()
{
    // Set up a test string to do searches in
    const tCIDLib::TCh* const pszTest = L"012345678901234567890123456XYZ";

    tCIDLib::TCard4     c4At;
    const tCIDLib::TCh* pszFind;

    // Do a substring search that should fail
    pszFind = TRawStr::pszFindSubStr(pszTest, L"ABC");
    if (pszFind)
    {
        strmOut << CUR_LN << L"Invalid match on sub string search\n";
        return;
    }

    //
    //  Do a first substring search and make sure that it finds it at the
    //  correct place.
    //
    pszFind = TRawStr::pszFindSubStr(pszTest, L"123");
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find first substring\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 1)
    {
        strmOut << CUR_LN << L"Did not find first substring at correct index\n";
        return;
    }

    // Now find subsequent matches and check the index found at
    pszFind = TRawStr::pszFindSubStr(pszTest, L"123", c4At+1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find second substring\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 11)
    {
        strmOut << CUR_LN << L"Did not find second substring at correct index\n";
        return;
    }

    pszFind = TRawStr::pszFindSubStr(pszTest, L"123", c4At+1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find third substring\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 21)
    {
        strmOut << CUR_LN << L"Did not find third substring at correct index\n";
        return;
    }

    // And there should not be another match
    pszFind = TRawStr::pszFindSubStr(pszTest, L"123", c4At+1);
    if (pszFind)
    {
        strmOut << CUR_LN << L"Should not have found next substring\n";
        return;
    }

    // Test boundary condition of match on last chars of string
    pszFind = TRawStr::pszFindSubStr(pszTest, L"XYZ");
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find trailing substring\n";
        return;
    }

    //
    //  Search for a single character string. This takes an optimized
    //  path internally.
    //
    pszFind = TRawStr::pszFindSubStr(pszTest, L"X");
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find trailing one char substring\n";
        return;
    }
}

static tCIDLib::TVoid TestFindSubStrReverse()
{
    // Set up a test string to do searches in
    const tCIDLib::TCh* const pszTest = L"XYZ012345678901234567890123456";

    tCIDLib::TCard4     c4At;
    const tCIDLib::TCh* pszFind;

    // Do a substring search that should fail
    pszFind = TRawStr::pszFindLastSubStr(pszTest, L"ABC");
    if (pszFind)
    {
        strmOut << CUR_LN << L"Invalid match on last sub string search\n";
        return;
    }

    //  Now do a set of searches that should work and test the indexes
    pszFind = TRawStr::pszFindLastSubStr(pszTest, L"234");
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find last sub string\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 25)
    {
        strmOut << CUR_LN << L"Did not find last substring at correct index\n";
        return;
    }

    pszFind = TRawStr::pszFindPrevSubStr(pszTest, L"234", c4At-1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find second last sub string\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 15)
    {
        strmOut << CUR_LN << L"Did not find second last substring at correct index\n";
        return;
    }

    pszFind = TRawStr::pszFindPrevSubStr(pszTest, L"234", c4At-1);
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find second last sub string\n";
        return;
    }
    c4At = pszFind - pszTest;

    if (c4At != 5)
    {
        strmOut << CUR_LN << L"Did not find third last substring at correct index\n";
        return;
    }

    // And do the next one which should fail
    pszFind = TRawStr::pszFindPrevSubStr(pszTest, L"234", c4At-1);
    if (pszFind)
    {
        strmOut << CUR_LN << L"Should not have found another sub string\n";
        return;
    }

    //
    //  Do some boundary condition ones, where the string is at the ends of
    //  the search string.
    //
    pszFind = TRawStr::pszFindLastSubStr(pszTest, L"XYZ");
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Failed to find leading last sub string search\n";
        return;
    }
    if (pszFind != pszTest)
    {
        strmOut << CUR_LN << L"Last leading sub string not at correct index\n";
        return;
    }

    pszFind = TRawStr::pszFindLastSubStr(pszTest, L"456");
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Invalid match on trailing sub string search\n";
        return;
    }

    //
    //  Search for a single character string. This takes an optimized
    //  path internally.
    //
    pszFind = TRawStr::pszFindSubStr(pszTest, L"X");
    if (!pszFind)
    {
        strmOut << CUR_LN << L"Did not find one char substring\n";
        return;
    }

    if (pszFind != pszTest)
    {
        strmOut << CUR_LN << L"Single char sub string not at correct index\n";
        return;
    }
}



// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------

tCIDLib::TVoid TestRawStrings(const tCIDLib::TCard4)
{
    TestBasics();
    TestOverflows();
    TestFormatting();
    TestBinConversion();
    TestConversions();

    TestFindCharForward();
    TestFindCharReverse();
    TestFindSubStrForward();
}
