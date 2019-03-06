//
// FILE NAME: CIDRTP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
#include    "CIDRTP_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDRTP& facCIDRTP()
{
    static TFacCIDRTP* pfacCIDRTP = 0;
    if (!pfacCIDRTP)
    {
        TBaseLock lockInit;
        if (!pfacCIDRTP)
            TRawMem::pExchangePtr(pfacCIDRTP, new TFacCIDRTP);
    }
    return *pfacCIDRTP;
}

