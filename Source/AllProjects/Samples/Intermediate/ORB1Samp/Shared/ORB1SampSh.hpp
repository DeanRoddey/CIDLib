//
// FILE NAME: ORB1SampSh.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2019
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
//  This is the main header for the facility that is shared between the client and
//  server sides. It defines the types and constants and such that they both need
//  to see.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#define ORB1SAMPSH_HPP
#if     defined(PROJ_ORB1SAMPSH)
#define ORB1SAMPSHEXP   CID_DLLEXPORT
#else
#define ORB1SAMPSHEXP   CID_DLLIMPORT
#endif



// ---------------------------------------------------------------------------
//  Bring in the underlying includes we need publicly
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Bring in our own includes
// ---------------------------------------------------------------------------
#include    "ORB1SampSh_Types.hpp"
#include    "ORB1SampSh_Record.hpp"

