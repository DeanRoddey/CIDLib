//
// FILE NAME: TestKernel_Environment.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/15/1997
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
//  This module tests the TKrnlEnvironment class. This class provides the
//  abstraction of the host OS' environmental variable access.
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
static tCIDLib::TVoid BasicTests(const tCIDLib::TCard4 c4RunIndex)
{
    static const tCIDLib::TCh* apszAddKeys[2] =
    {
        L"TestKey1"
        , L"TestKey2"
    };

    //
    //  Lets just see if we can find some values in the environment that
    //  we know the value of.
    //
    const tCIDLib::TCard4 c4BufChars = 512;
    tCIDLib::TCh szTmp[c4BufChars+1];

    if (!TKrnlEnvironment::bFind(L"TestVar1", szTmp, c4BufChars))
        strmOut << CUR_LN << L"Could not find 'TestVar1' in environment\n";

    if (!TKrnlEnvironment::bFind(L"TestVar2", szTmp, c4BufChars))
        strmOut << CUR_LN << L"Could not find 'TestVar2' in environment\n";

    //
    //  We cannot do these tests on the second run because they will look
    //  like a memory leak.
    //
    if (c4RunIndex == 0)
    {
        //
        //  Add a new element to the environment and then try to read it back and
        //  make sure its there and correct.
        //
        if (!TKrnlEnvironment::bAdd
        (
            apszAddKeys[c4RunIndex]
            , L"This is the value"))
        {
            throw TKrnlError::kerrLast();
        }

        if (!TKrnlEnvironment::bFind(apszAddKeys[c4RunIndex], szTmp, c4BufChars))
            strmOut << CUR_LN << L"Could not find just added variable in environment\n";

        if (!TRawStr::bCompareStr(szTmp, L"This is the value"))
            strmOut << CUR_LN << L"Just added var come back with different value\n";

        //
        //  Try to add it again and make sure it gets an error, since we cannot
        //  add a value twice.
        //
        if (TKrnlEnvironment::bAdd(apszAddKeys[c4RunIndex], L"This is the value"))
        {
            strmOut << CUR_LN << L"Failed to catch redundant add of same var\n";
        }
         else
        {
            if (TKrnlError::kerrLast().errcId() != kKrnlErrs::errcGen_AlreadyExists)
                strmOut << CUR_LN << L"Duplicate key add caused wrong error\n";
        }

        // Try to update the value of the new var we just added
        tCIDLib::TBoolean bAdded;
        if (!TKrnlEnvironment::bAddOrUpdate
        (
            apszAddKeys[c4RunIndex], L"This is the new value", bAdded))
        {
            throw TKrnlError::kerrLast();
        }

        if (bAdded)
            strmOut << CUR_LN << L"Update of existing value returned True\n";

        // Make sure we changed the value
        if (!TKrnlEnvironment::bFind(apszAddKeys[c4RunIndex], szTmp, c4BufChars))
            strmOut << CUR_LN << L"Could not find just updated variable in environment\n";

        if (!TRawStr::bCompareStr(szTmp, L"This is the new value"))
            strmOut << CUR_LN << L"Just updated var come back with different value\n";
    }
}


static tCIDLib::TVoid SpecialTests()
{
    const tCIDLib::TCard4 c4Len = 4096;
    tCIDLib::TCh szBuf1[c4Len + 1];
    tCIDLib::TCh szBuf2[c4Len + 1];

    // Try the special lib and exe path guys
    if (!TKrnlEnvironment::bFindLibPath(szBuf1, c4Len))
    {
        strmOut << CUR_LN << L"Failed to find library path\n";
        return;
    }
    const tCIDLib::TCard4 c4OrgLen = TRawStr::c4StrLen(szBuf1);

    // Append and prepend a path to it
    if (!TKrnlEnvironment::bAddToLibPath(L";\\Test", tCIDLib::EStartEnd::End))
    {
        strmOut << CUR_LN << L"Failed to append to library path\n";
        return;
    }

    if (!TKrnlEnvironment::bAddToLibPath(L"\\Test", tCIDLib::EStartEnd::Start))
    {
        strmOut << CUR_LN << L"Failed to prepend to library path\n";
        return;
    }

    // Build up what this should have resulted in
    TRawStr::CopyCatStr(szBuf2, c4Len, L"\\Test", kCIDLib::szMultiPathSep, szBuf1);
    if (szBuf1[c4OrgLen - 1] != kCIDLib::chMultiPathSep)
        TRawStr::CatStr(szBuf2, kCIDLib::szMultiPathSep, c4Len);
    TRawStr::CatStr(szBuf2, L"\\Test", c4Len);

    // Get it again and see if this is what we got
    if (!TKrnlEnvironment::bFindLibPath(szBuf1, c4Len))
    {
        strmOut << CUR_LN << L"Failed to find library path second time\n";
        return;
    }

    if (!TRawStr::bCompareStr(szBuf1, szBuf2))
    {
        strmOut << CUR_LN << L"Didn't get expected lib path results\n";
        strmOut << szBuf1 << "\n" << szBuf2 << EndLn;
        return;
    }
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestEnvironment(const tCIDLib::TCard4 c4RunIndex)
{
    // And do our tests
    BasicTests(c4RunIndex);
    SpecialTests();
}


