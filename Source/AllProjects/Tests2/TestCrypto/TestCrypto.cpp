//
// FILE NAME: TestCrypto.cpp
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
//  This is the main implementation file of the test program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "TestCrypto.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TTest_BaseCrypto,TTestFWTest)
RTTIDecls(TCryptoTestApp,TTestFWApp)



// ---------------------------------------------------------------------------
//  CLASS: TTest_BaseCrypto
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BaseCrypto: Public, static data
// ---------------------------------------------------------------------------
const tCIDLib::TCh* const TTest_BaseCrypto::pszPlainText
(
    L"This is a test of the encryption of some Unicode text. Unicode text is "
    L"kind of special because a lot of the time it will have every other byte "
    L"as a zero. And its also sensitive to byte order since each char is two "
    L"bytes in size, as apposed to ASCII text which is single byte.\n"
    L"What can you say about this kind of text? Its really the kind of text "
    L"that makes a man swell with pride as he reads it. And men now abed in "
    L"England shall hold their manhoods cheap, as any speaks who fought with "
    L"us here upon Saint Crispen's day. blah blah blah"
);


// ---------------------------------------------------------------------------
//  TTest_BaseCrypto: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BaseCrypto::TTest_BaseCrypto( const   TString&        strName
                                    , const TString&        strDescr
                                    , const tCIDLib::TCard4 c4Level) :

    TTestFWTest(strName, strDescr, c4Level)
{
}

TTest_BaseCrypto::~TTest_BaseCrypto()
{
}


// ---------------------------------------------------------------------------
//  TTest_BaseCrypto: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BaseCrypto::eCommonBlockTests(TBlockEncrypter&                crypToTest
                                    , TTextStringOutStream&         strmOut
                                    , const tCIDLib::TCard1* const  pc1EncIV
                                    , const tCIDLib::TCard1* const  pc1DecIV
                                    , tCIDLib::TBoolean&            bWarning)
{
    tCIDLib::TCard4     c4BytesDone;
    tCIDLib::TCard4     c4BytesToDo;

    //
    //  Create a memory buffer to encrypt into. This one is just made big
    //  enough to hold any cypher text that our tests will ever create. The
    //  cypher text can be larger than the plain text and and there is no way
    //  to know for sure, so you just have to have plenty of room. Memory
    //  buffers can auto expand, so you don't have to commit it all up front.
    //
    THeapBuf mbufCypher(512, 2048);

    //
    //  First do an encryption of a string. So we put the predefined text
    //  above into the string.
    //
    TString strToUse(pszPlainText);
    c4BytesDone = crypToTest.c4Encrypt(strToUse, mbufCypher, pc1EncIV);

    // Get a quick reference to the block size
    const tCIDLib::TCard4 c4BlockSize = crypToTest.c4BlockSize();

    //
    //  Calculate how many bytes should have been done. This is the length
    //  of the text in bytes rounded up to the next even block size of the
    //  passed encrypter. The text will be converted to UTF8 prior to
    //  encryption, to make it endian independent, and since all of the
    //  sample text is ASCII, the byte count will be the ingoing char
    //  count.
    //
    c4BytesToDo = strToUse.c4Length();
    if (c4BytesToDo % c4BlockSize)
        c4BytesToDo += c4BlockSize - (c4BytesToDo % c4BlockSize);

    // If the mode was CBC_IV, then add another block size
    if (crypToTest.eMode() == tCIDCrypto::EBlockModes::CBC_IV)
        c4BytesToDo += c4BlockSize;

    // Make sure it did the number of bytes expected
    if (c4BytesDone != c4BytesToDo)
    {
        strmOut << TFWCurLn << L"Bytes encrypted was wrong. Expected: "
                << c4BytesToDo << L" but got: " << c4BytesDone
                << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Now decrypt the data and make sure it comes back as it should,
    //  i.e. we get the same text back. We have to reset it so that it
    //  gets back to the initial state that it was in when we started
    //  the encryption.
    //
    strToUse.Clear();
    crypToTest.Reset();
    c4BytesDone = crypToTest.c4Decrypt(mbufCypher, strToUse, c4BytesToDo, pc1DecIV);

    // If the mode was CBC_IV, then get rid of extra block
    if (crypToTest.eMode() == tCIDCrypto::EBlockModes::CBC_IV)
        c4BytesToDo -= c4BlockSize;

    if (c4BytesDone != c4BytesToDo)
    {
        strmOut << TFWCurLn << L"Bytes decrypted was wrong. Expected: "
                << c4BytesToDo << L" but got: " << c4BytesDone
                << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // And make sure the text is the same
    if (strToUse != pszPlainText)
    {
        strmOut << TFWCurLn << L"Decrypted text not equal to original"
                << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }
    return tTestFWLib::ETestRes::Success;
}




// ---------------------------------------------------------------------------
//  CLASS: TCryptoTestApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TCryptoTestApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TCryptoTestApp::TCryptoTestApp()
{
}

TCryptoTestApp::~TCryptoTestApp()
{
}


// ----------------------------------------------------------------------------
//  TCryptoTestApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TCryptoTestApp::bInitialize(TString&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TCryptoTestApp::LoadTests()
{
    AddTest(new TTest_SChan1);

    // Load up our tests on our parent class
    AddTest(new TTest_MD51);
    AddTest(new TTest_SHA1_1);
    AddTest(new TTest_SHA256_1);
    AddTest(new TTest_HMACSHA256);
    AddTest(new TTest_UniqueId1);
    AddTest(new TTest_AES1);
    AddTest(new TTest_AES2);
    AddTest(new TTest_AES3);
    AddTest(new TTest_Blowfish1);
    AddTest(new TTest_XOR1);
}

tCIDLib::TVoid TCryptoTestApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TCryptoTestApp::PreTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TCryptoTestApp::Terminate()
{
    // Nothing to do
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TCryptoTestApp   tfwappCrypto;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TCryptoTestApp>(&tfwappCrypto, &TCryptoTestApp::eTestThread)
    )
)

