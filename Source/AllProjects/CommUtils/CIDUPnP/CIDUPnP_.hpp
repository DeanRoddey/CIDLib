//
// FILE NAME: CIDUPnP_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/20110
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main internal header for the facility. It includes the main
//  public header, any internal headers that are needed, intrafacility
//  function prototypes, intrafacility constants, and macros.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our own public header and any internal headers we need
// ---------------------------------------------------------------------------
#include    "CIDUPnP.hpp"
#include    "CIDUPnP_MessageIds.hpp"


// ---------------------------------------------------------------------------
//  Bring in underlying headers that only have to be visible internally
// ---------------------------------------------------------------------------
#include    "CIDSock.hpp"

#include    "CIDXML.hpp"

#include    "CIDNet.hpp"



// ---------------------------------------------------------------------------
//  This is the intra-facilities types namespace. It contains non-class types
//  for use within the facility.
// ---------------------------------------------------------------------------
namespace tCIDUPnP_
{
}


// ---------------------------------------------------------------------------
//  This is the intra-facilities constants namespace.
// ---------------------------------------------------------------------------
namespace kCIDUPnP_
{
    // -----------------------------------------------------------------------
    //  Some common text values we use in UPnP parameters or XML it returns.
    // -----------------------------------------------------------------------
    const TString   strVal_False(L"False");
    const TString   strVal_Master(L"Master");
    const TString   strVal_NotImpl(L"NOT_IMPLEMENTED");
    const TString   strVal_One(L"1");
    const TString   strVal_True(L"True");
    const TString   strVal_Zero(L"0");
}

