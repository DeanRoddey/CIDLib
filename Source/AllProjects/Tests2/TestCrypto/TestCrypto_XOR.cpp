//
// FILE NAME: TestCrypto_XOR.cpp
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
//  This file tests the XOR encryption algorithm.
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
RTTIDecls(TTest_XOR1,TTest_BaseCrypto)



// ---------------------------------------------------------------------------
//  CLASS: TTest_XOR1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_XOR1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_XOR1::TTest_XOR1() :

    TTest_BaseCrypto
    (
        L"XOR 1", L"Basic tests of XOR encryption support", 4
    )
{
}

TTest_XOR1::~TTest_XOR1()
{
}


// ---------------------------------------------------------------------------
//  TTest_XOR1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_XOR1::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    tTestFWLib::ETestRes eTestRes;

    // An initialization vector for those tests that use IV modes
    const tCIDLib::TCard1 ac1IV[16] =
    {
        0x51, 0x86, 0x6F, 0xD5, 0xB8, 0x5E, 0xCB, 0x8A
      , 0x4E, 0xF9, 0x97, 0x45, 0x61, 0x98, 0xDD, 0x78
    };

    // Do the common block cypher tests
    TCryptoKey ckeyTest(L"This is the test key");

    // Do a block encryption using each mode
    TXorBlockEncrypter crypToTest1(ckeyTest);
    eTestRes = eCommonBlockTests(crypToTest1, strmOut, nullptr, nullptr, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    TXorBlockEncrypter crypToTest2(ckeyTest, tCIDCrypto::EBlockModes::CBC);
    eTestRes = eCommonBlockTests(crypToTest2, strmOut, ac1IV, ac1IV, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    TXorBlockEncrypter crypToTest3(ckeyTest, tCIDCrypto::EBlockModes::CBC_IV);
    eTestRes = eCommonBlockTests(crypToTest3, strmOut, ac1IV, nullptr, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    return eRes;
}


