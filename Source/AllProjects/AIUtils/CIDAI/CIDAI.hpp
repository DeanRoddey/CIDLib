//
// FILE NAME: CIDAI.hpp
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
//  This is the main header of the CIDLib AI facility. This library facility
//  provides some core functionality required to implement pseudo artificial
//  intelligence, such as behavior trees.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDAI)
#define CIDAIEXP CID_DLLEXPORT
#else
#define CIDAIEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TXMLTreeElement;


// ---------------------------------------------------------------------------
//  And sub-include our core headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDAI_Type.hpp"
#include    "CIDAI_Shared.hpp"
#include    "CIDAI_ErrorIds.hpp"
#include    "CIDAI_MessageIds.hpp"

#include    "CIDAI_BTContext.hpp"
#include    "CIDAI_BTNode.hpp"
#include    "CIDAI_BTRootNode.hpp"

namespace tCIDAI
{
    using TNodeStack = TRefStack<TAIBTNode>;
    using TTreeList = TRefKeyedHashSet<TAIBTRootNode, TString, TStringKeyOps>;
};

#include    "CIDAI_BehaviorTree.hpp"
#include    "CIDAI_BTNodeFactory.hpp"
#include    "CIDAI_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy eval function
// ---------------------------------------------------------------------------
extern CIDAIEXP TFacCIDAI& facCIDAI();
