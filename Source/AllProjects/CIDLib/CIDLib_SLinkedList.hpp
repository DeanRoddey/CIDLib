//
// FILE NAME: CIDLib_SLinkedList.hpp
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
//  This is the header for the CIDLib_SLinkedList.Cpp file. This file
//  implements the concrete singly linked list collection type. The linked
//  list is defined in terms of an abstract node class, from which a template
//  class will later be defined. That template class will be instantiated
//  many times for particular styles of collections built upon this
//  basic linked list functionality.
//
//  Note that this concrete linked list class is not derived from the
//  base collection classes itself. The template classes that use it are
//  derived from the collection classes.
//
//  These have no copy constructor, since we can't depend on copyability,
//  but they can be moved.
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
//   CLASS: TSLstNode
//  PREFIX: node
// ---------------------------------------------------------------------------
class CIDLIBEXP TSLstNode : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TSLstNode(const TSLstNode&) = delete;
        TSLstNode(TSLstNode&&) = delete;

        ~TSLstNode() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSLstNode& operator=(const TSLstNode&) = delete;
        TSLstNode& operator=(TSLstNode&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TSLstNode* pnodeNext() const noexcept
        {
            return m_pnodeNext;
        }


    protected   :
        // -------------------------------------------------------------------
        //  Our linked list class is our friend. Can you say friend? Sure!
        //  That's real nice.
        // -------------------------------------------------------------------
        friend class TSLinkedList;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TSLstNode() = default;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pnodeNext
        //      The next node of this node
        // -------------------------------------------------------------------
        TSLstNode*      m_pnodeNext = nullptr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSLstNode,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TSLinkedList
//  PREFIX: llst
// ---------------------------------------------------------------------------
class CIDLIBEXP TSLinkedList : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSLinkedList() = default;

        TSLinkedList(const TSLinkedList&) = delete;

        TSLinkedList
        (
                    TSLinkedList&&          listSrc
        );

        ~TSLinkedList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSLinkedList& operator=(const TSLinkedList&) = delete;

        TSLinkedList& operator=
        (
                    TSLinkedList&&          listSrv
        );

        tCIDLib::TBoolean operator==
        (
            const   TSLinkedList&           llstSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TSLinkedList&           llstSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendNode
        (
                    TSLstNode* const        pnodeToAppend
        );

        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TBoolean bIsFirstNode
        (
            const   TSLstNode* const        pnodeToTest
        )   const noexcept;

        tCIDLib::TBoolean bIsLastNode
        (
            const   TSLstNode* const        pnodeToTest
        )   const noexcept;

        tCIDLib::TBoolean bIsMemberNode
        (
            const   TSLstNode* const        pnodeToTest
        )   const;

        tCIDLib::TCard4 c4ElemCount() const noexcept
        {
            return m_c4ElemCount;
        }

        tCIDLib::TVoid InsertNode
        (
                    TSLstNode* const        pnodeAfter
            ,       TSLstNode* const        pnodeToInsert
        );

        tCIDLib::TVoid MoveToHead
        (
                    TSLstNode* const        pnodeToMove
        );

        tCIDLib::TVoid MoveToTail
        (
                    TSLstNode* const        pnodeToMove
        );

        TSLstNode* pnodeHead() const;

        TSLstNode* pnodeTail() const;

        tCIDLib::TVoid PrependNode
        (
                    TSLstNode* const        pnodeToPrepend
        );

        tCIDLib::TVoid RemoveAll();

        tCIDLib::TVoid RemoveNode
        (
                    TSLstNode* const        pnodeToRemove
        );

        tCIDLib::TVoid Reverse();


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
        //      is nullptr, then there are no elements in the list.
        //
        //  m_pnodeTail
        //      This is the pointer to the last node of the linked list. If it
        //      is nullptr, then there are no elements in the list.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ElemCount = 0;
        TSLstNode*              m_pnodeHead = nullptr;
        TSLstNode*              m_pnodeTail = nullptr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSLinkedList,TObject)
};

#pragma CIDLIB_POPPACK


