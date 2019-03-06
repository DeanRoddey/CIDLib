//
// FILE NAME: CIDPack.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  As the name implies this is the main module for the CIDPack facility.
//  It provides the lazy eval access method for the facility object.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDPack_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDPack& facCIDPack()
{
    static TFacCIDPack* pfacCIDPack = 0;
    if (!pfacCIDPack)
    {
        TBaseLock lockInit;
        if (!pfacCIDPack)
            TRawMem::pExchangePtr(pfacCIDPack, new TFacCIDPack);
    }
    return *pfacCIDPack;
}

