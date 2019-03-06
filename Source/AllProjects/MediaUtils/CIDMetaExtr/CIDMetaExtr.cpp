//
// FILE NAME: CIDMetaExtr.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2007
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
#include    "CIDMetaExtr_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDMetaExtr& facCIDMetaExtr()
{
    static TFacCIDMetaExtr* pfacCIDMetaExtr = 0;
    if (!pfacCIDMetaExtr)
    {
        TBaseLock lockInit;
        if (!pfacCIDMetaExtr)
            TRawMem::pExchangePtr(pfacCIDMetaExtr, new TFacCIDMetaExtr);
    }
    return *pfacCIDMetaExtr;
}

