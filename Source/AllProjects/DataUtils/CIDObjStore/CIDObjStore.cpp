//
// FILE NAME: CIDObjStore.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2002
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
//  This is the main implementaiton file of the CIDLib object store facility.
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
#include    "CIDObjStore_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDObjStore& facCIDObjStore()
{
    static TFacCIDObjStore* pfacCIDObjStore = nullptr;
    if (!pfacCIDObjStore)
    {
        TBaseLock lockInit;
        if (!pfacCIDObjStore)
            pfacCIDObjStore = new TFacCIDObjStore;
    }
    return *pfacCIDObjStore;
}


