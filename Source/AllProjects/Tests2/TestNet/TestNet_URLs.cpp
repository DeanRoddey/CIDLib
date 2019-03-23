//
// FILE NAME: TestNet_URLs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/02/2007
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
//  This file does some URL oriented tests
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
RTTIDecls(TTest_URLBuild,TTestFWTest)
RTTIDecls(TTest_URLParse,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_URLBuild
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_URLBuild: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_URLBuild::TTest_URLBuild() :

    TTestFWTest
    (
        L"URL Building", L"Tests the BuildURL method", 5
    )
{
}

TTest_URLBuild::~TTest_URLBuild()
{
}


// ---------------------------------------------------------------------------
//  TTest_URLBuild: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_URLBuild::eRunTest(TTextStringOutStream&  strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Some query parameters to test with
    tCIDLib::TKVPList colQPs;
    colQPs.objAdd(TKeyValuePair(L"Test", L"Test Value"));

    // Test #1
    {
        // Should not require any encoding since no illegal chars
        TString strURL;
        TURL::BuildURL
        (
            L"http"
            , TString::strEmpty()
            , TString::strEmpty()
            , L"www.bongo.com"
            , 80
            , L"/query/conditions"
            , TString::strEmpty()
            , TString::strEmpty()
            , &colQPs
            , strURL
            , tCIDSock::EExpOpts::Encode
        );

        if (strURL != L"http://www.bongo.com/query/conditions?Test=Test+Value")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"BuildURL test #1 failed"
                    << kCIDLib::DNewLn;
        }

        // Test it parses back in
        TURL urlTest(strURL, tCIDSock::EQualified::Full);
    }

    // Test #2
    {
        // Requires encoding for query param value wiht a semicolon
        colQPs.objAdd(TKeyValuePair(L"Test2", L"New;Value"));

        TString strURL;
        TURL::BuildURL
        (
            L"http"
            , TString::strEmpty()
            , TString::strEmpty()
            , L"localhost"
            , 8080
            , L"/setvalue"
            , TString::strEmpty()
            , L"myfrag"
            , &colQPs
            , strURL
            , tCIDSock::EExpOpts::Encode
        );

        if (strURL != L"http://localhost:8080/setvalue?Test=Test+Value&Test2=New%3BValue#myfrag")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"BuildURL test #2 failed"
                    << kCIDLib::DNewLn;
        }

        // Test it parses back in
        TURL urlTest(strURL, tCIDSock::EQualified::Full);
    }

    // Test #3
    {
        // Doesn't require encoding but should be the same either way
        TString strURL;
        TURL::BuildURL
        (
            L"http"
            , L"bubba"
            , L"gumption"
            , L"localhost"
            , 0
            , L"/setvalue"
            , L"p1=1;p2=2"
            , L"myfrag"
            , 0
            , strURL
            , tCIDSock::EExpOpts::Encode
        );

        if (strURL != L"http://bubba:gumption@localhost/setvalue;p1=1;p2=2#myfrag")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"BuildURL test #3 failed"
                    << kCIDLib::DNewLn;
        }

        // Test it parses back in
        TURL urlTest(strURL, tCIDSock::EQualified::Full);
    }

    return eRes;
}






// ---------------------------------------------------------------------------
//  CLASS: TTest_URLParse
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_URLParse: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_URLParse::TTest_URLParse() :

    TTestFWTest
    (
        L"URL Parsing", L"Tests parsing of various URL scenarios", 5
    )
{
}

TTest_URLParse::~TTest_URLParse()
{
}


// ---------------------------------------------------------------------------
//  TTest_URLParse: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_URLParse::eRunTest(TTextStringOutStream&  strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test the parsing of URL query parms
    {
        const TString strTest(L"Value1=1&Value2=Another+1&Value3=&");
        tCIDLib::TKVPList colValues;

        const tCIDLib::TCard4 c4Count = TURL::c4ParseQueryParms
        (
            strTest
            , colValues
            , kCIDLib::True
            , TURL::EExpTypes::Query
        );

        if (c4Count != 3)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Got wrong query key count" << kCIDLib::EndLn;
        }

        if ((colValues[0].strValue() != L"1")
        ||  (colValues[1].strValue() != L"Another 1")
        ||  (colValues[2].strValue() != L""))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Got bad query values\n\n";
        }
    }

    //
    //  We need to set up a set of tests. We define a single parent URL, and
    //  then each test has a relative part and a resulting full URL.
    //
    //  Most of these test strings were lifted from the RFC.
    //
    {
        struct TURLTest
        {
            const tCIDLib::TCh* const   pszRelative;
            const tCIDLib::TCh* const   pszResult;
        };

        TURLTest aTests[] =
        {
            { L"g"          , L"http://a/b/c/g" }
          , { L"./g"        , L"http://a/b/c/g" }
          , { L"g/"         , L"http://a/b/c/g/" }
          , { L"/g"         , L"http://a/g" }
          , { L"?y"         , L"http://a/b/c/d;p?y" }
          , { L"g?y"        , L"http://a/b/c/g?y" }
          , { L"#s"         , L"http://a/b/c/d;p?q#s" }
          , { L"g#s"        , L"http://a/b/c/g#s" }
          , { L"g?y#s"      , L"http://a/b/c/g?y#s" }
          , { L";x"         , L"http://a/b/c/;x" }
          , { L"g;x"        , L"http://a/b/c/g;x" }
          , { L"g;x?y#s"    , L"http://a/b/c/g;x?y#s" }
          , { L"."          , L"http://a/b/c/" }
          , { L"./"         , L"http://a/b/c/" }
          , { L".."         , L"http://a/b/" }
          , { L"../"        , L"http://a/b/" }
          , { L"../g"       , L"http://a/b/g" }
          , { L"../.."      , L"http://a/" }
          , { L"../../"     , L"http://a/" }
          , { L"../../g"    , L"http://a/g" }
        };
        static const tCIDLib::TCard4 c4TestCount = tCIDLib::c4ArrayElems(aTests);

        // This is our parent URL from which all the tests work
        const tCIDLib::TCh* const pszParURL = L"http://a/b/c/d;p?q";
        TURL urlParent(pszParURL, tCIDSock::EQualified::Full);

        TString strFull;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            // Test it via construction
            {
                TURL urlTest(urlParent, aTests[c4Index].pszRelative);
                urlTest.QueryText(strFull, kCIDLib::False, kCIDLib::True);
                if (strFull != aTests[c4Index].pszResult)
                {
                    eRes = tTestFWLib::ETestRes::Failed;
                    strmOut << TFWCurLn << L"Got \""
                            << strFull
                            << L"\" but expected: " << aTests[c4Index].pszResult
                            << kCIDLib::DNewLn;
                }
            }

            // Test via a reset
            {
                TURL urlTest2;
                urlTest2.Reset(urlParent, aTests[c4Index].pszRelative);
                urlTest2.QueryText(strFull, kCIDLib::False, kCIDLib::True);
                if (strFull != aTests[c4Index].pszResult)
                {
                    eRes = tTestFWLib::ETestRes::Failed;
                    strmOut << TFWCurLn << L"Got \""
                            << strFull
                            << L"\" but expected: " << aTests[c4Index].pszResult
                            << kCIDLib::DNewLn;
                }
            }

            // And a reset from the original text for the base URL
            {
                TURL urlTest3;
                urlTest3.Reset(pszParURL, aTests[c4Index].pszRelative);
                urlTest3.QueryText(strFull, kCIDLib::False, kCIDLib::True);
                if (strFull != aTests[c4Index].pszResult)
                {
                    eRes = tTestFWLib::ETestRes::Failed;
                    strmOut << TFWCurLn << L"Got \""
                            << strFull
                            << L"\" but expected: " << aTests[c4Index].pszResult
                            << kCIDLib::DNewLn;
                }
            }
        }
    }

    //
    //  Set up a set of test structures that we can run in a loop below to
    //  insure that we are getting out what we expect from various format
    //  URLs.
    //
    {
        TURLInfo aTests[] =
        {
            {
                L"http://Bubba.com/The/Darned/File.Txt?TheQuery"
                , tCIDSock::EProtos::HTTP
                , 80
                , 0
                , L"Bubba.com"
                , 0
                , L"/The/Darned/File.Txt"
                , L"TheQuery"
                , 0
                , L"http://Bubba.com/The/Darned/File.Txt?TheQuery"
                , 0
                , 0
            }
          , {
                L"ftp://user:password@bubba.com/The/Darned/File.Txt#FragmentPart"
                , tCIDSock::EProtos::FTP
                , 21
                , L"FragmentPart"
                , L"bubba.com"
                , L"password"
                , L"/The/Darned/File.Txt"
                , 0
                , L"user"
                , L"ftp://user:password@bubba.com/The/Darned/File.Txt#FragmentPart"
                , L"ftp://bubba.com/The/Darned/File.Txt#FragmentPart"
                , 0
            }
          , {
                L"HTTP://user@bubba.com:90#FragmentPart"
                , tCIDSock::EProtos::HTTP
                , 90
                , L"FragmentPart"
                , L"bubba.com"
                , 0
                , 0
                , 0
                , L"user"
                , L"http://user@bubba.com:90#FragmentPart"
                , L"http://bubba.com:90#FragmentPart"
                , 0
            }
          , {
                L"http://user_name:password@bubba.com:1001/The/Darned/File.Txt?The+Query+Part"
                , tCIDSock::EProtos::HTTP
                , 1001
                , 0
                , L"bubba.com"
                , L"password"
                , L"/The/Darned/File.Txt"
                , L"The+Query+Part"
                , L"user_name"
                , L"http://user_name:password@bubba.com:1001/The/Darned/File.Txt?The+Query+Part"
                , L"http://bubba.com:1001/The/Darned/File.Txt?The+Query+Part"
                , L"http://user_name:password@bubba.com:1001/The/Darned/File.Txt?The Query Part"
            }
          , {
                L"file:///C:/SomePath/SomeFile.Txt"
                , tCIDSock::EProtos::File
                , 0
                , 0
                , 0
                , 0
                , L"/C:/SomePath/SomeFile.Txt"
                , 0
                , 0
                , L"file:///C:/SomePath/SomeFile.Txt"
                , 0
                , 0
            }
          , { // 5
                L"file://myhost.com/SomePath/SomeFile.Txt"
                , tCIDSock::EProtos::File
                , 0
                , 0
                , L"myhost.com"
                , 0
                , L"/SomePath/SomeFile.Txt"
                , 0
                , 0
                , L"file://myhost.com/SomePath/SomeFile.Txt"
                , 0
                , 0
            }
          , {
                L"http:///The%20Darned%7EFile.Txt"
                , tCIDSock::EProtos::HTTP
                , 80
                , 0
                , 0
                , 0
                , L"/The%20Darned%7EFile.Txt"
                , 0
                , 0
                , L"http:///The%20Darned%7EFile.Txt"
                , 0
                , L"http:///The Darned~File.Txt"
            }
          , {
                L"http:///The%C3%A8FileWithUTF8Exp.Txt"
                , tCIDSock::EProtos::HTTP
                , 80
                , 0
                , 0
                , 0
                , L"/The%C3%A8FileWithUTF8Exp.Txt"
                , 0
                , 0
                , L"http:///The%C3%A8FileWithUTF8Exp.Txt"
                , 0
                , 0
            }
          , {
                L"http://192.168.0.1/gett1.cgi"
                , tCIDSock::EProtos::HTTP
                , 80
                , 0
                , L"192.168.0.1"
                , 0
                , L"/gett1.cgi"
                , 0
                , 0
                , L"http://192.168.0.1/gett1.cgi"
                , 0
                , 0
            }
          , {
                L"https://192.168.0.1/gett1.cgi"
                , tCIDSock::EProtos::HTTPS
                , 443
                , 0
                , L"192.168.0.1"
                , 0
                , L"/gett1.cgi"
                , 0
                , 0
                , L"https://192.168.0.1/gett1.cgi"
                , 0
                , 0
            }
          , { // 10
                L"mailto:bubba@gump.com"
                , tCIDSock::EProtos::MailTo
                , 25
                , 0
                , L"gump.com"
                , 0
                , 0
                , 0
                , L"bubba"
                , L"mailto:bubba@gump.com"
                , 0
                , 0
            }
          , {
                L"sip:jane@doe.com"
                , tCIDSock::EProtos::SIP
                , 5060
                , 0
                , L"doe.com"
                , 0
                , 0
                , 0
                , L"jane"
                , L"sip:jane@doe.com"
                , 0
                , 0
            }
          , {
                L"http://wango.com/incidents?count=15&series=A%26E"
                , tCIDSock::EProtos::HTTP
                , 80
                , 0
                , L"wango.com"
                , 0
                , L"/incidents"
                , L"count=15&series=A%25E"
                , 0
                , L"http://wango.com/incidents?count=15&series=A%26E"
                , 0
                , L"http://wango.com/incidents?count=15&series=A&E"
            }
          , {
                L"/incidents?count=15"
                , tCIDSock::EProtos::None
                , 0
                , 0
                , 0
                , 0
                , L"/incidents"
                , L"count=15"
                , 0
                , L"/incidents?count=15"
                , 0
                , L"/incidents?count=15"
            }
          , {
                L"http://[::1]:8888/Testing"
                , tCIDSock::EProtos::HTTP
                , 8888
                , 0
                , L"::1"
                , 0
                , L"/Testing"
                , 0
                , 0
                , L"http://[::1]:8888/Testing"
                , 0
                , L"http://[::1]:8888/Testing"
            }
          , { // A really psycho one
                L"http://example.com/:@-._~!$&'()*+,=;:@-._~!$&'()*+,=:@-._~!$&'()*+,==?/?:@-._~!$'()*+,;=/?:@-._~!$'()*+,;==#/?:@-._~!$&'()*+,;="
                , tCIDSock::EProtos::HTTP
                , 80
                , L"/?:@-._~!$&'()*+,;="
                , L"example.com"
                , 0
                , L"/:@-._~!$&'()*+,="
                , L"/?:@-._~!$'()* ,;=/?:@-._~!$'()* ,;=="
                , 0
                , L"http://example.com/:@-._~!$&'()*+,=;:@-._~!$&'()*+,=:@-._~!$&'()*+,==?/?:@-._~!$'()*+,;=/?:@-._~!$'()*+,;==#/?:@-._~!$&'()*+,;="
                , 0
                , L"http://example.com/:@-._~!$&'()*+,=;:@-._~!$&'()*+,=:@-._~!$&'()*+,==?/?:@-._~!$'()* ,;=/?:@-._~!$'()* ,;==#/?:@-._~!$&'()*+,;="
            }
          , {
                L"http://192.168.1.30:9000/googlemusicimage/lh3.ggpht.com/TlXDfDGxnMGiV6g9Q-Ymuw6h9h7M5gykVCq0YCJwRLwOLC92B3v7jjN9xpi77ARiEpUt tQV4/image_96x96_p.jpg"
                , tCIDSock::EProtos::HTTP
                , 9000
                , 0
                , L"192.168.1.30"
                , 0
                , L"/googlemusicimage/lh3.ggpht.com/TlXDfDGxnMGiV6g9Q-Ymuw6h9h7M5gykVCq0YCJwRLwOLC92B3v7jjN9xpi77ARiEpUt tQV4/image_96x96_p.jpg"
                , 0
                , 0
                , L"http://192.168.1.30:9000/googlemusicimage/lh3.ggpht.com/TlXDfDGxnMGiV6g9Q-Ymuw6h9h7M5gykVCq0YCJwRLwOLC92B3v7jjN9xpi77ARiEpUt tQV4/image_96x96_p.jpg"
                , 0
                , 0
            }
        };
        static const tCIDLib::TCard4 c4TestCount = tCIDLib::c4ArrayElems(aTests);

        // Run through all of the tests
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
        {
            // Get a reference to the current test and construct a URL for it
            const TURLInfo& curTest = aTests[c4Index];
            {
                TURL urlTest(curTest.pszURL, tCIDSock::EQualified::DontCare);
                TestRes(urlTest, curTest, strmOut, eRes, c4Index);
            }

            // Do the same thing by way of resetting the URL
            {
                TURL urlTest2;
                urlTest2.Reset(curTest.pszURL, tCIDSock::EQualified::DontCare);
                TestRes(urlTest2, curTest, strmOut, eRes, c4Index);
            }
        }
    }

    //
    //  Make sure that if we set up a URL relative to a base URL, but the
    //  new one is fully qualified, we should get the new one completely.
    //
    {
        const tCIDLib::TCh* const pszNewURL = L"http://www.ganga.com/Test2";
        const tCIDLib::TCh* const pszBaseURL = L"http://www.gumbo.com/Test1";

        {
            TURL urlTest(pszBaseURL, pszNewURL);

            TString strFull;
            urlTest.QueryText(strFull, kCIDLib::False, kCIDLib::False);
            if (strFull != pszNewURL)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Should have gotten new URL\n\n";
            }
        }

        // Do it via reset
        {
            TURL urlTest;
            urlTest.Reset(pszBaseURL, pszNewURL);

            TString strFull;
            urlTest.QueryText(strFull, kCIDLib::False, kCIDLib::False);
            if (strFull != pszNewURL)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Should have gotten new URL\n\n";
            }

        }

        // Do it via reset over a already parsed base URL
        {
            TURL urlBase(pszBaseURL, tCIDSock::EQualified::Full);
            TURL urlTest;
            urlTest.Reset(urlBase, pszNewURL);

            TString strFull;
            urlTest.QueryText(strFull, kCIDLib::False, kCIDLib::False);
            if (strFull != pszNewURL)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Should have gotten new URL\n\n";
            }

        }
    }

    //
    //  Test the extraction of the alternate component parts extraction
    //  stuff.
    //
    {
        TURL urlTest
        (
            L"http://gee:wiz@www.test.com:8080/path1/path2;xy?qp1=2&qp2=3#superfrag"
            , tCIDSock::EQualified::Full
        );

        TString strProto;
        TString strUser;
        TString strHost;
        TString strPath;
        TString strSuffix;

        urlTest.QueryEncodedForm(strProto, strUser, strHost, strPath, strSuffix);
        if ((strProto != L"http://")
        ||  (strUser != L"gee:wiz@")
        ||  (strHost != L"www.test.com:8080")
        ||  (strPath != L"/path1/path2;xy")
        ||  (strSuffix != L"?qp1=2&qp2=3#superfrag"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Incorrect encoded form\n\n";
        }

        // And the more broken out form
        TString strPass;
        tCIDLib::TIPPortNum ippnHost;
        TString strParams;
        TString strFrag;
        TString strQPs;
        urlTest.QueryEncodedForm
        (
            strProto
            , strUser
            , strPass
            , strHost
            , ippnHost
            , strPath
            , strParams
            , strFrag
            , strQPs
        );
        if ((strProto != L"http")
        ||  (strUser != L"gee")
        ||  (strPass != L"wiz")
        ||  (strHost != L"www.test.com")
        ||  (ippnHost != 8080)
        ||  (strPath != L"/path1/path2")
        ||  (strParams != L"xy")
        ||  (strFrag != L"superfrag")
        ||  (strQPs != L"qp1=2&qp2=3"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Incorrect encoded form\n\n";
        }

        // And do the expanded form
        urlTest.Reset
        (
            L"http://gee:wiz@www.test.com:902/path1/path2%3C;xy%5B?qp1=2&qp2=%2A+3#superfrag"
            , tCIDSock::EQualified::Full
        );
        urlTest.QueryExpandedForm
        (
            strProto
            , strUser
            , strPass
            , strHost
            , ippnHost
            , strPath
            , strParams
            , strFrag
            , strQPs
        );
        if ((strProto != L"http")
        ||  (strUser != L"gee")
        ||  (strPass != L"wiz")
        ||  (strHost != L"www.test.com")
        ||  (ippnHost != 902)
        ||  (strPath != L"/path1/path2<")
        ||  (strParams != L"xy[")
        ||  (strFrag != L"superfrag")
        ||  (strQPs != L"qp1=2&qp2=* 3"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Incorrect expanded form\n\n";
        }
    }
    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_URLParse: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper for the tests above, to do some
//
tCIDLib::TVoid
TTest_URLParse::TestRes(const   TURL&                   urlTest
                        , const TURLInfo&               curTest
                        ,       TTextStringOutStream&   strmOut
                        ,       tTestFWLib::ETestRes&   eRes
                        , const tCIDLib::TCard4         c4TestInd)
{
    //
    //  Now test all of the components of it that are stored in the current
    //  test structure.
    //
    if (urlTest.eProto() != curTest.eProto)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Scheme was not correct (" << c4TestInd << L")\n\n";
    }

    if (urlTest.ippnHost() != curTest.ippnHost)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Port number was not correct (" << c4TestInd << L")\n\n";
    }

    if (!bTestStrs(urlTest.strFragment(), curTest.pszFragment))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Fragment part was not correct (" << c4TestInd << L")\n\n";
    }

    if (!bTestStrs(urlTest.strHost(), curTest.pszHost))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Host part was not correct (" << c4TestInd << L")\n\n";
    }

    if (!bTestStrs(urlTest.strPassword(), curTest.pszPassword))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Password part was not correct (" << c4TestInd << L")\n\n";
    }

    if (!bTestStrs(urlTest.strPath(), curTest.pszPath))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Path part was not correct (" << c4TestInd << L")\n\n";
    }

    if (!bTestStrs(urlTest.strUser(), curTest.pszUser))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"User part was not correct (" << c4TestInd << L")\n\n";
    }

    //
    //  Make sure the full URL text (which should be canonicalized, matches
    //  what it should be, in multiple forms. Expanded and encoded, and with
    //  and without security info (user/pass.)
    //
    TString strFull;
    urlTest.QueryText(strFull, kCIDLib::False, kCIDLib::True);
    if (!bTestStrs(strFull, curTest.pszFull))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Full URL was not correct ("
                << c4TestInd << L")" << kCIDLib::DNewLn;
    }

    // If there's non-security info version, check that
    if (curTest.pszFullNS)
    {
        urlTest.QueryText(strFull, kCIDLib::False, kCIDLib::False);

        if (!bTestStrs(strFull, curTest.pszFullNS))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Full NS URL was not correct ("
                    << c4TestInd << L")" << kCIDLib::DNewLn;
        }
    }

    // If there's an expanded version, test that
    if (curTest.pszFullExp)
    {
        urlTest.QueryText(strFull, kCIDLib::True, kCIDLib::True);

        if (!bTestStrs(strFull, curTest.pszFullExp))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Full exp URL was not correct ("
                    << c4TestInd << L")" << kCIDLib::DNewLn;
        }
    }
}


tCIDLib::TBoolean
TTest_URLParse::bTestStrs(const TString& strOne, const tCIDLib::TCh* const pszTwo)
{
    if (strOne.bIsEmpty() && !pszTwo)
        return kCIDLib::True;

    if (strOne.bIsEmpty() || !pszTwo)
        return kCIDLib::False;

    if (!strOne.bCompare(pszTwo))
        return kCIDLib::False;
    return kCIDLib::True;
}


