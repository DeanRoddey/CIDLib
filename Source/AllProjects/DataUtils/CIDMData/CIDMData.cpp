//
// FILE NAME: CIDMData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/03/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  As the name implies this is the main module for the CIDMData facility.
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
#include    "CIDMData_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDMData& facCIDMData()
{
    static TFacCIDMData* pfacCIDMData = nullptr;
    if (!pfacCIDMData)
    {
        TBaseLock lockInit;
        if (!pfacCIDMData)
            pfacCIDMData = new TFacCIDMData;
    }
    return *pfacCIDMData;
}
