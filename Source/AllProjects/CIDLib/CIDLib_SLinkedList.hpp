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

        ~TSLstNode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSLstNode& operator=(const TSLstNode&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TSLstNode* pnodeNext() const;


    protected   :
        // -------------------------------------------------------------------
        //  Our linked list class is our friend. Can you say friend? Sure!
        //  That's real nice.
        // -------------------------------------------------------------------
        friend class TSLinkedList;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TSLstNode();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pnodeNext
        //      The next node of this node
        // -------------------------------------------------------------------
        TSLstNode*      m_pnodeNext;


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
        TSLinkedList();

        TSLinkedList
        (
            const   TSLinkedList&           colSrc
        );

        ~TSLinkedList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSLinkedList& operator=(const TSLinkedList&) = delete;

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
        )   const;

        tCIDLib::TBoolean bIsLastNode
        (
            const   TSLstNode* const        pnodeToTest
        )   const;

        tCIDLib::TBoolean bIsMemberNode
        (
            const   TSLstNode* const        pnodeToTest
        )   const;

        tCIDLib::TCard4 c4ElemCount() const;

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
        //  m_pnodeTail
        //      This is the pointer to the last node of the linked list. If it
        //      is 0, then there are no elements in the list.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ElemCount;
        TSLstNode*              m_pnodeHead;
        TSLstNode*              m_pnodeTail;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSLinkedList,TObject)
};

#pragma CIDLIB_POPPACK


