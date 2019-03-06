//
// FILE NAME: CIDSock_Pinger.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/10/2012
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TSockPinger class, which is a simple wrapper
//  around the low level ping protocol.
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
RTTIDecls(TSockPinger,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSockPinger
//  PREFIX: sping
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSockPinger: Constructors and Destructor
// ---------------------------------------------------------------------------
TSockPinger::TSockPinger()
{
}

TSockPinger::~TSockPinger()
{
}


// ---------------------------------------------------------------------------
//  TSockPinger: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We'll wait for a reply up to the indicated number of millis. We watch for
//  shutdown requests as we wait and break out with a timeout error if we get
//  a request to stop.
//
//  If we get a reply, we return the time required for the response, which the
//  underlying kernel object figures out for us.
//
tCIDLib::TBoolean
TSockPinger::bWaitReply(const   tCIDLib::TCard4     c4WaitMillis
                        ,       tCIDLib::TCard4&    c4RepMSs
                        ,       tCIDLib::TCard4&    c4RepBytes
                        ,       TIPAddress&         ipaFrom)
{
    // Make sure we are waiting for a reply
    if (m_kspingThis.eState() != tCIDSock::EPingStates::WaitReply)
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcPing_NotWaiting
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , m_strRemAddr
        );
    }

    // Calculate the end time
    tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    tCIDLib::TEncodedTime enctEnd = enctCur + (c4WaitMillis * kCIDLib::enctOneMilliSec);

    TKrnlIPAddr kipaFrom;
    tCIDSock::EPingRes eRes = tCIDSock::EPingRes::Timeout;
    TThread* pthrCaller = TThread::pthrCaller();
    while(enctCur < enctEnd)
    {
        tCIDLib::TCard4 c4Wait = tCIDLib::TCard4
        (
            (enctEnd - enctCur) / kCIDLib::enctOneMilliSec
        );

        if (!c4Wait)
            break;
        if (c4Wait > 250)
            c4Wait = 250;

        eRes = m_kspingThis.eWaitRep(c4Wait, c4RepMSs, c4RepBytes, kipaFrom);

        //
        //  If we timed out, then just keep waiting, else break out with error
        //  or success now.
        //
        if (eRes != tCIDSock::EPingRes::Timeout)
            break;

        // Watch for a shutdown request. If so, break out with the timeout result
        if (pthrCaller->bCheckShutdownRequest())
            break;

        enctCur = TTime::enctNow();
    }

    const tCIDLib::TBoolean bRet(eRes == tCIDSock::EPingRes::Success);
    if (bRet)
        ipaFrom = kipaFrom;

    return bRet;
}


// Cancel an outstanding wait for a reply
tCIDLib::TVoid TSockPinger::CancelReply(const tCIDLib::TBoolean bResetStamp)
{
    // Make sure we are initialized
    CheckReady();

    if (!m_kspingThis.bCancelReply(bResetStamp))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcPing_CancelWait
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strRemAddr
        );
    }
}


// Provide access to our pinger object's last time stamp
tCIDLib::TEncodedTime TSockPinger::enctLastTime() const
{
    return m_kspingThis.enctLastTime();
}


// Provide access to our pinger object's state
tCIDSock::EPingStates TSockPinger::eState() const
{
    return m_kspingThis.eState();
}


//
//  Clean up the pinger and put it back into a state that requires a call to
//  StartPing() again to start it bakc up.
//
tCIDLib::TVoid TSockPinger::EndPing()
{
    if (!m_kspingThis.bTerminate())
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcPing_End
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Cleanup the stored remote address name
    m_strRemAddr.Clear();
}


// Get the remote address current setup on this pinger, if any
const TString& TSockPinger::strRemAddr() const
{
    return m_strRemAddr;
}


//
//  Initialize us to ping a particular remote address. In order to get
//  use of the caching system, we resolve the name here and pass the
//  kernel level object the resolved address.
//
tCIDLib::TVoid
TSockPinger::StartPing( const   TString&                strRemAddr
                        , const tCIDSock::EAddrTypes    eType
                        ,       TIPAddress&             ipaTar)
{
    // Clean up any existing session
    EndPing();

    // Try to resolve the address
    TKrnlIPAddr kipaTar;
    tCIDSock::EAddrCvtRes eRes = facCIDSock().eResolveAddr(strRemAddr, kipaTar, eType);

    // If the resolution failed, or we can't initialize the pinger, then give up
    if ((eRes == tCIDSock::EAddrCvtRes::Failed) || !m_kspingThis.bInitialize(kipaTar))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcPing_Start
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRemAddr
        );
    }

    // It worked so give back the ip address
    ipaTar = kipaTar;

    // And store the remote address
    m_strRemAddr = strRemAddr;
}


// Send a request out
tCIDLib::TVoid TSockPinger::SendRequest()
{
    // Make sure we are initialized
    CheckReady();

    if (!m_kspingThis.bSendRequest())
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcPing_SendReq
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strRemAddr
        );
    }
}


// ---------------------------------------------------------------------------
//  TSockPinger: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Called by some of the methods, to make sure that this object has been
//  set up for pinging.
//
tCIDLib::TVoid TSockPinger::CheckReady()
{
    if (m_kspingThis.eState() == tCIDSock::EPingStates::WaitInit)
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcPing_NotReady
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
}

