//
// FILE NAME: CIDMath.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/04/1998
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
#if     defined(PROJ_CIDMATH)
#define CIDMATHEXP  CID_DLLEXPORT
#else
#define CIDMATHEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDMath_Shared.hpp"
#include    "CIDMath_ErrorIds.hpp"

#include    "CIDMath_2DVector.hpp"
#include    "CIDMath_3DVector.hpp"
#include    "CIDMath_4By4Matrix.hpp"
#include    "CIDMath_Complex.hpp"
#include    "CIDMath_FArea.hpp"
#include    "CIDMath_ValueMap.hpp"
#include    "CIDMath_ValueMap2.hpp"
#include    "CIDMath_FValueMap.hpp"
#include    "CIDMath_FValueMap2.hpp"
#include    "CIDMath_Range.hpp"
#include    "CIDMath_GraphData.hpp"
#include    "CIDMath_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDMATHEXP TFacCIDMath& facCIDMath();
