//
// FILE NAME: CIDWebBrowser_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the public types header for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace tWebBrowser
{
    // -----------------------------------------------------------------------
    //  The simple on/off options we support
    // -----------------------------------------------------------------------
    enum class EBoolOpts
    {
        ShowAddrBar
        , ShowToolBar
        , SilentMode
    };


    // -----------------------------------------------------------------------
    //  Navigation operations supported
    // -----------------------------------------------------------------------
    enum class ENavOps
    {
        Back
        , Forward
        , Home
        , Search
        , Reload
        , Stop
    };
}

EnumBinStreamMacros(tWebBrowser::ENavOps)
