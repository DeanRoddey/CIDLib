//
// FILE NAME: TestCrypto_SHA11.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/23/2009
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
//  This file tests the SHA1 hash
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
RTTIDecls(TTest_SHA1_1,TTest_BaseCrypto)



// -----------------------------------------------------------------------------
//  Some local data to digest
//
//  apszTestResults
//      The known resulting hashes for the test vectors. These are in the
//      native character mode.
//
//  apszTestVectors
//      Some test vectors with known SHA1 digest values.
//
// -----------------------------------------------------------------------------
static const tCIDLib::TCard4        c4TestVectorCnt = 4;
static const tCIDLib::TCh* const    apszTestResults[c4TestVectorCnt] =
{
      L"A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D"
    , L"84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1"
    , L"2FD4E1C6 7A2D28FC ED849EE1 BB76E739 1B93EB12"
    , L"DE9F2C7F D25E1B3A FAD3E85A 0BD17D9B 100DB4B3"
};
static const tCIDLib::TSCh* const   apszTestVectors[c4TestVectorCnt] =
{
    "abc"
    , "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    , "The quick brown fox jumps over the lazy dog"
    , "The quick brown fox jumps over the lazy cog"
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SHA1_1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SHA1_1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SHA1_1::TTest_SHA1_1() :

    TTest_BaseCrypto
    (
        L"SHA1 1", L"Basic tests of SHA1 hash support", 4
    )
{
}

TTest_SHA1_1::~TTest_SHA1_1()
{
}


// ---------------------------------------------------------------------------
//  TTest_SHA1_1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_SHA1_1::eRunTest( TTextStringOutStream&   strmOut
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
        TSHA1Hasher mdigTest;
        TSHA1Hash mhashTest1;
        TSHA1Hash mhashTest2;

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
        mbufMsg[1]++;
        TSHA1Hash mhashTest3;

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
        TSHA1Hasher             mdigTest;
        TSHA1Hash               mhashTest;
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
                strmOut << TFWCurLn << L"Test SHA1 vector #"
                        << c4Index + 1 << L" failed" << kCIDLib::DNewLn;
                return tTestFWLib::ETestRes::Failed;
            }
        }

        // Do a generated one of a very long message a million 'a' characters
        tCIDLib::TCard1* pc1BigMsg = new tCIDLib::TCard1[1000000];
        TArrayJanitor<tCIDLib::TCard1> janBig(pc1BigMsg);

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 1000000; c4Index++)
            pc1BigMsg[c4Index] = 0x61;

        mdigTest.StartNew();
        mdigTest.DigestRaw(pc1BigMsg, 1000000);
        mdigTest.Complete(mhashTest);

        strmTest.Reset();
        strmTest << mhashTest << kCIDLib::FlushIt;

        if (strmTest.strData() != L"34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F")
        {
            strmOut << TFWCurLn << L"Big SHA1 test failed" << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    {
        //
        //  Test incremental hashing. We do one of the test vectors above and
        //  break it into two parts. This should result in the same hash as
        //  the original.
        //
        const tCIDLib::TSCh* const psz1 = "The quick brown fox ";
        const tCIDLib::TSCh* const psz2 = "jumps over the lazy dog";

        TSHA1Hasher             mdigTest;
        TSHA1Hash               mhashTest;
        TTextStringOutStream    strmTest(128);

        //
        //  This time don't call start new, which isn't required unless we
        //  start another one. So it should provide the right result.
        //
        mdigTest.DigestRaw((tCIDLib::TCard1*)psz1, TRawStr::c4StrLen(psz1));
        mdigTest.DigestRaw((tCIDLib::TCard1*)psz2, TRawStr::c4StrLen(psz2));
        mdigTest.Complete(mhashTest);

        strmTest << mhashTest << kCIDLib::FlushIt;
        if (strmTest.strData() != L"2FD4E1C6 7A2D28FC ED849EE1 BB76E739 1B93EB12")
        {
            strmOut << TFWCurLn << L"SHA1 incremental digest failed"
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
        TSHA1Hasher             mdigTest;
        TTextStringOutStream    strmTest(128);
        const tCIDLib::TCard1   ac1Data[7] =
        {
            0xF8, 0x31, 0x94, 0x57, 0x11, 0xA7, 0x03
        };

        mdigTest.StartNew();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 3000; c4Index++)
            mdigTest.DigestRaw(ac1Data, 7);
        TSHA1Hash mhashTest1;
        mdigTest.Complete(mhashTest1);

        // Now build up that data in a buffer and hash that
        THeapBuf mbufTest(3000 * 7, 3000 * 7);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 3000; c4Index++)
            mbufTest.CopyIn(ac1Data, 7, c4Index * 7);
        mdigTest.StartNew();
        mdigTest.DigestBuf(mbufTest);
        TSHA1Hash mhashTest2;
        mdigTest.Complete(mhashTest2);

        if (mhashTest1 != mhashTest2)
        {
            strmOut << TFWCurLn << L"SHA1 incremental digest test 2 failed"
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // Do it again via a stream over the test buffer, should get the same hash
        TSHA1Hash mhashTest3;
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
    //  Do a test of formatting in append mode. We'll just do two, then format
    //  them out in append mode. We should end up with the combined values.
    //
    //  This also tests the parsing of formatted ids.
    //
    {
        TSHA1Hash mhash1;
        mhash1.ParseFormatted(L"A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D");

        TSHA1Hash mhash2;
        mhash2.ParseFormatted(L"84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1");

        TString strFmt(L"XYZ");
        mhash1.FormatToStr(strFmt, kCIDLib::True);
        mhash2.FormatToStr(strFmt, kCIDLib::True);
        strFmt.Append(L"WUV");

        if (strFmt != L"XYZA9993E36 4706816A BA3E2571 7850C26C 9CD0D89D84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1WUV")
        {
            strmOut << TFWCurLn << L"SHA1 append format failed" << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }
    }

    return tTestFWLib::ETestRes::Success;
}

