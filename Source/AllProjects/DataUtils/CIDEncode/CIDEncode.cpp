//
// FILE NAME: CIDEncode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/26/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  As the name implies this is the main module for the CIDEncode facility.
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
#include    "CIDEncode_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDEncode& facCIDEncode()
{
    static TFacCIDEncode* pfacCIDEncode = 0;
    if (!pfacCIDEncode)
    {
        TBaseLock lockInit;
        if (!pfacCIDEncode)
            TRawMem::pExchangePtr(pfacCIDEncode, new TFacCIDEncode);
    }
    return *pfacCIDEncode;
}
