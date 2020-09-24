//
// FILE NAME: CIDLib_BasicDLinkedRefCol.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/14/1999
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
//  This header implements the TBasicDLinkedRefCol class. A number of the
//  by reference collection classes are based on the doubly linked list class,
//  and therefore have a lot of implementation that would be exactly the same.
//  So this class exists to hold that common code, and they derive from it.
//
//  Also provided here are the const and non-const cursor classes, which are
//  the standard cursor classes for this type of collection. They can serve as
//  the cursor for all of the collections that are based on TBasicDLinkedRefCol.
//  Since they are always type def'ed inside the derived class as a nested
//  class named TCursor and TWCursor, the actual type of the cursor does not
//  really matter.
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
//   CLASS: TBasicDLinkedRefCol
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem> class TBasicDLinkedRefCol : public TRefCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested aliases for the cursor and types used
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = TBasicDLinkedRefCol<TElem>;
        using TParType = TRefCollection<TElem>;
        using TNode = TBasicColRefNode<TElem>;


        // -------------------------------------------------------------------
        //  Our nested cursors
        // -------------------------------------------------------------------
        template <typename TElem2> class TConstCursor : public TBiColCursor<TElem2>
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
                    m_pnodeCur = static_cast<TBasicColRefNode<TElem2>*>
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
                tCIDLib::TBoolean bIsValid() const override
                {
                    if (!TParent::bIsValid())
                        return kCIDLib::False;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bNext() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TBasicColRefNode<TElem2>*>(m_pnodeCur->pnodeNext());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bPrevious() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = static_cast<TBasicColRefNode<TElem2>*>(m_pnodeCur->pnodePrev());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bSeekToEnd() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection
                    TLocker lockrCol(m_pcolCursoring);
                    m_pnodeCur = static_cast<TBasicColRefNode<TElem2>*>
                    (
                        m_pllstCursoring->pnodeTail()
                    );

                    // Get ourself back in sync with the linked list
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection
                    TLocker lockrCol(m_pcolCursoring);
                    m_pnodeCur = static_cast<TBasicColRefNode<TElem2>*>
                    (
                        m_pllstCursoring->pnodeHead()
                    );

                    // Get our serial back in sync with the collection
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    return (m_pnodeCur != nullptr);
                }

                const TElem2& objRCur() const override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(m_pnodeCur, CID_FILE, CID_LINE);
                    return *m_pnodeCur->pobjData();
                }


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                const TNode* pnodeCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }

                TNode* pnodeCur()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolCursoring);
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
                //      This is the linked list we are cursoring. It is gotten out of
                //      the collection during construction for faster access.
                //
                //  m_pnodeCur
                //      This is the current node that we are on in our iteration. It
                //      is 0 if we are at the end.
                // -----------------------------------------------------------
                const TMyType*      m_pcolCursoring;
                const TDLinkedList* m_pllstCursoring;
                TNode*              m_pnodeCur;


                // -----------------------------------------------------------
                //  Do any needed magic macros
                // -----------------------------------------------------------
                TemplateRTTIDefs
                (
                    TBasicDLinkedRefCol::TConstCursor<TElem2>
                    , TBiColCursor<TElem2>
                )
        };


        template <typename TElem2> class TNonConstCursor : public TConstCursor<TElem2>
        {
            public  :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TNonConstCursor() :

                    m_pcolNCCursoring(nullptr)
                {
                }

                explicit  TNonConstCursor(TMyType* const pcolToCursor) :

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

                TElem2& operator*() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return *const_cast<TElem2*>(this->pnodeCur()->pobjData());
                }

                TElem2* operator->() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return const_cast<TElem2*>(this->pnodeCur()->pobjData());
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
                TElem2& objWCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return *const_cast<TElem2*>(this->pnodeCur()->pobjData());
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
                    TBasicDLinkedRefCol::TNonConstCursor<TElem2>
                    , TBasicDLinkedRefCol::TConstCursor<TElem2>
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
        TBasicDLinkedRefCol(const   tCIDLib::EAdoptOpts eAdopt
                            , const tCIDLib::EMTStates  eMTSafe) :

            TRefCollection<TElem>(eAdopt, eMTSafe)
            , m_llstCol()
        {
        }

        TBasicDLinkedRefCol(const TMyType&) = delete;

        TBasicDLinkedRefCol(TMyType&& colSrc) :

            TRefCollection<TElem>(colSrc.eAdopt(), colSrc.eMTSafe())
            , m_llstCol()
        {
            *this = tCIDLib::ForceMove(colSrc);
        }

        ~TBasicDLinkedRefCol()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType&) = delete;

        TMyType& operator=(TMyType&& colSrc)
        {
            // We must have the same adoption type
            if (colSrc.eAdopt() != this->eAdopt())
                this->MovedAdopted(CID_FILE, CID_LINE);

            if (&colSrc != this)
            {
                TLocker lockrSrc(&colSrc);
                TLocker lockrThis(this);

                m_llstCol = tCIDLib::ForceMove(colSrc.m_llstCol);

                // Publish reloaded events for both
                this->PublishReloaded();
                colSrc.PublishReloaded();
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const override
        {
            TLocker lockrThis(this);
            tCIDLib::TBoolean bRet = m_llstCol.bIsEmpty();
            return bRet;
        }

        tCIDLib::TCard4 c4ElemCount() const override
        {
            TLocker lockrThis(this);
            tCIDLib::TCard4 c4Ret = m_llstCol.c4ElemCount();
            return c4Ret;
        }

        tCIDLib::TVoid OrphanElem(TElem* const pobjToRemove) override
        {
            TLocker lockrThis(this);

            // Find the node with this object
            TNode* pnodeToRemove = static_cast<TNode*>(m_llstCol.pnodeHead());
            while (pnodeToRemove)
            {
                if (pnodeToRemove->pobjData() == pobjToRemove)
                    break;
                pnodeToRemove = static_cast<TNode*>(pnodeToRemove->pnodeNext());
            }
            if (!pnodeToRemove)
                this->NotMemberNode(CID_FILE, CID_LINE);

            // Now orphan the data out and then flush the node.
            pnodeToRemove->OrphanData();
            m_llstCol.RemoveNode(pnodeToRemove);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();
        }

        [[nodiscard]] TConstCursor<TElem>* pcursNew() const override
        {
            TLocker lockrThis(this);
            return new TConstCursor<TElem>(this);
        }

        tCIDLib::TVoid RemoveAll() override
        {
            TLocker lockrThis(this);
            if (m_llstCol.bIsEmpty())
                return;

            m_llstCol.RemoveAll();
            this->c4IncSerialNum();
        }

        tCIDLib::TVoid RemoveElem(TElem* const pobjToRemove) override
        {
            TLocker lockrThis(this);

            // Find the node with this object
            TNode* pnodeToRemove = static_cast<TNode*>(m_llstCol.pnodeHead());
            while (pnodeToRemove)
            {
                if (pnodeToRemove->pobjData() == pobjToRemove)
                    break;
                pnodeToRemove = static_cast<TNode*>(pnodeToRemove->pnodeNext());
            }
            if (!pnodeToRemove)
                this->NotMemberNode(CID_FILE, CID_LINE);

            // Now flush the node
            m_llstCol.RemoveNode(pnodeToRemove);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddAtBottom(TElem* const pobjToAdd)
        {
            if (!pobjToAdd)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            TLocker lockrThis(this);
            m_llstCol.PrependNode(new TNode(pobjToAdd, this->eAdopt()));
            this->c4IncSerialNum();
        }

        tCIDLib::TVoid AddAtTop(TElem* const pobjToAdd)
        {
            if (!pobjToAdd)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            TLocker lockrThis(this);
            m_llstCol.AppendNode(new TNode(pobjToAdd, this->eAdopt()));
            this->c4IncSerialNum();
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachNC(IterCB iterCB)
        {
            TLocker lockrThis(this);
            TNode* pnodeHead = static_cast<TNode*>(m_llstCol.pnodeHead());
            while (pnodeHead)
            {
                if (!iterCB(pnodeHead->objData()))
                    return kCIDLib::False;
                pnodeHead = static_cast<TNode*>(pnodeHead->pnodeNext());
            }
            return kCIDLib::True;
        }


        // Get the bottom or top. If there is one, then drop it or delete it
        tCIDLib::TVoid DiscardBottom()
        {
            TElem* pobjBottom = pobjGetFromBottom(kCIDLib::False);
            if (pobjBottom && (this->eAdopt() == tCIDLib::EAdoptOpts::Adopt))
                delete pobjBottom;
        }

        tCIDLib::TVoid DiscardTop()
        {
            TElem* pobjTop = pobjGetFromTop(kCIDLib::False);
            if (pobjTop && (this->eAdopt() == tCIDLib::EAdoptOpts::Adopt))
                delete pobjTop;
        }


        tCIDLib::TVoid ExchangeBottom()
        {
            TLocker lockrSync(this);

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
            TLocker lockrSync(this);

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


        tCIDLib::TVoid InsertAfter( TElem* const pobjToAdopt, TCursor& cursAfter)
        {
            if (!pobjToAdopt)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            TLocker lockrThis(this);

            TNode* pnodeNew = new TNode(pobjToAdopt, this->eAdopt());
            if (!cursAfter.bIsValid())
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
                if (!cursAfter.bPrevious())
                    m_llstCol.PrependNode(pnodeNew);
                 else
                    m_llstCol.InsertNode(cursAfter.pnodeCur(), pnodeNew);
            }
        }

        [[nodiscard]] TElem*
        pobjGetFromBottom(const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True)
        {
            TLocker lockrThis(this);

            //
            //  See if there are any nodes. If not, throw an exception or
            //  return null, according to the parm.
            //
            if (m_llstCol.bIsEmpty())
            {
                if (bThrowIfNot)
                    this->ColIsEmpty(CID_FILE, CID_LINE);
                return nullptr;
            }

            // Get a pointer to the first node
            TNode* pnodeHead = static_cast<TNode*>(m_llstCol.pnodeHead());

            // Get a copy of the stored object
            TElem* pobjRet = pnodeHead->pobjData();

            //
            //  Now orphan it so that the flush won't delete the data object even
            //  if we've adopted it, then flush the node
            //
            pnodeHead->OrphanData();
            m_llstCol.RemoveNode(pnodeHead);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return pobjRet;
        }

        [[nodiscard]] TElem*
        pobjGetFromTop(const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True)
        {
            TLocker lockrThis(this);

            //
            //  See if there are any nodes. If not, throw an exception or
            //  return zero, according to the parm.
            //
            if (m_llstCol.bIsEmpty())
            {
                if (bThrowIfNot)
                    this->ColIsEmpty(CID_FILE, CID_LINE);
                return nullptr;
            }

            // Get a pointer to the last node
            TNode* pnodeLast = static_cast<TNode*>(m_llstCol.pnodeTail());

            // Get a copy of the stored object
            TElem* pobjRet = pnodeLast->pobjData();

            //
            //  Now orphan it so that the flush won't delete it even if we've
            //  adopted it, the flush the node
            //
            pnodeLast->OrphanData();
            m_llstCol.RemoveNode(pnodeLast);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return pobjRet;
        }

        const TElem* pobjPeekAtBottom() const
        {
            TLocker lockrThis(this);

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the first node and return a ref to the data
            TNode* pnodeFirst = static_cast<TNode*>(m_llstCol.pnodeHead());
            return pnodeFirst->pobjData();
        }

        TElem* pobjPeekAtBottom()
        {
            TLocker lockrThis(this);

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the first node and return a ref to the data
            TNode* pnodeFirst = static_cast<TNode*>(m_llstCol.pnodeHead());
            return pnodeFirst->pobjData();
        }

        const TElem* pobjPeekAtTop() const
        {
            TLocker lockrThis(this);

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the last node and return a ref to the data
            TNode* pnodeLast = static_cast<TNode*>(m_llstCol.pnodeTail());
            return pnodeLast->pobjData();
        }

        TElem* pobjPeekAtTop()
        {
            TLocker lockrThis(this);

            // See if there are any nodes. If not, throw an exception
            if (m_llstCol.bIsEmpty())
                this->ColIsEmpty(CID_FILE, CID_LINE);

            // Get a pointer to the last node and return a ref to the data
            TNode* pnodeLast = static_cast<TNode*>(m_llstCol.pnodeTail());
            return pnodeLast->pobjData();
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TLocker lockrThis(this);

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            // Get the node to flush and then move the cursor past it
            TNode* pnodeToRemove = cursAt.pnodeCur();
            cursAt.bNext();

            // Now flush the target node
            m_llstCol.RemoveNode(pnodeToRemove);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Get our cursor back into sync.
            cursAt.c4SerialNum(this->c4SerialNum());
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TConstCursor<TElem>;
        friend class TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TBasicDLinkedRefCol() :

            TParent(tCIDLib::EAdoptOpts::NoAdopt)
        {
        }


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


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_llstCol
        //      This is a doubly linked list that is the storage for the elements. All
        //      the derivatives of this class allow bidirectional iteration, so we used
        //      a doubly linked list.
        // -------------------------------------------------------------------
        TDLinkedList        m_llstCol;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TBasicDLinkedRefCol<TElem>,TRefCollection<TElem>)
};


#pragma CIDLIB_POPPACK

