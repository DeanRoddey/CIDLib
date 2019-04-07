//
// FILE NAME: CIDSock_SocketListener.cpp
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
//  This file implements the TStreamSocketListner and TDGramSocketListner
//  classes.
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
RTTIDecls(TSocketListener,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TSocketListener
//  PREFIX: sockl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSocketListener: Constructors and Destructor
// ---------------------------------------------------------------------------
TSocketListener::TSocketListener(const  tCIDLib::TIPPortNum ippnToUse
                                , const tCIDLib::TCard4     c4MaxWaiting) :

    m_c4Count(0)
    , m_c4MaxWaiting(c4MaxWaiting)
    , m_ippnListenOn(ippnToUse)
{
    // Zero the socket list
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 2; c4Index++)
        m_apksockList[c4Index] = nullptr;
}

TSocketListener::TSocketListener(const  tCIDLib::TIPPortNum     ippnToUse
                                , const tCIDSock::ESockProtos   eProtocol
                                , const tCIDLib::TCard4         c4MaxWaiting) :

    m_c4Count(0)
    , m_c4MaxWaiting(c4MaxWaiting)
    , m_ippnListenOn(ippnToUse)
{
    // Zero the socket list
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 2; c4Index++)
        m_apksockList[c4Index] = nullptr;

    Initialize(eProtocol);
}

TSocketListener::~TSocketListener()
{
    // Call our own cleanup, allow no exceptions to propogate
    try
    {
        Cleanup();
    }

    catch(TError& errToCatch)
    {
        if (facCIDSock().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TSocketListener: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return the count of listening sockets we have
tCIDLib::TCard4 TSocketListener::c4Count() const
{
    return m_c4Count;
}


tCIDLib::TCard4 TSocketListener::c4MaxWaiting() const
{
    return m_c4MaxWaiting;
}


// If we've been initailized, clean up back to unused state
tCIDLib::TVoid TSocketListener::Cleanup()
{
    //
    //  For all of the listening sockets we have, go through and close them.
    //  If they aren't open, this won't do anything, so it's safe to just
    //  call close on all fo them.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Count; c4Index++)
    {
        if (!m_apksockList[c4Index]->bClose())
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
        m_apksockList[c4Index] = nullptr;
    }
    m_c4Count = 0;
}


//
//  Return the socket we are set up to listen on. If they passed in zero, then this is
//  the actual port selected.
//
tCIDLib::TIPPortNum TSocketListener::ippnListenOn() const
{
    return m_ippnListenOn;
}


//
//  Waits up to the indicated time for a client to connect. It watches
//  for shutdown requests while waiting. If the wait is longer than 1 second
//  we'll wait in chunks and watch for shutdown requests.
//
TServerStreamSocket*
TSocketListener::psockListenFor(const   tCIDLib::TEncodedTime   enctWait
                                ,       TIPEndPoint&            ipepClient)
{
    // If we have no listening interfaces, then null now
    if (!m_c4Count)
        return 0;

    //
    //  Do a multi select on all of the sockets in our list to see if any
    //  of them have been signaled.
    //
    tCIDSock::EMSelFlags aeFlags[kCIDSock::c4MaxSelect];
    tCIDLib::TCard4 c4Count;

    // Wait for up to half a second at a time
    tCIDLib::TEncodedTime   enctCur = TKrnlTimeStamp::enctNow();
    tCIDLib::TEncodedTime   enctCurWait;
    tCIDLib::TEncodedTime   enctEnd;

    if (enctWait == kCIDLib::enctMaxWait)
        enctEnd = kCIDLib::enctMaxWait;
    else
        enctEnd = enctCur + enctWait;

    TThread* pthrCaller = nullptr;
    while (enctCur < enctEnd)
    {
        // Wait for a while
        if (enctCur + kCIDLib::enctHalfSecond > enctEnd)
            enctCurWait = enctEnd - enctCur;
        else
            enctCurWait = kCIDLib::enctHalfSecond;

        //
        //  Do a select for the current wait time. If we get an error back,
        //  then log if in verbose mode and return null.
        //
        c4Count = m_c4Count;
        if (!TKrnlSocket::bMultiSel(m_apksockList, aeFlags, c4Count, enctCurWait))
        {
            if (facCIDSock().bLogInfo())
            {
                facCIDSock().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kSockErrs::errcSock_MultiSelect
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppStatus
                );
            }
            return 0;
        }

        // If we got any hits, then break out
        if (c4Count)
            break;

        // Not yet, check for a shutdown request, and break out if so
        if (!pthrCaller)
            pthrCaller = TThread::pthrCaller();
        if (pthrCaller->bCheckShutdownRequest())
            break;

        enctCur = TKrnlTimeStamp::enctNow();
    }

    // Find the first one that's ready and return that guy
    TServerStreamSocket* psockRet = nullptr;
    if (c4Count)
    {
        TKrnlIPAddr         kipaClient;
        tCIDLib::TIPPortNum ippnClient;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Count; c4Index++)
        {
            if (tCIDLib::bAllBitsOn(aeFlags[c4Index], tCIDSock::EMSelFlags::Read))
            {
                //
                //  Accept the connection and fill in the new server socket
                //  handle with the new socket info. If we get an accept error,
                //  throw.
                //
                TSocketHandle hsockNew;
                if (!m_apksockList[c4Index]->bAccept(kipaClient, ippnClient, hsockNew))
                {
                    facCIDSock().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kSockErrs::errcSock_Accept
                        , TKrnlError::kerrLast()
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                    );
                }

                // Set up the return info and break out
                psockRet = new TServerStreamSocket(hsockNew);
                if (!TIPEndPoint::bIsNullObject(ipepClient))
                    ipepClient = TIPEndPoint(kipaClient, ippnClient);
                break;
            }
        }
    }
    return psockRet;
}


// Start up the listener if not already started
tCIDLib::TVoid
TSocketListener::Initialize(const   tCIDSock::ESockProtos   eProtocol
                            , const tCIDLib::TIPPortNum     ippnListenOn)
{
    // We can't already be initialized
    if (m_c4Count)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_ListAlreadyInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    // Store the port and call the other version
    m_ippnListenOn = ippnListenOn;
    Initialize(eProtocol);
}

tCIDLib::TVoid TSocketListener::Initialize(const tCIDSock::ESockProtos eProtocol)
{
    tCIDLib::TBoolean bRes;

    // We can't already be initialized
    if (m_c4Count)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_ListAlreadyInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    //
    //  For the available families, based on what is reported by the kernel
    //  IP stuff, we set up ANY addresses for them.
    //
    tCIDLib::TCard4 c4TryCnt = 0;
    TKrnlIPAddr akipaUse[2];
    if (TKrnlIP::bIPV4Avail())
    {
        akipaUse[c4TryCnt++].SetToSpecial
        (
            tCIDSock::ESpecAddrs::Any, tCIDSock::EAddrTypes::IPV4
        );
    }

    if (TKrnlIP::bIPV6Avail())
    {
        akipaUse[c4TryCnt++].SetToSpecial
        (
            tCIDSock::ESpecAddrs::Any, tCIDSock::EAddrTypes::IPV6
        );
    }

    CIDAssert(m_c4Count <= 2, L"Got more than 2 listening interfaces");

    // Loop through the ones we got and try to set them up
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TryCnt; c4Index++)
    {
        TKrnlSocket* pksockCur = new TKrnlSocket;
        bRes = pksockCur->bCreate
        (
            tCIDSock::ESocketTypes::Stream
            , eProtocol
            , akipaUse[c4Index].eType()
        );

        // Try to bind it if we created it
        if (bRes)
            bRes = pksockCur->bBindListen(akipaUse[c4Index], m_ippnListenOn);

        // And put it into listening mode if we bound
        if (bRes)
            bRes = pksockCur->bListen(m_c4MaxWaiting);


        // If it worked, keep it, else destroy it
        if (bRes)
        {
            m_apksockList[m_c4Count++] = pksockCur;

            //
            //  If the listening socket is zero, then get what we actually ended up on
            //  and use that for the rest of them, and return that as our listening
            //  port.
            //
            if (!m_ippnListenOn)
            {
                tCIDLib::TIPPortNum ippnUsed;
                TKrnlIPAddr         kipaUsed;
                if (pksockCur->bLocalEndPoint(ippnUsed, kipaUsed))
                {
                    m_ippnListenOn = ippnUsed;
                }
                 else
                {
                    CIDAssert2(L"The local server side ORB end point could not be obtained");
                }
            }
        }
         else
        {
            // If in verbose mode, log this
            if (facCIDSock().bLogInfo())
            {
                TKrnlString kstrAddr;
                if (!TKrnlIP::bTextFromIPAddr(akipaUse[c4Index], kstrAddr))
                    kstrAddr.Set(L"[unknown addr]");

                facCIDSock().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kSockErrs::errcSock_ListenerSetupErr
                    , tCIDLib::ESeverities::Info
                    , tCIDLib::EErrClasses::AppStatus
                    , TString(kstrAddr.pszValue())
                );
            }

            pksockCur->bClose();
            delete pksockCur;
        }
    }

    // If we couldn't bind to any interface, we failed
    if (!m_c4Count)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_NoListenIntf
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
            , TCardinal(m_ippnListenOn)
        );
    }

    CIDAssert(m_c4Count <= 2, L"Got more than 2 listening interfaces");
}


