//
// FILE NAME: CIDOrbUC.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
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
#include    "CIDOrbUC_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDOrbUC& facCIDOrbUC()
{
    static TFacCIDOrbUC* pfacCIDOrbUC = nullptr;
    if (!pfacCIDOrbUC)
    {
        TBaseLock lockInit;
        if (!pfacCIDOrbUC)
            pfacCIDOrbUC = new TFacCIDOrbUC;
    }
    return *pfacCIDOrbUC;
}

