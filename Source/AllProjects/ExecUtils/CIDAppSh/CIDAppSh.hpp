//
// FILE NAME: CIDAppSh.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
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
//  This is the main header of the CIDLib app shell facility. This library
//  facility provides a generic application shell that can be configured
//  to run one or more applications, passing along configured parameters,
//  and to manage those applications and restart them if they fail.
//
//  It supports an API that allows it to be managed by the host application,
//  to add or remove apps from its list, change the configuration for apps
//  in the list, change the restart/leavedown options, get information about
//  the apps configured, etc...
//
//  This facility is often wrapped by higher level, situation specific,
//  facilities that understand more about the apps being controlled, such as
//  how to ask them to shutdown or cycle and so forth. This facility just
//  provides the common facilities at the core of all such app shells.
//
//  The parameter string can contain the following replacement tokens which
//  will be updated when the app record is added to the engine (not every
//  time its started, just when the record is added to the engine.
//
//      %(h)        - The current host name
//      %(u)        - The current user's name
//      %(t)        - The current time, in the form HH:MM:SS, 24 hour clock
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
#if     defined(PROJ_CIDAPPSH)
#define CIDAPPSHEXP CID_DLLEXPORT
#else
#define CIDAPPSHEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDOrbUC.hpp"


// ---------------------------------------------------------------------------
//  Constants namespace
// ---------------------------------------------------------------------------
namespace kCIDAppSh
{
    // The maximum number of applications we will allow to exist at once
    const tCIDLib::TCard4 c4MaxApps = 8192;
};


// ---------------------------------------------------------------------------
//  And sub-include our core headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDAppSh_Type.hpp"
#include    "CIDAppSh_ErrorIds.hpp"
#include    "CIDAppSh_MessageIds.hpp"
#include    "CIDAppSh_AppRecord.hpp"
#include    "CIDAppSh_Engine.hpp"
#include    "CIDAppSh_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy eval function
// ---------------------------------------------------------------------------
extern CIDAPPSHEXP TFacCIDAppSh& facCIDAppSh();

