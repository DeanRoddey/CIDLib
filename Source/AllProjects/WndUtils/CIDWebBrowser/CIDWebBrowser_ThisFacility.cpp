//
// FILE NAME: CIDWebBrowser_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#include    "CIDWebBrowser_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDWebBrowser,TGUIFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDWebBrowser_ThisFacility
{
}


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDWebBrowser
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDWebBrowser: Constructors and operators
// ---------------------------------------------------------------------------
TFacCIDWebBrowser::TFacCIDWebBrowser() :

    TGUIFacility
    (
        L"CIDWebBrowser"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDWebBrowser::~TFacCIDWebBrowser()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDWebBrowser: Public, non-virtual methods
// ---------------------------------------------------------------------------

