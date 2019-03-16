//
// FILE NAME: CIDSock_Socket.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/09/1998
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
//  This is the header for the CIDSock_Socket.cpp file. This file implements
//  the TSocket class. This is an abstract base class that provides the
//  socket functionality common to both stream and datagram sockets.
//
//  We also define a base class for supporting multi-socket select operations.
//  Those operations are provided as static methods of the TSocket class. The
//  select item class takes a base TSocket pointer, so it can work in terms of
//  datagram or stream sockets, but the caller has to know which and cast them
//  back to their actual types when accessing them.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TSocket;


// ---------------------------------------------------------------------------
//   CLASS: TMSockSelItem
//  PREFIX: mssi
// ---------------------------------------------------------------------------
class CIDSOCKEXP TMSockSelItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TMSockSelItem
        (
                    TSocket* const          psockSel
        );

        TMSockSelItem(const TMSockSelItem&) = delete;

        ~TMSockSelItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMSockSelItem& operator=(const TMSockSelItem&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime enctLastMsg() const;

        tCIDLib::TEncodedTime enctLastMsg
        (
            const   tCIDLib::TEncodedTime   enctToSet
        );

        tCIDSock::EMSelFlags eFlags() const;

        tCIDSock::EMSelFlags eFlags
        (
            const   tCIDSock::EMSelFlags    eToSet
        );

        TSocket* psockSel();


    private :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TSocket;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_enctLastMsg
        //      If a client closes non-gracefully, we will never know it.
        //      our check for data available won't return an error, and we
        //      never normally send anything to the client other than an
        //      accept/reject msg on initial connection.
        //
        //      So, we keep up with the last time we got data on this
        //      connection. The caller can use this to ping the other side
        //      after some period of inactivity to make sure he's still
        //      alive or to just drop the connection. The multiselect
        //      methods of TStreamSocket below support setting the MaxIdle
        //      flag during a select if the last message is longer ago than
        //      a certain idle time.
        //
        //  m_eFlags
        //      The flags that indicate what statuses this particular socket
        //      was found to have.
        //
        //  m_psockSel
        //      The socket associated with this item. We do not adopt it, we
        //      just reference it.
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime   m_enctLastMsg;
        tCIDSock::EMSelFlags    m_eFlags;
        TSocket*                m_psockSel;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMSockSelItem, TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------
class CIDSOCKEXP TSocket : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bMultiReadSel
        (
                    TRefVector<TMSockSelItem>&  colList
            , const tCIDLib::TEncodedTime       enctWait
            , const tCIDLib::TEncodedTime       enctMaxIdle = 0
        );

        static tCIDLib::TBoolean bMultiSel
        (
                    TRefVector<TMSockSelItem>&  colList
            , const tCIDLib::TEncodedTime       enctWait
            , const tCIDLib::TEncodedTime       enctMaxIdle = 0
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSocket(const TSocket&) = delete;

        ~TSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSocket& operator=(const TSocket&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AssociateReadEvent
        (
                    TEvent&                 evToSignal
        );

        tCIDLib::TBoolean bDataReady
        (
                    tCIDLib::TCard4&        c4Bytes
        )   const;

        tCIDLib::TBoolean bDontRoute() const;

        tCIDLib::TBoolean bDontRoute
        (
            const   tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TBoolean bIsBound() const;

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TBoolean bLinger() const;

        tCIDLib::TBoolean bLinger
        (
            const   tCIDLib::TBoolean       bNewState
            , const tCIDLib::TCard4         c4Seconds
        );

        tCIDLib::TBoolean bReuseAddr() const;

        tCIDLib::TBoolean bReuseAddr
        (
            const   tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TBoolean bSendKeepAlive() const;

        tCIDLib::TBoolean bSendKeepAlive
        (
            const   tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TBoolean bUserFlag() const;

        tCIDLib::TBoolean bUserFlag
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bWaitForDataReady
        (
            const   tCIDLib::TEncodedTime   enctWait
        )   const;

        tCIDLib::TBoolean bWaitForDataReadyMS
        (
            const   tCIDLib::TCard4         c4WaitMillis = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TBoolean bWaitForSendReady
        (
            const   tCIDLib::TEncodedTime   enctWait
        )   const;

        tCIDLib::TBoolean bWaitForSendReadyMS
        (
            const   tCIDLib::TCard4         c4WaitMillis
        )   const;

        tCIDLib::TVoid BindListen
        (
            const   TIPEndPoint&            ipepLocal
        );

        tCIDLib::TVoid BindForRemote
        (
            const   TIPEndPoint&            ipepRem
            , const tCIDLib::TIPPortNum     ippnLocal
        );

        tCIDLib::TCard4 c4RecvBufferSize() const;

        tCIDLib::TCard4 c4RecvBufferSize
        (
            const   tCIDLib::TCard4         c4NewSize
        );

        tCIDLib::TCard4 c4SendBufferSize() const;

        tCIDLib::TCard4 c4SendBufferSize
        (
            const   tCIDLib::TCard4         c4NewSize
        );

        tCIDLib::TVoid ClearReadEvent
        (
                    TEvent&                 evToClear
        );

        tCIDLib::TVoid Close();

        tCIDSock::ESockEvs eEnumEvents
        (
                    TEvent&                 evReset
        );

        TIPEndPoint ipepLocalEndPoint() const;

        tCIDLib::TVoid QuerySelStatus
        (
                    tCIDLib::TBoolean&      bIsReadable
            ,       tCIDLib::TBoolean&      bIsWriteable
            ,       tCIDLib::TBoolean&      bHasExcepts
            , const tCIDLib::TEncodedTime   enctWait
        )   const;



    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TSocket();

        TSocket
        (
            const   tCIDSock::ESocketTypes  eType
            , const tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );

        TSocket
        (
            const   TSocketHandle&          hsockToAdopt
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid SocketClosed() = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsBound
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid Create
        (
            const   tCIDSock::ESocketTypes  eType
            , const tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );

        tCIDLib::EErrClasses eXlatKrnlErrClass
        (
            const   TKrnlError&             kerrToXlat
        )   const;

        tCIDLib::TErrCode errcXlatKrnlErr
        (
            const   TKrnlError&             kerrToXlat
            , const tCIDLib::TErrCode       errcDefault
        )   const;

        const TKrnlSocket& ksockImpl() const;

        TKrnlSocket& ksockImpl();



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIsBound
        //      This socket has been successfully bound already. It will be
        //      cleared during close.
        //
        //  m_bUserFlag
        //      A flag for use by the client code for its own purposes.
        //
        //  m_ksockImpl
        //      The kernel socket abstraction that provides the real
        //      implementation.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bIsBound;
        tCIDLib::TBoolean       m_bUserFlag;
        TKrnlSocket             m_ksockImpl;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSocket,TObject)
};

#pragma CIDLIB_POPPACK

