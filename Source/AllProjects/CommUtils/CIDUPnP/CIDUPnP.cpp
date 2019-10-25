//
// FILE NAME: CIDUPnP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2011
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
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
    static TFacCIDUPnP* pfacThis;
    static TAtomicFlag atomInit;
    if (!atomInit)
    {
        TBaseLock lockInit;
        if (!atomInit)
        {
            pfacThis = new TFacCIDUPnP();
            atomInit.Set();
        }
    }
    return *pfacThis;
}
