//
// FILE NAME: CIDDBase.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get all that we
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
#include    "CIDDBase_Constants.hpp"
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

