//
// FILE NAME: CIDPack.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2015
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
//  As the name implies this is the main module for the CIDPack facility.
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
#include    "CIDPack_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDPack& facCIDPack()
{
    static TFacCIDPack* pfacThis = nullptr;
    static TAtomicFlag atomInit;
    if (!atomInit)
    {
        TBaseLock lockInit;
        if (!atomInit)
        {
            pfacThis = new TFacCIDPack();
            atomInit.Set();
        }
    }
    return *pfacThis;
}
