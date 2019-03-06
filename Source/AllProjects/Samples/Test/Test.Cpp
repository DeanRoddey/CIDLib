//
// FILE NAME: Test.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDSChan.hpp"




// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);
TOutConsole conOut;


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestMainThread", eMainThreadFunc))


//
//  Handles any startup work we need to do. We do the work to set up the
//  writer object and the profile and get it ready to start accepting
//  samples.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Some test data
    const tCIDLib::TCard1 ac1Modulus[] =
    {
        0xC4, 0xF8, 0xE9, 0xE1, 0x5D, 0xCA, 0xDF, 0x2B
      , 0x96, 0xC7, 0x63, 0xD9, 0x81, 0x00, 0x6A, 0x64
      , 0x4F, 0xFB, 0x44, 0x15, 0x03, 0x0A, 0x16, 0xED
      , 0x12, 0x83, 0x88, 0x33, 0x40, 0xF2, 0xAA, 0x0E
      , 0x2B, 0xE2, 0xBE, 0x8F, 0xA6, 0x01, 0x50, 0xB9
      , 0x04, 0x69, 0x65, 0x83, 0x7C, 0x3E, 0x7D, 0x15
      , 0x1B, 0x7D, 0xE2, 0x37, 0xEB, 0xB9, 0x57, 0xC2
      , 0x06, 0x63, 0x89, 0x82, 0x50, 0x70, 0x3B, 0x3F
    };
    const tCIDLib::TCard4 c4ModBytes = tCIDLib::c4ArrayElems(ac1Modulus);

    const tCIDLib::TCard1 ac1PrivateKey[] =
    {
        0x8a, 0x7e, 0x79, 0xf3, 0xfb, 0xfe, 0xa8, 0xeb
      , 0xfd, 0x18, 0x35, 0x1c, 0xb9, 0x97, 0x91, 0x36
      , 0xf7, 0x05, 0xb4, 0xd9, 0x11, 0x4a, 0x06, 0xd4
      , 0xaa, 0x2f, 0xd1, 0x94, 0x38, 0x16, 0x67, 0x7a
      , 0x53, 0x74, 0x66, 0x18, 0x46, 0xa3, 0x0c, 0x45
      , 0xb3, 0x0a, 0x02, 0x4b, 0x4d, 0x22, 0xb1, 0x5a
      , 0xb3, 0x23, 0x62, 0x2b, 0x2d, 0xe4, 0x7b, 0xa2
      , 0x91, 0x15, 0xf0, 0x6e, 0xe4, 0x2c, 0x41
    };
    const tCIDLib::TCard4 c4PrivBytes = tCIDLib::c4ArrayElems(ac1PrivateKey);

    const tCIDLib::TCard1 ac1PublicKey[] = { 0x1, 0x00, 0x01 };


    // Set up the two keys and set them on an RSA encrypter
    TRSAEncrypter crypTest
    (
        TRSAKey(ac1Modulus, c4ModBytes, ac1PublicKey, tCIDLib::c4ArrayElems(ac1PublicKey))
        , TRSAKey(ac1Modulus, c4ModBytes, ac1PrivateKey, c4PrivBytes)
    );

    //
    //  Create some data on the order of what it would typically be used for, which
    //  is encyprting a session key plus some overhead.
    //
    tCIDLib::TCard4 c4DataBytes = 320;
    THeapBuf mbufData(c4DataBytes);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4DataBytes; c4Index++)
    {
        mbufData[c4Index] = tCIDLib::TCard1(c4Index);
    }

    const tCIDLib::TEncodedTime enctStart = TTime::enctNow();

    tCIDLib::TCard4 c4EncryptedLen = 0;
    TBinMBufOutStream strmCText(4192UL);
    {
        //
        //  For the source data, we'll just use the modulus. It's random enough and
        //  we already have it. We use the public key to encrypt it
        //
        TBinMBufInStream strmSrc(&mbufData, c4DataBytes);
        c4EncryptedLen = crypTest.c4Encrypt
        (
            strmSrc, strmCText, tCIDSChan::EKeyTypes::Public
        );

        // Be sure to flush the output out to the memory buffer
        strmCText.Flush();
    }

    //
    //  Now, we set up an input stream over the output stream so that we can stream
    //  in the data we wrote out. And we'll decrypt back to another stream.
    //
    TBinMBufOutStream strmPText(4192UL);
    tCIDLib::TCard4 c4DecryptedLen = 0;
    {
        TBinMBufInStream strmSrc(strmCText);

        c4DecryptedLen = crypTest.c4Decrypt
        (
            strmSrc, strmPText, tCIDSChan::EKeyTypes::Private
        );

        // Be sure to flush the output out to the memory buffer
        strmPText.Flush();
    }

    const tCIDLib::TEncodedTime enctEnd = TTime::enctNow();

    conOut  << L"Time was: " << tCIDLib::TCard4((enctEnd - enctStart) / kCIDLib::enctOneMilliSec)
            << kCIDLib::NewEndLn;

    // And now let's compare the results
    if (c4DecryptedLen != c4DataBytes)
    {
        conOut  << L"RSA round trip created different byte count. Src="
                << c4DataBytes
                << L"  Decryped=" << c4DecryptedLen
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::Normal;
    }

    // Compare the bytes
    const TMemBuf& mbufPData = strmPText.mbufData();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4DataBytes; c4Index++)
    {
        if (mbufPData[c4Index] != mbufData[c4Index])
        {
            conOut  << L"RSA round trip mismatch at index "
                    << c4Index << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }
    }

    return tCIDLib::EExitCodes::Normal;
}
