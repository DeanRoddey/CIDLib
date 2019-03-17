//
// FILE NAME: ORB1SampS.hpp
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
//  This is the server side part of the ORB1 Sample program. See the readme
//  file in the top level directory of this sample for details, but basically
//  we create an instance of the remote interface, register it with the name
//  server, and wait for incoming requests from the client.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

// ---------------------------------------------------------------------------
//  Bring in the underlying includes we need
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDORB.hpp"
#include    "CIDORBUC.hpp"

#include    "ORB1SampSh.hpp"


// ---------------------------------------------------------------------------
//  Bring in our own includes
// ---------------------------------------------------------------------------
#include    "ORB1SampS_ErrorIds.hpp"
#include    "ORB1SampS_IntfServerBase.hpp"
#include    "ORB1SampS_IntfImpl.hpp"
