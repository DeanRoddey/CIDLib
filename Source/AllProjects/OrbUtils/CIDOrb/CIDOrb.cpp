//
// FILE NAME: CIDOrb.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/03/2000
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
#include    "CIDOrb_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDOrb,TFacility)



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDOrb& facCIDOrb()
{
    static TFacCIDOrb* pfacCIDOrb = nullptr;
    if (!pfacCIDOrb)
    {
        TBaseLock lockInit;
        if (!pfacCIDOrb)
            pfacCIDOrb = new TFacCIDOrb;
    }
    return *pfacCIDOrb;
}

