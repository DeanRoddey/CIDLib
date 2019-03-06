//
// FILE NAME: CIDMath.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/04/1998
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
#include    "CIDMath_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDMath& facCIDMath()
{
    static TFacCIDMath* pfacCIDMath = 0;
    if (!pfacCIDMath)
    {
        TBaseLock lockInit;
        if (!pfacCIDMath)
            TRawMem::pExchangePtr(pfacCIDMath, new TFacCIDMath);
    }
    return *pfacCIDMath;
}

