//
// FILE NAME: CIDAI_BTVarTestNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/26/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the variable test node.
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
#include    "CIDAI_BTVarTestNode_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTVarTestNode, TAIBTNode)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTVarTestNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TAIBTVarTestNode: Constructors and Destructor
// -------------------------------------------------------------------

// We are a leaf node, so no children
TAIBTVarTestNode::TAIBTVarTestNode(const TString& strPath, const TString& strName) :

    TAIBTNode(strPath, strName, kCIDAI::strDefFact_VarTest, 0, 0)
{
}

TAIBTVarTestNode::~TAIBTVarTestNode()
{
}


// -------------------------------------------------------------------
//  TAIBTVarTestNode: Public, virtual methods
// -------------------------------------------------------------------
tCIDAI::ENodeStates TAIBTVarTestNode::eRun(TAIBehaviorTree& btreeOwner)
{
    // Look up the values, which we got as parameters
    const TString& strTargetVar = strFindParam(kCIDAI::strNParam_VarTestVar);
    const TString& strTargetVal = strFindParam(kCIDAI::strNParam_VarTestVal);

    // The comparison value
    const TString& strCompVal = strFindParam(kCIDAI::strNParam_VarTestComp);

    // If the variable doesn't exist, we fail
    TString strVarVal;
    if (!btreeOwner.ctxToUse().bQueryValue(strTargetVar, strTargetVal, strVarVal))
        return tCIDAI::ENodeStates::Failure;

    if (strVarVal.bCompareI(strCompVal))
        return tCIDAI::ENodeStates::Success;

    return tCIDAI::ENodeStates::Failure;
}

