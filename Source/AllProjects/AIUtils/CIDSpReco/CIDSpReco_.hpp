//
// FILE NAME: CIDSpReco_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
//  This is the main internal header for the facility. It provides any
//  internal non-class types, constants, enums, etc...
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
#include    "CIDSpReco.hpp"
#include    <windows.h>

__pragma(warning(push))
__pragma(warning(disable: 4996))
#include    <sphelper.h>
__pragma(warning(pop))


// ---------------------------------------------------------------------------
//  Bring in any underlying headers we only need internally
// ---------------------------------------------------------------------------
#include    "CIDKernel_InternalHelpers_.hpp"

#include    "CIDXML.hpp"


// ---------------------------------------------------------------------------
//  Internal types namespace
// ---------------------------------------------------------------------------
namespace tCIDSpReco_
{
};


// ---------------------------------------------------------------------------
//  Internal constants namespace
// ---------------------------------------------------------------------------
namespace kCIDSpReco_
{
};


// ---------------------------------------------------------------------------
//  Bring in our internal headers
// ---------------------------------------------------------------------------
#include    "CIDSpReco_WrapperStream_.hpp"

