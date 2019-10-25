//
// FILE NAME: CIDAdvGraph.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2015
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
#include    "CIDAdvGraph_.hpp"



// ---------------------------------------------------------------------------
//  We need to force in any system libraries we need
// ---------------------------------------------------------------------------
#pragma comment(lib, "D2D1.lib")



// ---------------------------------------------------------------------------
//  Our facility faulting in method
// ---------------------------------------------------------------------------
TFacCIDAdvGraph& facCIDAdvGraph()
{
    static TFacCIDAdvGraph* pfacThis;
    static TAtomicFlag atomInit;
    if (!atomInit)
    {
        TBaseLock lockInit;
        if (!atomInit)
        {
            pfacThis = new TFacCIDAdvGraph();
            atomInit.Set();
        }
    }
    return *pfacThis;
}
