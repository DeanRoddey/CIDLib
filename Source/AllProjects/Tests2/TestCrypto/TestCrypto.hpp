//
// FILE NAME: TestCrypto.hpp
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
//  This is the main header file of the CIDLib encryption test app. This is
//  a standard CIDLib test framework test app.
//
//  We want a little common functionality, so we define a base test class
//  derivative and derive our tests from that.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "CIDSock.hpp"
#include    "CIDCrypto.hpp"
#include    "CIDSChan.hpp"
#include    "TestFWLib.hpp"


// ---------------------------------------------------------------------------
//  Some global stuff
//
//  pchTest
//      A reasonable long text message used in some hashing and crypto tests.
// ---------------------------------------------------------------------------
namespace kTestCrypto
{
    static const tCIDLib::TCh* const pchTest =
    L"//  This file is part of a demonstration program of the CIDLib C++ Class"
    L"//  libraries. Its contents are distributed 'as is', to provide guidance on"
    L"//  the use of the CIDLib system. However, these demos are not intended to"
    L"//  represent a full fledged applications. Any direct use of demo code in"
    L"//  user applications is at the user's discretion, and no warranties are"
    L"//  implied as to its correctness or applicability.";
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_BaseCrypto
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BaseCrypto : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const tCIDLib::TCh* const pszPlainText;


        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TTest_BaseCrypto();


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TTest_BaseCrypto
        (
            const   TString&                strName
            , const TString&                strDescr
            , const tCIDLib::TCard4         c4Level
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eCommonBlockTests
        (
                    TBlockEncrypter&        crypToTest
            ,       TTextStringOutStream&   strmOutput
            , const tCIDLib::TCard1* const  pc1EncVec
            , const tCIDLib::TCard1* const  pc1DecVec
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_BaseCrypto,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_MD51
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_MD51 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_MD51();

        ~TTest_MD51();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_MD51,TTest_BaseCrypto)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SHA1_1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_SHA1_1 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_SHA1_1();

        ~TTest_SHA1_1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_SHA1_1,TTest_BaseCrypto)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SHA256_1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_SHA256_1 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_SHA256_1();

        ~TTest_SHA256_1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_SHA256_1,TTest_BaseCrypto)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_HMACSHA256
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_HMACSHA256 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_HMACSHA256();

        ~TTest_HMACSHA256();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_HMACSHA256,TTest_BaseCrypto)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_UniqueId1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_UniqueId1 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_UniqueId1();

        ~TTest_UniqueId1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_UniqueId1,TTest_BaseCrypto)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_AES1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_AES1 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_AES1();

        ~TTest_AES1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_AES1,TTest_BaseCrypto)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_AES2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_AES2 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_AES2();

        ~TTest_AES2();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_AES2,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_AES3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_AES3 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_AES3();

        ~TTest_AES3();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDoOne
        (
                    TTextStringOutStream&   strmOut
            , const tCIDCrypto::EBlockModes eMode
            , const tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCh* const     pszIV
            , const tCIDLib::TCh* const     pszPlain
            , const tCIDLib::TCh* const     pszCypher
        );

        tCIDLib::TCard1* pc1ConvertVal
        (
                    TTextStringOutStream&   strmOut
            , const tCIDLib::TCh* const     pszVal
            ,       tCIDLib::TCard4&        c4RetBytes
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_AES3,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_Blowfish1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Blowfish1 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Blowfish1();

        ~TTest_Blowfish1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eTestVectors
        (
                    TTextOutStream&         strmOut
            , const tCIDCrypto::EBlockModes eMode
            , const tCIDLib::TCard1* const  pc1EncIV
            , const tCIDLib::TCard1* const  pc1DecIV
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Blowfish1,TTest_BaseCrypto)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_XOR1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_XOR1 : public TTest_BaseCrypto
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_XOR1();

        ~TTest_XOR1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_XOR1,TTest_BaseCrypto)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SChan1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_SChan1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_SChan1();

        ~TTest_SChan1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_SChan1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TCryptoTest
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TCryptoTestApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TCryptoTestApp();

        TCryptoTestApp(const TCryptoTestApp&) = delete;
        TCryptoTestApp(TCryptoTestApp&&) = delete;

        ~TCryptoTestApp();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInitialize
        (
                    TString&                strErr
        )   final;

        tCIDLib::TVoid LoadTests() final;

        tCIDLib::TVoid PostTest
        (
            const   TTestFWTest&            tfwtFinished
        )   final;

        tCIDLib::TVoid PreTest
        (
            const   TTestFWTest&            tfwtStarting
        )   final;

        tCIDLib::TVoid Terminate() final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCryptoTestApp,TTestFWApp)
};


