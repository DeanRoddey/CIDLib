//
// FILE NAME: CIDMacroDbg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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


