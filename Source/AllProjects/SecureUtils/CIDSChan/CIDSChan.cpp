//
// FILE NAME: CIDSChan.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main file of the facility. It contains any global data or
//  basic initialization required for this facility.
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
#include    "CIDSChan_.hpp"


// ---------------------------------------------------------------------------
//  Public functions
// ---------------------------------------------------------------------------
TFacCIDSChan& facCIDSChan()
{
    static TFacCIDSChan* pfacCIDSChan = 0;
    if (!pfacCIDSChan)
    {
        TBaseLock lockInit;
        if (!pfacCIDSChan)
            TRawMem::pExchangePtr(pfacCIDSChan, new TFacCIDSChan);
    }
    return *pfacCIDSChan;
}
