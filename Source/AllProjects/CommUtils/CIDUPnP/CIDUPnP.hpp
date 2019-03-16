//
// FILE NAME: CIDUPnP.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2011
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
#if     defined(PROJ_CIDUPNP)
#define CIDUPNPEXP  CID_DLLEXPORT
#else
#define CIDUPNPEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers that have to be publically visible to
//  our consumers.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Temporary until we get this stuff worked out (to avoid lots of recompiles)
// ---------------------------------------------------------------------------
#include    "CIDKernel_UPnP.hpp"



// ---------------------------------------------------------------------------
//  We use pointers to some of the kernel level class. To avoid pushing those
//  headers on our clients, we just forward reference them here and bring the
//  kernel level headers in privately.
// ---------------------------------------------------------------------------
class TKrnlUPnPService;
class TKrnlUPnPDevice;
class TKrnlUPnPFinder;


// ---------------------------------------------------------------------------
//  Other forward references
// ---------------------------------------------------------------------------
class TXMLTreeElement;
class TXMLTreeParser;


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDUPnP_ErrorIds.hpp"
#include    "CIDUPnP_Shared.hpp"
#include    "CIDUPnP_Service.hpp"
#include    "CIDUPnP_MediaServices.hpp"
#include    "CIDUPnP_Device.hpp"
#include    "CIDUPnP_AsyncFinder.hpp"
#include    "CIDUPnP_SonosServices.hpp"
#include    "CIDUPnP_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDUPNPEXP TFacCIDUPnP& facCIDUPnP();



// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDUPNP_PREINST)
extern template class TRefKeyedHashSet<TUPnPAsyncFinderItem, TString, TStringKeyOps>;
#endif

