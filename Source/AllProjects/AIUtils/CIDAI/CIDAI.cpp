//
// FILE NAME: CIDAI.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main implementation file of the CIDLib AI facility.
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
#include    "CIDAI_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDAI& facCIDAI()
{
    static TFacCIDAI* pfacCIDAI = 0;
    if (!pfacCIDAI)
    {
        TBaseLock lockInit;
        if (!pfacCIDAI)
            TRawMem::pExchangePtr(pfacCIDAI, new TFacCIDAI);
    }
    return *pfacCIDAI;
}
