//
// FILE NAME: CIDAI_BTRootNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the simple root BT node class.
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
RTTIDecls(TAIBTRootNode, TAIBTNode)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTRootNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBTRootNode: Public, static methods
// ---------------------------------------------------------------------------

// To support keyed collections
const TString& TAIBTRootNode::strRootNodeKey(const TAIBTRootNode& btnodeSrc)
{
    return btnodeSrc.strName();
}


// -------------------------------------------------------------------
//  TAIBTRootNode: Constructors and Destructor
// -------------------------------------------------------------------
TAIBTRootNode::TAIBTRootNode(const  TString&            strPath
                            , const TString&            strName
                            , const tCIDLib::TCard4     c4Version) :

    TAIBTNode(strPath, strName, kCIDAI::strDefFact_Root, 1, 1)
    , m_c4Version(c4Version)
{
}

TAIBTRootNode::~TAIBTRootNode()
{
}


// -------------------------------------------------------------------
//  TAIBTRootNode: Public, virtual methods
// -------------------------------------------------------------------
tCIDAI::ENodeStates TAIBTRootNode::eRun(TAIBehaviorTree& btreeOwner)
{
    tCIDAI::ENodeStates eRet = tCIDAI::ENodeStates::Failure;
    try
    {
        eRet = eRunChildAt(btreeOwner, 0);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
    return eRet;
}


//
//  Root nodes must be given a name so that they can be looked up by reference from
//  other trees (via reference nodes.)
//
tCIDLib::TVoid
TAIBTRootNode::Validate(TAIBehaviorTree& btreeOwner, TVector<TAIBTErr>& colToFill)
{
    if (strName().bIsEmpty())
    {
        TAIBTErr bterrNew
        (
            strPath(), strType(), facCIDAI().strMsg(kAIErrs::errcBT_RootName)
        );
        colToFill.objAdd(bterrNew);
    }
}
