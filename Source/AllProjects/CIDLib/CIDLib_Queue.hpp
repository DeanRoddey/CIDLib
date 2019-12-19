//
// FILE NAME: CIDLib_Queue.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/1996
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
//  This is the header for the queue collection template. It implements the
//  TQueue class template, which provides LIFO and priority based storage of
//  its elements by value.
//
//  Because of the way queues are often used, to support thread farms, it
//  is much more thread conscious than most. It allows threads to call in
//  and block until data is ready or until the queue is not full. It uses
//  the thread wait list class to support this. We use the reason codes with
//  the wait list.
//
//  The queue being 'full' of course is up to the caller, none of our collections
//  have a built in max size. The bIsFull(x) method of the base collection
//  class can be used as a check. Our bWaitForSpace() also takes a size to
//  check.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Unlike TBag, TDeque, and TStack, this guy cannot use the standard double
//       linked list collection class, TBasicDLinkedCol. So it has a lot more
//      code than they do.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward reference the queue and queue cursor classes
// ---------------------------------------------------------------------------
template <typename TElem> class TQueue;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TQueueNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <typename TElem> class TQueueNode : public TDLstNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TQueueNode() = delete;

        TQueueNode( const   TElem&              objData
                    , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0) :

            m_ePriority(ePriority)
            , m_objData(objData)
        {
        }

        TQueueNode(         TElem&&             objData
                    , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0) :

            m_ePriority(ePriority)
            , m_objData(tCIDLib::ForceMove(objData))
        {
        }

        //
        //  A special one for in place elements. We can't figure out the next
        //  node until after this object is built and the element constructed.
        //
        template <typename... TArgs>
        TQueueNode(const tCIDLib::EQPrios ePriority, TArgs&&... Args) :

            m_ePriority(ePriority)
            , m_objData(tCIDLib::Forward<TArgs>(Args)...)
        {
        }

        TQueueNode(const TQueueNode<TElem>&) = delete;

        ~TQueueNode() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TQueueNode<TElem>& operator=(const TQueueNode<TElem>&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EQPrios ePriority()
        {
            return m_ePriority;
        }

        const TElem& objData() const
        {
            return m_objData;
        }

        TElem& objData()
        {
            return m_objData;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ePriority
        //      The priority of this node.
        //
        //  m_objData
        //      A copy of the data object we were given during ctor
        // -------------------------------------------------------------------
        tCIDLib::EQPrios    m_ePriority;
        TElem               m_objData;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TQueueNode<TElem>,TDLstNode)
};



// ---------------------------------------------------------------------------
//   CLASS: TQueue
//  PREFIX: que
// ---------------------------------------------------------------------------
template <typename TElem> class TQueue : public TCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested aliases for the cursor and node types used by a queue.
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = TQueue<TElem>;
        using TNode = TQueueNode<TElem>;


        // -------------------------------------------------------------------
        //  Our nested cursor classes
        // -------------------------------------------------------------------
        template <typename TElem> class TConstCursor : public TBiColCursor<TElem>
        {
            public  :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TConstCursor<TElem>() :

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
                    m_pnodeCur = static_cast<TQueueNode<TElem>*>
                    (
                        m_pllstCursoring->pnodeHead()
                    );
                }

                // We have to lock first, so we can't use member init! Just call operator
                TConstCursor(const TConstCursor& cursSrc)
                {
                    operator=(cursSrc);
                }

                // Can't actually delete it since that causes problems
                // TConstCursor(TConstCursor&&) = delete;

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
                        TMtxLocker lockCol
                        (
                            cursSrc.m_pcolCursoring
                            ? cursSrc.m_pcolCursoring->pmtxLock() : nullptr
                        );

                        TParent::operator=(cursSrc);
                        m_pcolCursoring = cursSrc.m_pcolCursoring;
                        m_pllstCursoring = cursSrc.m_pllstCursoring;
                        m_pnodeCur = cursSrc.m_pnodeCur;
                    }
                    return *this;
                }

                // Can't actually delete it since that causes problems
                // TConstCursor& operator=(TConstCursor&&) = delete;

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

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TQueueNode<TElem>*>(m_pnodeCur->pnodeNext());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bPrevious() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TQueueNode<TElem>*>
                    (
                        m_pnodeCur->pnodePrev()
                    );
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bSeekToEnd() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    m_pnodeCur = static_cast<TQueueNode<TElem>*>
                    (
                        m_pllstCursoring->pnodeTail()
                    );
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    m_pnodeCur = static_cast<TQueueNode<TElem>*>
                    (
                        m_pllstCursoring->pnodeHead()
                    );
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    return (m_pnodeCur != nullptr);
                }

                const TElem& objRCur() const final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(m_pnodeCur, CID_FILE, CID_LINE);
                    return m_pnodeCur->objData();
                }


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                const TNode* pnodeCur() const
                {
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }

                TNode* pnodeCur()
                {
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
                //      null if we are at the end.
                // -----------------------------------------------------------
                const TMyType*      m_pcolCursoring;
                const TDLinkedList* m_pllstCursoring;
                TNode*              m_pnodeCur;


                // -----------------------------------------------------------
                //  Do any needed magic macros
                // -----------------------------------------------------------
                TemplateRTTIDefs
                (
                    TMyType::TConstCursor<TElem>
                    , TBiColCursor<TElem>
                )
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

                explicit TNonConstCursor(TQueue* const pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_pcolNCCursoring(pcolToCursor)
                {
                }

                // We have to lock first, so we can't use member init! Just call operator
                TNonConstCursor(const TNonConstCursor& cursSrc)
                {
                    operator=(cursSrc);
                }

                // Can't actually delete it since that causes problems
                // TNonConstCursor(TNonConstCursor&&) = delete;

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
                        TMtxLocker lockCol
                        (
                            cursSrc.m_pcolNCCursoring
                            ? cursSrc.m_pcolNCCursoring->pmtxLock() : nullptr
                        );
                        TParent::operator=(cursSrc);
                        m_pcolNCCursoring = cursSrc.m_pcolNCCursoring;
                    }
                    return *this;
                }

                // Can't actually delete it since that causes problems
                // TNonConstCursor& operator=(TNonConstCursor&&) = delete;

                TElem& operator*() const
                {
                    TMtxLocker lockCol(m_pcolNCCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return const_cast<TElem&>(this->pnodeCur()->objData());
                }

                TElem* operator->() const
                {
                    TMtxLocker lockCol(m_pcolNCCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return &const_cast<TElem&>(this->pnodeCur()->objData());
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

                    // Lock the collection
                    TMtxLocker lockCol(m_pcolNCCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return const_cast<TElem&>(this->pnodeCur()->objData());
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolNCCursoring
                //      This is a pointer to the queue collection we are
                //      cursoring. We need our own non-const copy of the col.
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
        //  Alias our nested cursor classes
        // -------------------------------------------------------------------
        using TCursor   = TConstCursor<TElem>;
        using TNCCursor = TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TQueue(const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollection<TElem>(eMTSafe)
            , m_llstQueue()
        {
        }

        TQueue(const TMyType& colSrc) :

            TCollection<TElem>(colSrc)
        {
            // Lock the source collection so it won't change during this
            TMtxLocker lockSrc(colSrc.pmtxLock());

            // And copy any source info now that it's locked
            if (colSrc.c4ElemCount())
            {
                TCursor cursSrc(&colSrc);
                for (; cursSrc; ++cursSrc)
                    m_llstQueue.AppendNode(new TNode(cursSrc.objRCur()));
            }
        }

        TQueue(TMyType&&) = delete;

        ~TQueue() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& colSrc)
        {
            // Check for assignment to self
            if (this == &colSrc)
                return *this;

            TMtxLocker lockUs(this->pmtxLock());
            TMtxLocker lockSrc(colSrc.pmtxLock());

            // Call our parent first
            TParent::operator=(colSrc);

            // Flush our current content before copying
            RemoveAll();

            // Copy any source elements
            if (colSrc.c4ElemCount())
            {
                TCursor cursSrc(&colSrc);
                for (; cursSrc; ++cursSrc)
                    m_llstQueue.AppendNode(new TNode(*cursSrc));
            }

            // Invalidate any cursors
            this->c4IncSerialNum();

            //
            //  If there are any elements, then release all waiting threads
            //  and let them fight for the data. Don't care if there were
            //  actually any waiting. Also release any waiting for space
            //  in case some is now available as far as they are concerned.
            //
            if (m_llstQueue.c4ElemCount())
                m_twlWaiters.bReleaseAll(kCIDLib::c4TWLReason_WaitData);

            m_twlWaiters.bReleaseAll(kCIDLib::c4TWLReason_WaitSpace);

            return *this;
        }

        TMyType& operator=(TMyType&& colSrc) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const final
        {
            TMtxLocker lockQueue(this->pmtxLock());
            return m_llstQueue.bIsEmpty();
        }

        tCIDLib::TCard4 c4ElemCount() const final
        {
            TMtxLocker lockQueue(this->pmtxLock());
            return m_llstQueue.c4ElemCount();
        }

        TElem& objAdd(const TElem& objNew) final
        {
            TMtxLocker lockQueue(this->pmtxLock());
            TElem& objRet = objPut(objNew);
            this->c4IncSerialNum();
            return objRet;
        }

        [[nodiscard]] TCursor* pcursNew() const final
        {
            TMtxLocker lockQueue(this->pmtxLock());
            return new TCursor(this);
        }

        tCIDLib::TVoid RemoveAll() final
        {
            TMtxLocker lockQueue(this->pmtxLock());
            if (m_llstQueue.bIsEmpty())
                return;
            m_llstQueue.RemoveAll();
            this->c4IncSerialNum();

            // Wake up all threads waiting for space
            m_twlWaiters.bReleaseAll(kCIDLib::c4TWLReason_WaitSpace);
        }



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  If there is already an object in the queue that matches the passed
        //  one, update that object. Else, add a new one. The passed priority
        //  is not used if we update the existing one. The search goes from
        //  most recently added to least recently, so the first one that matches
        //  is updated.
        //
        template <typename TCompFunc> tCIDLib::TBoolean
        bAddOrUpdate(const  TElem&              objToPut
                    ,       TCompFunc           pfnComp
                    , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            TMtxLocker lockQueue(this->pmtxLock());

            //
            //  See if this element is already in the queue. If so, update
            //  it and return false to say we didn't add a new one. Tell it
            //  to search from the tail, so we update the most recently
            //  added one if there's more than one.
            //
            TNode* pnodeCur = pnodeFindObj(objToPut, pfnComp, kCIDLib::False);
            if (pnodeCur)
            {
                pnodeCur->objData() = objToPut;
                return kCIDLib::False;
            }

            // Not already there, so add the new node and return true
            objPut(objToPut, ePriority);
            return kCIDLib::True;
        }

        //
        //  If the newest element added matches this one, then just update
        //  that element. Else add it as a new element. The passed priority
        //  is not used if we update the existing one.
        //
        template <typename TCompFunc> tCIDLib::TBoolean
        bAddOrUpdateNewest( const   TElem&              objToPut
                            ,       TCompFunc           pnfComp
                            , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            TMtxLocker lockQueue(this->pmtxLock());

            //
            //  Get the tail node, if any. If there is one, and it matches
            //  the passed object. then update the passed object.
            //
            TNode* pnodeCur = static_cast<TNode*>(m_llstQueue.pnodeTail());
            if (pnodeCur
            &&  (pfnComp(pnodeCur->objData(), objToPut) == tCIDLib::ESortComps::Equal))
            {
                pnodeCur->objData() = objToPut;
                return kCIDLib::False;
            }

            // Didn't match, so let's add it as a new one
            objPut(objToPut, ePriority);
            return kCIDLib::True;
        }


        template <typename IterCB> tCIDLib::TBoolean bForEachNC(IterCB iterCB)
        {
            TMtxLocker lockThis(this->pmtxLock());
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


        //
        //  Get the next available object, waiting up to the indicated amount of time
        //  for it to show up. We have one that copies and one that moves out.
        //
        tCIDLib::TBoolean bGetNext(         TElem&              objToFill
                                    , const tCIDLib::TCard4     c4Millis = kCIDLib::c4MaxWait
                                    , const tCIDLib::TBoolean   bThrowIfTimeout = kCIDLib::False)
        {
            // Lock the queue
            TMtxLocker lockQueue(this->pmtxLock());

            // Call a helper to wait for something to show up
            tCIDLib::TBoolean bGotOne = TCollectionBase::bWaitForData
            (
                lockQueue, *this, c4Millis, m_twlWaiters, bThrowIfTimeout
            );

            if (bGotOne)
            {
                // Get the head node
                TNode* pnodeHead = static_cast<TNode*>(m_llstQueue.pnodeHead());

                // Get the object out of it and then flush the node
                objToFill = pnodeHead->objData();
                m_llstQueue.RemoveNode(pnodeHead);

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // Wake up one thread waiting for space
                m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
            }
            return bGotOne;
        }

        tCIDLib::TBoolean bGetNextMv(       TElem&              objToFill
                                    , const tCIDLib::TCard4     c4Millis = kCIDLib::c4MaxWait
                                    , const tCIDLib::TBoolean   bThrowIfTimeout = kCIDLib::False)
        {
            // Lock the queue
            TMtxLocker lockQueue(this->pmtxLock());

            // Call a helper to wait for something to show up
            tCIDLib::TBoolean bGotOne = TCollectionBase::bWaitForData
            (
                lockQueue, *this, c4Millis, m_twlWaiters, bThrowIfTimeout
            );

            if (bGotOne)
            {
                // Get the head node
                TNode* pnodeHead = static_cast<TNode*>(m_llstQueue.pnodeHead());

                // Get the object out of it and then flush the node
                objToFill = tCIDLib::ForceMove(pnodeHead->objData());
                m_llstQueue.RemoveNode(pnodeHead);

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // Wake up one thread waiting for space
                m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
            }
            return bGotOne;
        }


        //
        //  This method just looks at the next available element, and sees if
        //  it matches the passed value (using the passed comparator.) If
        //  so, then it gets the next element and returns it. Else it just
        //  returns false.
        //
        //  This ones doesn't wait, since if there's nothing in the queue to
        //  compare against, it doesn't have much meaning. So it returns
        //  false immediately if nothing is available.
        //
        template <typename TCompFunc> tCIDLib::TBoolean
        bGetNextIfMatches(  const   TElem&          objComp
                            ,       TElem&          objToFill
                            ,       TCompFunc       pfnComp)
        {
            // Lock the queue
            TMtxLocker lockQueue(this->pmtxLock());

            // Get the head node. If none, return false
            TNode* pnodeHead = static_cast<TNode*>(m_llstQueue.pnodeHead());
            if (!pnodeHead)
                return kCIDLib::False;

            // Compare it. If no match, return false
            if (pfnComp(pnodeHead->objData(), objComp) != tCIDLib::ESortComps::Equal)
                return kCIDLib::False;

            // Get the object out of it and then flush the node
            objToFill = pnodeHead->objData();
            m_llstQueue.RemoveNode(pnodeHead);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Wake up one thread waiting for space
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);

            return kCIDLib::True;
        }

        //
        //  Get high priority of all available objects, waiting up to the
        //  indicated amount of time for anything to show up to get.
        //
        //  NOTE that we want to get the highest priority one closest to the
        //  head of the list, i.e. the most recent one with the highest
        //  priority of any current items. So we start at the head and only
        //  take a new one if it's higher than that last.
        //
        //  Of cours this means an exhaustive search since we don't sort them
        //  by priority (because a queue can be accessed both FIFO and priority
        //  modes intermixed.
        //
        tCIDLib::TBoolean
        bGetHighestPrio(        TElem&              objToFill
                        , const tCIDLib::TCard4     c4Millis = kCIDLib::c4MaxWait
                        , const tCIDLib::TBoolean   bThrowIfTimeout = kCIDLib::False)
        {
            // Lock the queue
            TMtxLocker lockQueue(this->pmtxLock());

            // Call a helper to wait for something to show up
            tCIDLib::TBoolean bGotOne = TCollectionBase::bWaitForData
            (
                lockQueue, *this, c4Millis, m_twlWaiters, bThrowIfTimeout
            );

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

                // If 0, then none, so log an error since we know there should be
                if (!pnodeHighest)
                    this->ColIsEmpty(CID_FILE, CID_LINE);

                // Get a copy of the node data object
                objToFill = pnodeHighest->objData();

                // Delete the node that we found.
                m_llstQueue.RemoveNode(pnodeHighest);

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // Wake up one thread waiting for space
                m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitSpace);
            }
            return bGotOne;
        }

        //
        //  Add the passed object if another matching object isn't already in
        //  the queue.
        //
        template <typename TCompFunc> tCIDLib::TBoolean
        bPutIfNew(  const   TElem&              objToPut
                    ,       TCompFunc           pfnComp
                    , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            TMtxLocker lockQueue(this->pmtxLock());

            // See if this element is already in the queue. If so, return false
            if (pnodeFindObj(objToPut, pfnComp, kCIDLib::True))
                return kCIDLib::False;

            // Add the new node
            objPut(objToPut, ePriority);

            // We added it so return true
            return kCIDLib::True;
        }


        //
        //  If the current element count is at or beyond the indicated value,
        //  waits for up to the indicated time for space to become available,
        //  though of course the caller must still check since another thread
        //  could take that space before they can react.
        //
        tCIDLib::TBoolean
        bWaitForSpace(const tCIDLib::TCard4 c4Millis, const tCIDLib::TCard4 c4Limit)
        {
            TMtxLocker lockQueue(this->pmtxLock());

            if (m_llstQueue.c4ElemCount() < c4Limit)
                return kCIDLib::True;

            //
            //  It's 'full', so put us on the wait list with the wait space
            //  code to wait for some space to become available.
            //
            return m_twlWaiters.bWaitOnList
            (
                lockQueue, kCIDLib::c4TWLReason_WaitSpace, c4Millis
            );
        }


        // Construct an element in place
        template <typename... TArgs> TElem& objPlace(TArgs&&... Args)
        {
            // Get control of the queue before we modify it
            TMtxLocker lockQueue(this->pmtxLock());

            // Add the new node to the list
            TNode* pnodeNew = new TNode
            (
                tCIDLib::EQPrios::P0, tCIDLib::Forward<TArgs>(Args)...
            );
            m_llstQueue.AppendNode(pnodeNew);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  And wake up one waiting thread to handle this new element.
            //  Don't care if any are actually waiting.
            //
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitData);
            return pnodeNew->objData();
        }

        TElem&
        objPut( const   TElem&              objToPut
                , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            // Get control of the queue before we modify it
            TMtxLocker lockQueue(this->pmtxLock());

            // Add the new node to the list
            TNode* pnodeNew = new TNode(objToPut, ePriority);
            m_llstQueue.AppendNode(pnodeNew);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  And wake up one waiting thread to handle this new element.
            //  Don't care if any are actually waiting.
            //
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitData);
            return pnodeNew->objData();
        }

        TElem&
        objPut(         TElem&&             objToPut
                , const tCIDLib::EQPrios    ePriority = tCIDLib::EQPrios::P0)
        {
            // Get control of the queue before we modify it
            TMtxLocker lockQueue(this->pmtxLock());

            // Add the new node to the list
            TNode* pnodeNew = new TNode(tCIDLib::ForceMove(objToPut), ePriority);
            m_llstQueue.AppendNode(pnodeNew);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  And wake up one waiting thread to handle this new element.
            //  Don't care if any are actually waiting.
            //
            m_twlWaiters.bReleaseOne(kCIDLib::c4TWLReason_WaitData);
            return pnodeNew->objData();
        }


        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TMtxLocker lockQueue(this->pmtxLock());

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            // Get the node to remove and move the cursor forward
            TNode* pnodeToRemove = cursAt.pnodeCur();
            cursAt.bNext();

            // And flush the node
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
        template <typename TCompFunc>
        TNode* pnodeFindObj(const   TElem&              objToFind
                            ,       TCompFunc           pfnComp
                            , const tCIDLib::TBoolean   bFromFront)
        {
            if (bFromFront)
            {
                TNode* pnodeCur = (TNode*)m_llstQueue.pnodeHead();
                while (pnodeCur)
                {
                    if (pfnComp(pnodeCur->objData(), objToFind) == tCIDLib::ESortComps::Equal)
                        return pnodeCur;
                    pnodeCur = static_cast<TNode*>(pnodeCur->pnodeNext());
                }
            }
             else
            {
                TNode* pnodeCur = (TNode*)m_llstQueue.pnodeTail();
                while (pnodeCur)
                {
                    if (pfnComp(pnodeCur->objData(), objToFind) == tCIDLib::ESortComps::Equal)
                        return pnodeCur;
                    pnodeCur = static_cast<TNode*>(pnodeCur->pnodePrev());
                }
            }
            return 0;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_llstQueue
        //      The linked list that provides the storage for this collection.
        //
        //  m_twlWaiters
        //      This is the thread wait list that is used by threads to wait
        //      on input to arrive in the queue.
        // -------------------------------------------------------------------
        TDLinkedList        m_llstQueue;
        TThreadWaitList     m_twlWaiters;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TMyType)
        TemplateRTTIDefs(TMyType,TCollection<TElem>)
};

#pragma CIDLIB_POPPACK



