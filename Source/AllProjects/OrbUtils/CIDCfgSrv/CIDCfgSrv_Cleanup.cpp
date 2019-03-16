//
// FILE NAME: CIDCfgSrv_Cleanup.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
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
//  This method implements the private facility methods that are related to
//  program cleanup.
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
#include    "CIDCfgSrv.hpp"


// ---------------------------------------------------------------------------
//  TFacCIDCfgSrv: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacCIDCfgSrv::Cleanup()
{
    try
    {
        // Stop the rebinder first, or it would just re-register them again
        facCIDOrbUC().StopRebinder();

        // Get a name server proxy and unbind our objects
        tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();

        //
        //  Build up the binding names. We have to put our host name in it.
        //
        TString strTmp(TCIDCfgSrvServerBase::strBinding);
        strTmp.eReplaceToken(facCIDSock().strIPHostName(), L'h');
        orbcNS->RemoveBinding(strTmp, kCIDLib::False);

        strTmp = TCIDCfgSrvServerBase::strAdminBinding;
        strTmp.eReplaceToken(facCIDSock().strIPHostName(), L'h');
        orbcNS->RemoveBinding(strTmp, kCIDLib::False);

        // If we were given an alternative scope, then clean it up also
        strTmp = m_strAltNSScope;
        if (strTmp.chLast() != kCIDLib::chForwardSlash)
            strTmp.Append(kCIDLib::chForwardSlash);
        strTmp.Append(L"CfgRepoObj");
        orbcNS->RemoveBinding(strTmp, kCIDLib::False);

        //
        //  Degister the implementation object. This will cleanly shut it
        //  down and make sure all clients are out.
        //
        if (m_porbsImpl)
            facCIDOrb().DeregisterObject(m_porbsImpl);
    }

    catch(TError& errToCatch)
    {
        // If the error hasn't been logged, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    catch(...)
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvMsgs::midStatus_NSCleanupFailed
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Force the log server logger to local logging and terminate the
    //  Orb support.
    //
    try
    {
        if (m_plgrLogSrv)
            m_plgrLogSrv->bForceLocal(kCIDLib::True);
        facCIDOrb().Terminate();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            LogEventObj(errToCatch);
        }
    }

    catch(...)
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvMsgs::midStatus_OrbTermFailed
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  And stop the logger thread on the log server logger now. This
    //  will give him some time to get the last few logged things out.
    //  Anything after this will get lost.
    //
    try
    {
        if (m_plgrLogSrv)
            m_plgrLogSrv->Cleanup();
    }

    catch(...)
    {
    }

    // Clean up the wait event
    try
    {
        delete m_pevWait;
        m_pevWait = 0;
    }

    catch(...)
    {
    }
}

