//
// FILE NAME: CIDCfgSrv_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the the constants of this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace kCIDCfgSrv
{
    // -----------------------------------------------------------------------
    //  File related constants
    //
    //  pszRepFileName
    //      This is the name of the repository file we use for persistence.
    //      Its path can be changed, but the name is always the same. The
    //      object store adds the extension.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszRepFileName  = L"CIDCfgSrvRepo";
}

