//
// FILE NAME: CIDSpReco.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
//  This is the main header of a facility that encapsulates the MS speech
//  recognition platform. For now, this has not been 'split' for per-platform
//  support. But it needs to be at some point.
//
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
#if     defined(PROJ_CIDSPRECO)
#define CIDSPRECOEXP CID_DLLEXPORT
#else
#define CIDSPRECOEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"

#include    "CIDAudStream.hpp"



// ---------------------------------------------------------------------------
//  And sub-include our core headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDSpReco_Type.hpp"
#include    "CIDSpReco_ErrorIds.hpp"
#include    "CIDSpReco_MessageIds.hpp"
#include    "CIDSpReco_Shared.hpp"

#include    "CIDSpReco_Event.hpp"

namespace tCIDSpReco
{
    using TSpEventQ = TSafeQueue<TCIDSpeechRecoEv>;
}

#include    "CIDSpReco_Engine.hpp"
#include    "CIDSpReco_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy eval function
// ---------------------------------------------------------------------------
extern CIDSPRECOEXP TFacCIDSpReco& facCIDSpReco();
