//
// FILE NAME: TestKernel_Paths.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/09/1999
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
//  This module tests the TKrnlPathStr namespace.
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
//  Local types
//
//  TPathTestInfo
//      This is a simple structure used to set up tests for path components.
//      Since different platforms will have different paths to test, we don't
//      want to hard code them.
// ----------------------------------------------------------------------------
struct TPathTestInfo
{
    const tCIDLib::TCh* pszToTest;
    const tCIDLib::TCh* pszPath;
    const tCIDLib::TCh* pszName;
    const tCIDLib::TCh* pszExt;
    const tCIDLib::TCh* pszVolume;
    tCIDLib::TBoolean   bIsFQ;
};


// ----------------------------------------------------------------------------
//  Local data
//
//  aPathTests
//      The paths to test. Do this one for your platform, since unfortunately
//      hard coded paths are platform specific.
// ----------------------------------------------------------------------------
static const TPathTestInfo aPathTests[] =
{
        { L"Name", 0, L"Name", 0, 0, kCIDLib::False }
    ,   { L".\\Testing", L".\\", L"Testing", 0, 0, kCIDLib::False }
    ,   { L".", 0, L".", 0, 0, kCIDLib::False }
    ,   { L"\\.", L"\\", L".", 0, 0, kCIDLib::False }
    ,   { L"..", 0, L"..", 0, 0, kCIDLib::False }
    ,   { L".\\Path1\\Path2\\", L".\\Path1\\Path2\\", 0, 0, 0, kCIDLib::False }
    ,   { L".Ext", 0, L".Ext", 0, 0, kCIDLib::False }
    ,   { L"", 0, 0, 0, 0, kCIDLib::False }

    ,   { L"C:\\Path\\..\\Name.Ext", L"\\Path\\..\\", L"Name", L"Ext", L"C:\\", kCIDLib::True }
    ,   { L"C:\\.", L"\\", L".", 0, L"C:\\", kCIDLib::True }
    ,   { L"C:\\Path\\Name.Ext", L"\\Path\\", L"Name", L"Ext", L"C:\\", kCIDLib::True }
    ,   { L"\\\\Node\\Share\\Name.Ext", L"\\", L"Name", L"Ext", L"\\\\Node\\Share\\", kCIDLib::True }
    ,   { L"\\\\Node\\Share\\Path\\Name.Ext", L"\\Path\\", L"Name", L"Ext", L"\\\\Node\\Share\\", kCIDLib::True }
    ,   { L"\\\\MyNode\\MyShare", 0, 0, 0, L"\\\\MyNode\\MyShare\\", kCIDLib::True }
};
static const tCIDLib::TCard4 c4TestCount = tCIDLib::c4ArrayElems(aPathTests);



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::TVoid BasicPathTests()
{
    if (!TKrnlFileSys::bIsDirectory(L"C:")
    ||  !TKrnlFileSys::bIsDirectory(L"C:\\"))
    {
        strmOut << CUR_LN << L"Directory special case for Win32 failed\n";
    }

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
    {
        const tCIDLib::TBoolean bCur
        (
            TKrnlPathStr::bIsFullyQualified(aPathTests[c4Index].pszToTest)
        );

        if (bCur != aPathTests[c4Index].bIsFQ)
            strmOut << CUR_LN << L"FQ test #" << c4Index << L" failed\n";
    }
}


static
tCIDLib::TVoid TestComponents(  const   tCIDLib::TCh* const pszSrc
                                , const tCIDLib::TCh* const pszPath
                                , const tCIDLib::TCh* const pszName
                                , const tCIDLib::TCh* const pszExt
                                , const tCIDLib::TCh* const pszVolume)
{
    const tCIDLib::TCard4 c4BufSize = 1023;
    tCIDLib::TBoolean   bFailed = kCIDLib::False;
    tCIDLib::TCh        szBuf[c4BufSize+1];

    if (pszPath)
    {
        if (!TKrnlPathStr::bHasPath(pszSrc))
        {
            strmOut << CUR_LN << L"Check for path failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TKrnlPathStr::bQueryPath(pszSrc, szBuf, c4BufSize))
        {
            strmOut << CUR_LN << L"Get of path failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TRawStr::bCompareStr(szBuf, pszPath))
        {
            strmOut << CUR_LN << L"Returned path was incorrect\n";
            bFailed = kCIDLib::True;
        }
    }
     else
    {
        if (TKrnlPathStr::bHasPath(pszSrc))
        {
            strmOut << CUR_LN << L"Bogus report of path existence\n";
            bFailed = kCIDLib::True;
        }
    }

    if (pszName)
    {
        if (!TKrnlPathStr::bHasName(pszSrc))
        {
            strmOut << CUR_LN << L"Check for name failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TKrnlPathStr::bQueryName(pszSrc, szBuf, c4BufSize))
        {
            strmOut << CUR_LN << L"Get of name failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TRawStr::bCompareStr(szBuf, pszName))
        {
            strmOut << CUR_LN << L"Returned name was incorrect\n";
            bFailed = kCIDLib::True;
        }
    }
     else
    {
        if (TKrnlPathStr::bHasName(pszSrc))
        {
            strmOut << CUR_LN << L"Bogus report of name existence\n";
            bFailed = kCIDLib::True;
        }
    }

    if (pszExt)
    {
        if (!TKrnlPathStr::bHasExt(pszSrc))
        {
            strmOut << CUR_LN << L"Check for extension failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TKrnlPathStr::bQueryExt(pszSrc, szBuf, c4BufSize))
        {
            strmOut << CUR_LN << L"Get of extension failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TRawStr::bCompareStr(szBuf, pszExt))
        {
            strmOut << CUR_LN << L"Returned extension was incorrect\n";
            bFailed = kCIDLib::True;
        }
    }
     else
    {
        if (TKrnlPathStr::bHasExt(pszSrc))
        {
            strmOut << CUR_LN << L"Bogus report of extension existence\n";
            bFailed = kCIDLib::True;
        }
    }

    if (pszVolume)
    {
        if (!TKrnlPathStr::bHasVolume(pszSrc))
        {
            strmOut << CUR_LN << L"Check for volume failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TKrnlPathStr::bQueryVolume(pszSrc, szBuf, c4BufSize))
        {
            strmOut << CUR_LN << L"Get of volume failed\n";
            bFailed = kCIDLib::True;
        }

        if (!TRawStr::bCompareStr(szBuf, pszVolume))
        {
            strmOut << CUR_LN << L"Returned volume was incorrect\n";
            bFailed = kCIDLib::True;
        }
    }
     else
    {
        if (TKrnlPathStr::bHasVolume(pszSrc))
        {
            strmOut << CUR_LN << L"Bogus report of volume existence\n";
            bFailed = kCIDLib::True;
        }
    }

    if (bFailed)
        strmOut << CUR_LN << L"Failed on input path: " << pszSrc << L"\n";
}


static tCIDLib::TVoid PathParseTest()
{
    // Run all of the tests, calling the local helper method for each one
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
    {
        TestComponents
        (
            aPathTests[c4Index].pszToTest
            , aPathTests[c4Index].pszPath
            , aPathTests[c4Index].pszName
            , aPathTests[c4Index].pszExt
            , aPathTests[c4Index].pszVolume
        );
    }
}


static tCIDLib::TVoid PathLevelTest()
{
    tCIDLib::TCh* const pszTest = TRawStr::pszReplicate
    (
        L"\\Level1\\Level2\\Level3\\Level4.Txt\\"
    );

    // Remove each level and test the results
    if (!TKrnlPathStr::bRemoveLevel(pszTest))
    {
        strmOut << CUR_LN << L"Failed to remove level 4\n";
        return;
    }
    if (!TRawStr::bCompareStr(pszTest, L"\\Level1\\Level2\\Level3\\"))
    {
        strmOut << CUR_LN << L"Level 4 removal failed\n";
        return;
    }

    if (!TKrnlPathStr::bRemoveLevel(pszTest))
    {
        strmOut << CUR_LN << L"Failed to remove level 3\n";
        return;
    }
    if (!TRawStr::bCompareStr(pszTest, L"\\Level1\\Level2\\"))
    {
        strmOut << CUR_LN << L"Level 3 removal failed\n";
        return;
    }

    if (!TKrnlPathStr::bRemoveLevel(pszTest))
    {
        strmOut << CUR_LN << L"Failed to remove level 2\n";
        return;
    }
    if (!TRawStr::bCompareStr(pszTest, L"\\Level1\\"))
    {
        strmOut << CUR_LN << L"Level 2 removal failed\n";
        return;
    }

    if (!TKrnlPathStr::bRemoveLevel(pszTest))
    {
        strmOut << CUR_LN << L"Failed to remove level 1\n";
        return;
    }
    if (!TRawStr::bCompareStr(pszTest, L"\\"))
    {
        strmOut << CUR_LN << L"Level 1 removal failed\n";
        return;
    }

    if (TKrnlPathStr::bRemoveLevel(pszTest) || !TRawStr::bCompareStr(pszTest, L"\\"))
    {
        strmOut << CUR_LN << L"Removed a level that doesn't exist\n";
        return;
    }
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestPaths(const tCIDLib::TCard4)
{
    BasicPathTests();
    PathParseTest();
    PathLevelTest();
}

