//
// FILE NAME: TestKernel_HashMap.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/1997
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
//  This module tests the TKrnlHashMap class. This class is a very simple
//  hashed map for raw strings for use within the kernel.
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
static tCIDLib::TVoid BasicTests()
{
    static const tCIDLib::TCh* apszKeys[] =
    {
        L"PATH"
        , L"LIBPATH"
        , L"BUILDSOURCE"
        , L"USERNAME"
        , L"TMP"
        , L"CIDDRIVE"
    };

    static const tCIDLib::TCh* apszValues[] =
    {
        L"C:\\TEMP;D:\\SOMEDIR;"
        , L"C:\\DLL;D:\\APP\\DLL;"
        , L"J:\\CID_DEV\\Source"
        , L"Dean Roddey"
        , L"C:\\Tmp"
        , L"J:"
    };

    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(apszKeys);

    // Create a test hash map and give it the desired modulus.
    TKrnlHashMap  khshmTest(23);

    // There should be 0 elements
    if (khshmTest.c4ElemCount())
    {
        strmOut << CUR_LN
                << L"New hash map has non-zero element count\n";
        return;
    }

    //
    //  Add some values to it. They are just statically defined lists
    //  of strings above.
    //
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (!khshmTest.bAdd(apszKeys[c4Index], apszValues[c4Index]))
            throw TKrnlError::kerrLast();
    }

    // There should be c4Count elements now
    if (khshmTest.c4ElemCount() != c4Count)
    {
        strmOut << CUR_LN
                << L"Hash map has " << khshmTest.c4ElemCount()
                << L"elements instead of " << c4Count << EndLn;
        return;
    }

    //
    //  We should be able to find each key in the list and gets its
    //  value and compare it to the original.
    //
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCh* pszTmp = khshmTest.pszValueForKey(apszKeys[c4Index]);
        if (!pszTmp)
        {
            strmOut << CUR_LN << L"Did not find key:"
                    << apszKeys[c4Index] << L"\n";
            continue;
        }

        if (!TRawStr::bCompareStr(pszTmp, apszValues[c4Index]))
        {
            strmOut << CUR_LN
                    << L"Expected value: '" << apszValues[c4Index]
                    << L"' for key: '" << apszKeys[c4Index]
                    << L"' but got value: '" << pszTmp
                    << L"'\n";
        }
    }

    //
    //  Now lets update every element and set its value to the values
    //  in reverse of the original order.
    //
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (!khshmTest.bUpdateKey
        (
            apszKeys[c4Index]
            , apszValues[c4Count - (c4Index+1)]))
        {
            throw TKrnlError::kerrLast();
        }
    }

    // And test the again
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCh* pszTmp = khshmTest.pszValueForKey(apszKeys[c4Index]);
        if (!pszTmp)
        {
            strmOut << CUR_LN << L"Did not find key:"
                    << apszKeys[c4Index] << L"\n";
            continue;
        }

        if (!TRawStr::bCompareStr(pszTmp, apszValues[c4Count - (c4Index+1)]))
        {
            strmOut << CUR_LN
                    << L"Expected value: '"
                    << apszValues[c4Count - (c4Index+1)]
                    << L"' for key: '" << apszKeys[c4Index]
                    << L"' but got value: '" << pszTmp
                    << L"'\n";
        }
    }

    //
    //  And now we should be able to flush the map and not find any elements
    //  in it.
    //
    khshmTest.RemoveAll();

    // There should be 0 elements again
    if (khshmTest.c4ElemCount())
    {
        strmOut << CUR_LN
                << L"Flushed hash map has non-zero element count\n";
        return;
    }

    // Now try to find each element, all of which should fail
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCh* pszTmp = khshmTest.pszValueForKey(apszKeys[c4Index]);
        if (pszTmp)
        {
            strmOut << CUR_LN
                    << L"Found an element in a flushed hash map\n";
        }
    }

    //
    //  Re-add the elements back and do the search again
    //
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (!khshmTest.bAdd(apszKeys[c4Index], apszValues[c4Index]))
            throw TKrnlError::kerrLast();
    }

    // There should be c4Count elements again now
    if (khshmTest.c4ElemCount() != c4Count)
    {
        strmOut << CUR_LN
                << L"Hash map has " << khshmTest.c4ElemCount()
                << L"elements instead of " << c4Count << EndLn;
        return;
    }

    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCh* pszTmp = khshmTest.pszValueForKey(apszKeys[c4Index]);
        if (!pszTmp)
        {
            strmOut << CUR_LN << L"Did not find key:"
                    << apszKeys[c4Index] << L"\n";
            continue;
        }

        if (!TRawStr::bCompareStr(pszTmp, apszValues[c4Index]))
        {
            strmOut << CUR_LN
                    << L"Expected value: '" << apszValues[c4Index]
                    << L"' for key: '" << apszKeys[c4Index]
                    << L"' but got value: '" << pszTmp
                    << L"'\n";
        }
    }

    //
    //  And now remove each element by key. This should also result in an
    //  empty hash map.
    //
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (!khshmTest.bRemoveKey(apszKeys[c4Index]))
            throw TKrnlError::kerrLast();
    }

    // There should be 0 elements again
    if (khshmTest.c4ElemCount())
    {
        strmOut << CUR_LN
                << L"Emptied hash map has non-zero element count\n";
        return;
    }
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestHashMap(const tCIDLib::TCard4)
{
    // And do our tests
    BasicTests();
}
