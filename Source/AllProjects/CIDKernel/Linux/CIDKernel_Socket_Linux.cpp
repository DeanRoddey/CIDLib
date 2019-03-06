//
// FILE NAME: CIDKernel_Socket_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/30/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
namespace
{
    tCIDLib::TBoolean
    __bGetSockOpt(  const   tCIDLib::TSInt      iSock
                    , const tCIDLib::TSInt      iOpt
                    ,       tCIDLib::TSInt&     iReturn)
    {
        tCIDLib::TSInt iOptVal = 0;
        socklen_t OptValLen = sizeof(iOptVal);

        if (::getsockopt(iSock, SOL_SOCKET, iOpt, &iOptVal, &OptValLen))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        if (OptValLen != sizeof(iOptVal))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_GetOptSize);
            return kCIDLib::False;
        }

        iReturn = iOptVal;

        return kCIDLib::True;
    }


    tCIDLib::TBoolean
    __bSetSockOpt(  const   tCIDLib::TSInt      iSock
                    , const tCIDLib::TSInt      iOpt
                    , const tCIDLib::TSInt      iNewValue)
    {
        tCIDLib::TSInt iVal = iNewValue;
        if (::setsockopt(iSock,
                         SOL_SOCKET,
                         iOpt,
                         reinterpret_cast<tCIDLib::TVoid*>(&iVal),
                         sizeof(iVal)))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        return kCIDLib::True;
    }

    tCIDLib::TBoolean
    __bLookupProtocolNumber(tCIDSock::ESockProtos eCIDProto, tCIDLib::TSInt& iLinuxProto)
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
    __phsockiThis(0)
{
    __phsockiThis = new TSocketHandleImpl;
    __phsockiThis->iDescr = -1;
}

TSocketHandle::TSocketHandle(const TSocketHandle& hsockToCopy) :
    __phsockiThis(0)
{
    __phsockiThis = new TSocketHandleImpl;
    __phsockiThis->iDescr = hsockToCopy.__phsockiThis->iDescr;
}

TSocketHandle::~TSocketHandle()
{
    delete __phsockiThis;
    __phsockiThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TSocketHandle& TSocketHandle::operator=(const TSocketHandle& hsockToAssign)
{
    if (this == &hsockToAssign)
        return *this;

    __phsockiThis->iDescr = hsockToAssign.__phsockiThis->iDescr;

    return *this;
}


tCIDLib::TBoolean
TSocketHandle::operator==(const TSocketHandle& hsockToCompare) const
{
    return (__phsockiThis->iDescr == hsockToCompare.__phsockiThis->iDescr);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TSocketHandle::bValid() const
{
    return (__phsockiThis->iDescr != -1);
}

tCIDLib::TVoid TSocketHandle::Clear()
{
    __phsockiThis->iDescr = -1;
}

tCIDLib::TVoid
TSocketHandle::FormatToStr(       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(__phsockiThis->iDescr)
        , pszToFill
        , c4MaxChars
    );
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlSocket
//  PREFIX: ksock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSocket: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSocket::TKrnlSocket()
{
}

TKrnlSocket::TKrnlSocket(const TSocketHandle& hsockToAdopt) :
    __hsockThis(hsockToAdopt)
{
}


TKrnlSocket::~TKrnlSocket()
{
    //
    //  This should only happen in emergencies, i.e. the higher levels
    //  usually close down cleanly. But, just in case, we just do a
    //  no questions asked close.
    //
    if (__hsockThis.__phsockiThis->iDescr != -1)
    {
        if (::close(__hsockThis.__phsockiThis->iDescr))
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
        __hsockThis.__phsockiThis->iDescr = -1;
    }
}


// ---------------------------------------------------------------------------
//  TKrnlSocket: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSocket::bAccept( tCIDLib::TIPAddr&       ipaClient
                                        , tCIDLib::TIPPortNum&  ippnClient
                                        , TSocketHandle&        hsockToFill)
{
    sockaddr_in SockAddr = {0};
    socklen_t Len = sizeof(SockAddr);

    tCIDLib::TSInt iSock = ::accept(__hsockThis.__phsockiThis->iDescr,
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
    ipaClient = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    ippnClient = tCIDLib::TIPPortNum(::ntohs(SockAddr.sin_port));

    // Fill in the passed kernel socket object with the accepted socket info
    hsockToFill.__phsockiThis->iDescr = iSock;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bAllowBroadcast(tCIDLib::TBoolean& bToFill) const
{
    tCIDLib::TSInt iAllowed;
    tCIDLib::TBoolean bRc = __bGetSockOpt(__hsockThis.__phsockiThis->iDescr,
                                          SO_BROADCAST,
                                          iAllowed);
    if (bRc)
    {
        bToFill = iAllowed ? kCIDLib::True : kCIDLib::False;
        return kCIDLib::True;
    }

    return kCIDLib::False;
}

tCIDLib::TBoolean
TKrnlSocket::bAllowBroadcast(const tCIDLib::TBoolean bNewState)
{
    return __bSetSockOpt(__hsockThis.__phsockiThis->iDescr,
                         SO_BROADCAST,
                         (bNewState ? 1 : 0));
}


tCIDLib::TBoolean TKrnlSocket::bBind(const  tCIDLib::TIPAddr    ipaBindTo
                                    , const tCIDLib::TIPPortNum ippnTarget)
{
    sockaddr_in SockAddr = {0};
    SockAddr.sin_family = AF_INET;

    // IP addresses are assumed to be in network order
    SockAddr.sin_addr.s_addr = ipaBindTo;

    // Make sure the port number gets converted to network byte order
    SockAddr.sin_port = ::htons(ippnTarget);

    if (::bind(__hsockThis.__phsockiThis->iDescr,
               reinterpret_cast<sockaddr*>(&SockAddr),
               sizeof(SockAddr)))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bClose()
{
    if (__hsockThis.__phsockiThis->iDescr != -1)
    {
        if (::close(__hsockThis.__phsockiThis->iDescr))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
        __hsockThis.__phsockiThis->iDescr = -1;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bConnect(  const   tCIDLib::TIPAddr    ipaConnectTo
                        , const tCIDLib::TIPPortNum ippnTarget)
{
    // Create a socket address for the target
    sockaddr_in SockAddr = {0};
    SockAddr.sin_family = AF_INET;

    // IP addresses are assumed to be in network byte order
    SockAddr.sin_addr.s_addr = ipaConnectTo;

    // Get the port number into network byte order
    SockAddr.sin_port = ::htons(ippnTarget);

    if (::connect(__hsockThis.__phsockiThis->iDescr,
                  reinterpret_cast<sockaddr*>(&SockAddr),
                  sizeof(SockAddr)))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bCreate(   const   tCIDSock::ESocketTypes   eType
                        , const tCIDSock::ESockProtos  eProtocol)
{
    // Make sure its not already created
    if (__hsockThis.__phsockiThis->iDescr != -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    // Convert the passed socket type to the internal value
    const tCIDLib::TSInt iType = (eType == tCIDSock::ESocketTypes::Stream) ?
                                 SOCK_STREAM : SOCK_DGRAM;

    tCIDLib::TSInt iProtocol;
    if (!__bLookupProtocolNumber(eProtocol, iProtocol))
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
    __hsockThis.__phsockiThis->iDescr = iTmp;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bDataReady(tCIDLib::TCard4& c4ToFill) const
{
    // Query the amount of data readable via the next read operation
    tCIDLib::TSInt iNumBytes = 0;

    if (::ioctl(__hsockThis.__phsockiThis->iDescr, FIONREAD, &iNumBytes))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4ToFill = tCIDLib::TCard4(iNumBytes);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bDontRoute(tCIDLib::TBoolean& bToFill) const
{
    tCIDLib::TSInt iDontRoute;
    tCIDLib::TBoolean bRc = __bGetSockOpt(__hsockThis.__phsockiThis->iDescr,
                                          SO_DONTROUTE,
                                          iDontRoute);
    if (bRc)
    {
        bToFill = iDontRoute ? kCIDLib::True : kCIDLib::False;
        return kCIDLib::True;
    }

    return kCIDLib::False;
}

tCIDLib::TBoolean TKrnlSocket::bDontRoute(const tCIDLib::TBoolean bNewState)
{
    return __bSetSockOpt(__hsockThis.__phsockiThis->iDescr,
                         SO_DONTROUTE,
                         (bNewState ? 1 : 0));
}


tCIDLib::TBoolean TKrnlSocket::bIsOpen(tCIDLib::TBoolean& bToFill) const
{
    bToFill = (__hsockThis.__phsockiThis->iDescr != -1);
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bLinger(tCIDLib::TBoolean& bToFill) const
{
    struct linger LingerInfo;
    socklen_t LingerLen = sizeof(LingerInfo);

    if (::getsockopt(__hsockThis.__phsockiThis->iDescr,
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

    if (::setsockopt(__hsockThis.__phsockiThis->iDescr,
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

    if (::listen(__hsockThis.__phsockiThis->iDescr, iActualBL))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlSocket::bLocalEndPoint(tCIDLib::TIPPortNum&    ippnToFill
                            , tCIDLib::TIPAddr&     ipaToFill) const
{
    sockaddr_in SockAddr = {0};
    socklen_t Len = sizeof(SockAddr);

    if (::getsockname(__hsockThis.__phsockiThis->iDescr,
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
    ipaToFill = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bMaxMsgSize(tCIDLib::TCard4& c4ToFill) const
{
    c4ToFill = UIO_MAXIOV;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bQueryStatus(          tCIDLib::TBoolean&  bIsReadable
                            ,       tCIDLib::TBoolean&  bIsWriteable
                            ,       tCIDLib::TBoolean&  bHasExcepts
                            , const tCIDLib::TCard4     c4Timeout) const
{
    // Set up the select structures for our socket
    fd_set FDRead, FDWrite, FDExcept;

    FD_ZERO(&FDRead);
    FD_SET(__hsockThis.__phsockiThis->iDescr, &FDRead);
    FD_ZERO(&FDWrite);
    FD_SET(__hsockThis.__phsockiThis->iDescr, &FDWrite);
    FD_ZERO(&FDExcept);
    FD_SET(__hsockThis.__phsockiThis->iDescr, &FDExcept);

    //
    //  Set up the timeout. We allow the caller to pass in c4MaxWait to
    //  have a totally blocking select (which means a nul time structure
    //  passed to select.) Otherwise, we have to set up the structure.
    //
    timeval* pTime = 0;
    timeval ActualTime;
    if (c4Timeout != kCIDLib::c4MaxWait)
    {
        pTime = &ActualTime;
        ActualTime.tv_sec = c4Timeout / 1000;
        ActualTime.tv_usec = (c4Timeout % 1000) * 1000;
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
    bIsReadable = FD_ISSET(__hsockThis.__phsockiThis->iDescr, &FDRead) ? kCIDLib::True : kCIDLib::False;
    bIsWriteable = FD_ISSET(__hsockThis.__phsockiThis->iDescr, &FDWrite) ? kCIDLib::True : kCIDLib::False;
    bHasExcepts = FD_ISSET(__hsockThis.__phsockiThis->iDescr, &FDExcept) ? kCIDLib::True : kCIDLib::False;

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

    const tCIDLib::TSInt iRet = ::recv(__hsockThis.__phsockiThis->iDescr,
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
                            ,       tCIDLib::TIPAddr&       ipaFrom
                            ,       tCIDLib::TIPPortNum&    ippnFrom
                            , const tCIDLib::TBoolean       bPeek)
{
    // Create a socket address for the target
    sockaddr_in SockAddr = {0};
    socklen_t Size = sizeof(SockAddr);

    // Set up the flags
    const tCIDLib::TCard4 c4Flags = bPeek ? MSG_PEEK : 0;

    tCIDLib::TSInt iRet = ::recvfrom(__hsockThis.__phsockiThis->iDescr,
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
    ipaFrom = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    c4Received = static_cast<tCIDLib::TCard4>(iRet);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bRecvBufferSize(tCIDLib::TCard4& c4ToFill) const
{
    tCIDLib::TSInt iVal = 0;

    if (!__bGetSockOpt(__hsockThis.__phsockiThis->iDescr,
                       SO_RCVBUF,
                       iVal))
    {
        return kCIDLib::False;
    }

    c4ToFill = tCIDLib::TCard4(iVal);

    return kCIDLib::False;
}

tCIDLib::TBoolean TKrnlSocket::bRecvBufferSize(const tCIDLib::TCard4 c4NewSize)
{
    return __bSetSockOpt(__hsockThis.__phsockiThis->iDescr,
                         SO_RCVBUF,
                         tCIDLib::TInt4(c4NewSize));
}


tCIDLib::TBoolean
TKrnlSocket::bRemoteEndPoint(   tCIDLib::TIPPortNum&    ippnToFill
                                , tCIDLib::TIPAddr&     ipaToFill) const
{
    sockaddr_in SockAddr = {0};
    socklen_t Len = sizeof(SockAddr);

    if (::getpeername(__hsockThis.__phsockiThis->iDescr,
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
    ipaToFill = tCIDLib::TIPAddr(SockAddr.sin_addr.s_addr);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bSend(const   tCIDLib::TVoid* const  pToSend
                                    , const tCIDLib::TCard4         c4BytesToSend
                                    ,       tCIDLib::TCard4&        c4Sent)
{
    tCIDLib::TSInt iRet = ::send(__hsockThis.__phsockiThis->iDescr,
                                 pToSend,
                                 c4BytesToSend,
                                 0);

    if (iRet == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4Sent = tCIDLib::TCard4(iRet);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bSendBufferSize(tCIDLib::TCard4& c4ToFill) const
{
    tCIDLib::TSInt iVal = 0;

    if (!__bGetSockOpt(__hsockThis.__phsockiThis->iDescr,
                       SO_SNDBUF,
                       iVal))
    {
        return kCIDLib::False;
    }

    c4ToFill = tCIDLib::TCard4(iVal);

    return kCIDLib::False;
}

tCIDLib::TBoolean TKrnlSocket::bSendBufferSize(const tCIDLib::TCard4 c4NewSize)
{
    return __bSetSockOpt(__hsockThis.__phsockiThis->iDescr,
                         SO_SNDBUF,
                         tCIDLib::TInt4(c4NewSize));
}


tCIDLib::TBoolean TKrnlSocket::bSendKeepAlive(tCIDLib::TBoolean& bToFill) const
{
    tCIDLib::TSInt iKeepAlive;
    tCIDLib::TBoolean bRc = __bGetSockOpt(__hsockThis.__phsockiThis->iDescr,
                                          SO_KEEPALIVE,
                                          iKeepAlive);
    if (bRc)
    {
        bToFill = iKeepAlive ? kCIDLib::True : kCIDLib::False;
        return kCIDLib::True;
    }

    return kCIDLib::False;
}

tCIDLib::TBoolean TKrnlSocket::bSendKeepAlive(const tCIDLib::TBoolean bNewState)
{
    return __bSetSockOpt(__hsockThis.__phsockiThis->iDescr,
                         SO_KEEPALIVE,
                         (bNewState ? 1 : 0));
}


tCIDLib::TBoolean
TKrnlSocket::bSendTo(   const   tCIDLib::TVoid* const   pToSend
                        , const tCIDLib::TCard4         c4BytesToSend
                        ,       tCIDLib::TCard4&        c4Sent
                        , const tCIDLib::TIPAddr        ipaTarget
                        , const tCIDLib::TIPPortNum     ippnTarget)
{
    // Create a socket address for the target
    sockaddr_in SockAddr = {0};
    SockAddr.sin_family = AF_INET;

    // IP addresses are assumed to be in network byte order
    SockAddr.sin_addr.s_addr = ipaTarget;

    // Get the port number into network byte order
    SockAddr.sin_port = ::htons(ippnTarget);

    tCIDLib::TSInt iRet = ::sendto(__hsockThis.__phsockiThis->iDescr,
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


tCIDLib::TBoolean TKrnlSocket::bSockError(tCIDLib::TInt4& i4ToFill) const
{
    tCIDLib::TSInt iVal = 0;

    if (!__bGetSockOpt(__hsockThis.__phsockiThis->iDescr,
                       SO_ERROR,
                       iVal))
    {
        return kCIDLib::False;
    }

    i4ToFill = iVal;

    return kCIDLib::False;
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

    if (::shutdown(__hsockThis.__phsockiThis->iDescr, iHow))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlSocket::bWaitForData(          tCIDLib::TBoolean&  bGotData
                            , const tCIDLib::TCard4     c4Timeout) const
{
    // Set up the select structures for our socket
    fd_set FDRead;
    FD_ZERO(&FDRead);
    FD_SET(__hsockThis.__phsockiThis->iDescr, &FDRead);

    //
    //  Set up the timeout. We allow the caller to pass in c4MaxWait to
    //  have a totally blocking select (which means a nul time structure
    //  passed to select.) Otherwise, we have to set up the structure.
    //
    timeval* pTime = 0;
    timeval ActualTime;
    if (c4Timeout != kCIDLib::c4MaxWait)
    {
        pTime = &ActualTime;
        ActualTime.tv_sec = c4Timeout / 1000;
        ActualTime.tv_usec = (c4Timeout % 1000) * 1000;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select
    (
        0
        , &FDRead
        , 0
        , 0
        , pTime
    );

    if (iCount == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    bGotData = FD_ISSET(__hsockThis.__phsockiThis->iDescr, &FDRead) != 0;

    return kCIDLib::True;
}
