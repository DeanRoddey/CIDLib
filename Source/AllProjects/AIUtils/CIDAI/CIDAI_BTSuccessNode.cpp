//
// FILE NAME: CIDAI_BTSuccessNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the success node type.
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
#include    "CIDAI_BTSuccessNode_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTSuccessNode, TAIBTNode)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTSuccessNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TAIBTSuccessNode: Constructors and Destructor
// -------------------------------------------------------------------
TAIBTSuccessNode::TAIBTSuccessNode(const  TString& strPath, const TString& strName) :

    TAIBTNode(strPath, strName, kCIDAI::strDefFact_Inverter, 0, 0)
{
}

TAIBTSuccessNode::~TAIBTSuccessNode()
{
}


// -------------------------------------------------------------------
//  TAIBTSuccessNode: Public, virtual methods
// -------------------------------------------------------------------
tCIDAI::ENodeStates TAIBTSuccessNode::eRun(TAIBehaviorTree& btreeOwner)
{
    return tCIDAI::ENodeStates::Success;
}
