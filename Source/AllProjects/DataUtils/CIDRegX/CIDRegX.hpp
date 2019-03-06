//
// FILE NAME: CIDRegX.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/20/1999
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
#if     defined(PROJ_CIDREGX)
#define CIDREGXEXP  CID_DLLEXPORT
#else
#define CIDREGXEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDRegX_ErrorIds.hpp"
#include    "CIDRegX_ThisFacility.hpp"
#include    "CIDRegX_RegExNFA.hpp"
#include    "CIDRegX_RegExEngine.hpp"



// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDREGXEXP TFacCIDRegX& facCIDRegX();
