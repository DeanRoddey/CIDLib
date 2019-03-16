//
// FILE NAME: TestNet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the main header file of the CIDLib network clases test app. It
//  tests URLs, socket, TCPIP and other related classes.
//
//  This is a standard CIDLib test framework test app. All of our tests as
//  fairly small so they are all in a single _Tests file.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "CIDNet.hpp"
#include    "TestFWLib.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TTest_HTTPAuthParse
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_HTTPAuthParse : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        struct TTestInfo
        {
            tCIDLib::TBoolean       bShouldWork;
            const tCIDLib::TCh*     pszToParse;
            const tCIDLib::TCh*     pszRealm;
            const tCIDLib::TCh*     pszAlgorithm;
            const tCIDLib::TCh*     pszDomain;
            const tCIDLib::TCh*     pszNonce;
            const tCIDLib::TCh*     pszOpaque;
            tCIDNet::EHTTPAuthTypes eType;
        };


        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_HTTPAuthParse();

        ~TTest_HTTPAuthParse();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckTest
        (
                    TTextStringOutStream&   strmOutput
            , const TTestInfo&              ToCheck
            , const tCIDLib::TCard4         c4Index
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_HTTPAuthParse,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_HTTPGET
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_HTTPGET : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_HTTPGET();

        ~TTest_HTTPGET();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDoGET
        (
                    TTextStringOutStream&   strmOut
            ,       TAsyncHTTPQ&            ahttpqTest
            , const TString&                strURL
            , const TString&                strTestName
            , const tCIDLib::TBoolean       bShouldFail = kCIDLib::False
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_HTTPGET,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_EndPoints
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_EndPoints : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_EndPoints();

        ~TTest_EndPoints();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestAnAddress
        (
                    TTextStringOutStream&   strmOutput
            ,       TIPAddress&             ipaToTest
            , const TString&                strAddress
            , const tCIDLib::TCard1         ac1Data[16]
            , const tCIDLib::TCard4         c4DataSz
            , const tCIDSock::EAddrTypes    eType
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_EndPoints,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_JSON1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_JSON1();

        ~TTest_JSON1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_JSON1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_JSON2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_JSON2();

        ~TTest_JSON2();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_JSON2,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_JSON3 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_JSON3();

        ~TTest_JSON3();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_JSON3,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON4
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_JSON4 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_JSON4();

        ~TTest_JSON4();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_JSON4,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON5
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_JSON5 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_JSON5();

        ~TTest_JSON5();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_JSON5,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TNeTTest_MPMIMEDecode1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_MPMIMEDecode1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_MPMIMEDecode1();

        ~TTest_MPMIMEDecode1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_MPMIMEDecode1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TNeTTest_URLParse
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_URLParse : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_URLParse();

        ~TTest_URLParse();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        struct TURLInfo
        {
            tCIDLib::TCh*       pszURL;

            tCIDSock::EProtos   eProto;
            tCIDLib::TIPPortNum ippnHost;
            tCIDLib::TCh*       pszFragment;
            tCIDLib::TCh*       pszHost;
            tCIDLib::TCh*       pszPassword;
            tCIDLib::TCh*       pszPath;
            tCIDLib::TCh*       pszQuery;
            tCIDLib::TCh*       pszUser;
            tCIDLib::TCh*       pszFull;
            tCIDLib::TCh*       pszFullNS;
            tCIDLib::TCh*       pszFullExp;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestStrs
        (
            const   TString&                strOne
            , const tCIDLib::TCh* const     pszTwo
        );

        tCIDLib::TVoid TestRes
        (
            const   TURL&                   urlTest
            , const TURLInfo&               curTest
            ,       TTextStringOutStream&   strmOut
            ,       tTestFWLib::ETestRes&   eRes
            , const tCIDLib::TCard4         c4TestInd
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_URLParse,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TNeTTest_URLBuild
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_URLBuild : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_URLBuild();

        ~TTest_URLBuild();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_URLBuild,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TNetTest
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TNetTestApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TNetTestApp();

        TNetTestApp(const TNetTestApp&) = delete;

        ~TNetTestApp();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInitialize
        (
                    TString&                strErr
        )   override;

        tCIDLib::TVoid LoadTests() override;

        tCIDLib::TVoid PostTest
        (
            const   TTestFWTest&            tfwtFinished
        )   override;

        tCIDLib::TVoid PreTest
        (
            const   TTestFWTest&            tfwtStarting
        )   override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TNetTestApp,TTestFWApp)
};


