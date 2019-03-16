//
// FILE NAME: CIDSIP.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/05/2013
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
#if     defined(PROJ_CIDSIP)
#define CIDSIPEXP   CID_DLLEXPORT
#else
#define CIDSIPEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"

#include    "CIDSock.hpp"

#include    "CIDNet.hpp"



// ---------------------------------------------------------------------------
//  Include the fundamental headers first
// ---------------------------------------------------------------------------
#include    "CIDSIP_ErrorIds.hpp"
#include    "CIDSIP_ThisFacility.hpp"
#include    "CIDSIP_Constant.hpp"
#include    "CIDSIP_Type.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object
// ---------------------------------------------------------------------------
extern CIDSIPEXP TFacCIDSIP& facCIDSIP();


// ---------------------------------------------------------------------------
//  And subinclude our other headers
// ---------------------------------------------------------------------------
#include    "CIDSIP_Msg.hpp"
#include    "CIDSIP_Transaction.hpp"
#include    "CIDSIP_Dialog.hpp"
#include    "CIDSIP_Agent.hpp"
#include    "CIDSIP_ClientAgent.hpp"

