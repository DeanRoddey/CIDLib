//
// FILE NAME: CIDSChan_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/11/2104
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
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
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
#include    "CIDSChan_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDSChan,TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDSChan
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDSChan: Constructors and Destructors
// ---------------------------------------------------------------------------
TFacCIDSChan::TFacCIDSChan() :

    TFacility
    (
        L"CIDSChan"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDSChan::~TFacCIDSChan()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDSChan: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method will test a certificate info string and see if it is valid and break
//  out the parts. The possible forms are:
//
//      UStore:[store], [name]
//          [type]  -   From current user store
//          [store] -   The store name to load from
//          [name]  -   The CN of the certificate to look for.
//
//      MStore:[store], [name]
//          [type]  -   From local machine store
//          [store] -   The store name to load from.
//          [name]  -   The CN of the certificate to look for.
//
//      File:[path], [name]
//          [type]  -   From a file
//          [store] -   The path to the certificate file to load
//          [name]  -   The CN of the certificate to look for

//
tCIDLib::TBoolean
TFacCIDSChan::bParseCertInfo(const  TString&    strToParse
                            ,       TString&    strType
                            ,       TString&    strStore
                            ,       TString&    strName)
{
    strType = strToParse;
    if (!strType.bSplit(strStore, L':'))
        return kCIDLib::False;

    strType.StripWhitespace();
    strStore.StripWhitespace();
    strType.ToLower();

    // Now, based on the type, deal with the other bits
    tCIDLib::TBoolean bRes = kCIDLib::False;

    // Break out the name
    if (strStore.bSplit(strName, L','))
    {
        strStore.StripWhitespace();
        strName.StripWhitespace();

        // Neither the store nor the name can be empty
        bRes = !strStore.bIsEmpty() && !strName.bIsEmpty();
    }

    return bRes;
}


