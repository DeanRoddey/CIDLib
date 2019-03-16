//
// FILE NAME: CIDMacroDbg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
#include    "CIDMacroDbg_.hpp"


//
//  This is the lazy eval accessor method for the facility object.
//
TFacCIDMacroDbg& facCIDMacroDbg()
{
    static TFacCIDMacroDbg* pfacCIDMacroDbg = nullptr;
    if (!pfacCIDMacroDbg)
    {
        TBaseLock lockInit;
        if (!pfacCIDMacroDbg)
            pfacCIDMacroDbg = new TFacCIDMacroDbg;
    }
    return *pfacCIDMacroDbg;
}


