//
// FILE NAME: CIDAI_BTLinkNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/26/2016
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
//  This file implements the link mode, which is a simple but key feature.
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
#include    "CIDAI_BTLinkNode_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTLinkNode, TAIBTNode)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTLinkNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TAIBTLinkNode: Constructors and Destructor
// -------------------------------------------------------------------
TAIBTLinkNode::TAIBTLinkNode(const  TString& strPath, const TString& strName) :

    TAIBTNode(strPath, strName, kCIDAI::strDefFact_Link, 1, 1)
{
}

TAIBTLinkNode::~TAIBTLinkNode()
{
}


// -------------------------------------------------------------------
//  TAIBTLinkNode: Public, virtual methods
// -------------------------------------------------------------------
tCIDAI::ENodeStates TAIBTLinkNode::eRun(TAIBehaviorTree& btreeOwner)
{
    // Find our target tree
    TAIBTRootNode* pbtnodeTar = btreeOwner.pbtnodeFindTree
    (
        strFindParam(kCIDAI::strNParam_LinkTarget)
    );

    //
    //  This guy is a special case. The node really isn't a child of ours, so we can't
    //  call eRunChildAt. We have to use a special helper that will run an arbitrary
    //  node.
    //
    return eRunNode(btreeOwner, *pbtnodeTar);
}



//
//  We check to see if the target tree exists
//
tCIDLib::TVoid
TAIBTLinkNode::Validate(TAIBehaviorTree& btreeOwner, TVector<TAIBTErr>& colToFill)
{
    // See if the target tree exists
    const TString& strTarTree = strFindParam(kCIDAI::strNParam_LinkTarget);
    TAIBTRootNode* pbtnodeTar = btreeOwner.pbtnodeFindTree(strTarTree, kCIDLib::False);

    if (!pbtnodeTar)
    {
        TAIBTErr bterrNew
        (
            strPath()
            , strType()
            , facCIDAI().strMsg(kAIErrs::errcBT_TarLinkNotFnd, strTarTree)
        );
        colToFill.objAdd(bterrNew);
    }
}
