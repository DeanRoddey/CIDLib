//
// FILE NAME: CIDSIP_CAgent.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2013
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
//  This file implements the client agent class class.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSIPCAgent,TSIPAgent)



// ---------------------------------------------------------------------------
//   CLASS: TSIPCAgent
//  PREFIX: sipcag
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSIPAgent: Destructor
// ---------------------------------------------------------------------------

//
//  Pass on the provided info to the base class. Tell him we are client side
//  and provide him with a call id since we are the client side and have to
//  provide it.
//
TSIPCAgent::TSIPCAgent( const   TString&    strLocAddr
                        , const TString&    strLocContact
                        , const TString&    strLocDisplay) :

    TSIPAgent
    (
        tCIDLib::ECSSides::Client
        , strLocAddr
        , strLocContact
        , strLocDisplay
        , TUniqueId::strMakeId()
    )
{
}

TSIPCAgent::~TSIPCAgent()
{
}


// ---------------------------------------------------------------------------
//  TSIPCAgent: Public, non-virtual methods
// ---------------------------------------------------------------------------


