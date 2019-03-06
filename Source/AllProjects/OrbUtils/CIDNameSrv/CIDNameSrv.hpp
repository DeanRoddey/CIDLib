//
// FILE NAME: CIDNameSrv.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/09/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main header of the CIDLib naming service. This program is
//  a backend program which implements a simple CIDIDL interface. This
//  interface allows other programs to register their server side ORB
//  objects under a hierarchical name space. Other programs can then search
//  through this hierarchy, find the objects that they need to interact
//  with, and get acces to those objects.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDOrbUC.hpp"


// ---------------------------------------------------------------------------
//  And sub-include our core headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDNameSrv_ErrorIds.hpp"
#include    "CIDNameSrv_MessageIds.hpp"


// ---------------------------------------------------------------------------
//  Types namespace
// ---------------------------------------------------------------------------
namespace tCIDNameSrv
{
}


// ---------------------------------------------------------------------------
//  Constants namespace
// ---------------------------------------------------------------------------
namespace kCIDNameSrv
{
}


// ---------------------------------------------------------------------------
//  And sub-include our higher level headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDNameSrv_Node.hpp"
#include    "CIDNameSrv_CoreAdminImpl.hpp"
#include    "CIDNameSrv_NameSrvServerBase.hpp"
#include    "CIDNameSrv_Impl.hpp"
#include    "CIDNameSrv_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Make the facility object visible to all the program files. It is actually
//  declared in the main cpp file.
// ---------------------------------------------------------------------------
extern TFacCIDNameSrv   facCIDNameSrv;

