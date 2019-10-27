//
// FILE NAME: CIDKernel_Socket_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/30/1998
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
//  This file provides the Linux specific implementation of the TKrnlSocket
//  class, which provides the  host abstraction for socket services.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    <netinet/tcp.h>


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
namespace CIDKernel_Socket_Linux
{
    // -----------------------------------------------------------------------
    //  Socket constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4    hInvalid = tCIDLib::TCard4(-1);

    tCIDLib::TBoolean
    bLookupProtocolNumber(tCIDSock::ESockProtos eCIDProto, tCIDLib::TSInt& iLinuxProto)
    {
        const tCIDLib::TSCh* pszProtoName;
        switch (eCIDProto)
        {
        case tCIDSock::ESockProtos::IP:
            pszProtoName = "ip";
            break;
        case tCIDSock::ESockProtos::ICMP:
            pszProtoName = "icmp";
            break;
        case tCIDSock::ESockProtos::ICMP6:
            pszProtoName = "icmp6";
            break;
        case tCIDSock::ESockProtos::IGMP:
            pszProtoName = "igmp";
            break;
        case tCIDSock::ESockProtos::TCP:
            pszProtoName = "tcp";
            break;
        case tCIDSock::ESockProtos::PUP:
            pszProtoName = "pup";
            break;
        case tCIDSock::ESockProtos::UDP:
            pszProtoName = "udp";
            break;
        case tCIDSock::ESockProtos::IDP:
            pszProtoName = "idp";
            break;
        case tCIDSock::ESockProtos::RawIP:
            pszProtoName = "raw";
            break;
        default:
            return kCIDLib::False;
        }

        struct protoent* ProtoEnt = ::getprotobyname(pszProtoName);

        if (!ProtoEnt)
            return kCIDLib::False;

        iLinuxProto = ProtoEnt->p_proto;

        return kCIDLib::True;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TSocketHandle
//  PREFIX: hsock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSocketHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TSocketHandle::TSocketHandle() :

    m_phsockiThis(nullptr)
{
    m_phsockiThis = new TSocketHandleImpl;
    m_phsockiThis->iDescr = -1;
}

TSocketHandle::TSocketHandle(const TSocketHandle& hsockToCopy) :

    m_phsockiThis(nullptr)
{
    m_phsockiThis = new TSocketHandleImpl;
    m_phsockiThis->iDescr = hsockToCopy.m_phsockiThis->iDescr;
}

TSocketHandle::~TSocketHandle()
{
    delete m_phsockiThis;
    m_phsockiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TSocketHandle& TSocketHandle::operator=(const TSocketHandle& hsockToAssign)
{
    if (this == &hsockToAssign)
        return *this;

    m_phsockiThis->iDescr = hsockToAssign.m_phsockiThis->iDescr;

    return *this;
}


tCIDLib::TBoolean
TSocketHandle::operator==(const TSocketHandle& hsockToCompare) const
{
    return (m_phsockiThis->iDescr == hsockToCompare.m_phsockiThis->iDescr);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TSocketHandle::bIsValid() const
{
    return (m_phsockiThis->iDescr != -1);
}

tCIDLib::TVoid TSocketHandle::Clear()
{
    m_phsockiThis->iDescr = -1;
}

tCIDLib::TVoid
TSocketHandle::FormatToStr(       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phsockiThis->iDescr)
        , pszToFill
        , c4MaxChars
    );
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlSocket
//  PREFIX: ksock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSocket: Public, static methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TKrnlSocket::bDoSSLPageOp(  const   tCIDNet::ESSLOps                eOp
                            , const tCIDLib::TCh* const             pszTarSrv
                            , const tCIDLib::TIPPortNum             ippnTar
                            , const tCIDLib::TCh* const             pszURL
                            , const tCIDLib::TCh* const             pszAgent
                            , const tCIDLib::TCh* const             pszAccept
                            ,       tCIDLib::TCard1*&               pc1OutBuf
                            ,       tCIDLib::TCard4&                c4ContLen
                            ,       TKrnlString&                    kstrContType
                            ,       tCIDLib::TCard4&                c4ResCode
                            , const tCIDLib::TCh* const             pszInHdrs
                            ,       TKrnlLList<TKrnlKVPair>* const  pkllstOutHdrs)
{
    // <TBD> Deal with this
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}

tCIDLib::TBoolean TKrnlSocket::
bMultiReadSel(          TKrnlSocket*            apsockList[kCIDSock::c4MaxSelect]
                ,       tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect]
                ,       tCIDLib::TCard4&        c4Count
                , const tCIDLib::TEncodedTime   enctWait)
{
    // <TBD> Deal with this
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;    
}

tCIDLib::TBoolean
TKrnlSocket::bMultiSel(         TKrnlSocket*            apsockList[kCIDSock::c4MaxSelect]
                        ,       tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect]
                        ,       tCIDLib::TCard4&        c4Count
                        , const tCIDLib::TEncodedTime   enctWait)
{
    // <TBD> Deal with this
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);    
    return kCIDLib::False;        
}



// ---------------------------------------------------------------------------
//  TKrnlSocket: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSocket::TKrnlSocket()
{
}

TKrnlSocket::TKrnlSocket(const TSocketHandle& hsockToAdopt) :

    m_hsockThis(hsockToAdopt)
{
}


TKrnlSocket::~TKrnlSocket()
{
    //
    //  This should only happen in emergencies, i.e. the higher levels
    //  usually close down cleanly. But, just in case, we just do a
    //  no questions asked close.
    //
    if (m_hsockThis.m_phsockiThis->iDescr != CIDKernel_Socket_Linux::hInvalid)
    {
        if (::close(m_hsockThis.m_phsockiThis->iDescr))
        {
            #if CID_DEBUG_ON
            TKrnlError::SetLastHostError(errno);

            kmodCIDKernel.KrnlErrorPopUp
            (
                TKrnlError::kerrLast()
                , CID_FILE
                , CID_LINE
            );
            #endif
        }
        m_hsockThis.m_phsockiThis->iDescr = CIDKernel_Socket_Linux::hInvalid;
    }
}


// ---------------------------------------------------------------------------
//  TKrnlSocket: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSocket::bAccept( TKrnlIPAddr&            kipaClient
                                        , tCIDLib::TIPPortNum&  ippnClient
                                        , TSocketHandle&        hsockToFill)
{
    sockaddr_in SockAddr = {0};
    socklen_t Len = sizeof(SockAddr);

    tCIDLib::TSInt iSock = ::accept(m_hsockThis.m_phsockiThis->iDescr,
                                    reinterpret_cast<sockaddr*>(&SockAddr),
                                    &Len);

    if (iSock == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (Len != sizeof(SockAddr))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidData);
        return kCIDLib::False;
    }

    // Keep the IP address in network order
    // <TBD> deal with this
    // ipaClient = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    ippnClient = tCIDLib::TIPPortNum(::ntohs(SockAddr.sin_port));

    // Fill in the passed kernel socket object with the accepted socket info
    hsockToFill.m_phsockiThis->iDescr = iSock;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bAssociateReadEvent(TKrnlEvent& kevToSignal)
{
    // <TBD> Does this have any meaning here? If not, we are in deep doo
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;    
}



// <TBD> Deal with the way binding changed since this stuff was done
tCIDLib::TBoolean
TKrnlSocket::bBindListen(const  TKrnlIPAddr&        kipaListen
                        , const tCIDLib::TIPPortNum ippnLocal)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;        
}


tCIDLib::TBoolean
TKrnlSocket::bBindForRemote(const   TKrnlIPAddr&        kipaTarAddr
                            , const tCIDLib::TIPPortNum ippnLocal)
{
    /*
    sockaddr_in SockAddr = {0};
    SockAddr.sin_family = AF_INET;

    // IP addresses are assumed to be in network order
    SockAddr.sin_addr.s_addr = ipaBindTo;

    // Make sure the port number gets converted to network byte order
    SockAddr.sin_port = ::htons(ippnTarget);

    if (::bind(m_hsockThis.m_phsockiThis->iDescr,
               reinterpret_cast<sockaddr*>(&SockAddr),
               sizeof(SockAddr)))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
    */

    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;           

}



// Clears the read event flag
tCIDLib::TBoolean TKrnlSocket::bClearReadEvent(TKrnlEvent& kevToClear)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;               
}


tCIDLib::TBoolean TKrnlSocket::bClose()
{
    if (m_hsockThis.m_phsockiThis->iDescr != -1)
    {
        if (::close(m_hsockThis.m_phsockiThis->iDescr))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
        m_hsockThis.m_phsockiThis->iDescr = -1;
    }
    return kCIDLib::True;
}



// <TDB> Deal with the change to the kernel addr object and wait time
tCIDLib::TBoolean
TKrnlSocket::bConnect(  const   TKrnlIPAddr&            kipaConnectTo
                        , const tCIDLib::TIPPortNum     ippnTarget
                        , const tCIDLib::TEncodedTime   enctWait)
{
    /*
    // Create a socket address for the target
    sockaddr_in SockAddr = {0};
    SockAddr.sin_family = AF_INET;

    // IP addresses are assumed to be in network byte order
    SockAddr.sin_addr.s_addr = ipaConnectTo;

    // Get the port number into network byte order
    SockAddr.sin_port = ::htons(ippnTarget);

    if (::connect(m_hsockThis.m_phsockiThis->iDescr,
                  reinterpret_cast<sockaddr*>(&SockAddr),
                  sizeof(SockAddr)))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    return kCIDLib::True;
    */

   return kCIDLib::False;
}


// Deal with IPV6 support
tCIDLib::TBoolean
TKrnlSocket::bCreate(   const   tCIDSock::ESocketTypes  eType
                        , const tCIDSock::ESockProtos   eProtocol
                        , const tCIDSock::EAddrTypes    eAddrType)
{
    // Make sure its not already created
    if (m_hsockThis.m_phsockiThis->iDescr != -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    // Convert the passed socket type to the internal value
    const tCIDLib::TSInt iType = (eType == tCIDSock::ESocketTypes::Stream) ?
                                 SOCK_STREAM : SOCK_DGRAM;

    tCIDLib::TSInt iProtocol;
    if (!CIDKernel_Socket_Linux::bLookupProtocolNumber(eProtocol, iProtocol))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_ProtoType);
        return kCIDLib::False;
    }

    // And try to create the socket
    const tCIDLib::TSInt iTmp = ::socket(AF_INET, iType, iProtocol);

    if (iTmp == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    // It worked, so store the socket id
    m_hsockThis.m_phsockiThis->iDescr = iTmp;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bDataReady(tCIDLib::TCard4& c4ToFill) const
{
    // Query the amount of data readable via the next read operation
    tCIDLib::TSInt iNumBytes = 0;

    if (::ioctl(m_hsockThis.m_phsockiThis->iDescr, FIONREAD, &iNumBytes))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4ToFill = tCIDLib::TCard4(iNumBytes);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bEnumEvents(TKrnlEvent& kevReset, tCIDSock::ESockEvs& eToFill)
{
    // <TBD> Does this work on Linux?
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;                   
}



tCIDLib::TBoolean
TKrnlSocket::bGetSockOpt(const  TKrnlSocket::EBSockOpts eOpt
                        ,       tCIDLib::TBoolean&      bStatus) const
{
        // Translate the option into a level and option flag
    tCIDLib::TSInt iLevel = 0;
    tCIDLib::TSInt iOpt = 0;

    // Some of these we flip the meaning of
    tCIDLib::TBoolean bNegate = kCIDLib::False;
    switch(eOpt)
    {
        case EBSockOpts::AllowBroadcast :
            iLevel = SOL_SOCKET;
            iOpt = SO_BROADCAST;
            break;

        case EBSockOpts::DontRoute :
            iLevel = SOL_SOCKET;
            iOpt = SO_DONTROUTE;
            break;

        case EBSockOpts::KeepAlive :
            iLevel = SOL_SOCKET;
            iOpt = SO_KEEPALIVE;
            break;

        case EBSockOpts::Nagle :
            iLevel = IPPROTO_TCP;
            iOpt = TCP_NODELAY;
            bNegate = kCIDLib::True;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    tCIDLib::TSInt iVal;
    socklen_t slSize = sizeof(iVal);
    const tCIDLib::TSInt iRes = ::getsockopt
    (
        m_hsockThis.m_phsockiThis->iDescr
        , iLevel
        , iOpt
        , &iVal
        , &slSize
    );

    if (iRes < 0) 
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (slSize != sizeof(iVal))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_GetOptSize);
        return kCIDLib::False;        
    }

    // Get the result back into the caller's parm. Negate if indicated
    bStatus = iVal ? kCIDLib::True : kCIDLib::False;
    if (bNegate)
        bStatus = !bStatus;    

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bGetSockOpt(const  TKrnlSocket::EISockOpts eOpt
                        ,       tCIDLib::TInt4&         i4Value) const
{
    // This one is a no-op for us
    if (eOpt == EISockOpts::MaxMsgSize)
        return kCIDLib::True;

    // Translate the option into a level and option flag
    tCIDLib::TSInt iLevel = 0;
    tCIDLib::TSInt iOpt = 0;

    switch(eOpt)
    {
        case EISockOpts::LastError :
            iLevel = SOL_SOCKET;
            iLevel = SO_ERROR;
            break;

        case EISockOpts::ReceiveBuf :
            iLevel = SOL_SOCKET;
            iOpt = SO_RCVBUF;
            break;

        case EISockOpts::SendBuf :
            iLevel = SOL_SOCKET;
            iOpt = SO_SNDBUF;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    int iFlag;
    socklen_t slVal;
    if (::getsockopt
    (
        m_hsockThis.m_phsockiThis->iDescr
        , iLevel
        , iOpt
        , reinterpret_cast<char*>(&iFlag)
        , &slVal) < 0)
    {
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(errno), errno);
        return kCIDLib::False;
    }

    if (slVal != sizeof(int))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_GetOptSize);
        return kCIDLib::False;
    }

    i4Value = iFlag;
    return kCIDLib::True;
}



tCIDLib::TBoolean TKrnlSocket::bIsOpen(tCIDLib::TBoolean& bToFill) const
{
    bToFill = (m_hsockThis.m_phsockiThis->iDescr != -1);
    return kCIDLib::True;
}


// Add a multicast group address to this socket
tCIDLib::TBoolean
TKrnlSocket::bJoinMulticastGroup(const  TKrnlIPAddr& kipaGroup
                                , const TKrnlIPAddr& kipaIntf)
{
    // <TBD> Figure this out
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;                   
}


tCIDLib::TBoolean TKrnlSocket::bLinger(tCIDLib::TBoolean& bToFill) const
{
    struct linger LingerInfo;
    socklen_t LingerLen = sizeof(LingerInfo);

    if (::getsockopt(m_hsockThis.m_phsockiThis->iDescr,
                     SOL_SOCKET,
                     SO_LINGER,
                     &LingerInfo,
                     &LingerLen))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (LingerLen != sizeof(LingerInfo))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_GetOptSize);
        return kCIDLib::False;
    }

    bToFill = LingerInfo.l_onoff ? kCIDLib::True : kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bLinger(   const   tCIDLib::TBoolean   bNewState
                        , const tCIDLib::TCard4     c4Time)
{
    struct linger LingerInfo;
    LingerInfo.l_onoff = bNewState ? 1 : 0;
    LingerInfo.l_linger = tCIDLib::TSInt(c4Time);

    if (::setsockopt(m_hsockThis.m_phsockiThis->iDescr,
                     SOL_SOCKET,
                     SO_LINGER,
                     &LingerInfo,
                     sizeof(LingerInfo)))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bListen(const tCIDLib::TCard4 c4Backlog)
{
    const tCIDLib::TSInt iActualBL = c4Backlog ? c4Backlog : SOMAXCONN;

    if (::listen(m_hsockThis.m_phsockiThis->iDescr, iActualBL))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlSocket::bLocalEndPoint(tCIDLib::TIPPortNum&    ippnToFill
                            , TKrnlIPAddr&          kipaToFill) const
{
    sockaddr_in SockAddr = {0};
    socklen_t Len = sizeof(SockAddr);

    if (::getsockname(m_hsockThis.m_phsockiThis->iDescr,
                      reinterpret_cast<sockaddr*>(&SockAddr),
                      &Len))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (Len != sizeof(SockAddr))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidData);
        return kCIDLib::False;
    }

    // Get the port number into local byte order
    ippnToFill = tCIDLib::TIPPortNum(::ntohs(SockAddr.sin_port));

    // And return the bound to remote address
    // <TBD> Figure this out, the change to the address object
    // ipaToFill = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bQuerySelStatus(       tCIDLib::TBoolean&      bIsReadable
                            ,       tCIDLib::TBoolean&      bIsWriteable
                            ,       tCIDLib::TBoolean&      bHasExcepts
                            , const tCIDLib::TEncodedTime   enctWait) const
{
    // Set up the select structures for our socket
    fd_set FDRead, FDWrite, FDExcept;

    FD_ZERO(&FDRead);
    FD_SET(m_hsockThis.m_phsockiThis->iDescr, &FDRead);
    FD_ZERO(&FDWrite);
    FD_SET(m_hsockThis.m_phsockiThis->iDescr, &FDWrite);
    FD_ZERO(&FDExcept);
    FD_SET(m_hsockThis.m_phsockiThis->iDescr, &FDExcept);

    //
    //  Set up the timeout. We allow the caller to pass in c4MaxWait to
    //  have a totally blocking select (which means a nul time structure
    //  passed to select.) Otherwise, we have to set up the structure.
    //
    timeval* pTime = 0;
    timeval ActualTime;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        pTime = &ActualTime;
        ActualTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        ActualTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (ActualTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(1, &FDRead, &FDWrite, &FDExcept, pTime);

    if (iCount == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    bIsReadable = FD_ISSET(m_hsockThis.m_phsockiThis->iDescr, &FDRead) ? kCIDLib::True : kCIDLib::False;
    bIsWriteable = FD_ISSET(m_hsockThis.m_phsockiThis->iDescr, &FDWrite) ? kCIDLib::True : kCIDLib::False;
    bHasExcepts = FD_ISSET(m_hsockThis.m_phsockiThis->iDescr, &FDExcept) ? kCIDLib::True : kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bReceive(          tCIDLib::TVoid* const   pToFill
                        , const tCIDLib::TCard4         c4BytesToRecv
                        ,       tCIDLib::TCard4&        c4Received
                        , const tCIDLib::TBoolean       bPeek)
{
    // Set up the flags
    const tCIDLib::TCard4 c4Flags = bPeek ? MSG_PEEK : 0;

    const tCIDLib::TSInt iRet = ::recv(m_hsockThis.m_phsockiThis->iDescr,
                                       pToFill,
                                       tCIDLib::TSInt(c4BytesToRecv),
                                       c4Flags);

    if (iRet == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4Received = static_cast<tCIDLib::TCard4>(iRet);

    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlSocket::bReceiveFrom(          tCIDLib::TVoid* const   pToFill
                            , const tCIDLib::TCard4         c4BytesToRecv
                            ,       tCIDLib::TCard4&        c4Received
                            ,       TKrnlIPAddr&            kipaFrom
                            ,       tCIDLib::TIPPortNum&    ippnFrom
                            , const tCIDLib::TBoolean       bPeek)
{
    // Create a socket address for the target
    sockaddr_in SockAddr = {0};
    socklen_t Size = sizeof(SockAddr);

    // Set up the flags
    const tCIDLib::TCard4 c4Flags = bPeek ? MSG_PEEK : 0;

    tCIDLib::TSInt iRet = ::recvfrom(m_hsockThis.m_phsockiThis->iDescr,
                                     pToFill,
                                     tCIDLib::TSInt(c4BytesToRecv),
                                     c4Flags,
                                     reinterpret_cast<sockaddr*>(&SockAddr),
                                     &Size);

    if (iRet == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (Size != sizeof(SockAddr))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidData);
        return kCIDLib::False;
    }

    // Swap the port number to local order for internal use
    ippnFrom = tCIDLib::TIPPortNum(::ntohs(SockAddr.sin_port));

    // Keep the IP address in network order
    // <TBD> Deal with change to address object
    // ipaFrom = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    c4Received = static_cast<tCIDLib::TCard4>(iRet);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bRemoteEndPoint(   tCIDLib::TIPPortNum&    ippnToFill
                                , TKrnlIPAddr&          kipaToFill) const
{
    sockaddr_in SockAddr = {0};
    socklen_t Len = sizeof(SockAddr);

    if (::getpeername(m_hsockThis.m_phsockiThis->iDescr,
                      reinterpret_cast<sockaddr*>(&SockAddr),
                      &Len))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (Len != sizeof(SockAddr))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidData);
        return kCIDLib::False;
    }

    // Get the port into local byte order
    ippnToFill = tCIDLib::TIPPortNum(::ntohs(SockAddr.sin_port));

    // And return the bound to remote address
    // <TBD> Deal with change to address object
    // ipaToFill = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bSend(const   tCIDLib::TVoid* const  pToSend
                                    , const tCIDLib::TCard4         c4BytesToSend)
{
    tCIDLib::TSInt iRet = ::send
    (
        m_hsockThis.m_phsockiThis->iDescr
        , pToSend
        , c4BytesToSend
        , 0
    );

    if (iRet == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }


    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlSocket::bSendTo(   const   tCIDLib::TVoid* const   pToSend
                        , const tCIDLib::TCard4         c4BytesToSend
                        ,       tCIDLib::TCard4&        c4Sent
                        , const TKrnlIPAddr&            kipaTarget
                        , const tCIDLib::TIPPortNum     ippnTarget)
{
    // Create a socket address for the target
    sockaddr_in SockAddr = {0};
    SockAddr.sin_family = AF_INET;

    // IP addresses are assumed to be in network byte order
    // <TBD> Deal with change to address object
    // SockAddr.sin_addr.s_addr = ipaTarget;

    // Get the port number into network byte order
    SockAddr.sin_port = ::htons(ippnTarget);

    tCIDLib::TSInt iRet = ::sendto(m_hsockThis.m_phsockiThis->iDescr,
                                   pToSend,
                                   tCIDLib::TSInt(c4BytesToSend),
                                   0,
                                   reinterpret_cast<sockaddr*>(&SockAddr),
                                   sizeof(SockAddr));

    if (iRet == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4Sent = tCIDLib::TCard4(iRet);

    return kCIDLib::True;
}


// Set socket options
tCIDLib::TBoolean
TKrnlSocket::bSetSockOpt(       TKrnlSocket::EBSockOpts eOpt
                        , const tCIDLib::TBoolean       bNewValue)
{
    // Translate the option into a level and option flag
    tCIDLib::TCard4 c4Level;
    tCIDLib::TCard4 c4Opt;

    tCIDLib::TSInt iValue = bNewValue ? 1 : 0;    

    switch(eOpt)
    {
        case EBSockOpts::AllowBroadcast :
            c4Level = SOL_SOCKET;
            c4Opt = SO_BROADCAST;
            break;

        case EBSockOpts::DontRoute :
            c4Level = SOL_SOCKET;
            c4Opt = SO_DONTROUTE;
            break;

        case EBSockOpts::KeepAlive :
            c4Level = SOL_SOCKET;
            c4Opt = SO_KEEPALIVE;
            break;

        case EBSockOpts::Nagle :
            c4Level = IPPROTO_TCP;
            c4Opt = TCP_NODELAY;

            // This one we negate
            iValue = bNewValue ? 0 : 1;    
            break;

        case EBSockOpts::ReuseAddr :
            c4Level = SOL_SOCKET;
            c4Opt = SO_REUSEADDR;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    if (::setsockopt
    (
        m_hsockThis.m_phsockiThis->iDescr
        , c4Level
        , c4Opt
        , reinterpret_cast<const char*>(&iValue)
        , sizeof(iValue)) < 0)
    {
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(errno), errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bSetSockOpt(       TKrnlSocket::EISockOpts eOpt
                        , const tCIDLib::TInt4          i4NewValue)
{
    // Translate the option into a level and option flag
    tCIDLib::TCard4 c4Level;
    tCIDLib::TCard4 c4Opt;
    tCIDLib::TSInt iFlag = i4NewValue;

    switch(eOpt)
    {
        case EISockOpts::ReceiveBuf :
            c4Level = SOL_SOCKET;
            c4Opt = SO_RCVBUF;
            break;

        case EISockOpts::SendBuf :
            c4Level = SOL_SOCKET;
            c4Opt = SO_SNDBUF;
            break;

        case EISockOpts::TTL :
            c4Level = IPPROTO_IP;
            c4Opt = IP_TTL;
            break;

        case EISockOpts::TTLV6 :
            c4Level = IPPROTO_IPV6;
            c4Opt = IPV6_UNICAST_HOPS;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    if (::setsockopt
    (
        m_hsockThis.m_phsockiThis->iDescr
        , c4Level
        , c4Opt
        , reinterpret_cast<const char*>(&iFlag)
        , sizeof(int)) < 0)
    {
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(errno), errno);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



tCIDLib::TBoolean TKrnlSocket::bShutdown(const tCIDSock::ESockShutdown eMode)
{
    // Create the 'how' flag
    tCIDLib::TSInt iHow;
    if (eMode == tCIDSock::ESockShutdown::Both)
        iHow = 2;
    else if (eMode == tCIDSock::ESockShutdown::Send)
        iHow = 1;
    else
        iHow = 0;

    if (::shutdown(m_hsockThis.m_phsockiThis->iDescr, iHow))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bWaitForConnectReady(          tCIDLib::TBoolean&      bGotConnect
                                    , const tCIDLib::TEncodedTime   enctWait)
{
    // Set up the select structures for our socket
    fd_set FDConn;
    FD_ZERO(&FDConn);
    FD_SET(m_hsockThis.m_phsockiThis->iDescr, &FDConn);

    timeval WTime;
    timeval* pWTime = 0;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDConn, 0, 0, pWTime);
    if (iCount == -1)
    {
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(errno), errno);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    if (iCount == 1)
        bGotConnect = FD_ISSET(m_hsockThis.m_phsockiThis->iDescr, &FDConn) != 0;
    else
        bGotConnect = kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bWaitForDataReady(         tCIDLib::TBoolean&      bGotData
                                , const tCIDLib::TEncodedTime   enctWait) const
{
    // Set up the select structures for our socket
    fd_set FDRead;
    FD_ZERO(&FDRead);
    FD_SET(m_hsockThis.m_phsockiThis->iDescr, &FDRead);

    timeval WTime;
    timeval* pWTime = 0;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDRead, 0, 0, pWTime);
    if (iCount == -1)
    {
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(errno), errno);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    if (iCount == 1)
        bGotData = FD_ISSET(m_hsockThis.m_phsockiThis->iDescr, &FDRead) != 0;
    else
        bGotData = kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bWaitForSendReady(         tCIDLib::TBoolean&      bReady
                                , const tCIDLib::TEncodedTime   enctWait) const
{
    // Assume not ready until proven otherwise
    bReady = kCIDLib::False;

    //
    //  Set up the select structures for our socket. In order to wake
    //  up immediately if the socket get's dropped on the other side while
    //  we wait, we set up both a read and a write. The read gets the
    //  socket going down, and the write gets the ready to send.
    //
    fd_set FDRead;
    FD_ZERO(&FDRead);
    FD_SET(m_hsockThis.m_phsockiThis->iDescr, &FDRead);

    fd_set FDWrite;
    FD_ZERO(&FDWrite);
    FD_SET(m_hsockThis.m_phsockiThis->iDescr, &FDWrite);    

    timeval WTime;
    timeval* pWTime = 0;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDRead, &FDWrite, 0, pWTime);
    if (iCount == -1)
    {
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(errno), errno);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    if (iCount == 1)
    {
        //
        //  If we get the read indicator, then the socket has been dropped.
        //  So, what we do is lie and say it's ready. That'll make the caller
        //  try to send, which will get the appropriate error.
        //
        if (FD_ISSET(m_hsockThis.m_phsockiThis->iDescr, &FDRead))
        {
            bReady = kCIDLib::True;
            return kCIDLib::True;
        }

        bReady = FD_ISSET(m_hsockThis.m_phsockiThis->iDescr, &FDWrite) != 0;
    }
     
    return kCIDLib::True;
}

