//
// FILE NAME: CIDSock_DataGramSocket.cpp
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
//  This file implements the TDatagramSocket class, along with two simple
//  derivatives TClientDatagramSocket and TServerDatagramSocket.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDSock_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDatagramSocket,TSocket)
RTTIDecls(TClientDatagramSocket,TDatagramSocket)
RTTIDecls(TServerDatagramSocket,TDatagramSocket)



// ---------------------------------------------------------------------------
//   CLASS: TDatagramSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDatagramSocket: Destructor
// ---------------------------------------------------------------------------
TDatagramSocket::~TDatagramSocket()
{
}


// ---------------------------------------------------------------------------
//  TDatagramSocket: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDatagramSocket::bAllowBroadcast() const
{
    tCIDLib::TBoolean bRet;
    if (!ksockImpl().bGetSockOpt(TKrnlSocket::EBSockOpts::AllowBroadcast, bRet))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_AllowBroadcast)
        );
    }
    return bRet;
}

tCIDLib::TBoolean
TDatagramSocket::bAllowBroadcast(const tCIDLib::TBoolean bNewState)
{
    if (!ksockImpl().bSetSockOpt(TKrnlSocket::EBSockOpts::AllowBroadcast, bNewState))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_AllowBroadcast)
        );
    }
    return bNewState;
}


tCIDLib::TCard4 TDatagramSocket::c4MaxMsgSize() const
{
    tCIDLib::TInt4 i4Ret;
    if (!ksockImpl().bGetSockOpt(TKrnlSocket::EISockOpts::MaxMsgSize, i4Ret))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_MaxMsgSize)
        );
    }
    return tCIDLib::TCard4(i4Ret);
}


tCIDLib::TCard4
TDatagramSocket::c4ReceiveRawFrom(          TIPEndPoint&            ipepFrom
                                    ,       tCIDLib::TVoid* const   pToFill
                                    , const tCIDLib::TEncodedTime   enctWait
                                    , const tCIDLib::TCard4         c4BytesToRecv
                                    , const tCIDLib::TBoolean       bPeek)
{
    TKrnlIPAddr         kipaFrom;
    tCIDLib::TIPPortNum ippnFrom;

    //
    //  Wait for up to the timeout period for a datagram packet to arrive.
    //  Returning without anything if we don't get any in time.
    //
    tCIDLib::TCard4 c4Got = 0;
    if (bWaitForDataReady(enctWait))
    {
        if (!ksockImpl().bReceiveFrom(pToFill, c4BytesToRecv, c4Got, kipaFrom, ippnFrom, bPeek))
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_ReceiveFrom
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // Fill in the from endpoint
        ipepFrom = TIPEndPoint(kipaFrom, ippnFrom);
    }
    return c4Got;
}

tCIDLib::TCard4
TDatagramSocket::c4ReceiveMBufFrom(         TIPEndPoint&            ipepFrom
                                    ,       TMemBuf&                mbufToFill
                                    , const tCIDLib::TEncodedTime   enctWait
                                    , const tCIDLib::TCard4         c4BytesToRecv
                                    , const tCIDLib::TBoolean       bPeek)
{
    // Make sure the byte to receive is not larger than the max buffer size
    if (c4BytesToRecv > mbufToFill.c4MaxSize())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufToFill.c4MaxSize())
            , TCardinal(c4BytesToRecv)
        );
    }

    //
    //  Figure out the actual size. If c4BytesToRecv is zero, then we use
    //  the current size of the buffer.
    //
    const tCIDLib::TCard4 c4Count = c4BytesToRecv ?
                                    c4BytesToRecv : mbufToFill.c4Size();

    // If the buffer allocation is smaller than the actual, then reallocate it
    if (mbufToFill.c4Size() < c4Count)
        mbufToFill.Reallocate(c4Count, kCIDLib::False);

    // And call the other version
    return c4ReceiveRawFrom(ipepFrom, mbufToFill.pData(), enctWait, c4Count, bPeek);
}


tCIDLib::TCard4
TDatagramSocket::c4SendTo(  const   TIPEndPoint&            ipepTo
                            , const tCIDLib::TVoid* const   pToSend
                            , const tCIDLib::TCard4         c4BytesToSend)
{
    tCIDLib::TCard4 c4Ret;
    if (!ksockImpl().bSendTo(pToSend
                            , c4BytesToSend
                            , c4Ret
                            , ipepTo.kipaThis()
                            , ipepTo.ippnThis()))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SendTo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , ipepTo
        );
    }

    //
    //  If we were not bound, then this will implicitly do so, so we need
    //  to set the flag in our parent class.
    //
    if (!bIsBound())
        bIsBound(kCIDLib::True);

    return c4Ret;
}

tCIDLib::TCard4
TDatagramSocket::c4SendTo(  const   TIPEndPoint&    ipepTo
                            , const TMemBuf&        mbufToSend
                            , const tCIDLib::TCard4 c4BytesToSend)
{
    // Make sure the bytes to send is not larger than the current buffer size
    if (c4BytesToSend > mbufToSend.c4Size())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufToSend.c4Size())
            , TCardinal(c4BytesToSend)
        );
    }

    //
    //  First get the actual bytes. If the c4BytesToSend is zero, then we
    //  use the allocation size of the buffer.
    //
    const tCIDLib::TCard4 c4ActualSz = c4BytesToSend ? c4BytesToSend :
                                        mbufToSend.c4Size();

    // And call the other version
    return c4SendTo(ipepTo, mbufToSend.pc1Data(), c4ActualSz);
}


// Adds the indicated address as a multi-cast group for this socket
tCIDLib::TVoid
TDatagramSocket::JoinMulticastGroup(const   TIPAddress& ipaGroup
                                    , const TIPAddress& ipaIntf)
{
    if (!ksockImpl().bJoinMulticastGroup(ipaGroup.kipaThis(), ipaIntf.kipaThis()))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_JoinMCGrp
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , ipaGroup
        );
    }
}


tCIDLib::TVoid TDatagramSocket::Open(const  tCIDSock::ESockProtos   eProtocol
                                    , const tCIDSock::EAddrTypes    eAddrType)
{
    // Call our parent to create the socket, providing the correct type
    Create(tCIDSock::ESocketTypes::Datagram, eProtocol, eAddrType);
}



// ---------------------------------------------------------------------------
//  TDatagramSocket: Hidden Constructors and operators
// ---------------------------------------------------------------------------
TDatagramSocket::TDatagramSocket()
{
}


// ---------------------------------------------------------------------------
//  TDatagramSocket: Protected, inherited methods
// ---------------------------------------------------------------------------

// The base class calls this. For us, it's a no-op
tCIDLib::TVoid TDatagramSocket::SocketClosed()
{
}




// ---------------------------------------------------------------------------
//   CLASS: TClientDatagramSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TClientDatagramSocket: Constructors and Destructor
// ---------------------------------------------------------------------------
TClientDatagramSocket::TClientDatagramSocket()
{
}

TClientDatagramSocket::TClientDatagramSocket(const  tCIDSock::ESockProtos   eProtocol
                                            , const tCIDSock::EAddrTypes    eAddrType)
{
    Open(eProtocol, eAddrType);
}

TClientDatagramSocket::~TClientDatagramSocket()
{
}


// ---------------------------------------------------------------------------
//  TClientDatagramSocket: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  Sends a Wake On LAN broadcast package to the indicated MAC address. This
//  can be used to wake up another computer if it supports WOL.
//
tCIDLib::TVoid TClientDatagramSocket::SendWOLBroadcast(const TString& strMACAddr)
{
    const tCIDLib::TCard4 c4MACRep = 16;
    const tCIDLib::TCard4 c4MACLen = 12;
    const tCIDLib::TCard4 c4BinMACLen = 6;

    //
    //  If the mac address is in the form with colons or dashes, then remove
    //  them along with any whitespace.
    //
    TString strAddr(strMACAddr);
    TString strRemove(kCIDLib::szWhitespace);
    strRemove.Append(kCIDLib::chColon);
    strRemove.Append(kCIDLib::chHyphenMinus);
    strAddr.Strip(strRemove, tCIDLib::EStripModes::Complete);

    //
    //  And now let's make sure it's a legal MAC address. It must be c4MaxLen
    //  hex digits.
    //
    if (strAddr.c4Length() != c4MACLen)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_BadMACAddr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strAddr
        );
    }

    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4MACLen; c4Index++)
    {
        if (!TRawStr::bIsHexDigit(strAddr[c4Index]))
        {
            facCIDSock().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcTCP_BadMACAddr
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strAddr
            );
        }
    }

    //
    //  Get it into a binary form (each two digits get turned into a hex
    //  byte) to make the work below a lot more efficient. We'll do an el
    //  cheapo cast-based transcoding of the MAC address since we know that
    //  they are all hex digits.
    //
    tCIDLib::TCard1 ac1MAC[c4BinMACLen];
    TString strTmp;
    for (c4Index = 0; c4Index < c4MACLen; c4Index += 2)
    {
        strTmp.CopyInSubStr(strAddr, c4Index, 2);
        ac1MAC[c4Index >> 1] = tCIDLib::TCard1(strTmp.c4Val(tCIDLib::ERadices::Hex));
    }

    //
    //  Ok. It's a potentially good MAC address. So let's build the packet
    //  up. The packet is 6 bytes of 0xFF, followed by the MAC address repeated
    //  c4MaxRep times.
    //
    const tCIDLib::TCard4 c4Len = c4BinMACLen + (c4MACRep * c4BinMACLen);
    THeapBuf mbufData(c4Len, c4Len);

    for (c4Index = 0; c4Index < c4BinMACLen; c4Index++)
        mbufData[c4Index] = 0xFF;

    for (tCIDLib::TCard4 c4RepInd = 0; c4RepInd < c4MACRep; c4RepInd++)
    {
        for (tCIDLib::TCard4 c4MACInd = 0; c4MACInd < c4BinMACLen; c4MACInd++)
            mbufData[c4Index++] = ac1MAC[c4MACInd];
    }

    //
    //  And now set up the broadcast end point on port 9, and send the packet.
    //  For now we just assume IPV4 broadcast is the way WOL will be done and
    //  don't try to deal with the IPV6 thing, which doesn't support broadcast.
    //
    TIPEndPoint ipepBroadcast
    (
        tCIDSock::ESpecAddrs::Broadcast, tCIDSock::EAddrTypes::IPV4, 9
    );

    if (c4SendTo(ipepBroadcast, mbufData, c4Len) != c4Len)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_NotAllData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
        );
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TServerDatagramSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TServerDatagramSocket: Constructors and Destructor
// ---------------------------------------------------------------------------
TServerDatagramSocket::TServerDatagramSocket()
{
}

TServerDatagramSocket::TServerDatagramSocket(const  tCIDSock::ESockProtos   eProtocol
                                            , const TIPEndPoint&            ipepLocal)
{
    // Open it and then do a listening bind
    Open(eProtocol, ipepLocal.eAddrType());
    BindListen(ipepLocal);
}

TServerDatagramSocket::~TServerDatagramSocket()
{
}

