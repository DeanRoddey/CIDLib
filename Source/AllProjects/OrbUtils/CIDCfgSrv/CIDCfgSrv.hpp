//
// FILE NAME: CIDCfgSrv.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main header of the CIDLib configuration server. This program
//  is a backend program which implements a centralized configuration data
//  repository, to allow distributed applications to keep all their config
//  data in a centralized place, for easy maintenance.
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
#include    "CIDObjStore.hpp"



// ---------------------------------------------------------------------------
//  And sub-include our core headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDCfgSrv_ErrorIds.hpp"
#include    "CIDCfgSrv_MessageIds.hpp"


// ---------------------------------------------------------------------------
//  And sub-include our higher level headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDCfgSrv_Type.hpp"
#include    "CIDCfgSrv_Constant.hpp"
#include    "CIDCfgSrv_CoreAdminImpl.hpp"
#include    "CIDCfgSrv_CfgServerBase.hpp"
#include    "CIDCfgSrv_CfgServerImpl.hpp"
#include    "CIDCfgSrv_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Make the facility object visible to all the program files. It is actually
//  declared in the main cpp file.
// ---------------------------------------------------------------------------
extern TFacCIDCfgSrv   facCIDCfgSrv;

