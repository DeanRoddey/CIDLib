//
// FILE NAME: TestCrypto_MD5.cpp
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
//  This file tests the MD5 encryption algorithm.
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
RTTIDecls(TTest_MD51,TTest_BaseCrypto)



// -----------------------------------------------------------------------------
//  Some local data to digest
//
//  apszTestResults
//      The known resulting hashes for the test vectors. These are in the
//      native character mode.
//
//  apszTestVectors
//      Some test vectors with known MD5 digest values.
//
//  pchTest
//      A test string to do some digesting on.
// -----------------------------------------------------------------------------
static const tCIDLib::TCard4        c4TestVectorCnt = 8;
static const tCIDLib::TCh* const    apszTestResults[c4TestVectorCnt] =
{
      L"D41D8CD98F00B204-E9800998ECF8427E"
    , L"0CC175B9C0F1B6A8-31C399E269772661"
    , L"900150983CD24FB0-D6963F7D28E17F72"
    , L"F96B697D7CB7938D-525A2F31AAF161D0"
    , L"C3FCD3D76192E400-7DFB496CCA67E13B"
    , L"D174AB98D277D9F5-A5611C2C9F419D9F"
    , L"57EDF4A22BE3C955-AC49DA2E2107B67A"
    , L"939E7578ED9E3C51-8A452ACEE763BCE9"
};
static const tCIDLib::TSCh* const   apszTestVectors[c4TestVectorCnt] =
{
    ""
    , "a"
    , "abc"
    , "message digest"
    , "abcdefghijklmnopqrstuvwxyz"
    , "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    , "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    , "Mufasa:testrealm@host.com:Circle Of Life"
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_MD51
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_MD51: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_MD51::TTest_MD51() :

    TTest_BaseCrypto
    (
        L"MD5 1", L"Basic tests of MD5 hash support", 4
    )
{
}

TTest_MD51::~TTest_MD51()
{
}


// ---------------------------------------------------------------------------
//  TTest_MD51: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_MD51::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    // Get the size in bytes of the test message
    const tCIDLib::TCard4 c4MsgBytes = TRawStr::c4StrLen(kTestCrypto::pchTest)
                                        * kCIDLib::c4CharBytes;

    // Get the test text into a memory buffer
    THeapBuf mbufMsg(c4MsgBytes, c4MsgBytes);
    mbufMsg.CopyIn(kTestCrypto::pchTest, c4MsgBytes);

    //
    //  Digest the message twice and make sure that the hashs come back
    //  the same.
    //
    {
        TMessageDigest5 mdigTest;
        TMD5Hash mhashTest1;
        TMD5Hash mhashTest2;

        mdigTest.StartNew();
        mdigTest.DigestBuf(mbufMsg);
        mdigTest.Complete(mhashTest1);

        mdigTest.StartNew();
        mdigTest.DigestBuf(mbufMsg);
        mdigTest.Complete(mhashTest2);

        if (mhashTest1 != mhashTest2)
        {
            strmOut << TFWCurLn
                    << L"Two digests of msg created different hashes"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // Modify a byte and do another, making sure they are not different
        mbufMsg.PutCard1(mbufMsg[1] + 1, 1UL);
        TMD5Hash mhashTest3;
        mdigTest.StartNew();
        mdigTest.DigestBuf(mbufMsg);
        mdigTest.Complete(mhashTest3);

        if (mhashTest1 == mhashTest3)
        {
            strmOut << TFWCurLn
                    << L"Digests of different msgs created same hashes"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }


    //
    //  Loop through the test vectors. For each one, get the vector text into
    //  a memory buffer, hash it, format the hash to a string, and compare it
    //  to the preformatted result from the test vector table.
    //
    {
        TMessageDigest5         mdigTest;
        TMD5Hash                mhashTest;
        TTextStringOutStream    strmTest(128);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestVectorCnt; c4Index++)
        {
            //
            //  We have to use short chars here, because the standard tests
            //  are not Unicode oriented. So, we have to really just treat
            //  them as non-text data.
            //
            tCIDLib::TCard4 c4VecLen = 0;
            while (apszTestVectors[c4Index][c4VecLen])
                c4VecLen++;

            THeapBuf mbufMsg(c4VecLen + 1, c4VecLen + 1);
            mbufMsg.CopyIn(apszTestVectors[c4Index], c4VecLen);

            mdigTest.StartNew();
            mdigTest.DigestBuf(mbufMsg, c4VecLen);
            mdigTest.Complete(mhashTest);

            strmTest.Reset();
            strmTest << mhashTest << kCIDLib::FlushIt;

            if (strmTest.strData() != apszTestResults[c4Index])
            {
                strmOut << TFWCurLn << L"Test MD5 vector #"
                        << c4Index + 1 << L" failed" << kCIDLib::DNewLn;
                return tTestFWLib::ETestRes::Failed;
            }
        }
    }


    //
    //  Test incremental hashing. Take one of the test vector strings above
    //  and break it into two parts that we digest separately. It should still
    //  proivde the same result as the original one did.
    //
    {
        TMessageDigest5         mdigTest;
        TMD5Hash                mhashTest;
        TTextStringOutStream    strmTest(128);

        const tCIDLib::TSCh* const psz1 = "12345678901234567890123456789012345";
        const tCIDLib::TSCh* const psz2 = "678901234567890123456789012345678901234567890";

        //
        //  This time don't call start new, which isn't required unless we
        //  start another one. So it should provide the right result.
        //
        mdigTest.DigestRaw((tCIDLib::TCard1*)psz1, TRawStr::c4StrLen(psz1));
        mdigTest.DigestRaw((tCIDLib::TCard1*)psz2, TRawStr::c4StrLen(psz2));
        mdigTest.Complete(mhashTest);

        strmTest << mhashTest << kCIDLib::FlushIt;
        if (strmTest.strData() != L"57EDF4A22BE3C955-AC49DA2E2107B67A")
        {
            strmOut << TFWCurLn << L"Incremental MD5 text failed"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    {
        //
        //  A more abusive incremental hashing test. We repeatedly hash a
        //  short value incrementally. The valueis prime length, so it should
        //  rotate through every possible internal hash block boundy scenario
        //  with the 3000 interations we do.
        //
        //  Then we build up that same value as a whole buffer and hash that.
        //  They should be the same.
        //
        //  The we put a stream over the buffer and hash it via the stream, which
        //  should also create same hash.
        //
        TMessageDigest5         mdigTest;
        TTextStringOutStream    strmTest(128);
        const tCIDLib::TCard1   ac1Data[7] =
        {
            0xF8, 0x31, 0x94, 0x57, 0x11, 0xA7, 0x03
        };

        mdigTest.StartNew();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 3000; c4Index++)
            mdigTest.DigestRaw(ac1Data, 7);
        TMD5Hash mhashTest1;
        mdigTest.Complete(mhashTest1);

        // Now build up that data in a buffer and hash that
        THeapBuf mbufTest(3000 * 7, 3000 * 7);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 3000; c4Index++)
            mbufTest.CopyIn(ac1Data, 7, c4Index * 7);
        mdigTest.StartNew();
        mdigTest.DigestBuf(mbufTest);
        TMD5Hash mhashTest2;
        mdigTest.Complete(mhashTest2);

        if (mhashTest1 != mhashTest2)
        {
            strmOut << TFWCurLn << L"MD5 incremental digest test 2 failed"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // But a stream over the buffer and do it again
        TMD5Hash mhashTest3;
        {
            TBinMBufInStream strmSrc(&mbufTest, mbufTest.c4Size());
            mdigTest.StartNew();
            mdigTest.DigestSrc(strmSrc, mbufTest.c4Size());
            mdigTest.Complete(mhashTest3);
        }

        if (mhashTest3 != mhashTest1)
        {
            strmOut << TFWCurLn << L"Digest via stream created wrong result"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    //
    //  In theory our own strings will be hashed by default to UTF-8, which
    //  should be the same for the above short character samples. So create
    //  a string from one of those and has that and compare it against the
    //  expected result.
    //
    {
        TMessageDigest5         mdigTest;
        TMD5Hash                mhashTest;
        TString                 strSrc(apszTestVectors[0]);
        TTextStringOutStream    strmTest(128);

        mdigTest.StartNew();
        mdigTest.DigestStr(strSrc);
        mdigTest.Complete(mhashTest);

        strmTest.Reset();
        strmTest << mhashTest << kCIDLib::FlushIt;

        if (strmTest.strData() != apszTestResults[0])
        {
            strmOut << TFWCurLn << L"TString hash gave wrong results"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    //
    //  Do a test of formatting in append mode. We'll just do two, then format
    //  them out in append mode. We should end up with the combined values.
    //
    {
        TMD5Hash mhash1;
        mhash1.ParseFormatted(L"0123456789ABCDEF-0123456789ABCDEF");

        TMD5Hash mhash2;
        mhash2.ParseFormatted(L"123456789ABCDEFA-123456789ABCDEFA");

        TString strFmt("XYZ");
        mhash1.FormatToStr(strFmt, kCIDLib::True);
        mhash2.FormatToStr(strFmt, kCIDLib::True);
        strFmt.Append(L"WUV");

        if (strFmt != L"XYZ0123456789ABCDEF-0123456789ABCDEF123456789ABCDEFA-123456789ABCDEFAWUV")
        {
            strmOut << TFWCurLn << L"MD5 append format failed" << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    //
    //  Do a test of formatting in non-=append mode. We'll just do two, then format
    //  them out in non-append mode. We should end up with the last value.
    //
    {
        TMD5Hash mhash1;
        mhash1.ParseFormatted(L"0123456789ABCDEF-0123456789ABCDEF");

        TMD5Hash mhash2;
        mhash2.ParseFormatted(L"123456789ABCDEFA-123456789ABCDEFA");

        TString strFmt(L"XYZ");
        mhash1.FormatToStr(strFmt, kCIDLib::False);
        mhash2.FormatToStr(strFmt, kCIDLib::False);

        if (strFmt != L"123456789ABCDEFA-123456789ABCDEFA")
        {
            strmOut << TFWCurLn << L"MD5 non-append format failed" << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    return tTestFWLib::ETestRes::Success;
}

