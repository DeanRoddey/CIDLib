//
// FILE NAME: CIDLib_DLinkedList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/02/1993
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
//  This file implements the concrete collection class, TDLinkedList, and its
//  abstract linked list node class. The linked list class is defined in terms
//  of the abstract node class, from which many type specific versions are
//  instantiated via a template. The derived types determine the type of data
//  is in a node and what capabilities that the data must provide.
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
RTTIDecls(TDLstNode,TObject)
RTTIDecls(TDLinkedList,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TDLstNode
//  PREFIX: node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDLstNode: Constructors and Destructor
// ---------------------------------------------------------------------------
TDLstNode::TDLstNode() :

    m_pnodeNext(nullptr)
    , m_pnodePrev(nullptr)
{
}

TDLstNode::~TDLstNode()
{
}


// ---------------------------------------------------------------------------
//  TDLstNode: Public, non-virtual methods
// ---------------------------------------------------------------------------
TDLstNode* TDLstNode::pnodeNext() const
{
    return m_pnodeNext;
}

TDLstNode* TDLstNode::pnodePrev() const
{
    return m_pnodePrev;
}




// ---------------------------------------------------------------------------
//   CLASS: TDLinkedList
//  PREFIX: llst
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDLinkedList: Constructors and Destructor
// ---------------------------------------------------------------------------
TDLinkedList::TDLinkedList() :

    m_c4ElemCount(0)
    , m_pnodeHead(nullptr)
    , m_pnodeTail(nullptr)
{
}

TDLinkedList::~TDLinkedList()
{
    RemoveAll();
}


// ---------------------------------------------------------------------------
//  TDLinkedList: Public operators
// ---------------------------------------------------------------------------

TDLinkedList& TDLinkedList::operator=(TDLinkedList&& llstSrc)
{
    if (&llstSrc != this)
    {
        tCIDLib::Swap(m_c4ElemCount, llstSrc.m_c4ElemCount);
        tCIDLib::Swap(m_pnodeHead, llstSrc.m_pnodeHead);
        tCIDLib::Swap(m_pnodeTail, llstSrc.m_pnodeTail);
    }
    return *this;
}


// We cannot compare the elements here. The using class must do that
tCIDLib::TBoolean
TDLinkedList::operator==(const TDLinkedList& llstToTest) const
{
    if (this != &llstToTest)
    {
        if (m_c4ElemCount != llstToTest.m_c4ElemCount)
            return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TDLinkedList::operator!=(const TDLinkedList& llstToTest) const
{
    return !operator==(llstToTest);
}


// ---------------------------------------------------------------------------
//  TDLinkedList: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDLinkedList::AppendNode(TDLstNode* const pnodeToAppend)
{
    // If debugging, make sure its not already a member
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

        // Set hte new node's prev to the current tail and 0 its next
        pnodeToAppend->m_pnodePrev = m_pnodeTail;
        pnodeToAppend->m_pnodeNext = nullptr;

        // Now store it as the new tail
        m_pnodeTail = pnodeToAppend;
    }
}


tCIDLib::TBoolean TDLinkedList::bIsEmpty() const
{
    return (m_c4ElemCount == 0);
}

tCIDLib::TBoolean
TDLinkedList::bIsFirstNode(const TDLstNode* const pnodeToTest) const
{
    return (pnodeToTest == m_pnodeHead);
}


tCIDLib::TBoolean
TDLinkedList::bIsLastNode(const TDLstNode* const pnodeToTest) const
{
    return (pnodeToTest == m_pnodeTail);
}


tCIDLib::TBoolean
TDLinkedList::bIsMemberNode(const TDLstNode* const pnodeToTest) const
{
    TDLstNode* pnodeCur  = m_pnodeHead;

    while (pnodeCur)
    {
        if (pnodeCur == pnodeToTest)
            return kCIDLib::True;

        // Move up to the next node
        pnodeCur = pnodeCur->m_pnodeNext;
    }
    return kCIDLib::False;
}


tCIDLib::TCard4 TDLinkedList::c4ElemCount() const
{
    return m_c4ElemCount;
}


tCIDLib::TVoid
TDLinkedList::ExchangeNodes(TDLstNode* const pnode1, TDLstNode* const pnode2)
{
    // If debugging, make sure they are both members
    #if CID_DEBUG_ON
    if (!bIsMemberNode(pnode1) || !bIsMemberNode(pnode2))
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
    #endif

    // Handle freakout scenario of the 2 nodes being the same
    if (pnode1 == pnode2)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_SameNode
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::BadParms
        );
    }

    //
    //  If there are not at least 2 nodes, then an internal fatal error. The
    //  client app must be using at least one invalid node.
    //
    if (m_c4ElemCount < 2)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_NotEnoughNodes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    // If the head pointer is nul, then an internal error
    if (!m_pnodeHead)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_NullHead
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
     else
    {
        //
        //  We need to make sure that node 1 is before node 2, otherwise we need
        //  to flip them.
        //
        TDLstNode* pnodeTmp = m_pnodeHead;
        TDLstNode* pnodeTmp1 = pnode1;
        TDLstNode* pnodeTmp2 = pnode2;
        tCIDLib::TCard4 c4Ind = 0;
        for (; c4Ind < m_c4ElemCount; c4Ind++)
        {
            //
            //  If we find the 1st node first, then just break out. If we find the
            //  second node first, then flip them.
            //
            if (pnodeTmp == pnodeTmp1)
            {
                break;
            }
            else if (pnodeTmp == pnode2)
            {
                pnodeTmp = pnodeTmp1;
                pnodeTmp1 = pnode2;
                pnodeTmp2 = pnodeTmp;
                break;
            }

            pnodeTmp = pnodeTmp->m_pnodeNext;
        }

        if (c4Ind == m_c4ElemCount)
        {
            // If we got here, then a node is missing
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcCol_NotMemberNode
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
            );
        }

        //
        //  Get local copies of the next/prev pointers of each node so that we
        //  won't have to worry about changing the original and still needing
        //  the old value.
        //
        TDLstNode* pnode1Prev  = pnodeTmp1->m_pnodePrev;
        TDLstNode* pnode2Prev  = pnodeTmp2->m_pnodePrev;
        TDLstNode* pnode1Next  = pnodeTmp1->m_pnodeNext;
        TDLstNode* pnode2Next  = pnodeTmp2->m_pnodeNext;

        //
        //  Handle the next node pointer of the node before pnode1. It should now
        //  point to pnode2, unless pnode1 is the 1st node.
        //
        if (pnode1Prev)
        pnodeTmp1->m_pnodePrev->m_pnodeNext = pnodeTmp2;

        //
        //  Handle the previous node pointer of the node after pnode2. It should
        //  now point to pnode1, unless pnode2 is the last node
        //
        if (pnode2Next)
        pnodeTmp2->m_pnodeNext->m_pnodePrev = pnodeTmp1;

        //
        //  Handle the previous node pointer of pnode2. It should point to the
        //  previous node of pnode1. If this guy is nul, then pnode2 is now the
        //  1st node, so point the list head at him.
        //
        pnodeTmp2->m_pnodePrev = pnode1Prev;

        if (!pnodeTmp2->m_pnodePrev)
            m_pnodeHead = pnodeTmp2;

        //
        //  Handle the next node pointer of pnode1 and make it point to pnode2's
        //  next node, which could be nul if pnode2 was last. This would now make
        //  pnode1 last.
        //
        pnodeTmp1->m_pnodeNext = pnode2Next;

        //
        //  Handle the next node pointer to pnode2 and make it point to pnode1's
        //  next node.
        //
        //  We also have a potential special case here if the nodes being
        //  exchanged are side by side, in which case pnode2's next node should
        //  point at pnode1. Otherwise pnode2 would end up pointing at itself.
        //
        if (pnode1Next == pnodeTmp2)
        {
            pnodeTmp2->m_pnodeNext = pnodeTmp1;
        }
         else
        {
            pnodeTmp2->m_pnodeNext = pnode1Next;
            pnode2Prev->m_pnodeNext = pnodeTmp1;
        }

        //
        //  The same problem exists for the previous node of pnode1. If they are
        //  side by side, then just point it at pnode1; else, point it at the
        //  previous node of pnode2.
        //
        if (pnode2Prev == pnodeTmp1)
        {
            pnodeTmp1->m_pnodePrev = pnodeTmp2;
        }
         else
        {
            pnodeTmp1->m_pnodePrev = pnode2Prev;
            pnode1Next->m_pnodePrev = pnodeTmp2;
        }
    }
}


tCIDLib::TVoid
TDLinkedList::InsertNode(   TDLstNode* const    pnodeAfter
                            , TDLstNode* const  pnodeToInsert)
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

    // Get a pointer to the after node's next node
    TDLstNode* pnodeNext = pnodeAfter->m_pnodeNext;

    //
    //  Insert the node into the list. Handle the special case of the
    //  insert after node being the last node.
    //
    if (m_pnodeTail == pnodeAfter)
    {
        // Point the list tail at the new node
        m_pnodeTail = pnodeToInsert;

        // Point new node's prev node at pnodeAFter and link forward
        pnodeToInsert->m_pnodePrev = pnodeAfter;
        pnodeAfter->m_pnodeNext = pnodeToInsert;
    }
     else
    {
       // No special case, just do the normal patch around
       pnodeAfter->m_pnodeNext = pnodeToInsert;
       pnodeToInsert->m_pnodePrev = pnodeAfter;
       pnodeToInsert->m_pnodeNext = pnodeNext;
       pnodeNext->m_pnodePrev = pnodeToInsert;
    }

    // Bump up the count of items
    m_c4ElemCount++;
}


tCIDLib::TVoid TDLinkedList::MoveToHead(TDLstNode* const pnodeToMove)
{
    // If debugging, make sure its a member
    #if CID_DEBUG_ON
    if (!bIsMemberNode(pnodeToMove))
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
    #endif

    // If there are not at least 2 nodes, then nothing to do
    if (m_c4ElemCount < 2)
        return;

    // If this node is already the first, then nothing to do
    if (!pnodeToMove->m_pnodePrev)
        return;

    //
    //  Pull the node out of the list. If its the last one, then handle that
    //  specially. We already checked for it already being at the head.
    //
    if (!pnodeToMove->m_pnodeNext)
    {
        // Set our previous nodes's next node to null
        pnodeToMove->m_pnodePrev->m_pnodeNext = nullptr;

        // And make it the new tail
        m_pnodeTail = pnodeToMove->m_pnodePrev;
    }
     else
    {
        // Set our previous node's next node to our next node
        pnodeToMove->m_pnodePrev->m_pnodeNext = pnodeToMove->m_pnodeNext;

        // Set our next node's previous to our previous
        pnodeToMove->m_pnodeNext->m_pnodePrev = pnodeToMove->m_pnodePrev;
    }

    // Bump down the node count
    m_c4ElemCount--;

    // And insert the node at the start
    PrependNode(pnodeToMove);
}


tCIDLib::TVoid TDLinkedList::MoveToTail(TDLstNode* const pnodeToMove)
{
    // If debugging, then insure its a member
    #if CID_DEBUG_ON
    if (!bIsMemberNode(pnodeToMove))
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
    #endif

    // If there are not at least 2 nodes, then nothng to do
    if (m_c4ElemCount < 2)
        return;

    // If this node is already the last, then nothing to do
    if (!pnodeToMove->m_pnodeNext)
        return;

    //
    //  Pull the node out of the list. Handle it being the first node
    //  separately. We already checked whether it was already at the tail.
    //
    if (!pnodeToMove->m_pnodePrev)
    {
        // Set our next node's previous node to null
        pnodeToMove->m_pnodeNext->m_pnodePrev = nullptr;

        // Store it as the new head
        m_pnodeHead = pnodeToMove->m_pnodeNext;
    }
     else
    {
        // Set our previous node's next node to our next node
        pnodeToMove->m_pnodePrev->m_pnodeNext = pnodeToMove->m_pnodeNext;

        // Set our next node's previous to our previous
        pnodeToMove->m_pnodeNext->m_pnodePrev = pnodeToMove->m_pnodePrev;
    }

    // Bump down the node count
    m_c4ElemCount--;

    // And append it
    AppendNode(pnodeToMove);
}


tCIDLib::TVoid TDLinkedList::PrependNode(TDLstNode* const pnodeToPrepend)
{
    // If there are no current nodes, then just call AppendNode()
    if (!m_c4ElemCount)
    {
        AppendNode(pnodeToPrepend);
        return;
    }

    // If debugging, make sure its not already a member node
    #if CID_DEBUG_ON
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
    //  So get a pointer to the current first node. The new node's prev
    //  node pointer is left 0 to indicate it is the first.
    //
    //  If there was only 1 node, then m_pnodeTail still points to the old
    //  head which is correct.
    //
    TDLstNode* pnodeCurFirst = m_pnodeHead;

    m_pnodeHead = pnodeToPrepend;
    m_pnodeHead->m_pnodeNext = pnodeCurFirst;
    pnodeCurFirst->m_pnodePrev = pnodeToPrepend;
}


TDLstNode* TDLinkedList::pnodeHead() const
{
    return m_pnodeHead;
}


TDLstNode* TDLinkedList::pnodeTail() const
{
    return m_pnodeTail;
}


tCIDLib::TVoid TDLinkedList::RemoveAll()
{
    //
    //  If no head node, then an empty list so just return. But clear other
    //  members just in case!
    //
    if (!m_pnodeHead)
    {
        m_c4ElemCount   = 0;
        m_pnodeHead     = nullptr;
        m_pnodeTail     = nullptr;
        return;
    }

    // There is at least one node, so loop through them
    TDLstNode* pnodeCur  = m_pnodeHead;
    TDLstNode* pnodeNext = nullptr;

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


tCIDLib::TVoid TDLinkedList::RemoveNode(TDLstNode* const pnodeToRemove)
{
    // If debugging, make sure its a member
    #if CID_DEBUG_ON
    if (!bIsMemberNode(pnodeToRemove))
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
    #endif

    // If there is no head node, then an empty list so just return
    if (!m_pnodeHead)
        return;

    // If only 1 node, then just flush the list, which is faster
    if (m_c4ElemCount == 1)
    {
        RemoveAll();
        return;
    }

    // Get a pointer to the next/prev nodes of the passed node
    TDLstNode* pnodePrev = pnodeToRemove->m_pnodePrev;
    TDLstNode* pnodeNext = pnodeToRemove->m_pnodeNext;

    //
    //  Adjust the next node of pnodePrev and previous node of the
    //  pnodeNext to skip over the current node, thus removing it from
    //  the chain. Check for the special case that this is the first or
    //  last node.
    //
    if (!pnodeNext)
    {
        //
        //  The delete record is end of the line, so this one is easy. Just
        //  make the pnodePrev's next node nul, which makes it the new end
        //  of the list.
        //
        pnodePrev->m_pnodeNext = nullptr;
        m_pnodeTail = pnodePrev;
    }
     else if (!pnodePrev)
    {
        //
        //  The delete record is the start of the line, so we have to point
        //  the list's head to nodeNext and set the next node's previous
        //  node to nul to indicate that it is now the first node.
        //
        m_pnodeHead  = pnodeNext;
        pnodeNext->m_pnodePrev = nullptr;
    }
     else
    {
        //
        //  No special conditions, just do the normal patch around the
        //  deleted node
        //
       pnodePrev->m_pnodeNext = pnodeNext;
       pnodeNext->m_pnodePrev = pnodePrev;
    }

    // Bump down the count of nodes
    m_c4ElemCount--;

    // Delete the node object
    delete pnodeToRemove;
}
