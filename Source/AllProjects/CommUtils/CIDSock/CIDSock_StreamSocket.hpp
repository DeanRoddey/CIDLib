//
// FILE NAME: CIDSock_StreamSocket.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/09/1998
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
//  This is the header for the CIDSock_StreamSocket.cpp file. This file implements
//  the TStreamSocket class, which is derived from the base TSocket class. It adds
//  the necessary functionality to support stream style sockets.
//
//  Also implemented here are TClientStreamSocket and TServerStreamSocket, which
//  provide the final bits required to set up a stream socket for client and server
//  side operations.
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
//   CLASS: TStreamSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------
class CIDSOCKEXP TStreamSocket : public TSocket
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TStreamSocket(const TStreamSocket&) = delete;

        ~TStreamSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStreamSocket& operator=(const TStreamSocket&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsConnected() const;

        tCIDLib::TBoolean bIsShutdown() const;

        tCIDLib::TBoolean bNagleOn() const;

        tCIDLib::TBoolean bNagleOn
        (
            const   tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TCard4 c4ReceiveRaw
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4MaxToRecv
        );

        tCIDLib::TCard4 c4ReceiveMBuf
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4MaxToRecv = 0
        );

        tCIDLib::TCard4 c4ReceiveRawTO
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TEncodedTime   enctWait
            , const tCIDLib::TCard4         c4MaxToRecv
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReceiveMBufTO
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TEncodedTime   enctWait
            , const tCIDLib::TCard4         c4MaxToRecv = 0
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReceiveRawTOMS
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4Millis
            , const tCIDLib::TCard4         c4MaxToRecv
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReceiveMBufTOMS
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4Millis
            , const tCIDLib::TCard4         c4MaxToRecv = 0
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4Shutdown
        (
                    TMemBuf&                mbufToFill = TMemBuf::Nul_TMemBuf()
        );

        tCIDLib::TVoid Connect
        (
            const   TIPEndPoint&            ipepConnectTo
            , const tCIDLib::TEncodedTime   enctWait = (kCIDLib::enctOneSecond * 5)
        );

        TIPEndPoint ipepRemoteEndPoint() const;

        tCIDLib::TVoid Open
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );

        tCIDLib::TVoid Send
        (
            const   tCIDLib::TVoid* const   pToSend
            , const tCIDLib::TCard4         c4BytesToSend
        );

        tCIDLib::TVoid Send
        (
            const   TMemBuf&                mbufToSend
            , const tCIDLib::TCard4         c4BytesToSend = 0
        );

        tCIDLib::TVoid SendThrottled
        (
            const   tCIDLib::TVoid* const   pToSend
            , const tCIDLib::TEncodedTime   enctWait
            , const tCIDLib::TCard4         c4BytesToSend
        );

        tCIDLib::TVoid SendThrottled
        (
            const   TMemBuf&                mbufToSend
            , const tCIDLib::TEncodedTime   enctWait
            , const tCIDLib::TCard4         c4BytesToSend = 0
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TStreamSocket();

        TStreamSocket
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );

        TStreamSocket
        (
            const   TSocketHandle&          hsockToAdopt
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SocketClosed() final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckConnected
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid DumpConnection();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIsConnected
        //      This flag is set when this socket is connected successfully and
        //      cleared when it is shutdown or closed.
        //
        //  m_bIsShutdown
        //      This flag is true on a default construction, and false on
        //      any ctor that creates the socket. It is set by a call to
        //      c4Shutdown().
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bIsConnected;
        tCIDLib::TBoolean   m_bIsShutdown;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStreamSocket,TSocket)
};



// ---------------------------------------------------------------------------
//   CLASS: TClientStreamSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------
class CIDSOCKEXP TClientStreamSocket : public TStreamSocket
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TClientStreamSocket()  =delete;

        TClientStreamSocket
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );

        TClientStreamSocket
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const TIPEndPoint&            ipepTarget
        );

        TClientStreamSocket(const TClientStreamSocket&) = delete;

        ~TClientStreamSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TClientStreamSocket& operator=(const TClientStreamSocket&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TClientStreamSocket,TStreamSocket)
};


// ---------------------------------------------------------------------------
//   CLASS: TServerStreamSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------
class CIDSOCKEXP TServerStreamSocket : public TStreamSocket
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TServerStreamSocket(const TServerStreamSocket&) = delete;

        ~TServerStreamSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TServerStreamSocket& operator=(const TServerStreamSocket&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Friend classes
        // -------------------------------------------------------------------
        friend class TSocketListener;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TServerStreamSocket();

        TServerStreamSocket
        (
            const   TSocketHandle&          hsockToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TServerStreamSocket,TStreamSocket)
};


// ---------------------------------------------------------------------------
//   CLASS: TStreamSocketJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDSOCKEXP TStreamSocketJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStreamSocketJan() = delete;

        TStreamSocketJan
        (
                    TStreamSocket* const    psockToSanitize
        );

        TStreamSocketJan(const TStreamSocketJan&) = delete;

        ~TStreamSocketJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStreamSocketJan& operator=(const TStreamSocketJan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TStreamSocket* psockData();

        TStreamSocket* psockOrphan();

        tCIDLib::TVoid Orphan();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pockToSanitize
        //      The socket to clean up. It may be zero by the time we dtor, in which
        //      case we do nothing.
        // -------------------------------------------------------------------
        TStreamSocket*  m_psockToSanitize;
};


#pragma CIDLIB_POPPACK



