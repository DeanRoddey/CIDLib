//
// FILE NAME: CIDComm.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
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
#include    "CIDComm_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDComm& facCIDComm()
{
    static TFacCIDComm* pfacThis = nullptr;
    static TAtomicFlag atomInit;
    if (!atomInit)
    {
        TBaseLock lockInit;
        if (!atomInit)
        {
            pfacThis = new TFacCIDComm();
            atomInit.Set();
        }
    }
    return *pfacThis;
}
