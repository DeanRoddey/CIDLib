//
// FILE NAME: CIDLib_Process.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/31/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDLib_Process.Cpp file. This file
//  implements the simple TProcess namespace. This namespace exports a couple
//  of simple methods to control the current process.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


class TExternalProcess;

namespace TProcess
{
    CIDLIBEXP tCIDLib::TBoolean bActivateProcess
    (
        const   TExternalProcess&       extpTarget
    );

    CIDLIBEXP tCIDLib::TBoolean bCheckSingleInstanceInfo
    (
        const   TResourceName&          rsnToUse
        ,       tCIDLib::TBoolean&      bFound
        ,       TExternalProcess* const pextpTarget = 0
    );

    CIDLIBEXP tCIDLib::TBoolean bSetSingleInstanceInfo
    (
        const   TResourceName&          rsnToUse
        , const tCIDLib::TBoolean       bBringOldForward
        , const tCIDLib::TBoolean       bThrowIfFail = kCIDLib::False
    );

    CIDLIBEXP tCIDLib::TVoid ExitProcess
    (
        const   tCIDLib::EExitCodes     eExit
    );

    CIDLIBEXP TProcessHandle hprocThis();

    CIDLIBEXP tCIDLib::TVoid SetPriorityClass
    (
        const   tCIDLib::EPrioClasses   eClass
    );

    CIDLIBEXP tCIDLib::TProcessId pidThis();

    CIDLIBEXP TString strProcessName();
}

