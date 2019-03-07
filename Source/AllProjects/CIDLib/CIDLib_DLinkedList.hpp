//
// FILE NAME: CIDLib_LinkedList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/25/1995
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_LinkedList.Cpp file. This file implements the
//  concrete doubly linked list collection type. The linked list is defined in terms
//  of an abstract node class, from which a template class will later be defined. That
//  template class will be instantiated many times for particular styles of collections
//  built upon this basic linked list functionality.
//
//  Note that this concrete linked list class is not derived from the base collection
//  classes itself. The template classes that use it are derived from the collection
//  classes.
//
//  We don't support copy/assign. We don't know how to copy the elements, so the derived
//  class has to do it.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TDLstNode
//  PREFIX: node
// ---------------------------------------------------------------------------
class CIDLIBEXP TDLstNode : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDLstNode(const TDLstNode&) = delete;

        ~TDLstNode();


        // -------------------------------------------------------------------
        //  Public opreators
        // -------------------------------------------------------------------
        const TDLstNode& operator=(const TDLstNode&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TDLstNode* pnodeNext() const;

        TDLstNode* pnodePrev() const;


    protected   :
        // -------------------------------------------------------------------
        //  Our container class is our friend. Can you say friend? Sure!
        //  That's real nice.
        // -------------------------------------------------------------------
        friend class TDLinkedList;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TDLstNode();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pnodeNext
        //  m_pnodePrev
        //      The next and previous nodes of this node
        // -------------------------------------------------------------------
        TDLstNode*      m_pnodeNext;
        TDLstNode*      m_pnodePrev;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDLstNode,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TDLinkedList
//  PREFIX: llst
// ---------------------------------------------------------------------------
class CIDLIBEXP TDLinkedList : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDLinkedList();

        TDLinkedList
        (
            const   TDLinkedList&           colSrc
        );

        TDLinkedList(TDLinkedList&& llstSrc) :

            TDLinkedList()
        {
            *this = tCIDLib::ForceMove(llstSrc);
        }

        ~TDLinkedList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TDLinkedList&            llstSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TDLinkedList&            llstSrc
        )   const;

        TDLinkedList& operator=(const TDLinkedList&) = delete;

        TDLinkedList& operator=
        (
                    TDLinkedList&&          llstSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendNode
        (
                    TDLstNode* const        pnodeToAdopt
        );

        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TBoolean bIsFirstNode
        (
            const   TDLstNode* const        pnodeToTest
        )   const;

        tCIDLib::TBoolean bIsLastNode
        (
            const   TDLstNode* const        pnodeToTest
        )   const;

        tCIDLib::TBoolean bIsMemberNode
        (
            const   TDLstNode* const        pnodeToTest
        )   const;

        tCIDLib::TCard4 c4ElemCount() const;

        tCIDLib::TVoid ExchangeNodes
        (
                    TDLstNode* const        pnode1
            ,       TDLstNode* const        pnode2
        );

        tCIDLib::TVoid RemoveAll();

        tCIDLib::TVoid RemoveNode
        (
                    TDLstNode* const        pnodeToRemove
        );

        tCIDLib::TVoid InsertNode
        (
                    TDLstNode* const        pnodeAfter
            ,       TDLstNode* const        pnodeToInsert
        );

        tCIDLib::TVoid MoveToHead
        (
                    TDLstNode* const        pnodeToMove
        );

        tCIDLib::TVoid MoveToTail
        (
                    TDLstNode* const        pnodeToMove
        );

        TDLstNode* pnodeHead() const;

        TDLstNode* pnodeTail() const;

        tCIDLib::TVoid PrependNode
        (
                    TDLstNode* const        pnodeToPrepend
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ElemCount
        //      The number of objects currently in the container. It is
        //      maintained as elems are added or removed.
        //
        //  m_pnodeHead
        //      This is a pointer to the 1st node of the linked list. If it
        //      is 0, then there are no elements in the list.
        //
        //  m_pnodTail
        //      This is the pointer to the last node of the linked list. If it
        //      is 0, then there are no elements in the list.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4ElemCount;
        TDLstNode*      m_pnodeHead;
        TDLstNode*      m_pnodeTail;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDLinkedList,TObject)
};

#pragma CIDLIB_POPPACK

