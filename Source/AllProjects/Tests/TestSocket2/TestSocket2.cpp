//
// FILE NAME: TestSocket.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/10/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the main module for the program. It does some basic tests of the
//  TCP/IP socket classes. It provides both a client and server side, which
//  are started using command line parameters. This one is oriented towards
//  testing the TSocket::bMultiSelect() function, which allows a single
//  thread to watch for changes on multiple sockets and serve them all, as
//  apposed to the other alternative of one thread per client connection.
//
//  These parameters, and their defaults, are listed below:
//
//      CLIENT:
//          /Port=12000
//          /Times=128
//          /Interval=500
//          /Server=xxx
//
//      SERVER:
//          /Port=12000
//
//  The /Server=xxx parameter indicates the IP address or DNS name of the
//  server to connect to.
//
//  The /Times and /Interval parameters indicate how many times to send the
//  test message (from 1 to 64K) and how long to wait between attempts. This
//  lets us start up multiple clients spitting out messages at different
//  times.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes.
// ---------------------------------------------------------------------------
#include    "TestSocket2.hpp"



// ---------------------------------------------------------------------------
//  Global data
// ---------------------------------------------------------------------------
TFacTestSocket2                facTestSocket2;


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestSocket2MainThread"
        , TMemberFunc<TFacTestSocket2>(&facTestSocket2, &TFacTestSocket2::eMainThread)
    )
)


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TWorkPacket,TObject)
RTTIDecls(TFacTestSocket2,TFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacTestSocket2
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacTestSocket2: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacTestSocket2::TFacTestSocket2() :

    TFacility
    (
        L"TestSocket2"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::None
    )
    , m_bShutdown(kCIDLib::False)
    , m_c4Interval(500)
    , m_c4PLTimes(128)
    , m_c4ShutdownVal(kCIDLib::c4MaxCard)
    , m_colSelect
      (
        tCIDLib::EAdoptOpts::Adopt
        , kCIDSock::c4MaxSelect / 2
        , kCIDSock::c4MaxSelect
        , tCIDLib::EMTStates::Safe
      )
    , m_colWork(tCIDLib::EAdoptOpts::Adopt, tCIDLib::EMTStates::Safe)
    , m_eMode(EModes::Server)
    , m_ippnPort(12000)
    , m_thrListener
      (
        L"TestSocket2Listener"
        , TMemberFunc<TFacTestSocket2>(this, &TFacTestSocket2::eListenThread)
      )
    , m_thrWorker
      (
        L"TestSocket2Worker"
        , TMemberFunc<TFacTestSocket2>(this, &TFacTestSocket2::eWorkerThread)
      )
{
}

TFacTestSocket2::~TFacTestSocket2()
{
}


// ---------------------------------------------------------------------------
//  TFacCQCServer: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacTestSocket2::bHandleSignal(const tCIDLib::ESignals)
{
    m_bShutdown = kCIDLib::True;
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TFacTestSocket2: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes
TFacTestSocket2::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Set ourself as a signal handler
    TSignals::c4AddHandler(this);

    try
    {
        //
        //  Check the command line parameters, and store the results in the
        //  global parms.
        //
        tCIDLib::EExitCodes eRet = eParseParms();
        if (eRet != tCIDLib::EExitCodes::Normal)
            return eRet;

        // Show the settings
        m_conOut    << L"     Port=" << m_ippnPort << kCIDLib::NewLn
                    << L" Interval=" << m_c4Interval << kCIDLib::NewLn
                    << L"    Times=" << m_c4PLTimes << kCIDLib::NewLn
                    << L"   Server=" << m_strServer << kCIDLib::EndLn;

        //
        //  Now do what we need to do according to the mode that we're
        //  in.
        //
        if (m_eMode == EModes::Client)
            return eInvokeClient();
        else
            return eInvokeServer();
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        m_conOut << L"A CIDLib runtime error occured during processing.\n"
                 << L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        m_conOut << L"A kernel error occured during processing.\nError="
                 << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        m_conOut << L"A general exception occured during processing"
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


// ---------------------------------------------------------------------------
//  TFacTestSocket2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TFacTestSocket2::eInvokeClient()
{
    // Announce ourself
    m_conOut << kCIDLib::NewLn << L"Starting Client" << kCIDLib::EndLn;

    //
    //  Create a client stream socket, with a default local end point, and
    //  and the remote end point indicated by the user. We indicate no
    //  preference for the IPV4/6 type on the connection so we'll get whatever
    //  is preferred by the OS.
    //
    m_conOut << L"Creating the client socket" << kCIDLib::EndLn;
    TClientStreamSocket sockClient
    (
        tCIDSock::ESockProtos::TCP
        , TIPEndPoint(m_strServer, m_ippnPort, tCIDSock::EAddrTypes::Unspec)
    );

    // Say what end point we really got bound to on this side
    m_conOut    << L"Client bound to: " << sockClient.ipepLocalEndPoint()
                << kCIDLib::EndLn;

    // Get the starting time
    const tCIDLib::TEncodedTime enctStart = TTime::enctNow();

    // And send it as many times as indicated
    tCIDLib::TCard4 c4Reply;
    TIPEndPoint     ipepFrom;
    for (tCIDLib::TCard4 c4Count = 0; c4Count < m_c4PLTimes; c4Count++)
    {
        sockClient.Send(&c4Count, sizeof(c4Count));

        // Now wait for a message and see if it gets echoed back
        if (sockClient.c4ReceiveRawTOMS(&c4Reply, 1000, sizeof(c4Reply)) != sizeof(c4Reply))
        {
            m_conOut << L"Did not get a reply back" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        if (c4Reply != c4Count + 1)
        {
            m_conOut << L"The data recieve was not the same as those sent"
                     << kCIDLib::EndLn;
        }

        TThread::Sleep(m_c4Interval);
    }

    // Send a final 'shutdown' indicator of max value
    sockClient.Send(&m_c4ShutdownVal, sizeof(m_c4ShutdownVal));

    // Do a clean shutdown of the socket
    sockClient.c4Shutdown();

    // Get the ending time
    const tCIDLib::TEncodedTime enctEnd = TTime::enctNow();

    // And display the milliseconds ellapsed
    m_conOut << L"Completed successfully in "
             << ((enctEnd - enctStart) / 10000) << L" milliseconds" << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::EExitCodes TFacTestSocket2::eInvokeServer()
{
    // Announce ourself
    {
        TCritSecLocker lockOutput(&m_crsOutput);
        m_conOut << kCIDLib::NewLn << L"Starting Server" << kCIDLib::EndLn;
    }

    // Start up the listener and worker threads
    m_thrListener.Start();
    m_thrWorker.Start();

    //
    //  We put out a message every so many packets, just to make it clear that
    //  the server is doing something.
    //
    tCIDLib::TCard4 c4PacketCnt = 0;

    while (kCIDLib::True)
    {
        if (m_bShutdown)
        {
            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Asking listener thread to stop" << kCIDLib::EndLn;
            }

            try
            {
                m_thrListener.ReqShutdownSync();
                m_thrListener.eWaitForDeath(5000);
            }

            catch(const TError& errToCatch)
            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Failed to stop listener. Error=\n" << errToCatch
                         << kCIDLib::EndLn;
            }

            catch(...)
            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Failed to stop listener.\n" << kCIDLib::EndLn;
            }

            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Asking worker thread to stop" << kCIDLib::EndLn;
            }

            try
            {
                m_thrWorker.ReqShutdownSync();
                m_thrWorker.eWaitForDeath(5000);
            }

            catch(const TError& errToCatch)
            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Failed to stop worker. Error=\n" << errToCatch
                         << kCIDLib::EndLn;
            }

            catch(...)
            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Failed to stop worker thread\n" << kCIDLib::EndLn;
            }
            break;
        }

        //
        //  If we don't have any sockets yet, then just sleep a bit and
        //  try again in a bit.
        //
        if (m_colSelect.bIsEmpty())
        {
            TThread::Sleep(100);
            continue;
        }

        //
        //  We have some sockets, so do a multi-select on them and watch for any
        //  that have changed their read status. If none have, then jump back to
        //  the top.
        //
        if (!TStreamSocket::bMultiReadSel(m_colSelect, kCIDLib::enctHalfSecond))
            continue;

        //
        //  We got a change, so check them out. For the ones that have
        //  dropped, remove them from the list. For the ones with messages
        //  available, read them and respond.
        //
        TSelList::TNCCursor cursSocks(&m_colSelect);
        if (cursSocks.bReset())
        {
            do
            {
                if (!(cursSocks.objWCur().eFlags() & tCIDSock::EMSelFlags::Read))
                    continue;

                try
                {
                    // Get the socket as its actual type
                    TStreamSocket* psockCur = static_cast<TStreamSocket*>
                    (
                        cursSocks.objWCur().psockSel()
                    );

                    // Pull the data value off the socket
                    tCIDLib::TCard4 c4Data;
                    psockCur->c4ReceiveRawTOMS(&c4Data, 1000, sizeof(c4Data));

                    //
                    //  If it's the shutdown value, then that's a shutdown
                    //  indicator, so remove this one from the list.
                    //
                    if (c4Data == m_c4ShutdownVal)
                    {
                        TCritSecLocker lockOutput(&m_crsOutput);
                        m_conOut    << L"Client at "
                                    << psockCur->ipepRemoteEndPoint()
                                    << L" has completed"
                                    << kCIDLib::EndLn;
                        m_colSelect.RemoveAt(cursSocks);
                    }
                     else
                    {
                        // Queue up a work packet
                        m_colWork.Add(new TWorkPacket(psockCur, c4Data));

                        c4PacketCnt++;
                        if (!(c4PacketCnt % 32))
                        {
                            TCritSecLocker lockOutput(&m_crsOutput);
                            m_conOut << c4PacketCnt
                                     << L" packets processed so far"
                                     << kCIDLib::EndLn;
                        }
                    }
                }

                catch(const TError& errToCatch)
                {
                    TCritSecLocker lockOutput(&m_crsOutput);

                    // If not just a lost connection, dump the exception
                    if (!errToCatch.bCheckEvent(facCIDSock().strName()
                                                , kSockErrs::errcSock_NotConnected))
                    {
                        m_conOut << errToCatch << kCIDLib::EndLn;
                    }

                    m_conOut << L"Lost client connection" << kCIDLib::EndLn;
                    m_colSelect.RemoveAt(cursSocks);
                }
            }   while (cursSocks.bNext());
        }
    }

    // Destructor will cleanly shutdown the server socket
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::EExitCodes
TFacTestSocket2::eListenThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    {
        TCritSecLocker lockOutput(&m_crsOutput);
        m_conOut << L"Creating listening socket..." << kCIDLib::EndLn;
    }

    //
    //  Set up a listener on the local address and the defined server port.
    //  Defaults are to allow a the default backlog and to support loopback
    //  interfaces.
    //
    TSocketListener socklListener(m_ippnPort, tCIDSock::ESockProtos::TCP);

    {
        TCritSecLocker lockOutput(&m_crsOutput);
        m_conOut << L"Waiting for connections..." << kCIDLib::EndLn;
    }

    try
    {
        TIPEndPoint ipepClient;
        while (kCIDLib::True)
        {
            if (thrThis.bCheckShutdownRequest())
            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Listener thread exiting..." << kCIDLib::EndLn;
                break;
            }

            TServerStreamSocket* psockServer = socklListener.psockListenFor
            (
                kCIDLib::enctHalfSecond, ipepClient
            );

            if (psockServer)
            {
                TCritSecLocker lockOutput(&m_crsOutput);
                m_conOut << L"Connection from " << ipepClient << kCIDLib::EndLn;

                //
                //  Put this new socket into the list. Its a thread safe
                //  collection so we can just do this single op without any
                //  sync.
                //
                m_colSelect.Add(new TMSockSelItem(psockServer));
            }
        }
    }

    catch(const TError& errToCatch)
    {
        TCritSecLocker lockOutput(&m_crsOutput);
        m_conOut    << L"Exception in listener thread:\n   "
                    << errToCatch << kCIDLib::EndLn;
    }

    catch(...)
    {
        TCritSecLocker lockOutput(&m_crsOutput);
        m_conOut << L"Unknown exception in listener thread" << kCIDLib::EndLn;
    }
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::EExitCodes TFacTestSocket2::eParseParms()
{
    TString strParm;
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        strParm = *cursParms;

        if (strParm.bStartsWithI(L"/Times="))
        {
            strParm.Cut(0, 7);
            m_c4PLTimes = strParm.c4Val();
        }
         else if (strParm.bStartsWithI(L"/Server="))
        {
            strParm.Cut(0, 8);
            m_strServer = strParm;
            m_eMode = EModes::Client;
        }
         else if (strParm.bStartsWithI(L"/Port="))
        {
            strParm.Cut(0, 6);
            m_ippnPort = strParm.c4Val();
        }
         else if (strParm.bStartsWithI(L"/Interval="))
        {
            strParm.Cut(0, 10);
            m_c4Interval = strParm.c4Val();
        }
         else
        {
            m_conOut << L"Unknown parameter: " << strParm << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    // Make sure the server was set
    if ((m_eMode == EModes::Client) && m_strServer.bIsEmpty())
    {
        m_conOut << L"Client invocations require the /Server= parameter"
                 << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }
    return tCIDLib::EExitCodes::Normal;
}



tCIDLib::EExitCodes
TFacTestSocket2::eWorkerThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    {
        TCritSecLocker lockOutput(&m_crsOutput);
        m_conOut << L"Worker thread started..." << kCIDLib::EndLn;
    }

    while (kCIDLib::True)
    {
        try
        {
            if (thrThis.bCheckShutdownRequest())
                break;

            TWorkPacket* pwpackNew = 0;
            pwpackNew = m_colWork.pobjGetNext(200, kCIDLib::False);

            if (pwpackNew)
            {
                pwpackNew->m_c4Data++;
                pwpackNew->m_psockReply->Send(&pwpackNew->m_c4Data, sizeof(tCIDLib::TCard4));
            }
        }

        catch(const TError& errToCatch)
        {
            TCritSecLocker lockOutput(&m_crsOutput);
            m_conOut    << L"Exception in worker thread:\n   "
                        << errToCatch << kCIDLib::EndLn;
        }

        catch(...)
        {
            TCritSecLocker lockOutput(&m_crsOutput);
            m_conOut << L"Unknown exception in worker thread" << kCIDLib::EndLn;
        }
    }

    {
        TCritSecLocker lockOutput(&m_crsOutput);
        m_conOut << L"Worker thread done" << kCIDLib::EndLn;
    }
    return tCIDLib::EExitCodes::Normal;
}

