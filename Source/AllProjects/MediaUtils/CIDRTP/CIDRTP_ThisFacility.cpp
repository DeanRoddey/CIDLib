//
// FILE NAME: CIDRTP_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
//  This file implements the facility class for this facility.
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
#include    "CIDRTP_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDRTP,TFacility)



// ---------------------------------------------------------------------------
//  TFacCIDRTP: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDRTP::TFacCIDRTP() :

    TFacility
    (
        L"CIDRTP"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
    //
    //  Get our current time stamp, shift it down 11 bits and keep the low 32 bits.
    //  Creation of this facility object is synchronized, so we don't have to sync
    //  our seeding here.
    //
    tCIDLib::TCard4 c4Seed = tCIDLib::TCard4(TTime::enctNow() >> 11);
    m_randGenId.Seed(c4Seed);
}

TFacCIDRTP::~TFacCIDRTP()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDRTP: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method will generate a random sync src id. It will be returned in the
//  transmission byte order, so it doesn't have to be flipped.
//
tCIDLib::TCard4 TFacCIDRTP::c4GenSyncSrcId()
{
    tCIDLib::TCard4 c4Ret;

    // Sync access while we do this
    {
        TLocker lockrSync(&m_mtxSync);
        c4Ret = m_randGenId.c4GetNextNum();
    }

    // If we are little endian, then flip it
    #if defined(CIDLIB_LITTLEENDIAN)
    c4Ret = TRawBits::c4SwapBytes(c4Ret);
    #endif

    return c4Ret;
}



