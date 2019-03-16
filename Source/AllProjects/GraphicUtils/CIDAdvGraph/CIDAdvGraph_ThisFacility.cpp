//
// FILE NAME: CIDAdvGraph_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
#include    "CIDAdvGraph_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDAdvGraph,TFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDAdvGraph_ThisFacility
{
};



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDAdvGraph
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDAdvGraph: Constructors and operators
// ---------------------------------------------------------------------------
TFacCIDAdvGraph::TFacCIDAdvGraph() :

    TFacility
    (
        L"CIDAdvGraph"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDAdvGraph::~TFacCIDAdvGraph()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDAdvGraph: Public, static methods
// ---------------------------------------------------------------------------

