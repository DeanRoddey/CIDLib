//
// FILE NAME: CIDWebSock_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements TFacCIDWebSock class.
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
#include    "CIDWebSock_.hpp"




// ---------------------------------------------------------------------------
//  TFacCIDWebSock: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDWebSock::TFacCIDWebSock() :

    TFacility
    (
        L"CIDWebSock"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDWebSock::~TFacCIDWebSock()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDWebSock: Public, non-virtual methods
// ---------------------------------------------------------------------------

