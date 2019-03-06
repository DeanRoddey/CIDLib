//
// FILE NAME: CIDImgFact.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/02/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main file of the facility. It just provides the lazy
//  evaluation method for the facilty object.
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
#include    "CIDImgFact_.hpp"


//
//  This is the lazy eval method for the facility object.
//
TFacCIDImgFact& facCIDImgFact()
{
    static TFacCIDImgFact* pfacCIDImgFact = 0;
    if (!pfacCIDImgFact)
    {
        TBaseLock lockInit;
        if (!pfacCIDImgFact)
            TRawMem::pExchangePtr(pfacCIDImgFact, new TFacCIDImgFact);
    }
    return *pfacCIDImgFact;
}


