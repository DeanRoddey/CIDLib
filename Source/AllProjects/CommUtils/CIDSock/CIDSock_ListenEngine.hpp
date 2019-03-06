//
// FILE NAME: CIDSock_ListenEngine.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/10/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDSock_ListenEngine.cpp file. This file implements
//  the TSockListenerEng class. This class provides a function that most server side
//  applications will require, so it's good to do it once, right, and let everyone
//  use it.
//
//  Essentially most servers have to listen on at least one, sometimes two, sockets
//  for connections from clients, and then to hand those sockets off to threads that
//  will service them. The reason for two sockets is that some servers support
//  secure vs. non-secure connections, or perhaps some other type of distinctions. It
//  can be whatever they want it to be, they are just told which one of the ports
//  a connection came from.
//
//  So we spin up one or two threads, each of which creates a listener object and
//  waits for client connections on them. When they get connections, they put an
//  object into a thread safe queue. Client threads will call our pslecWait() method
//  to wait for a sockect connection from a client. If one comes within their timeout
//  then it will return with new connection info.
//
//  Typically the server will create a thread farm and each one will call into this
//  wait method, which serves to throttle them and allows for efficient processing of
//  multiple clients in an overlapping way.
//
//  The only point of contention is the queue and it is thread safe, so we don't need
//  any other syncrhonization.
//
//  Each thread will listen on all available interfaces, with the exception that
//  loopsbacks can optionally be ignored.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  When we are destroyed, any connection objects left in the queue will just
//      be closed.
//
//  2)  The connection objects own the sockets, so that, should the object
//      be dropped, the socket will be cleaned up. If the caller wants to keep the
//      socket, it should orphan it out of the connection object, or just keep the
//      connection object around as a specialized socket janitor.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TSockListenerEng;


// ---------------------------------------------------------------------------
//   CLASS: TSockLEngConn
//  PREFIX: slec
// ---------------------------------------------------------------------------
class CIDSOCKEXP TSockLEngConn : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSockLEngConn() = delete;

        TSockLEngConn
        (
                    TServerStreamSocket* const psockAdopt
            , const tCIDLib::TBoolean       bSecure
            , const TIPEndPoint&            ipepClient
        );

        TSockLEngConn(const TSockLEngConn&) = delete;

        ~TSockLEngConn();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSockLEngConn& operator=(const TSockLEngConn&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSecure() const;

        tCIDLib::TEncodedTime enctAt() const;

        const TIPEndPoint& ipepClient() const;

        TServerStreamSocket* psockConn();

        tCIDLib::TVoid Orphan();


    protected :
        // -------------------------------------------------------------------
        //  Friends of our class
        // -------------------------------------------------------------------
        friend class TSockListenerEng;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Cleanup();


        // -------------------------------------------------------------------
        //  Private class data
        //
        //  m_bSecure
        //      Indicates whether this is a connection on the secure or non-secure
        //      port, which the client code needs to know.
        //
        //  m_enctAt
        //      The time at which this connection object was created.
        //
        //  m_ipepClient
        //      The IP end point of the client that connected.
        //
        //  m_psockConn
        //      A pointer to the socket. We own it unless the client code orphans
        //      it out.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bSecure;
        tCIDLib::TEncodedTime   m_enctAt;
        TIPEndPoint             m_ipepClient;
        TServerStreamSocket*    m_psockConn;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSockLEngConn,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TSockListenerEng
//  PREFIX: sle
// ---------------------------------------------------------------------------
class CIDSOCKEXP TSockListenerEng : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public data types
        // -------------------------------------------------------------------
        using TConnQueue = TRefQueue<TSockLEngConn>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSockListenerEng();

        TSockListenerEng(const TSockListenerEng&) = delete;

        ~TSockListenerEng();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSockListenerEng& operator=(const TSockListenerEng&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MaxWaiting() const;

        tCIDLib::TVoid Cleanup();

        tCIDLib::TIPPortNum ippnNonSecure() const;

        tCIDLib::TIPPortNum ippnSecure() const;

        tCIDLib::TVoid Initialize
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDLib::TIPPortNum     ippnNonSecure
            , const tCIDLib::TIPPortNum     ippnSecure = 0
            , const tCIDLib::TCard4         c4MaxWaiting = 0
        );

        TSockLEngConn* pslecWait
        (
            const   tCIDLib::TCard4         c4WaitMSs
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eListenThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxWaiting
        //  m_eProtocol
        //      The listener setup info we were given, which are in turn used by the
        //      threads to set up their respective listeners.
        //
        //  m_colConnQ
        //      The threads drop new sockets into this queue, and bWait() pulls them
        //      out for return.
        //
        //  m_ippnNonSecure
        //  m_ippnSecure
        //      The ports we listen on. If either is zero, then its thread is not
        //      started.
        //
        //  m_thrNonSecure
        //  m_thrSecure
        //      The threads we will spin up, though only for those ports that are
        //      enabled. Both are started up on eListenThread, since the code is
        //      the same either way.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxWaiting;
        TConnQueue              m_colConnQ;
        tCIDSock::ESockProtos   m_eProtocol;
        tCIDLib::TIPPortNum     m_ippnNonSecure;
        tCIDLib::TIPPortNum     m_ippnSecure;
        TThread                 m_thrNonSecure;
        TThread                 m_thrSecure;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSockListenerEng,TObject)
};

#pragma CIDLIB_POPPACK


