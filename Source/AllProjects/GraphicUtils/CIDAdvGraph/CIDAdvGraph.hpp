//
// FILE NAME: CIDAdvGraph.hpp
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer. It also insures consistent order of inclusion which
//  is needed for the precompiled headers stuff to work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once



// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDADVGRAPH)
#define CIDADVGREXP   DLLEXPORT
#else
#define CIDADVGREXP   DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include the needed underlying headers.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDCtrls.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDAdvGraph_ErrorIds.hpp"
// #include    "CIDAdvGraph_ResourceIds.hpp"

#include    "CIDAdvGraph_Window.hpp"
#include    "CIDAdvGraph_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object
// ---------------------------------------------------------------------------
extern CIDADVGREXP TFacCIDAdvGraph& facCIDAdvGraph();


