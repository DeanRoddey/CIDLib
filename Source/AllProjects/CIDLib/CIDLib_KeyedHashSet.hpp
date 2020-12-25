//
// FILE NAME: CIDLib_KeyedHashSet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/22/1997
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
//  This is the public header for the TKeyedHashSet class. This class provides
//  an unordered collection of keyed elements. They are 'keyed' because only
//  some subset of their contents is considered their value for purposes of
//  lookup, as apposed to the whole value of the elements. Duplicate keys are
//  not allowed, since it is a set.
//
//  What comprises the key is very flexible because it takes a key extraction
//  method that handles getting the key out of an element. Hashing of the key
//  and comparison of keys is handled by a key ops object that is provided
//  the client code.
//
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
template <typename TElem,class TKey,class TKeyOps> class TKeyedHashSet;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKeyedHashSetNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <typename TElem,class TKey> class TKeyedHashSetNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKeyedHashSetNode(const TKeyedHashSetNode<TElem,TKey>&) = delete;

        TKeyedHashSetNode(  const   TElem& objData
                            ,       TKeyedHashSetNode<TElem,TKey>* pnodeNext) :

            m_objData(objData)
            , m_pnodeNext(pnodeNext)
        {
        }

        TKeyedHashSetNode(  TElem&&                             objData
                            , TKeyedHashSetNode<TElem,TKey>*    pnodeNext) :

            m_objData(tCIDLib::ForceMove(objData))
            , m_pnodeNext(pnodeNext)
        {
        }

        //
        //  A special one for in place elements. We can't figure out the next
        //  node until after this object is built and the element constructed.
        //  We have a special enum to allow this constructor to never clash
        //  with the one above.
        //
        enum class EForceCall { Val1 };
        template <typename... TArgs>
        TKeyedHashSetNode(const EForceCall, TArgs&&... Args) :

            m_objData(tCIDLib::Forward<TArgs>(Args)...)
            , m_pnodeNext(nullptr)
        {
        }

        ~TKeyedHashSetNode() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKeyedHashSetNode<TElem,TKey>& operator=(const TKeyedHashSetNode<TElem,TKey>&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TElem& objData() const
        {
            return m_objData;
        }

        TElem& objData()
        {
            return m_objData;
        }

        TElem& objData(const TElem& objToSet)
        {
            m_objData = objToSet;
            return m_objData;
        }

        TKeyedHashSetNode<TElem,TKey>* pnodeNext()
        {
            return m_pnodeNext;
        }

        TKeyedHashSetNode<TElem,TKey>*
        pnodeNext(TKeyedHashSetNode<TElem,TKey>* const pnodeToSet)
        {
            m_pnodeNext = pnodeToSet;
            return m_pnodeNext;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_objData
        //      This is the data for this node. Its a by value copy of the
        //      user's data. It contains the key field as well.
        //
        //  m_pnodeNext
        //      This is a pointer to the next node in the bucket that this
        //      node is part of.
        // -------------------------------------------------------------------
        TElem                           m_objData;
        TKeyedHashSetNode<TElem,TKey>*  m_pnodeNext;
};


// ---------------------------------------------------------------------------
//   CLASS: TKeyedHashSet
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem, class TKey, class TKeyOps>
class TKeyedHashSet : public TCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"TKeyedHashSet<TElem,TKey,TKeyOps>");
            return clsRet;
        }


        // -------------------------------------------------------------------
        //  Nested aliases for the node and key ops types used by a keyed hash
        //  set. And one for the key field extraction function that the
        //  user provides. And another is provided for the object equality
        //  function the user provides in some methods.
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = TKeyedHashSet<TElem, TKey, TKeyOps>;
        using TParType = TCollection<TElem>;
        using TNode = TKeyedHashSetNode<TElem, TKey>;
        using TKeyExtract = const TKey& (*)(const TElem&);



        // -------------------------------------------------------------------
        //  Our nested cursor classes
        // -------------------------------------------------------------------
        template <typename TElem, class TKey, class TKeyOps> class TConstCursor :

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
                        pclsThis = new TClass(L"TKeyedHashSet::TConstCursor<TElem,TKey,TKeyOps>");
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
                        TLocker lockrCol(cursSrc.m_pcolCursoring);
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

                    TLocker lockrCol(m_pcolCursoring);
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

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    if (m_pnodeCur)
                        m_pnodeCur = m_pcolCursoring->pnodeFindPrevious(m_pnodeCur, m_hshCurBucket);

                    // If not found, set our bucket to bad first
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadFirst;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
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

                    TLocker lockrCol(m_pcolCursoring);
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

                    TLocker lockrCol(m_pcolCursoring);
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

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    return m_hshCurBucket;
                }

                const TNode* pnodeCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    return m_pnodeCur;
                }

                TNode* pnodeCur()
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
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
                //      is the hash modulus (which an actual hash cannot ever be and
                //      what we naturally get when iterating through all the buckets
                //      and not finding anything, so we end up past the last bucket at
                //      the modulus.)
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
                //      The current node in the hash table, if the current hash bucket
                //      is valid, this must be valid, else it is meaningless and should
                //      be set to null.
                // -----------------------------------------------------------
                const tCIDLib::THashVal m_hshBadFirst;
                tCIDLib::THashVal       m_hshBadLast;
                tCIDLib::THashVal       m_hshCurBucket;
                const TMyType*          m_pcolCursoring;
                TNode*                  m_pnodeCur;
        };


        template <typename TElem, class TKey, class TKeyOps> class TNonConstCursor :
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
                        pclsThis = new TClass(L"TKeyedHashSet::TNonConstCursor<TElem,TKey,TKeyOps>");
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
                    return const_cast<TElem&>(this->pnodeCur()->objData());
                }

                TElem* operator->() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
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
                tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const final
                {
                    if (clsTarget == clsThis())
                        return kCIDLib::True;
                    return TParent::bIsDescendantOf(clsTarget);
                }

                const TClass& clsIsA() const final
                {
                    return clsThis();
                }

                const TClass& clsParent() const final
                {
                    return TParent::clsThis();
                }


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                TElem& objWCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolNCCursoring);
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
        //  Aliases for our nested cursor classes
        // -------------------------------------------------------------------
        using TCursor = TConstCursor<TElem, TKey, TKeyOps>;
        using TNCCursor = TNonConstCursor<TElem, TKey, TKeyOps>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKeyedHashSet<TElem, TKey, TKeyOps>() = delete;

        TKeyedHashSet(  const   tCIDLib::TCard4     c4Modulus
                        , const TKeyOps&            kopsToUse
                        ,       TKeyExtract         pfnKeyExtract
                        , const tCIDLib::EMTStates  eMTState = tCIDLib::EMTStates::Unsafe) :

            TParType(eMTState)
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(c4Modulus)
            , m_pfnKeyExtract(pfnKeyExtract)
            , m_kopsToUse(kopsToUse)
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
                throw;
            }
        }

        TKeyedHashSet(const TMyType& colSrc) :

            TParType(colSrc)
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(colSrc.m_c4HashModulus)
            , m_pfnKeyExtract(colSrc.m_pfnKeyExtract)
            , m_kopsToUse(colSrc.m_kopsToUse)
        {
            try
            {
                // Allocate and initialize the bucket array
                m_apBuckets = new TNode*[m_c4HashModulus];
                TRawMem::SetMemBuf
                (
                    m_apBuckets
                    , tCIDLib::TCard1(0)
                    , sizeof(tCIDLib::TVoid*) * m_c4HashModulus
                );

                // Lock the source, so it won't change during this operation
                TLocker lockrToDup(&colSrc);

                //
                //  If there are any source nodes, then copy them. This is
                //  an unordered set so we can't just do the simple cursor
                //  based loop and add or they will not end in the same
                //  order.
                //
                if (colSrc.c4ElemCount())
                    RepNodes(colSrc, kCIDLib::False);

                // Copy the destination element count
                m_c4CurElements = colSrc.m_c4CurElements;
            }

            catch(...)
            {
                delete [] m_apBuckets;
                throw;
            }
        }

        // Do minimial setup then call move operator
        TKeyedHashSet(const TMyType&& colSrc) :

            TParType(colSrc.eMTSafe())
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(3)
            , m_pfnKeyExtract(colSrc.m_pfnKeyExtract)
            , m_kopsToUse(colSrc.m_kopsToUse)
        {
            m_apBuckets = new TNode*[m_c4HashModulus];
            TRawMem::SetMemBuf
            (
                m_apBuckets, tCIDLib::TCard1(0), sizeof(tCIDLib::TVoid*) * m_c4HashModulus
            );

            *this = tCIDLib::ForceMove(colSrc);
        }

        ~TKeyedHashSet()
        {
            // Flush the collection
            RemoveAll();

            // And delete the bucket list itself
            delete [] m_apBuckets;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& colSrc)
        {
            if (this != &colSrc)
            {
                TLocker lockrUs(this);
                TLocker lockrSrc(&colSrc);

                // Call our parent first
                TParType::operator=(colSrc);

                // Flush our current content before copying
                RemoveAll();

                // If the hash mod is different, re-allocate
                if (m_c4HashModulus != colSrc.m_c4HashModulus)
                {
                    delete [] m_apBuckets;
                    m_apBuckets = nullptr;
                    m_c4HashModulus = colSrc.m_c4HashModulus;
                    m_apBuckets = new TNode*[m_c4HashModulus];
                    TRawMem::SetMemBuf
                    (
                        m_apBuckets
                        , tCIDLib::TCard1(0)
                        , sizeof(tCIDLib::TVoid*) * m_c4HashModulus
                    );
                }

                // If there are any source nodes, then replicate them
                if (colSrc.c4ElemCount())
                    RepNodes(colSrc, kCIDLib::False);

                // And now take his current elements value and dup the key ops
                m_c4CurElements = colSrc.m_c4CurElements;
                m_kopsToUse = colSrc.m_kopsToUse;

                // Invalidate any cursors
                this->c4IncSerialNum();
            }
            return *this;
        }

        //
        //  We have to swap the modulus and key ops as well since they affect the
        //  buckets and where the elements are in them.
        //
        TMyType& operator=(TMyType&& colSrc)
        {
            if (&colSrc != this)
            {
                TLocker lockrSrc(&colSrc);
                TLocker lockrThis(this);

                TParType::operator=(tCIDLib::ForceMove(colSrc));

                tCIDLib::Swap(m_apBuckets, colSrc.m_apBuckets);
                tCIDLib::Swap(m_c4CurElements, colSrc.m_c4CurElements);
                tCIDLib::Swap(m_c4HashModulus, colSrc.m_c4HashModulus);
                tCIDLib::Swap(m_pfnKeyExtract, colSrc.m_pfnKeyExtract);
                tCIDLib::Swap(m_kopsToUse, colSrc.m_kopsToUse);

                // Publish reload events for both
                this->PublishReloaded();
                colSrc.PublishReloaded();
            }
            return *this;
        }

        const TElem& operator[](const TKey& objKeyToFind) const
        {
            // Just delegate to the other method that does the same thing
            return objFindByKey(objKeyToFind);
        }

        TElem& operator[](const TKey& objKeyToFind)
        {
            // Just delegate to the other method that does the same thing
            return objFindByKey(objKeyToFind);
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const final
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TCollection<TElem>::bIsDescendantOf(clsTarget);
        }

        tCIDLib::TBoolean bIsEmpty() const final
        {
            TLocker lockrSync(this);
            return (m_c4CurElements == 0);
        }

        tCIDLib::TCard4 c4ElemCount() const final
        {
            TLocker lockrSync(this);
             return m_c4CurElements;
        }

        const TClass& clsIsA() const final
        {
            return clsThis();
        }

        const TClass& clsParent() const final
        {
            return TCollection<TElem>::clsThis();
        }

        TElem& objAdd(const TElem& objToAdd) final
        {
            TLocker lockrSync(this);

            // See if this element is already in the collection
            const TKey& objKey = m_pfnKeyExtract(objToAdd);
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(objKey, hshElem);

            // If so, we cannot allow it
            if (pnodeCheck)
                this->DuplicateKey(objKey, CID_FILE, CID_LINE);

            //
            //  Add it to the appropriate bucket. We just put it at the head
            //  since the order does not matter. We just construct the
            //  node and pass it the current head, which it will make its
            //  next node.
            //
            m_apBuckets[hshElem] = new TNode(objToAdd, m_apBuckets[hshElem]);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Bump up the element count
            m_c4CurElements++;

            return m_apBuckets[hshElem]->objData();
        }


        [[nodiscard]] TCursor* pcursNew() const final
        {
            TLocker lockrSync(this);
            return new TCursor(this);
        }

        [[nodiscard]] TObject* pobjDuplicate() const final
        {
            TLocker lockrSync(this);
            return new TMyType(*this);
        }

        tCIDLib::TVoid RemoveAll() final
        {
            TLocker lockrSync(this);
            if (!m_c4CurElements)
                return;

            // For each bucket, iterator it and delete its elements.
            for (tCIDLib::TCard4 c4BucketInd = 0; c4BucketInd < m_c4HashModulus; c4BucketInd++)
            {
                if (!m_apBuckets[c4BucketInd])
                    continue;

                TNode* pnodeCur = m_apBuckets[c4BucketInd];
                while (pnodeCur)
                {
                    TNode* pnodeNext = pnodeCur->pnodeNext();
                    delete pnodeCur;
                    pnodeCur = pnodeNext;
                }
                m_apBuckets[c4BucketInd] = nullptr;
            }

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Now reset the needed members
             m_c4CurElements = 0;
        }



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendFrom(const TMyType& colSrc)
        {
            if (colSrc.c4ElemCount())
            {
                TCursor cursSrc(&colSrc);
                for (; cursSrc; ++cursSrc)
                    objAdd(*cursSrc);
            }
        }

        tCIDLib::TBoolean bExtract(const TKey& keyToFind, TElem& objToFill)
        {
            TLocker lockrSync(this);

            // Get the hash of the element
            const tCIDLib::THashVal hshElem = m_kopsToUse.hshKey
            (
                keyToFind, m_c4HashModulus
            );

            // Search the bucket if its not empty
            TNode* pnodePrev = nullptr;
            TNode* pnodeToRemove = m_apBuckets[hshElem];
            while (pnodeToRemove)
            {
                // If this key matches, then break out
                if (m_kopsToUse.bCompKeys
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
                return kCIDLib::False;

            //
            //  If the previous node is 0, then this is the head node, else we
            //  have to point the previous node out the find node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshElem] = pnodeToRemove->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeToRemove->pnodeNext());

            // Get the object out before we toast it
            objToFill = pnodeToRemove->objData();

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // So we can now toast the found node and reduce the element count
            delete pnodeToRemove;
            m_c4CurElements--;

            return kCIDLib::True;
        }

        tCIDLib::TBoolean bFindByKey(const TKey& keyToFind, TElem& objToFill) const
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeRet = pnodeFind(keyToFind, hshElem);

            if (!pnodeRet)
                return kCIDLib::False;

            objToFill = pnodeRet->objData();
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bKeyExists(const TKey& keyToFind) const
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(keyToFind, hshElem);

            return (pnodeCheck != nullptr);
        }

        tCIDLib::TBoolean
        bRemoveKey( const   TKey&               objKeyToRemove
                    , const tCIDLib::TBoolean   bThrowIfNot = kCIDLib::True)
        {
            TLocker lockrSync(this);

            // Get the hash of the element
            const tCIDLib::THashVal hshElem = m_kopsToUse.hshKey
            (
                objKeyToRemove, m_c4HashModulus
            );

            // Search the bucket if its not empty
            TNode* pnodePrev = nullptr;
            TNode* pnodeToRemove = m_apBuckets[hshElem];
            while (pnodeToRemove)
            {
                // If this key matches, then break out
                if (m_kopsToUse.bCompKeys
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

            // So we can now toast the found node and reduce the element count
            delete pnodeToRemove;
            m_c4CurElements--;
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bRemoveKeyIfExists(const TKey& objKeyToRemove)
        {
            // Just call the other, indicating not to throw if not found
            return bRemoveKey(objKeyToRemove, kCIDLib::False);
        }

        tCIDLib::TCard4 c4HashModulus() const
        {
            TLocker lockrSync(this);
            return m_c4HashModulus;
        }


        //
        //  Return a cursor for a specific key. We find the node (or lack thereof)
        //  and set up a cursor using a protected constructor. We call pnodeFind
        //  to do the lookup.
        //
        TCursor cursFindByKey(const TKey& objKeyToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);
            if (!pnodeRet)
                hshKey = m_c4HashModulus;
            return TCursor(this, hshKey, pnodeRet);
        }

        TNCCursor cursFindByKey(const TKey& objKeyToFind)
        {
            TLocker lockrSync(this);

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
        template <typename IterCB> tCIDLib::TBoolean bForEachNC(IterCB iterCB)
        {
            TLocker lockrThis(this);

            tCIDLib::THashVal   hshCurBucket = 0;
            TNode*              pnodeCur = pnodeFindFirst(hshCurBucket);
            while (pnodeCur)
            {
                if (!iterCB(pnodeCur->objData()))
                    return kCIDLib::False;

                // In debug, make sure they didn't modify the hash of this element
                #if CID_DEBUG_ON
                const tCIDLib::THashVal hshCheck = m_kopsToUse.hshKey
                (
                    m_pfnKeyExtract(pnodeCur->objData()), m_c4HashModulus
                );
                if (hshCheck != hshCurBucket)
                    this->HashChanged(CID_FILE, CID_LINE);
                #endif

                pnodeCur = pnodeFindNext(pnodeCur, hshCurBucket);
            }
            return kCIDLib::True;
        }

        template <typename T = TElem> T& objAddMove(T&& objToAdd)
        {
            TLocker lockrSync(this);

            // See if this element is already in the collection
            const TKey& objKey = m_pfnKeyExtract(objToAdd);
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(objKey, hshElem);

            // If so, we cannot allow it
            if (pnodeCheck)
                this->DuplicateKey(objKey, CID_FILE, CID_LINE);

            //
            //  Add it to the appropriate bucket. We just put it at the head
            //  since the order does not matter. We just construct the
            //  node and pass it the current head, which it will make its
            //  next node.
            //
            m_apBuckets[hshElem] = new TNode(tCIDLib::ForceMove(objToAdd), m_apBuckets[hshElem]);

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Bump up the element count
            m_c4CurElements++;

            return m_apBuckets[hshElem]->objData();
        }

        // Add the passed object if new, and return whether it was added or not
        TElem& objAddIfNew( const   TElem&              objToAdd
                            ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeRet = pnodeFind(m_pfnKeyExtract(objToAdd), hshElem);

            if (pnodeRet)
            {
                bAdded = kCIDLib::False;
                return pnodeRet->objData();
            }

            bAdded = kCIDLib::True;
            return objAdd(objToAdd);
        }

        //
        //  Note that we never have to deal with the key changing here. If
        //  if did, we will think it's a new one and add it.
        //
        TElem& objAddOrUpdate(  const   TElem&              objToAdd
                                ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(m_pfnKeyExtract(objToAdd), hshElem);

            if (pnodeCheck)
            {
                pnodeCheck->objData(objToAdd);
                bAdded = kCIDLib::False;
                return pnodeCheck->objData();
            }

            bAdded = kCIDLib::True;
            return objAdd(objToAdd);
        }

        TElem& objFindByKey(const TKey& objKeyToFind)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so throw an exception
            if (!pnodeRet)
                this->KeyNotFound(CID_FILE, CID_LINE);

            // we found it so return the object data
            return pnodeRet->objData();
        }

        const TElem& objFindByKey(const TKey& objKeyToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so throw an exception
            if (!pnodeRet)
                this->KeyNotFound(CID_FILE, CID_LINE);

            // we found it so return the object data
            return pnodeRet->objData();
        }

        TElem& objFindOrAdd(const   TElem&              objToFindOrAdd
                            ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind
            (
                m_pfnKeyExtract(objToFindOrAdd), hshKey
            );

            // Set the caller's added flag
            bAdded = (pnodeRet == nullptr);

            // Not found so add it
            if (bAdded)
            {
                //
                //  Add it to the appropriate bucket. We just put it at the
                //  head since the order does not matter. We just construct
                //  the node and pass it the current head, which it will make
                //  it its next node.
                //
                pnodeRet = new TNode(objToFindOrAdd, m_apBuckets[hshKey]);
                m_apBuckets[hshKey] = pnodeRet;

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // Bump up the element count
                m_c4CurElements++;
            }
            return pnodeRet->objData();
        }

        // Construct an element in place
        template <typename... TArgs> TElem& objPlace(TArgs&&... Args)
        {
            TLocker lockrSync(this);

            //
            //  Because we are forwarding, we have to go ahead and create the node
            //  first so that we have the object to test and see if it's in the list
            //  already. This would only be wasted in an error scenario where we
            //  are going to throw, so not a biggie.
            //
            TJanitor<TNode> janNode
            (
                new TNode(TNode::EForceCall::Val1, tCIDLib::Forward<TArgs>(Args)...)
            );

            // See if this element is already in the collection
            const TKey& objKey = m_pfnKeyExtract(janNode.pobjThis()->objData());
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(objKey, hshElem);

            // If so, we cannot allow it
            if (pnodeCheck)
                this->DuplicateKey(objKey, CID_FILE, CID_LINE);

            //
            //  Add it to the appropriate bucket. We just put it at the head
            //  since the order does not matter. We just construct the
            //  node and pass it the current head, which it will make its
            //  next node.
            //
            janNode.pobjThis()->pnodeNext(m_apBuckets[hshElem]);
            m_apBuckets[hshElem] = janNode.pobjOrphan();

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Bump up the element count
            m_c4CurElements++;

            return m_apBuckets[hshElem]->objData();
        }

        const TElem* pobjFindByKey(const TKey& objKeyToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so return null pointer
            if (!pnodeRet)
                return nullptr;

            // we found it so return the object data
            return &pnodeRet->objData();
        }

        TElem* pobjFindByKey(const TKey& objKeyToFind)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so return null pointer
            if (!pnodeRet)
                return nullptr;

            // we found it so return the object data
            return &pnodeRet->objData();
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TLocker lockrSync(this);

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

            // Move the cursor back to the next node past the target
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

            // So we can now toast the found node and bump the element count
            delete pnodeToRemove;
            m_c4CurElements--;

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Get the cursor back into sync.
            cursAt.c4SerialNum(this->c4SerialNum());
        }

        tCIDLib::TVoid ReplaceValue(const TElem& objNewValue)
        {
            TLocker lockrSync(this);

            // See if this element is in the collection
            tCIDLib::THashVal hshElem;
            TNode* pnodeRep = pnodeFind(m_pfnKeyExtract(objNewValue), hshElem);

            if (!pnodeRep)
                this->KeyNotFound(CID_FILE, CID_LINE);

            // Don't let them change the key
            if (!m_kopsToUse.bCompKeys( m_pfnKeyExtract(pnodeRep->objData())
                                        , m_pfnKeyExtract(objNewValue)))
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcCol_ChangedKey
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }

            // Copy in the new data
            pnodeRep->objData(objNewValue);
        }


        tCIDLib::TVoid Reset(const  tCIDLib::EMTStates  eMTSafe
                            , const tCIDLib::TCard4     c4HashModulus)
        {
            TLocker lockrSync(this);

            // First we have to remove all elements from the collection
            RemoveAll();

            this->SetMTState(eMTSafe);

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
            return nullptr;
        }

        //
        //  Search forward through the buckets for the next node after the one
        //  passed. Null if we hit the end. The hash is updated to the bucket
        //  that the next one is found it otherwise.
        //
        TNode* pnodeFindNext(TKeyedHashSetNode<TElem,TKey>* pnodeLast
                            , tCIDLib::THashVal&            hshToUpdate) const
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
        TNode* pnodeFindPrevious(const TNode* pnodeLast, tCIDLib::THashVal& hshToUpdate) const
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

        // Searches the indicated bucket for the indicated key
        TNode* pnodeFindKey(const   TKey&               objKey
                            , const tCIDLib::THashVal   hshBucket) const
        {
            // Search this bucket for the target node and return its previous
            TNode* pnodeCur = m_apBuckets[hshBucket];
            while (pnodeCur)
            {
                if (m_kopsToUse.bCompKeys(m_pfnKeyExtract(pnodeCur->objData()), objKey))
                    return pnodeCur;
                pnodeCur = pnodeCur->pnodeNext();
            }
            return nullptr;
        }

        // Finds the passed key and returns the node and the key hash
        TNode* pnodeFind(const TKey& objKeyToFind, tCIDLib::THashVal& hshElem) const
        {
            hshElem = m_kopsToUse.hshKey(objKeyToFind, m_c4HashModulus);
            TNode* pnodeCur = m_apBuckets[hshElem];
            while (pnodeCur)
            {
                if (m_kopsToUse.bCompKeys(  m_pfnKeyExtract(pnodeCur->objData())
                                            , objKeyToFind))
                {
                    break;
                }
                pnodeCur = pnodeCur->pnodeNext();
            }
            return pnodeCur;
        }

        // Finds the passed node in the passed bucket. Returns the previous node
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


        tCIDLib::TVoid
        RepNodes(const  TKeyedHashSet<TElem,TKey,TKeyOps>&  colSrc
                , const tCIDLib::TBoolean                   bCheckDups)
        {
            //
            //  Loop through all of the entries in the source collection's
            //  hash table and, for any that have entries, replicate them
            //  to the same place in the destination.
            //
            tCIDLib::TCard4 c4BucketInd;
            for (c4BucketInd = 0; c4BucketInd < colSrc.m_c4HashModulus; c4BucketInd++)
            {
                // If empty, do the next one
                if (!colSrc.m_apBuckets[c4BucketInd])
                    continue;

                TKeyedHashSet<TElem,TKey,TKeyOps>::TNode* pnodeCur = nullptr;
                TKeyedHashSet<TElem,TKey,TKeyOps>::TNode* pnodeSrc = colSrc.m_apBuckets[c4BucketInd];

                //
                //  NOTE We start the last node on the last node of our (target)
                //  bucket, so that we add to the bucket.
                //
                TKeyedHashSet<TElem,TKey,TKeyOps>::TNode* pnodeLast = m_apBuckets[c4BucketInd];
                if (pnodeLast)
                {
                    while (pnodeLast->pnodeNext())
                        pnodeLast = pnodeLast->pnodeNext();
                }

                while (pnodeSrc)
                {
                    //
                    //  If told to watch for dups, then make sure that this
                    //  key isn't already in the list. It could only be in
                    //  this bucket, since they would both hash the same.
                    //
                    const TKey& objKey = m_pfnKeyExtract(pnodeSrc->objData());
                    if (bCheckDups && pnodeFindKey(objKey, c4BucketInd))
                        this->DuplicateKey(objKey, CID_FILE, CID_LINE);

                    // Create a new node with a copy of the src's data
                    pnodeCur = new TNode(pnodeSrc->objData(), nullptr);

                    //
                    //  Set last node's next to this one, if there was a
                    //  last. Else, save this as the head node in the dest.
                    //
                    if (pnodeLast)
                        pnodeLast->pnodeNext(pnodeCur);
                    else
                        m_apBuckets[c4BucketInd] = pnodeCur;

                    // Bump our element count
                    m_c4CurElements++;

                    // Save current as last
                    pnodeLast = pnodeCur;

                    // And move to next source node
                    pnodeSrc = pnodeSrc->pnodeNext();
                }
            }
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_apBuckets
        //      This is the array of hash table buckets. Each list holds the
        //      elements that hash to that index. It is allocated to the
        //      size of the hash modulus.
        //
        //  m_c4CurElements
        //      The current number of elements that the table has.
        //
        //  m_c4HashModulus
        //      The modulus divisor for the hash. This is also the number
        //      elements allocated for the m_alstTable array.
        //
        //  m_kopsToUse
        //      A key ops object that provides all of the operations that
        //      we have to do on key field objects. We own and destruct it
        //      when we destruct.
        //
        //  m_pfnKeyExtract
        //      The key extraction function provided by the user. It pulls
        //      out a reference to the key field from the data field.
        // -------------------------------------------------------------------
        TNode**             m_apBuckets;
        tCIDLib::TCard4     m_c4CurElements;
        tCIDLib::TCard4     m_c4HashModulus;
        TKeyOps             m_kopsToUse;
        TKeyExtract         m_pfnKeyExtract;
};

#pragma CIDLIB_POPPACK


//
//  Unlike the usual way that classes support binary streaming, i.e. through
//  the MStreamable mixin interface, we cannot do that here because that would
//  require that all our element types are streamable. So we provide a global
//  operator for those folks who want to use it. This means that collections
//  cannot be streamed polymorphically via the base classes.
//
template <typename TElem, class TKey, class TKeyOps>
TBinOutStream& operator<<(          TBinOutStream&                      strmOut
                            , const TKeyedHashSet<TElem,TKey,TKeyOps>&  colToStream)
{
    // Don't let it change during this
    TLocker lockrThis(&colToStream);

    //
    //  Stream out a leading stream marker, then the element counts, the
    //  thread safety setting, and our hash modulus. We got the rid of the max
    //  count so just a zero place holder.
    //
    tCIDLib::TCard4 c4Count     = colToStream.c4ElemCount();

    strmOut     <<  tCIDLib::EStreamMarkers::StartObject
                <<  c4Count
                <<  tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard)
                <<  tCIDLib::TCard4(0)
                <<  colToStream.eMTSafe()
                <<  colToStream.c4HashModulus();

    // If there were any elements, then stream them
    TKeyedHashSet<TElem,TKey,TKeyOps>::TCursor cursOut(&colToStream);
    while (cursOut.bIsValid())
    {
        strmOut << cursOut.objRCur();
        ++cursOut;
    }
    strmOut << tCIDLib::EStreamMarkers::EndObject;
    return strmOut;
}


// We cannot lock the collection, since we might delete the mutex!
template <typename TElem, class TKey, class TKeyOps>
TBinInStream& operator>>(TBinInStream&                          strmIn
                        , TKeyedHashSet<TElem,TKey,TKeyOps>&    colToStream)
{
    // Flush the collection first
    colToStream.RemoveAll();

    // Make sure we see the stream marker
    strmIn.CheckForMarker(tCIDLib::EStreamMarkers::StartObject, CID_FILE, CID_LINE);

    // Stream in the state of the collection itself
    tCIDLib::TCard4     c4Count;
    tCIDLib::TCard4     c4XORCount;
    tCIDLib::TCard4     c4HashModulus;
    tCIDLib::TCard4     c4OldMax;
    tCIDLib::EMTStates  eMTSafe;
    strmIn >> c4Count >> c4XORCount >> c4OldMax >> eMTSafe >> c4HashModulus;

    if (c4XORCount != tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard))
        TCollectionBase::BadStoredCount(colToStream.clsIsA());

    // Update it for this new modulus if it's different
    colToStream.Reset(eMTSafe, c4HashModulus);

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
