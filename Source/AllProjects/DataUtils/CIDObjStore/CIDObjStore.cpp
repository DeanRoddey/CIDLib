//
// FILE NAME: CIDObjStore.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main implementaiton file of the CIDLib object store facility.
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
#include    "CIDObjStore_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDObjStore& facCIDObjStore()
{
    static TFacCIDObjStore* pfacCIDObjStore = 0;
    if (!pfacCIDObjStore)
    {
        TBaseLock lockInit;
        if (!pfacCIDObjStore)
            TRawMem::pExchangePtr(pfacCIDObjStore, new TFacCIDObjStore);
    }
    return *pfacCIDObjStore;
}


