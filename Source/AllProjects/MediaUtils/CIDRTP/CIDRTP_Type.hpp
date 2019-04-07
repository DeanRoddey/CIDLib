//
// FILE NAME: CIDRTP_Types.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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

