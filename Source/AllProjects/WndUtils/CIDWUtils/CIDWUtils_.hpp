//
// FILE NAME: CIDWUtils_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our own public header
// ---------------------------------------------------------------------------
#include    "CIDWUtils.hpp"


// ---------------------------------------------------------------------------
//  Include any external headers we only need internally
// ---------------------------------------------------------------------------
#include    <windows.h>
#include    <Commctrl.h>

// Causes lots of problems
#undef      DialogBox


// ---------------------------------------------------------------------------
// Bring in any underlying stuff that we only need internally
// ---------------------------------------------------------------------------
#include    "CIDComm.hpp"
#include    "CIDRegX.hpp"


// ---------------------------------------------------------------------------
// Bring in our own headers we only need internally
// ---------------------------------------------------------------------------
#include    "CIDWUtils_MessageIds.hpp"
#include    "CIDWUtils_ResourceIds.hpp"

