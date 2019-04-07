//
// FILE NAME: TestCIDLib_TextStreams.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/1997
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  text stream classes.
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


// -----------------------------------------------------------------------------
//  Local static data
// -----------------------------------------------------------------------------
static const tCIDLib::TCh* szTestText1 =
{
    L"1234567890"
};

static const tCIDLib::TCh* szTestText2 =
{
    L"Frapit, scand'lous moppet. Freezing wonder, plunders, blunders "
    L"and thunderous flop. Flowing poets, mowed low by bullets. Singing "
    L"rings scream, knowing no hits. Zits and flotsom, billious ransom "
    L"of kinds of minds, who crossed no transom of flowing bits of seasons "
    L"reasoned not by youth "
    L"Who reigns, games with sane names never lied to lowered truth."
};

static const tCIDLib::TCh* szTestText3 =
{
    L"This is line #1\n"
    L"This is line #2\n"
    L"1892.9821\n"
    L"10\n"
    L"False\n"
};




// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid CommonTests(  TTextOutStream&     strmOut
                                    , TTextInStream&    strmTestIn
                                    , TTextOutStream&   strmTestOut)
{
    TString strTmp;

    // Stream out a short text block
    strmTestOut << szTestText1 << L"\n";

    // Stream out a long text bock
    strmTestOut << szTestText2 << kCIDLib::NewLn;

    // Stream out some lines with various data in it
    strmTestOut << TString(szTestText3);

    // String out a null pointer, which should create a special text string
    strmTestOut << (tCIDLib::TCh*)0 << kCIDLib::NewLn;

    // And flush out the data
    strmTestOut.Flush();

    // Reset the input stream to prepare for reading in what we just wrote
    strmTestIn.Reset();

    // Stream in the first short text line
    strmTestIn.c4GetLine(strTmp);
    if (strTmp != szTestText1)
    {
        strmOut << CUR_LN << L"Streamed in text != to original text"
                << kCIDLib::EndLn;
        return;
    }

    //
    //  Stream a line of text in from the stream. This should get us back
    //  the initial long text block we streamed out.
    //
    strmTestIn.c4GetLine(strTmp);
    if (strTmp != szTestText2)
    {
        strmOut << CUR_LN << L"Streamed in text != to original text"
                << kCIDLib::EndLn;
        return;
    }

    //
    //  Now lets start streaming back in the test lines from the second
    //  block of strings. The first two are just text for comparison.
    //
    strmTestIn.c4GetLine(strTmp);
    if (strTmp != L"This is line #1")
    {
        strmOut << CUR_LN << L"Streamed in text != to original text"
                << kCIDLib::EndLn;
        return;
    }

    strmTestIn.c4GetLine(strTmp);
    if (strTmp != L"This is line #2")
    {
        strmOut << CUR_LN << L"Streamed in text != to original text"
                << kCIDLib::EndLn;
        return;
    }

    //
    //  And now do the subsequent ones, streaming them in as their binary
    //  forms and make sure they are correct.
    //
    tCIDLib::TBoolean   bTmp;
    tCIDLib::TFloat8    f8Tmp;
    tCIDLib::TInt4      i4Tmp;

    strmTestIn >> f8Tmp;
    if (f8Tmp != 1892.9821)
    {
        strmOut << CUR_LN << L"Streamed in value was: " << f8Tmp
                << L" but should have been 1892.9821" << kCIDLib::EndLn;
        return;
    }

    strmTestIn >> i4Tmp;
    if (i4Tmp != 10)
    {
        strmOut << CUR_LN << L"Streamed in value was: " << i4Tmp
                << L" but should have been 10" << kCIDLib::EndLn;
        return;
    }

    strmTestIn >> bTmp;
    if (bTmp)
    {
        strmOut << CUR_LN << L"Streamed in value was != False" << kCIDLib::EndLn;
        return;
    }

    // See if we get the magic null pointer representation next
    strmTestIn.c4GetLine(strTmp);
    if (strTmp != kCIDLib::pszNullStr)
    {
        strmOut << CUR_LN << L"Streamed null pointer != to magic value"
                << kCIDLib::EndLn;
        return;
    }

    //
    //  Test that we we show up as at the end of stream, and that we get a
    //  'read at end of stream' if we try to read more.
    //
    if (!strmTestIn.bEndOfStream())
    {
        strmOut << CUR_LN << L"Should be at the end of stream, but isn't"
                << kCIDLib::EndLn;
    }

    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        strmTestIn >> f8Tmp;
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcStrm_EndOfStream))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
            TModule::LogEventObj(errToCatch);
        }
    }

    if (!bGotIt)
    {
        strmOut << CUR_LN << L"Failed to get correct 'end of stream' error"
                << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid BasicStreamTests(TTextOutStream& strmOut)
{
    //
    //  We create text streams with each of the available implementations
    //  and then pass it to a common testing function. These should all
    //  create the same results.
    //
    {
        TTextFileOutStream strmTestOut
        (
            L"TestTextFileStream.Dat"
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::None
            , tCIDLib::EAccessModes::Write
        );

        TTextFileInStream strmTestIn
        (
            L"TestTextFileStream.Dat"
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllRead
            , tCIDLib::EFileFlags::None
            , tCIDLib::EAccessModes::Read
        );
        CommonTests(strmOut, strmTestIn, strmTestOut);
    }

    // Do the file again with a UTF-16 Little Endian converter this time
    {
        TTextFileOutStream strmTestOut
        (
            L"TestTextFileStream.Dat"
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::None
            , tCIDLib::EAccessModes::Write
            , new TUTFConverter(TUTFConverter::EEncodings::UTF16_LE)
        );

        TTextFileInStream strmTestIn
        (
            L"TestTextFileStream.Dat"
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllRead
            , tCIDLib::EFileFlags::None
            , tCIDLib::EAccessModes::Read
            , new TUTFConverter(TUTFConverter::EEncodings::UTF16_LE)
        );
        CommonTests(strmOut, strmTestIn, strmTestOut);
    }

    // Now try a memory buffer based text stream with default converter
    {
        THeapBuf mbufTmp(4096, 8192);
        TTextMBufOutStream strmTestOut
        (
            &mbufTmp
            , tCIDLib::EAdoptOpts::NoAdopt
        );

        TTextMBufInStream strmTestIn(strmTestOut);
        CommonTests(strmOut, strmTestIn, strmTestOut);
    }

    // And do it again with a UTF-16 Big Endian converter
    {
        THeapBuf mbufTmp(4096, 8192);
        TTextMBufOutStream strmTestOut
        (
            &mbufTmp
            , tCIDLib::EAdoptOpts::NoAdopt
            , new TUTFConverter(TUTFConverter::EEncodings::UTF16_BE)
        );

        TTextMBufInStream strmTestIn(strmTestOut);
        CommonTests(strmOut, strmTestIn, strmTestOut);
    }

    // And try a string based text stream
    {
        TString strTmp(1024UL);
        TTextStringOutStream strmTestOut(&strTmp);
        TTextStringInStream strmTestIn(strmTestOut);
        CommonTests(strmOut, strmTestIn, strmTestOut);
    }
}


static tCIDLib::TVoid NumFormatTests(TTextOutStream& strmOut)
{
    TTextStringOutStream strmTest(1024UL);

    // Test radix formatting via stream format objects
    TStreamFmt strmfDec(tCIDLib::ERadices::Dec);
    TStreamFmt strmfHex(tCIDLib::ERadices::Hex);

    strmTest << 15UL << strmfHex << 15UL << strmfDec << 17UL << strmfHex << 17UL
             << kCIDLib::FlushIt;

    if (strmTest.strData() != L"15F1711")
        strmOut << CUR_LN << L"Radix stream format failed" << kCIDLib::EndLn;


    // And do it using raw stream formatter objects
    const TTextOutStream::Radix sfmtDecRad(tCIDLib::ERadices::Dec);
    const TTextOutStream::Radix sfmtHexRad(tCIDLib::ERadices::Hex);
    strmTest.Reset();

    strmTest << sfmtDecRad << 18UL << sfmtHexRad << 13UL << sfmtDecRad
             << 17UL << sfmtHexRad << 17UL << kCIDLib::FlushIt;

    if (strmTest.strData() != L"18D1711")
        strmOut << CUR_LN << L"Radix stream format failed" << kCIDLib::EndLn;
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the text stream classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestTextStreams()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        BasicStreamTests(strmOut());

        pszCurTest = L"numeric formatting";
        NumFormatTests(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut() << L"Exception occured during the " << pszCurTest
                  << L" text streams test" << kCIDLib::EndLn;
        throw;
    }
}
