//
// FILE NAME: CIDAudStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
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
//  This facility defines basic audio in/out stream interfaces. It also provides
//  implementations in terms of the Wave interface of windows which provides a
//  reasonably simple but efficient mechanism for buffered audio streaming.
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
#if     defined(PROJ_CIDAUDSTREAM)
#define CIDAUDSTREAMEXP  CID_DLLEXPORT
#else
#define CIDAUDSTREAMEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ---------------------------------------------------------------------------
//  Facility constants namespace
// ---------------------------------------------------------------------------
namespace kCIDAudStream
{

}


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDAudStream_ErrorIds.hpp"
#include    "CIDAudStream_Type.hpp"
#include    "CIDAudStream_Shared.hpp"
#include    "CIDAudStream_SrcStream.hpp"
#include    "CIDAudStream_WaveInStream.hpp"
#include    "CIDAudStream_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDAUDSTREAMEXP TFacCIDAudStream& facCIDAudStream();


