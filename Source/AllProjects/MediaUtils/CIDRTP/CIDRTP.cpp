//
// FILE NAME: CIDRTP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
#include    "CIDRTP_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDRTP& facCIDRTP()
{
    static TFacCIDRTP* pfacCIDRTP = nullptr;
    if (!pfacCIDRTP)
    {
        TBaseLock lockInit;
        if (!pfacCIDRTP)
            pfacCIDRTP = new TFacCIDRTP;
    }
    return *pfacCIDRTP;
}

