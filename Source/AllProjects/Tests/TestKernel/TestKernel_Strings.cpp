//
// FILE NAME: TestKernel_Strings.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2020
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2020
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
//  This module tests the TKrnlString class, which is used mostly to transfer
//  string data between the kernel and CIDLib, but sometimes is used internally
//  to make things simpler since it provides some basic but useful functionality
//  that we don't want to replicate.
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

// Test various appending methods
static tCIDLib::TVoid TestAppend()
{
    // Very basic append tests
    {
        TKrnlString kstrTest1;
        kstrTest1.Append(L"1");
        if ((kstrTest1.c4Length() != 1)
        ||  !TRawStr::bCompareStr(kstrTest1.pszValue(), L"1"))
        {
            strmOut << CUR_LN << L"First append result was wrong\n";
            return;
        }

        kstrTest1.Append(L"2");
        if ((kstrTest1.c4Length() != 2)
        ||  !TRawStr::bCompareStr(kstrTest1.pszValue(), L"12"))
        {
            strmOut << CUR_LN << L"Second append result was wrong\n";
            return;
        }

        kstrTest1.Append(L"3", L"4", L"5", L"6");
        if ((kstrTest1.c4Length() != 6)
        ||  !TRawStr::bCompareStr(kstrTest1.pszValue(), L"123456"))
        {
            strmOut << CUR_LN << L"Multi-string append result was wrong\n";
            return;
        }

        // Get the buffer length, then append something long
        const tCIDLib::TCard4 c4OrgBufSz = kstrTest1.c4BufSize();
        kstrTest1.Append
        (
            L"This is a long string to append, which should cause the buffer to "
            L"have to expand and we should get a bigger buffer size, one would assume "
            L"given the windiness of this speech, which just goes on and one and on, "
            L"making much ado about little or nothing."
        );
        if (kstrTest1.c4BufSize() <= c4OrgBufSz)
        {
            strmOut << CUR_LN << L"Buffer was not expanded by long append\n";
            return;
        }
    }

    // Append other string objects
    {
        TKrnlString kstrTest1(L"12345678");
        TKrnlString kstrTest2(L"90123456");
        kstrTest1.Append(kstrTest2);

        // Source should be unaffected
        if (kstrTest2 != L"90123456")
        {
            strmOut << CUR_LN << L"Append source string was modified\n";
            return;
        }

        if (kstrTest1 != L"1234567890123456")
        {
            strmOut << CUR_LN << L"Append of string object failed\n";
            return;
        }
    }
}




static tCIDLib::TVoid TestCtors()
{
    // Basic default ctor tests
    {
        TKrnlString kstrTest;
        if (kstrTest.c4Length() != 0)
        {
            strmOut << CUR_LN << L"Default ctor did not create zero length\n";
            return;
        }

        if (!kstrTest.bIsEmpty())
        {
            strmOut << CUR_LN << L"Default ctor string reported not empty\n";
            return;
        }

        // The default buffer size is 32 if not set
        if (kstrTest.c4BufSize() != 32)
        {
            strmOut << CUR_LN << L"Default ctor allocated wrong buffer size\n";
            return;
        }
    }

    // Indicate a specific buffer size
    {
        TKrnlString kstrTest(81);
        if (kstrTest.c4BufSize() != 81)
        {
            strmOut << CUR_LN << L"Buf size ctor created wrong size\n";
            return;
        }
    }

    // Test various ctors that take incoming initial values
    {
        TKrnlString kstrTest1(L"1234");
        if ((kstrTest1.c4Length() != 4)
        ||  !TRawStr::bCompareStr(kstrTest1.pszValue(), L"1234"))
        {
            strmOut << CUR_LN << L"1234 ctor set wrong size or initial contents\n";
            return;
        }

        TKrnlString kstrTest2(L"1", L"2", L"3", L"4", L"5");
        if ((kstrTest2.c4Length() != 5)
        ||  !TRawStr::bCompareStr(kstrTest2.pszValue(), L"12345"))
        {
            strmOut << CUR_LN << L"Multi-src ctor set wrong size or initial contents\n";
            return;
        }

        TKrnlString kstrTest3("Short char");
        if ((kstrTest3.c4Length() != 10)
        ||  !TRawStr::bCompareStr(kstrTest3.pszValue(), L"Short char"))
        {
            strmOut << CUR_LN << L"Short char ctor set wrong size or initial contents\n";
            return;
        }
    }
}



// Test some odds and ends stuff
static tCIDLib::TVoid TestMiscellaneous()
{
    // Test the clear method
    {
        TKrnlString kstrTest(L"Some text");
        if (kstrTest != L"Some text")
        {
            strmOut << CUR_LN << L"Wrong text after ctor\n";
            return;
        }

        kstrTest.Clear();
        if ((kstrTest.c4Length() != 0)
        ||  !kstrTest.bIsEmpty()
        ||  (kstrTest != L""))
        {
            strmOut << CUR_LN << L"Cleared string was not empty\n";
            return;
        }
    }

    // Test the setting of an unterminated buffer
    {
        TKrnlString kstrTest;
        kstrTest.SetUnterminated(L"1234567890", 5);

        if ((kstrTest.c4Length() != 5) || (kstrTest != L"12345"))
        {
            strmOut << CUR_LN << L"Unterminated buffer set failed\n";
            return;
        }
    }

    // Test setting from a short string
    {
        TKrnlString kstrTest;
        kstrTest.SetFromShort("A short string");
        if (kstrTest != L"A short string")
        {
            strmOut << CUR_LN << L"Wrong text after set from short string\n";
            return;
        }
    }

    // Test manual buffer expansion
    {
        TKrnlString kstrTest;
        const tCIDLib::TCard4 c4OrgBufSz = kstrTest.c4BufSize();
        kstrTest.Reallocate(c4OrgBufSz + 100);
        if (kstrTest.c4BufSize() <= c4OrgBufSz)
        {
            strmOut << CUR_LN << L"Buffer reallocation did not change buffer size\n";
            return;
        }
    }

    // Test the append if not already method
    {
        TKrnlString kstrTest(L"ABC;");
        kstrTest.AppendIfNotAlready(L';');
        if (kstrTest != L"ABC;")
        {
            strmOut << CUR_LN << L"Semicolon was already there but appended anyway\n";
            return;
        }

        kstrTest.AppendIfNotAlready(L'-');
        if (kstrTest != L"ABC;-")
        {
            strmOut << CUR_LN << L"Hyphen should have been appended\n";
            return;
        }
    }
}


// Test the special ctors and operators
static tCIDLib::TVoid TestSpecialMethods()
{
    TKrnlString kstrTest1(L"Value 1");
    TKrnlString kstrTest2(kstrTest1);

    if (kstrTest2 != kstrTest1)
    {
        strmOut << CUR_LN << L"Copied string different from source\n";
        return;
    }

    TKrnlString kstrTest3;
    kstrTest3 = kstrTest2;
    if (kstrTest3 != kstrTest2)
    {
        strmOut << CUR_LN << L"Assigned string different from source\n";
        return;
    }

    TKrnlString kstrTest4(tCIDLib::ForceMove(kstrTest3));
    if (kstrTest4 != kstrTest1)
    {
        strmOut << CUR_LN << L"Moved into string different from source\n";
        return;
    }

    if (kstrTest3.c4Length() != 0)
    {
        strmOut << CUR_LN << L"Moved out of string was not zero length\n";
        return;
    }

    if (kstrTest4 == kstrTest3)
    {
        strmOut << CUR_LN << L"Unequal strings tested equal\n";
        return;
    }
}



// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------

tCIDLib::TVoid TestStrings(const tCIDLib::TCard4)
{
    TestCtors();
    TestSpecialMethods();
    TestAppend();
    TestMiscellaneous();
}
