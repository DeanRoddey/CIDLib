//
// FILE NAME: CIDSock_SocketListener.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/10/1998
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
//  This is the header for the CIDSock_SocketListener.cpp file. This file
//  implements the TSocketListener class. It provides the functionality
//  to support the server side operation of waiting for a connecting client
//  and creating a new socket for that client connection.
//
//  This is much complicated because of IPV6 support and the need to be
//  compatible with platforms that do not support dual mode stacks. In such
//  situations we cannot create a single IPV6 socket, bind to ANY, and
//  accept both V4 and V6 connections. Instead we have to iterate the available
//  interfaces and listen on any of them that are IP family connections.
//
//  So we may have up to two (V4 and V6) sockets for each of the available
//  interfaces. This kind of sucks, but we haev to support XP and the dual
//  mode stack isn't available until Vista or beyond.
//
//  This could be easily updated to also support a single, specific address
//  binding as well, by just providing a new ctor that takes an IP end point
//  and doing a family specific binding on that.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that this class is not derived from TSocket, since it is not
//      really a socket in the communications sense. It is only used to
//      listen for connections, and as mentioned before it's not necessarily
//      a single socket.
//
//  2)  The new V6 issues discussed above also mean that there's no longer
//      a single listening address as their used to be.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TSocketListener
//  PREFIX: sockl
// ---------------------------------------------------------------------------
class CIDSOCKEXP TSocketListener : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSocketListener() = delete;

        TSocketListener
        (
            const   tCIDLib::TIPPortNum     ippnToUse
            , const tCIDLib::TCard4         c4MaxWaiting = 0
        );

        TSocketListener
        (
            const   tCIDLib::TIPPortNum     ippnToUse
            , const tCIDSock::ESockProtos   eProtocol
            , const tCIDLib::TCard4         c4MaxWaiting = 0
        );

        TSocketListener(const TSocketListener&) = delete;

        ~TSocketListener();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSocketListener& operator=(const TSocketListener&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Count() const;

        tCIDLib::TCard4 c4MaxWaiting() const;

        tCIDLib::TVoid Cleanup();

        tCIDLib::TIPPortNum ippnListenOn() const;

        [[nodiscard]] TServerStreamSocket* psockListenFor
        (
            const   tCIDLib::TEncodedTime   enctWait
            ,       TIPEndPoint&            ipepClient = TIPEndPoint::Nul_TIPEndPoint()
        );

        tCIDLib::TVoid Initialize
        (
            const   tCIDSock::ESockProtos   eProtocol
        );


        tCIDLib::TVoid Initialize
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDLib::TIPPortNum     ippnListenOn
        );


    private :
        // -------------------------------------------------------------------
        //  Private class constants
        //
        //  c4ListenCnt
        //      The max number of listening sockets we create (one for all IPV4
        //      and one for all IPV6 worst case, if both are available.)
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4 c4ListenCnt = 2;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_apksockList
        //      We just use kernel level sockets here and set them up in the
        //      array of pointers that is required by the multi-select kernel
        //      methods This way we can be very efficient. There will never be
        //      more than 2 (an ANY address on IPV4 and/or IPV6.)
        //
        //  m_c4Count
        //      The number of sockets we loaded in m_apkSockList. If we have
        //      not been initialized successfully yet, this will be zero.
        //
        //  m_c4MaxWaiting
        //      The maximum number of waiting clients to allow.
        //
        //  m_ippnListenOn
        //      The port number we were asked to listen on. It's stored in the ctor for
        //      later use in Initialize(), and also for return via getter for reporting
        //      purposes. If it is zero, then it is updated during init to the actual
        //      port that got used.
        // -------------------------------------------------------------------
        TKrnlSocket*            m_apksockList[c4ListenCnt];
        tCIDLib::TCard4         m_c4Count;
        tCIDLib::TCard4         m_c4MaxWaiting;
        tCIDLib::TIPPortNum     m_ippnListenOn;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSocketListener,TObject)
};

#pragma CIDLIB_POPPACK

