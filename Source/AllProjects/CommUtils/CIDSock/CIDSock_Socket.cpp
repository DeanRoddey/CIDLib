//
// FILE NAME: CIDSock_Socket.cpp
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
//  This file implements the TSocket class.
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
RTTIDecls(TMSockSelItem,TObject)
RTTIDecls(TSocket,TObject)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDSock_Socket
{
    // -----------------------------------------------------------------------
    //  Some stats cache items we maintain
    // -----------------------------------------------------------------------
    TAtomicFlag         atomInitDone;
    TStatsCacheItem     sciSockCnt;
}



// ---------------------------------------------------------------------------
//   CLASS: TMSockSelItem
//  PREFIX: mssi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMSockSelItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TMSockSelItem::TMSockSelItem(TSocket* const psockSel) :

    m_enctLastMsg(TTime::enctNow())
    , m_eFlags(tCIDSock::EMSelFlags::None)
    , m_psockSel(psockSel)
{
}

TMSockSelItem::~TMSockSelItem()
{
    // We don't own the socket
}


// ---------------------------------------------------------------------------
//  TMSockSelItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the last message received stamp
tCIDLib::TEncodedTime TMSockSelItem::enctLastMsg() const
{
    return m_enctLastMsg;
}

tCIDLib::TEncodedTime
TMSockSelItem::enctLastMsg(const tCIDLib::TEncodedTime enctToSet)
{
    m_enctLastMsg = enctToSet;
    return m_enctLastMsg;
}


// Get set the flags
tCIDSock::EMSelFlags TMSockSelItem::eFlags() const
{
    return m_eFlags;
}

tCIDSock::EMSelFlags TMSockSelItem::eFlags(const tCIDSock::EMSelFlags eToSet)
{
    m_eFlags = eToSet;
    return m_eFlags;
}


// Return the ssocket for this select item
TSocket* TMSockSelItem::psockSel()
{
    return m_psockSel;
}




// ---------------------------------------------------------------------------
//   CLASS: TSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSocket: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TSocket::bMultiReadSel(         TRefVector<TMSockSelItem>&  colList
                        , const tCIDLib::TEncodedTime       enctWait
                        , const tCIDLib::TEncodedTime       enctMaxIdle)
{
    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();

    //
    //  Make sure the count is legal. If its zero, just return false now.
    //  If its more than we can select, then throw.
    //
    if (!c4Count)
        return kCIDLib::False;

    if (c4Count > kCIDSock::c4MaxSelect)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BadMSelCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(kCIDSock::c4MaxSelect)
            , TCardinal(c4Count)
        );
    }

    // We have to translate to a list of kernel sockets for our call
    CIDLib_Suppress(26494)
    TKrnlSocket* apksockList[kCIDSock::c4MaxSelect];
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        apksockList[c4Index] = &colList[c4Index]->m_psockSel->ksockImpl();

    CIDLib_Suppress(26494)
    tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect];
    tCIDLib::TCard4         c4Changes = c4Count;
    if (!TKrnlSocket::bMultiReadSel(apksockList, aeFlags, c4Changes, enctWait))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_MultiSelect
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Get the flags back over to the caller's objects. We only got back
    //  the ones that changed.
    //
    const tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TMSockSelItem* pmssiCur = colList[c4Index];
        tCIDSock::EMSelFlags eCur = aeFlags[c4Index];

        if (tCIDLib::bAllBitsOn(eCur, tCIDSock::EMSelFlags::Read))
        {
            // Update the last message stamp
            pmssiCur->enctLastMsg(enctCur);
        }
         else if (enctMaxIdle)
        {
            //
            //  If they provided a max idle time, then see if that has
            //  been exceeded. If so, set the MaxIdle flag. We have to bump
            //  the changes flag since we are forcing one in this case.
            //
            if ((enctCur - pmssiCur->enctLastMsg()) >= enctMaxIdle)
            {
                eCur = tCIDLib::eOREnumBits(eCur, tCIDSock::EMSelFlags::MaxIdle);
                c4Changes++;
            }
        }

        // Store the flags on this one
        pmssiCur->eFlags(eCur);
    }

    // Return true if any changes, else false
    return (c4Changes != 0);
}


tCIDLib::TBoolean
TSocket::bMultiSel(         TRefVector<TMSockSelItem>&  colList
                    , const tCIDLib::TEncodedTime       enctWait
                    , const tCIDLib::TEncodedTime       enctMaxIdle)
{
    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();

    //
    //  Make sure the count is legal. If its zero, just return false now.
    //  If its more than we can select, then throw.
    //
    if (!c4Count)
        return kCIDLib::False;

    if (c4Count > kCIDSock::c4MaxSelect)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BadMSelCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(kCIDSock::c4MaxSelect)
            , TCardinal(c4Count)
        );
    }

    // We have to translate to a list of kernel sockets for our call
    CIDLib_Suppress(26494)
    TKrnlSocket* apksockList[kCIDSock::c4MaxSelect];
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        apksockList[c4Index] = &colList[c4Index]->m_psockSel->ksockImpl();

    CIDLib_Suppress(26494)
    tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect];
    tCIDLib::TCard4         c4Changes = c4Count;
    if (!TKrnlSocket::bMultiSel(apksockList, aeFlags, c4Changes, enctWait))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_MultiSelect
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Get the flags back over to the caller's objects. And update the
    //  time stamp of any of them that are readable. If any are not
    //  readable and the max idle parm is non-zero, check how long since
    //  the last read. If longer than max idle, then force the maxidle
    //  flag on.
    //
    const tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TMSockSelItem* pmssiCur = colList[c4Index];
        tCIDSock::EMSelFlags eCur = aeFlags[c4Index];

        if (tCIDLib::bAllBitsOn(eCur, tCIDSock::EMSelFlags::Read))
        {
            // Update the last message stamp
            pmssiCur->enctLastMsg(enctCur);
        }
         else if (enctMaxIdle)
        {
            //
            //  If they provided a max idle time, then see if that has
            //  been exceeded. If so, set the MaxIdle flag. We have to bump
            //  the changes flag since we are forcing one in this case.
            //
            if ((enctCur - pmssiCur->enctLastMsg()) >= enctMaxIdle)
            {
                eCur = tCIDLib::eOREnumBits(eCur, tCIDSock::EMSelFlags::MaxIdle);
                c4Changes++;
            }
        }

        // Store the flags on this one
        pmssiCur->eFlags(eCur);
    }

    // Return true if any changes, else false
    return (c4Changes != 0);
}



// ---------------------------------------------------------------------------
//  TSocket: Destructors
// ---------------------------------------------------------------------------
TSocket::~TSocket()
{
    // Once we close it, it's not bound anymore
    m_bIsBound = kCIDLib::False;

    //
    //  At this level all we are responsible for is a close of the socket. Don't just
    //  blindly close because we need to decrement the open sockets stat if we really
    //  are going to close it.
    //
    tCIDLib::TBoolean bOpen;
    if (m_ksockImpl.bIsOpen(bOpen) && bOpen)
    {
        // Dec the open socket stat
        TStatsCache::c8DecCounter(CIDSock_Socket::sciSockCnt);

        if (!m_ksockImpl.bClose())
        {
            facCIDSock().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_Close
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::Internal
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TSocket: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Associate an even with this socket to use with async reads
tCIDLib::TVoid TSocket::AssociateReadEvent(TEvent& evToSignal)
{
    if (!m_ksockImpl.bAssociateReadEvent(evToSignal.kevImpl()))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_AssocEvent
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


//
//  Checks to see if data is ready and returns the bytes available. Could
//  be more by the time you get back to actually read it.
//
tCIDLib::TBoolean TSocket::bDataReady(tCIDLib::TCard4& c4Bytes) const
{
    if (!m_ksockImpl.bDataReady(c4Bytes))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_Wait
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return (c4Bytes != 0);
}


// Get or set the dontroute socket option
tCIDLib::TBoolean TSocket::bDontRoute() const
{
    tCIDLib::TBoolean bRet;
    if (!m_ksockImpl.bGetSockOpt(TKrnlSocket::EBSockOpts::DontRoute, bRet))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_DontRoute)
        );
    }
    return bRet;
}

tCIDLib::TBoolean TSocket::bDontRoute(const tCIDLib::TBoolean bNewState)
{
    if (!m_ksockImpl.bSetSockOpt(TKrnlSocket::EBSockOpts::DontRoute, bNewState))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_DontRoute)
        );
    }
    return bNewState;
}


// Tells the caller if we are bound or not
tCIDLib::TBoolean TSocket::bIsBound() const
{
    return m_bIsBound;
}


// Tells the caller if we are open or not. Might not be bound or connected
tCIDLib::TBoolean TSocket::bIsOpen() const
{
    tCIDLib::TBoolean bRet;
    if (!m_ksockImpl.bIsOpen(bRet))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_OpenCheckFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return bRet;
}


// Set or get the linger socket option
tCIDLib::TBoolean TSocket::bLinger() const
{
    tCIDLib::TBoolean bRet;
    if (!m_ksockImpl.bLinger(bRet))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_Linger)
        );
    }
    return bRet;
}

tCIDLib::TBoolean TSocket::bLinger( const   tCIDLib::TBoolean   bNewState
                                    , const tCIDLib::TCard4     c4Seconds)
{
    if (!m_ksockImpl.bLinger(bNewState, c4Seconds))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_Linger)
        );
    }
    return bNewState;
}


// Set or get the reuseaddr socket option
tCIDLib::TBoolean TSocket::bReuseAddr() const
{
    tCIDLib::TBoolean bRet;
    if (!m_ksockImpl.bGetSockOpt(TKrnlSocket::EBSockOpts::ReuseAddr, bRet))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_ReuseAddr)
        );
    }
    return bRet;
}

tCIDLib::TBoolean TSocket::bReuseAddr(const tCIDLib::TBoolean bNewState)
{
    if (!m_ksockImpl.bSetSockOpt(TKrnlSocket::EBSockOpts::ReuseAddr, bNewState))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_ReuseAddr)
        );
    }
    return bNewState;
}


// Set or get the keepalive socket option
tCIDLib::TBoolean TSocket::bSendKeepAlive() const
{
    tCIDLib::TBoolean bRet;
    if (!m_ksockImpl.bGetSockOpt(TKrnlSocket::EBSockOpts::KeepAlive, bRet))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_KeepAlive)
        );
    }
    return bRet;
}

tCIDLib::TBoolean TSocket::bSendKeepAlive(const tCIDLib::TBoolean bNewState)
{
    if (!m_ksockImpl.bSetSockOpt(TKrnlSocket::EBSockOpts::KeepAlive, bNewState))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_KeepAlive)
        );
    }
    return bNewState;
}


//
//  We have a flag that client code can set and read for its own purposes.
//
tCIDLib::TBoolean TSocket::bUserFlag() const
{
    return m_bUserFlag;
}

tCIDLib::TBoolean TSocket::bUserFlag(const tCIDLib::TBoolean bToSet)
{
    m_bUserFlag = bToSet;
    return m_bUserFlag;
}


//
//  Wait for it to be ok to send without blocking. If it's longer than a
//  second, then we use a loop and watch for shutdown requests. Note that
//  this will also catch the kCIDLib::enctMaxWait scenario.
//
tCIDLib::TBoolean
TSocket::bWaitForSendReady(const tCIDLib::TEncodedTime enctWait) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    if (enctWait > kCIDLib::enctOneSecond)
    {
        tCIDLib::TEncodedTime enctCur = TKrnlTimeStamp::enctNow();
        tCIDLib::TEncodedTime enctCurWait = 0;
        tCIDLib::TEncodedTime enctEnd = 0;
        if (enctWait == kCIDLib::enctMaxWait)
            enctEnd = kCIDLib::enctMaxWait;
        else
            enctEnd = enctCur + enctWait;

        TThread* pthrCaller = nullptr;
        while (enctCur < enctEnd)
        {
            // Wait for up to half a second this time around
            if (enctCur + kCIDLib::enctHalfSecond > enctEnd)
                enctCurWait = (enctEnd - enctCur);
            else
                enctCurWait = kCIDLib::enctHalfSecond;

            if (!m_ksockImpl.bWaitForSendReady(bRet, enctCurWait))
            {
                facCIDSock().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSockErrs::errcSock_QueryStatus
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }

            // If ready, break out now
            if (bRet)
                break;

            // Not yet, check for a shutdown request
            if (!pthrCaller)
                pthrCaller = TThread::pthrCaller();
            if (pthrCaller->bCheckShutdownRequest())
                break;

            enctCur = TKrnlTimeStamp::enctNow();
        }
    }
     else
    {
        if (!m_ksockImpl.bWaitForSendReady(bRet, enctWait))
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_QueryStatus
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
    return bRet;
}

tCIDLib::TBoolean
TSocket::bWaitForSendReadyMS(const tCIDLib::TCard4 c4WaitMillis) const
{
    // Just convert to a wait time
    const tCIDLib::TEncodedTime enctWait(c4WaitMillis * kCIDLib::enctOneMilliSec);
    return bWaitForSendReady(enctWait);
}


//
//  Wait for a time for data to be ready to read. If it's longer than a
//  second, then we use a loop and watch for shutdown requests. Note that
//  this will also catch the kCIDLib::enctMaxWait scenario.
//
tCIDLib::TBoolean
TSocket::bWaitForDataReady(const tCIDLib::TEncodedTime enctWait) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    if (enctWait > kCIDLib::enctOneSecond)
    {
        tCIDLib::TEncodedTime enctCur = TKrnlTimeStamp::enctNow();
        tCIDLib::TEncodedTime enctCurWait = 0;
        tCIDLib::TEncodedTime enctEnd = 0;
        if (enctWait == kCIDLib::enctMaxWait)
            enctEnd = kCIDLib::enctMaxWait;
        else
            enctEnd = enctCur + enctWait;

        TThread* pthrCaller = nullptr;
        while (enctCur < enctEnd)
        {
            // Wait for a while
            if (enctCur + kCIDLib::enctHalfSecond > enctEnd)
                enctCurWait = (enctEnd - enctCur);
            else
                enctCurWait = kCIDLib::enctHalfSecond;

            if (!m_ksockImpl.bWaitForDataReady(bRet, enctCurWait))
            {
                facCIDSock().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSockErrs::errcSock_QueryStatus
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }

            // If ready, break out now
            if (bRet)
                break;

            // Not yet, check for a shutdown request
            if (!pthrCaller)
                pthrCaller = TThread::pthrCaller();
            if (pthrCaller->bCheckShutdownRequest())
                break;

            enctCur = TKrnlTimeStamp::enctNow();
        }
    }
     else
    {
        if (!m_ksockImpl.bWaitForDataReady(bRet, enctWait))
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_QueryStatus
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
    return bRet;
}

tCIDLib::TBoolean
TSocket::bWaitForDataReadyMS(const tCIDLib::TCard4 c4WaitMillis) const
{
    // Just convert to encoded time and call the other version
    const tCIDLib::TEncodedTime enctWait(c4WaitMillis * kCIDLib::enctOneMilliSec);
    return bWaitForDataReady(enctWait);
}


//
//  Mostly for listening sockets, to allow them to bind to a specific local address
//  and port. It has to already be open or this will fail.
//
tCIDLib::TVoid TSocket::BindListen(const TIPEndPoint& ipepLocal)
{
    if (m_bIsBound)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_AlreadyBound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , ipepLocalEndPoint()
        );
    }

    if (!m_ksockImpl.bBindListen(ipepLocal.kipaThis(), ipepLocal.ippnThis()))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BindListen
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , ipepLocal
        );
    }

    // Mark it as bound
    m_bIsBound = kCIDLib::True;
}


//
//  In some cases they had to bind locally and have a fixed target, so we can can
//  get them bound to a known good address. It has to already be open, or this will
//  fail.
//
tCIDLib::TVoid
TSocket::BindForRemote( const   TIPEndPoint&        ipepRemote
                        , const tCIDLib::TIPPortNum ippnLocal)
{
    if (m_bIsBound)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_AlreadyBound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , ipepLocalEndPoint()
        );
    }

    if (!m_ksockImpl.bBindForRemote(ipepRemote.kipaThis(), ippnLocal))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BindRemote
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , ipepRemote
            , TCardinal(ippnLocal)
        );
    }

    // Mark it as bound
    m_bIsBound = kCIDLib::True;
}


// Get or set the receive buffer size
tCIDLib::TCard4 TSocket::c4RecvBufferSize() const
{
    tCIDLib::TInt4 i4Ret;
    if (!m_ksockImpl.bGetSockOpt(TKrnlSocket::EISockOpts::ReceiveBuf, i4Ret))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_RecvBufSize)
        );
    }
    return tCIDLib::TCard4(i4Ret);
}

tCIDLib::TCard4 TSocket::c4RecvBufferSize(const tCIDLib::TCard4 c4NewSize)
{
    if (!m_ksockImpl.bSetSockOpt(TKrnlSocket::EISockOpts::ReceiveBuf
                                , tCIDLib::TInt4(c4NewSize)))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_RecvBufSize)
        );
    }
    return c4NewSize;
}


// Get or set the send buffer size
tCIDLib::TCard4 TSocket::c4SendBufferSize() const
{
    tCIDLib::TInt4 i4Ret;
    if (!m_ksockImpl.bGetSockOpt(TKrnlSocket::EISockOpts::SendBuf, i4Ret))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_SendBufSize)
        );
    }
    return tCIDLib::TCard4(i4Ret);
}

tCIDLib::TCard4 TSocket::c4SendBufferSize(const tCIDLib::TCard4 c4NewSize)
{
    if (!m_ksockImpl.bSetSockOpt(TKrnlSocket::EISockOpts::SendBuf
                                , tCIDLib::TInt4(c4NewSize)))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDSock().strMsg(kSockMsgs::midSock_SendBufSize)
        );
    }
    return c4NewSize;
}


tCIDLib::TVoid TSocket::ClearReadEvent(TEvent& evToClear)
{
    if (!m_ksockImpl.bClearReadEvent(evToClear.kevImpl()))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_ClearEvent
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


//
//  Does a hard close of the socket. For stream sockets, you should use the
//  shutdown method in most cases, which will then call this method after
//  doing a graceful shutdown.
//
//  Just in case, there is a protected virtual that we call, to let the
//  derived class know it's closed.
//
tCIDLib::TVoid TSocket::Close()
{
    // Once we close it, it's not bound anymore
    m_bIsBound = kCIDLib::False;

    tCIDLib::TBoolean bOpen;
    if (m_ksockImpl.bIsOpen(bOpen) && bOpen)
    {
        // Dec the open socket stat
        TStatsCache::c8DecCounter(CIDSock_Socket::sciSockCnt);

        if (!m_ksockImpl.bClose())
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_Close
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
            );
        }
    }

    // Let the derived class know
    SocketClosed();
}


tCIDSock::ESockEvs TSocket::eEnumEvents(TEvent& evReset)
{
    tCIDSock::ESockEvs eEvents;
    if (!m_ksockImpl.bEnumEvents(evReset.kevImpl(), eEvents))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_EnumEvents
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return eEvents;
}


// Returns the local end point that this socket is bound to, if any
TIPEndPoint TSocket::ipepLocalEndPoint() const
{
    TKrnlIPAddr         kipaLocal;
    tCIDLib::TIPPortNum ippnLocal;

    if (!m_ksockImpl.bLocalEndPoint(ippnLocal, kipaLocal))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QueryLocalBind
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return TIPEndPoint(kipaLocal, ippnLocal);
}


// Queries the 'select' values for this socket
tCIDLib::TVoid
TSocket::QuerySelStatus(        tCIDLib::TBoolean&      bIsReadable
                        ,       tCIDLib::TBoolean&      bIsWriteable
                        ,       tCIDLib::TBoolean&      bHasExcepts
                        , const tCIDLib::TEncodedTime   enctWait) const
{
    if (!m_ksockImpl.bQuerySelStatus(bIsReadable, bIsWriteable, bHasExcepts, enctWait))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QueryStatus
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// ---------------------------------------------------------------------------
//  TSocket: Hidden constructors
// ---------------------------------------------------------------------------

// Creates a closed socket which must be created later to be of use
TSocket::TSocket() :

    m_bIsBound(kCIDLib::False)
    , m_bUserFlag(kCIDLib::False)
{
    // Initialize our stats cache item if needed
    if (!CIDSock_Socket::atomInitDone)
    {
        TBaseLock lockInit;
        if (!CIDSock_Socket::atomInitDone)
        {
            TStatsCache::RegisterItem
            (
                kCIDSock::pszStat_Net_OpenSockCnt
                , tCIDLib::EStatItemTypes::Counter
                , CIDSock_Socket::sciSockCnt
            );

            CIDSock_Socket::atomInitDone.Set();
        }
    }
}

// Creates an open socket, ready to be connected (or used if datagram)
TSocket::TSocket(const  tCIDSock::ESocketTypes  eType
                , const tCIDSock::ESockProtos   eProtocol
                , const tCIDSock::EAddrTypes    eAddrType) :

    m_bIsBound(kCIDLib::False)
    , m_bUserFlag(kCIDLib::False)
{
    // Initialize our stats cache item if needed
    if (!CIDSock_Socket::atomInitDone)
    {
        TBaseLock lockInit;
        if (!CIDSock_Socket::atomInitDone)
        {
            TStatsCache::RegisterItem
            (
                kCIDSock::pszStat_Net_OpenSockCnt
                , tCIDLib::EStatItemTypes::Counter
                , CIDSock_Socket::sciSockCnt
            );

            CIDSock_Socket::atomInitDone.Set();
        }
    }

    if (!m_ksockImpl.bCreate(eType, eProtocol, eAddrType))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Bump the open socket stat
    TStatsCache::c8IncCounter(CIDSock_Socket::sciSockCnt);
}


//
//  A special case, seldom used, where we might get a socket back from some scheme
//  other than our own ctors and need to insert that socket into a socket object.
//
TSocket::TSocket(const TSocketHandle& hsockToAdopt) :

    m_bIsBound(kCIDLib::False)
    , m_ksockImpl(hsockToAdopt)
    , m_bUserFlag(kCIDLib::False)
{
    // Initialize our stats cache item if needed
    if (!CIDSock_Socket::atomInitDone)
    {
        TBaseLock lockInit;
        if (!CIDSock_Socket::atomInitDone)
        {
            TStatsCache::RegisterItem
            (
                kCIDSock::pszStat_Net_OpenSockCnt
                , tCIDLib::EStatItemTypes::Counter
                , CIDSock_Socket::sciSockCnt
            );
            CIDSock_Socket::atomInitDone.Set();
        }
    }

    // If it's open, then bump the stat
    tCIDLib::TBoolean bOpen;
    if (m_ksockImpl.bIsOpen(bOpen) && bOpen)
        TStatsCache::c8IncCounter(CIDSock_Socket::sciSockCnt);
}


// ---------------------------------------------------------------------------
//  TSocket: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSocket::bIsBound(const tCIDLib::TBoolean bIsBound)
{
    m_bIsBound = bIsBound;
    return m_bIsBound;
}


tCIDLib::TVoid TSocket::Create( const   tCIDSock::ESocketTypes  eType
                                , const tCIDSock::ESockProtos   eProtocol
                                , const tCIDSock::EAddrTypes    eAddrType)
{
    if (!m_ksockImpl.bCreate(eType, eProtocol, eAddrType))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Bump the open socket stat
    TStatsCache::c8IncCounter(CIDSock_Socket::sciSockCnt);
}


// Translates socket errors into our errors, at the kernel and socket levels
tCIDLib::EErrClasses
TSocket::eXlatKrnlErrClass(const TKrnlError& kerrToXlat) const
{
    tCIDLib::EErrClasses eRet = tCIDLib::EErrClasses::Unknown;
    switch(kerrToXlat.errcId())
    {
        // These all indicate a loss of the connection
        case kKrnlErrs::errcNet_ConnAborted :
        case kKrnlErrs::errcNet_ConnReset :
        case kKrnlErrs::errcGen_NotConnected :
        case kKrnlErrs::errcNet_NotASocket :
            eRet = tCIDLib::EErrClasses::LostConnection;
            break;

        case kKrnlErrs::errcGen_Already :
        case kKrnlErrs::errcGen_AlreadyConnected :
            eRet = tCIDLib::EErrClasses::Already;
            break;

        case kKrnlErrs::errcGen_Busy :
        case kKrnlErrs::errcFl_TooManyOpenFiles :
        case kKrnlErrs::errcGen_TooMany :
        case kKrnlErrs::errcNet_NoBuffers :
        case kKrnlErrs::errcProc_ProcessLimit :
            eRet = tCIDLib::EErrClasses::OutResource;
            break;

        case kKrnlErrs::errcGen_NotSupported :
            eRet = tCIDLib::EErrClasses::NotSupported;
            break;

        case kKrnlErrs::errcGen_Timeout :
            eRet = tCIDLib::EErrClasses::Timeout;
            break;

        case kKrnlErrs::errcData_NameTooLong :
        case kKrnlErrs::errcNet_MsgSize :
            eRet = tCIDLib::EErrClasses::Format;
            break;

        case kKrnlErrs::errcGen_GeneralFault :
        case kKrnlErrs::errcGen_NotAvailable :
        case kKrnlErrs::errcNet_NetworkDown :
        case kKrnlErrs::errcNet_NetworkUnreachable :
        case kKrnlErrs::errcNet_HostDown :
        case kKrnlErrs::errcNet_HostUnreachable :
        case kKrnlErrs::errcNet_NetworkReset :
            eRet = tCIDLib::EErrClasses::SystemFailure;
            break;

        case kKrnlErrs::errcData_InvalidHandle :
        case kKrnlErrs::errcData_InvalidValue :
            eRet = tCIDLib::EErrClasses::AppError;
            break;

        case kKrnlErrs::errcNet_DestAdrRequired :
        case kKrnlErrs::errcGen_NotInitialized :
            eRet = tCIDLib::EErrClasses::Internal;
            break;

        default :
            break;
    }
    return eRet;
}


tCIDLib::TErrCode
TSocket::errcXlatKrnlErr(   const   TKrnlError&         kerrToXlat
                            , const tCIDLib::TErrCode   errcDefault) const
{
    tCIDLib::TErrCode errcRet = errcDefault;
    switch(kerrToXlat.errcId())
    {
        case kKrnlErrs::errcGen_NotSupported :
            errcRet = kSockErrs::errcSock_Unsupported;
            break;

        case kKrnlErrs::errcNet_ConnAborted :
            errcRet = kSockErrs::errcSock_ConnAborted;
            break;

        case kKrnlErrs::errcGen_Timeout :
            errcRet = kSockErrs::errcSock_Timeout;
            break;

        case kKrnlErrs::errcNet_NetworkUnreachable :
            errcRet = kSockErrs::errcSock_NetworkUnreachable;
            break;

        case kKrnlErrs::errcNet_MsgSize :
            errcRet = kSockErrs::errcSock_DGBufTooSmall;
            break;

        case kKrnlErrs::errcNet_NetworkDown :
            errcRet = kSockErrs::errcSock_NetworkDown;
            break;

        case kKrnlErrs::errcNet_ConnRefused :
            errcRet = kSockErrs::errcSock_ConnRefused;
            break;

        case kKrnlErrs::errcNet_ConnReset :
            errcRet = kSockErrs::errcSock_NotConnected ;
            break;

        case kKrnlErrs::errcNet_HostUnreachable :
            errcRet = kSockErrs::errcSock_HostUnreachable;
            break;

        case kKrnlErrs::errcGen_AlreadyConnected :
            errcRet = kSockErrs::errcSock_AlreadyConnected;
            break;

        case kKrnlErrs::errcData_NameTooLong :
            errcRet = kSockErrs::errcSock_NameTooLong;
            break;

        default :
            break;
    }
    return errcRet;
}


// Provide address to the kernel socket wrapper to derived classes
const TKrnlSocket& TSocket::ksockImpl() const
{
    return m_ksockImpl;
}

TKrnlSocket& TSocket::ksockImpl()
{
    return m_ksockImpl;
}


