//
// FILE NAME: CIDAI_BTDefNodeFatcory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the default BT node factory
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

#include    "CIDAI_BTInverterNode_.hpp"
#include    "CIDAI_BTLinkNode_.hpp"
#include    "CIDAI_BTRandChoiceNode_.hpp"
#include    "CIDAI_BTSelNode_.hpp"
#include    "CIDAI_BTSeqNode_.hpp"
#include    "CIDAI_BTSuccessNode_.hpp"

#include    "CIDAI_BTDefNodeFactory_.hpp"
#include    "CIDAI_BTVarTestNode_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTDefNodeFact, TAIBTNodeFact)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDAI_BTDefNodeFactory
{
    // -----------------------------------------------------------------------
    //  The names of the node types we support
    // -----------------------------------------------------------------------
    const TString   strType_Inverter(L"Inverter");
    const TString   strType_Link(L"Link");
    const TString   strType_RandChoice(L"RandChoice");
    const TString   strType_Selector(L"Selector");
    const TString   strType_Sequence(L"Sequence");
    const TString   strType_Success(L"Success");
    const TString   strType_VarTest(L"VarTest");
}


// ---------------------------------------------------------------------------
//   CLASS: TAIBTDefNodeFact
//  PREFIX: nfact
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBTDefNodeFact: Constructors and Destructor
// ---------------------------------------------------------------------------
TAIBTDefNodeFact::TAIBTDefNodeFact() :

    TAIBTNodeFact(kCIDAI::strFact_Default)
{
}

TAIBTDefNodeFact::~TAIBTDefNodeFact()
{
}


// ---------------------------------------------------------------------------
//  TAIBTDefNodeFact: Protected, inherited methods
// ---------------------------------------------------------------------------
TAIBTNode*
TAIBTDefNodeFact::pbtnodeNew(const  TString&            strPath
                            , const TString&            strName
                            , const TString&            strType)
{
    if (strType == CIDAI_BTDefNodeFactory::strType_Inverter)
        return new TAIBTInverterNode(strPath, strName);
    else if (strType == CIDAI_BTDefNodeFactory::strType_Link)
        return new TAIBTLinkNode(strPath, strName);
    else if (strType == CIDAI_BTDefNodeFactory::strType_RandChoice)
        return new TAIBTRandChoiceNode(strPath, strName);
    else if (strType == CIDAI_BTDefNodeFactory::strType_Selector)
        return new TAIBTSelNode(strPath, strName);
    else if (strType == CIDAI_BTDefNodeFactory::strType_Sequence)
        return new TAIBTSeqNode(strPath, strName);
    else if (strType == CIDAI_BTDefNodeFactory::strType_Success)
        return new TAIBTSuccessNode(strPath, strName);
    else if (strType == CIDAI_BTDefNodeFactory::strType_VarTest)
        return new TAIBTVarTestNode(strPath, strName);

    return nullptr;
}
