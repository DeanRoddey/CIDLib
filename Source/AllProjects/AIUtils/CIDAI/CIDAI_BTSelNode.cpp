//
// FILE NAME: CIDAI_BTSelNode.cpp
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
//  This file implements the fundamental selector node type.
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
#include    "CIDAI_BTSelNode_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTSelNode, TAIBTNode)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTSelNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TAIBTSelNode: Constructors and Destructor
// -------------------------------------------------------------------
TAIBTSelNode::TAIBTSelNode(const TString& strPath, const TString& strName) :

    TAIBTNode
    (
        strPath
        , strName
        , kCIDAI::strDefFact_Selector
        , 1
        , kCIDAI::c4MaxChildNodes
    )
{
}

TAIBTSelNode::~TAIBTSelNode()
{
}


// -------------------------------------------------------------------
//  TAIBTSelNode: Public, virtual methods
// -------------------------------------------------------------------
tCIDAI::ENodeStates TAIBTSelNode::eRun(TAIBehaviorTree& btreeOwner)
{
    // Run our children in order until one succeeds or they all fail
    const tCIDLib::TCard4 c4Count = c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        tCIDAI::ENodeStates eCurState = eRunChildAt(btreeOwner, c4Index);
        if (eCurState == tCIDAI::ENodeStates::Success)
            return tCIDAI::ENodeStates::Success;
    }
    return tCIDAI::ENodeStates::Failure;
}

