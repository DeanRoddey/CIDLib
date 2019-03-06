//
// FILE NAME: CIDLib_USB.hpp
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
//  This is the header for the file CIDLib_USB.cpp file, which implements
//  the TUSBDev namespace. This guy provides some helper methods for dealing
//  with USB devices.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   NAMESPACE: TUSBDev
// ---------------------------------------------------------------------------
namespace TUSBDev
{
    CIDLIBEXP tCIDLib::TBoolean bCheckForDevice
    (
        const   TString&            strIntfID
        , const tCIDLib::TCard2     c2VendorId
        , const tCIDLib::TCard2     c2ProductId
    );

    CIDLIBEXP tCIDLib::TBoolean bFindHIDDev
    (
        const   tCIDLib::TCard2     c2VendorId
        , const tCIDLib::TCard2     c2ProductId
        ,       TString&            strToFill
    );
}

#pragma CIDLIB_POPPACK


