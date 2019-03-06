//
// FILE NAME: CIDLib_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main internal header for the facility. It includes the main
//  public header, any internal headers that are needed, intrafacility function
//  prototypes, intrafacility constants, and macros.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our own public header and any internal headers we need
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Include any intra-facility headers
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  This is the intra-facilities types namespace. It contains non-class types
//  for use within the facility.
// ---------------------------------------------------------------------------
namespace tCIDLib_
{
}


// ---------------------------------------------------------------------------
//  This is the intra-facilities constants namespace.
// ---------------------------------------------------------------------------
namespace kCIDLib_
{
    // -----------------------------------------------------------------------
    //  These are the names of environment variables that CIDLib looks for
    //  internally. This makes them easy to find and update.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszLocalLog = L"CID_LOCALLOG";


    // -----------------------------------------------------------------------
    //  There are some very fundamental places where hard coded strings must
    //  be used if things go awry during fundamental init.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszBadLocalLog      = L"The logger info (CID_LOCALLOG= or /LocalLog=) was badly formatted";
    const tCIDLib::TCh* const   pszErrCreatingLgr   = L"The indicated target logger could not be created";
    const tCIDLib::TCh* const   pszTitle1           = L"CIDLib C++ Frameworks";
    const tCIDLib::TCh* const   pszTitle2           = L"Copyright (c) Charmed Quark Software, 1992..2010";
    const tCIDLib::TCh* const   pszExceptDuringLog  = L"An exception occured while trying to log an error";
    const tCIDLib::TCh* const   pszRecursiveError   = L"A recursive error occured. See the error log.";
    const tCIDLib::TCh* const   pszStrLoadFailure   = L"Could not load system message. Id=%(1), Err=%(2)";
    const tCIDLib::TCh* const   pszFalse            = L"False";
    const tCIDLib::TCh* const   pszTrue             = L"True";
    const tCIDLib::TCh* const   pszUnhandledRTE     = L"Unhandled runtime error in thread %(1)";
    const tCIDLib::TCh* const   pszUnhandledSE      = L"Unhandled system error in thread %(1)";


    // -----------------------------------------------------------------------
    //  If the user does not provide an explicit expansion increment, then its
    //  calculated using the initial/max sizes. If that calcuation comes out
    //  larger than this value, then its clipped back to this value.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4MaxDefMemExpandInc = 8192;
}

