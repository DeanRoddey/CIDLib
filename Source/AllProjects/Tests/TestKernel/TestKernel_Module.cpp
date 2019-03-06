//
// FILE NAME: TestKernel_TestModule.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/29/1999
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
//  This module tests the TKrnlModule class, which is a very key class and
//  the basis for the TFacility class in CIDLib.Dll.
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
static tCIDLib::TVoid MsgFileTest()
{
    const tCIDLib::TCard4 c4TextLen = 1024;
    tCIDLib::TCh szText[c4TextLen+1];

    // Use our global module object to try to load the test messages
    if (!kmodTestKernel.bLoadCIDFacMsg
    (
        kTestKrnlMsgs::midTestId1
        , szText
        , c4TextLen))
    {
        strmOut << CUR_LN << "Could not load message id: "
             << kTestKrnlMsgs::midTestId1 << EndLn;
    }

    if (!TRawStr::bCompareStr(szText, L"This is the text for Test id 1"))
    {
        strmOut << CUR_LN << L"Text for message id: "
                << kTestKrnlMsgs::midTestId1
                << L" did not match expected text\n";
    }

    // Use our global module object to try to load the test messages
    if (!kmodTestKernel.bLoadCIDFacMsg
    (
        kTestKrnlMsgs::midTestId3
        , szText
        , c4TextLen))
    {
        strmOut << CUR_LN << L"Could not load message id: "
                << kTestKrnlMsgs::midTestId3 << EndLn;
    }

    if (!TRawStr::bCompareStr
    (
        szText, L"This is the text for Test id 3, which is longer than the others"))
    {
        strmOut << CUR_LN << L"Text for message id: "
                << kTestKrnlMsgs::midTestId3
                << L" did not match expected text\n";
    }


    //
    //  Try one via the other mechanism, where it returns us the const pointer
    //  to the string.
    //
    const tCIDLib::TCh* pszMsg = kmodTestKernel.pszLoadCIDFacMsg(kTestKrnlMsgs::midTestId2);
    if (!TRawStr::bCompareStr(pszMsg, L"This is the text for Test id 2"))
    {
        strmOut << CUR_LN << L"Text for message id: "
                << kTestKrnlMsgs::midTestId2
                << L" did not match expected text\n";
    }
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestModules(const tCIDLib::TCard4)
{
    // Make sure we can access CIDLib type message files ok
    MsgFileTest();
}

