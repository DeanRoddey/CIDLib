//
// FILE NAME: CIDSock_DatagramSocket.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/09/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDSock_DGramSocket.cpp file. This file
//  implements the TDatagramSocket class, which is derived from the base
//  TSocket class. It adds the necessary functionality to support databram
//  style sockets.
//
//  Also implemented here are TClientDatagramSocket and TServerDatagramSocket,
//  which provide the final bits required to set up a datagram socket for
//  client and server side operations.
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
//   CLASS: TDatagramSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------
class CIDSOCKEXP TDatagramSocket : public TSocket
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TDatagramSocket(const TDatagramSocket&) = delete;

        ~TDatagramSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDatagramSocket& operator=(const TDatagramSocket&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAllowBroadcast() const;

        tCIDLib::TBoolean bAllowBroadcast
        (
            const   tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TCard4 c4MaxMsgSize() const;

        tCIDLib::TCard4 c4ReceiveRawFrom
        (
                    TIPEndPoint&            ipepFrom
            ,       tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TEncodedTime   enctWait
            , const tCIDLib::TCard4         c4BytesToRecv
            , const tCIDLib::TBoolean       bPeek = kCIDLib::False
        );

        tCIDLib::TCard4 c4ReceiveMBufFrom
        (
                    TIPEndPoint&            ipepFrom
            ,       TMemBuf&                mbufToFill
            , const tCIDLib::TEncodedTime   enctWait
            , const tCIDLib::TCard4         c4BytesToRecv = 0
            , const tCIDLib::TBoolean       bPeek = kCIDLib::False
        );

        tCIDLib::TCard4 c4SendTo
        (
            const   TIPEndPoint&            ipepTo
            , const tCIDLib::TVoid* const   pToSend
            , const tCIDLib::TCard4         c4BytesToSend
        );

        tCIDLib::TCard4 c4SendTo
        (
            const   TIPEndPoint&            ipepTo
            , const TMemBuf&                mbufToSend
            , const tCIDLib::TCard4         c4BytesToSend = 0
        );

        tCIDLib::TVoid JoinMulticastGroup
        (
            const   TIPAddress&             ipaGroup
            , const TIPAddress&             ipaIntf
        );

        tCIDLib::TVoid Open
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TDatagramSocket();


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SocketClosed() override;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDatagramSocket,TSocket)
};



// ---------------------------------------------------------------------------
//   CLASS: TClientDatagramSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------
class CIDSOCKEXP TClientDatagramSocket : public TDatagramSocket
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TClientDatagramSocket();

        TClientDatagramSocket
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );

        TClientDatagramSocket(const TClientDatagramSocket&) = delete;

        ~TClientDatagramSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TClientDatagramSocket& operator=(const TClientDatagramSocket&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SendWOLBroadcast
        (
            const   TString&                strMACAddr
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TClientDatagramSocket,TDatagramSocket)
};


// ---------------------------------------------------------------------------
//   CLASS: TServerDatagramSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------
class CIDSOCKEXP TServerDatagramSocket : public TDatagramSocket
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TServerDatagramSocket();

        TServerDatagramSocket
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const TIPEndPoint&            ipepLocal
        );

        TServerDatagramSocket(const TServerDatagramSocket&) = delete;

        ~TServerDatagramSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TServerDatagramSocket& operator=(const TServerDatagramSocket&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ipepClient
        //      This is filled in with the end point of the client that we
        //      get connected to.
        // -------------------------------------------------------------------
        TIPEndPoint     m_ipepClient;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TServerDatagramSocket,TDatagramSocket)
};

#pragma CIDLIB_POPPACK


