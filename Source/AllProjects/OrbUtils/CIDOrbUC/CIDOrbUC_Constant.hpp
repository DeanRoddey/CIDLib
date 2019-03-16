//
// FILE NAME: CIDOrbC_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
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
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  The constants namespace for this facility
// ---------------------------------------------------------------------------
namespace kCIDOrbUC
{
    // -----------------------------------------------------------------------
    //  Name server oriented contants
    //
    //  ippnDefLogSrvport
    //  ippnDefNSPort
    //      The ports on which the name and log servers listen by default.
    //
    //  pszNSInstanceId
    //      The name server must have a fixed instance id, because it must be
    //      findable by everyone, so that they can then find everything else.
    // -----------------------------------------------------------------------
    const tCIDLib::TIPPortNum   ippnNameSrvDefPort  = 13502;
    const tCIDLib::TIPPortNum   ippnLogSrvDefPort   = 13503;
    const tCIDLib::TIPPortNum   ippnCfgSrvDefPort   = 13504;
    const tCIDLib::TCh* const   pszNSInstanceId     = L"C7D1445942EA2EAA-0EE3FF7875548D09";


    // -----------------------------------------------------------------------
    //  Logging server related constants
    //
    //  c8LogFlRolloverSz
    //  c8LogFlReopenSz
    //      The log server logger will use local fallback files if it cannot
    //      get to the log server. This is the size it uses as a rollover
    //      threshold, at which point it creates a new local file. When a
    //      process starts up, and finds an existing most recent file, it
    //      will start using that file if its less than the re-open size. Else,
    //      it will just move on to a new file.
    //
    //  pszXXXX
    //      Some convenience constants for some standard name server scopes
    //      and bindings, to keep people from having to use hard coded strings.
    //      Some of them are going to be registered with their host name in
    //      the path, because it is necessary for them to be contacted by other
    //      hosts, i.e. they don't always (though usually) talk to their own
    //      local server. %h represents the host.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard8       c8LogFlRolloverSz       = 128 * 1024;
    const tCIDLib::TCard8       c8LogFlReopenSz         = 96 * 1024;


    // -----------------------------------------------------------------------
    //  These are the names of some named events that are used by our standard
    //  servers. The event resource name is always BlockEvent, and the others
    //  are the app name part of the resource name. These are used by the
    //  servers to block indefinitely on and by other processes to ask them
    //  to exit.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszEvCfgSrv             = L"CIDCfgSrv";
    const tCIDLib::TCh* const   pszEvLogSrv             = L"CIDLogSrv";
    const tCIDLib::TCh* const   pszEvNameSrv            = L"CIDNameSrv";
    const tCIDLib::TCh* const   pszEvResource           = L"BlockEvent";
}

#pragma CIDLIB_POPPACK

