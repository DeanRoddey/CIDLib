//
// FILE NAME: CIDSIP_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/05/2013
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
//  This file implements TFacCIDSIP class.
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
#include    "CIDSIP_.hpp"



// ---------------------------------------------------------------------------
//  TFacCIDSIP: Public, static data
// ---------------------------------------------------------------------------
const TString    TFacCIDSIP::strMeth_Ack("ACK");
const TString    TFacCIDSIP::strMeth_Bye("BYE");
const TString    TFacCIDSIP::strMeth_Invite("INVITE");
const TString    TFacCIDSIP::strMeth_OK("OK");
const TString    TFacCIDSIP::strMeth_Ringing("RINGING");



// ---------------------------------------------------------------------------
//  TFacCIDSIP: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDSIP::TFacCIDSIP() :

    TFacility
    (
        L"CIDSIP"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDSIP::~TFacCIDSIP()
{
}


