//
// FILE NAME: CIDDAE.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer.
//
//  This facility provides digital audio extraction support, to pull digital
//  audio off of a CDROM disk and support for converting that data to various
//  file formats.
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
#if     defined(PROJ_CIDDAE)
#define CIDDAEEXP  CID_DLLEXPORT
#else
#define CIDDAEEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ---------------------------------------------------------------------------
//  This one isn't normally included, so we have to do it manually
// ---------------------------------------------------------------------------
#if         !defined(CIDKERNEL_REMMEDIA_HPP)
#include    "CIDKernel_RemMedia.hpp"
#endif


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TJPEGImage;


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDDAE_ErrorIds.hpp"
#include    "CIDDAE_Shared.hpp"
#include    "CIDDAE_Type.hpp"

#include    "CIDDAE_Codec.hpp"
#include    "CIDDAE_Ripper.hpp"

#include    "CIDDAE_WaveEncoder.hpp"
#include    "CIDDAE_WMAEncoder.hpp"

#include    "CIDDAE_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDDAEEXP TFacCIDDAE& facCIDDAE();


