//
// FILE NAME: CIDWebBrowser_Type.hpp
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
