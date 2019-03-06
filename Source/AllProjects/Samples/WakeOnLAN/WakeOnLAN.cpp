//
// FILE NAME: WakeOnLAN.cpp
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
//  This is the main (only) module of the WakeOnLAN demo program. This program
//  implements a WOL transmission. You just provide a MAC address on the
//  command line.
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
//  Forward references
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eMainThreadFunc
(
    TThread&            thrThis
    , tCIDLib::TVoid*   pData
);



// ----------------------------------------------------------------------------
//  Local data
//
//  conIn
//  conOut
//      This is an interactive program, so we create an input and output
//      console to do our interaction with the user. We create the input
//      console so that it supports recall and editing, with a recall buffer
//      of 16 elements.
// ----------------------------------------------------------------------------
static TInConsole   conIn(kCIDLib::True, 16);
static TOutConsole  conOut;



// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"SendWOLMainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local helper methods
// ----------------------------------------------------------------------------
static void ShowUsage()
{
    conOut  << L"Usage:\n  WakeOnLAN macaddr\n\n"
            << L"    The address can be in the forms:\n"
            << L"       0030BD037444\n"
            << L"       00:30:BD:03:74:44\n"
            << L"       00-30-BD-03-74-44\n"
            << kCIDLib::EndLn;
}


// ----------------------------------------------------------------------------
//  Program entry point
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
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
        conOut  << L"\nWakeOnLAN.Exe\n"
                << L"CIDLib WOL Demo\n" << kCIDLib::EndLn;

        //
        //  The caller must provide us with the mac address, so we have to at
        //  least have one parameter.
        //
        TString strMACAddr;
        if (TSysInfo::c4CmdLineParmCount() != 1)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }
        strMACAddr = TSysInfo::strCmdLineParmAt(0);

        //
        //  Create a datagram socket, enable broadasts and send the WOL. This
        //  is an IPV4 thing only, at least in broadcast form, so we force an
        //  IPV4 socket type.
        //
        TClientDatagramSocket sockClient
        (
            tCIDSock::ESockProtos::UDP, tCIDSock::EAddrTypes::IPV4
        );
        sockClient.bAllowBroadcast(kCIDLib::True);
        sockClient.SendWOLBroadcast(strMACAddr);
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut  << L"A CIDLib runtime error occured during processing.\n  Error: "
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
        conOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}


