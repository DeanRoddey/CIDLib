//
// FILE NAME: CIDKernel_USB.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_USB.cpp file, which provides some
//  common functionality for interfacing with USB devices. Most of the methods
//  are provided by the per-platform layer.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TKrnlUSBDev
{
    KRNLEXPORT tCIDLib::TBoolean bCheckForDevice
    (
        const   tCIDLib::TCh* const pszDevId
        , const tCIDLib::TCard2     c2VendorId
        , const tCIDLib::TCard2     c2ProductId
        ,       tCIDLib::TBoolean&  bFound
    );

    KRNLEXPORT tCIDLib::TBoolean bFindHIDDev
    (
        const   tCIDLib::TCard2     c2VendorId
        , const tCIDLib::TCard2     c2ProductId
        ,       tCIDLib::TCh* const pszToFill
        , const tCIDLib::TCard4     c4MaxChars
        ,       tCIDLib::TBoolean&  bFound
    );
};


