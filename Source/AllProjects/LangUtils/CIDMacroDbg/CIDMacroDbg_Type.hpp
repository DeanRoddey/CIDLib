//
// FILE NAME: CIDMacroDbg_Type.hpp
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


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace tCIDMacroDbg
{
    // -----------------------------------------------------------------------
    //  The events we send out to notification handlers
    // -----------------------------------------------------------------------
    enum class EEvents
    {
        IDEState
        , LineChanged
        , NewFile
    };


    // -----------------------------------------------------------------------
    //  Returned from the macro exception display dialog. Indicates what
    //  they want to do.
    // -----------------------------------------------------------------------
    enum class EMExceptRes
    {
        Ok
        , GoTo
        , Stop
    };
}

#pragma CIDLIB_POPPACK


