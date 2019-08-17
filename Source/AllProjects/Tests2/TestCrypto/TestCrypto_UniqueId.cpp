//
// FILE NAME: TestCrypto_UniqueId.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2007
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file tests the UniqueId encryption algorithm.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestCrypto.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_UniqueId1,TTest_BaseCrypto)




// ---------------------------------------------------------------------------
//  CLASS: TTest_UniqueId1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_UniqueId1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_UniqueId1::TTest_UniqueId1() :

    TTest_BaseCrypto
    (
        L"UniqueId 1", L"Basic tests of unique id generation support", 4
    )
{
}

TTest_UniqueId1::~TTest_UniqueId1()
{
}


// ---------------------------------------------------------------------------
//  TTest_UniqueId1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_UniqueId1::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    //
    //  Create an id, get it into a binary message hash object, then
    //  format that object and see if get the same string back.
    //
    TString strTmpId = TUniqueId::strMakeId();

    TMD5Hash mhashTmp;
    mhashTmp.ParseFormatted(strTmpId);

    TString strTmpId2(mhashTmp);
    if (strTmpId2 != strTmpId)
    {
        strmOut << TFWCurLn
                << L"Text/bin/text round trip of id failed"
                << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Success;
    }
    return tTestFWLib::ETestRes::Success;
}


