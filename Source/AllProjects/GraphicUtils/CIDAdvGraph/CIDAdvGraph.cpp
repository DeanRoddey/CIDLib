//
// FILE NAME: CIDAdvGraph.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
    static TFacCIDAdvGraph* pfacCIDAdvGraph = 0;
    if (!pfacCIDAdvGraph)
    {
        TBaseLock lockInit;
        if (!pfacCIDAdvGraph)
            TRawMem::pExchangePtr(pfacCIDAdvGraph, new TFacCIDAdvGraph);
    }
    return *pfacCIDAdvGraph;
}

