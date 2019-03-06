//
// FILE NAME: TestCrypto_AES.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file tests the AES encryption algorithm.
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
RTTIDecls(TTest_AES1, TTest_BaseCrypto)
RTTIDecls(TTest_AES2, TTestFWTest)
RTTIDecls(TTest_AES3, TTestFWTest)


// ---------------------------------------------------------------------------
//  CLASS: TTest_AES1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_AES1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_AES1::TTest_AES1() :

    TTest_BaseCrypto
    (
        L"AES 1", L"Basic tests of AES encryption support", 4
    )
{
}

TTest_AES1::~TTest_AES1()
{
}


// ---------------------------------------------------------------------------
//  TTest_AES1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_AES1::eRunTest(TTextStringOutStream&  strmOut
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


    //
    //  Do the common block encryption tests. We'll use a separate key
    //  for each type, to check the three key lenghts we support. Worst
    //  case key is 256 bits, so 32 bytes.
    //
    const tCIDLib::TCard1 ac1Key[32] =
    {
        0x4E, 0xF9, 0x97, 0x45, 0x61, 0x98, 0xDD, 0x78
      , 0x51, 0x86, 0x6F, 0xD5, 0xB8, 0x5E, 0xCB, 0x8A
      , 0x7D, 0x85, 0x6F, 0x9A, 0x61, 0x30, 0x63, 0xF2
      , 0x24, 0x66, 0xDD, 0x87, 0x8B, 0x96, 0x3C, 0x9D
    };


    // Do the common block encryption tests
    TCryptoKey ckey128(ac1Key, 16);
    TCryptoKey ckey192(ac1Key, 24);
    TCryptoKey ckey256(ac1Key, 32);

    TAESEncrypter crypToTest1(ckey128);
    eTestRes = eCommonBlockTests(crypToTest1, strmOut, nullptr, nullptr, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    TAESEncrypter crypToTest2(ckey192, tCIDCrypto::EBlockModes::CBC);
    eTestRes = eCommonBlockTests(crypToTest2, strmOut, ac1IV, ac1IV, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    TAESEncrypter crypToTest3(ckey256, tCIDCrypto::EBlockModes::CBC_IV);
    eTestRes = eCommonBlockTests(crypToTest3, strmOut, ac1IV, nullptr, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    //
    //  Do a key reset and run a common test again, to insure it handles
    //  a data reset correctly.
    //
    crypToTest1.SetNewKey(ckey256, tCIDCrypto::EBlockModes::CBC);
    eTestRes = eCommonBlockTests(crypToTest1, strmOut, ac1IV, ac1IV, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    crypToTest1.SetNewKey(ckey128, tCIDCrypto::EBlockModes::CBC_IV);
    eTestRes = eCommonBlockTests(crypToTest1, strmOut, ac1IV, nullptr, bWarning);
    if (eTestRes < eRes)
        eRes = eTestRes;

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_AES2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_AES2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_AES2::TTest_AES2() :

    TTest_BaseCrypto(L"AES 2", L"AES Tests 2", 4)
{
}

TTest_AES2::~TTest_AES2()
{
}


// ---------------------------------------------------------------------------
//  TTest_AES1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_AES2::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  This is a known multi-block test for CBC mode. So we are verifying
    //  that are getting the correct cypher text, not just that our own algorithm
    //  can round trip ok.
    //
    const tCIDLib::TCard1 ac1Key[16] =
    {
        0x42, 0x78, 0xB8, 0x40, 0xFB, 0x44, 0xAA, 0xA7
      , 0x57, 0xC1, 0xBF, 0x04, 0xAC, 0xBE, 0x1A, 0x3E

    };

    const tCIDLib::TCard1 ac1IV[16] =
    {
        0x57, 0xF0, 0x2A, 0x5C, 0x53, 0x39, 0xDA, 0xEB
      , 0x0A, 0x29, 0x08, 0xA0, 0x6A, 0xC6, 0x39, 0x3F
    };

    const tCIDLib::TCard1 ac1Plain[32] =
    {
        0x3C, 0x88, 0x8B, 0xBB, 0xB1, 0xA8, 0xEB, 0x9F
      , 0x3E, 0x9B, 0x87, 0xAC, 0xAA, 0xD9, 0x86, 0xC4
      , 0x66, 0xE2, 0xF7, 0x07, 0x1C, 0x83, 0x08, 0x3B
      , 0x8A, 0x55, 0x79, 0x71, 0x91, 0x88, 0x50, 0xE5
    };

    const tCIDLib::TCard1 ac1Cypher[32] =
    {
        0x47, 0x9C, 0x89, 0xEC, 0x14, 0xBC, 0x98, 0x99
      , 0x4E, 0x62, 0xB2, 0xC7, 0x05, 0xB5, 0x01, 0x4E
      , 0x17, 0x5B, 0xD7, 0x83, 0x2E, 0x7E, 0x60, 0xA1
      , 0xE9, 0x2A, 0xAC, 0x56, 0x8A, 0x86, 0x1E, 0xB7

    };

    // This is a CBC mode test, so we only do it in that mode
    tCIDLib::TCard1 ac1Out[32];
    TCryptoKey ckeyTest(ac1Key, 16);

    TAESEncrypter crypTest(ckeyTest, tCIDCrypto::EBlockModes::CBC);
    crypTest.c4Encrypt(ac1Plain, ac1Out, 32, 32, ac1IV);

    // Compare to the expected cypher text
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 32; c4Index++)
    {
        if (ac1Cypher[c4Index] != ac1Out[c4Index])
        {
            strmOut << TFWCurLn << L"Got incorrect CBC cypher text"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // Now decrypt and compare to the original
    crypTest.c4Decrypt(ac1Cypher, ac1Out, 32, 32, ac1IV);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 32; c4Index++)
    {
        if (ac1Plain[c4Index] != ac1Out[c4Index])
        {
            strmOut << TFWCurLn << L"Got incorrect CBC plain text"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }
    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_AES3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_AES3: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_AES3::TTest_AES3() :

    TTest_BaseCrypto(L"AES 3", L"AES Test Vectors", 4)
{
}

TTest_AES3::~TTest_AES3()
{
}


// ---------------------------------------------------------------------------
//  TTest_AES3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_AES3::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    struct TTest
    {
        const tCIDLib::TCh* const   pchKey;
        const tCIDLib::TCh* const   pchIV;
        const tCIDLib::TCh* const   pchPlain;
        const tCIDLib::TCh* const   pchCypher;
    };


    // Do some OFB mode single block tests
    {
        TTest aTests[] =
        {
            {
                L"00000000000000000000000000000000"
                , L"ffffe000000000000000000000000000"
                , L"00000000000000000000000000000000"
                , L"dd35cea2799940b40db3f819cb94c08b"
            }
          , {
                L"00000000000000000000000000000000"
                , L"ffffffffffffffffffffffffffffffe0"
                , L"00000000000000000000000000000000"
                , L"8568261797de176bf0b43becc6285afb"
            }
          , {
                L"00000000000000000000000000000000"
                , L"ffffffffffffffffffffffffffff8000"
                , L"00000000000000000000000000000000"
                , L"4b706f7f92406352394037a6d4f4688d"
            }
        };
        const tCIDLib::TCard4 c4TestCnt = tCIDLib::c4ArrayElems(aTests);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
        {
            const TTest& curTest = aTests[c4Index];
            const tCIDLib::TBoolean bRes = bDoOne
            (
                strmOut
                , tCIDCrypto::EBlockModes::OFB
                , curTest.pchKey
                , curTest.pchIV
                , curTest.pchPlain
                , curTest.pchCypher
            );

            if (!bRes)
            {
                strmOut << TFWCurLn
                        << L"OFB test " << (c4Index + 1) << L" failed"
                        << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
                break;
            }
        }
    }


    // Do some OFB mode multi block tests
    {
        TTest aTests[] =
        {
            {
                L"C9F4CE21B4C7DAAA4F93E292DC605BC5"
                , L"5E5A8CF2808C720E01C1ED92D470A45D"
                , L"8E19C5CACD015A662E7F40CDECADBF79A68081C06D9544B41C2DD248E77633B4"
                , L"885DC48ADD7EE6A1839BC5C5E03BEAE071301ECF91A0111520CDE0D3A112F5D2"
            }
          , {
                L"18B9887A34438FB2E759027E54E334B6"
                , L"A5BE8621E58DAE325C6B868FD783E2CD"
                , L"8CD659DF925950B516F737FC92D2FAFA008C008C9DFE0E75ED2D68F6FF79399FF21"
                  L"83464B8C37CF31AAFC145FCBFAC73E3F87ECCB435F424BF1C6D6EFB504E8E93E8"
                  L"A668A2210E3D3B4FD437AD1A5842"
                , L"82736A8FD3C07941B1173C9C51BFE0D8590F237AE36E7B93481B7B4AD88DA9D68F42"
                  L"7305B95E4B90F7212DBA00F6D6BDE7E39A74DA702012F36C4CD0A0C27756052AC"
                  L"1BD3BF1501C86C1186A69A7296C"
            }
        };
        const tCIDLib::TCard4 c4TestCnt = tCIDLib::c4ArrayElems(aTests);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
        {
            const TTest& curTest = aTests[c4Index];
            const tCIDLib::TBoolean bRes = bDoOne
            (
                strmOut
                , tCIDCrypto::EBlockModes::OFB
                , curTest.pchKey
                , curTest.pchIV
                , curTest.pchPlain
                , curTest.pchCypher
            );

            if (!bRes)
            {
                strmOut << TFWCurLn
                        << L"OFB test " << (c4Index + 1) << L" failed"
                        << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
                break;
            }
        }
    }
    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_AES3: Private non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TTest_AES3::bDoOne(         TTextStringOutStream&   strmOut
                    , const tCIDCrypto::EBlockModes eMode
                    , const tCIDLib::TCh* const     pszKey
                    , const tCIDLib::TCh* const     pszIV
                    , const tCIDLib::TCh* const     pszPlain
                    , const tCIDLib::TCh* const     pszCypher)
{
    tCIDLib::TCard4 c4KeyLen;
    tCIDLib::TCard1* pc1Key = pc1ConvertVal(strmOut, pszKey, c4KeyLen);
    TArrayJanitor<tCIDLib::TCard1> janKey(pc1Key);

    tCIDLib::TCard4 c4IVLen;
    tCIDLib::TCard1* pc1IV = pc1ConvertVal(strmOut, pszIV, c4IVLen);
    TArrayJanitor<tCIDLib::TCard1> janIV(pc1IV);

    tCIDLib::TCard4 c4PlainLen;
    tCIDLib::TCard1* pc1Plain = pc1ConvertVal(strmOut, pszPlain, c4PlainLen);
    TArrayJanitor<tCIDLib::TCard1> janPlain(pc1Plain);

    tCIDLib::TCard4 c4CypherLen;
    tCIDLib::TCard1* pc1Cypher = pc1ConvertVal(strmOut, pszCypher, c4CypherLen);
    TArrayJanitor<tCIDLib::TCard1> janCypher(pc1Cypher);

    if (!pc1Key || !pc1IV || !pc1Plain || !pc1Cypher)
        return kCIDLib::False;

    TCryptoKey ckeyTest(pc1Key, c4KeyLen);
    TAESEncrypter crypTest(ckeyTest, eMode);

    // IV has to be block length
    if (c4IVLen != crypTest.c4BlockSize())
    {
        strmOut << TFWCurLn << L"IV length != block size" << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    // Cypher text has to be multple of block
    if (c4CypherLen % crypTest.c4BlockSize())
    {
        strmOut << TFWCurLn << L"Cypher text not multiple of block size" << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    tCIDLib::TCard1* pc1Out = new tCIDLib::TCard1[c4CypherLen];
    TArrayJanitor<tCIDLib::TCard1> janOut(pc1Out);
    tCIDLib::TCard4 c4ResultBytes = crypTest.c4Encrypt
    (
        pc1Plain, pc1Out, c4PlainLen, c4CypherLen, pc1IV
    );

    if (c4ResultBytes != c4CypherLen)
    {
        strmOut << TFWCurLn << L"Encrypted bytes != expected cypher text size"
                << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    // Compare to the expected cypher text
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CypherLen; c4Index++)
    {
        if (pc1Cypher[c4Index] != pc1Out[c4Index])
        {
            strmOut << TFWCurLn << L"Got incorrect cypher text at index "
                    << c4Index << kCIDLib::DNewLn;
            return kCIDLib::False;
        }
    }

    // Now decrypt it back
    c4ResultBytes = crypTest.c4Decrypt
    (
        pc1Cypher, pc1Out, c4CypherLen, c4PlainLen, pc1IV
    );

    // And compare to the original now
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CypherLen; c4Index++)
    {
        if (pc1Plain[c4Index] != pc1Out[c4Index])
        {
            strmOut << TFWCurLn << L"Got incorrect plain text at index "
                    << c4Index << kCIDLib::DNewLn;
            return kCIDLib::False;
        }
    }

    return kCIDLib::True;
}


//
//  Converts one of the string values we use to store the text data into an array
//  of bytes.
//
tCIDLib::TCard1*
TTest_AES3::pc1ConvertVal(          TTextStringOutStream&   strmOut
                            , const tCIDLib::TCh* const     pszVal
                            ,       tCIDLib::TCard4&        c4RetBytes)
{
    c4RetBytes = TRawStr::c4StrLen(pszVal) / 2;

    tCIDLib::TCard1* pc1Ret = new tCIDLib::TCard1[c4RetBytes];
    tCIDLib::TCard1 c1Val;
    tCIDLib::TCard4 c4SrcInd = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RetBytes; c4Index++)
    {
        if (!TRawStr::bMakeHexByte(pszVal[c4SrcInd], pszVal[c4SrcInd + 1], c1Val))
        {
            strmOut << TFWCurLn
                    << L"Non hex byte in test data: " << pszVal
                    << kCIDLib::DNewLn;
            return nullptr;
        }
        pc1Ret[c4Index] = c1Val;
        c4SrcInd += 2;
    }
    return pc1Ret;
}
