//
// FILE NAME: CIDComm.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
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
//  have to offer.
//
//  This facility implements core support for serial ports. These are supported
//  via an abstract base class that defines the functionality that comm ports
//  must support. Then we provide a derivative for regular local ports.
//
//  We also define a 'port factory' abstraction which is a means by which
//  various types of serisl ports are made available to the program. We provide
//  a concreate implementation for local ports.
//
//  This factory scheme is mostly used in CQC where it allows CQC programs to
//  use serial ports on GC-100s, Just Add Power devices, and also to support
//  its remote port server that allows for access to remote client ports.
//
//  At some point we might move some of that down to this level.
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
#if     defined(PROJ_CIDCOMM)
#define CIDCOMMEXP  CID_DLLEXPORT
#else
#define CIDCOMMEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDComm_Type.hpp"
#include    "CIDComm_ErrorIds.hpp"

#include    "CIDComm_PortCfg.hpp"
#include    "CIDComm_PortBase.hpp"
#include    "CIDComm_PortFactory.hpp"

#include    "CIDComm_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDCOMMEXP TFacCIDComm& facCIDComm();

