//
// FILE NAME: CIDAudStream_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#include    "CIDAudStream_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDAudStream,TFacility)



// ---------------------------------------------------------------------------
//  TFacCIDAudStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDAudStream::TFacCIDAudStream() :

    TFacility
    (
        L"CIDAudStream"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDAudStream::~TFacCIDAudStream()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDAudStream: Public, non-virtual methods
// ---------------------------------------------------------------------------

