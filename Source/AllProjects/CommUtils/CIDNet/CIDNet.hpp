//
// FILE NAME: CIDNet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#if     defined(PROJ_CIDNET)
#define CIDNETEXP   CID_DLLEXPORT
#else
#define CIDNETEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDSock.hpp"
#include    "CIDSChan.hpp"
#include    "CIDXML.hpp"



// ---------------------------------------------------------------------------
//  Include the fundamental headers first
// ---------------------------------------------------------------------------
#include    "CIDNet_ErrorIds.hpp"
#include    "CIDNet_Shared.hpp"
#include    "CIDNet_Constant.hpp"
#include    "CIDNet_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDNETEXP TFacCIDNet& facCIDNet();


// ---------------------------------------------------------------------------
//  And subinclude our other headers
// ---------------------------------------------------------------------------
#include    "CIDNet_CoreParser.hpp"
#include    "CIDNet_HTTPClient.hpp"
#include    "CIDNet_SMTPClient.hpp"
#include    "CIDNet_JSONParser.hpp"
#include    "CIDNet_XMLURLEntitySrc.hpp"



// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDNET_PREINST)
extern template class TRefQueue<TEmailMsg>;
extern template class TRefVector<TEmailAttachment>;
extern template class TRefVector<TJSONValue>;
#endif


