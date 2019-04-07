//
// FILE NAME: CIDAI_BTRandChoiceNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
//  This file implements the fundamental sequential node type.
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
#include    "CIDAI_BTRandChoiceNode_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTRandChoiceNode, TAIBTNode)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTRandChoiceNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TAIBTRandChoiceNode: Constructors and Destructor
// -------------------------------------------------------------------
TAIBTRandChoiceNode::TAIBTRandChoiceNode(const TString& strPath, const TString& strName) :

    TAIBTNode
    (
        strPath
        , strName
        , kCIDAI::strDefFact_RandChoice
        , 1
        , kCIDAI::c4MaxChildNodes
    )
    , m_prandSel(nullptr)
{
}

TAIBTRandChoiceNode::~TAIBTRandChoiceNode()
{
    // Clean up our random number generator if we created it
    delete m_prandSel;
}


// -------------------------------------------------------------------
//  TAIBTRandChoiceNode: Public, virtual methods
// -------------------------------------------------------------------
tCIDAI::ENodeStates TAIBTRandChoiceNode::eRun(TAIBehaviorTree& btreeOwner)
{
    // If we haven't created our random number generator yet, do that
    if (!m_prandSel)
    {
        m_prandSel = new TRandomNum();
        m_prandSel->Seed(TTime::c4Millis());
    }

    // Randomly select one of our children.
    const tCIDLib::TCard4 c4Count = c4ChildCount();
    tCIDLib::TCard4 c4SelInd = m_prandSel->c4GetNextNum() % c4Count;

    return eRunChildAt(btreeOwner, c4SelInd);
}

