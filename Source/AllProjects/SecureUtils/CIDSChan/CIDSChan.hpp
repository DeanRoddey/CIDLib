//
// FILE NAME: CIDSChan.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/2104
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
#if     defined(PROJ_CIDSCHAN)
#define CIDSCHANEXP CID_DLLEXPORT
#else
#define CIDSCHANEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDCrypto.hpp"
#include    "CIDSock.hpp"


// ---------------------------------------------------------------------------
//  Facility types
// ---------------------------------------------------------------------------
namespace tCIDSChan
{
    enum class EKeyTypes
    {
        Public
        , Private
    };
}


// ---------------------------------------------------------------------------
//  Include our public headers
// ---------------------------------------------------------------------------
#include "CIDSChan_ErrorIds.hpp"
#include "CIDSChan_Shared.hpp"
#include "CIDSChan_SChan.hpp"
#include "CIDSChan_DataSrc.hpp"
#include "CIDSChan_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function
// ---------------------------------------------------------------------------
extern CIDSCHANEXP TFacCIDSChan& facCIDSChan();


