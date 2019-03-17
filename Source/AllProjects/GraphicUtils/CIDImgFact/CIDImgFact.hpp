//
// FILE NAME: CIDImgFact.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/02/2013
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
//  This is the main public header for the image factory facility. Include
//  this to get all of the functionality offered.
//
//  This facility provides a way to gen up images generically, convert them
//  back and forth between formats, etc... So it makes it fairly easy to deal
//  with images without having to do much work to load them and convert them.
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


