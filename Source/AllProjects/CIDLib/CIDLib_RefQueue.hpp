//
// FILE NAME: CIDLib_RefQueue.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/21/2000
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
//  This is the header for the 'by ref' queue collection template. It
//  implements the TQueue class template, which provides LIFO and priority
//  based storage of its elements by reference.
//
// CAVEATS/GOTCHAS:
//
//  1)  Unlike most other by ref collections, this guy cannot use the standard
//      double linked list collection class, TBasicDLinkedRefCol. So it has a
//      lot more code than they do.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward reference the ref queue and queue cursor classes
// ---------------------------------------------------------------------------
template <typename TElem> class TRefQueue;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TRefQueueNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <typename TElem> class TRefQueueNode : public TDLstNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefQueueNode() = delete;

        TRefQueueNode(  TElem* const                pobjData
                        , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0) :

            m_ePriority(ePriority)
            , m_pobjData(pobjData)
        {
        }

        TRefQueueNode(const TRefQueueNode<TElem>&) = delete;
        TRefQueueNode(TRefQueueNode<TElem>&&) = delete;

        ~TRefQueueNode()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRefQueueNode<TElem>& operator=(const TRefQueueNode<TElem>&) = delete;
        TRefQueueNode<TElem>& operator=(TRefQueueNode<TElem>&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DropData(const tCIDLib::EAdoptOpts eAdopt)
        {
            if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete m_pobjData;
            m_pobjData = nullptr;
        }

        tCIDLib::EQPrios ePriority()
        {
            return m_ePriority;
        }

        const TElem* pobjData() const
        {
            return m_pobjData;
        }

        TElem* pobjData()
        {
            return m_pobjData;
        }

        [[nodiscard]] TElem* pobjOrphan()
        {
            TElem* pobjRet = m_pobjData;
            m_pobjData = nullptr;
            return pobjRet;
        }

        tCIDLib::TVoid SetData(         TElem* const        pobjData
                                , const tCIDLib::EAdoptOpts eAdopt)
        {
            if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete m_pobjData;
            m_pobjData = pobjData;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ePriority
        //      The priority of this node.
        //
        //  m_pobjData
        //      A pointer to the data object of this node.
        // -------------------------------------------------------------------
        tCIDLib::EQPrios    m_ePriority;
        TElem*              m_pobjData;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TRefQueueNode<TElem>,TDLstNode)
};



// ---------------------------------------------------------------------------
//   CLASS: TRefQueue
//  PREFIX: que
// ---------------------------------------------------------------------------
template <typename TElem> class TRefQueue : public TRefCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Class type aliases
        // -------------------------------------------------------------------
        using TMyElemType   = TElem;
        using TMyType       = TRefQueue<TElem>;
        using TNode         = TRefQueueNode<TElem>;
        using TObjEqFunc    = tCIDLib::TBoolean (*)(const TElem&, const TElem&);


        // -------------------------------------------------------------------
        //  Our nested cursor classes
        // -------------------------------------------------------------------
        template <typename TElem> class TConstCursor : public TBiColCursor<TElem>
        {
            public  :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TConstCursor() :

                    m_pcolCursoring(nullptr)
                    , m_pllstCursoring(nullptr)
                    , m_pnodeCur(nullptr)
                {
                }

                explicit TConstCursor(const TMyType* const pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_pcolCursoring(pcolToCursor)
                    , m_pllstCursoring(&pcolToCursor->llstQueue())
                    , m_pnodeCur(nullptr)
                {
                    m_pnodeCur = static_cast<TRefQueueNode<TElem>*>
                    (
                        m_pllstCursoring->pnodeHead()
                    );
                }

                // We have to lock first, so we can't use member init!
                TConstCursor(const TConstCursor& cursSrc)
                {
                    operator=(cursSrc);
                }

                ~TConstCursor()
                {
                }


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TConstCursor& operator=(const TConstCursor& cursSrc)
                {
                    if (this != &cursSrc)
                    {
                        TLocker lockrCol(cursSrc.m_pcolCursoring);
                        TParent::operator=(cursSrc);
                        m_pcolCursoring = cursSrc.m_pcolCursoring;
                        m_pllstCursoring = cursSrc.m_pllstCursoring;
                        m_pnodeCur = cursSrc.m_pnodeCur;
                    }
                    return *this;
                }

                tCIDLib::TBoolean operator==(const TConstCursor& cursSrc) const
                {
                    if (!TParent::operator==(cursSrc))
                        return kCIDLib::False;
                    return (m_pnodeCur == cursSrc.m_pnodeCur);
                }

                tCIDLib::TBoolean operator!=(const TConstCursor& cursSrc) const
                {
                    return !TConstCursor::operator==(cursSrc);
                }

                TConstCursor& operator++()
                {
                    this->bNext();
                    return *this;
                }

                TConstCursor operator++(int)
                {
                    TConstCursor cursTmp(*this);
                    this->bNext();
                    return cursTmp;
                }

                // -----------------------------------------------------------
                //  Public, inherited methods
                // -----------------------------------------------------------
                tCIDLib::TBoolean bIsValid() const final
                {
                    if (!TParent::bIsValid())
                        return kCIDLib::False;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bNext() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TRefQueueNode<TElem>*>(m_pnodeCur->pnodeNext());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bPrevious() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TRefQueueNode<TElem>*>(m_pnodeCur->pnodePrev());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bSeekToEnd() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    m_pnodeCur = static_cast<TRefQueueNode<TElem>*>
                    (
                        m_pllstCursoring->pnodeTail()
                    );
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    m_pnodeCur = static_cast<TRefQueueNode<TElem>*>
                    (
                        m_pllstCursoring->pnodeHead()
                    );
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    return (m_pnodeCur != nullptr);
                }

                const TElem& objRCur() const final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(m_pnodeCur, CID_FILE, CID_LINE);
                    return *pnodeCur()->pobjData();
                }

                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                const TNode* pnodeCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }

                TNode* pnodeCur()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }


            protected   :
                // -----------------------------------------------------------
                //  Declare our friends
                // -----------------------------------------------------------
                friend class TMyType;


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolCursoring
                //      This is a pointer to the queue collection we are cursoring.
                //
                //  m_pllstCursoring
                //      This is the linked list we are cursoring. It is extracted from
                //      the queue during construction for fast access.
                //
                //  m_pnodeCur
                //      This is the current node that we are on in our iteration. It is
                //      0 if we are at the end.
                // -----------------------------------------------------------
                const TMyType*      m_pcolCursoring;
                const TDLinkedList* m_pllstCursoring;
                TNode*              m_pnodeCur;


                // -----------------------------------------------------------
                //  Do any needed magic macros
                // -----------------------------------------------------------
                TemplateRTTIDefs(TMyType::TConstCursor<TElem>, TBiColCursor<TElem>)
        };

        template <typename TElem> class TNonConstCursor : public TConstCursor<TElem>
        {
            public  :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TNonConstCursor() :

                    m_pcolNCCursoring(nullptr)
                {
                }

                explicit TNonConstCursor(TMyType* const pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_pcolNCCursoring(pcolToCursor)
                {
                }

                // We have to lock first, so we can't use member init!
                TNonConstCursor(const TNonConstCursor& cursSrc)
                {
                    operator=(cursSrc);
                }

                ~TNonConstCursor()
                {
                }


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TNonConstCursor& operator=(const TNonConstCursor& cursSrc)
                {
                    if (this != &cursSrc)
                    {
                        TLocker lockrCol(cursSrc.m_pcolNCCursoring);
                        TParent::operator=(cursSrc);
                        m_pcolNCCursoring = cursSrc.m_pcolNCCursoring;
                    }
                    return *this;
                }

                TElem& operator*() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return *const_cast<TElem*>(this->pnodeCur()->pobjData());
                }

                TElem* operator->() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return const_cast<TElem*>(this->pnodeCur()->pobjData());
                }

                TNonConstCursor& operator++()
                {
                    this->bNext();
                    return *this;
                }

                TNonConstCursor operator++(int)
                {
                    TNonConstCursor cursTmp(*this);
                    this->bNext();
                    return cursTmp;
                }


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                TElem& objWCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return *const_cast<TElem*>(this->pnodeCur()->pobjData());
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolNCCursoring
                //      This is a pointer to the queue collection we are
                //      cursoring. We need our own non-const pointer.
                // -----------------------------------------------------------
                TMyType*    m_pcolNCCursoring;


                // -----------------------------------------------------------
                //  Do any needed magic macros
                // -----------------------------------------------------------
                TemplateRTTIDefs
                (
                    TMyType::TNonConstCursor<TElem>
                    , TMyType::TConstCursor<TElem>
                )
        };


        // -------------------------------------------------------------------
        //  Typedefs for our nested cursors
        // -------------------------------------------------------------------
        using TCursor       = TConstCursor<TElem>;
        using TNCCursor     = TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefQueue(  const   tCIDLib::EAdoptOpts eAdopt = tCIDLib::EAdoptOpts::Adopt
                    , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TRefCollection<TElem>(eMTSafe)
            , m_eAdopt(eAdopt)
            , m_llstQueue()
        {
        }

        TRefQueue(const TRefQueue<TElem>&) = delete;
        TRefQueue(TRefQueue<TElem>&&) = delete;

        ~TRefQueue() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRefQueue<TElem>& operator=(const TRefQueue<TElem>&) = delete;
        TRefQueue<TElem>& operator=(TRefQueue<TElem>&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add(TElem* const pobjNew) final
        {
            // This is just passed on to the Put method
            Put(pobjNew);
        }

        tCIDLib::TBoolean bIsEmpty() const final
        {
            TLocker lockrQueue(this);
            return m_llstQueue.bIsEmpty();
        }

        tCIDLib::TCard4 c4ElemCount() const final
        {
            TLocker lockrQueue(this);
            return m_llstQueue.c4ElemCount();
        }

        tCIDLib::EAdoptOpts eAdopt() const final
        {
            return m_eAdopt;
        }

        tCIDLib::TVoid GiveAllTo(TRefCollection<TElem>& colTarget) final
        {
            // Look and add all of our items to the target
            TLocker lockrThis(this);

            // Orphan all the data objects over to the target collection
            TNode* pnodeCur = static_cast<TNode*>(m_llstQueue.pnodeHead());
            while (pnodeCur)
            {
                colTarget.Add(pnodeCur->pobjOrphan());
                pnodeCur = static_cast<TNode*>(pnodeCur->pnodeNext());
            }

            // And now clear out the linked list nodes
            m_llstQueue.RemoveAll();

            // And invalidate cursors
            this->c4IncSerialNum();

            // Wake up all threads waiting for space
            m_twlWaiters.bReleaseAll(kCIDLib::c4TWLReason_WaitSpace);
        }

        tCIDLib::TVoid OrphanElem(TElem* const pobjToRemove) final
        {
            TLocker lockrQueue(this);

            // Get the node to flush
            TNode* pnodeToRemove = pnodeFindObj(pobjToRemove);

            // Make sure the cursor's valid
            if (!pnodeToRemove)
                this->NotMemberNode(CID_FILE, CID_LINE);

            //
            //  Orphan the object out of the node, then remove the node. We have to
            //  supress the no-discard error by cast to voide
            //
            static_cast<void>(pnodeToRemove->pobjOrphan());
            m_llstQueue.RemoveNode(pnodeToRemove);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Wake up one thread waiting for space
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
        }

        [[nodiscard]] TCursor* pcursNew() const final
        {
            TLocker lockrQueue(this);
            return new TCursor(this);
        }

        tCIDLib::TVoid RemoveAll() final
        {
            TLocker lockrQueue(this);
            if (m_llstQueue.bIsEmpty())
                return;
            m_llstQueue.RemoveAll();
            this->c4IncSerialNum();

            // Wake up all threads waiting for space
            m_twlWaiters.bReleaseAll(kCIDLib::c4TWLReason_WaitSpace);
        }

        tCIDLib::TVoid RemoveElem(TElem* const pobjToRemove) final
        {
            TLocker lockrQueue(this);

            // Get the node to flush
            TNode* pnodeToRemove = pnodeFindObj(pobjToRemove);

            // Make sure the cursor's valid
            if (!pnodeToRemove)
                this->NotMemberNode(CID_FILE, CID_LINE);

            //
            //  And flush the node. Ask it to drop the data first. If we
            //  own the data, this will delete it.
            //
            pnodeToRemove->DropData(m_eAdopt);
            m_llstQueue.RemoveNode(pnodeToRemove);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Wake up one thread waiting for space
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  If the passed object matches the newest (last queued) object,
        //  then it will replace that object. Else it will be added as new
        //  node. The return indicates if we added it as a new object or not.
        //
        //  If we replace the newest element, and we are non-adopting, then
        //  the one we replaced is returned in pobjOld. If we are adopting,
        //  or we didn't get a match (i.e. we added a new one), then pobjOld
        //  is set to zero.
        //
        tCIDLib::TBoolean
        bAddOrUpdateNewest(         TElem* const        pobjNew
                            ,       TObjEqFunc          pfnComp
                            ,       TElem*&             pobjOld
                            , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            if (!pobjNew)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            TLocker lockrQueue(this);

            //
            //  Get the tail node, if any. If there is one, and it matches
            //  the passed object.
            //
            TNode* pnodeCur = static_cast<TNode*>(m_llstQueue.pnodeTail());
            CIDLib_Suppress(6011)  // We null checked above
            if (pnodeCur && pfnComp(*pnodeCur->pobjData(), *pobjNew))
            {
                if (m_eAdopt == tCIDLib::EAdoptOpts::NoAdopt)
                    pobjOld = pnodeCur->pobjOrphan();
                else
                    pobjOld = nullptr;

                pnodeCur->SetData(pobjNew, m_eAdopt);
                return kCIDLib::False;
            }

            // There was no match, so add it as a new one
            pobjOld = nullptr;
            Put(pobjNew, ePriority);
            return kCIDLib::True;
        }


        template <typename IterCB> tCIDLib::TBoolean bForEachNC(IterCB iterCB)
        {
            TLocker lockrThis(this);
            TQueueNode<TElem>* pnodeCur = static_cast<TQueueNode<TElem>*>
            (
                m_llstQueue.pnodeHead()
            );
            while (pnodeCur)
            {
                if (!iterCB(pnodeCur->objData()))
                    return kCIDLib::False;
                pnodeCur = static_cast<TQueueNode<TElem>*>(pnodeCur->pnodeNext());
            }
            return kCIDLib::True;
        }


        tCIDLib::TBoolean
        bPutIfNew(          TElem* const        pobjToPut
                    , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            if (!pobjToPut)
                this->NullNodeAdded(CID_FILE, CID_LINE);
            TLocker lockrQueue(this);

           //
            //  See if this element is already in the queue. If so, return
            //  false. If we are adopting, then delete the incoming.
            //
            if (pnodeFindObj(pobjToPut))
            {
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    delete pobjToPut;
                return kCIDLib::False;
            }

            // Add the new node
            m_llstQueue.AppendNode(new TNode(pobjToPut, ePriority));

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  Tell the thread wait list to wake up one thread to handle this
            //  new element added.
            //
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitData);

            // We added it so return true
            return kCIDLib::True;
        }

        tCIDLib::TBoolean
        bPutIfNew(          TElem* const        pobjToPut
                    ,       TObjEqFunc          pfnComp
                    , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            if (!pobjToPut)
                this->NullNodeAdded(CID_FILE, CID_LINE);
            TLocker lockrQueue(this);

            //
            //  See if another element in the queue matches this one. If so,
            //  we don't add the new one. If we are adopting, we delete the
            //  incoming.
            //
            CIDLib_Suppress(6011)  // We null checked above
            TNode* pnodeCur = pnodeFindObj(*pobjToPut, pfnComp);
            if (pnodeCur)
            {
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    delete pobjToPut;
                return kCIDLib::False;
            }

            // Add the new node
            m_llstQueue.AppendNode(new TNode(pobjToPut, ePriority));

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  Tell the thread wait list to wake up one thread to handle this
            //  new element added.
            //
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitData);

            // We added it so return true
            return kCIDLib::True;
        }


        //
        //  Waits for up to the indicated time for space to become available,
        //  though of course the caller must still be careful since it might
        //  not be true by the time they react, unless they have locked or
        //  only one thread is adding to the queue.
        //
        tCIDLib::TBoolean
        bWaitForSpace(const tCIDLib::TCard4 c4Millis, const tCIDLib::TCard4 c4Limit)
        {
            TLocker lockrQueue(this);

            // If it's not 'full', then return true now
            if (m_llstQueue.c4ElemCount() < c4Limit)
                return kCIDLib::True;

            //
            //  It's full, so put us on the wait list with the wait space
            //  code to wait for some space to become available.
            //
            return m_twlWaiters.bWaitOnList
            (
                lockrQueue, kCIDLib::c4TWLReason_WaitSpace, c4Millis
            );
        }


        [[nodiscard]] TRefQueue<TElem>* pcolMakeNewOf() const
        {
            TLocker lockrQueue(this);

            // Make a new one with the same basic state, but not content!
            return new TRefQueue<TElem>(m_eAdopt);
        }


        [[nodiscard]] TElem*
        pobjGetNext(const   tCIDLib::TCard4     c4Millis = kCIDLib::c4MaxCard
                    , const tCIDLib::TBoolean   bThrowTimeout = kCIDLib::True)
        {
            // Lock the queue
            TLocker lockrQueue(this);

            // Call a helper to wait for something to show up
            tCIDLib::TBoolean bGotOne = TCollectionBase::bWaitForData
            (
                lockrQueue, *this, c4Millis, m_twlWaiters, bThrowTimeout
            );

            //
            //  If we gone one, get it out, else return null. We have the
            //  lock again at this point.
            //
            TElem* pobjRet = nullptr;
            if (bGotOne)
            {
                // Get the head node
                TNode* pnodeHead = static_cast<TNode*>(m_llstQueue.pnodeHead());

                // Orphan the object out of it and then flush the node
                pobjRet = pnodeHead->pobjOrphan();
                m_llstQueue.RemoveNode(pnodeHead);

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // Wake up one thread waiting for space
                m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
            }

            // Return the object
            return pobjRet;
        }

        //
        //  This method just looks at the next available element, and sees if
        //  it matches the passed value (using the passed comparator.) If
        //  so, then it gets the next element and returns it. Else it just
        //  returns null.
        //
        //  This ones doesn't wait, since if there's nothing in the queue to
        //  compare against, it doesn't have much meaning. So it returns
        //  immediately if nothing is available.
        //
        TElem*
        pobjGetNextIfMatches(const TElem& objComp, TObjEqFunc pfnComp)
        {
            // Lock the queue
            TLocker lockrQueue(this);

            // Get the head node. If none, nothing to do
            TNode* pnodeHead = static_cast<TNode*>(m_llstQueue.pnodeHead());
            if (!pnodeHead)
                return nullptr;

            // Compare it. If no match, return zero
            if (!pfnComp(*pnodeHead->pobjData(), objComp))
                return nullptr;

            // Orphan the object out of it and then flush the node
            TElem* pobjRet = pnodeHead->pobjOrphan();
            m_llstQueue.RemoveNode(pnodeHead);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Wake up one thread waiting for space
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);

            return pobjRet;
        }

        [[nodiscard]] TElem*
        pobjGetHighestPrio(const tCIDLib::TCard4 c4Millis = kCIDLib::c4MaxCard)
        {
            // Lock the queue
            TLocker lockrQueue(this);

            // Call a helper to wait for something to show up
            tCIDLib::TBoolean bGotOne = TCollectionBase::bWaitForData
            (
                lockrQueue, *this, c4Millis, m_twlWaiters, kCIDLib::True
            );

            //
            //  If we gone one, get it out, else return null. We have the
            //  lock again at this point.
            //
            TElem* pobjRet = nullptr;
            if (bGotOne)
            {
                //
                //  Get the highest priority node. We have to loop through and
                //  find the one with the highest priority.
                //
                tCIDLib::EQPrios    eHighest    = tCIDLib::EQPrios::P0;
                TNode*              pnodeCur    = (TNode*)m_llstQueue.pnodeHead();
                TNode*              pnodeHighest= pnodeCur;
                while (pnodeCur)
                {
                    if (pnodeCur->ePriority() > eHighest)
                    {
                        eHighest = pnodeCur->ePriority();
                        pnodeHighest = pnodeCur;
                    }
                    pnodeCur = static_cast<TNode*>(pnodeCur->pnodeNext());
                }

                // If 0, then none, so log an error since there should be
                if (!pnodeHighest)
                    this->ColIsEmpty(CID_FILE, CID_LINE);

                // Orphan the data object out
                pobjRet = pnodeHighest->pobjOrphan();

                // Delete the node that we found
                m_llstQueue.RemoveNode(pnodeHighest);

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // Wake up one thread waiting for space
                m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
            }

            // And return the object
            return pobjRet;
        }

        [[nodiscard]] TElem* pobjOrphanNewest()
        {
            TLocker lockrQueue(this);

            TNode* pnodeLast = static_cast<TNode*>(m_llstQueue.pnodeTail());
            if (!pnodeLast)
                return nullptr;

            // Orphan the object out of the node, then remove the node.
            TElem* pobjRet = pnodeLast->pobjOrphan();
            m_llstQueue.RemoveNode(pnodeLast);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Wake up one thread waiting for space
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);

            return pobjRet;
        }

        tCIDLib::TVoid
        Put(        TElem* const        pobjToPut
            , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            if (!pobjToPut)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            TLocker lockrQueue(this);

            // Add the new node
            m_llstQueue.AppendNode(new TNode(pobjToPut, ePriority));

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  Tell the thread wait list to wake up one thread to handle
            //  this new element added.
            //
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitData);
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TLocker lockrQueue(this);

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            // Get the node to remove and move the cursor past it
            TNode* pnodeToRemove = cursAt.pnodeCur();
            cursAt.bNext();

            //
            //  And flush the node. Ask it to drop the data first. If we
            //  own the data, this will delete it.
            //
            pnodeToRemove->DropData(m_eAdopt);
            m_llstQueue.RemoveNode(pnodeToRemove);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Get the cursor back into sync.
            cursAt.c4SerialNum(this->c4SerialNum());

            // Wake up one thread waiting for space
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TConstCursor<TElem>;
        friend class TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        const TDLinkedList& llstQueue() const
        {
            return m_llstQueue;
        }

        TDLinkedList& llstQueue()
        {
            return m_llstQueue;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        // Find the passed object by equal pointers
        TNode* pnodeFindObj(TElem* const pobjToFind)
        {
            TNode* pnodeCur = (TNode*)m_llstQueue.pnodeHead();
            while (pnodeCur)
            {
                if (pnodeCur->pobjData() == pobjToFind)
                    return pnodeCur;
                pnodeCur = static_cast<TNode*>(pnodeCur->pnodeNext());
            }
            return nullptr;
        }

        // Find a match of the passed object by a comparator function
        TNode* pnodeFindObj(const   TElem&              objToFind
                            ,       TObjEqFunc          pfnComp)
        {
            TNode* pnodeCur = (TNode*)m_llstQueue.pnodeHead();
            while (pnodeCur)
            {
                if ((*pfnComp)(*pnodeCur->pobjData(), objToFind))
                    return pnodeCur;
                pnodeCur = static_cast<TNode*>(pnodeCur->pnodeNext());
            }
            return nullptr;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopt
        //      This indicates whether the data objects are adopted or just
        //      referenced. If adopted, we clean them up as required.
        //
        //  m_llstQueue
        //      The linked list that provides the storage for this collection.
        //
        //  m_twlWaiters
        //      This is the thread wait list that is used by threads to wait
        //      on input to arrive in the queue.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopt;
        TDLinkedList        m_llstQueue;
        TThreadWaitList     m_twlWaiters;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TRefQueue<TElem>,TRefCollection<TElem>)
};


#pragma CIDLIB_POPPACK

