//
// FILE NAME: CIDAI_BTSeqNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
#include    "CIDAI_BTSeqNode_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTSeqNode, TAIBTNode)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTSeqNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TAIBTSeqNode: Constructors and Destructor
// -------------------------------------------------------------------
TAIBTSeqNode::TAIBTSeqNode(const TString& strPath, const TString& strName) :

    TAIBTNode
    (
        strPath
        , strName
        , kCIDAI::strDefFact_Sequence
        , 1
        , kCIDAI::c4MaxChildNodes
    )
{
}

TAIBTSeqNode::~TAIBTSeqNode()
{
}


// -------------------------------------------------------------------
//  TAIBTSeqNode: Public, virtual methods
// -------------------------------------------------------------------
tCIDAI::ENodeStates TAIBTSeqNode::eRun(TAIBehaviorTree& btreeOwner)
{
    // Run our children in order
    const tCIDLib::TCard4 c4Count = c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        tCIDAI::ENodeStates eCurState = eRunChildAt(btreeOwner, c4Index);
        if (eCurState != tCIDAI::ENodeStates::Success)
            return eCurState;
    }
    return tCIDAI::ENodeStates::Success;
}

