//
// FILE NAME: CIDOrbUC.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
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
//  This facility is where the ORB really meets reality. At the level of the
//  ORB engine itself (CIDOrb facility), only the most basic functionality of
//  dispatching messages from clients and processing messages on the servers
//  is supported. And the stuff that is required to queue up those things on
//  both sides and correctly get messages from clients to the right remote
//  interface and back.
//
//  At this level we provide tools for creating client proxy objects, and
//  provide pre-fab versions of some of the most common ones, for the name
//  server, configuration server and log server, plus some commonly used
//  interfaces that some servers may implement.
//
//  Our facility class provides a number of utility methods that all ORB
//  based programs will use.
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
#if     defined(PROJ_CIDORBUC)
#define CIDORBUCEXP CID_DLLEXPORT
#else
#define CIDORBUCEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDOrb.hpp"

#include    "CIDObjStore.hpp"


// ---------------------------------------------------------------------------
//  Include our fundamental headers
// ---------------------------------------------------------------------------
#include    "CIDOrbUC_ErrorIds.hpp"
#include    "CIDOrbUC_MessageIds.hpp"
#include    "CIDOrbUC_Type.hpp"
#include    "CIDOrbUC_Constant.hpp"

#include    "CIDOrbUC_NSInfo.hpp"
#include    "CIDOrbUC_NameSrvClientProxy.hpp"
#include    "CIDOrbUC_LogSrvClientProxy.hpp"
#include    "CIDOrbUC_CfgSrvClientProxy.hpp"
#include    "CIDOrbUC_RemBrowserClientProxy.hpp"
#include    "CIDOrbUC_CoreAdminClientProxy.hpp"



// ---------------------------------------------------------------------------
//  We couldn't put these into CIDOrbUC_Constant.hpp, because they require
//  knowledge of the client proxy classes we generate (headers above.) So we
//  extend the types namespace here for this stuff.
// ---------------------------------------------------------------------------
namespace tCIDOrbUC
{
    // -----------------------------------------------------------------------
    //  A typedef for the very commonly used counted pointer to a name server
    //  client proxy.
    // -----------------------------------------------------------------------
    using TNSrvProxy = TCntPtr<TCIDNameSrvClientProxy>;
    using TLSrvProxy = TCntPtr<TCIDLogSrvClientProxy>;
    using TRemBrowserProxy = TCntPtr<TRemBrowserClientProxy>;
    using TCoreAdminProxy = TCntPtr<TCIDCoreAdminClientProxy>;
}


// ---------------------------------------------------------------------------
//  And now include the higher level headers, which might need to see stuff
//  from the namespaces above.
// ---------------------------------------------------------------------------
#include    "CIDOrbUC_LogSrvServerBase.hpp"
#include    "CIDOrbUC_CoreAdminServerBase.hpp"
#include    "CIDOrbUC_CoreAdminBaseImpl.hpp"
#include    "CIDOrbUC_RemBrowserServerBase.hpp"

#include    "CIDOrbUC_RemDiskBrowserImpl.hpp"

#include    "CIDOrbUC_CfgSrvClient.hpp"
#include    "CIDOrbUC_LogSrvLogger.hpp"
#include    "CIDOrbUC_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object
// ---------------------------------------------------------------------------
extern CIDORBUCEXP TFacCIDOrbUC& facCIDOrbUC();


// ---------------------------------------------------------------------------
//  Streaming support for some of our enums
// ---------------------------------------------------------------------------
EnumBinStreamMacros(tCIDOrbUC::EDumpFmts)

