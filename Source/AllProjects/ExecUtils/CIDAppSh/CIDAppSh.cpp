//
// FILE NAME: CIDAppSh.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

