//
// FILE NAME: CIDEncode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/22/1994
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the public header for the CIDEncode Facility. The client
//  application only has to include this one header to access all of the
//  functionality of this facility.
//
//  This facility provides text encoding/decoding functionality. It handles
//  transcoding text between the internal Unicode format of CQC and external
//  encodings and code pages.
//
//  Generally encodings are created by calling methods on the facility
//  class and generating them dynamically from an encoding name. So we don't
//  export the headers (which also lowers downstream compilation overhead.)
//  If you absolutely have to you can include one of the headers directly.
//
//  Some of the core ones, UTF, ASCII, are down in the CIDLib layer, so
//  those are always directly visible.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Import any needed underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDENCODE)
#define CIDENCODEEXP    CID_DLLEXPORT
#else
#define CIDENCODEEXP    CID_DLLIMPORT
#endif



// ---------------------------------------------------------------------------
//  Include all of the public source module headers. These includes are in the
//  correct order of dependency so that the client application does not need
//  to worry about it.
// ---------------------------------------------------------------------------
#include    "CIDEncode_ErrorIds.hpp"
#include    "CIDEncode_Type.hpp"
#include    "CIDEncode_ThisFacility.hpp"
#include    "CIDEncode_Mode1Base.hpp"


// ---------------------------------------------------------------------------
//  Export the facility lazy evaluation object
// ---------------------------------------------------------------------------
extern CIDENCODEEXP TFacCIDEncode& facCIDEncode();

