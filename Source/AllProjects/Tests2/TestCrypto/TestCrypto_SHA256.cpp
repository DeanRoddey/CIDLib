//
// FILE NAME: TestCrypto_SHA2561.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/09/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file tests the SHA256 hash
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
RTTIDecls(TTest_SHA256_1, TTest_BaseCrypto)
RTTIDecls(TTest_HMACSHA256, TTest_BaseCrypto)


// ---------------------------------------------------------------------------
//  CLASS: TTest_SHA256_1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SHA256_1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SHA256_1::TTest_SHA256_1() :

    TTest_BaseCrypto
    (
        L"SHA256 1", L"Basic tests of SHA256 hash support", 4
    )
{
}

TTest_SHA256_1::~TTest_SHA256_1()
{
}


// ---------------------------------------------------------------------------
//  TTest_SHA256_1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_SHA256_1::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    //
    //  Set up some known test vectors and run through them
    //
    struct TTest
    {
        const   tCIDLib::TCh*   pszName;
        tCIDLib::TCard1         ac1Input[64];
        tCIDLib::TCard4         c4InputSz;
        tCIDLib::TCard1         ac1Output[32];
    };
    TTest aTests[] =
    {
        {
            L"Empty input"
            , { }
            , 0
            , {
                  0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14
                , 0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24
                , 0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C
                , 0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55
              }
        }
      , {
            L"abc test"
            , { 0x61, 0x62, 0x63 }
            , 3
            , {
                  0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA
                , 0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23
                , 0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C
                , 0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD
              }
        }
    };
    const tCIDLib::TCard4 c4TestCnt = tCIDLib::c4ArrayElems(aTests);

    // Just run through the above tests and do each one
    TSHA256Hasher mdigTest;
    TSHA256Hash mhashTest;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
    {
        const TTest& testCur = aTests[c4Index];

        mdigTest.StartNew();
        mdigTest.DigestRaw(testCur.ac1Input, testCur.c4InputSz);
        mdigTest.Complete(mhashTest);

        TSHA256Hash mhashResult(testCur.ac1Output, 32);
        if (mhashTest != mhashResult)
        {
            strmOut << TFWCurLn << L"SHA256 " << testCur.pszName
                    << L" test failed" << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    //
    //  Do the same but this time as two separate passes of half the bytes each, just
    //  to make sure we deal with partial blocks correctly
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
    {
        const TTest& testCur = aTests[c4Index];

        // Can't do it if less than 2 bytes
        if (testCur.c4InputSz < 2)
            continue;

        const tCIDLib::TCard4 c4HalfSz = testCur.c4InputSz / 2;
        mdigTest.StartNew();
        mdigTest.DigestRaw(testCur.ac1Input, c4HalfSz);
        mdigTest.DigestRaw(testCur.ac1Input + c4HalfSz, testCur.c4InputSz - c4HalfSz);
        mdigTest.Complete(mhashTest);

        TSHA256Hash mhashResult(testCur.ac1Output, 32);
        if (mhashTest != mhashResult)
        {
            strmOut << TFWCurLn << L"SHA256 " << testCur.pszName
                    << L" halfsize test failed" << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    return tTestFWLib::ETestRes::Success;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_HMACSHA256
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_HMACSHA256: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_HMACSHA256::TTest_HMACSHA256() :

    TTest_BaseCrypto
    (
        L"SHA256 1", L"Tests of HMAC-SHA256 authentication code", 4
    )
{
}

TTest_HMACSHA256::~TTest_HMACSHA256()
{
}


// ---------------------------------------------------------------------------
//  TTest_HMACSHA256: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_HMACSHA256::eRunTest( TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    //
    //  Set up some known test vectors and run through them
    //
    struct TTest
    {
        const   tCIDLib::TCh*   pszName;
        tCIDLib::TCard1         ac1Key[64];
        tCIDLib::TCard4         c4KeySz;
        tCIDLib::TCard1         ac1Msg[64];
        tCIDLib::TCard4         c4MsgSz;
        tCIDLib::TCard1         ac1Output[32];
    };
    TTest aTests[] =
    {
        {
            L"Hi There"
            , {
                  0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B
                , 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B
                , 0x0B, 0x0B, 0x0B, 0x0B
              }
            , 20
            , {
                  0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65
              }
            , 8
            , {
                  0xB0, 0x34, 0x4C, 0x61, 0xD8, 0xDB, 0x38, 0x53
                , 0x5C, 0xA8, 0xAF, 0xCE, 0xAF, 0x0B, 0xF1, 0x2B
                , 0x88, 0x1D, 0xC2, 0x00, 0xC9, 0x83, 0x3D, 0xA7
                , 0x26, 0xE9, 0x37, 0x6C, 0x2E, 0x32, 0xCF, 0xF7
              }
        }
      , {
            L"Jefe"
            , {
                  0x4A, 0x65, 0x66, 0x65
              }
            , 4
            , {
                  0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6F, 0x20
                , 0x79, 0x61, 0x20, 0x77, 0x61, 0x6E, 0x74, 0x20
                , 0x66, 0x6F, 0x72, 0x20, 0x6E, 0x6F, 0x74, 0x68
                , 0x69, 0x6E, 0x67, 0x3F
              }
            , 28
            , {
                  0x5B, 0xDC, 0xC1, 0x46, 0xBF, 0x60, 0x75, 0x4E
                , 0x6A, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xC7
                , 0x5A, 0x00, 0x3F, 0x08, 0x9D, 0x27, 0x39, 0x83
                , 0x9D, 0xEC, 0x58, 0xB9, 0x64, 0xEC, 0x38, 0x43
              }
        }
    };
    const tCIDLib::TCard4 c4TestCnt = tCIDLib::c4ArrayElems(aTests);

    // Just run through the above tests and do each one
    TSHA256Hasher mdigTest;
    TSHA256Hash mhashTest;
    THeapBuf mbufKey(64, 64);
    THeapBuf mbufMsg(256, 256);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
    {
        const TTest& testCur = aTests[c4Index];

        mbufKey.CopyIn(testCur.ac1Key, testCur.c4KeySz);
        mbufMsg.CopyIn(testCur.ac1Msg, testCur.c4MsgSz);

        facCIDCrypto().GenerateHMAC
        (
            mbufKey
            , testCur.c4KeySz
            , mbufMsg
            , testCur.c4MsgSz
            , mdigTest
            , mhashTest
        );

        // Compare the bytes
        TSHA256Hash mhashResult(testCur.ac1Output, 32);
        if (mhashTest != mhashResult)
        {
            strmOut << TFWCurLn << L"HMAC-SHA256 " << testCur.pszName
                    << L" test failed" << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    return tTestFWLib::ETestRes::Success;
}

