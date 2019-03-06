//
// FILE NAME: CIDSIP_Agent.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TSIPAgent class.
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
RTTIDecls(TSIPAgent,TObject)


// ---------------------------------------------------------------------------
//  Local data, types, and constants
// ---------------------------------------------------------------------------
namespace CIDNet_SIPAgent
{
}



// ---------------------------------------------------------------------------
//   CLASS: TSIPAgent
//  PREFIX: sipag
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSIPAgent: Destructor
// ---------------------------------------------------------------------------
TSIPAgent::TSIPAgent(const  tCIDLib::ECSSides   eSide
                    , const TString&            strLocAddr
                    , const TString&            strLocContact
                    , const TString&            strLocDisplay
                    , const TString&            strCallID) :

    m_eSide(eSide)
    , m_strCallID(strCallID)
    , m_strLocAddr(strLocAddr)
    , m_strLocContact(strLocContact)
    , m_strLocDisplay(strLocDisplay)
{
    // Generate our local tag
    m_strLocTag = TUniqueId::strMakeId();
}

TSIPAgent::~TSIPAgent()
{
}


// ---------------------------------------------------------------------------
//  TSIPAgent: Public, non-virtual methods
// ---------------------------------------------------------------------------






// ---------------------------------------------------------------------------
//  TSIPAgent: Private, non-virtual methods
// ---------------------------------------------------------------------------



