//
// FILE NAME: CIDCtrls_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/1997
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
//  This is the main internal header for the facility. It includes the main
//  public header, and any internal ones that are needed. It also includes
//  the system headers, which are only used internally.
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
#include    "CIDCtrls.hpp"


// ---------------------------------------------------------------------------
//  Include any external headers we only need internally
// ---------------------------------------------------------------------------
#define     OEMRESOURCE
#include    <windows.h>
#include    <Commctrl.h>

// Causes a big problem when it redefines EWndThemes::DialgoBox
#undef      DialogBox


// ---------------------------------------------------------------------------
//  Underlying stuff we only need internally
// ---------------------------------------------------------------------------
#include    "CIDPNG.hpp"


// ---------------------------------------------------------------------------
//  This is the intra-facilities types namespace. It contains non-class types
//  for use within the facility.
// ---------------------------------------------------------------------------
namespace tCIDCtrls_
{
}


// ---------------------------------------------------------------------------
//  This is the intra-facilities constants namespace.
// ---------------------------------------------------------------------------
namespace kCIDCtrls_
{
}


// ---------------------------------------------------------------------------
//  Include any internal headers
// ---------------------------------------------------------------------------
#include    "CIDCtrls_MessageIds.hpp"


