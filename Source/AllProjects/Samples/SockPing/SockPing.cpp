//
// FILE NAME: SockPing.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2000
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
//  This is the main (only) module of the SockPing demo program. This program
//  demonstrates the SockPinger class, which implements the standard ICMP
//  ping protocol. You just provide a target host.
//
//
//  Like most of the basic samples, this one doesn't create a facility object, it
//  just starts a main thread on a local
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDSock.hpp"


// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"SockPingMainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local helper methods
// ----------------------------------------------------------------------------
static void ShowUsage(TTextOutStream& strmOut)
{
    strmOut << L"Usage:\n  SockPing tarhost-or-addr \n\n"
            << L"    Provide a host target to ping\n"
            << kCIDLib::EndLn;
}


// ----------------------------------------------------------------------------
//  Program entry point
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // We'll just use the standard output
    TTextOutStream& strmOut = TSysInfo::strmOut();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Output a little program blurb
        strmOut << L"\nSockPing.Exe\n"
                << L"CIDLib Socket Ping Demo\n" << kCIDLib::EndLn;

        // The caller must provide us with a target host to ping
        TString strHost;
        if (TSysInfo::c4CmdLineParmCount() != 1)
        {
            ShowUsage(strmOut);
            return tCIDLib::EExitCodes::BadParameters;
        }
        strHost = TSysInfo::strCmdLineParmAt(0);

        // Create a pinger object
        TSockPinger spingTest;

        //
        //  Initialize it for the target host. He'll give us back an IP
        //  address object for us to use for display purposes.
        //
        TIPAddress ipaTar;
        spingTest.StartPing(strHost, tCIDSock::EAddrTypes::Unspec, ipaTar);

        // Output what we are doing
        strmOut << L"\n   Pinging address " << strHost
                << L"  (" << ipaTar << L")"
                << kCIDLib::NewEndLn;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 3; c4Index++)
        {
            // Send a request
            spingTest.SendRequest();

            // Wait up to ten seconds for a reply
            tCIDLib::TCard4 c4RepBytes;
            tCIDLib::TCard4 c4RepMSs;
            if (spingTest.bWaitReply(10000, c4RepMSs, c4RepBytes, ipaTar))
            {
                strmOut << L"  Got a response. Time=" << c4RepMSs
                        << L"ms  Bytes=" << c4RepBytes
                        << L"  From=" << ipaTar;
            }
             else
            {
                strmOut << L"  Did not get a response";
            }
            strmOut << kCIDLib::EndLn;

            // Sleep a bit before the next round
            TThread::Sleep(1000);
        }

        // Clean it up
        spingTest.EndPing();
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        strmOut << L"A CIDLib runtime error occured during processing.\n  Error: "
                << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        strmOut << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}


