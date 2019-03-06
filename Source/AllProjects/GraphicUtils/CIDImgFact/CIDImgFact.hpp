//
// FILE NAME: CIDImgFact.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/02/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main public header for the facility.
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
#if     defined(PROJ_CIDIMGFACT)
#define CIDIMGFACTEXP  CID_DLLEXPORT
#else
#define CIDIMGFACTEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDImage.hpp"


// ---------------------------------------------------------------------------
//  Some forward references
// ---------------------------------------------------------------------------
class TJPEGImage;
class TPNGImage;


// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDImgFact_ErrorIds.hpp"
#include    "CIDImgFact_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDIMGFACTEXP TFacCIDImgFact& facCIDImgFact();


