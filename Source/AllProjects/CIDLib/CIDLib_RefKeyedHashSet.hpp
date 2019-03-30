//
// FILE NAME: CIDLib_RefKeyedHashSet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2002
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
//  This is the public header for the TRefKeyedHashSet class. This class
//  provides an unordered collection of keyed elements by pointer. They are
//  'keyed' because only some subset of their contents is considered their
//  value for purposes of lookup, as apposed to the whole value of the
//  elements. Duplicate keys are not allowed, since it is a set.
//
//  What comprises the key is very flexible because it takes a key extraction
//  method that handles getting the key out of an element. Hashing of the key
//  and comparison of keys is handled by a key ops object that is provided
//  the client code.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that these classes cannot use the various magic macros because of
//      the multiple template instantiation parameters confuse the macro
//      processor and make it think we are passing too many parameters. So it
//      implements all that stuff manually. Oh well...
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward reference some internal structures and classes
// ---------------------------------------------------------------------------
template <class TElem,class TKey,class TKeyOps> class TRefKeyedHashSet;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TRefKeyedHashSetNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <class TElem,class TKey> class TRefKeyedHashSetNode
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TMyType = TRefKeyedHashSetNode<TElem, TKey>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefKeyedHashSetNode() = delete;

        TRefKeyedHashSetNode(TElem* const pobjData, TMyType* pnodeNext) :

            m_pobjData(pobjData)
            , m_pnodeNext(pnodeNext)
        {
        }

        TRefKeyedHashSetNode(const TMyType&) = delete;
        TRefKeyedHashSetNode( TMyType&&) = delete;

        ~TRefKeyedHashSetNode() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType&) = delete;
        TMyType& operator=( TMyType&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DeleteData()
        {
            if (m_pobjData)
            {
                delete m_pobjData;
                m_pobjData = nullptr;
            }
        }

        const TElem& objData() const
        {
            return *m_pobjData;
        }

        TElem& objData()
        {
            return *m_pobjData;
        }

        TMyType* pnodeNext()
        {
            return m_pnodeNext;
        }

        TMyType* pnodeNext(TMyType* const pnodeToSet)
        {
            m_pnodeNext = pnodeToSet;
            return m_pnodeNext;
        }

        const TElem* pobjData() const
        {
            return m_pobjData;
        }

        TElem* pobjData()
        {
            return m_pobjData;
        }

        TElem* pobjOrphan()
        {
            TElem* pobjRet = m_pobjData;
            m_pobjData = nullptr;
            return pobjRet;
        }

        tCIDLib::TVoid Set(TElem* const pobjToSet)
        {
            m_pobjData = pobjToSet;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pobjData
        //      This is the data for this node. Its a pointer to the data. The
        //      collection is responsible for deleting it when/if needed.
        //
        //  m_pnodeNext
        //      This is a pointer to the next node in the bucket that this
        //      node is part of.
        // -------------------------------------------------------------------
        TElem*      m_pobjData;
        TMyType*    m_pnodeNext;
};


// ---------------------------------------------------------------------------
//   CLASS: TRefKeyedHashSet
//  PREFIX: col
// ---------------------------------------------------------------------------
template <class TElem, class TKey, class TKeyOps>
class TRefKeyedHashSet : public TRefCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass* pclsThis = nullptr;
            if (!pclsThis)
            {
                TBaseLock lockInit;
                pclsThis = new TClass(L"TRefKeyedHashSet<TElem,TKey,TKeyOps>");
            }
            return *pclsThis;
        }


        // -------------------------------------------------------------------
        //  Nested aliases for the node and key ops types used by a keyed hash
        //  set. And one for the key field extraction function that the
        //  user provides. And another is provided for the object equality
        //  function the user provides in some methods.
        // -------------------------------------------------------------------
        using TMyType = TRefKeyedHashSet<TElem, TKey, TKeyOps>;
        using TNode = TRefKeyedHashSetNode<TElem,TKey> ;
        using TKeyExtract = const TKey& (*)(const TElem&);


        // -------------------------------------------------------------------
        //  Our nested cursor classes
        // -------------------------------------------------------------------
        template <class TElem, class TKey, class TKeyOps> class TConstCursor :

            public TBiColCursor<TElem>
        {
            public  :
                // -----------------------------------------------------------
                //  Public types
                // -----------------------------------------------------------
                using TParent = TBiColCursor<TElem>;


                // -----------------------------------------------------------
                //  Public, static methods
                // -----------------------------------------------------------
                static const TClass& clsThis()
                {
                    static const TClass* pclsThis = nullptr;
                    if (!pclsThis)
                    {
                        TBaseLock lockInit;
                        pclsThis = new TClass(L"TRefKeyedHashSet::TConstCursor<TElem,TKey,TKeyOps>");
                    }
                    return *pclsThis;
                }


                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TConstCursor() :

                    m_hshBadFirst(tCIDLib::THashVal(-1))
                    , m_hshBadLast(0)
                    , m_hshCurBucket(0)
                    , m_pcolCursoring(nullptr)
                    , m_pnodeCur(nullptr)
                {
                }

                explicit TConstCursor(const TMyType* pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_hshBadFirst(tCIDLib::THashVal(-1))
                    , m_hshBadLast(pcolToCursor->c4HashModulus())
                    , m_hshCurBucket(0)
                    , m_pcolCursoring(pcolToCursor)
                    , m_pnodeCur(nullptr)
                {
                    m_pnodeCur = m_pcolCursoring->pnodeFindFirst(m_hshCurBucket);
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadLast;
                }

                // We have to lock first, so we can't use member init!
                TConstCursor(const TConstCursor& cursSrc) :

                    m_hshBadFirst(tCIDLib::THashVal(-1))
                {
                    operator=(cursSrc);
                }

                ~TConstCursor() {}


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TConstCursor& operator=(const TConstCursor& cursSrc)
                {
                    // Don't have to copy bad first, it's the same for everyone
                    if (this != &cursSrc)
                    {
                        TMtxLocker lockCol
                        (
                            cursSrc.m_pcolCursoring
                            ? cursSrc.m_pcolCursoring->pmtxLock() : nullptr
                        );

                        TParent::operator=(cursSrc);
                        m_hshBadLast    = cursSrc.m_hshBadLast;
                        m_hshCurBucket  = cursSrc.m_hshCurBucket;
                        m_pcolCursoring = cursSrc.m_pcolCursoring;
                        m_pnodeCur      = cursSrc.m_pnodeCur;
                    }
                    return *this;
                }

                tCIDLib::TBoolean operator==(const TConstCursor& cursSrc) const
                {
                    if (!TParent::operator==(cursSrc))
                        return kCIDLib::False;

                    return
                    (
                        (m_pcolCursoring == cursSrc.m_pcolCursoring)
                        && (m_hshCurBucket == cursSrc.m_hshCurBucket)
                        && (m_pnodeCur == cursSrc.m_pnodeCur)
                    );
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
                tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
                {
                    if (clsTarget == clsThis())
                        return kCIDLib::True;
                    return TParent::bIsDescendantOf(clsTarget);
                }

                tCIDLib::TBoolean bIsValid() const override
                {
                    if (!TParent::bIsValid())
                        return kCIDLib::False;

                    // The current bucket can be bad in either direction
                    return
                    (
                        (m_hshCurBucket != m_hshBadFirst)
                        && (m_hshCurBucket != m_hshBadLast)
                    );
                }

                tCIDLib::TBoolean bNext() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    if (m_pnodeCur)
                        m_pnodeCur = m_pcolCursoring->pnodeFindNext(m_pnodeCur, m_hshCurBucket);

                    // If not found, set is invalid in the forward direction
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadLast;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bPrevious() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    if (m_pnodeCur)
                        m_pnodeCur = m_pcolCursoring->pnodeFindPrevious(m_pnodeCur, m_hshCurBucket);

                    // If not found, set is invalid in the forward direction
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadLast;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    m_pnodeCur = m_pcolCursoring->pnodeFindFirst(m_hshCurBucket);

                    // If not, set us to bad last
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadLast;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bSeekToEnd() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    m_pnodeCur = m_pcolCursoring->pnodeFindLast(m_hshCurBucket);
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());

                    // If no such node, set us to bad last
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadLast;
                    return (m_pnodeCur != nullptr);
                }

                const TClass& clsIsA() const override
                {
                    return clsThis();
                }

                const TClass& clsParent() const override
                {
                    return TParent::clsThis();
                }

                const TElem& objRCur() const override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    return m_pnodeCur->objData();
                }


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::THashVal hshCurHash() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    return m_hshCurBucket;
                }

                const TNode* pnodeCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }

                TNode* pnodeCur()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }


            protected   :
                // -----------------------------------------------------------
                //  Declare our friends
                // -----------------------------------------------------------
                friend class TMyType;


                // -----------------------------------------------------------
                //  Hidden constructors (for the collection itself)
                // -----------------------------------------------------------
                TConstCursor(const TMyType*             pcolToCursor
                            , const tCIDLib::THashVal   hshCur
                            ,       TNode* const        pnodeCur) :

                    TParent(pcolToCursor)
                    , m_hshBadFirst(tCIDLib::THashVal(-1))
                    , m_hshBadLast(pcolToCursor->c4HashModulus())
                    , m_hshCurBucket(hshCur)
                    , m_pcolCursoring(pcolToCursor)
                    , m_pnodeCur(pnodeCur)
                {
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_hshBadFirst
                //  m_hshBadLast
                //      We set this at ctor and use them as invalid cursor markers
                //      in the two directions. First is -1 cast to a hash. And bad
                //      is the hash modulus cast to a hash.
                //
                //  m_hshCurBucket
                //      The hash of the bucket within the set that we are cursoring.
                //      The hash table has a bucket per possible hash value, so we
                //      have to keep up with the one we are working on.
                //
                //  m_pcolCursoring
                //      A pointer to the hash set collection that we are cursoring
                //
                //  m_pnodeCur
                //      The current node in the hash table, 0 if not on a valid node
                //      at this time.
                // -----------------------------------------------------------
                const tCIDLib::THashVal m_hshBadFirst;
                tCIDLib::THashVal       m_hshBadLast;
                tCIDLib::THashVal       m_hshCurBucket;
                const TMyType*          m_pcolCursoring;
                TNode*                  m_pnodeCur;
        };


        template <class TElem, class TKey, class TKeyOps> class TNonConstCursor :

            public TConstCursor<TElem, TKey, TKeyOps>
        {
            public  :
                // -----------------------------------------------------------
                //  Public types
                // -----------------------------------------------------------
                using TParent = TConstCursor<TElem, TKey, TKeyOps>;


                // -----------------------------------------------------------
                //  Public, static methods
                // -----------------------------------------------------------
                static const TClass& clsThis()
                {
                    static const TClass* pclsThis = nullptr;
                    if (!pclsThis)
                    {
                        TBaseLock lockInit;
                        pclsThis = new TClass(L"TRefKeyedHashSet::TNonConstCursor<TElem,TKey,TKeyOps>");
                    }
                    return *pclsThis;
                }


                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TNonConstCursor() :

                    m_pcolNCCursoring(nullptr)
                {
                }

                explicit TNonConstCursor(TMyType* pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_pcolNCCursoring(pcolToCursor)
                {
                }

                // We have to lock first, so we can't use member init!
                TNonConstCursor(const TNonConstCursor& cursSrc)
                {
                    operator=(cursSrc);
                }

                ~TNonConstCursor() {}


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
                //  Public, inherited methods
                // -----------------------------------------------------------
                tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
                {
                    if (clsTarget == clsThis())
                        return kCIDLib::True;
                    return TParent::bIsDescendantOf(clsTarget);
                }

                const TClass& clsIsA() const override
                {
                    return clsThis();
                }

                const TClass& clsParent() const override
                {
                    return TParent::clsThis();
                }


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                TElem& objWCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TMtxLocker lockCol(m_pcolNCCursoring->pmtxLock());
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return const_cast<TElem&>(this->pnodeCur()->objData());
                }


            protected   :
                // -----------------------------------------------------------
                //  Declare our friends
                // -----------------------------------------------------------
                friend class TMyType;


                // -----------------------------------------------------------
                //  Hidden constructors (for the collection itself)
                // -----------------------------------------------------------
                TNonConstCursor(        TMyType*            pcolToCursor
                                , const tCIDLib::THashVal   hshCur
                                ,       TNode* const        pnodeCur) :

                    TParent(pcolToCursor, hshCur, pnodeCur)
                    , m_pcolNCCursoring(pcolToCursor)
                {
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolNCCursoring
                //      A pointer to the hash set collection that we are
                //      cursoring. We need our own non-const pointer.
                // -----------------------------------------------------------
                TMyType*    m_pcolNCCursoring;
        };


        // -------------------------------------------------------------------
        //  Typedefs for our nested cursor classes
        // -------------------------------------------------------------------
        using TCursor   = TConstCursor<TElem, TKey, TKeyOps>;
        using TNCCursor = TNonConstCursor<TElem, TKey, TKeyOps>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefKeyedHashSet<TElem, TKey, TKeyOps>() = delete;

        TRefKeyedHashSet(const  tCIDLib::EAdoptOpts eAdopt
                        , const tCIDLib::TCard4     c4Modulus
                        ,       TKeyOps* const      pkopsToAdopt
                        ,       TKeyExtract         pfnKeyExtract
                        , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TRefCollection<TElem>(eMTSafe)
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(c4Modulus)
            , m_eAdopt(eAdopt)
            , m_pfnKeyExtract(pfnKeyExtract)
            , m_pkopsToUse(pkopsToAdopt)
        {
            try
            {
                // Allocate and initialize the bucket array
                m_apBuckets = new TNode*[c4Modulus];
                TRawMem::SetMemBuf
                (
                    m_apBuckets
                    , tCIDLib::TCard1(0)
                    , sizeof(tCIDLib::TVoid*) * c4Modulus
                );
            }

            catch(...)
            {
                delete [] m_apBuckets;
                delete pkopsToAdopt;
                throw;
            }
        }

        TRefKeyedHashSet(const TMyType&) = delete;
        TRefKeyedHashSet( TMyType&&) = delete;

        ~TRefKeyedHashSet()
        {
            // Flush the collection, destroying user data if we are adopting
            RemoveAllElems(m_eAdopt == tCIDLib::EAdoptOpts::Adopt);

            // Delete the key ops object
            delete m_pkopsToUse;

            // And delete the bucket list itself
            delete [] m_apBuckets;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType&) = delete;
        TMyType& operator=( TMyType&&) = delete;

        const TElem& operator[](const TKey& objKeyToFind) const
        {
            // Just delegate to the other method that does the same thing
            return *pobjFindByKey(objKeyToFind);
        }

        TElem& operator[](const TKey& objKeyToFind)
        {
            // Just delegate to the other method that does the same thing
            return *pobjFindByKey(objKeyToFind);
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add(TElem* const pobjToAdd) override
        {
            if (!pobjToAdd)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            TMtxLocker lockSync(this->pmtxLock());

            // See if this element is already in the collection
            const TKey& objKey = m_pfnKeyExtract(*pobjToAdd);
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(objKey, hshElem);

            //
            //  If so, we cannot allow it. If we are adopting, then delete
            //  the data before we throw.
            //
            if (pnodeCheck)
            {
                // We can't delete the object until after the throw!
                TJanitor<TElem> janElem(0);
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    janElem.Set(pobjToAdd);
                this->DuplicateKey(objKey, CID_FILE, CID_LINE);
            }

            //
            //  Add it to the appropriate bucket. We just put it at the head
            //  since the order does not matter. We just construct the
            //  node and pass it the current head, which it will make its
            //  next node.
            //
            m_apBuckets[hshElem] = new TNode(pobjToAdd, m_apBuckets[hshElem]);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Bump up the element count
            m_c4CurElements++;
        }

        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TRefCollection<TElem>::bIsDescendantOf(clsTarget);
        }

        tCIDLib::TBoolean bIsEmpty() const override
        {
            TMtxLocker lockSync(this->pmtxLock());
            return (m_c4CurElements == 0);
        }

        tCIDLib::TCard4 c4ElemCount() const override
        {
            TMtxLocker lockSync(this->pmtxLock());
             return m_c4CurElements;
        }

        const TClass& clsIsA() const override
        {
            return clsThis();
        }

        const TClass& clsParent() const override
        {
            return TRefCollection<TElem>::clsThis();
        }

        tCIDLib::EAdoptOpts eAdopt() const override
        {
            return m_eAdopt;
        }

        tCIDLib::TVoid GiveAllTo(TRefCollection<TElem>& colTarget) override
        {
            // Look and add all of our items to the target
            TMtxLocker lockThis(this->pmtxLock());

            // Orphan all the user data objects to the target collection
            tCIDLib::THashVal hshCur;
            TNode* pnodeCur = pnodeFindFirst(hshCur);
            while (pnodeCur)
            {
                colTarget.Add(pnodeCur->pobjOrphan());
                pnodeCur = pnodeFindNext(pnodeCur, hshCur);
            }

            //
            //  Now flush ourself, which leaves them in his hands. This will
            //  clean up our node data, but there won't be any user data to
            //  destroy now.
            //
            RemoveAll();
        }

        tCIDLib::TVoid OrphanElem(TElem* const pobjToOrphan) override
        {
            TMtxLocker lockSync(this->pmtxLock());

            // Get the hash of the element
            const TKey& objKey = m_pfnKeyExtract(*pobjToOrphan);
            const tCIDLib::THashVal hshElem = m_pkopsToUse->hshKey
            (
                objKey
                , m_c4HashModulus
            );

            // Search the bucket if its not empty
            TNode* pnodePrev = nullptr;
            TNode* pnodeToOrphan = m_apBuckets[hshElem];
            while (pnodeToOrphan)
            {
                // If this key matches, then break out
                if (m_pkopsToUse->bCompKeys
                (
                    m_pfnKeyExtract(pnodeToOrphan->objData())
                    , objKey))
                {
                    break;
                }

                pnodePrev = pnodeToOrphan;
                pnodeToOrphan = pnodeToOrphan->pnodeNext();
            }

            if (!pnodeToOrphan)
                this->KeyNotFound(CID_FILE, CID_LINE);

            //
            //  If the previous node is 0, then this is the head node, else we
            //  have to point the previous node out the find node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshElem] = pnodeToOrphan->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeToOrphan->pnodeNext());

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // We can now toast the found node and reduce the element count
            delete pnodeToOrphan;
            m_c4CurElements--;
        }

        TCursor* pcursNew() const override
        {
            TMtxLocker lockSync(this->pmtxLock());
            return new TCursor(this);
        }

        tCIDLib::TVoid RemoveAll() override
        {
            TMtxLocker lockSync(this->pmtxLock());
            if (!m_c4CurElements)
                return;

            // If adopting, we delete all user data as well
            RemoveAllElems(m_eAdopt == tCIDLib::EAdoptOpts::Adopt);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();
        }

        tCIDLib::TVoid RemoveElem(TElem* const pobjToRemove) override
        {
            TMtxLocker lockSync(this->pmtxLock());

            // Get the hash of the element
            const TKey& objKey = m_pfnKeyExtract(*pobjToRemove);
            const tCIDLib::THashVal hshElem = m_pkopsToUse->hshKey
            (
                objKey
                , m_c4HashModulus
            );

            // Search the bucket if its not empty
            TNode* pnodePrev = nullptr;
            TNode* pnodeToRemove = m_apBuckets[hshElem];
            while (pnodeToRemove)
            {
                // If this key matches, then break out
                if (m_pkopsToUse->bCompKeys
                (
                    m_pfnKeyExtract(pnodeToRemove->objData())
                    , objKey))
                {
                    break;
                }

                pnodePrev = pnodeToRemove;
                pnodeToRemove = pnodeToRemove->pnodeNext();
            }

            if (!pnodeToRemove)
                this->KeyNotFound(CID_FILE, CID_LINE);

            //
            //  If the previous node is 0, then this is the head node, else we
            //  have to point the previous node out the find node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshElem] = pnodeToRemove->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeToRemove->pnodeNext());

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  We can now toast the found node and reduce the element count.
            //  If we are adopting, then clean up the user data.
            //
            if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                pnodeToRemove->DeleteData();
            delete pnodeToRemove;
            m_c4CurElements--;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAddIfNew(TElem* const pobjToAdd)
        {
            if (!pobjToAdd)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            TMtxLocker lockSync(this->pmtxLock());

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeRet = pnodeFind(m_pfnKeyExtract(*pobjToAdd), hshElem);

            // If already exists, then return false
            if (pnodeRet)
            {
                // If adopting, clean up the object since it's our responsibility
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    delete pobjToAdd;
                return kCIDLib::False;
            }

            // Not already there, so add it and return true
            Add(pobjToAdd);
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bKeyExists(const TKey& keyToFind) const
        {
            TMtxLocker lockSync(this->pmtxLock());

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(keyToFind, hshElem);

            return (pnodeCheck != nullptr);
        }

        tCIDLib::TBoolean
        bRemoveKey( const   TKey&               objKeyToRemove
                    , const tCIDLib::TBoolean   bThrowIfNot = kCIDLib::True)
        {
            TMtxLocker lockSync(this->pmtxLock());

            // Get the hash of the element
            const tCIDLib::THashVal hshElem = m_pkopsToUse->hshKey
            (
                objKeyToRemove
                , m_c4HashModulus
            );

            // Search the bucket if its not empty
            TNode* pnodePrev = nullptr;
            TNode* pnodeToRemove = m_apBuckets[hshElem];
            while (pnodeToRemove)
            {
                // If this key matches, then break out
                if (m_pkopsToUse->bCompKeys
                (
                    m_pfnKeyExtract(pnodeToRemove->objData())
                    , objKeyToRemove))
                {
                    break;
                }

                pnodePrev = pnodeToRemove;
                pnodeToRemove = pnodeToRemove->pnodeNext();
            }

            if (!pnodeToRemove)
            {
                // Throw if told to, else just return false
                if (bThrowIfNot)
                    this->KeyNotFound(CID_FILE, CID_LINE);
                return kCIDLib::False;
            }

            //
            //  If the previous node is 0, then this is the head node, else we
            //  have to point the previous node out the find node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshElem] = pnodeToRemove->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeToRemove->pnodeNext());

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            //
            //  We can now toast the found node and reduce the element count.
            //  If we are adopting, then clean up the user data.
            //
            if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                pnodeToRemove->DeleteData();
            delete pnodeToRemove;
            m_c4CurElements--;
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bRemoveKeyIfExists(const TKey& objKeyToRemove)
        {
            // Just call the other version, indicating not to throw
            return bRemoveKey(objKeyToRemove, kCIDLib::False);
        }

        tCIDLib::TCard4 c4HashModulus() const
        {
            TMtxLocker lockSync(this->pmtxLock());
            return m_c4HashModulus;
        }


        //
        //  Return a cursor for a specific key. We find the node (or lack thereof)
        //  and set up a cursor using a protected constructor. We call pnodeFind
        //  to do the lookup.
        //
        TCursor cursFindByKey(const TKey& objKeyToFind) const
        {
            TMtxLocker lockSync(this->pmtxLock());

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);
            if (!pnodeRet)
                hshKey = m_c4HashModulus;
            return TCursor(this, hshKey, pnodeRet);
        }

        TNCCursor cursFindByKey(const TKey& objKeyToFind)
        {
            TMtxLocker lockSync(this->pmtxLock());

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);
            if (!pnodeRet)
                hshKey = m_c4HashModulus;
            return TNCCursor(this, hshKey, pnodeRet);
        }


        //
        //  We have to do non-const in each collection derivative. The base collection
        //  class doesn't know about non-const cursors so it can't do this generically.
        //  The collection is constant here, just the elements are non-const.
        //
        //  DO NOT change the element in a way that would modify the hash!
        //
        template <typename IterCB> tCIDLib::TVoid ForEachNC(IterCB iterCB) const
        {
            TMtxLocker lockThis(this->pmtxLock());

            tCIDLib::THashVal   hshCurBucket = 0;
            TNode*              pnodeCur = pnodeFindFirst(hshCurBucket);
            while (pnodeCur)
            {
                if (!iterCB(pnodeCur->objData()))
                    break;

                // In debug, make sure they didn't modify the hash of this element
                #if CID_DEBUG_ON
                const tCIDLib::THashVal hshCheck = m_pkopsToUse->hshKey
                (
                    m_pfnKeyExtract(pnodeCur->objData()), m_c4HashModulus
                );
                if (hshCheck != hshCurBucket)
                    this->HashChanged(CID_FILE, CID_LINE);
                #endif

                pnodeCur = pnodeFindNext(pnodeCur, hshCurBucket);
            }
        }


        // If the key exists, extract the associated element
        TElem* pobjExtract(const TKey& keyToFind)
        {
            TMtxLocker lockSync(this->pmtxLock());

            // Get the hash of the element
            const tCIDLib::THashVal hshElem = m_pkopsToUse->hshKey
            (
                keyToFind, m_c4HashModulus
            );

            // Search the bucket if its not empty
            TNode* pnodePrev = nullptr;
            TNode* pnodeToRemove = m_apBuckets[hshElem];
            while (pnodeToRemove)
            {
                // If this key matches, then break out
                if (m_pkopsToUse->bCompKeys
                (
                    m_pfnKeyExtract(pnodeToRemove->objData())
                    , keyToFind))
                {
                    break;
                }

                pnodePrev = pnodeToRemove;
                pnodeToRemove = pnodeToRemove->pnodeNext();
            }

            if (!pnodeToRemove)
                return nullptr;

            //
            //  If the previous node is 0, then this is the head node, else we
            //  have to point the previous node out the find node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshElem] = pnodeToRemove->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeToRemove->pnodeNext());

            // Get the object out before we toast it
            TElem* pobjRet = pnodeToRemove->pobjOrphan();

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // So we can now toast the found node and reduce the element count
            delete pnodeToRemove;
            m_c4CurElements--;

            return pobjRet;
        }

        TElem* pobjFindByKey(const  TKey&               objKeyToFind
                            , const tCIDLib::TBoolean   bThrowIfNotFound = kCIDLib::True)
        {
            TMtxLocker lockSync(this->pmtxLock());

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so throw an exception or return null
            if (!pnodeRet)
            {
                if (bThrowIfNotFound)
                    this->KeyNotFound(CID_FILE, CID_LINE);
                return nullptr;
            }

            // we found it so return the object pointer
            return pnodeRet->pobjData();
        }

        const TElem* pobjFindByKey( const   TKey&             objKeyToFind
                                    , const tCIDLib::TBoolean bThrowIfNotFound = kCIDLib::True) const

        {
            TMtxLocker lockSync(this->pmtxLock());

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so throw an exception or return null
            if (!pnodeRet)
            {
                if (bThrowIfNotFound)
                    this->KeyNotFound(CID_FILE, CID_LINE);
                return nullptr;
            }

            // we found it so return the object pointer
            return pnodeRet->pobjData();
        }

        //
        //  Even if adopting, this allows us to just release all of the elements,
        //  because someone else has them. We do have to clean up our data, but not
        //  the user data.
        //
        tCIDLib::TVoid OrphanAll()
        {
            TMtxLocker lockSync(this->pmtxLock());

            // Remove all elements and tell it not to delete user data
            RemoveAllElems(kCIDLib::False);
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TMtxLocker lockSync(this->pmtxLock());

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            //
            // We have to get the bucket for the current element and find the
            //  previous node for the target node.
            //
            TNode* pnodeToRemove = cursAt.pnodeCur();
            const tCIDLib::THashVal hshElem = cursAt.hshCurHash();
            TNode* const pnodePrev = pnodeFindPrev(pnodeToRemove, hshElem);

            // Move the cursor to the next node past the target
            cursAt.bNext();

            //
            //  If the previous node is 0, then this is the head node,
            //  else we have to point the previous node out the find
            //  node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshElem] = pnodeToRemove->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeToRemove->pnodeNext());

            //
            //  So we can now toast the found node and bump the element count.
            //  If we are adopting, then clean up the user data.
            //
            if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                pnodeToRemove->DeleteData();
            delete pnodeToRemove;
            m_c4CurElements--;

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Get the cursor back into sync.
            cursAt.c4SerialNum(this->c4SerialNum());
        }

        tCIDLib::TVoid Set( const   tCIDLib::TCard4     c4MaxCount
                            , const tCIDLib::EMTStates  eToSet
                            , const tCIDLib::TCard4     c4HashModulus)
        {
            TMtxLocker lockSync(this->pmtxLock());

            //
            //  First we have to remove all elements from the collection, deleting
            //  all user data if we are adopting.
            //
            RemoveAllElems(m_eAdopt == tCIDLib::EAdoptOpts::Adopt);

            // Call our parent to set the thread safety state
            this->eMTState(eToSet);

            // Reset the buckets if the hash mod changed
            if (c4HashModulus != m_c4HashModulus)
            {
                delete [] m_apBuckets;
                m_apBuckets = new TNode*[m_c4HashModulus];
                m_c4HashModulus = c4HashModulus;
                TRawMem::SetMemBuf
                (
                    m_apBuckets
                    , tCIDLib::TCard1(0)
                    , sizeof(tCIDLib::TVoid*) * m_c4HashModulus
                );
            }

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();
        }


    protected  :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TConstCursor<TElem,TKey,TKeyOps>;
        friend class TNonConstCursor<TElem,TKey,TKeyOps>;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        TNode* pnodeFindFirst(tCIDLib::THashVal& hshToUpdate) const
        {
            for (tCIDLib::TCard4 c4BucketInd = 0; c4BucketInd < m_c4HashModulus; c4BucketInd++)
            {
                if (m_apBuckets[c4BucketInd])
                {
                    hshToUpdate = c4BucketInd;
                    return m_apBuckets[c4BucketInd];
                }
            }
            return nullptr;
        }

        TNode* pnodeFindLast(tCIDLib::THashVal& hshToUpdate) const
        {
            // Start at the last bucket and work back
            tCIDLib::TCard4 c4BucketInd = m_c4HashModulus - 1;

            tCIDLib::TBoolean bDone = kCIDLib::False;
            while (!bDone)
            {
                // If we find a non-empty bucket, find its last node
                if (m_apBuckets[c4BucketInd])
                {
                    TNode* pnodeCur = m_apBuckets[c4BucketInd];
                    while (pnodeCur->pnodeNext())
                        pnodeCur = pnodeCur->pnodeNext();
                    hshToUpdate = c4BucketInd;
                    return pnodeCur;
                }

                // If we are at the zeroth bucket, we didn't find any
                if (!c4BucketInd)
                    break;
                c4BucketInd--;
            }
            return 0;
        }

        TNode* pnodeFindNext(TNode* pnodeLast, tCIDLib::THashVal& hshToUpdate) const
        {
            // Move up to the next node
            TNode* m_pnodeCur = pnodeLast->pnodeNext();
            if (m_pnodeCur)
                return m_pnodeCur;

            // Search subsequent bucket for non-empty one
            tCIDLib::TBoolean bDone = kCIDLib::False;
            while (!bDone)
            {
                // If we hit the end of buckets, then we are done
                hshToUpdate++;
                if (hshToUpdate == m_c4HashModulus)
                    break;

                // If we find one that's not empty, we start there next
                if (m_apBuckets[hshToUpdate])
                    return m_apBuckets[hshToUpdate];
            }
            return nullptr;
        }


        //
        //  Search forward through the buckets for the next node after the one
        //  passed. Null if we are aleady at teh first. The hash is updated to
        //  the bucket that the next one is found it otherwise.
        //
        TNode* pnodeFindPrevious(TNode* pnodeLast, tCIDLib::THashVal& hshToUpdate) const
        {
            //
            //  Move back to the previous node in the current bucket. If its a legal
            //  node, then we are done.
            //
            TNode* pnodeCur = pnodeFindPrev(pnodeLast, hshToUpdate);
            if (pnodeCur)
                return pnodeCur;

            // Search previous buckets for non-empty one
            tCIDLib::TBoolean bDone = kCIDLib::False;
            while (!bDone)
            {
                // If we hit the start of buckets, then we are done
                if (!hshToUpdate)
                    break;
                hshToUpdate--;

                //
                //  If we find one that's not empty, then go to its last
                //  node and start there.
                //
                if (m_apBuckets[hshToUpdate])
                {
                    pnodeCur = m_apBuckets[hshToUpdate];
                    while (pnodeCur->pnodeNext())
                        pnodeCur = pnodeCur->pnodeNext();
                    return pnodeCur;
                }
            }
            return nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TNode* pnodeFind(const TKey& objKeyToFind, tCIDLib::THashVal& hshElem) const
        {
            // Get the hash of the element
            hshElem = m_pkopsToUse->hshKey(objKeyToFind, m_c4HashModulus);

            TNode* pnodeCur = m_apBuckets[hshElem];
            while (pnodeCur)
            {
                if (m_pkopsToUse->bCompKeys(m_pfnKeyExtract(pnodeCur->objData())
                                            , objKeyToFind))
                {
                    break;
                }
                pnodeCur = pnodeCur->pnodeNext();
            }
            return pnodeCur;
        }

        TNode* pnodeFindPrev(const  TNode* const        pnodeToFind
                            , const tCIDLib::THashVal   hshBucket) const
        {
            // Search this bucket for the target node and return its previous
            TNode* pnodeCur = m_apBuckets[hshBucket];

            // Watch for passed node being the bucket head or it being zero
            if ((pnodeCur == pnodeToFind) || !pnodeToFind)
                return nullptr;

            // That's not it, so search the bucket
            while (pnodeCur)
            {
                if (pnodeCur->pnodeNext() == pnodeToFind)
                    return pnodeCur;
                pnodeCur = pnodeCur->pnodeNext();
            }

            this->NodeNotFound(CID_FILE, CID_LINE);

            // Make the compiler happy, but won't get here
            return nullptr;
        }

        tCIDLib::TVoid RemoveAllElems(const tCIDLib::TBoolean bDelUserData)
        {
            // For each bucket, iterator it and delete its elements
            for (tCIDLib::TCard4 c4BucketInd = 0; c4BucketInd < m_c4HashModulus; c4BucketInd++)
            {
                if (!m_apBuckets[c4BucketInd])
                    continue;

                TNode* pnodeCur = m_apBuckets[c4BucketInd];
                while (pnodeCur)
                {
                    // Get the next node before we toast this one
                    TNode* pnodeNext = pnodeCur->pnodeNext();

                    // If asked, delete the user data
                    if (bDelUserData)
                        pnodeCur->DeleteData();

                    // Now delete the node itself
                    delete pnodeCur;
                    pnodeCur = pnodeNext;
                }

                // Null the buck head node
                m_apBuckets[c4BucketInd] = nullptr;
            }

            // Now reset the needed members
             m_c4CurElements = 0;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_apBuckets
        //      This is the array of hash table buckets. Each list holds the
        //      elements that hash to that index, in the form of a linked list.
        //      This guy is allocated to the size of the hash modulus.
        //
        //  m_c4CurElements
        //      The current number of elements that the table has.
        //
        //  m_c4HashModulus
        //      The modulus divisor for the hash. This is also the number
        //      elements allocated for the m_alstTable array.
        //
        //  m_eAdopt
        //      Indicates whether we are to adopt our element.
        //
        //  m_pfnKeyExtract
        //      The key extraction function provided by the user. It pulls
        //      out a reference to the key field from the data field.
        //
        //  m_pkopsToUse
        //      A key ops object that provides all of the operations that
        //      we have to do on key field objects. We own and destruct it
        //      when we destruct.
        // -------------------------------------------------------------------
        TNode**             m_apBuckets;
        tCIDLib::TCard4     m_c4CurElements;
        tCIDLib::TCard4     m_c4HashModulus;
        tCIDLib::EAdoptOpts m_eAdopt;
        TKeyExtract         m_pfnKeyExtract;
        TKeyOps*            m_pkopsToUse;
};

#pragma CIDLIB_POPPACK


