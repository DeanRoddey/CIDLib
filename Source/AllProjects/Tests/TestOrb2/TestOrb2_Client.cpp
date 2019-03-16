//
// FILE NAME: TestOrb2_Client.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
#include    "TestOrb2.hpp"


tCIDLib::TBoolean TFacTestOrb2::bOrbClientTest(const tCIDLib::TIPPortNum ippnSrv)
{
    tCIDLib::TBoolean bRet = kCIDLib::True;

    //
    //  Read in the object id that the server side wrote out. We'll use
    //  it to create client proxy objects for them.
    //
    TPathStr pathOOId(L"TestOrb2Id.OOId");

    TOrbObjId ooidSrv;
    {
        TBinFileInStream strmIn
        (
            pathOOId
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::None
        );
        strmIn >> ooidSrv;
    }

    try
    {
        // Create our client proxy object
        TTestOrb2IntfClientProxy intfTest(ooidSrv, TString::strEmpty());

        // Get the start time
        tCIDLib::TEncodedTime enctStart = TTime::enctNow();

        // Loop, doing a ping to the server
        const tCIDLib::TCard4 c4Count = 50000;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            intfTest.Ping();
        }

        // And get the end time
        tCIDLib::TEncodedTime enctEnd = TTime::enctNow();

        // Display the interval
        const tCIDLib::TEncodedTime enctMSecs = (enctEnd - enctStart) / 10000;
        m_conOut    << L"   MSecs: " << enctMSecs << L"ms" << kCIDLib::NewLn
                    << L" Per/Sec: " << (c4Count / (enctMSecs / 1000))
                    << kCIDLib::EndLn;
    }

    catch(const TError& errToCatch)
    {
        m_conOut << kCIDLib::NewLn << errToCatch << kCIDLib::EndLn;
        bRet = kCIDLib::False;
    }

    // Dump out the client ORB oriented stats
    tCIDLib::TCard8              c8Stamp = 0;
    TVector<TStatsCacheItemInfo> colStats;
    const tCIDLib::TCard4 c4StatCnt = TStatsCache::c4QueryValuesInScope
    (
        kCIDOrb::pszStat_Scope_Client, colStats, kCIDLib::False, c8Stamp
    );

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4StatCnt; c4Index++)
    {
        const TStatsCacheItemInfo& sciiCur = colStats[c4Index];

        m_conOut << sciiCur.strName() << L" = " << sciiCur.c8Value()
                 << kCIDLib::NewLn;
    }
    m_conOut << kCIDLib::EndLn;

    return bRet;
}

