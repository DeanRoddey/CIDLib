//
// FILE NAME: ShellTester.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/01/2003
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"ShellTesterMainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Get the first parm, which is the number of milliseconds to wait
    //  when we get asked to shut down. And the second is the name of the
    //  event we should block on.
    //
    tCIDLib::TCard4 c4WaitMillis = TSysInfo::strCmdLineParmAt(0).c4Val();

    // Create the event and block on it
    TEvent evWait
    (
        TResourceName(L"CharmedQuark", L"AppShellTester", TSysInfo::strCmdLineParmAt(1))
        , tCIDLib::EEventStates::Reset
        , tCIDLib::ECreateActs::CreateIfNew
        , kCIDLib::False
    );
    evWait.WaitFor();

    // Wait for the indicated number of millis, then exit
    TThread::Sleep(c4WaitMillis);

    return tCIDLib::EExitCodes::Normal;
}
