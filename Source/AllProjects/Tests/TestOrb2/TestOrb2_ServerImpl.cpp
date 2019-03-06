//
// FILE NAME: TestOrb2_ServerImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/08/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This module implements the server side of the test
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
#include    "TestOrb2.hpp"


// ---------------------------------------------------------------------------
//  Provide our implementation of the server stub
// ---------------------------------------------------------------------------
class TTestOrb2ServerImpl : public TTestOrb2IntfServerBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestOrb2ServerImpl()
        {
        }

        ~TTestOrb2ServerImpl()
        {
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Ping()
        {
            // Its just a do-nothing return
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize()
        {
        }

        tCIDLib::TVoid Terminate()
        {
        }


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TTestOrb2ServerImpl(const TTestOrb2ServerImpl&);
        tCIDLib::TVoid operator=(const TTestOrb2ServerImpl&);
};



// ---------------------------------------------------------------------------
//  TFacTestOrb2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacTestOrb2::bOrbServerTest()
{
    TString strFailReason = L"Unknown";
    try
    {
        TTestOrb2ServerImpl* porbsTmp;

        //
        //  Create an instances of our server interface. We register
        //  it with the ORB, which will then be able to dispatch
        //  commands to it. We write out the object id for it, so
        //  that the client version of this test can read it in and
        //  create client proxies. This avoids having to use the naming
        //  service for this simple test.
        //
        TPathStr pathOOId(L"TestOrb2Id.OOId");

        // Create a binary output stream for the id object
        TBinFileOutStream strmOut
        (
            pathOOId
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::None
        );

        // Now create the object, register it, and write the id
        strFailReason = L"Creating Server instance";
        porbsTmp = new TTestOrb2ServerImpl();

        strFailReason = L"Registering object with the ORB";
        facCIDOrb().RegisterObject(porbsTmp);

        strFailReason = L"Writing out id file";
        strmOut << porbsTmp->ooidThis() << kCIDLib::FlushIt;
        strmOut.Close();

        // Just wait for any key then exit
        m_conOut << L"Space for stats, any other exits\n" << kCIDLib::FlushIt;

        while (kCIDLib::True)
        {
            tCIDLib::EConKeys eKeyType;
            if (m_conIn.chReadChar(eKeyType, kCIDLib::c4MaxWait) == kCIDLib::chSpace)
            {
                // Dump the server side orb stats
                tCIDLib::TCard8              c8Stamp = 0;
                TVector<TStatsCacheItemInfo> colStats;
                const tCIDLib::TCard4 c4StatCnt = TStatsCache::c4QueryValuesInScope
                (
                    kCIDOrb::pszStat_Scope_Server, colStats, kCIDLib::False, c8Stamp
                );

                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4StatCnt; c4Index++)
                {
                    const TStatsCacheItemInfo& sciiCur = colStats[c4Index];

                    m_conOut << sciiCur.strName() << L" = " << sciiCur.c8Value()
                             << kCIDLib::NewLn;
                }
                m_conOut << kCIDLib::EndLn;
            }
             else
            {
                break;
            }
            m_conOut << kCIDLib::EndLn;
        }
    }

    catch(const TError&)
    {
        m_conOut << L"TError exception while: " << strFailReason << kCIDLib::EndLn;
        throw;
    }

    catch(...)
    {
        m_conOut << L"Unknown exception while: " << strFailReason << kCIDLib::EndLn;
        throw;
    }

    return kCIDLib::True;
}

