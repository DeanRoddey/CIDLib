//
// FILE NAME: TestCIDLib_QStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/09/2002
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
//  Local functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid BasicQStreamTests(TTextOutStream& strmOut)
{
    TTextQOutStream strmTest(16, tCIDLib::EReadModes::NonDestructive);
    TVector<TString> colLines;
    tCIDLib::TCard4  c4MsgIndex = 0;

    // We should get zero lines this time
    if (strmTest.bQueryNewMsgs(c4MsgIndex, colLines))
    {
        strmOut << CUR_LN << L"Should not have gotten new lines" << kCIDLib::EndLn;
        return;
    }

    // Put a few elements into the stream
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < 4; c4Index++)
        strmTest << L"This is line #" << c4Index << L"\n";
    strmTest.Flush();

    if (strmTest.c4LineCount() != 4)
    {
        strmOut << CUR_LN
                << L"Expected 4 queue members" << kCIDLib::EndLn;
        return;
    }

    // We should get 4 lines this time
    colLines.RemoveAll();
    if (strmTest.bQueryNewMsgs(c4MsgIndex, colLines))
    {
        if (colLines.c4ElemCount() != 4)
        {
            strmOut << CUR_LN << L"Should have gotten 4 lines" << kCIDLib::EndLn;
            return;
        }
    }
     else
    {
        strmOut << CUR_LN << L"Should have gotten new lines" << kCIDLib::EndLn;
        return;
    }

    // Add 4 more lines and get them again
    for (; c4Index < 8; c4Index++)
        strmTest << L"This is line #" << c4Index << L"\n";
    strmTest.Flush();

    // We should get 4 more lines this time
    colLines.RemoveAll();
    if (strmTest.bQueryNewMsgs(c4MsgIndex, colLines))
    {
        if (colLines.c4ElemCount() != 4)
        {
            strmOut << CUR_LN << L"Should have gotten 4 lines" << kCIDLib::EndLn;
            return;
        }
    }
     else
    {
        strmOut << CUR_LN << L"Should have gotten new lines" << kCIDLib::EndLn;
        return;
    }

    // Continue adding up to the max value
    for (; c4Index < 16; c4Index++)
        strmTest << L"This is line #" << c4Index << L"\n";
    strmTest.Flush();

    if (strmTest.c4LineCount() != 16)
    {
        strmOut << CUR_LN
                << L"Expected 16 queue members" << kCIDLib::EndLn;
        return;
    }

    // Now go over the max, which should make it recycle elements
    for (; c4Index < 20; c4Index++)
        strmTest << L"This is line #" << c4Index << L"\n";
    strmTest.Flush();

    if (strmTest.c4LineCount() != 16)
    {
        strmOut << CUR_LN
                << L"Expected 16 queue members" << kCIDLib::EndLn;
        return;
    }

    // The overall message index should now be 20
    if (strmTest.c4CurMsgIndex() != 20)
    {
        strmOut << CUR_LN
                << L"Expected message index to be 20, but it was "
                << strmTest.c4CurMsgIndex() << kCIDLib::EndLn;
        return;
    }

    colLines.RemoveAll();
    if (strmTest.bQueryNewMsgs(c4MsgIndex, colLines))
    {
        if (colLines.c4ElemCount() != 12)
        {
            strmOut << CUR_LN << L"Should have gotten 12 lines" << kCIDLib::EndLn;
            return;
        }
    }
     else
    {
        strmOut << CUR_LN << L"Should have gotten new lines" << kCIDLib::EndLn;
        return;
    }
}


// ----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This method is one of the testing methods of the facility class. It tests
//  out the Q based text stream class.
//
tCIDLib::TVoid TFacTestCIDLib::TestQStreams()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        BasicQStreamTests(strmOut());

        // <TBD> Add a test for destructive read mode
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"An exception occured during the "
                    << pszCurTest
                    << L" QStream test" << kCIDLib::EndLn;
        throw;
    }
}


