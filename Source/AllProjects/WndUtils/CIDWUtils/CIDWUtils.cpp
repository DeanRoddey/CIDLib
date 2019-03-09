//
// FILE NAME: CIDWUtils.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
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
#include    "CIDWUtils_.hpp"


// ---------------------------------------------------------------------------
//  Force in any libraries we need
// ---------------------------------------------------------------------------
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "User32.lib")


//
//  This is the lazy eval accessor method for the facility object.
//
TFacCIDWUtils& facCIDWUtils()
{
    static TFacCIDWUtils* pfacCIDWUtils = nullptr;
    if (!pfacCIDWUtils)
    {
        TBaseLock lockInit;
        if (!pfacCIDWUtils)
            pfacCIDWUtils = new TFacCIDWUtils;
    }
    return *pfacCIDWUtils;
}

