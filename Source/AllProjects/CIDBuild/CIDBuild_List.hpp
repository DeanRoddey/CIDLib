//
// FILE NAME: CIDBuild_List.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/1998
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
//  This is the header for the CIDBuild_List.Cpp file. This file implements
//  the TList class, which is used internally to maintain lists of things
//  that are accessed by file names. It is a template class so that it can be
//  easily instantiated for a number of different types.
//
// CAVEATS/GOTCHAS:
//
//  1)  We have to use some special out of line helpers here since we are one
//      of the first headers and some stuff isn't available to us inline.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
template <typename T> class TListCursor;


// ---------------------------------------------------------------------------
//   CLASS: TList
//  PREFIX: list
// ---------------------------------------------------------------------------
template <typename T> class TList
{
    public :
        // -------------------------------------------------------------------
        //  Nested cursor type
        // -------------------------------------------------------------------
        using TCursor = TListCursor<T>;


        // -------------------------------------------------------------------
        //  Constructors and Operators
        // -------------------------------------------------------------------
        TList(const tCIDLib::EAdoptOpts eAdopt = tCIDLib::EAdoptOpts::Adopt) :

            m_eAdopt(eAdopt)
            , m_pnodeHead(0)
            , m_c4ElemCount(0)
        {
        }

        TList(const TList&) = delete;
        TList(TList&&) = delete;

        ~TList()
        {
            RemoveAll();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TList& operator=(const TList&) = delete;
        TList& operator=(TList&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add(T* const ptToAdopt)
        {
            if (m_pnodeHead)
            {
                // Run up to the last node
                TNode* pnodeLast = m_pnodeHead;
                while (pnodeLast->pnodeNext)
                    pnodeLast = pnodeLast->pnodeNext;

                // Create a new one and add it as the next node for the last node
                TNode* pnodeNew = new TNode;
                pnodeNew->pnodeNext = nullptr;
                pnodeNew->ptElement = ptToAdopt;
                pnodeLast->pnodeNext = pnodeNew;
            }
             else
            {
                m_pnodeHead = new TNode;
                m_pnodeHead->pnodeNext = nullptr;
                m_pnodeHead->ptElement = ptToAdopt;
            }
            m_c4ElemCount++;
        }

        tCIDLib::TBoolean bEmpty() const
        {
            return (m_c4ElemCount == 0);
        }

        tCIDLib::TCard4 c4ElemCount() const
        {
            return m_c4ElemCount;
        }

        tCIDLib::EAdoptOpts eAdoptMode() const
        {
            return m_eAdopt;
        }

        tCIDLib::TVoid RemoveAll()
        {
            TNode* pnodeCur = m_pnodeHead;
            while (pnodeCur)
            {
                // Save the next node before we kill the current one
                TNode* pnodeNext = pnodeCur->pnodeNext;

                //
                //  If we are in Adopt mode, then kill the element. Kill the
                //  current node regardless.
                //
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    delete pnodeCur->ptElement;

                delete pnodeCur;
                pnodeCur = pnodeNext;
            }

            m_c4ElemCount = 0;
            m_pnodeHead = 0;
        }

        tCIDLib::TVoid Remove(T* const ptToRemove)
        {
            //
            //  Lets find the one to remove, keeping up with the node that
            //  came before it so that we can patch it out.
            //
            TNode* pnodeCur = m_pnodeHead;
            TNode* pnodePrev = 0;
            while (pnodeCur)
            {
                if (pnodeCur->ptElement == ptToRemove)
                    break;

                pnodePrev = pnodeCur;
                pnodeCur = pnodeCur->pnodeNext;
            }

            if (!pnodeCur)
            {
                tCIDBuild::LogMsg(L"Element to remove was not in the list");
                throw tCIDBuild::EErrors::NotFound;
            }

            //
            //  Patch around the node to remove. There are a couple of
            //  scenarios here to deal with.
            //
            if (!pnodePrev)
            {
                //
                //  There is no previous node, so its the first node. So we
                //  just point the head at its next node. If this was the
                //  only node, this will naturally zero the head out.
                //
                m_pnodeHead = pnodeCur->pnodeNext;
            }
             else
            {
                //
                //  There is a previous node, so set its next node to the
                //  next node of the current node.
                //
                pnodePrev->pnodeNext = pnodeCur->pnodeNext;
            }

            // And now clean up the element (if we adopted) and the node
            if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete pnodeCur->ptElement;
            delete pnodeCur;
        }


    private :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TListCursor<T>;


        // -------------------------------------------------------------------
        //  Private, nested types
        // -------------------------------------------------------------------
        struct TNode
        {
            T*              ptElement;
            TNode*          pnodeNext;
        };


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopt
        //      Indicates whether this list adopts its elements or just
        //      references them.
        //
        //  m_pnodeHead
        //      This is the head of the linked list.
        //
        //  m_c4ElemCount
        //      The number of elements in the map.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopt;
        TNode*              m_pnodeHead;
        tCIDLib::TCard4     m_c4ElemCount;
};


template <typename T> class TListCursor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TListCursor(const TList<T>* plistToCursor) :

            m_pnodeCur(nullptr)
            , m_plistTarget(plistToCursor)
        {
        }

        ~TListCursor()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        const T& operator*() const
        {
            return tCurElement();
        }

        T& operator*()
        {
            return tCurElement();
        }

        T* operator->()
        {
            return &tCurElement();
        }

        const T* operator->() const
        {
            return &tCurElement();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNext()
        {
            if (m_pnodeCur == nullptr)
                return kCIDLib::False;

            m_pnodeCur = m_pnodeCur->pnodeNext;
            return (m_pnodeCur != nullptr);
        }

        tCIDLib::TBoolean bResetIter()
        {
            m_pnodeCur = m_plistTarget->m_pnodeHead;
            return (m_pnodeCur != nullptr);
        }

        const T& tCurElement() const
        {
            if (!m_pnodeCur)
            {
                tCIDBuild::LogMsg(L"List cursor is invalid");
                throw tCIDBuild::EErrors::Internal;
            }
            return *m_pnodeCur->ptElement;
        }

        T& tCurElement()
        {
            if (!m_pnodeCur)
            {
                tCIDBuild::LogMsg(L"List cursor is invalid");
                throw tCIDBuild::EErrors::Internal;
            }
            return *m_pnodeCur->ptElement;
        }

        tCIDLib::TCard4 c4ElemCount() const
        {
            return m_plistTarget->uiElemCount();
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pnodeCur
        //      This is the current node with the current list that the
        //      iterator is pointing to.
        //
        //  m_plistTarget
        //      The list that we are cursoring.
        // -------------------------------------------------------------------
        typename TList<T>::TNode*   m_pnodeCur;
        const TList<T>*             m_plistTarget;
};

