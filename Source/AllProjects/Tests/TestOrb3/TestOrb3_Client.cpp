//
// FILE NAME: TestOrb3_Client.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module implements the client side of the test
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestOrb3.hpp"


// ----------------------------------------------------------------------------
//  Local, static data
// ----------------------------------------------------------------------------
namespace TestOrb3_Client
{
    TMutex*         pmtxSync;
    TOrbObjId*      pooidSrv1;
    TOrbObjId*      pooidSrv2;
    TTextOutStream* pstrmOut;
}


// ----------------------------------------------------------------------------
//  Local static methods
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eThreadFunc1(TThread& thrThis, tCIDLib::TVoid* pData)
{
    thrThis.Sync();

    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut  << L"Starting thread: "
                                    << thrThis.strName()
                                    << kCIDLib::EndLn;
    }

    tCIDLib::TCard4 c4Val = 1;
    TString strVal;
    try
    {
        // Create our client proxy object
        TTestOrb3IntfClientProxy intfTest(*TestOrb3_Client::pooidSrv1, TString::strEmpty());


        // Show the connection info
        {
            TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
            *TestOrb3_Client::pstrmOut  << L"Object Id="
                                        << *TestOrb3_Client::pooidSrv1
                                        << kCIDLib::NewEndLn;
        }


        while (kCIDLib::True)
        {
            // If we get an exit signal, then exit
            if (thrThis.bCheckShutdownRequest())
                break;

            intfTest.DelayRet();

            if (intfTest.c4AddTo(c4Val) != c4Val + 1)
            {
                TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
                *TestOrb3_Client::pstrmOut << L"c4ToAdd() failed" << kCIDLib::EndLn;
                break;
            }

            strVal = intfTest.strFormatValue(c4Val);
            if (strVal.c4Val() != c4Val)
            {
                TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
                *TestOrb3_Client::pstrmOut  << L"strFormatValue() failed"
                                            << kCIDLib::EndLn;
                break;
            }

            // Sleep a little
            thrThis.Sleep(50);

            c4Val++;
        }
    }

    catch(const TError& errToCatch)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << errToCatch << kCIDLib::EndLn;
    }

    catch(...)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << L"Unknown exception in Func1" << kCIDLib::EndLn;
    }

    TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
    *TestOrb3_Client::pstrmOut << L"Func1 exiting" << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::EExitCodes eThreadFunc2(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // Get the data out, then let the caller go
    thrThis.Sync();

    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut  << L"Starting thread: "
                                    << thrThis.strName()
                                    << kCIDLib::EndLn;
    }

    tCIDLib::TCard4 c4Val = 1;
    TString strVal;
    try
    {
        // Create our client proxy object
        TTestOrb3IntfClientProxy intfTest(*TestOrb3_Client::pooidSrv2, TString::strEmpty());

        while (kCIDLib::True)
        {
            // If we get an exit signal, then exit
            if (thrThis.bCheckShutdownRequest())
                break;

            intfTest.NoParmsNoRet();

            strVal = intfTest.strFormatValue(c4Val);
            if (strVal.c4Val() != c4Val)
            {
                TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
                *TestOrb3_Client::pstrmOut << L"strFormatValue() failed" << kCIDLib::NewLn;
                break;
            }

            intfTest.DelayRet();

            // Sleep a little
            thrThis.Sleep(30);
        }
    }

    catch(const TError& errToCatch)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << errToCatch << kCIDLib::EndLn;
    }

    catch(...)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << L"Unknown exception in Func2" << kCIDLib::EndLn;
    }

    TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
    *TestOrb3_Client::pstrmOut << L"Func2 exiting" << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::EExitCodes eThreadFunc3(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // Get the data out, then let the caller go
    thrThis.Sync();

    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut  << L"Starting thread: "
                                    << thrThis.strName()
                                    << kCIDLib::EndLn;
    }

    tCIDLib::TCard4 c4Val = 1;
    TString strVal;
    try
    {
        // Create our client proxy object
        TTestOrb3IntfClientProxy intfTest(*TestOrb3_Client::pooidSrv1, TString::strEmpty());

        while (kCIDLib::True)
        {
            // If we get an exit signal, then exit
            if (thrThis.bCheckShutdownRequest())
                break;

            if (intfTest.c4AddTo(c4Val) != c4Val + 1)
            {
                TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
                *TestOrb3_Client::pstrmOut << L"c4ToAdd() failed" << kCIDLib::EndLn;
                break;
            }

            // Sleep a little
            thrThis.Sleep(10);
        }
    }

    catch(const TError& errToCatch)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << errToCatch << kCIDLib::EndLn;
    }

    catch(...)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << L"Unknown exception in Func3" << kCIDLib::EndLn;
    }

    TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
    *TestOrb3_Client::pstrmOut << L"Func3 exiting" << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}



// ----------------------------------------------------------------------------
//  Global methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TFacTestOrb3::bOrbClientTest(const tCIDLib::TIPPortNum ippnSrv)
{
    tCIDLib::TBoolean bRet = kCIDLib::True;

    //
    //  Read in the object ids that the server side wrote out. We'll use
    //  these to create client proxy objects for them.
    //
    TBinFileInStream strmIn
    (
        L"TestOrb3Id1.OOId"
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::None
    );

    TOrbObjId ooidSrv1;
    strmIn >> ooidSrv1;
    strmIn.Close();

    strmIn.Open
    (
        L"TestOrb3Id2.OOId"
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::None
    );

    TOrbObjId ooidSrv2;
    strmIn >> ooidSrv2;
    strmIn.Close();

    // Store the shared data
    TestOrb3_Client::pmtxSync = new TMutex;
    TestOrb3_Client::pooidSrv1 = &ooidSrv1;
    TestOrb3_Client::pooidSrv2 = &ooidSrv2;
    TestOrb3_Client::pstrmOut = &m_conOut;

    // Spin up our test threads, passing each one the object id
    TThread thr1(L"TestOrb3Func1", &eThreadFunc1);
    TThread thr2(L"TestOrb3Func2", &eThreadFunc2);
    TThread thr3(L"TestOrb3Func3", &eThreadFunc3);
    thr1.Start();
    thr2.Start();
    thr3.Start();

    {
        #if CID_DEBUG_MEM
        TThread::Sleep(500);
        TKrnlMemCheck kmchkTest;
        kmchkTest.ReportToFile(L"MemoryDump.Txt");
        #endif

        // Wait a while for them to run
        TThread::Sleep(15000);
    }

    thr3.ReqShutdownSync();
    thr2.ReqShutdownSync();
    thr1.ReqShutdownSync();

    try
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << L"Waiting for thread3 to stop" << kCIDLib::EndLn;
        thr3.eWaitForDeath(1000);
    }

    catch(const TError& errToCatch)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << errToCatch << kCIDLib::EndLn;
    }

    try
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << L"Waiting for thread2 to stop" << kCIDLib::EndLn;
        thr2.eWaitForDeath(1000);
    }

    catch(const TError& errToCatch)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << errToCatch << kCIDLib::EndLn;
    }

    try
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << L"Waiting for thread1 to stop" << kCIDLib::EndLn;
        thr1.eWaitForDeath(1000);
    }

    catch(const TError& errToCatch)
    {
        TMtxLocker lockSync(TestOrb3_Client::pmtxSync);
        *TestOrb3_Client::pstrmOut << errToCatch << kCIDLib::EndLn;
    }

    // Dump out the client ORB oriented stats
    tCIDLib::TCard8              c8Stamp = 0;
    TVector<TStatsCacheItemInfo> colStats;
    const tCIDLib::TCard4 c4StatCnt = TStatsCache::c4QueryValuesInScope
    (
        kCIDOrb::pszStat_Scope_Client, colStats, kCIDLib::False, c8Stamp
    );

    m_conOut << L"\n\nORB Client Stats\n----------------------\n";
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4StatCnt; c4Index++)
    {
        const TStatsCacheItemInfo& sciiCur = colStats[c4Index];

        m_conOut << sciiCur.strName() << L" = " << sciiCur.c8Value()
                 << kCIDLib::NewLn;
    }
    m_conOut << kCIDLib::EndLn;

    return bRet;
}

