//
// FILE NAME: CIDPNG.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/18/2004
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
#include    "CIDPNG_.hpp"


//
//  This is the lazy eval method for the facility object.
//
TFacCIDPNG& facCIDPNG()
{
    static TFacCIDPNG* pfacCIDPNG = nullptr;
    if (!pfacCIDPNG)
    {
        TBaseLock lockInit;
        if (!pfacCIDPNG)
            pfacCIDPNG = new TFacCIDPNG;
    }
    return *pfacCIDPNG;
}


