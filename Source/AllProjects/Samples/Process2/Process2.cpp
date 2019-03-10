//
// FILE NAME: Process2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1997
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
//  This is the main module for the second process control oriented demo
//  programs. This program demonstrates the execution of another process
//  and waiting for it to die. It just executes the Process1 program and
//  waits for it to die before exiting.
//
//
//  As with most of these basic samples, this guy does not create a facility
//  object, it just starts up a thread on a local function.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
//
//  We don't have a main header of our own for this simple program, so just
//  include the CIDLib facility header, which is the only one we need.
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ----------------------------------------------------------------------------
//  Local static data
//
//  strmOut
//      For this program, instead of using the usual TOutConsole class, we
//      use a text file output stream, writing to the standard output. This
//      does not allow for interactivity like a console does, but we don't
//      need any here, so its a useful and lower overhead option.
// ----------------------------------------------------------------------------
static TTextFileOutStream   strmOut(tCIDLib::EStdFiles::StdOut);


// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Process2MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

// The main thread is started here
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Start up the worker thread and just wait for it to die. Indicate
    //  what we are doing to the user.
    //
    strmOut << kCIDLib::NewLn << L"Starting Process1 program..." << kCIDLib::EndLn;

    //
    //  Create an external process object. We just let it inherit our
    //  environment, so all we have to provide is the program name.
    //
    TExternalProcess extpToRun;

    // And now start it up
    strmOut << L"Waiting for Process1 to die..." << kCIDLib::EndLn;
    extpToRun.Start(L"Process1.Exe", TString::strEmpty());
    tCIDLib::EExitCodes eExit = extpToRun.eWaitForDeath();

    // Display the exit code
    strmOut << kCIDLib::NewLn << L"Process1 returned exit code: "
            << eExit << kCIDLib::NewLn << kCIDLib::EndLn;

    //
    //  Get the process times and display them. The start/end times are
    //  actual times. The kernel/user times are just amounts of time
    //  spent, so we want to format them a little differently from the
    //  default.
    //
    if (eExit == tCIDLib::EExitCodes::Normal)
    {
        strmOut << L"Process Times:" << kCIDLib::NewLn;
        TTime tmStart, tmEnd, tmKernel, tmUser;
        extpToRun.QueryProcessTimes(tmStart, tmEnd, tmKernel, tmUser);
        strmOut << L"         Start: " << tmStart << kCIDLib::NewLn
                << L"           End: " << tmEnd << kCIDLib::NewLn
                << L" Kernel(msecs): " << tmKernel.enctMilliSeconds() << kCIDLib::NewLn
                << L"   User(msecs): " << tmUser.enctMilliSeconds() << kCIDLib::EndLn;
    }

    return tCIDLib::EExitCodes::Normal;
}
