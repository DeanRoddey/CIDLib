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
//  This facility provide corae image manipulation functionality. The core class
/// is TPixelArray which represents an in-memory image bitmap that can be
//  manipulated to process image data.
//
//  TPixelArray is actually a wrapper around an internal implementation class,
//  TPixelArrayImpl. This guy is a base class, though still concrete. He provides
//  reasonable though un-accelerated versions of all of the required processing
//  methods. They are all virtual but he provides default implementations of them
//  all.
//
//  Each platform implements a few private methods of TPixelArray whose job is to
//  gen up an implementation object and returnit to TPixelArray. A given platform
//  may choose to just use the default implementations by creating a default
//  impl object. Or they can define their own impl object and return it.
//
//  Their impl object only has to override those methods that it feels it can do
//  better than the default. So it's not an all or nothing deal. If they have
//  SIMD or native accelerated implementations for some methods they can override
//  just those. So it can be done in a piecemeal fashion.
//
//  Any given platform driver may also look around and decide to create one or another
//  impl object, perhaps one that uses hardware acceleration if it is available else
//  the default or a less aggress per-platform one. Since all work is done via an impl
//  object, dynamic loading of code based on hardware capabilties is easily done.
//
//
//  * I've just begun to do the per-platform split. The old Win32 specific stuff
//    has not been split out into its own platform specific impl class because we
//    need to provide default implementations for those in the base impl class first.
//    Until then it will only work on Win32 platforms still (unless the platform
//    impl overrides all  the methods of the default that still do Win32 specific
//    stuff.)
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
class   TPixelArrayImpl;
class   TPixelArray;
class   TCIDImage;


// ---------------------------------------------------------------------------
//  Facility constants
// ---------------------------------------------------------------------------
namespace kCIDImage
{
    // -----------------------------------------------------------------------
    //  Pixel arrays pad rows by 4 byte boundaries
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4LinePadding = 4;
}


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
//  Facility types
// ---------------------------------------------------------------------------
namespace tCIDImage
{
    // -----------------------------------------------------------------------
    //  Smart pointers to images, which are often allocated because they are
    //  polymoprhic.
    // -----------------------------------------------------------------------
    using TCntImgPtr = TCntPtr<TCIDImage>;
    using TUImgPtr = TUniquePtr<TCIDImage>;
}


// ---------------------------------------------------------------------------
//  Export our facility object lazy evaluator function.
// ---------------------------------------------------------------------------
extern CIDIMGEXP TFacCIDImage& facCIDImage();
