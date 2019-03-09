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
    static TFacCIDMetaExtr* pfacCIDMetaExtr = nullptr;
    if (!pfacCIDMetaExtr)
    {
        TBaseLock lockInit;
        if (!pfacCIDMetaExtr)
            pfacCIDMetaExtr = new TFacCIDMetaExtr;
    }
    return *pfacCIDMetaExtr;
}

