//
// FILE NAME: CIDDocComp_HelpNode.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/03/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  The implementation the basic recursive markup containing node.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDDocComp.hpp"



// ---------------------------------------------------------------------------
//   CLASS: THelpNode
//  PREFIX: hn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THelpNode: Constructors and Destructor
// ---------------------------------------------------------------------------

THelpNode::THelpNode() :

    m_c4Extra(0)
    , m_eType(tCIDDocComp::EType_None)
{
}

THelpNode::~THelpNode()
{
}


// ---------------------------------------------------------------------------
//  THelpNode: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean THelpNode::bIsEmpty() const
{
    //
    //  If this guy isn't actual content, and it holds no content. Or, it's a text node
    //  and there's no text, consider it empty.
    //
    return
    (
        m_colNodes.bIsEmpty() || ((m_eType == tCIDDocComp::EType_Text) && m_strText.bIsEmpty())
    );
}
