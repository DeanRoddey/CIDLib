//
// FILE NAME: CIDGraphDev.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
#if     defined(PROJ_CIDGRAPHDEV)
#define CIDGRDEVEXP   DLLEXPORT
#else
#define CIDGRDEVEXP   DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include the needed underlying headers.
// ---------------------------------------------------------------------------
#include    "CIDMath.hpp"

#include    "CIDImgFact.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TGraphData;
class TBitmap;
class TFontSelAttrs;
class TFontMetrics;
class TGraphDrawDev;


// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDGraphDev_Shared.hpp"
#include    "CIDGraphDev_Type.hpp"
#include    "CIDGraphDev_ErrorIds.hpp"
#include    "CIDGraphDev_ResourceIds.hpp"
#include    "CIDGraphDev_Constants.hpp"

#include    "CIDGraphDev_FontSelAttrs.hpp"
#include    "CIDGraphDev_FontMetrics.hpp"
#include    "CIDGraphDev_Brush.hpp"
#include    "CIDGraphDev_Pen.hpp"
#include    "CIDGraphDev_Font.hpp"
#include    "CIDGraphDev_Region.hpp"
#include    "CIDGraphDev_Device.hpp"
#include    "CIDGraphDev_Drawable.hpp"
#include    "CIDGraphDev_InfoDevice.hpp"
#include    "CIDGraphDev_DeviceJanitor.hpp"
#include    "CIDGraphDev_PaintDevice.hpp"
#include    "CIDGraphDev_PrintDevice.hpp"

#include    "CIDGraphDev_Bitmap.hpp"

#include    "CIDGraphDev_PointArray.hpp"
#include    "CIDGraphDev_MemDevice.hpp"
#include    "CIDGraphDev_Icon.hpp"
#include    "CIDGraphDev_ImgCacheItem.hpp"

#include    "CIDGraphDev_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object
// ---------------------------------------------------------------------------
extern CIDGRDEVEXP TFacCIDGraphDev& facCIDGraphDev();


