//
// FILE NAME: CIDGraphDev.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
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
#include    "CIDGraphDev_.hpp"



// ---------------------------------------------------------------------------
//  We need to force in the sysetm's graphics API library
// ---------------------------------------------------------------------------
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")


// ---------------------------------------------------------------------------
//  Our facility faulting in method
// ---------------------------------------------------------------------------
TFacCIDGraphDev& facCIDGraphDev()
{
    static TFacCIDGraphDev* pfacCIDGraphDev = nullptr;
    if (!pfacCIDGraphDev)
    {
        TBaseLock lockInit;
        if (!pfacCIDGraphDev)
            pfacCIDGraphDev = new TFacCIDGraphDev;
    }
    return *pfacCIDGraphDev;
}

