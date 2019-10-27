//
// FILE NAME: CIDKernel_Socket.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/07/1998
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
//  This is the header for the CIDKernel_Socket.Cpp file, which implements
//  the TKrnlSocket class. This class provides the abstraction for the
//  host socket services.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TKrnlSockPinger;

// ---------------------------------------------------------------------------
//   CLASS: TKrnlSocket
//  PREFIX: ksock
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlSocket
{
    public  :
        // -------------------------------------------------------------------
        //  The available socket options to set/get at the kernel level. At
        //  the CIDSock level individual methods are exposed do that only those
        //  options legal for a given type of socket are available. But at
        //  this level we assume the caller knows what they are doing, and
        //  can have just two methods for sock options.
        //
        //  We have one for each set of options that have a particular data
        //  representation, i.e. boolean flag, signed value, etc...
        // -------------------------------------------------------------------
        enum class EBSockOpts
        {
            AllowBroadcast
            , DontRoute
            , KeepAlive
            , Nagle
            , ReuseAddr
        };

        enum class EISockOpts
        {
            LastError
            , MaxMsgSize
            , ReceiveBuf
            , SendBuf
            , TTL
            , TTLV6
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        //
        //  These are all platform provided
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bDoSSLPageOp
        (
            const   tCIDNet::ESSLOps        eOp
            , const tCIDLib::TCh* const     pszTargetAddr
            , const tCIDLib::TIPPortNum     ippnTar
            , const tCIDLib::TCh* const     pszURL
            , const tCIDLib::TCh* const     pszAgent
            , const tCIDLib::TCh* const     pszAccept
            ,       tCIDLib::TCard1*&       pc1OutBuf
            ,       tCIDLib::TCard4&        c4ContLen
            ,       TKrnlString&            kstrContType
            ,       tCIDLib::TCard4&        c4ResCode
            , const tCIDLib::TCh* const     pszInHdrs
            ,       TKrnlLList<TKrnlKVPair>* const pkllstOutHdrs
        );

        static tCIDLib::TBoolean bMultiReadSel
        (
                    TKrnlSocket*            apsockList[kCIDSock::c4MaxSelect]
            ,       tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect]
            ,       tCIDLib::TCard4&        c4Count
            , const tCIDLib::TEncodedTime   enctWait
        );

        static tCIDLib::TBoolean bMultiSel
        (
                    TKrnlSocket*            apsockList[kCIDSock::c4MaxSelect]
            ,       tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect]
            ,       tCIDLib::TCard4&        c4Count
            , const tCIDLib::TEncodedTime   enctWait
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSocket();

        TKrnlSocket
        (
            const   TSocketHandle&          hsockToAdopt
        );

        TKrnlSocket(const TKrnlSocket&) = delete;

        ~TKrnlSocket();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlSocket& operator=(const TKrnlSocket&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAccept
        (
                    TKrnlIPAddr&            kipaClient
            ,       tCIDLib::TIPPortNum&    ippnClient
            ,       TSocketHandle&          hsockToFill
        );

        tCIDLib::TBoolean bAssociateReadEvent
        (
                    TKrnlEvent&             kevToSignal
        );

        tCIDLib::TBoolean bBindForRemote
        (
            const   TKrnlIPAddr&            kipaTarAddr
            , const tCIDLib::TIPPortNum     ippnLocal
        );

        tCIDLib::TBoolean bBindListen
        (
            const   TKrnlIPAddr&            kipaLocalAddr
            , const tCIDLib::TIPPortNum     ippnLocal
        );

        tCIDLib::TBoolean bClearReadEvent
        (
                    TKrnlEvent&             kevToSignal
        );

        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bConnect
        (
            const   TKrnlIPAddr&            kipaConnectTo
            , const tCIDLib::TIPPortNum     ippnTarget
            , const tCIDLib::TEncodedTime   enctWait
        );

        tCIDLib::TBoolean bCreate
        (
            const   tCIDSock::ESocketTypes  eType
            , const tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType = tCIDSock::EAddrTypes::Count
        );

        tCIDLib::TBoolean bDataReady
        (
                    tCIDLib::TCard4&        c4ToFill
        )   const;

        tCIDLib::TBoolean bEnumEvents
        (
                    TKrnlEvent&             kevReset
            ,       tCIDSock::ESockEvs&     eToFill
        );

        tCIDLib::TBoolean bGetSockOpt
        (
            const   EBSockOpts              eOpt
            ,       tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bGetSockOpt
        (
            const   EISockOpts              eOpt
            ,       tCIDLib::TInt4&         i4ToFill
        )   const;

        tCIDLib::TBoolean bIsOpen
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bJoinMulticastGroup
        (
            const   TKrnlIPAddr&            kipaGroup
            , const TKrnlIPAddr&            kipaIntf
        );

        tCIDLib::TBoolean bLinger
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bLinger
        (
            const   tCIDLib::TBoolean       bNewState
            , const tCIDLib::TCard4         c4Time
        );

        tCIDLib::TBoolean bListen
        (
            const   tCIDLib::TCard4         c4Backlog
        );

        tCIDLib::TBoolean bLocalEndPoint
        (
                    tCIDLib::TIPPortNum&    ippnToFill
            ,       TKrnlIPAddr&            kipaToFill
        )   const;

        tCIDLib::TBoolean bQuerySelStatus
        (
                    tCIDLib::TBoolean&      bIsReadable
            ,       tCIDLib::TBoolean&      bIsWriteable
            ,       tCIDLib::TBoolean&      bHasExcepts
            , const tCIDLib::TEncodedTime   enctWait
        )   const;

        tCIDLib::TBoolean bReceive
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4BytesToRecv
            ,       tCIDLib::TCard4&        c4Received
            , const tCIDLib::TBoolean       bPeek = kCIDLib::False
        );

        tCIDLib::TBoolean bReceiveFrom
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4BytesToRecv
            ,       tCIDLib::TCard4&        c4Received
            ,       TKrnlIPAddr&            kipaFrom
            ,       tCIDLib::TIPPortNum&    ippnFrom
            , const tCIDLib::TBoolean       bPeek = kCIDLib::False
        );

        tCIDLib::TBoolean bRemoteEndPoint
        (
                    tCIDLib::TIPPortNum&    ippnToFill
            ,       TKrnlIPAddr&            kipaToFill
        )   const;

        tCIDLib::TBoolean bSend
        (
            const   tCIDLib::TVoid* const   pToSend
            , const tCIDLib::TCard4         c4BytesToSend
        );

        tCIDLib::TBoolean bSetSockOpt
        (
            const   EBSockOpts              eOpt
            , const tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bSetSockOpt
        (
            const   EISockOpts              eOpt
            , const tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TBoolean bSendTo
        (
            const   tCIDLib::TVoid* const   pToSend
            , const tCIDLib::TCard4         c4BytesToSend
            ,       tCIDLib::TCard4&        c4Sent
            , const TKrnlIPAddr&            kipaTarget
            , const tCIDLib::TIPPortNum     ippnTarget
        );

        tCIDLib::TBoolean bShutdown
        (
            const   tCIDSock::ESockShutdown eMode
        );

        tCIDLib::TBoolean bWaitForConnectReady
        (
                    tCIDLib::TBoolean&      bGotConnect
            , const tCIDLib::TEncodedTime   enctWait
        );

        tCIDLib::TBoolean bWaitForDataReady
        (
                    tCIDLib::TBoolean&      bGotData
            , const tCIDLib::TEncodedTime   enctWait
        )   const;

        tCIDLib::TBoolean bWaitForSendReady
        (
                    tCIDLib::TBoolean&      bReady
            , const tCIDLib::TEncodedTime   enctWait
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hsockThis
        //      The handle to the socket that this object represents.
        // -------------------------------------------------------------------
        TSocketHandle   m_hsockThis;
};

#pragma CIDLIB_POPPACK


