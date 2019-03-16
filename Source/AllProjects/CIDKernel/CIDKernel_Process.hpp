//
// FILE NAME: CIDKernel_Process.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/1998
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
//  This is the header for the CIDKernel_Process.Cpp file, which implements
//  the TKrnlProcess namespace. This is the kernel wrapper to access current
//  process information and control. There is not a lot that can be done to
//  the current process, since its mostly just a static bag which owns or
//  holds its various resources.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TKrnlProcess
{
    KRNLEXPORT tCIDLib::TBoolean bActivateProcess
    (
        const   tCIDLib::TProcessId     pidToActivate
    );

    KRNLEXPORT tCIDLib::TBoolean bCheckSingleInstanceInfo
    (
        const   tCIDLib::TCh* const     pszResName
        ,       tCIDLib::TBoolean&      bFound
        ,       TKrnlExtProcess* const  pkextpTarget
    );

    KRNLEXPORT tCIDLib::TBoolean bSetPriorityClass
    (
        const   tCIDLib::EPrioClasses   eClass
    );

    KRNLEXPORT tCIDLib::TBoolean bSetSingleInstanceInfo
    (
        const   tCIDLib::TCh* const     pszResName
        , const tCIDLib::TBoolean       bBringOldForward
        ,       tCIDLib::TBoolean&      bFoundOld
    );

    KRNLEXPORT tCIDLib::TVoid ExitProcess
    (
        const   tCIDLib::EExitCodes     eExitCode
    );

    KRNLEXPORT const TProcessHandle& hprocThis();

    KRNLEXPORT tCIDLib::TProcessId pidThis();
}


