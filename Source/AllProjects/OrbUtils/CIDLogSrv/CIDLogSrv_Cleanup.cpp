//
// FILE NAME: CIDLogSrv_Cleanup.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/02/2001
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
#include    "CIDLogSrv.hpp"


// ---------------------------------------------------------------------------
//  TFacCIDLogSrv: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacCIDLogSrv::Cleanup()
{
    try
    {
        // Stop the rebinder first, or it would just re-register them again
        facCIDOrbUC().StopRebinder();

		//
        //	Get a name server proxy and unbind our objects. Since we stopping,
		//	most likely the NS is as well. So we use a short timeout. If it's
		//	already gone we won't block long. If it's going to, then it doesn't
		//	matter if we don't quite get it.
		//
        tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy(2000);
        orbcNS->RemoveBinding(TCIDLogSrvServerBase::strBinding, kCIDLib::False);
        orbcNS->RemoveBinding(TCIDLogSrvServerBase::strAdminBinding, kCIDLib::False);
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
            , kLogSMsgs::midStatus_NSCleanupFailed
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Try to shut down the Orb support
    try
    {
        facCIDOrb().Terminate();
    }

    catch(TError& errToCatch)
    {
        // If the error hasn't been logged, then log it
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
            , kLogSMsgs::midStatus_OrbTermFailed
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // And now we can let the wait event go
    delete m_pevWait;
    m_pevWait = 0;
}

