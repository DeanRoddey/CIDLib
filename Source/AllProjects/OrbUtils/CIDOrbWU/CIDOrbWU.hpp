//
// FILE NAME: CIDOrbWU.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/2004
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
//  This is the main header of the ORB/Windowing facility, which provides
//  some standard GUI dialogs and widgets that build on the ORB services.
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
#if     defined(PROJ_CIDORBWU)
#define CIDORBWUEXP CID_DLLEXPORT
#else
#define CIDORBWUEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDOrbUC.hpp"
#include    "CIDWUtils.hpp"


// ---------------------------------------------------------------------------
//  Include our fundamental headers
// ---------------------------------------------------------------------------
#include    "CIDOrbWU_Type.hpp"
#include    "CIDOrbWU_Constant.hpp"

#include    "CIDOrbWU_RemFileOpenBrowser.hpp"
#include    "CIDOrbWU_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object
// ---------------------------------------------------------------------------
extern CIDORBWUEXP TFacCIDOrbWU& facCIDOrbWU();


