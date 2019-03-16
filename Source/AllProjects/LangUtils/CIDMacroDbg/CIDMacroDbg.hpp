//
// FILE NAME: CIDMacroDbg.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
//  This is the main public header for the CML embeddable IDE facility. It is
//  the single point of contact for the outside world. By including it, they
//  get what we have to offer.
//
//  The functionality here would not be very typically used. It's for programs
//  that want to let users edit and debug CML code.
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
#if     defined(PROJ_CIDMACRODBG)
#define CIDMACRODBGEXP    DLLEXPORT
#else
#define CIDMACRODBGEXP    DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our needed underlying headers.
// ---------------------------------------------------------------------------
#include    "CIDCtrls.hpp"
#include    "CIDWUtils.hpp"
#include    "CIDMacroEng.hpp"


// ---------------------------------------------------------------------------
//  Include our public headers
// ---------------------------------------------------------------------------
#include    "CIDMacroDbg_Type.hpp"
#include    "CIDMacroDbg_ErrorIds.hpp"
#include    "CIDMacroDbg_ResourceIds.hpp"
#include    "CIDMacroDbg_Constant.hpp"

#include    "CIDMacroDbg_StateInfo.hpp"
#include    "CIDMacroDbg_SrcTab.hpp"
#include    "CIDMacroDbg_DbgMainWnd.hpp"
#include    "CIDMacroDbg_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function.
// ---------------------------------------------------------------------------
extern CIDMACRODBGEXP TFacCIDMacroDbg& facCIDMacroDbg();


