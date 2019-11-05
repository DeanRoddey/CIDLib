//
// FILE NAME: CIDOrb_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/08/2001
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
    constexpr tCIDLib::TCard4       c4MaxPayload = 0x800000;


    // -----------------------------------------------------------------------
    //  Some common constants used by both client and server internals
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4IdModulus = 109;


    // -----------------------------------------------------------------------
    //  The time limit within which a binding lease must be renewed or it
    //  will be removed from the name server. And also the time that the
    //  client side rebinder uses as a safety value. It will want to try to
    //  renew any that are older than this.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TEncodedTime enctBindingLeaseTime = (3 * kCIDLib::enctOneMinute);
    constexpr tCIDLib::TEncodedTime enctBindingThreshold = (2 * kCIDLib::enctOneMinute);


    // -----------------------------------------------------------------------
    //  If the client side ORB server target hasn't been used by any of the
    //  client proxies that reference it for some time, we'll just send a
    //  ping ourself to the server, so it won't drop the connect as idle.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TEncodedTime enctMaxIdle = (60 * kCIDLib::enctOneSecond);
    constexpr tCIDLib::TEncodedTime enctHalfMaxIdle = (30 * kCIDLib::enctOneSecond);


    // -----------------------------------------------------------------------
    //  Internal Orb message ids
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4   c4Accepted      = 0x19A458F1;
    constexpr tCIDLib::TCard4   c4RejectTooMany = 0xE9220A4C;
    constexpr tCIDLib::TCard4   c4NotBlessed    = 0x9FF98FA3;


    // -----------------------------------------------------------------------
    //  Some status that we register in the stats cache, for giving a view of
    //  the health of the ORB. These are per-process, so there's no need for
    //  a process name to be put into the key paths.
    //
    //  Some of these are actually maintained by the CIDOrbUC facility.
    // -----------------------------------------------------------------------
    constexpr const tCIDLib::TCh* const   pszStat_Scope_ORB           = L"/Stats/ORB/";
    constexpr const tCIDLib::TCh* const   pszStat_Scope_Client        = L"/Stats/ORB/Client/";
    constexpr const tCIDLib::TCh* const   pszStat_Scope_Server        = L"/Stats/ORB/Srv/";

    constexpr const tCIDLib::TCh* const   pszStat_Cl_CmdCache         = L"/Stats/ORB/Client/CmdCache";
    constexpr const tCIDLib::TCh* const   pszStat_Cl_RebindFails      = L"/Stats/ORB/Client/RebindFails";
    constexpr const tCIDLib::TCh* const   pszStat_Cl_SrvCache         = L"/Stats/ORB/Client/SrvCache";
    constexpr const tCIDLib::TCh* const   pszStat_Cl_SrvTargets       = L"/Stats/ORB/Client/SrvTargets";
    constexpr const tCIDLib::TCh* const   pszStat_Cl_WaitList         = L"/Stats/ORB/Client/WaitList";

    constexpr const tCIDLib::TCh* const   pszStat_Srv_ActiveCmds      = L"/Stats/ORB/Srv/ActiveCmds";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_ClientHWMark    = L"/Stats/ORB/Srv/ClientHWMark";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_CurClients      = L"/Stats/ORB/Srv/CurClients";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_DroppedRetPacks = L"/Stats/ORB/Srv/DroppedRetPacks";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_MaxClients      = L"/Stats/ORB/Srv/MaxClients";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_QueuedCmds      = L"/Stats/ORB/Srv/QueuedCmds";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_RegisteredObjs  = L"/Stats/ORB/Srv/RegisteredObjs";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_WorkerThreads   = L"/Stats/ORB/Srv/WorkThreads";
    constexpr const tCIDLib::TCh* const   pszStat_Srv_WorkQItems      = L"/Stats/ORB/Srv/WorkQItems";
}

#pragma CIDLIB_POPPACK

