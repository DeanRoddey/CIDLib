//
// FILE NAME: CIDMacroDbg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
//  Include most of our own public header and any internal headers
// ---------------------------------------------------------------------------
#include    "CIDMacroDbg.hpp"


// ---------------------------------------------------------------------------
//  Bring in any facilities that we only need internally
// ---------------------------------------------------------------------------
#include    "CIDCtrls.hpp"

#include    "CIDWUtils.hpp"


// ---------------------------------------------------------------------------
//  And bring in all our internal headers
// ---------------------------------------------------------------------------
#include    "CIDMacroDbg_MessageIds.hpp"
#include    "CIDMacroDbg_GetParmsDlg_.hpp"

#include    "CIDMacroDbg_BuildResTab_.hpp"
#include    "CIDMacroDbg_TextOutTab_.hpp"
#include    "CIDMacroDbg_ObjListTab_.hpp"
#include    "CIDMacroDbg_SearchTab_.hpp"
#include    "CIDMacroDbg_BreakPntsTab_.hpp"



// ---------------------------------------------------------------------------
//  Internal constants
// ---------------------------------------------------------------------------
namespace kCIDMacroDbg_
{
    // -----------------------------------------------------------------------
    //  These are the names we give to the various windows, for logging and lookup
    //  purposes.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszTab_BreakPoints      = L"BreakPoints";
    const tCIDLib::TCh* const   pszTab_BuildResults     = L"BuildResults";
    const tCIDLib::TCh* const   pszTab_CallStack        = L"CallStack";
    const tCIDLib::TCh* const   pszTab_DisplayLocals    = L"DisplayLocals";
    const tCIDLib::TCh* const   pszTab_DisplayMembers   = L"DisplayMembers";
    const tCIDLib::TCh* const   pszTab_DisplayParms     = L"DisplayParms";
    const tCIDLib::TCh* const   pszTab_MacroOutput      = L"MacroOutput";
    const tCIDLib::TCh* const   pszTab_SearchTab        = L"SearchTab";
    const tCIDLib::TCh* const   pszTab_SrcViewer        = L"SourceViewer";


    // -----------------------------------------------------------------------
    //  Max lines that the queued output stream (used as the macro console
    //  stream) can hold. We have to get the data out before this many messages
    //  are output, or we'll miss some.
    //
    //  And the max lines of macro output we can display without throwing away
    //  old lines.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4MaxMacroOutLines = 512;


    // -----------------------------------------------------------------------
    //  How far the various MDI windows deflate inside the client border to
    //  create the area for their own content.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4BorderMargin = 4;
}

