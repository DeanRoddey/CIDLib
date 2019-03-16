//
// FILE NAME: TestOrb3_ServerImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/08/2001
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
#include    "TestOrb3.hpp"


// ---------------------------------------------------------------------------
//  Provide our implementation of the server stub
// ---------------------------------------------------------------------------
class TTestOrb3ServerImpl : public TTestOrb3IntfServerBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestOrb3ServerImpl()
        {
        }

        ~TTestOrb3ServerImpl()
        {
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddToBuf(tCIDLib::TCard4& c4Bytes, THeapBuf& mbufIO) override
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Bytes; c4Index++)
                mbufIO[c4Index]++;
        }

        tCIDLib::TCard4 c4AddTo(const tCIDLib::TCard4 c4ToAdjust) override
        {
            return c4ToAdjust + 1;
        }

        tCIDLib::TVoid DelayRet() override
        {
            TThread::Sleep(100);
        }

        tCIDLib::TVoid NoParmsNoRet() override
        {
        }

        TString strFormatValue(const tCIDLib::TCard4 c4ToFormat) override
        {
            TString strRet;
            strRet.AppendFormatted(c4ToFormat);
            return strRet;
        }

        tCIDLib::TVoid UploadBuf(const tCIDLib::TCard4 c4Bytes, const TMemBuf& mbufin) override
        {
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize()
        {
            TParent::Initialize();
        }

        tCIDLib::TVoid Terminate()
        {
            TParent::Terminate();
        }


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TTestOrb3ServerImpl(const TTestOrb3ServerImpl&);
        tCIDLib::TVoid operator=(const TTestOrb3ServerImpl&);


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestOrb3ServerImpl, TTestOrb3IntfServerBase)
};

RTTIDecls(TTestOrb3ServerImpl, TTestOrb3IntfServerBase)



// ---------------------------------------------------------------------------
//  TFacTestOrb3: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacTestOrb3::bOrbServerTest()
{
    TString strFailReason = L"Unknown";
    try
    {
        TTestOrb3ServerImpl* porbsTmp;

        //
        //  Create two instances of our server interface. We register them with
        //  the ORB, which will then be able to dispatch commands to them. We
        //  write out the object ids for them, so that the client version of
        //  this test can read them in and create client proxies. This avoids
        //  having to use the naming service for this simple test.
        //
        strFailReason = L"Creating Server instance 1";
        porbsTmp = new TTestOrb3ServerImpl();

        strFailReason = L"Registering object 1 with the ORB";
        facCIDOrb().RegisterObject(porbsTmp);

        strFailReason = L"Writing out id 1 file";
        TBinFileOutStream strmOut
        (
            L"TestOrb3Id1.OOId"
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::None
        );
        strmOut << porbsTmp->ooidThis() << kCIDLib::FlushIt;
        strmOut.Close();

        strFailReason = L"Creating Server instance 2";
        porbsTmp = new TTestOrb3ServerImpl();
        strFailReason = L"Registering object 2 with the ORB";
        facCIDOrb().RegisterObject(porbsTmp);

        strFailReason = L"Writing out id 2 file";
        strmOut.Open
        (
            L"TestOrb3Id2.OOId"
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::None
        );
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

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

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

