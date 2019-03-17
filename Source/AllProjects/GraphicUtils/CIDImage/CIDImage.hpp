//
// FILE NAME: CIDImage.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
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
//  This facility is the base facility for image manipulation. The main class
/// is TPixelArray which represents an in-memory image bitmap that can be
//  manipulated.
//
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer.
//
// CAVEATS/GOTCHAS:
//
//  1)  The pixel array class definitely needs the per-platform split because
//      currently it uses some assembly language. Once it's split, it would
//      obviously be nice to update the Win32 stuff to support some SIMD
//      code for better performance.
//
//      Some of the stuff could probably be replaced with bitmap raster ops
//      that would achieve the same thing and get hardware support if it is
//      available without us having to do anything.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDIMAGE)
#define CIDIMGEXP   CID_DLLEXPORT
#else
#define CIDIMGEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include the needed underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Some forward references for everyone
// ---------------------------------------------------------------------------
class   TPixelArray;
class   TCIDImage;


// ---------------------------------------------------------------------------
//  Include our other public headers
// ---------------------------------------------------------------------------
#include    "CIDImage_Type.hpp"
#include    "CIDImage_ErrorIds.hpp"
#include    "CIDImage_Color.hpp"
#include    "CIDImage_ColorPalette.hpp"
#include    "CIDImage_HSVColor.hpp"
#include    "CIDImage_ThisFacility.hpp"
#include    "CIDImage_PixelArray.hpp"
#include    "CIDImage_Image.hpp"
#include    "CIDImage_BitmapImage.hpp"

// ---------------------------------------------------------------------------
//  Export our facility object lazy evaluator function.
// ---------------------------------------------------------------------------
extern CIDIMGEXP TFacCIDImage& facCIDImage();
