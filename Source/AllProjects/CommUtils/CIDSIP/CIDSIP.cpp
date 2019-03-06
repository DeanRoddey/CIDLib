//
// FILE NAME: CIDSIP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/05/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main file of the facility.
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
#include    "CIDSIP_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDSIP,TFacility)


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDSIP& facCIDSIP()
{
    static TFacCIDSIP* pfacCIDSIP = 0;
    if (!pfacCIDSIP)
    {
        TBaseLock lockInit;
        if (!pfacCIDSIP)
            TRawMem::pExchangePtr(pfacCIDSIP, new TFacCIDSIP);
    }
    return *pfacCIDSIP;
}

