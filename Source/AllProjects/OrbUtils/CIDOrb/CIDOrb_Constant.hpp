//
// FILE NAME: CIDOrb_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/08/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for our constants.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace kCIDOrb
{
    // -----------------------------------------------------------------------
    //  The maximum single packet size we'll handle in the ORB. Anyone looking
    //  to transfer more than this has to chunk it himself.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4MaxPayload = 0x800000;


    // -----------------------------------------------------------------------
    //  Some common constants used by both client and server internals
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4IdModulus = 109;


    // -----------------------------------------------------------------------
    //  The time limit within which a binding lease must be renewed or it
    //  will be removed from the name server. And also the time that the
    //  client side rebinder uses as a safety value. It will want to try to
    //  renew any that are older than this.
    // -----------------------------------------------------------------------
    const tCIDLib::TEncodedTime enctBindingLeaseTime = (3 * kCIDLib::enctOneMinute);
    const tCIDLib::TEncodedTime enctBindingThreshold = (2 * kCIDLib::enctOneMinute);


    // -----------------------------------------------------------------------
    //  If the client side ORB server target hasn't been used by any of the
    //  client proxies that reference it for some time, we'll just send a
    //  ping ourself to the server, so it won't drop the connect as idle.
    // -----------------------------------------------------------------------
    const tCIDLib::TEncodedTime enctMaxIdle = (60 * kCIDLib::enctOneSecond);
    const tCIDLib::TEncodedTime enctHalfMaxIdle = (30 * kCIDLib::enctOneSecond);


    // -----------------------------------------------------------------------
    //  Internal Orb message ids
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4Accepted      = 0x19A458F1;
    const tCIDLib::TCard4   c4RejectTooMany = 0xE9220A4C;
    const tCIDLib::TCard4   c4NotBlessed    = 0x9FF98FA3;


    // -----------------------------------------------------------------------
    //  Some status that we register in the stats cache, for giving a view of
    //  the health of the ORB. These are per-process, so there's no need for
    //  a process name to be put into the key paths.
    //
    //  Some of these are actually maintained by the CIDOrbUC facility.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszStat_Scope_ORB           = L"/Stats/ORB/";
    const tCIDLib::TCh* const   pszStat_Scope_Client        = L"/Stats/ORB/Client/";
    const tCIDLib::TCh* const   pszStat_Scope_Server        = L"/Stats/ORB/Srv/";

    const tCIDLib::TCh* const   pszStat_Cl_CmdCache         = L"/Stats/ORB/Client/CmdCache";
    const tCIDLib::TCh* const   pszStat_Cl_RebindFails      = L"/Stats/ORB/Client/RebindFails";
    const tCIDLib::TCh* const   pszStat_Cl_SrvCache         = L"/Stats/ORB/Client/SrvCache";
    const tCIDLib::TCh* const   pszStat_Cl_SrvTargets       = L"/Stats/ORB/Client/SrvTargets";
    const tCIDLib::TCh* const   pszStat_Cl_WaitList         = L"/Stats/ORB/Client/WaitList";

    const tCIDLib::TCh* const   pszStat_Srv_ActiveCmds      = L"/Stats/ORB/Srv/ActiveCmds";
    const tCIDLib::TCh* const   pszStat_Srv_ClientHWMark    = L"/Stats/ORB/Srv/ClientHWMark";
    const tCIDLib::TCh* const   pszStat_Srv_CurClients      = L"/Stats/ORB/Srv/CurClients";
    const tCIDLib::TCh* const   pszStat_Srv_DroppedRetPacks = L"/Stats/ORB/Srv/DroppedRetPacks";
    const tCIDLib::TCh* const   pszStat_Srv_MaxClients      = L"/Stats/ORB/Srv/MaxClients";
    const tCIDLib::TCh* const   pszStat_Srv_QueuedCmds      = L"/Stats/ORB/Srv/QueuedCmds";
    const tCIDLib::TCh* const   pszStat_Srv_RegisteredObjs  = L"/Stats/ORB/Srv/RegisteredObjs";
    const tCIDLib::TCh* const   pszStat_Srv_WorkerThreads   = L"/Stats/ORB/Srv/WorkThreads";
    const tCIDLib::TCh* const   pszStat_Srv_WorkQItems      = L"/Stats/ORB/Srv/WorkQItems";
}

#pragma CIDLIB_POPPACK

