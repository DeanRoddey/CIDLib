//
// FILE NAME: TestSocket.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/10/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  are started using command line parameters. It basic blasts a payload of
//  numbers at the server and expects it to send them back modified by
//  bumping them up by one.
//
//  These parameters, and their defaults, are listed below:
//
//      CLIENT:
//          /Port=12000
//          /Type=StreamClient|DatagramClient
//          /Size=64
//          /Times=128
//          /Server=xxx
//          /AddrType=4|6
//
//      SERVER:
//          /Port=12000
//          /Type=StreamServer|DatagramServer
//          /Size=64
//
//          (Datagram only)
//          /AddrType=4|6
//
//
//  The /Server=xxx parameter indicates the IP address or DNS name of the
//  server to connect to.
//
//  The /Times and /Size parameters indicate the size of the test payload
//  to send, and how many times to send it. The size must be from 1 to 64K.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDSock, which brings in all we need.
// ---------------------------------------------------------------------------
#include    "CIDSock.hpp"


// ---------------------------------------------------------------------------
//  Local types
// ---------------------------------------------------------------------------
enum EModes
{
    EMode_StreamServer
    , EMode_DatagramServer
    , EMode_StreamClient
    , EMode_DatagramClient
};


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Local data
//
//  c4PLSize
//  c4PLTimes
//      The size of the data buffer to send, and the number of times to
//      send it. The sizes must be from 1 to 64K.
//
//  conOut
//      Our output console for this program. Consoles are specializations
//      of the output text stream, TTextOutStream.
//
//  eMode
//      Indicates the mode that we are playing in this instance. It is set
//      by the /Type=xxx parameter.
//
//  ippnPort
//      For the server modes, this is the port to listen on. For the clients,
//      this is the port to connect to. Defaults to 12000 if not provided.
//
//  strServer
//      The server to connect to. Means nothing for server side.
// ---------------------------------------------------------------------------
static const tCIDLib::TCard4    c4MaxPLSize = 0x10000;
static tCIDLib::TCard4          c4PLSize = 64;
static tCIDLib::TCard4          c4PLTimes = 128;
static TOutConsole              conOut;
static tCIDSock::EAddrTypes     eSrvAddrType = tCIDSock::EAddrTypes::IPV4;

static tCIDLib::TBoolean        bBindToLoopbacks = kCIDLib::False;
static EModes                   eMode;
static tCIDLib::TIPPortNum      ippnPort = 12000;
static TString                  strServer;


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestTCPIP", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  These are the functions that get invoked according to the mode that
//  this instance is running in.
//
static tCIDLib::EExitCodes eInvokeDatagramClient()
{
    // Announce ourself
    conOut << kCIDLib::NewLn << L"Starting TCPIP Datagram Client" << kCIDLib::EndLn;

    // Create an end point for the server we want to talk to
    TIPEndPoint ipepServer(strServer, ippnPort, eSrvAddrType);

    // And create our client side socket. No binding required for a client
    TClientDatagramSocket sockClient(tCIDSock::ESockProtos::UDP, ipepServer.eAddrType());

    //
    //  Allocate our test buffer, which the command line provided the
    //  size for.
    //
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4PLSize];

    // Get the starting time
    const tCIDLib::TEncodedTime enctStart = TTime::enctNow();

    // And send it as many times as indicated
    tCIDLib::TCard4 c4BufInd;
    tCIDLib::TCard4 c4RetCount;
    TIPEndPoint     ipepFrom;
    for (tCIDLib::TCard4 c4Count = 0; c4Count < c4PLTimes; c4Count++)
    {
        // Set up the send buffer
        for (c4BufInd = 0; c4BufInd < c4PLSize; c4BufInd++)
            pc1Buf[c4BufInd] = tCIDLib::TCard1(c4BufInd);

        c4RetCount = sockClient.c4SendTo(ipepServer, pc1Buf, c4PLSize);

        if (c4RetCount != c4PLSize)
        {
            conOut << L"Did not send all bytes!" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        // Now wait for a message and see if it gets echoed back
        for (c4BufInd = 0; c4BufInd < c4PLSize; c4BufInd++)
            pc1Buf[c4BufInd] = 0xFF;
        c4RetCount = sockClient.c4ReceiveRawFrom
        (
            ipepFrom, pc1Buf, kCIDLib::enctOneSecond, c4PLSize
        );

        if (c4RetCount != c4PLSize)
        {
            conOut  << L"Got " << c4RetCount << L" byte back. Needed "
                    << c4PLSize << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        // Check the return to all be bumped up one
        for (c4BufInd = 0; c4BufInd < c4PLSize; c4BufInd++)
        {
            if (pc1Buf[c4BufInd] != tCIDLib::TCard1(c4BufInd + 1))
            {
                conOut  << L"Expected " << tCIDLib::TCard1(c4BufInd + 1)
                        << L" but got " << pc1Buf[c4BufInd]
                        << L", at index " << c4BufInd
                        << kCIDLib::EndLn;
                break;
            }
        }
    }

    // Get the ending time
    const tCIDLib::TEncodedTime enctEnd = TTime::enctNow();

    // And display the milliseconds ellapsed
    conOut  << L"Completed successfully in "
            << ((enctEnd - enctStart) / 10000) << L" milliseconds" << kCIDLib::EndLn;
    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::EExitCodes eInvokeStreamClient()
{
    // Announce ourself
    conOut << kCIDLib::NewLn << L"Starting TCPIP Stream Client" << kCIDLib::EndLn;

    if (TStatsCache::c8CheckValue(kCIDSock::pszStat_Net_OpenSockCnt))
        conOut << L"!!There should be no open sockets now" << kCIDLib::EndLn;

    //
    //  Create a client stream socket, with a default local end point, and
    //  and the remote end point indicated by the user.
    //
    conOut << L"Creating the client socket" << kCIDLib::EndLn;

    TClientStreamSocket sockClient
    (
        tCIDSock::ESockProtos::TCP
        , TIPEndPoint(strServer, ippnPort, eSrvAddrType)
    );

    if (TStatsCache::c8CheckValue(kCIDSock::pszStat_Net_OpenSockCnt) != 1)
        conOut << L"!!There should be one open socket now" << kCIDLib::EndLn;

    // Say what end point we really got bound to on this side
    conOut  << L"Client bound to: " << sockClient.ipepLocalEndPoint() << kCIDLib::EndLn;

    //
    //  Allocate our test buffers, which the command line provided the
    //  size for.
    //
    tCIDLib::TCard1* pc1OutBuf = new tCIDLib::TCard1[c4PLSize];
    tCIDLib::TCard1* pc1InBuf = new tCIDLib::TCard1[c4PLSize];

    // Get the starting time
    const tCIDLib::TEncodedTime enctStart = TTime::enctNow();

    // And send it as many times as indicated
    tCIDLib::TCard4 c4BufInd;
    tCIDLib::TCard4 c4RetCount;
    TIPEndPoint     ipepFrom;
    for (tCIDLib::TCard4 c4Count = 0; c4Count < c4PLTimes; c4Count++)
    {
        // Set up the send buffer
        for (c4BufInd = 0; c4BufInd < c4PLSize; c4BufInd++)
            pc1OutBuf[c4BufInd] = tCIDLib::TCard1(c4BufInd);

        sockClient.Send(pc1OutBuf, c4PLSize);

        // Now wait for a message and see if it gets echoed back
        for (c4BufInd = 0; c4BufInd < c4PLSize; c4BufInd++)
            pc1InBuf[c4BufInd] = 0xFF;
        c4RetCount = 0;
        while (c4RetCount < c4PLSize)
        {
            const tCIDLib::TCard4 c4Bytes = sockClient.c4ReceiveRaw
            (
                &pc1InBuf[c4RetCount], c4PLSize - c4RetCount
            );
            c4RetCount += c4Bytes;
        }

        if (c4RetCount != c4PLSize)
        {
            conOut  << L"Got " << c4RetCount << L" byte back. Needed "
                    << c4PLSize << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        // Check the return to all be bumped up one
        for (c4BufInd = 0; c4BufInd < c4PLSize; c4BufInd++)
        {
            if (pc1InBuf[c4BufInd] != tCIDLib::TCard1((c4BufInd + 1)))
            {
                conOut  << L"The data recieve was not the same as those sent. "
                        << L"First deviation at index " << c4BufInd
                        << kCIDLib::EndLn;
                break;
            }
        }
    }

    // Get the ending time
    const tCIDLib::TEncodedTime enctEnd = TTime::enctNow();

    // And display the milliseconds ellapsed
    conOut  << L"Completed successfully in "
            << ((enctEnd - enctStart) / 10000) << L" milliseconds" << kCIDLib::EndLn;

    sockClient.Close();

    if (TStatsCache::c8CheckValue(kCIDSock::pszStat_Net_OpenSockCnt))
        conOut << L"!!All sockets hsould be closd now" << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::EExitCodes eInvokeDatagramServer()
{
    // Announce ourself
    conOut << kCIDLib::NewLn << L"Starting TCPIP Datagram Server" << kCIDLib::EndLn;

    //
    //  Set up a listening socket on the workstation address and the
    //  defined server port.
    //
    conOut << L"Creating server socket..." << kCIDLib::EndLn;

    // Listen on all interfaces in the address mode requested
    TIPEndPoint ipepListen(tCIDSock::ESpecAddrs::Any, eSrvAddrType, ippnPort);
    TServerDatagramSocket sockServer(tCIDSock::ESockProtos::UDP, ipepListen);

    //
    //  Allocate our test buffer. We just make it the max size that the
    //  program allows.
    //
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4MaxPLSize];

    TIPEndPoint ipepFrom;
    while (kCIDLib::True)
    {
        try
        {
            while (kCIDLib::True)
            {
                const tCIDLib::TCard4 c4Got = sockServer.c4ReceiveRawFrom
                (
                    ipepFrom, pc1Buf, kCIDLib::enctMaxWait, c4PLSize
                );

                if (!c4Got)
                {
                    conOut << L"Got no bytes on c4ReceiveFrom() call" << kCIDLib::EndLn;
                    break;
                }

                //
                //  Update the contents by bumping up each value by
                //  one.
                //
                for (tCIDLib::TCard4 c4BufInd = 0; c4BufInd < c4Got; c4BufInd++)
                    pc1Buf[c4BufInd]++;

                const tCIDLib::TCard4 c4Sent = sockServer.c4SendTo
                (
                    ipepFrom, pc1Buf, c4Got
                );

                if (c4Got != c4Sent)
                {
                    conOut << L"Sent " << c4Sent << L" bytes, but "
                           << c4Got << L" were received" << kCIDLib::EndLn;
                }
            }
        }

        catch(const TError& errToCatch)
        {
            conOut << L"Exception=\n" << errToCatch << kCIDLib::EndLn;
        }
    }

    // Destructor will cleanly shutdown the server socket
    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::EExitCodes eInvokeStreamServer()
{
    // Announce ourself
    conOut << kCIDLib::NewLn << L"Starting TCPIP Stream Server" << kCIDLib::EndLn;

    //
    //  Set up a listener on, with a small backlog. Initialize it for TCP
    //  and tell it to include loopback addresses.
    //
    conOut << L"Creating listening socket..." << kCIDLib::EndLn;

    TSocketListener socklListener(ippnPort, 2);
    socklListener.Initialize(tCIDSock::ESockProtos::TCP);

    conOut  << L"Listening on " << socklListener.c4Count() << L" interfaces."
            << kCIDLib::EndLn;

    //
    //  Allocate our test buffer, which the command line provided the
    //  size for.
    //
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4PLSize];

    while (kCIDLib::True)
    {
        // This will block until we get a connection
        conOut << L"Waiting for connection..." << kCIDLib::EndLn;
        TIPEndPoint ipepClient;
        TServerStreamSocket* psockServer = socklListener.psockListenFor
        (
            kCIDLib::enctMaxWait, ipepClient
        );
        TJanitor<TServerStreamSocket> janSocket(psockServer);
        conOut << L"Connection from " << ipepClient << kCIDLib::EndLn;

        tCIDLib::TCard4 c4Got;
        try
        {
            while (kCIDLib::True)
            {
                //
                //  Read until we get the correct number of bytes, but don't
                //  read more than that.
                //
                c4Got = 0;
                while (c4Got < c4PLSize)
                {
                    const tCIDLib::TCard4 c4Bytes = psockServer->c4ReceiveRaw
                    (
                        &pc1Buf[c4Got], c4PLSize - c4Got
                    );
                    c4Got += c4Bytes;
                }

                if (!c4Got)
                {
                    conOut << L"Got no bytes on c4Receive() call" << kCIDLib::EndLn;
                    break;
                }

                //
                //  Update the contents by bumping up each value by
                //  one.
                //
                if (!c4Got)
                {
                    conOut << L"Didn't get any items" << kCIDLib::EndLn;
                    break;
                }

                for (tCIDLib::TCard4 c4BufInd = 0; c4BufInd < c4Got; c4BufInd++)
                    pc1Buf[c4BufInd]++;

                // Echo the message back
                psockServer->Send(pc1Buf, c4Got);
            }
        }

        catch(TError& errToCatch)
        {
            // If it was just a disconnect, then continue. Otherwise, rethrow
            if (!errToCatch.bCheckEvent(facCIDSock().strName()
                                        , kSockErrs::errcSock_NotConnected))
            {
                conOut << L"Exception occurred=\n" << errToCatch << kCIDLib::EndLn;
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            conOut << L"Client disconnected" << kCIDLib::EndLn;
        }

        // The janitor will close down the current socket here
    }

    // Destructor will cleanly shutdown the listening socket
    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::EExitCodes eParseParms()
{

    TString strParm;
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        strParm = *cursParms;

        if (strParm.bStartsWithI(L"/Type="))
        {
            strParm.Cut(0, 6);

            if (strParm.bCompareI(L"StreamServer"))
                eMode = EMode_StreamServer;
            else if (strParm.bCompareI(L"DatagramServer"))
                eMode = EMode_DatagramServer;
            else if (strParm.bCompareI(L"StreamClient"))
                eMode = EMode_StreamClient;
            else if (strParm.bCompareI(L"DatagramClient"))
                eMode = EMode_DatagramClient;
            else
            {
                conOut << L"/Type=[StreamServer|StreamClient|"
                          L"DatagramServer|DatagramClient]\n"
                       << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }
        }
         else if (strParm.bStartsWithI(L"/Size="))
        {
            strParm.Cut(0, 6);
            c4PLSize = strParm.c4Val();

            if (!c4PLSize || (c4PLSize > 0x10000))
            {
                conOut << L"/Size= must be >= 1 <= 64K" << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }
        }
         else if (strParm.bStartsWithI(L"/Times="))
        {
            strParm.Cut(0, 7);
            c4PLTimes = strParm.c4Val();
        }
         else if (strParm.bStartsWithI(L"/Server="))
        {
            strParm.Cut(0, 8);
            strServer = strParm;
        }
         else if (strParm.bStartsWithI(L"/Port="))
        {
            strParm.Cut(0, 6);
            ippnPort = strParm.c4Val();
        }
         else if (strParm.bStartsWithI(L"/AddrType="))
        {
            strParm.Cut(0, 10);
            if (strParm == "4")
                eSrvAddrType = tCIDSock::EAddrTypes::IPV4;
            else if (strParm == "6")
                eSrvAddrType = tCIDSock::EAddrTypes::IPV6;
            else
            {
                conOut << L"Unknown server address type" << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }
        }
         else
        {
            conOut << L"Unknown parameter: " << strParm << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    // Make sure the server was set
    if (((eMode == EMode_DatagramClient)
    ||  (eMode == EMode_StreamClient))
    &&  strServer.bIsEmpty())
    {
        conOut << L"Client invocations require the /Server= parameter" << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }

    return tCIDLib::EExitCodes::Normal;
}



//
//  This is the the thread function for the main thread. It is started by
//  the CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

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
        conOut  << L"  Port=" << ippnPort << kCIDLib::NewLn
                << L"  Size=" << c4PLSize << kCIDLib::NewLn
                << L" Times=" << c4PLTimes << kCIDLib::NewLn
                << L"Server=" << strServer << kCIDLib::NewEndLn;

        //
        //  Now do what we need to do according to the mode that we're
        //  in.
        //
        if (eMode == EMode_StreamClient)
            return eInvokeStreamClient();
        else if (eMode == EMode_StreamServer)
            return eInvokeStreamServer();
        else if (eMode == EMode_DatagramClient)
            return eInvokeDatagramClient();
        else if (eMode == EMode_DatagramServer)
            return eInvokeDatagramServer();
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut << L"A CIDLib runtime error occured during processing.\n\n"
                 << errToCatch << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"A kernel error occured during processing.\nError="
                 << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing"
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}
