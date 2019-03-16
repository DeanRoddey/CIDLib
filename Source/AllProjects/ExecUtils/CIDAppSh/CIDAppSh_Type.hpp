//
// FILE NAME: CIDAppSh_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/01/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the types header for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  Public types namespace
// ---------------------------------------------------------------------------
namespace tCIDAppSh
{
    // -----------------------------------------------------------------------
    //  The status that an app shell record can have. This let's the monitor
    //  thread do the correct thing. The meaning of the states are:
    //
    //  1.  Inactive. Client code as stopped it and asked us to keep it
    //      stopped for now. So we don't try to restart it.
    //  2.  WaitRun. It's active, and it waiting to be started, or restarted
    //      if it failed or existed or was stopped without being marke as
    //      inactive.
    //  3.  Running. Last we know we started it running. So, if we see that it
    //      is not, it's because it exited for some reason, so we need to
    //      report the error and put it back into WaitRun state to try to get
    //      it going again.
    // -----------------------------------------------------------------------
    enum class EStatus
    {
        Inactive
        , WaitRun
        , Running
    };


    // -----------------------------------------------------------------------
    //  The available application levels. Apps at the same level are assumed
    //  to have no interdependencies, and can be started or stopped in
    //  parallel.
    // -----------------------------------------------------------------------
    enum class EAppLevels
    {
        L0
        , L1
        , L2
        , L3
        , L4
        , L5
        , L6
        , L7
        , L8
        , L9

        , Count
        , Min           = L0
        , Max           = L9
    };
}


// ---------------------------------------------------------------------------
//  Do standard macros for some enums
// ---------------------------------------------------------------------------
StdEnumTricks(tCIDAppSh::EAppLevels)

#pragma CIDLIB_POPPACK

