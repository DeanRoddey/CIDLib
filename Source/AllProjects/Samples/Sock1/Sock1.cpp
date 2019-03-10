//
// FILE NAME: Sock1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2000
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
//  This is the main (only) module of the Sock1 demo program. This program
//  demonstrates some very simple use of the socket classes. It will do a
//  standard socket query of a server's time. You just provide the server name
//  or TCP/IP address of the server on the command line and it will create a
//  datagram socket, read the date back from the server, and display it.
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
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDSock, which will bring in all we need.
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
//  Do the magic main module code
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Sock1MainThread", eMainThreadFunc))



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
        strmOut << L"\nSock1.Exe\n"
                << L"CIDLib Socket Demo #1\n" << kCIDLib::EndLn;

        //
        //  We require two parameters. The first is the address of the server.,
        //  and the second indicates whether TCP or UDP should be used.
        //
        if (TSysInfo::c4CmdLineParmCount() != 2)
        {
            strmOut << L"Usage:\n   Sock1 srvaddr [TCP|UDP]\n" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }
        TString strSrvAddr = TSysInfo::strCmdLineParmAt(0);
        TString strProtocol = TSysInfo::strCmdLineParmAt(1);

        //
        //  Create an end point for the daytime protocol. We pass the server
        //  address string and the well known port number. We have to insure
        //  this one is an IPV4 address.
        //
        TIPEndPoint ipepDayTime(strSrvAddr, 13, tCIDSock::EAddrTypes::IPV4);

        //
        //  Depending on the protocol, create the socket, do the write,
        //  and get the response.
        //
        THeapBuf mbufRead(1024, 1024);

        tCIDLib::TCard1 c1Dummy = 0;
        tCIDLib::TCard4 c4Count = 0;
        strProtocol.ToUpper();
        TIPEndPoint ipepFrom;

        if (strProtocol == L"UDP")
        {
            TClientDatagramSocket sockSrv
            (
                tCIDSock::ESockProtos::UDP, ipepDayTime.eAddrType()
            );

            //
            //  Now lets send a data gram packet to the server. It can have any
            //  payload since the server doesn't care. It just responds to any
            //  datagram with a string. So we just send a zero byte.
            //
            sockSrv.c4SendTo(ipepDayTime, &c1Dummy, 1);

            //
            //  And now we should expect a string back. We create a heap buffer
            //  object of 1K and make sure its fully allocated (by setting the
            //  initial and max sizes both to 1K.) Then we provide that as the
            //  target for the read. We are using a millisecond timeout, so we
            //  use the 'MS' vresion.
            //
            c4Count = sockSrv.c4ReceiveMBufFrom
            (
                ipepFrom, mbufRead, kCIDLib::enctFiveSeconds, 1024
            );
        }
         else if (strProtocol == L"TCP")
        {
            TClientStreamSocket sockSrv(tCIDSock::ESockProtos::TCP, ipepDayTime);
            ipepFrom = sockSrv.ipepRemoteEndPoint();
            c4Count = sockSrv.c4ReceiveMBufTO
            (
                mbufRead, kCIDLib::enctTenSeconds, 1024, tCIDLib::EAllData::OkIfNotAll
            );
        }
         else
        {
            strmOut << L"Unknown protocol type\n" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  If we got something back, display it. Else, complain that we didn't
        //  get any response.
        //
        if (c4Count)
        {
            // Output the end point info we got
            strmOut << L"Recieved msg from: " << ipepFrom << kCIDLib::EndLn;

            //
            //  We know we got back a buffer of ASCII chars, but we have to
            //  transcode that before we can display it. So, lets create a
            //  memory buffer based text input stream and install a US-ASCII
            //  transcoder on it.
            //
            //  We give it the count of bytes we read as the initial end of
            //  stream position. This will allow us to read that many bytes
            //  before getting an end of stream exception.
            //
            TTextMBufInStream strmTmp
            (
                &mbufRead, c4Count, tCIDLib::EAdoptOpts::NoAdopt, new TUSASCIIConverter
            );

            // Lets read a line of text from it and print it out
            TString strTmp;
            strmTmp.c4GetLine(strTmp);
            strmOut << L"         Response: " << strTmp << kCIDLib::EndLn;
        }
         else
        {
            strmOut << L"Did not get any data from the query\n" << kCIDLib::EndLn;
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
