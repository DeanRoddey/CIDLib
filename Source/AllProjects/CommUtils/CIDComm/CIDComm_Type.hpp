//
// FILE NAME: CIDComm_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file is the main types file of the facility. It contains all of
//  the non-class types of the facility.
//
// CAVEATS/GOTCHAS:
//
//  1)  Some of the fundamental types are required to exist at the CIDKernel
//      level, for the low level platform support of comm ports. We only
//      provide the other stuff here.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  The facility types namespace
//
//  Most of these are in CIDKernel_Types.hpp so that these can be shared by
//  the CIDKernel wrappers that we access this stuff through.
// ---------------------------------------------------------------------------
namespace tCIDComm
{
    // -----------------------------------------------------------------------
    //  The two serial port types
    // -----------------------------------------------------------------------
    enum class EPortTypes
    {
        RS232
        , RS422

        , Count
        , Min       = RS232
        , Max       = RS422
    };


    CIDCOMMEXP const TString& strXlatEDataBits
    (
        const   tCIDComm::EDataBits     eToXlat
    );

    CIDCOMMEXP const TString& strXlatEParities
    (
        const   tCIDComm::EParities     eToXlat
    );

    CIDCOMMEXP const TString& strXlatEPortDTR
    (
        const   tCIDComm::EPortDTR      eToXlat
    );

    CIDCOMMEXP const TString& strXlatEPortRTS
    (
        const   tCIDComm::EPortRTS      eToXlat
    );

    CIDCOMMEXP const TString& strXlatEStopBits
    (
        const   tCIDComm::EStopBits     eToXlat
    );
}


// ---------------------------------------------------------------------------
//  Make some of our enums streamable
// ---------------------------------------------------------------------------
EnumStreamMacros(tCIDComm::EErrors)
EnumStreamMacros(tCIDComm::EDataBits)
EnumStreamMacros(tCIDComm::EOpenFlags)
EnumStreamMacros(tCIDComm::EParities)
EnumStreamMacros(tCIDComm::EPortDTR)
EnumStreamMacros(tCIDComm::EPortRTS)
EnumStreamMacros(tCIDComm::EPortTypes)
EnumStreamMacros(tCIDComm::EStopBits)

EnumBinStreamMacros(tCIDComm::EFlags)

StdEnumTricks(tCIDComm::EDataBits)
StdEnumTricks(tCIDComm::EParities)
StdEnumTricks(tCIDComm::EPortDTR)
StdEnumTricks(tCIDComm::EPortRTS)
StdEnumTricks(tCIDComm::EStopBits)

BmpEnumTricks(tCIDComm::EErrors)
BmpEnumTricks(tCIDComm::EFlags)
