//
// FILE NAME: TestKernel_File.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This module tests the TKrnlFile, TKrnlDirIter, and xxxx classes. These
//  are all file system oriented class so we do them all here.
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
static tCIDLib::TVoid BasicFileTest()
{
    // Create a file object and open it
    TKrnlFile   kflTest(L"Basic.TestFile");

    // Make sure the file name is correct
    if (!TRawStr::bCompareStr(kflTest.pszName(), L"Basic.TestFile"))
    {
        strmOut << CUR_LN
                << L"File not correct. It was: " << kflTest.pszName()
                << EndLn;
        return;
    }

    // Make sure the handle is 0
    if (kflTest.bIsValid())
    {
        strmOut << CUR_LN << L"Handle should not be valid yet\n";
        return;
    }

    if (!kflTest.bOpen
    (
        tCIDLib::EAccessModes::Excl_ReadWrite
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan))
    {
        strmOut << CUR_LN << L"Error opening file. "
                << TKrnlError::kerrLast() << EndLn;
    }

    //
    //  Write out a buffer to the file. We just create a buffer and
    //  set each value to its index for testing.
    //
    const tCIDLib::TCard4 c4TableSize = 4096;
    tCIDLib::TCard2 ac2ToWrite[c4TableSize];
    tCIDLib::TCard2 c2Cur;
    for (c2Cur = 0; c2Cur < c4TableSize; c2Cur++)
        ac2ToWrite[c2Cur] = c2Cur;

    // Now write it out
    tCIDLib::TCard4 c4Actual;
    if (!kflTest.bWriteBuffer(ac2ToWrite, sizeof(ac2ToWrite), c4Actual))
    {
        strmOut << CUR_LN << L"Error writing to file. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c4Actual != sizeof(ac2ToWrite))
    {
        strmOut << CUR_LN << L"Only wrote "
                << c4Actual << L" bytes to file\n";
    }

    // Check the current file position and see if its at the expected pos
    tCIDLib::TCard8 c8CurPos;
    if (!kflTest.bQueryFilePtr(c8CurPos))
    {
        strmOut << CUR_LN << L"Error getting file position. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c8CurPos != sizeof(ac2ToWrite))
    {
        strmOut << CUR_LN
                << L"Current position was "
                << tCIDLib::TInt4(c8CurPos) << EndLn;
    }

    // Seek back to the start and check the position again
    if (!kflTest.bSetFilePointer(0))
    {
        strmOut << CUR_LN << L"Error seeking back to 0. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (!kflTest.bQueryFilePtr(c8CurPos))
    {
        strmOut << CUR_LN << L"Error getting file position. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c8CurPos != 0)
    {
        strmOut << CUR_LN
                << L"Current position was "
                << tCIDLib::TInt4(c8CurPos) << EndLn;
    }

    //
    //  Zero out the buffer and read back into again now. Check that we got
    //  the expected data back.
    //
    for (c2Cur = 0; c2Cur < c4TableSize; c2Cur++)
        ac2ToWrite[c2Cur] = 0;

    if (!kflTest.bReadBuffer(ac2ToWrite, sizeof(ac2ToWrite), c4Actual))
    {
        strmOut << CUR_LN << L"Error reading in data. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c4Actual != sizeof(ac2ToWrite))
    {
        strmOut << CUR_LN << L"Only read "
                << c4Actual << L" bytes from file\n";
    }

    for (c2Cur = 0; c2Cur < c4TableSize; c2Cur++)
    {
        if (ac2ToWrite[c2Cur] != c2Cur)
        {
            strmOut << CUR_LN << L"Expected " << c2Cur
                    << L" but got " << ac2ToWrite[c2Cur] << EndLn;
            break;
        }
    }

    // Truncate the file at 16 bytes
    if (!kflTest.bTruncateAt(16))
    {
        strmOut << CUR_LN << L"Error truncating file. "
                << TKrnlError::kerrLast() << EndLn;
    }

    // The file pointer should now be at 16
    if (!kflTest.bQueryFilePtr(c8CurPos))
    {
        strmOut << CUR_LN << L"Error getting file position. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (c8CurPos != 16)
    {
        strmOut << CUR_LN
                << L"Current position was "
                << tCIDLib::TInt4(c8CurPos) << EndLn;
    }

    //
    //  Set the file time to a particular time, then read it back and
    //  see if it comes back what we set it.
    //
    TKrnlTimeStamp ktmsNow;
    if (!ktmsNow.bSetTo(tCIDLib::ESpecialTimes::CurrentTime))
        throw TKrnlError::kerrLast();

    if (!kflTest.bSetFileTimes(ktmsNow.enctTime(), ktmsNow.enctTime()))
        throw TKrnlError::kerrLast();

    // Query back the times at once first and test them out
    tCIDLib::TEncodedTime enctLastAccess, enctLastWrite;
    if (!kflTest.bQueryFileTimes(enctLastAccess, enctLastWrite))
        throw TKrnlError::kerrLast();

    if ((enctLastAccess != ktmsNow.enctTime())
    ||  (enctLastWrite != ktmsNow.enctTime()))
    {
        strmOut << CUR_LN << L"Set times did not come back the same\n";
        return;
    }

    // And now query them via the one at a time APIs
    if (!kflTest.bQueryLastAccessTime(enctLastAccess))
        throw TKrnlError::kerrLast();

    if (enctLastAccess != ktmsNow.enctTime())
        strmOut << CUR_LN << L"LastAccess time did not come back as set\n";

    if (!kflTest.bQueryLastWriteTime(enctLastWrite))
        throw TKrnlError::kerrLast();

    if (enctLastWrite != ktmsNow.enctTime())
        strmOut << CUR_LN << L"LastWrite time did not come back as set\n";

    //
    //  Close the file now. After we close it, the handle should no longer
    //  be valid.
    //
    if (!kflTest.bClose())
    {
        strmOut << CUR_LN << L"Error closing file. "
                << TKrnlError::kerrLast() << EndLn;
        return;
    }

    if (kflTest.bIsValid())
        strmOut << CUR_LN << L"Handle should not be valid now\n";
}


static tCIDLib::TVoid PathNormalizeTest()
{
    struct TTestItem
    {
        const tCIDLib::TCh*     pszIn;
        const tCIDLib::TCh*     pszOut;
    };

    TTestItem aTests[] =
    {
        { L"\\.."                                   , L"\\" }
      , { L"\\..\\"                                 , L"\\" }
      , { L".."                                     , L"\\" }
      , { L"."                                      , L"\\" }
      , { L"\\."                                    , L"\\" }
      , { L"\\Test1\\..\\Test2"                     , L"\\Test2" }
      , { L"\\Test1\\..\\..\\Test2"                 , L"\\Test2" }
      , { L"\\Test1\\.\\Test2"                      , L"\\Test1\\Test2" }
      , { L"\\Test1\\Test2\\Test3\\Test4\\..\\..\\" , L"\\Test1\\Test2\\" }
      , { L"\\Test1\\Test2\\.\\Test3\\..\\..\\"     , L"\\Test1\\" }

      , { L"C:\\Test1\\.\\Test2"                    , L"C:\\Test1\\Test2" }
      , { L"C:.."                                   , L"C:" }
      , { L"C:."                                    , L"C:" }
      , { L"\\\\UNCName\\Test1\\.."                 , L"\\\\UNCName" }
      , { L"\\\\UNCName\\Test1\\..\\.."             , L"\\\\" }
      , { L"\\\\UNCName\\Test1\\..\\..\\.."         , L"\\\\" }
      , { L"\\\\UNCName\\Test1\\..\\..\\..\\"       , L"\\\\" }
    };
    const tCIDLib::TCard4 c4TestCount = tCIDLib::c4ArrayElems(aTests);

    tCIDLib::TCh szBuf[kCIDLib::c4MaxPathLen + 1];

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
    {
        if (!TKrnlFileSys::bNormalizePath(  aTests[c4Index].pszIn
                                            , szBuf
                                            , kCIDLib::c4MaxPathLen)
        || !TRawStr::bCompareStr(szBuf, aTests[c4Index].pszOut))
        {
            strmOut << CUR_LN << L"Normalization test " << c4Index << L" failed\n"
                    << szBuf << EndLn;
        }
    }
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestFiles(const tCIDLib::TCard4)
{
    BasicFileTest();
    PathNormalizeTest();
}

