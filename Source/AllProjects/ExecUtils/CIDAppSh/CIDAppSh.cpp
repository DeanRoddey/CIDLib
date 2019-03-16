//
// FILE NAME: CIDAppSh.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
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
//  This is the main implementaiton file of the CIDLib application shell
//  facility.
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
#include    "CIDAppSh_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDAppSh& facCIDAppSh()
{
    static TFacCIDAppSh* pfacCIDAppShell = nullptr;
    if (!pfacCIDAppShell)
    {
        TBaseLock lockInit;
        if (!pfacCIDAppShell)
            pfacCIDAppShell = new TFacCIDAppSh;
    }
    return *pfacCIDAppShell;
}

