//
// FILE NAME: CIDLib_BasicDLinkedCol.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/29/1998
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
//  This header implements the TBasicDLinkedCol class. A number of the
//  collection classes are based on the doubly linked list class, and therefore
//  have a lot of implementation that would be exactly the same. So this class
//  exists to hold that common code, and they derive from it.
//
//  Also provided here the const and non-const cursor classes, which are the
//  standard cursor class for this type of collection. They can serve as the
//  cursors for all of the collections that are based on TBasicDLinkedCol.
//  Since they are always type def'ed inside the derived class as a nested
//  class named TConstCursor and TNonContsCursor, the actual type of the
//  cursor does not really matter.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TBasicDLinkedCol
//  PREFIX: col
// ---------------------------------------------------------------------------
template <class TElem> class TBasicDLinkedCol : public TCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested aliases for the cursors and types used.
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = TBasicDLinkedCol<TElem>;
        using TNode = TBasicColNode<TElem>;



        // -------------------------------------------------------------------
        //  Our nested read-only cursor class
        // -------------------------------------------------------------------
        template <class TElem> class TConstCursor : public TBiColCursor<TElem>
        {
            public  :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TConstCursor() :

                    m_pllstCursoring(nullptr)
                    , m_pcolCursoring(nullptr)
                    , m_pnodeCur(nullptr)
                {
                }

                explicit TConstCursor(const TMyType* const pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_pllstCursoring(&pcolToCursor->llstCol())
                    , m_pcolCursoring(pcolToCursor)
                    , m_pnodeCur(nullptr)
                {
                    m_pnodeCur = static_cast<TBasicColNode<TElem>*>
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
                        // Lock the src if it's been set and is thread safe
                        TMtxLocker lockCol
                        (
                            cursSrc.m_pcolCursoring
                            ? cursSrc.m_pcolCursoring->pmtxLock() : nullptr
                        );

                        TParent::operator=(cursSrc);
                        m_pllstCursoring = cursSrc.m_pllstCursoring;
                        m_pcolCursoring = cursSrc.m_pcolCursoring;
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
                tCIDLib::TBoolean bIsValid() const
                {
                    if (!TParent::bIsValid())
                        return kCIDLib::False;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bNext()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TNode*>(m_pnodeCur->pnodeNext());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bPrevious()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TNode*>(m_pnodeCur->pnodePrev());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bSeekToEnd()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    m_pnodeCur = static_cast<TNode*>(m_pllstCursoring->pnodeTail());
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);
                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    m_pnodeCur = static_cast<TNode*>(m_pllstCursoring->pnodeHead());
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    return (m_pnodeCur != nullptr);
                }

                const TElem& objRCur() const
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
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }

                TNode* pnodeCur()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }


            protected :
                // -----------------------------------------------------------
                //  Declare our friends
                // -----------------------------------------------------------
                friend class TMyType;


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolCursoring
                //      The basic doubly linked collection we are cursoring.
                //
                //  m_pllstCursoring
                //      This is the linked list we are cursoring. It is gotten
                //      out of the collection during construction for faster
                //      access.
                //
                //  m_pnodeCur
                //      This is the current node that we are on in our
                //      iteration. It is null if we are at the end.
                // -----------------------------------------------------------
                const TMyType*      m_pcolCursoring;
                const TDLinkedList* m_pllstCursoring;
                TNode*              m_pnodeCur;


                // -----------------------------------------------------------
                //  Do any needed magic macros
                // -----------------------------------------------------------
                TemplateRTTIDefs
                (
                    TBasicDLinkedCol::TConstCursor<TElem>
                    , TBiColCursor<TElem>
                )
        };


        // -------------------------------------------------------------------
        //  Our nested non-const cursor class
        // -------------------------------------------------------------------
        template <class TElem> class TNonConstCursor : public TConstCursor<TElem>
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

                    // Lock the collection and check the serial number
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
                //      The basic doubly linked collection we are cursoring.
                // -----------------------------------------------------------
                TMyType*    m_pcolNCCursoring;


                // -----------------------------------------------------------
                //  Do any needed magic macros
                // -----------------------------------------------------------
                TemplateRTTIDefs
                (
                    TBasicDLinkedCol::TNonConstCursor<TElem>
                    , TBasicDLinkedCol::TConstCursor<TElem>
                )
        };


        // -------------------------------------------------------------------
        //  More aliases for the nested cursor classes
        // -------------------------------------------------------------------
        using TCursor = typename TConstCursor<TElem>;
        using TNCCursor = typename TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBasicDLinkedCol(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollection<TElem>(eMTSafe)
            , m_llstCol()
        {
        }

        TBasicDLinkedCol(const TBasicDLinkedCol<TElem>& colSrc) :

            TCollection<TElem>(colSrc)
            , m_llstCol()
        {
            // Lock the source collection so it won't change during this
            TMtxLocker lockSrc(colSrc.pmtxLock());

            // And copy any source elements
            if (colSrc.c4ElemCount())
            {
                TCursor cursSrc(&colSrc);
                while (cursSrc.bIsValid())
                {
                    m_llstCol.AppendNode(new TNode(cursSrc.objRCur()));
                    ++cursSrc;
                }
            }
        }

        TBasicDLinkedCol(TBasicDLinkedCol<TElem>&& colSrc) :

            TBasicDLinkedCol()
        {
            *this = tCIDLib::ForceMove(colSrc);
        }

        ~TBasicDLinkedCol()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBasicDLinkedCol<TElem>& operator=(const TBasicDLinkedCol<TElem>& colSrc)
        {
            // Check for assignment to self
            if (this != &colSrc)
            {
                // Lock the source while we do this. We CANNOT lock ourself!
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
                        m_llstCol.AppendNode(new TNode(cursSrc.objRCur()));
                }

                // Invalidate any cursors
                this->c4IncSerialNum();
            }
            return *this;
        }

        TBasicDLinkedCol<TElem>& operator=(TBasicDLinkedCol<TElem>&& colSrc)
        {
            if (&colSrc != this)
            {
                TParent::operator=(tCIDLib::ForceMove(colSrc));
                m_llstCol = tCIDLib::ForceMove(colSrc.m_llstCol);
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const
        {
            TMtxLocker lockSync(this->pmtxLock());
            tCIDLib::TBoolean bRet = m_llstCol.bIsEmpty();
            return bRet;
        }

        tCIDLib::TCard4 c4ElemCount() const
        {
            TMtxLocker lockSync(this->pmtxLock());
            tCIDLib::TCard4 c4Ret = m_llstCol.c4ElemCount();
            return c4Ret;
        }

        [[nodiscard]] TCursor* pcursNew() const
        {
            TMtxLocker lockSync(this->pmtxLock());
            return new TCursor(this);
        }

        tCIDLib::TVoid RemoveAll()
        {
            TMtxLocker lockSync(this->pmtxLock());
            if (m_llstCol.bIsEmpty())
                return;

            m_llstCol.RemoveAll();
            this->c4IncSerialNum();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGetFromBottom(TElem& objToFill)
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, return false
            if (m_llstCol.bIsEmpty())
                return kCIDLib::False;

            // Get a pointer to the first node
            TNode* pnodeHead = static_cast<TNode*>(m_llstCol.pnodeHead());

            // Get a copy of the stored object, then delete the node
            objToFill = pnodeHead->objData();
            m_llstCol.RemoveNode(pnodeHead);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return kCIDLib::True;
        }

        tCIDLib::TBoolean bGetFromTop(TElem& objToFill)
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, return false
            if (m_llstCol.bIsEmpty())
                return kCIDLib::False;

            // Get a pointer to the last node
            TNode* pnodeLast = static_cast<TNode*>(m_llstCol.pnodeTail());

            // Get a copy of the stored object and delete the node
            objToFill = pnodeLast->objData();
            m_llstCol.RemoveNode(pnodeLast);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return kCIDLib::True;
        }


        tCIDLib::TVoid DiscardBottom()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the first node and delete it
            TNode* pnodeHead = static_cast<TNode*>(m_llstCol.pnodeHead());
            m_llstCol.RemoveNode(pnodeHead);
            this->c4IncSerialNum();
        }

        tCIDLib::TVoid DiscardTop()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the last node and delete it
            TNode* pnodeTail = static_cast<TNode*>(m_llstCol.pnodeTail());
            m_llstCol.RemoveNode(pnodeTail);
            this->c4IncSerialNum();
        }

        tCIDLib::TVoid ExchangeBottom()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // Get the head and and the next
            TNode* pnodeHead = static_cast<TNode*>(m_llstCol.pnodeHead());
            if (pnodeHead)
            {
                TNode* pnodeNext = static_cast<TNode*>(pnodeHead->pnodeNext());
                if (pnodeNext)
                {
                    m_llstCol.ExchangeNodes(pnodeHead, pnodeNext);
                    this->c4IncSerialNum();
                }
            }
        }

        tCIDLib::TVoid ExchangeTop()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // Get the tail and and the previous
            TNode* pnodeTail = static_cast<TNode*>(m_llstCol.pnodeTail());
            if (pnodeTail)
            {
                TNode* pnodePrev = static_cast<TNode*>(pnodeTail->pnodePrev());
                if (pnodePrev)
                {
                    m_llstCol.ExchangeNodes(pnodeTail, pnodePrev);
                    this->c4IncSerialNum();
                }
            }
        }

        template <typename IterCB> tCIDLib::TVoid ForEachNC(IterCB iterCB) const
        {
            TMtxLocker lockThis(this->pmtxLock());
            TNode* pnodeHead = static_cast<TNode*>(m_llstCol.pnodeHead());
            while (pnodeHead)
            {
                if (!iterCB(pnodeHead->objData()))
                    break;
                pnodeHead = static_cast<TNode*>(pnodeHead->pnodeNext());
            }
        }

        TElem& objAddAtBottom(const TElem& objToAdd)
        {
            TMtxLocker lockSync(TParent::pmtxLock());
            TNode* pnodeNew = new TNode(objToAdd);
            m_llstCol.PrependNode(pnodeNew);
            this->c4IncSerialNum();
            return pnodeNew->objData();
        }

        TElem& objAddAtTop(const TElem& objToAdd)
        {
            TMtxLocker lockSync(TParent::pmtxLock());
            TNode* pnodeNew = new TNode(objToAdd);
            m_llstCol.AppendNode(pnodeNew);
            this->c4IncSerialNum();
            return pnodeNew->objData();
        }

        const TElem& objPeekAtBottom() const
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the first node and return a ref to the data
            TNode* pnodeFirst = static_cast<TNode*>(m_llstCol.pnodeHead());
            return pnodeFirst->objData();
        }

        TElem& objPeekAtBottom()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the first node and return a ref to the data
            TNode* pnodeFirst = static_cast<TNode*>(m_llstCol.pnodeHead());
            return pnodeFirst->objData();
        }

        const TElem& objPeekAtTop() const
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the last node and return a ref to the data
            TNode* pnodeLast = static_cast<TNode*>(m_llstCol.pnodeTail());
            return pnodeLast->objData();
        }

        TElem& objPeekAtTop()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the last node amd return a ref to the data
            TNode* pnodeLast = static_cast<TNode*>(m_llstCol.pnodeTail());
            return pnodeLast->objData();
        }

        TElem objGetFromBottom()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the first node
            TNode* pnodeHead = static_cast<TNode*>(m_llstCol.pnodeHead());

            // Get a copy of the stored object
            TElem objRet = pnodeHead->objData();

            // Delete the node.
            m_llstCol.RemoveNode(pnodeHead);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return objRet;
        }

        TElem objGetFromTop()
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the last node
            TNode* pnodeLast = static_cast<TNode*>(m_llstCol.pnodeTail());

            // Get a copy of the stored object
            TElem objRet = pnodeLast->objData();

            // Delete the node.
            m_llstCol.RemoveNode(pnodeLast);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return objRet;
        }

        TElem& objInsertAfter(const TElem& objToInsert, TCursor& cursPos)
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // Create a new node to add
            TNode* pnodeNew = new TNode(objToInsert);

            if (!cursPos.bIsValid())
            {
                m_llstCol.AppendNode(pnodeNew);
            }
             else
            {
                //
                //  Move back now so that we can insert after this node. If
                //  going back invalidates, then we prepend at the head.
                //  Else, we insert after the node.
                //
                if (!cursPos.bPrevious())
                    m_llstCol.PrependNode(pnodeNew);
                 else
                    m_llstCol.InsertNode(cursPos.pnodeCur(), pnodeNew);
            }
            return pnodeNew->objData();
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            // Get the node to flush
            TNode* pnodeToRemove = cursAt.pnodeCur();

            // Move the cursor to the next element, past the target
            cursAt.bNext();

            // Now flush the node
            m_llstCol.RemoveNode(pnodeToRemove);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Get the cursor back into sync.
            cursAt.c4SerialNum(this->c4SerialNum());
        }


        tCIDLib::TVoid Set(const tCIDLib::EMTStates  eToSet)
        {
            TMtxLocker lockSync(TParent::pmtxLock());

            // Flush it all out
            RemoveAll();

            // Call our parent to set the new thread state
            this->eMTState(eToSet);
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
        const TDLinkedList& llstCol() const
        {
            return m_llstCol;
        }

        TDLinkedList& llstCol()
        {
            return m_llstCol;
        }


    private     :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_llstCol
        //      This is a doubly linked list that is the storage for the
        //      elements. All the derivatives of this class allow bidirectional
        //      iteration, so we used a doubly linked list.
        // -------------------------------------------------------------------
        TDLinkedList    m_llstCol;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TBasicDLinkedCol<TElem>,TCollection<TElem>)
};

#pragma CIDLIB_POPPACK


//
//  Unlike the usual way that classes support binary streaming, i.e. through
//  the MStreamable mixin interface, we cannot do that here because that would
//  require that all our element types are streamable. So we provide a global
//  operator for those folks who want to use it. This means that collections
//  cannot be streamed polymorphically via the base classes.
//
template <typename TElem>
TBinOutStream& operator<<(          TBinOutStream&              strmOut
                            , const TBasicDLinkedCol<TElem>&    colToStream)
{
    // Don't let it change during this
    TMtxLocker lockThis(colToStream.pmtxLock());

    //
    //  Stream out a leading stream marker, then the element count, and the
    //  thread safety setting.
    //
    tCIDLib::TCard4 c4Count     = colToStream.c4ElemCount();

    //
    //  We got rid of the max size, so just stream a zero now, which we might/
    //  reuse in the future.
    //
    strmOut <<  tCIDLib::EStreamMarkers::StartObject
            <<  c4Count
            <<  tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard)
            <<  tCIDLib::TCard4(0)
            <<  colToStream.eMTState();

    // If there were any elements, then stream them
    if (c4Count)
    {
        typename TBasicDLinkedCol<TElem>::TCursor cursStream(&colToStream);
        while (cursStream.bIsValid())
        {
            strmOut << cursStream.objRCur();
            ++cursStream;
        }
    }

    strmOut << tCIDLib::EStreamMarkers::EndObject;
    return strmOut;
}


// We cannot lock the collection, since we might delete the mutex
template <typename TElem>
TBinInStream& operator>>(TBinInStream&              strmIn
                        , TBasicDLinkedCol<TElem>&  colToStream)
{
    // Flush the collection first
    colToStream.RemoveAll();

    // Make sure we see the stream marker
    strmIn.CheckForMarker(tCIDLib::EStreamMarkers::StartObject, CID_FILE, CID_LINE);

    //
    //  Stream in the state of the collection itself. WE got rid of the max count
    //  so just stream it into a dummy value.
    //
    tCIDLib::TCard4     c4OldMax;
    tCIDLib::TCard4     c4Count;
    tCIDLib::TCard4     c4XORCount;
    tCIDLib::EMTStates  eMTState;
    strmIn >> c4Count >> c4XORCount >> c4OldMax >> eMTState;

    if (c4XORCount != tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard))
        TCollectionBase::BadStoredCount(colToStream.clsIsA());

    // Set the thread safety on this guy
    colToStream.Set(eMTState);

    // If there were any elements, then stream them in
    if (c4Count)
    {
        TElem objTmp;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strmIn >> objTmp;
            colToStream.objAdd(objTmp);
        }
    }

    strmIn.CheckForMarker(tCIDLib::EStreamMarkers::EndObject, CID_FILE, CID_LINE);
    return strmIn;
}


