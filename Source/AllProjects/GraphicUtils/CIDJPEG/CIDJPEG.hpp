//
// FILE NAME: CIDJPEG.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2005
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  the JPEG graphics file format. This facility builds on the CIDImage
//  facility, which defines basic CIDLib stuff for reading and writing
//  and managing graphics formats generically.
//
//  Since JPEG is sooo dang complex, we use the guts of the standard JPEG
//  Working Group's JPEG support library. We pull it in and integrate it
//  into our own system as best it can be, and all the details are hidden
//  inside this facility. This is one of very small number of exceptions
//  where this is done.
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
#if     defined(PROJ_CIDJPEG)
#define CIDJPEGEXP  CID_DLLEXPORT
#else
#define CIDJPEGEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDImage.hpp"



// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDJPEG_Constant.hpp"
#include    "CIDJPEG_Type.hpp"
#include    "CIDJPEG_ErrorIds.hpp"
#include    "CIDJPEG_Image.hpp"
#include    "CIDJPEG_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDJPEGEXP TFacCIDJPEG& facCIDJPEG();


