//
// FILE NAME: CIDOrb_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2000
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
//  This is the header for our non-class types.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace tCIDOrb
{
    // -----------------------------------------------------------------------
    //  Used by the TOrbReply and TOrbCmd classes
    // -----------------------------------------------------------------------
    enum class ECmdModes
    {
        Cmd
        , Reply
    };


    // -----------------------------------------------------------------------
    //  The stages a command item goes through on the client. The state mean:
    //
    //  Free      - It's available for use by client threads
    //  Wait      - The command has been queued by a client thread, and the
    //              thread is waiting on it to be picked up.
    //  CmdQ      - It is waiting in the command queue waiting to get sent
    //  ReplyList - It is in the reply list, waiting for the reply to come
    //              back.
    //  Ready     - The command has been processed and the reply has been
    //              placed into it.
    //  Orphaned  - The client has given up waiting, so the client ORB should
    //              return the item to the cache itself.
    // -----------------------------------------------------------------------
    enum class ECmdStages
    {
        Free
        , Wait
        , CmdQ
        , ReplyList
        , Ready
        , Orphaned
    };


    // -----------------------------------------------------------------------
    //  The result of a packet read
    // -----------------------------------------------------------------------
    enum class EReadRes
    {
        Lost
        , NoPacket
        , Packet
        , KeepAlive
    };


    // -----------------------------------------------------------------------
    //  The result of each item in a rebind pass to the name server
    // -----------------------------------------------------------------------
    enum class ERebindRes
    {
        Success
        , NotATerminal
        , Exception
    };


    //
    //  A simple packet header which is prepended to packets going either
    //  way in the ORB. It needs to be byte packed. It's never persisted
    //  so it can be changed, but that will break backwards version
    //  compatibility.
    //
    #pragma CIDLIB_PACK(1)
    struct TPacketHdr
    {
        tCIDLib::TCard4     c4MagicVal;
        tCIDLib::THashVal   hshData;
        tCIDLib::TCard4     c4DataBytes;
        tCIDLib::TCard4     c4SequenceId;
        tCIDLib::TCard4     c4MagicVal2;
    };
    #pragma CIDLIB_POPPACK
}

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Streaming macros for some of our enum types
// ---------------------------------------------------------------------------
EnumStreamMacros(tCIDOrb::ECmdModes)
EnumBinStreamMacros(tCIDOrb::ERebindRes)
