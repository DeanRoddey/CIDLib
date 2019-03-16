//
// FILE NAME: CIDAI_BTSuccessNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/2016
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
