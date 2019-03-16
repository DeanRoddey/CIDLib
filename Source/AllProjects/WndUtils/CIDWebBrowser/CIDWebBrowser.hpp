//
// FILE NAME: CIDWebBrowser.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2005
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
#if     defined(PROJ_CIDWEBBROWSER)
#define CIDWBRWSEXP  DLLEXPORT
#else
#define CIDWBRWSEXP  DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our needed underlying headers. CIDCtrls brings in most of what
//  we need.
// ---------------------------------------------------------------------------
#include    "CIDCtrls.hpp"


// ---------------------------------------------------------------------------
//  Include our public headers
// ---------------------------------------------------------------------------
#include    "CIDWebBrowser_Type.hpp"
#include    "CIDWebBrowser_ErrorIds.hpp"
#include    "CIDWebBrowser_BrwsWindow.hpp"
#include    "CIDWebBrowser_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function.
// ---------------------------------------------------------------------------
extern CIDWBRWSEXP TFacCIDWebBrowser& facCIDWebBrowser();

