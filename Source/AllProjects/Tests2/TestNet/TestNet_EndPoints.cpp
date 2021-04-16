//
// FILE NAME: TestNet_EndPoints.cpp
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
RTTIDecls(TTest_EndPoints,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_EndPoints
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_EndPoints: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_EndPoints::TTest_EndPoints() :

    TTestFWTest
    (
        L"End Points", L"Tests IP end point and address classes", 5
    )
{
}

TTest_EndPoints::~TTest_EndPoints()
{
}


// ---------------------------------------------------------------------------
//  TTest_EndPoints: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_EndPoints::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    tCIDLib::TBoolean bRes;
    tCIDLib::TCard1 ac1Data[16];


    // Test the address class
    {
        //
        //  Create an IP address object via the dotted notation constructor.
        //  we can indicate unspecified because the format of the address should
        //  force it to be IPV4. Remember the data is in network order, so the
        //  bytes will be in the same order as the digits in the dotted form.
        //
        TIPAddress ipaTest(L"1.2.3.4", tCIDSock::EAddrTypes::Unspec);
        ac1Data[0] = 1; ac1Data[1] = 2; ac1Data[2] = 3; ac1Data[3] = 4;
        bRes = bTestAnAddress
        (
            strmOut, ipaTest, L"1.2.3.4", ac1Data, 4, tCIDSock::EAddrTypes::IPV4
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;

        // Test assignment operator
        TIPAddress ipaTest2(L"9.9.9.9", tCIDSock::EAddrTypes::Unspec);
        ipaTest2 = ipaTest;
        if (ipaTest2 != ipaTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Assignment operator failed\n\n";
        }

        // And test copy construction
        TIPAddress ipaTest3(ipaTest2);
        if (ipaTest != ipaTest3)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Copy constructor failed\n\n";
        }

        //
        //  Do one with an IPV6 address. We'll do the loopback. Indicate
        //  unspecified so it'll have to figure it out itself. Then do aother
        //  where we indicate it specifically.
        //
        TIPAddress ipaTest6(L"::1", tCIDSock::EAddrTypes::Unspec);
        TIPAddress ipaTest6b(L"::1", tCIDSock::EAddrTypes::IPV6);
        TRawMem::SetMemBuf(ac1Data, kCIDLib::c1MinCard, 16);
        ac1Data[15] = 1;

        bRes = bTestAnAddress
        (
            strmOut, ipaTest6, L"::1", ac1Data, 16, tCIDSock::EAddrTypes::IPV6
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;

        bRes = bTestAnAddress
        (
            strmOut, ipaTest6b, L"::1", ac1Data, 16, tCIDSock::EAddrTypes::IPV6
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;

        // And they should be equal
        if (ipaTest6 != ipaTest6b)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Two IPV6 addresses should have been equal\n\n";
        }
    }

    // Test the end point class
    {
        //
        //  Create an IP endpoint object via a string and test it for the
        //  underlying TIPAddress class stuff first. It derives from there
        //  so we can use the same testing function to test that part of it.
        //
        TIPEndPoint ipepTest(L"2.3.4.5", 99, tCIDSock::EAddrTypes::IPV4);
        ac1Data[0] = 2; ac1Data[1] = 3; ac1Data[2] = 4; ac1Data[3] = 5;

        (
            strmOut, ipepTest, L"2.3.4.5", ac1Data, 4, tCIDSock::EAddrTypes::IPV4
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;

        //
        //  And now do some extra tests that are end point specific. Start by
        //  checking that the port number comes back correctly and can be set
        //  and gotten again.
        //
        if (ipepTest.ippnThis() != 99)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Port set in ctor came back different\n\n";
        }

        ipepTest.ippnThis(100);
        if (ipepTest.ippnThis() != 100)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Port set via access method came back different\n\n";
        }

        // Create some others for in/equality
        TIPEndPoint ipepTest2(L"2.3.4.5", 500, tCIDSock::EAddrTypes::IPV4);
        TIPEndPoint ipepTest3(ipepTest);

        if (ipepTest != ipepTest3)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Inequality operator failed\n\n";
        }

        if (ipepTest == ipepTest2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Equality operator failed\n\n";
        }

        // Test out assignment
        ipepTest2 = ipepTest;
        if (ipepTest2 != ipepTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Assignment operator failed\n\n";
        }

        // Test copy construction
        TIPEndPoint ipepTest4(ipepTest);
        if (ipepTest4 != ipepTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"Copy constructor failed\n\n";
        }
    }

    // Do a round trip streaming test
    {
        // Create linked memory buffer streams
        TBinMBufOutStream strmOut(256UL);
        TBinMBufInStream  strmIn(strmOut);

        // Stream both types of end point out
        TIPEndPoint ipepOut4(facCIDSock().strIPHostName(), 30288, tCIDSock::EAddrTypes::IPV4);
        TIPEndPoint ipepOut6(facCIDSock().strIPHostName(), 30289, tCIDSock::EAddrTypes::IPV6);
        strmOut << ipepOut4 << ipepOut6 << kCIDLib::FlushIt;

        // Stream them back into new ones
        TIPEndPoint ipepIn4;
        TIPEndPoint ipepIn6;
        strmIn >> ipepIn4 >> ipepIn6;

        // And compare
        if (ipepIn4 != ipepOut4)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"V4 round trip stream failed\n\n";
        }

        if (ipepIn6 != ipepOut6)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << L"V6 round trip stream failed\n\n";
        }
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_EndPoints: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TTest_EndPoints::bTestAnAddress(        TTextStringOutStream&   strmOut
                                ,       TIPAddress&             ipaToTest
                                , const TString&                strAddress
                                , const tCIDLib::TCard1         ac1Data[16]
                                , const tCIDLib::TCard4         c4DataSz
                                , const tCIDSock::EAddrTypes    eType)
{
    // Check that the text is correct
    if (ipaToTest.strAsText() != strAddress)
    {
        strmOut << TFWCurLn << L"Address text is not correct\n\n";
        return kCIDLib::False;
    }

    // Make sure it's the expected type
    if (ipaToTest.eAddrType() != eType)
    {
        strmOut << TFWCurLn << L"The address family type was wrong\n\n";
        return kCIDLib::False;
    }

    //
    //  Check the count of address bytes. We have to go to the kernel
    //  level address to get this.
    //
    if (ipaToTest.kipaThis().c4Count() != c4DataSz)
    {
        strmOut << TFWCurLn << L"The address data was the wrong size\n\n";
        return kCIDLib::False;
    }

    //
    //  Test that the data bytes are corrrect
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4DataSz; c4Index++)
    {
        if (ipaToTest.kipaThis().pc1Data()[c4Index] != ac1Data[c4Index])
        {
            strmOut << TFWCurLn << L"Address byte " << (c4Index + 1)
                    << L" was not correct\n\n";
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


