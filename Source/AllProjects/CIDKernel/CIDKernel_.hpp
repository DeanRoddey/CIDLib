//
// FILE NAME: CIDKernel_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1992
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
//  This is the main internal header for the facility. It includes the main
//  public header, and any internal ones that are needed. It also includes
//  the system headers, which are only used internally, via the
//  CIDKernel_PlatformInternal_.hpp file. This file comes from the per-
//  platform directory and insures that the correct host headers are
//  included for a particular host.
//
//  Also included here is platform independent part of the internal constants
//  and types namespaces. Any platform dependent additions to these
//  namespaces can be brought in via the CIDKernel_PlatformInternal_.hpp
//  header.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Bring in our own public header and any internal headers we need to
//  have visible only internally.
// ---------------------------------------------------------------------------
#include    "CIDKernel.hpp"
#include    "CIDKernel_MessageIds.hpp"



// ---------------------------------------------------------------------------
//  Bring in the per-platform internals header. This will bring in the
//  system headers that are needed within the kernel, plus any other types
//  or definitions that are required internally for all per-platform
//  implementation files.
// ---------------------------------------------------------------------------
#include    "CIDKernel_PlatformInternal_.hpp"


// ---------------------------------------------------------------------------
//  The internal constants namespaces
// ---------------------------------------------------------------------------
namespace kCIDKernel_
{
    // -----------------------------------------------------------------------
    //  These are hard coded defaults for the text in the low level popups
    //  that are used if fundamental initialization goes awry. If the
    //  translatable text cannot be loaded (or if the error occurs before it
    //  could be done), then these are backups.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszDefTitle1    = L"CIDLib Kernel Abstraction Layer";
    const tCIDLib::TCh* const   pszDefTitle2    = L"Copyright (c) CIDLib 1992..2010";


    // -----------------------------------------------------------------------
    //  Defaults for the low level init/term strings. If these cannot be
    //  loaded from message text, these are used.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszKrnlInitErr  = L"A kernel exception occured during facility initialization. Subsystem=";
    const tCIDLib::TCh* const   pszKrnlTermErr  = L"A kernel exception occured during facility termination. Subsystem=";
    const tCIDLib::TCh* const   pszSysInitErr   = L"A system exception occured during facility initialization. Subsystem=";
    const tCIDLib::TCh* const   pszSysTermErr   = L"A system exception occured during facility termination. Subsystem=";
    const tCIDLib::TCh* const   pszModInitErr   = L"Could not initialize the kernel facility object";


    // -----------------------------------------------------------------------
    //  Some low level messages that get displayed if text cannot be loaded.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszMsgNotFound  = L"Could not find message";
    const tCIDLib::TCh* const   pszResNotFound  = L"Could not find string resource";
    const tCIDLib::TCh* const   pszNoMsgFile    = L"The module's message file could not be loaded, locale messages not available";
    const tCIDLib::TCh* const   pszUnknownErr   = L"An unknown error occured";
}



// ---------------------------------------------------------------------------
//  The internal types namespaces
// ---------------------------------------------------------------------------
namespace tCIDKernel_
{
    // -----------------------------------------------------------------------
    //  This is a small structure used to map host errors to our errors.
    // -----------------------------------------------------------------------
    struct TErrMap
    {
        tCIDLib::TOSErrCode     errcHostErr;
        tCIDLib::TErrCode       errcCIDErr;
    };
}

