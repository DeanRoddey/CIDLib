//
// FILE NAME: CIDRegX.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/1999
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
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDRegX_.hpp"


//
//  This is the lazy eval method for the facility object.
//
TFacCIDRegX& facCIDRegX()
{
    static TFacCIDRegX* pfacThis = nullptr;
    static TAtomicFlag atomInit;
    if (!atomInit)
    {
        TBaseLock lockInit;
        if (!atomInit)
        {
            pfacThis = new TFacCIDRegX();
            atomInit.Set();
        }
    }
    return *pfacThis;
}
