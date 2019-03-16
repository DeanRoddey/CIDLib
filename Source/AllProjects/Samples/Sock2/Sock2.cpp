//
// FILE NAME: Sock2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2000
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//  This is the main (only) module of the Sock2 demo program. This program
//  demonstrates some very simple use of the socket classes. It will do a
//  standard 'echo' transaction with a server. This is similar to the 'ping'
//  command but its handled in a different way. Many servers will respond to
//  any UDP datagram on port 7 by just echoing the data back.
//
//  This program just loops 3 times and sends 64 bytes with particular values.
//  Each time it reads the values back and checks them. If it they come back
//  the same, we know it worked.
//
//
//  Like most of the basic samples, this one doesn't create a facility object, it
//  just starts a main thread on a local
//
// CAVEATS/GOTCHAS:
//
//  1)  The mechanisms for measuring the response times here are very gross
//      level and shouldn't be taken too seriously.
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
//  Local data
//
//  strmOut
//      We don't need any interactivity for this program so we just set up
//      an output text stream, which writes to the standard output. Use the
//      default text encoding for this platform.
// ----------------------------------------------------------------------------
static TTextFileOutStream strmOut(tCIDLib::EStdFiles::StdOut);



// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread.
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Sock2MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Output a little program blurb
        strmOut << L"\nSock2.Exe\n"
                << L"CIDLib Socket Demo #2\n" << kCIDLib::EndLn;

        //
        //  We require one parameter, which is the address of the server. If
        //  its not there, then complain and bail, else get it into a string.
        //
        if (TSysInfo::c4CmdLineParmCount() != 1)
        {
            strmOut << L"Usage:\n   Sock2 srvaddr\n" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }
        TString strSrvAddr = TSysInfo::strCmdLineParmAt(0);

        //
        //  Create an end point for the echo protocol. We pass the server
        //  address string and the well known port number, which is 7 for the
        //  echo service.
        //
        TIPEndPoint ipepEcho(strSrvAddr, 7, tCIDSock::EAddrTypes::Unspec);

        //
        //  Create a datagram socket with the UDP protocol. Use the address
        //  type that we got above in the address resolution.
        //
        TClientDatagramSocket sockClient
        (
            tCIDSock::ESockProtos::UDP, ipepEcho.eAddrType()
        );


        // Announce what we are doing
        strmOut << L"Echoing to " << strSrvAddr << L"("
                << ipepEcho << L")" << kCIDLib::EndLn;

        //
        //  Set up our little dummy message payload. Its just 64 bytes that
        //  are numbered consequtively.
        //
        const tCIDLib::TCard4 c4BufSize = 64;
        tCIDLib::TCard1 ac1Buf[c4BufSize];
        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < c4BufSize; c4Index++)
            ac1Buf[c4Index] = tCIDLib::TCard1(c4Index);

        //
        //  Loop three times and do the transaction each time around. We
        //  do a gross level measurement of the round trip time just for fun,
        //  though the resolution of the timing methods on any particular
        //  platform are iffy.
        //
        tCIDLib::TCard1 ac1RetBuf[c4BufSize];
        TIPEndPoint     ipepFrom;
        for (tCIDLib::TCard4 c4LoopCount = 0; c4LoopCount < 3; c4LoopCount++)
        {
            // Announce each packet that we send
            strmOut << L"Sending " << c4BufSize << L" bytes..." << kCIDLib::EndLn;

            //
            //  Now get a start time stamp and send the data gram to the
            //  echo end point.
            //
            TTime tmStart(tCIDLib::ESpecialTimes::CurrentTime);
            tCIDLib::TCard4 c4Count = sockClient.c4SendTo(ipepEcho, ac1Buf, c4BufSize);

            // If we didn't send all the bytes, then give up
            if (c4Count != c4BufSize)
            {
                strmOut << L"Failed to send message to server\n" << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::FatalError;
            }

            //
            //
            //  Now ask for the data back. We just read it into a raw local
            //  array. After we return, get an ending time stamp.
            //
            c4Count = sockClient.c4ReceiveRawFrom
            (
                ipepFrom, ac1RetBuf, kCIDLib::enctOneSecond, c4BufSize
            );
            TTime tmEnd(tCIDLib::ESpecialTimes::CurrentTime);

            // If it failed, then say how many bytes we got, then bail out
            if (c4Count != c4BufSize)
            {
                if (c4Count)
                {
                    strmOut << L"Got " << c4Count
                            << L" bytes, but expected to get "
                            << c4BufSize << kCIDLib::EndLn;
                }
                 else
                {
                    strmOut << L"Timed out waiting for the response" << kCIDLib::EndLn;
                }
                return tCIDLib::EExitCodes::FatalError;
            }

            // Check the values we got back. If bad, say so and get out
            if (!TRawMem::bCompareMemBuf(ac1Buf, ac1RetBuf, c4BufSize))
            {
                strmOut << L"Got back bad data\n" << kCIDLib::EndLn;
                tCIDLib::EExitCodes::FatalError;
            }

            //
            //  It worked, so calculate the milliseconds. This value is a float
            //  so we cast it to an unsigned before we display, to throw away
            //  the partial milliseconds.
            //
            strmOut << L"  Got reply in "
                    << tCIDLib::TCard4(tmEnd.enctMilliSeconds() - tmStart.enctMilliSeconds())
                    << L" milliseconds" << kCIDLib::EndLn;
        }
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
