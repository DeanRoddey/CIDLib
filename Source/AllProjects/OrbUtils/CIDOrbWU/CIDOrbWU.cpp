//
// FILE NAME: CIDOrbWU.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Just provides the facility faulting in function.
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
#include    "CIDOrbWU_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDOrbWU& facCIDOrbWU()
{
    static TFacCIDOrbWU* pfacCIDOrbWU = nullptr;
    if (!pfacCIDOrbWU)
    {
        TBaseLock lockInit;
        if (!pfacCIDOrbWU)
            pfacCIDOrbWU = new TFacCIDOrbWU;
    }
    return *pfacCIDOrbWU;
}


