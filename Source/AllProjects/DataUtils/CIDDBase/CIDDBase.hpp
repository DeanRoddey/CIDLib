//
// FILE NAME: CIDDBase.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
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
//  of contact for the outside world. By including it, they get all that we
//  have to offer.
//
//  This facility provides an objectified ODBC interface. So it provides wrappers
//  for connections, statements, column, bindings and such.
//
//  You must call the initialization method on the facility object to enable
//  database support first. And you should call the terminate method to be a
//  good citizen before you exit, or at any point where you no longer need to
//  do database operations.
//
//  This lets the per-platform support do any required work to enable database
//  support. That info is available to any other per-platform code.
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
#if     defined(PROJ_CIDDBASE)
#define CIDDBASEEXP CID_DLLEXPORT
#else
#define CIDDBASEEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers. CIDLib will bring in what we need
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDDBase_Types.hpp"
#include    "CIDDBase_ErrorIds.hpp"

#include    "CIDDBase_Connection.hpp"
#include    "CIDDBase_Bindings.hpp"
#include    "CIDDBase_Statement.hpp"
#include    "CIDDBase_DirectStatement.hpp"
#include    "CIDDBase_TransJanitor.hpp"

#include    "CIDDBase_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility factory method
// ---------------------------------------------------------------------------
extern CIDDBASEEXP TFacCIDDBase& facCIDDBase();

