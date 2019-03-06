//
// FILE NAME: TestCIDLib_TextConverters.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the small
//  set of TextConverter derived classes that exist at the CIDLib level. Most
//  of them are in the CIDEncode facility, but some fundamental ones are
//  provided by CIDLib because he uses them.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"


// A pure ASCII string to test with
static const tCIDLib::TCh* const  apszPlainASCIIText =
    L"This file is part of a demonstration program of the CIDLib C++ "
    L"Frameworks. Its contents are distributed 'as is', to provide guidance on "
    L"the use of the CIDLib system. However, these demos are not intended to "
    L"represent a full fledged applications. Any direct use of demo code in "
    L"user applications is at the user's discretion, and no warranties are "
    L"implied as to its correctness or applicability.";



// -----------------------------------------------------------------------------
//  Local static functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid
BasicConvert(TTextOutStream& strmOut, TTextConverter& tcvtTarget)
{
    TExpByteBuf expbOut;
    TString     strIn;
    TString     strOut;

    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(apszPlainASCIIText);

    // Transcode our ASCII test string to the target encoding
    tCIDLib::TCard4 c4CharsEaten = tcvtTarget.c4ConvertTo
    (
        apszPlainASCIIText
        , c4SrcLen
        , expbOut
    );

    if (c4CharsEaten != c4SrcLen)
    {
        strmOut << CUR_LN << L"Didn't eat all source chars" << kCIDLib::EndLn;
        return;
    }

    // Convert it back out to our input string
    tCIDLib::TCard4 c4BytesEaten = tcvtTarget.c4ConvertFrom
    (
        expbOut.pc1Buffer()
        , expbOut.c4Bytes()
        , strIn
    );

    if (c4BytesEaten != expbOut.c4Bytes())
    {
        strmOut << CUR_LN << L"Didn't eat all source bytes" << kCIDLib::EndLn;
        return;
    }

    // See if our resulting string is equal to the original
    if (strIn != apszPlainASCIIText)
    {
        strmOut << CUR_LN << L"Didn't get back original text" << kCIDLib::EndLn;
        return;
    }

    // Now do the conversion from a string object
    c4CharsEaten = tcvtTarget.c4ConvertTo
    (
        strIn.pszBuffer()
        , strIn.c4Length()
        , expbOut
    );

    // Check the chars eaten again
    if (c4CharsEaten != c4SrcLen)
    {
        strmOut << CUR_LN << L"Didn't eat all source chars" << kCIDLib::EndLn;
        return;
    }

    // See if we got the same output bytes as last time
    if (expbOut.c4Bytes() != c4BytesEaten)
    {
        strmOut << CUR_LN << L"Got different cypher bytes this time" << kCIDLib::EndLn;
        return;
    }

    // And convert back again to make sure it went ok
    c4BytesEaten = tcvtTarget.c4ConvertFrom
    (
        expbOut.pc1Buffer()
        , expbOut.c4Bytes()
        , strOut
    );

    if (c4BytesEaten != expbOut.c4Bytes())
    {
        strmOut << CUR_LN << L"Didn't eat all source bytes" << kCIDLib::EndLn;
        return;
    }

    // And see if we got the same string again
    if (strIn != strOut)
    {
        strmOut << CUR_LN << L"Didn't get back original text" << kCIDLib::EndLn;
        return;
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the TextConverter collections.
//
tCIDLib::TVoid TFacTestCIDLib::TestTextConverters()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        // Do the basic converter tests for all the variants
        {
            pszCurTest = L"basic UTF8";
            TUTF8Converter tcvtUTF8;
            BasicConvert(strmOut(), tcvtUTF8);
        }

        {
            pszCurTest = L"basic UTT16LE";
            TUTF16LEConverter tcvtUTF16LE;
            BasicConvert(strmOut(), tcvtUTF16LE);
        }

        {
            pszCurTest = L"basic UTT16BE";
            TUTF16BEConverter tcvtUTF16BE;
            BasicConvert(strmOut(), tcvtUTF16BE);
        }

        {
            pszCurTest = L"basic USASCII";
            TUSASCIIConverter tcvtUSASCII;
            BasicConvert(strmOut(), tcvtUSASCII);
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" stack test" << kCIDLib::EndLn;
        throw;
    }
}


