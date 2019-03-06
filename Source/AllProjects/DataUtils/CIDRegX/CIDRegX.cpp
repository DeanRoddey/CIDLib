//
// FILE NAME: CIDRegX.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/1999
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
#include    "CIDRegX_.hpp"


//
//  This is the lazy eval method for the facility object.
//
TFacCIDRegX& facCIDRegX()
{
    static TFacCIDRegX* pfacCIDRegX = 0;
    if (!pfacCIDRegX)
    {
        TBaseLock lockInit;
        if (!pfacCIDRegX)
            TRawMem::pExchangePtr(pfacCIDRegX, new TFacCIDRegX);
    }
    return *pfacCIDRegX;
}
