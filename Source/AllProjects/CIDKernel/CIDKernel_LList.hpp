//
// FILE NAME: CIDKernel_LList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/08/2011
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
//  This is the header for the CIDKernel_LList.Cpp file. This file implements
//  a very simple singly linked list type templatized class. This is for those
//  places where kernel level APIs have to handle non-fixed sized lists. It
//  vastly simplifies things this way.
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
//   CLASS: TKrnlLList
//  PREFIX: kllist
// ---------------------------------------------------------------------------
template <class TElem> class TKrnlLList
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlLList() :

            m_c4ElemCount(0)
            , m_pCursor(nullptr)
            , m_pHead(nullptr)
            , m_pTail(nullptr)
        {
        }

        TKrnlLList(const TKrnlLList&) = delete;
        TKrnlLList(TKrnlLList&&) = delete;

        ~TKrnlLList()
        {
            FreeAll();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlLList& operator=(const TKrnlLList&) = delete;
        TKrnlLList& operator=(TKrnlLList&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  Check if we are at the end of an interation (or perhaps haven't
        //  started one yet.)
        //
        tCIDLib::TBoolean bAtEnd() const
        {
            return (m_pCursor == nullptr);
        }

        tCIDLib::TBoolean bIsEmpty() const
        {
            return (m_pHead == nullptr);
        }

        tCIDLib::TBoolean bNext(TElem*& pNew)
        {
            // If no cursor is set, then try to reset it. Else, move to the next one
            if (m_pCursor)
                m_pCursor = m_pCursor->m_pNext;
            else
                m_pCursor = m_pHead;

            // If we are done, then give up now
            if (!m_pCursor)
            {
                pNew = nullptr;
                return kCIDLib::False;
            }

            // Give back a pointer to the current one
            pNew = m_pCursor->m_pobjVal;
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bResetCursor()
        {
            //
            //  Null the cursor pointer. Return if the list is empty or not, so they
            //  don't bother iterating if nothing's there.
            //
            m_pCursor = nullptr;
            return (m_pHead != nullptr);
        }

        tCIDLib::TCard4 c4ElemCount() const
        {
            return m_c4ElemCount;
        }

        TElem* pobjAddNew(TElem* const pobjToAdd)
        {
            // Remember the current tail
            TNode* pOldTail = m_pTail;

            // Put in a new tail, which will have no next pointer
            m_pTail = new TNode(pobjToAdd);

            // If we had an old tail, make this guy its next now
            if (pOldTail)
                pOldTail->m_pNext = m_pTail;

            // If we had no head, this is the first, so save it as the head
            if (!m_pHead)
                m_pHead = m_pTail;

            m_c4ElemCount++;
            return pobjToAdd;
        }

        const TElem* pobjHead() const
        {
            if (!m_pHead)
                return nullptr;
            return m_pHead->m_pobjVal;
        }

        TElem* pobjHead()
        {
            if (!m_pHead)
                return nullptr;
            return m_pHead->m_pobjVal;
        }

        [[nodiscard]] TElem* pobjOrphanCur()
        {
            // If the cursor isn't valid, return null
            if (!m_pCursor)
                return nullptr;

            //
            //  We can't change the cursor yet, but remember what the next
            //  node will be so we can store it later, before we modify the
            //  cursor node. If there is no next node, then the cursor will
            //  end up null, which will return false on the next iteration.
            //
            TNode* pNextCursor = m_pCursor->m_pNext;

            // Get the return object out and clear the node pointer to orphan it
            TElem* pRet = m_pCursor->m_pobjVal;
            m_pCursor->m_pobjVal = nullptr;

            if (m_pCursor == m_pHead)
            {
                //
                //  We are removing the head, so just make the head the next
                //  pointer. If there isn't one, then we end up empty.
                //
                m_pHead = m_pHead->m_pNext;
            }
             else
            {
                //
                //  This is the heavy scenario. We have to run up through the
                //  list to find the previous node so that we can patch around
                //  the one being removed.
                //
                TNode* pPrev = m_pHead;
                while (pPrev->m_pNext != m_pCursor)
                    pPrev = pPrev->m_pNext;

                // Ok, now patch around the cursor node
                pPrev->m_pNext = m_pCursor->m_pNext;
            }

            // And now we can delete the node and store the new one
            delete m_pCursor;
            m_pCursor = pNextCursor;

            m_c4ElemCount--;

            // Give back the orphaned user object
            return pRet;
        }

        tCIDLib::TVoid RemoveAll()
        {
            FreeAll();
        }

        tCIDLib::TVoid RemoveCurrent()
        {
            // Orphan the current. If we get one, delete it
            TElem* pobjRemove = pobjOrphanCur();
            if (pobjRemove)
            {
                delete pobjRemove;
                m_c4ElemCount--;
            }
        }


    private :
        // -------------------------------------------------------------------
        //  Our little node class
        // -------------------------------------------------------------------
        class TNode
        {
            public :
                TNode(TElem* const pobjVal) :

                    m_pNext(nullptr)
                    , m_pobjVal(pobjVal)
                {
                }

                TNode(const TNode&) = delete;
                TNode(TNode&&) = delete;

                ~TNode()
                {
                    if (m_pobjVal)
                    {
                        delete m_pobjVal;
                        m_pobjVal = nullptr;
                    }
                }

                TNode& operator=(const TNode&) = delete;
                TNode& operator=(TNode&&) = delete;

                TNode*  m_pNext;
                TElem*  m_pobjVal;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FreeAll() noexcept
        {
            TNode* pCur = m_pHead;

            m_pCursor = nullptr;
            m_pHead = nullptr;
            m_pTail = nullptr;

            while (pCur)
            {
                TNode* pSaveN = pCur->m_pNext;
                delete pCur;
                pCur = pSaveN;
            }

            m_c4ElemCount = 0;
        }

        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ElemCount
        //      The current count of elements, to avoid having to iterate the list to
        //      figure that out.
        //
        //  m_pCursor
        //      Our internal cursor node pointer. These are not intended for
        //      fancy use so an internal cursor is fine.
        //
        //  m_pHead
        //      A pointer to the head of the list
        //
        //  m_pTail
        //      A pointer to the last node in the list, to make adding new ones
        //      far more efficient.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4ElemCount;
        TNode*          m_pCursor;
        TNode*          m_pHead;
        TNode*          m_pTail;
};

#pragma CIDLIB_POPPACK

