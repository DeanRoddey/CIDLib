//
// FILE NAME: CIDSock_ListenEngine.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/10/2015
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
//  This file implements the TSockListenerEng class, which provides a standard
//  framework for servers to listen for client connections.
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
RTTIDecls(TSockLEngConn,TObject)
RTTIDecls(TSockListenerEng,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSockLEngConn
//  PREFIX: slec
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSockLEngConn: Constructors and Destructor
// ---------------------------------------------------------------------------
TSockLEngConn::TSockLEngConn(       TServerStreamSocket* const  psockAdopt
                            , const tCIDLib::TBoolean           bSecure
                            , const TIPEndPoint&                ipepClient) :
    m_bSecure(bSecure)
    , m_enctAt(TTime::enctNow())
    , m_ipepClient(ipepClient)
    , m_psockConn(psockAdopt)
{
}

TSockLEngConn::~TSockLEngConn()
{
    // If not orphaned, close it and delete it
    Cleanup();
}


// ---------------------------------------------------------------------------
//  TSockLEngConn: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSockLEngConn::bSecure() const
{
    return m_bSecure;
}

tCIDLib::TEncodedTime TSockLEngConn::enctAt() const
{
    return m_enctAt;
}

const TIPEndPoint& TSockLEngConn::ipepClient() const
{
    return m_ipepClient;
}

TServerStreamSocket* TSockLEngConn::psockConn()
{
    return m_psockConn;
}

tCIDLib::TVoid TSockLEngConn::Orphan()
{
    m_psockConn = nullptr;
}


// ---------------------------------------------------------------------------
//  TSockLEngConn: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is caleld to make sure that the socket has been cleaned up. If the caller has
//  not orphaned it, or closed it, then we need to do so.
//
tCIDLib::TVoid TSockLEngConn::Cleanup()
{
    if (m_psockConn)
    {
        try
        {
            if (m_psockConn->bIsOpen())
                m_psockConn->Close();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }

        try
        {
            delete m_psockConn;
        }

        catch(...)
        {
        }
        m_psockConn = nullptr;
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TSockListenerEng
//  PREFIX: sle
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSockListenerEng: Constructors and Destructor
// ---------------------------------------------------------------------------
TSockListenerEng::TSockListenerEng() :

    m_c4MaxWaiting(1)
    , m_colConnQ(tCIDLib::EAdoptOpts::Adopt, tCIDLib::EMTStates::Safe)
    , m_eProtocol(tCIDSock::ESockProtos::Count)
    , m_ippnNonSecure(0)
    , m_ippnSecure(0)
    , m_thrNonSecure
      (
        facCIDLib().strNextThreadName(TString(L"SockListEngNSec"))
        , TMemberFunc<TSockListenerEng>(this, &TSockListenerEng::eListenThread)
      )
    , m_thrSecure
      (
        facCIDLib().strNextThreadName(TString(L"SockListEngSec"))
        , TMemberFunc<TSockListenerEng>(this, &TSockListenerEng::eListenThread)
      )
{
}

TSockListenerEng::~TSockListenerEng()
{
}


// ---------------------------------------------------------------------------
//  TSockListenerEng: Public, non-virtual methods
// ---------------------------------------------------------------------------


tCIDLib::TCard4 TSockListenerEng::c4MaxWaiting() const
{
    return m_c4MaxWaiting;
}


tCIDLib::TVoid TSockListenerEng::Cleanup()
{
    // Stop the threads
    if (m_ippnNonSecure)
        m_thrNonSecure.ReqShutdownNoSync();

    if (m_ippnSecure)
        m_thrSecure.ReqShutdownNoSync();

    if (m_ippnNonSecure)
    {
        try
        {
            m_thrNonSecure.eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    if (m_ippnSecure)
    {
        try
        {
            m_thrSecure.eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    //
    //  If there are any connections in the list, flush them, which will close the
    //  sockets.
    //
    m_colConnQ.RemoveAll();
}


// Provide access to the configured secure and non-secure ports
tCIDLib::TIPPortNum TSockListenerEng::ippnNonSecure() const
{
    return m_ippnNonSecure;
}

tCIDLib::TIPPortNum TSockListenerEng::ippnSecure() const
{
    return m_ippnSecure;
}


//
//  This must be called after ctor to set up this object and start it processing.
//  Cleanup must be called before this can be called again.
//
tCIDLib::TVoid
TSockListenerEng::Initialize(const  tCIDSock::ESockProtos eProtocol
                            , const tCIDLib::TIPPortNum     ippnNonSecure
                            , const tCIDLib::TIPPortNum     ippnSecure
                            , const tCIDLib::TCard4         c4MaxWaiting)
{
    // If either thread is running, this can't be done
    if (m_thrNonSecure.bIsRunning() || m_thrSecure.bIsRunning())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcLEng_AlreadyRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    // If both ports are zero, then there's nothing to do. Log something if so
    if (!ippnNonSecure && !ippnSecure)
    {
        facCIDSock().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcLEng_NoPorts
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Init
        );
        return;
    }

    //
    //  Looks reasonable so store the data. Max waiting can be zero and the listener
    //  objects that the threads create will provide a default.
    //
    m_c4MaxWaiting = c4MaxWaiting;
    m_eProtocol = eProtocol;
    m_ippnNonSecure = ippnNonSecure;
    m_ippnSecure = ippnSecure;

    //
    //  Start up our threads as appropriate. We use the same thread function since
    //  they do the same thing. We just tell each thread instance which mode it is
    //  servicing.
    //
    tCIDLib::TBoolean bSecureMode;
    if (m_ippnNonSecure)
    {
        bSecureMode = kCIDLib::False;
        m_thrNonSecure.Start(&bSecureMode);
    }

    if (m_ippnSecure)
    {
        bSecureMode = kCIDLib::True;
        m_thrSecure.Start(&bSecureMode);
    }
}


//
//  Wait for up to the indicated length of time for a connection to show up in the
//  queue. Returns a null pointer if it times out.
//
TSockLEngConn* TSockListenerEng::pslecWait(const tCIDLib::TCard4 c4WaitMSs)
{
    // Wait up to the indicated type for a connection
    return m_colConnQ.pobjGetNext(c4WaitMSs, kCIDLib::False);
}



// ---------------------------------------------------------------------------
//  TSockListenerEng: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We start up one or two thread instances on this method, one fore each secure
//  and/or non-secure port.
//
tCIDLib::EExitCodes
TSockListenerEng::eListenThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    //
    //  Get the passed info, which tells us whether we are doing the secure or non-
    //  secure port.
    //
    const tCIDLib::TBoolean bSecure = *static_cast<tCIDLib::TBoolean*>(pData);

    // And now let the caller go
    thrThis.Sync();

    // Create our listener object
    const tCIDLib::TIPPortNum ippnListen = bSecure ? m_ippnSecure : m_ippnNonSecure;
    TSocketListener socklSrv(ippnListen, m_c4MaxWaiting);

    //
    //  Let's try to initialize the listener. This could fail if something else has
    //  our port in use, or perhaps there's no usable interfaces for that port right
    //  now. So we just keep trying periodically until we get it or are asked to
    //  shutdown.
    //
    const tCIDLib::TEncodedTime enctLogInterval = kCIDLib::enctOneMinute * 10;
    tCIDLib::TEncodedTime enctNextLog = 0;
    while(kCIDLib::True)
    {
        try
        {
            socklSrv.Initialize(m_eProtocol);

            // It worked, so break out
            break;
        }

        catch(TError& errToCatch)
        {
            if (facCIDSock().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        catch(...)
        {
        }

        // If we've not logged within the last log interval, then do so
        const tCIDLib::TEncodedTime enctCur = TTime::enctNow();
        if (enctCur > enctNextLog)
        {
            facCIDSock().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcLEng_InitFailed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(ippnListen)
            );

            // Reset the next log interval time
            enctNextLog = enctCur + enctLogInterval;
        }

        // Paus a while. If we are asked to shutdown, then just return
        if (!thrThis.bSleep(15000))
            return tCIDLib::EExitCodes::Normal;
    }


    //
    //  OK, we have the listener, so now we just wait for connections and queue
    //  them up when we get them.
    //
    TIPEndPoint ipepClient;
    while(kCIDLib::True)
    {
        if (thrThis.bCheckShutdownRequest())
            break;

        //
        //  Wait a while for a connection. This guy shouldn't throw any exceptions,
        //  but let's not take any chances.
        //
        TServerStreamSocket* psockNew = nullptr;
        try
        {
            psockNew = socklSrv.psockListenFor(kCIDLib::enctOneSecond, ipepClient);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            facCIDSock().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcLEng_ListenFailed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(ippnListen)
            );

            // Just in case, don't let us go wild, break out if asked to
            if (!thrThis.bSleep(500))
                break;
        }

        // If we got one, then queue it up
        if (psockNew)
        {
            TSockLEngConn* pslecNew = new TSockLEngConn(psockNew, bSecure, ipepClient);
            try
            {
                m_colConnQ.Add(pslecNew);
            }

            catch(TError& errToCatch)
            {
                //
                //  It will have been cleaned up since the queue is adopting. Even if
                //  it failed to store it, it will take care fo it.
                //
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);

                facCIDSock().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kSockErrs::errcLEng_CantQueue
                    , tCIDLib::ESeverities::Failed
                    , errToCatch.eClass()
                    , ipepClient
                    , TCardinal(ippnListen)
                );
            }
        }
    }
    return tCIDLib::EExitCodes::Normal;
}

