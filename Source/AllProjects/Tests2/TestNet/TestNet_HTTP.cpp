//
// FILE NAME: TestNet_HTTP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/02/2007
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
//  This file does some HTTP client side oriented tests
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
#include    "TestNet.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_HTTPAuthParse,TTestFWTest)
RTTIDecls(TTest_HTTPGET,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_HTTPAuthParse
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_HTTPAuthParse: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_HTTPAuthParse::TTest_HTTPAuthParse() :

    TTestFWTest
    (
        L"HTTP Auth Parse", L"Tests parsing of HTTP authentication header lines", 4
    )
{
}

TTest_HTTPAuthParse::~TTest_HTTPAuthParse()
{
}


// ---------------------------------------------------------------------------
//  TTest_HTTPAuthParse: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_HTTPAuthParse::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Set up some lines to test and the expected values
    TTestInfo aTests[] =
    {
        {
            kCIDLib::True
            , L"Digest   realm=\"CQC Automation System\",algorithm=\"MD5\",  qop=auth, domain=\"/Secure/secure.html\",nonce=\"B399C6E03ED91F6BBFE8D02E4E50467A\",opaque=\"1234567890\""
            , L"CQC Automation System"
            , L"MD5"
            , L"/Secure/secure.html"
            , L"B399C6E03ED91F6BBFE8D02E4E50467A"
            , L"1234567890"
            , tCIDNet::EHTTPAuthTypes::Digest
        }
      , {
            kCIDLib::True
            , L"Basic   realm=Test"
            , L"Test"
            , L""
            , L""
            , L""
            , L""
            , tCIDNet::EHTTPAuthTypes::Basic
        }

        // Negative tests
      , {
            kCIDLib::False
            , L"Basic   realm"
            , L""
            , L""
            , L""
            , L""
            , L""
            , tCIDNet::EHTTPAuthTypes::Basic
        }
      , {
            kCIDLib::False
            , L"Basic   realm=\"Unterminated Quote"
            , L""
            , L""
            , L""
            , L""
            , L""
            , tCIDNet::EHTTPAuthTypes::Basic
        }
    };
    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(aTests);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TTestInfo& CurTest = aTests[c4Index];

        if (!bCheckTest(strmOut, CurTest, c4Index))
            eRes = tTestFWLib::ETestRes::Failed;
    }
    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_HTTPAuthParse: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TTest_HTTPAuthParse::bCheckTest(        TTextStringOutStream&   strmOut
                                , const TTestInfo&              ToCheck
                                , const tCIDLib::TCard4         c4Index)
{
    tCIDNet::EHTTPAuthTypes eType;
    TString                 strAlgorithm;
    TString                 strDomain;
    TString                 strNonce;
    TString                 strOpaque;
    TString                 strRealm;
    TString                 strReqLine;

    try
    {
        THTTPClient::ParseAuthReq
        (
            ToCheck.pszToParse
            , eType
            , strRealm
            , strNonce
            , strOpaque
            , strDomain
            , strAlgorithm
        );
    }

    catch(const TError& errToCatch)
    {
        // If it should have worked, then it failed
        if (ToCheck.bShouldWork)
        {
            TModule::LogEventObj(errToCatch);
            strmOut << L"Exception occurred in test. Index=" << c4Index
                    << kCIDLib::DNewLn;
            return kCIDLib::False;
        }

        // Otherwise it did what it should have
        return kCIDLib::True;
    }

    if (strRealm != ToCheck.pszRealm)
    {
        strmOut << TFWCurLn << L"Realm text is not correct. Index="
                << c4Index << L"\n\n";
        return kCIDLib::False;
    }

    if (strNonce != ToCheck.pszNonce)
    {
        strmOut << TFWCurLn << L"Nonce text is not correct. Index="
                << c4Index << L"\n\n";
        return kCIDLib::False;
    }

    if (strOpaque != ToCheck.pszOpaque)
    {
        strmOut << TFWCurLn << L"Opaque text is not correct. Index="
                << c4Index << L"\n\n";
        return kCIDLib::False;
    }

    if (strAlgorithm != ToCheck.pszAlgorithm)
    {
        strmOut << TFWCurLn << L"Algorithm text is not correct. Index="
                << c4Index << L"\n\n";
        return kCIDLib::False;
    }

    if (ToCheck.eType != eType)
    {
        strmOut << TFWCurLn << L"Authenticate type is not correct. Index="
                << c4Index << L"\n\n";
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_HTTPGET
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_HTTPGET: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_HTTPGET::TTest_HTTPGET() :

    TTestFWTest
    (
        L"HTTP GET", L"Tests Basic HTTP GET operations, sync and async", 4
    )
{
}

TTest_HTTPGET::~TTest_HTTPGET()
{
}


// ---------------------------------------------------------------------------
//  TTest_HTTPGET: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_HTTPGET::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Try a couple async queries
    TAsyncHTTPQ ahttpqTest;

    // The iniital state should be complete
    if (ahttpqTest.eState() != tCIDLib::EAsyncWaitRes::Complete)
    {
        strmOut << TFWCurLn << L"Initial async wait state was not 'complete'\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Start up a GET operation
    //
    if (bDoGET(strmOut, ahttpqTest, L"http://www.google.com", L"Test 1"))
    {
        // That worked, so try another one
        if (!bDoGET(strmOut, ahttpqTest, L"http://www.microsoft.com", L"Test 2"))
            eRes = tTestFWLib::ETestRes::Failed;
    }
     else
    {
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do one we know should fail
    if (!bDoGET(strmOut, ahttpqTest, L"http://SomeBogusAddr.net.com.goober", L"Fail Test", kCIDLib::True))
    {
        strmOut << TFWCurLn << L"Test should have failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_HTTPGET: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TTest_HTTPGET::bDoGET(          TTextStringOutStream&   strmOut
                        ,       TAsyncHTTPQ&            ahttpqTest
                        , const TString&                strURL
                        , const TString&                strTestName
                        , const tCIDLib::TBoolean       bShouldFail)
{
    try
    {
        TURL urlTest(strURL, tCIDSock::EQualified::Full);
        tCIDLib::TKVPList colInHdrLines;
        colInHdrLines.objAdd(TKeyValuePair(L"Connection", L"Close"));
        ahttpqTest.StartGETRedir
        (
            strTestName
            , urlTest
            , 5000
            , L"Async HTTP Test Client"
            , L"text/html"
            , colInHdrLines
        );

        // Check the status until it changes to not waiting
        while (kCIDLib::True)
        {
            if (ahttpqTest.eState() != tCIDLib::EAsyncWaitRes::Waiting)
                break;
            TThread::Sleep(250);
        }

        if (ahttpqTest.eState() == tCIDLib::EAsyncWaitRes::Complete)
        {
            // Get the results
            try
            {
                tCIDNet::EHTTPCodes eCodeType;
                TString             strRepText;
                tCIDLib::TKVPList   colOutHdrLines;
                TString             strContType;
                THeapBuf            mbufCont;
                tCIDLib::TCard4     c4ContLen;
                TString             strUltimateURL;

                const tCIDLib::TCard4 c4ResCode = ahttpqTest.c4GetOutput
                (
                    eCodeType
                    , strRepText
                    , colOutHdrLines
                    , strContType
                    , mbufCont
                    , c4ContLen
                    , strUltimateURL
                );

                if ((c4ResCode != 200) && !bShouldFail)
                {
                    strmOut << TFWCurLn << L"Got HTTP code " << c4ResCode
                            << L" from async GET. Test="
                            << strTestName << L"\n\n";
                    return kCIDLib::False;
                }

                if ((c4ContLen == 0) && !bShouldFail)
                {
                    strmOut << TFWCurLn << L"Got zero bytes from async GET. Test="
                            << strTestName << L"\n\n";
                    return kCIDLib::False;
                }
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }
        }
         else if (ahttpqTest.eState() == tCIDLib::EAsyncWaitRes::Error)
        {
            if (!bShouldFail)
            {
                TModule::LogEventObj(ahttpqTest.errLast());
                strmOut << TFWCurLn << L"Got error from async GET. Test="
                        << strTestName << L"\n\n";
            }

            // If should fail, then failing is an success, else vice versa
            return bShouldFail;
        }
         else
        {
            strmOut << TFWCurLn << L"Unknown async HTTP result ("
                    << TCardinal(tCIDLib::c4EnumOrd(ahttpqTest.eState()))
                    << L"). Test=" << strTestName << L"\n\n";
            return kCIDLib::False;
        }
    }

    catch (TError& errToCatch)
    {
        // If this one shouldn't fail, then this is a failure
        if (!bShouldFail)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            strmOut << TFWCurLn << L"Exception in async GET. Test="
                    << strTestName << L"\n\n";

            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


