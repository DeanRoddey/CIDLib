//
// FILE NAME: CIDPNG.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/18/2004
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
//  This is the main public header for the facility. This facility implements
//  the PNG graphics file format. This facility builds on the CIDImage
//  facility, which defines basic CIDLib stuff for reading and writing
//  and managing graphics formats generically.
//
//  So we implement the class to read/write the format, to get the pixel
//  data in/out in the standard pixel array format so it can be converted
//  to/from the internal bitmap format, and the class to get out basic
//  image format info.
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
#if     defined(PROJ_CIDPNG)
#define CIDPNGEXP  CID_DLLEXPORT
#else
#define CIDPNGEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDImage.hpp"



// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDPNG_Type.hpp"
#include    "CIDPNG_ErrorIds.hpp"
#include    "CIDPNG_Image.hpp"
#include    "CIDPNG_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDPNGEXP TFacCIDPNG& facCIDPNG();


