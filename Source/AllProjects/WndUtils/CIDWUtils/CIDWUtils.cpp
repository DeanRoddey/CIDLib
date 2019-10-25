//
// FILE NAME: CIDWUtils.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
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
    static TFacCIDWUtils* pfacThis;
    static TAtomicFlag atomInit;
    if (!atomInit)
    {
        TBaseLock lockInit;
        if (!atomInit)
        {
            pfacThis = new TFacCIDWUtils();
            atomInit.Set();
        }
    }
    return *pfacThis;
}

