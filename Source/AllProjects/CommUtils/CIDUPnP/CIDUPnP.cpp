//
// FILE NAME: CIDUPnP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2011
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
//  For this file we want to pre instantiate some collections, but not for any other
//  files. Only this file defines this token, which prevents the use of the extern
//  declaration.
// ---------------------------------------------------------------------------
#define CIDUPNP_PREINST 1


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDUPnP_.hpp"


// ---------------------------------------------------------------------------
//  Pre-generate some template classes
// ---------------------------------------------------------------------------
template class CIDUPNPEXP TRefKeyedHashSet<TUPnPAsyncFinderItem, TString, TStringKeyOps>;



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDUPnP& facCIDUPnP()
{
    static TFacCIDUPnP* pfacCIDUPnP = 0;
    if (!pfacCIDUPnP)
    {
        TBaseLock lockInit;
        if (!pfacCIDUPnP)
            TRawMem::pExchangePtr(pfacCIDUPnP, new TFacCIDUPnP);
    }
    return *pfacCIDUPnP;
}

