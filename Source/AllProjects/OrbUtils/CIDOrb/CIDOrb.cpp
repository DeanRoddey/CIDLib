//
// FILE NAME: CIDOrb.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/03/2000
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
#include    "CIDOrb_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDOrb,TFacility)



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDOrb& facCIDOrb()
{
    static TFacCIDOrb* pfacCIDOrb = nullptr;
    if (!pfacCIDOrb)
    {
        TBaseLock lockInit;
        if (!pfacCIDOrb)
            TRawMem::pExchangePtr(pfacCIDOrb, new TFacCIDOrb);
    }
    return *pfacCIDOrb;
}

