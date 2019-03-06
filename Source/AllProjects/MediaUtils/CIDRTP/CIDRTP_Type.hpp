//
// FILE NAME: CIDRTP_Types.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the internal types and constants header for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace tCIDRTP
{
    //
    //  The commands that the RTSP client can send us once the session is up
    //  and running. The command is translated to this enum and passed to the
    //  session object.
    //
    enum class ERTSPCmds
    {
        Play
        , Pause

        , Count
    };


    //
    //  The possible states of an RTPS session.
    //
    enum class ERTSPStates
    {
        Ready
        , Playing
        , Recording
        , Done
    };
};


#pragma CIDLIB_POPPACK

