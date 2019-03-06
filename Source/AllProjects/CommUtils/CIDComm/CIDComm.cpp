//
// FILE NAME: CIDComm.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
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
#include    "CIDComm_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDComm& facCIDComm()
{
    static TFacCIDComm* pfacCIDComm = 0;
    if (!pfacCIDComm)
    {
        TBaseLock lockInit;
        if (!pfacCIDComm)
            TRawMem::pExchangePtr(pfacCIDComm, new TFacCIDComm);
    }
    return *pfacCIDComm;
}

