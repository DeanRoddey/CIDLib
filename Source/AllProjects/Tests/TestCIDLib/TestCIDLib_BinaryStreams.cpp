//
// FILE NAME: TestCIDLib_BinaryStreams.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  binary stream classes.
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
//  Local functions
// -----------------------------------------------------------------------------

//
//  This method provides some common output tests for binary streams. Its
//  called with output streams based on various types of data sinks.
//
static tCIDLib::TBoolean
bCommonTests(       TTextOutStream&     strmOut
            ,       TBinOutStream&      strmTestOut
            ,       TBinInStream&       strmTestIn)
{
    // Both streams should report that they are open
    if (!strmTestOut.bIsOpen())
        strmOut << CUR_LN << L"    Reported as not open" << kCIDLib::EndLn;

    if (!strmTestIn.bIsOpen())
        strmOut << CUR_LN << L"    Reported as not open" << kCIDLib::EndLn;

    // Write some values out to the stream
    strmTestOut << tCIDLib::TCard1(1)
                << tCIDLib::TCard2(2)
                << tCIDLib::TCard4(4)
                << tCIDLib::TFloat4(4.4)
                << tCIDLib::TFloat8(8.8);

    // The current position should be 19 at this point, even though its not flushed
    if (strmTestOut.c8CurPos() != 19)
        strmOut << L"    " << CUR_LN << L"Current position is incorrect" << kCIDLib::EndLn;

    // Flush it and test again
    strmTestOut.Flush();
    if (strmTestOut.c8CurPos() != 19)
        strmOut << L"    " << CUR_LN << L"Flush changed out position" << kCIDLib::EndLn;

    // Reset the stream. This should set the position to 0, and truncate it
    strmTestOut.Reset();
    if (strmTestOut.c8CurPos() != 0)
        strmOut << L"    " << CUR_LN << L"Reset didn't zero out position" << kCIDLib::EndLn;
    if (strmTestIn.c8CurPos() != 0)
        strmOut << L"    " << CUR_LN << L"Reset didn't zero in position" << kCIDLib::EndLn;

    // Seek to the end, which should stay zero since it was truncated
    strmTestOut.SeekToEnd();
    if (strmTestOut.c8CurPos() != 0)
        strmOut << L"    Seek to end gives wrong position" << kCIDLib::EndLn;

    // Write an array out, and test the position again
    tCIDLib::TCard4 ac4Test[6] = { 10, 12, 14, 16, 18, 20 };
    strmTestOut.WriteArray(ac4Test, 6);
    if (strmTestOut.c8CurPos() != 24)
        strmOut << L"    " << CUR_LN << L"Array write caused bad position" << kCIDLib::EndLn;

    //
    //  Reset the output stream again and lets write out some data and read
    //  it back in via the input stream.
    //
    strmTestOut.Reset();
    strmTestOut << tCIDLib::TCard1(1)
                << tCIDLib::TCard2(2)
                << tCIDLib::TCard4(4)
                << tCIDLib::TCh(L'A')
                << tCIDLib::TFloat4(4.4)
                << tCIDLib::TFloat8(8.8);

    // Write some objects, and a class info object to the stream
    strmTestOut << TArea(6, 7, 8, 9) << TString(L"This is a test");
    strmTestOut.WriteClassInfo(TArea::clsThis());
    strmTestOut << kCIDLib::FlushIt;

    // Remember the current position to test latter on the input stream
    const tCIDLib::TCard8 c8Save = strmTestOut.c8CurPos();

    //
    //  Now lets try to stream that data back in and make sure we get it back
    //  as expected.
    //
    tCIDLib::TCard1     c1Test;
    tCIDLib::TCard2     c2Test;
    tCIDLib::TCard4     c4Test;
    tCIDLib::TCh        chTest;
    tCIDLib::TFloat4    f4Test;
    tCIDLib::TFloat8    f8Test;
    TClass              clsTest;
    TArea               areaTest;
    TString             strTest;

    // Test the values first
    strmTestIn  >> c1Test >> c2Test >> c4Test  >> chTest >> f4Test
                >> f8Test >> areaTest >> strTest;

    if ((c1Test != 1)
    ||  (c2Test != 2)
    ||  (c4Test != 4)
    ||  (chTest != L'A')
    ||  (f4Test != tCIDLib::TFloat4(4.4))
    ||  (f8Test != tCIDLib::TFloat8(8.8)))
    {
        strmOut << L"    " << CUR_LN << L"Values read back in were incorrect"
                << kCIDLib::EndLn;
    }

    if (areaTest != TArea(6, 7, 8, 9))
        strmOut << L"    " << CUR_LN << L"Area read back in was wrong" << kCIDLib::EndLn;

    if (strTest != TString(L"This is a test"))
        strmOut << L"    " << CUR_LN << L"String read back in was wrong" << kCIDLib::EndLn;

    // Read back in the class info
    clsTest = strmTestIn.clsReadClassInfo();
    if (clsTest != TArea::clsThis())
    {
        strmOut << L"    " << CUR_LN << L"Class info read back was incorrect"
                << kCIDLib::EndLn;
    }

    // Make sure that the position is where is should be
    if (strmTestIn.c8CurPos() != c8Save)
    {
        strmOut << L"    " << CUR_LN << L"In stream ended up at bad position"
                << kCIDLib::EndLn;
    }

    // And we should show up as at the end of stream now
    if (!strmTestIn.bEndOfStream())
    {
        strmOut << L"    " << CUR_LN << L"Should be at end of stream but isn't"
                << kCIDLib::EndLn;
    }
    return kCIDLib::True;
}


static tCIDLib::TVoid BasicStreamTests(TTextOutStream& strmOut)
{
    {
        //
        //  First, lets test a memory buffer based stream. So create an out
        //  stream and do the common tests on it.
        //
        strmOut << L"  Common tests, memory based" << kCIDLib::EndLn;
        TBinMBufOutStream strmTestOut(4096, 8192);
        TBinMBufInStream  strmTestIn(strmTestOut);

        if (!strmTestOut.bLittleEndian() || !strmTestIn.bLittleEndian())
            strmOut << CUR_LN << L"  Default endian wasn't little" << kCIDLib::EndLn;

        if (!bCommonTests(strmOut, strmTestOut, strmTestIn))
            strmOut << CUR_LN << L"  Common Memory based tests failed" << kCIDLib::EndLn;

        // Try a type specific truncation operation
        strmTestOut.TruncateAt(16);
        strmTestOut.SeekToEnd();
        if (strmTestOut.c8CurPos() != 16)
            strmOut << CUR_LN << L"  Truncate failed to create correct end" << kCIDLib::EndLn;

        // And test the TCard4 style position stuff
        if (strmTestOut.c4CurPos() != strmTestOut.c8CurPos())
            strmOut << CUR_LN << L"  Card4/FilePos versions of pos disagree" << kCIDLib::EndLn;

        // Check the size, which is the end of the data
        if (strmTestOut.c4CurSize() != 16)
            strmOut << CUR_LN << L"  Unexpected buffer size" << kCIDLib::EndLn;
    }

    {
        // Now do them for file based streams
        strmOut << L"  Common out tests, file based" << kCIDLib::EndLn;
        TBinFileOutStream strmTestOut
        (
            L"BinStreamTest.File"
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllOwnerAccess
            , tCIDLib::EFileFlags::None
        );
        TBinFileInStream  strmTestIn
        (
            L"BinStreamTest.File"
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::None
        );

        if (!bCommonTests(strmOut, strmTestOut, strmTestIn))
            strmOut << CUR_LN << L"  Common File based tests failed" << kCIDLib::EndLn;

        // Try a truncation operation
        strmTestOut.TruncateAt(16);
        strmTestOut.SeekToEnd();
        if (strmTestOut.c8CurPos() != 16)
            strmOut << CUR_LN << L"  Truncate failed to create correct end" << kCIDLib::EndLn;

        // Manually close one and test the close status
        strmTestOut.Close();
        if (strmTestOut.bIsOpen())
            strmOut << CUR_LN << L"  File stream showed open after close" << kCIDLib::EndLn;

        // We should be able to reopen it
        strmTestOut.Open
        (
            L"BinStreamTest.File"
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::None
        );

        if (!strmTestOut.bIsOpen())
            strmOut << CUR_LN << L"  Re-open failed to show as open" << kCIDLib::EndLn;
    }
}



// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the binary stream classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestBinStreams()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        BasicStreamTests(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" binary streams test" << kCIDLib::EndLn;
        throw;
    }
}
