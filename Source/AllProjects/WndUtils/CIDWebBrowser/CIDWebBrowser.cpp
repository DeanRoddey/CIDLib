//
// FILE NAME: CIDWebBrowser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2005
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
#include    "CIDWebBrowser_.hpp"


// ---------------------------------------------------------------------------
//  The lazy eval method for our facility object
// ---------------------------------------------------------------------------
TFacCIDWebBrowser& facCIDWebBrowser()
{
    static TFacCIDWebBrowser* pfacCIDWebBrowser = 0;
    if (!pfacCIDWebBrowser)
    {
        TBaseLock lockInit;
        if (!pfacCIDWebBrowser)
            TRawMem::pExchangePtr(pfacCIDWebBrowser, new TFacCIDWebBrowser);
    }
    return *pfacCIDWebBrowser;
}


