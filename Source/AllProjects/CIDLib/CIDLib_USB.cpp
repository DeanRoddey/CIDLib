//
// FILE NAME: CIDLib_USB.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2004
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
//  This file implements the core USB helper methods namespace
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  USBDev methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TUSBDev::bCheckForDevice(const  TString&        strIntfID
                        , const tCIDLib::TCard2 c2VendId
                        , const tCIDLib::TCard2 c2ProdId)
{
    tCIDLib::TBoolean bFound;
    if (!TKrnlUSBDev::bCheckForDevice(strIntfID.pszBuffer(), c2VendId, c2ProdId, bFound))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcUSB_FindHIDDevice
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return bFound;
}


tCIDLib::TBoolean
TUSBDev::bFindHIDDev(const  tCIDLib::TCard2     c2VendId
                    , const tCIDLib::TCard2     c2ProdId
                    ,       TString&            strToFill)
{
    const tCIDLib::TCard4 c4BufSz = 2048;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    tCIDLib::TBoolean bFound;
    if (!TKrnlUSBDev::bFindHIDDev(c2VendId, c2ProdId, achBuf, c4BufSz, bFound))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcUSB_FindHIDDevice
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // It worked, so set the caller's string
    if (bFound)
        strToFill = achBuf;
    return bFound;
}

