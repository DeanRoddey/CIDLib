//
// FILE NAME: CIDImage.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
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
#include    "CIDImage_.hpp"


TFacCIDImage& facCIDImage()
{
    static TFacCIDImage* pfacCIDImage = nullptr;
    if (!pfacCIDImage)
    {
        TBaseLock lockInit;
        if (!pfacCIDImage)
            pfacCIDImage = new TFacCIDImage;
    }
    return *pfacCIDImage;
}

