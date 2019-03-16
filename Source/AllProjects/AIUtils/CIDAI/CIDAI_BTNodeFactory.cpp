//
// FILE NAME: CIDAI_BTNodeFactory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2016
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
//  This file implements the abstract base class from which all BT node factories are
//  derived.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDAI_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTNodeFact, TObject)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTNodeFact
//  PREFIX: nfact
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBTNodeFact: Public, static methods
// ---------------------------------------------------------------------------

// For keyed collection supports
const TString& TAIBTNodeFact::strKey(const TAIBTNodeFact& nfactSrc)
{
    return nfactSrc.m_strFactKey;
}


// ---------------------------------------------------------------------------
//  TAIBTNodeFact: Constructors and Destructor
// ---------------------------------------------------------------------------

// Nothing to do at this level
TAIBTNodeFact::~TAIBTNodeFact()
{
}


// ---------------------------------------------------------------------------
//  TAIBTNodeFact: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We make this non-virtual and call a virtual protected on the derived class. This
//  way we can intercept all newly created nodes and do whatever we need.
//
TAIBTNode*
TAIBTNodeFact::pbtnodeMakeNew(  const   TString&            strPath
                                , const TString&            strName
                                , const TString&            strType
                                , const tCIDLib::TBoolean   bFlag)
{
    TAIBTNode* pbtnodeRet = pbtnodeNew(strPath, strName, strType);

    // Store the flag on the node if we created it
    if (pbtnodeRet)
        pbtnodeRet->bFlag(bFlag);

    return pbtnodeRet;
}


// Provide access to the factory key for this factory
const TString& TAIBTNodeFact::strFactKey() const
{
    return m_strFactKey;
}


// ---------------------------------------------------------------------------
//  TAIBTNodeFact: Hidden constructors
// ---------------------------------------------------------------------------
TAIBTNodeFact::TAIBTNodeFact(const TString& strFactKey) :

    m_strFactKey(strFactKey)
{
}

