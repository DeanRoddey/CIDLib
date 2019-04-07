//
// FILE NAME: CIDLib_SLinkedList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/22/1997
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
//  This file implements the concrete collection class, TSLinkedList, and its
//  abstract linked list node class. The linked list class is defined in terms
//  of the abstract node class, from which many type specific versions are
//  instantiated via a template. The derived types determine the type of data
//  is in a node and what capabilities that the data must provide.
//
//  We can't do copy/assign here since we can't replicate the elements. The
//  containing class has to do that, as well as comparisons.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TSLstNode,TObject)
RTTIDecls(TSLinkedList,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TSLstNode
//  PREFIX: node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSListNode: Constructors and Destructor
// ---------------------------------------------------------------------------
TSLstNode::TSLstNode() :

    m_pnodeNext(nullptr)
{
}

TSLstNode::~TSLstNode()
{
}


// ---------------------------------------------------------------------------
//  TSListNode: Public, non-virtual methods
// ---------------------------------------------------------------------------
TSLstNode* TSLstNode::pnodeNext() const
{
    return m_pnodeNext;
}



// ---------------------------------------------------------------------------
//   CLASS: TSLinkedList
//  PREFIX: llst
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSLinkedList: Constructors and Destructor
// ---------------------------------------------------------------------------
TSLinkedList::TSLinkedList() :

    m_c4ElemCount(0)
    , m_pnodeHead(nullptr)
    , m_pnodeTail(nullptr)
{
}

TSLinkedList::~TSLinkedList()
{
    RemoveAll();
}


// ---------------------------------------------------------------------------
//  TSLinkedList: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSLinkedList::operator==(const TSLinkedList& llstToTest) const
{
    if (this != &llstToTest)
    {
        if (m_c4ElemCount != llstToTest.m_c4ElemCount)
            return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TSLinkedList::operator!=(const TSLinkedList& llstToTest) const
{
    return !operator==(llstToTest);
}


// ---------------------------------------------------------------------------
//  TSLinkedList: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TSLinkedList::AppendNode(TSLstNode* const pnodeToAppend)
{
    // If debugging, then make sure its not already a member
    #if CID_DEBUG_ON
    if (bIsMemberNode(pnodeToAppend))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_AlreadyMemberNode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    #endif

    // Bump up the count of items
    m_c4ElemCount++;

    //
    //  Check for special case of an empty list with no entries yet. If so,
    //  then just store away the new node.
    //
    if (!m_pnodeHead)
    {
        // No current nodes, so point the head and tail at this node
        m_pnodeHead  = pnodeToAppend;
        m_pnodeTail  = pnodeToAppend;
    }
     else
    {
        // Point the current tail's next at the new node
        m_pnodeTail->m_pnodeNext = pnodeToAppend;

        // Set the new node's next node to null
        pnodeToAppend->m_pnodeNext = nullptr;

        // Now store it as the new tail
        m_pnodeTail = pnodeToAppend;
    }
}


tCIDLib::TBoolean TSLinkedList::bIsEmpty() const
{
    return (m_c4ElemCount == 0);
}


tCIDLib::TBoolean
TSLinkedList::bIsFirstNode(const TSLstNode* const pnodeToTest) const
{
    return (pnodeToTest == m_pnodeHead);
}


tCIDLib::TBoolean
TSLinkedList::bIsLastNode(const TSLstNode* const pnodeToTest) const
{
    return (pnodeToTest == m_pnodeTail);
}


tCIDLib::TBoolean
TSLinkedList::bIsMemberNode(const TSLstNode* const pnodeToTest) const
{
    TSLstNode* pnodeCur = m_pnodeHead;

    while (pnodeCur)
    {
        if (pnodeToTest == pnodeCur)
            return kCIDLib::True;

        // Move up to the next node
        pnodeCur = pnodeCur->m_pnodeNext;
    }
    return kCIDLib::False;
}


tCIDLib::TCard4 TSLinkedList::c4ElemCount() const
{
    return m_c4ElemCount;
}


tCIDLib::TVoid
TSLinkedList::InsertNode(   TSLstNode* const    pnodeAfter
                            , TSLstNode* const  pnodeToInsert)
{
    //
    //  If the after node is 0, then that means insert it at the head,
    //  so we just turn it into a call to PrependNode().
    //
    if (!pnodeAfter)
    {
        PrependNode(pnodeToInsert);
        return;
    }

    //
    //  If there are no current nodes, then an error because pnodeAfter
    //  cannot exist.
    //
    if (!m_c4ElemCount)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_NotMemberNode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Insure that new node is not already in the list and that the
    //  pnodeAfter is in the list, if we are in debug mode.
    //
    #if CID_DEBUG_ON
    if (bIsMemberNode(pnodeToInsert) || !bIsMemberNode(pnodeAfter))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_InvalidInsert
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    #endif

    //
    //  Get a copy of the after node's next node pointer, so we don't
    //  have to worry about when we change it below.
    //
    TSLstNode* pnodeNext = pnodeAfter->m_pnodeNext;

    //
    //  Insert the node into the list. Handle the special case of the
    //  insert after node being the tail node.
    //
    if (m_pnodeTail == pnodeAfter)
    {
        // Point the list tail at the new node
        m_pnodeTail = pnodeToInsert;

        // Point the after node's next node at the new tail
        pnodeAfter->m_pnodeNext = pnodeToInsert;
    }
     else
    {
       // No special case, just do the normal patch around
       pnodeAfter->m_pnodeNext = pnodeToInsert;
       pnodeToInsert->m_pnodeNext = pnodeNext;
    }

    // Bump up the count of items
    m_c4ElemCount++;
}


tCIDLib::TVoid TSLinkedList::MoveToHead(TSLstNode* const pnodeToMove)
{
    // If there are not at least 2 nodes, then nothing to do
    if (m_c4ElemCount < 2)
        return;

    // If this node is already the first, then nothing to do
    if (pnodeToMove == m_pnodeHead)
        return;

    //
    //  Find the previous node of the passed node, which also makes sure
    //  that the node is in the list.
    //
    TSLstNode* pnodePrev = nullptr;
    if (m_pnodeHead && (pnodeToMove != m_pnodeHead))
    {
        pnodePrev = m_pnodeHead;
        while (pnodePrev)
        {
            if (pnodePrev->m_pnodeNext == pnodeToMove)
                break;
            pnodePrev = pnodePrev->m_pnodeNext;
        }
    }

    // If not found, then the node was not part of this list.
    if (!pnodePrev && (pnodeToMove != m_pnodeHead))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_NotMemberNode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Pull the node out of the list. If its the last one, then handle that
    //  specially. We already checked for it already being at the head.
    //
    if (!pnodeToMove->m_pnodeNext)
    {
        // Set our previous nodes's next node to null
        pnodePrev->m_pnodeNext = nullptr;

        // And make it the new tail
        m_pnodeTail = pnodePrev;
    }
     else
    {
        // Set our previous node's next node to our next node
        pnodePrev->m_pnodeNext = pnodeToMove->m_pnodeNext;
    }

    // Bump down the count of nodes
    m_c4ElemCount--;

    // And insert the node at the start
    PrependNode(pnodeToMove);
}


tCIDLib::TVoid TSLinkedList::MoveToTail(TSLstNode* const pnodeToMove)
{
    // If there are not at least 2 nodes, then nothng to do
    if (m_c4ElemCount < 2)
        return;

    // If this node is already the last, then nothing to do
    if (!pnodeToMove->m_pnodeNext)
        return;

    //
    //  Find the previous node of the passed node, which also makes sure
    //  that the node is in the list.
    //
    TSLstNode* pnodePrev = nullptr;

    if (m_pnodeHead && (pnodeToMove != m_pnodeHead))
    {
        pnodePrev = m_pnodeHead;
        while (pnodePrev)
        {
            if (pnodePrev->m_pnodeNext == pnodeToMove)
                break;
            pnodePrev = pnodePrev->m_pnodeNext;
        }
    }

    // If not found, then the node was not part of this list.
    if (!pnodePrev && (pnodeToMove != m_pnodeHead))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_NotMemberNode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Pull the node out of the list. Handle it being the first node
    //  separately. We already checked whether it was already at the tail.
    //
    if (!pnodePrev)
    {
        // Store the move node's next as the new head
        m_pnodeHead = pnodeToMove->m_pnodeNext;
    }
     else
    {
        // Set our previous node's next node to our next node
        pnodePrev->m_pnodeNext = pnodeToMove->m_pnodeNext;
    }

    // Bump down the count of nodes
    m_c4ElemCount--;

    // And append the node that we removed
    AppendNode(pnodeToMove);
}


TSLstNode* TSLinkedList::pnodeHead() const
{
    return m_pnodeHead;
}


TSLstNode* TSLinkedList::pnodeTail() const
{
    return m_pnodeTail;
}


tCIDLib::TVoid TSLinkedList::PrependNode(TSLstNode* const pnodeToPrepend)
{
    // If there are no current nodes, then just call AppendNode()
    if (!m_c4ElemCount)
    {
        AppendNode(pnodeToPrepend);
        return;
    }

    // If debugging, make sure that its not already a member
    #if CID_DEBUGON_ON
    if (bIsMemberNode(pnodeToPrepend))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_AlreadyMemberNode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    #endif

    // Bump up the count of items
    m_c4ElemCount++;

    //
    //  There are only two options here, one was an empty list and was
    //  handled above. The other is that we just patch in the new node.
    //
    //  If there was only 1 node, then m_pnodeTail still points to the old
    //  head which is correct.
    //
    TSLstNode* pnodeCurFirst = m_pnodeHead;

    m_pnodeHead = pnodeToPrepend;
    m_pnodeHead->m_pnodeNext = pnodeCurFirst;
}


tCIDLib::TVoid TSLinkedList::RemoveAll()
{
    // If no head node, then an empty list so just return
    if (!m_pnodeHead)
        return;

    // There is at least one node, so loop through them
    TSLstNode* pnodeCur  = m_pnodeHead;
    TSLstNode* pnodeNext = nullptr;

    do
    {
        // Before we destroy this node, save its next node
        pnodeNext = pnodeCur->m_pnodeNext;

        // Free the node object
        delete pnodeCur;

        // Put the saved next node into the current node pointer
        pnodeCur = pnodeNext;

    }   while (pnodeCur);

    // 0 out the current item count and the head/tail pointers
    m_c4ElemCount   = 0;
    m_pnodeHead     = nullptr;
    m_pnodeTail     = nullptr;
}


tCIDLib::TVoid
TSLinkedList::RemoveNode(TSLstNode* const pnodeToRemove)
{
    //
    //  We need to run up to the node in question and find its previous
    //  node, and this also insures that its in this list as well.
    //
    TSLstNode* pnodePrev = nullptr;
    if (m_pnodeHead && (pnodeToRemove != m_pnodeHead))
    {
        pnodePrev = m_pnodeHead;
        while (pnodePrev)
        {
            if (pnodePrev->m_pnodeNext == pnodeToRemove)
                break;
            pnodePrev = pnodePrev->m_pnodeNext;
        }
    }

    // If not found, then the node was not part of this list.
    if (!pnodePrev && (pnodeToRemove != m_pnodeHead))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_NotMemberNode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Get a pointer to the next node of the passed node, so we don't
    //  have to worry about having changed it below.
    //
    TSLstNode* pnodeNext = pnodeToRemove->m_pnodeNext;

    //
    //  Adjust the next node of pnodePrev to skip over the current node,
    //  thus removing it from the chain. Check for the special case that
    //  this is the first or last node, or the only node.
    //
    if (m_c4ElemCount == 1)
    {
        m_pnodeHead = nullptr;
        m_pnodeTail = nullptr;
    }
     else if (!pnodePrev)
    {
        //
        //  The delete record is the start of the line, so we have to point
        //  the list's head to nodeNext.
        //
        m_pnodeHead  = pnodeNext;
    }
     else if (!pnodeNext)
    {
        //
        //  The delete record is end of the line, so this one is easy. Just
        //  make the pnodePrev's next node nul, which makes it the new end
        //  of the list and store it as the new tail node.
        //
        pnodePrev->m_pnodeNext = nullptr;
        m_pnodeTail = pnodePrev;
    }
     else
    {
        //
        //  No special conditions, just do the normal patch around the
        //  deleted node
        //
       pnodePrev->m_pnodeNext = pnodeNext;
    }

    // Bump down the count of nodes
    m_c4ElemCount--;

    // Delete the node object
    delete pnodeToRemove;
}

