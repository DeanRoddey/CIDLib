//
// FILE NAME: CIDXML_CMNodes.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/25/1999
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
//  This file implements all of the content model node classes which are used
//  by the NFA to DFA algorithm.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDXML_.hpp"
#include    "CIDXML_CMNodes_.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TXMLCMBinOp
// PREFIX: xcmn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLCMNode: Public Destructor
// ---------------------------------------------------------------------------
TXMLCMNode::~TXMLCMNode()
{
    delete m_pbtsFirst;
    delete m_pbtsLast;
}


// ---------------------------------------------------------------------------
//  TXMLCMNode: Hidden constructors
// ---------------------------------------------------------------------------
TXMLCMNode::TXMLCMNode(const tCIDXML::ECMNodeTypes eNodeType) :

    m_c4MaxNFAStates(0)
    , m_eNodeType(eNodeType)
    , m_pbtsFirst(nullptr)
    , m_pbtsLast(nullptr)
{
}



// ---------------------------------------------------------------------------
//  CLASS: TXMLCMLeft
// PREFIX: xcmn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLCMLeaf: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLCMLeaf::TXMLCMLeaf( const   tCIDLib::TCard4 c4ElemId
                        , const tCIDLib::TCard4 c4StatePos) :

    TXMLCMNode(tCIDXML::ECMNodeTypes::Leaf)
    , m_c4ElemId(c4ElemId)
    , m_c4StatePos(c4StatePos)
{
}

TXMLCMLeaf::~TXMLCMLeaf()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TXMLCMBinOp
// PREFIX: xcmn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLCMBinOp: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLCMBinOp::TXMLCMBinOp(const  tCIDXML::ECMNodeTypes   eType
                        ,       TXMLCMNode* const       pcmnLeftToAdopt
                        ,       TXMLCMNode* const       pcmnRightToAdopt) :

    TXMLCMNode(eType)
    , m_pxcmnLeft(pcmnLeftToAdopt)
    , m_pxcmnRight(pcmnRightToAdopt)
{
    #if CID_DEBUG_ON
    if ((eNodeType() != tCIDXML::ECMNodeTypes::Sequence)
    &&  (eNodeType() != tCIDXML::ECMNodeTypes::Alternation))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCM_InvalidTypeForBinOp
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif
}

TXMLCMBinOp::~TXMLCMBinOp()
{
    delete m_pxcmnLeft;
    delete m_pxcmnRight;
}


// ---------------------------------------------------------------------------
//  TXMLCMBinOp: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TXMLCMBinOp::bIsNullable() const
{
    #if CID_DEBUG_ON
    if ((eNodeType() != tCIDXML::ECMNodeTypes::Sequence)
    &&  (eNodeType() != tCIDXML::ECMNodeTypes::Alternation))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCM_InvalidTypeForBinOp
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  For an alternation node, if either child is nullable then this parent
    //  is nullable. For a sequence, then they both have to be nullable for
    //  this node to be nullable.
    //
    if (eNodeType() == tCIDXML::ECMNodeTypes::Sequence)
        return (m_pxcmnLeft->bIsNullable() && m_pxcmnRight->bIsNullable());

    return (m_pxcmnLeft->bIsNullable() || m_pxcmnRight->bIsNullable());
}


// ---------------------------------------------------------------------------
//  TXMLCMBinOp: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLCMBinOp::CalculateFirst(TBitset& btsToSet) const
{
    //
    //  For an alternation, our first pos is the union of the first pos sets
    //  of the children. For a sequence, if our left child is nullable, then
    //  its the same, else its just the left child's first pos set.
    //
    if (eNodeType() == tCIDXML::ECMNodeTypes::Alternation)
    {
        btsToSet = m_pxcmnLeft->btsFirstPos();
        btsToSet |= m_pxcmnRight->btsFirstPos();
    }
     else
    {
        btsToSet = m_pxcmnLeft->btsFirstPos();
        if (m_pxcmnLeft->bIsNullable())
            btsToSet |= m_pxcmnRight->btsFirstPos();
    }
}

tCIDLib::TVoid TXMLCMBinOp::CalculateLast(TBitset& btsToSet) const
{
    //
    //  For an alternation, our last pos is the union of the last pos sets
    //  of the children. For a sequence, if our right child is nullable, then
    //  its the same, else its just the right child's last pos set.
    //
    if (eNodeType() == tCIDXML::ECMNodeTypes::Alternation)
    {
        btsToSet = m_pxcmnLeft->btsLastPos();
        btsToSet |= m_pxcmnRight->btsLastPos();
    }
     else
    {
        btsToSet = m_pxcmnRight->btsLastPos();
        if (m_pxcmnRight->bIsNullable())
            btsToSet |= m_pxcmnLeft->btsLastPos();
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TXMLCMUnaryOp
// PREFIX: xcmn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLCMUnaryOp: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLCMUnaryOp::TXMLCMUnaryOp(const  tCIDXML::ECMNodeTypes   eType
                            ,       TXMLCMNode* const       pcmnToAdopt) :

    TXMLCMNode(eType)
    , m_pxcmnChild(pcmnToAdopt)
{
    #if CID_DEBUG_ON
    if ((eNodeType() != tCIDXML::ECMNodeTypes::ZeroOrOne)
    &&  (eNodeType() != tCIDXML::ECMNodeTypes::OneOrMore)
    &&  (eNodeType() != tCIDXML::ECMNodeTypes::ZeroOrMore))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCM_InvalidTypeForUnaryOp
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif
}

TXMLCMUnaryOp::~TXMLCMUnaryOp()
{
    delete m_pxcmnChild;
}


