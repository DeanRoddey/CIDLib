//
// FILE NAME: CIDDAE.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
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
#include    "CIDDAE_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDDAE& facCIDDAE()
{
    static TFacCIDDAE* pfacCIDDAE = nullptr;
    if (!pfacCIDDAE)
    {
        TBaseLock lockInit;
        if (!pfacCIDDAE)
            pfacCIDDAE = new TFacCIDDAE;
    }
    return *pfacCIDDAE;
}

