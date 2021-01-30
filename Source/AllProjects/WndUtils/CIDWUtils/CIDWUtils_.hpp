//
// FILE NAME: CIDWUtils_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
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
//  Include any external headers we only need internally. The code analyzer doesn't
//  always correctly ignore errors here, so force it to.
// ---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#include    <windows.h>
#include    <Commctrl.h>
#pragma warning(pop)

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

