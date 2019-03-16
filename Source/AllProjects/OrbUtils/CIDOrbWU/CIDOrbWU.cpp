//
// FILE NAME: CIDOrbWU.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/2004
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  Just provides the facility faulting in function.
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
#include    "CIDOrbWU_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDOrbWU& facCIDOrbWU()
{
    static TFacCIDOrbWU* pfacCIDOrbWU = nullptr;
    if (!pfacCIDOrbWU)
    {
        TBaseLock lockInit;
        if (!pfacCIDOrbWU)
            pfacCIDOrbWU = new TFacCIDOrbWU;
    }
    return *pfacCIDOrbWU;
}


