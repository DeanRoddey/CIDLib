//
// FILE NAME: CIDJPEG.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2005
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
#include    "CIDJPEG_.hpp"


//
//  This is the lazy eval method for the facility object.
//
TFacCIDJPEG& facCIDJPEG()
{
    static TFacCIDJPEG* pfacCIDJPEG = 0;
    if (!pfacCIDJPEG)
    {
        TBaseLock lockInit;
        if (!pfacCIDJPEG)
            TRawMem::pExchangePtr(pfacCIDJPEG, new TFacCIDJPEG);
    }
    return *pfacCIDJPEG;
}


