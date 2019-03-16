//
// FILE NAME: CIDZLib.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/2004
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
//  the ZLib compression algorithm, specialized for integration into the
//  CIDLib libraries.
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
#if     defined(PROJ_CIDZLIB)
#define CIDZLIBEXP  CID_DLLEXPORT
#else
#define CIDZLIBEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDZLib_Constant.hpp"
#include    "CIDZLib_Type.hpp"
#include    "CIDZLib_ErrorIds.hpp"
#include    "CIDZLib_Compressor.hpp"
#include    "CIDZLib_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDZLIBEXP TFacCIDZLib& facCIDZLib();

