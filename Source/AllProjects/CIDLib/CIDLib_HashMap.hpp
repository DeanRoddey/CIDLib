//
// FILE NAME: CIDLib_HashMap.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/23/2002
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
//  This is the public header for the THashMap class. This class provides
//  an unordered collection of keyed elements. Unlike the hash set collections,
//  in which the key is part of the actual data, this collection stores the
//  data and the key separately.
//
//  Since the key is always explicitly external from the data, we don't need
//  a key extraction function from the user.
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
struct  THashMapList;
template <typename TElem,class TKey,class TKeyOps> class THashMap;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: THashMapNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <typename TKey, class TValue> class THashMapNode
{
    public  :
        // -------------------------------------------------------------------
        //  class types
        // -------------------------------------------------------------------
        using TPair = TKeyObjPair<TKey,TValue>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THashMapNode(const  TPair&                      kobjData
                    ,       THashMapNode<TKey,TValue>*  pnodeNext) :

            m_kobjPair(kobjData)
            , m_pnodeNext(pnodeNext)
        {
        }

        THashMapNode(const  TKey&                       objKey
                    , const TValue&                     objValue
                    ,       THashMapNode<TKey,TValue>*  pnodeNext) :

            m_kobjPair(objKey, objValue)
            , m_pnodeNext(pnodeNext)
        {
        }

        THashMapNode(const THashMapNode<TKey,TValue>&) = delete;

        ~THashMapNode() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THashMapNode<TKey,TValue>& operator=(const THashMapNode<TKey,TValue>&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TPair& objPair() const
        {
            return m_kobjPair;
        }

        TPair& objPair()
        {
            return m_kobjPair;
        }

        TPair& kobjPair(const TPair& kobjToSet)
        {
            m_kobjPair = objToSet;
            return m_kobjPair;
        }

        const TKey& objKey() const
        {
            return m_kobjPair.objKey();
        }

        TKey& objKey()
        {
            return m_kobjPair.objKey();
        }

        const TValue& objValue() const
        {
            return m_kobjPair.objValue();
        }

        TValue& objValue()
        {
            return m_kobjPair.objValue();
        }

        TValue& objValue(const TValue& objToSet)
        {
            m_kobjPair.objValue(objToSet);
            return m_kobjPair.objValue();
        }

        THashMapNode<TKey,TValue>* pnodeNext()
        {
            return m_pnodeNext;
        }

        THashMapNode<TKey,TValue>*
        pnodeNext(THashMapNode<TKey,TValue>* const pnodeToSet)
        {
            m_pnodeNext = pnodeToSet;
            return m_pnodeNext;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kobjPair
        //      This is the data for this node. Its a by value copy of the
        //      user's data.
        //
        //  m_pnodeNext
        //      This is a pointer to the next node in the bucket that this
        //      node is part of.
        // -------------------------------------------------------------------
        TPair                       m_kobjPair;
        THashMapNode<TKey,TValue>*  m_pnodeNext;
};


// ---------------------------------------------------------------------------
//   CLASS: THashMap
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem, class TKey, class TKeyOps> class THashMap

    : public TMapCollection<TElem, TKey>
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"THashMap<TElem,TKey,TKeyOps>");
            return clsRet;
        }


        // -------------------------------------------------------------------
        //  Nested aliases s for the node and key ops types used by a keyed
        //  hash map.
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = THashMap<TElem, TKey, TKeyOps>;
        using TPair = TKeyObjPair<TKey,TElem>;
        using TNode = THashMapNode<TKey,TElem>;


        // -------------------------------------------------------------------
        //  Our nested cursor classes
        // -------------------------------------------------------------------
        template <typename TElem, class TKey, class TKeyOps> class TConstCursor :

            public TBiColCursor<TPair>
        {
            public  :
                // -----------------------------------------------------------
                //  Public types
                // -----------------------------------------------------------
                using TParent = TBiColCursor<TPair>;


                // -----------------------------------------------------------
                //  Public, static methods
                // -----------------------------------------------------------
                static const TClass& clsThis()
                {
                    static const TClass* pclsThis = nullptr;
                    if (!pclsThis)
                    {
                        TBaseLock lockInit;
                        if (!pclsThis)
                            pclsThis = new TClass(L"THashMap::TConstCursor<TElem, TKey, TKeyOps>");
                    }
                    return *pclsThis;
                }

                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TConstCursor() :

                    m_hshCurBucket(0)
                    , m_pcolCursoring(nullptr)
                    , m_pnodeCur(nullptr)
                {
                }

                explicit TConstCursor(const TMyType* pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_hshCurBucket(0)
                    , m_pcolCursoring(pcolToCursor)
                    , m_pnodeCur(nullptr)
                {
                    m_pnodeCur = m_pcolCursoring->pnodeFindFirst(m_hshCurBucket);
                }

                // We have to lock first, so we can't use member init!
                TConstCursor(const TConstCursor& cursSrc)
                {
                    operator=(cursSrc);
                }

                ~TConstCursor() {}


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TConstCursor& operator=(const TConstCursor& cursSrc)
                {
                    if (this != &cursSrc)
                    {
                        TLocker lockrCol(cursSrc.m_pcolCursoring);
                        TParent::operator=(cursSrc);
                        m_hshCurBucket = cursSrc.m_hshCurBucket;
                        m_pcolCursoring = cursSrc.m_pcolCursoring;
                        m_pnodeCur = cursSrc.m_pnodeCur;
                    }
                    return *this;
                }

                tCIDLib::TBoolean operator==(const TConstCursor& cursSrc) const
                {
                    if (!TParent::operator==(cursSrc))
                        return kCIDLib::False;
                    return
                    (
                        (m_hshCurBucket == cursSrc.m_hshCurBucket)
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
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bNext() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = m_pcolCursoring->pnodeFindNext(m_pnodeCur, m_hshCurBucket);
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bPrevious() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (!m_pnodeCur)
                        return kCIDLib::False;
                    m_pnodeCur = m_pcolCursoring->pnodeFindPrev(m_pnodeCur, m_hshCurBucket);
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    m_pnodeCur = m_pcolCursoring->pnodeFindFirst(m_hshCurBucket);
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bSeekToEnd() override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    m_pnodeCur = m_pcolCursoring->pnodeFindLast(m_hshCurBucket);
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
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

                const TMyType::TPair& objRCur() const override
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(m_pnodeCur, CID_FILE, CID_LINE);
                    return m_pnodeCur->objPair();
                }


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::THashVal hshCurHash() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(m_pnodeCur, CID_FILE, CID_LINE);
                    return m_hshCurBucket;
                }

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


            protected   :
                // -----------------------------------------------------------
                //  Declare our friends
                // -----------------------------------------------------------
                friend class TMyType;


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_hshCurBucket
                //      The hash of the bucket within the map that we are
                //      cursoring. The hash table has a bucket per possible
                //      hash value, so we have to keep up with the one we are
                //      working on.
                //
                //  m_pcolCursoring
                //      A pointer to the hash map collection that we are
                //      cursoring
                //
                //  m_pnodeCur
                //      The current node in the hash table, 0 if not on a
                //      valid node at this time.
                // -----------------------------------------------------------
                tCIDLib::THashVal   m_hshCurBucket;
                const TMyType*      m_pcolCursoring;
                TNode*              m_pnodeCur;
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
                        if (!pclsThis)
                            pclsThis = new TClass(L"THashMap::TNonConstCursor<TElem, TKey, TKeyOps>");
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

                explicit  TNonConstCursor(TMyType* pcolToCursor) :

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

                TMyType::TPair& operator*() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return const_cast<TMyType::TPair&>(this->pnodeCur()->objPair());
                }

                TMyType::TPair* operator->() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return &const_cast<TMyType::TPair&>(this->pnodeCur()->objPair());
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
                TMyType::TPair& objWCur() const
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    // Lock the collection and check the serial number
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return const_cast<TMyType::TPair&>(this->pnodeCur()->objPair());
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolCursoring
                //      A pointer to the hash map collection that we are
                //      cursoring. We need our own non-const pointer.
                // -----------------------------------------------------------
                TMyType*    m_pcolNCCursoring;
        };


        // -------------------------------------------------------------------
        //  Aliaes for our nested cursor classes
        // -------------------------------------------------------------------
        using TCursor = TConstCursor<TElem, TKey, TKeyOps>;
        using TNCCursor = TNonConstCursor<TElem, TKey, TKeyOps>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THashMap<TElem,TKey,TKeyOps>() = delete;

        THashMap(   const   tCIDLib::TCard4     c4Modulus
                    , const TKeyOps&            kopsToUse) :

            TMapCollection<TElem, TKey>()
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(c4Modulus)
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

        THashMap(const TMyType& colSrc) :

            TMapCollection<TElem, TKey>(colSrc)
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(colSrc.m_c4HashModulus)
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

                // If there are any source nodes, then replicate them
                if (colSrc.c4ElemCount())
                    RepNodes(colSrc);

                // Copy the destination element count
                m_c4CurElements = colSrc.m_c4CurElements;
            }

            catch(...)
            {
                delete [] m_apBuckets;
                throw;
            }
        }

        ~THashMap()
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
                TMapCollection<TElem,TKey>::operator=(colSrc);

                // Flush our current content before copying
                RemoveAll();

                // If the hash mod is different, re-allocate
                if (m_c4HashModulus != colSrc.m_c4HashModulus)
                {
                    delete [] m_apBuckets;
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
                    RepNodes(colSrc);

                // And now take his current elements value and dup the key ops
                m_c4CurElements = colSrc.m_c4CurElements;
                m_kopsToUse = colSrc.m_kopsToUse;

                // Invalidate any cursors
                this->c4IncSerialNum();
            }
            return *this;
        }

        TMyType& operator=(TMyType&&) = delete;

        const TPair& operator[](const TKey& objKeyToFind) const
        {
            // Just delegate to the other method that does the same thing
            return kobjFindByKey(objKeyToFind);
        }

        TPair& operator[](const TKey& objKeyToFind)
        {
            // Just delegate to the other method that does the same thing
            return kobjFindByKey(objKeyToFind);
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TMapCollection<TElem,TKey>::bIsDescendantOf(clsTarget);
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

        const TClass& clsIsA() const override
        {
            return clsThis();
        }

        const TClass& clsParent() const override
        {
            return TMapCollection<TElem,TKey>::clsThis();
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

        TPair& objAdd(const TPair& kobjToAdd) final
        {
            TLocker lockrSync(this);

            // See if this element is already in the collection
            tCIDLib::THashVal hshKey;
            TNode* pnodeCheck = pnodeFind(kobjToAdd.objKey(), hshKey);

            // If so, we cannot allow it
            if (pnodeCheck)
                this->DuplicateKey(kobjToAdd.objKey(), CID_FILE, CID_LINE);

            // Add this guy at the head of his bucket
            m_apBuckets[hshKey] = new TNode(kobjToAdd, m_apBuckets[hshKey]);
            m_c4CurElements++;

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return m_apBuckets[hshKey]->objPair();
        }

        [[nodiscard]] TCursor* pcursNew() const final
        {
            TLocker lockrSync(this);
            return new TCursor(this);
        }

        [[nodiscard]] TObject* pobjDuplicate() const final
        {
            return new TMyType(*this);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFindByKey(const TKey& keyToFind, TElem& objToFill) const
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(keyToFind, hshKey);

            if (!pnodeRet)
                return kCIDLib::False;

            objToFill = pnodeRet->objValue();
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bKeyExists(const TKey& keyToFind) const
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshKey;
            TNode* pnodeCheck = pnodeFind(keyToFind, hshKey);

            return (pnodeCheck != nullptr);
        }

        tCIDLib::TBoolean bRemoveIfExists(const TKey& objKeyToRemove)
        {
            TLocker lockrSync(this);

            // Get the hash of the element
            const tCIDLib::THashVal hshKey = m_kopsToUse.hshKey
            (
                objKeyToRemove, m_c4HashModulus
            );

            // Search the bucket if its not empty
            TNode* pnodeToRem = m_apBuckets[hshKey];
            while (pnodeToRem)
            {
                // If this key matches, then break out
                if (m_kopsToUse.bCompKeys(pnodeToRem->objKey(), objKeyToRemove))
                    break;
                pnodeToRem = pnodeToRem->pnodeNext();
            }

            if (pnodeToRem)
            {
                // We have to find the previous node for the target node.
                TNode* const pnodePrev = pnodeFindPrev(pnodeToRem, hshKey);

                //
                //  If the previous node is 0, then this is the head node, else we
                //  have to point the previous node out the find node's next node.
                //
                if (!pnodePrev)
                    m_apBuckets[hshKey] = pnodeToRem->pnodeNext();
                else
                    pnodePrev->pnodeNext(pnodeToRem->pnodeNext());

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // So we can now toast the found node and reduce the element count
                delete pnodeToRem;
                pnodeToRem = nullptr;
                m_c4CurElements--;

                // And return that we found it and remove it
                return kCIDLib::True;
            }

            // We never removed it
            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4HashModulus() const
        {
            TLocker lockrSync(this);
            return m_c4HashModulus;
        }


        //
        //  Add object/key separately, which is more efficient than creating
        //  the pair and then copying it in. A number of other methods call
        //  this so it's worth some duplication relative to objAdd.
        //
        TPair& kobjAdd(const TKey& objKey, const TElem& objToAdd)
        {
            TLocker lockrSync(this);

            // See if this element is already in the collection
            tCIDLib::THashVal hshKey;
            TNode* pnodeCheck = pnodeFind(objKey, hshKey);

            // If so, we cannot allow it
            if (pnodeCheck)
                this->DuplicateKey(objKey, CID_FILE, CID_LINE);

            // Add this guy at the head of his bucket
            m_apBuckets[hshKey] = new TNode(objKey, objToAdd, m_apBuckets[hshKey]);
            m_c4CurElements++;

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            return m_apBuckets[hshKey]->objPair();
        }

        TPair& kobjAddIfNew(const   TKey&               objKey
                            , const TElem&              objToAdd
                            ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKey, hshKey);

            if (pnodeRet)
            {
                bAdded = kCIDLib::False;
                return pnodeRet->objPair();
            }

            // Add this guy at the head of his bucket
            m_apBuckets[hshKey] = new TNode(objKey, objToAdd, m_apBuckets[hshKey]);
            m_c4CurElements++;

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            bAdded = kCIDLib::True;
            return m_apBuckets[hshKey]->objPair();
        }

        TPair& kobjAddOrUpdate( const   TKey&               objKey
                                , const TElem&              objToAdd
                                ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshKey;
            TNode* pnodeCheck = pnodeFind(objKey, hshKey);

            if (pnodeCheck)
            {
                pnodeCheck->objValue(objToAdd);
                bAdded = kCIDLib::False;
                return pnodeCheck->objPair();
            }

            // Add this guy at the head of his bucket
            m_apBuckets[hshKey] = new TNode(objKey, objToAdd, m_apBuckets[hshKey]);
            m_c4CurElements++;

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            bAdded = kCIDLib::True;
            return m_apBuckets[hshKey]->objPair();
        }

        TPair& kobjFindByKey(const TKey& objKeyToFind)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so throw an exception
            if (!pnodeRet)
                this->KeyNotFound(CID_FILE, CID_LINE);

            // we found it so return the object value
            return pnodeRet->objPair();
        }

        const TPair& kobjFindByKey(const TKey& objKeyToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so throw an exception
            if (!pnodeRet)
                this->KeyNotFound(CID_FILE, CID_LINE);

            // we found it so return the object value
            return pnodeRet->objPair();
        }

        TPair& kobjFindOrAdd(const  TKey&               objKey
                            , const TElem&              objToFindOrAdd
                            ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKey, hshKey);

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
                pnodeRet = new TNode(objKey, objToFindOrAdd, m_apBuckets[hshKey]);
                m_apBuckets[hshKey] = pnodeRet;
                m_c4CurElements++;

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();
            }
            return pnodeRet->objPair();
        }

        const TPair* pkobjFindByKey(const TKey& objKeyToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so return null pointer
            if (!pnodeRet)
                return nullptr;

            // we found it so return the object data
            return &pnodeRet->objPair();
        }

        TPair* pkobjFindByKey(const TKey& objKeyToFind)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objKeyToFind, hshKey);

            // Not found so return null pointer
            if (!pnodeRet)
                return nullptr;

            // we found it so return the object data
            return &pnodeRet->objPair();
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TLocker lockrSync(this);

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            // Get the node to flush
            TNode* pnodeToRemove = cursAt.pnodeCur();

            //
            //  We have to get the bucket for the current element and find the
            //  previous node for the target node.
            //
            const tCIDLib::THashVal hshKey = cursAt.hshCurHash();
            TNode* const pnodePrev = pnodeFindPrev(pnodeToRemove, hshKey);

            // Move the cursor to the next node past the target
            cursAt.bNext();

            //
            //  If the previous node is 0, then this is the head node,
            //  else we have to point the previous node out the find
            //  node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshKey] = pnodeToRemove->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeToRemove->pnodeNext());

            // So we can now toast the found node and bump the element count
            delete pnodeToRemove;
            m_c4CurElements--;

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // Get our cursor back into sync.
            cursAt.c4SerialNum(this->c4SerialNum());
        }

        tCIDLib::TVoid RemoveKey(const TKey& objKeyToRemove)
        {
            if (!bRemoveIfExists(objKeyToRemove))
                this->KeyNotFound(CID_FILE, CID_LINE);
        }

        tCIDLib::TVoid SetModulus(const tCIDLib::TCard4 c4HashModulus)
        {
            TLocker lockrSync(this);

            // First we have to remove all elements from the collection
            RemoveAll();

            // If the hash modulus changed we have to re-do our buckets
            if (c4HashModulus != m_c4HashModulus)
            {
                delete [] m_apBuckets;
                m_c4HashModulus = c4HashModulus;
                m_apBuckets = new TNode*[m_c4HashModulus];
                TRawMem::SetMemBuf
                (
                    m_apBuckets
                    , tCIDLib::TCard1(0)
                    , sizeof(tCIDLib::TVoid*) * m_c4HashModulus
                );
            }
        }

        tCIDLib::TVoid Update(const TKey& objKey, const TElem& objNewValue)
        {
            TLocker lockrSync(this);

            // See if this element is in the collection
            tCIDLib::THashVal hshKey;
            TNode* pnodeRep = pnodeFind(objKey, hshKey);

            if (!pnodeRep)
                this->KeyNotFound(CID_FILE, CID_LINE);

            // Copy in the new data
            pnodeRep->objValue(objNewValue);
        }


    protected  :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TCursor;
        friend class TNCCursor;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        TNode* pnodeFindFirst(tCIDLib::THashVal& hshToUpdate) const
        {
            tCIDLib::TCard4 c4BucketInd;
            for (c4BucketInd = 0; c4BucketInd < m_c4HashModulus; c4BucketInd++)
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

        #pragma warning(suppress : 26461) // Can't make last node const
        TNode* pnodeFindNext(TNode* const pnodeLast, tCIDLib::THashVal& hshToUpdate) const
        {
            // Move up to the next node
            TNode* pnodeCur = pnodeLast->pnodeNext();
            if (pnodeCur)
                return pnodeCur;

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

        #pragma warning(suppress : 26461) // Can't make last node const
        TNode* pnodeFindPrevious(TNode* const pnodeLast, tCIDLib::THashVal& hshToUpdate) const
        {
            //
            //  Move back to the previous node. If its a legal node, then we
            //  are done.
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
        TNode* pnodeFind(const TKey& objKeyToFind, tCIDLib::THashVal& hshKey) const
        {
            // Get the hash of the element
            hshKey = m_kopsToUse.hshKey(objKeyToFind, m_c4HashModulus);

            // If this bucket is empty, then obviously not here
            if (!m_apBuckets[hshKey])
                return nullptr;

            // The bucket is not empty so check it
            TNode* pnodeCur = m_apBuckets[hshKey];
            while (pnodeCur)
            {
                if (m_kopsToUse.bCompKeys(pnodeCur->objKey(), objKeyToFind))
                    return pnodeCur;
                pnodeCur = pnodeCur->pnodeNext();
            }
            return nullptr;
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

        tCIDLib::TVoid RepNodes(const TMyType& colSrc)
        {
            CIDAssert
            (
                (colSrc.m_c4HashModulus == m_c4HashModulus) && !m_c4CurElements
                , L"RepNodes called under invalid conditions"
            );

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

                TMyType::TNode* pnodeCur = nullptr;
                TMyType::TNode* pnodeLast = nullptr;
                TMyType::TNode* pnodeSrc = colSrc.m_apBuckets[c4BucketInd];
                while (pnodeSrc)
                {
                    // Replicate this bucket's nodes
                    pnodeCur = new TNode(pnodeSrc->objPair(), 0);

                    //
                    //  Set last node's next to this one, if there was a
                    //  last. Else, save this as the head node in the dest.
                    //
                    if (pnodeLast)
                        pnodeLast->pnodeNext(pnodeCur);
                    else
                        m_apBuckets[c4BucketInd] = pnodeCur;

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
        //      we have to do on key field objects.
        // -------------------------------------------------------------------
        TNode**             m_apBuckets;
        tCIDLib::TCard4     m_c4CurElements;
        tCIDLib::TCard4     m_c4HashModulus;
        TKeyOps             m_kopsToUse;
};


// ---------------------------------------------------------------------------
//   CLASS: TSafeHashMap
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem, class TKey, class TKeyOps> class TSafeHashMap

    : public THashMap<TElem, TKey, TKeyOps>
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"TSafeHashMap<TElem,TKey,TKeyOps>");
            return clsRet;
        }


        // -------------------------------------------------------------------
        //  Nested aliases s for the node and key ops types used by a keyed
        //  hash map.
        // -------------------------------------------------------------------
        using TMyType = TSafeHashMap<TElem, TKey, TKeyOps>;
        using TParType = THashMap<TElem, TKey, TKeyOps>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeHashMap<TElem,TKey,TKeyOps>() = delete;

        TSafeHashMap(const  tCIDLib::TCard4     c4Modulus
                    , const TKeyOps&            kopsToUse) :

            TParType(c4Modulus, kopsToUse)
        {
        }

        TSafeHashMap(const TMyType& colSrc) : TParType(colSrc)
        {
        }

        ~TSafeHashMap()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& colSrc)
        {
            return TParType::operator=(colSrc);
        }

        TMyType& operator=(TMyType&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const final
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TParType::bIsDescendantOf(clsTarget);
        }

        tCIDLib::TBoolean bTryLock(const tCIDLib::TCard4 c4WaitMS) const final
        {
            return m_mtxSync.bTryLock(c4WaitMS);
        }

        const TClass& clsIsA() const final
        {
            return clsThis();
        }

        const TClass& clsParent() const final
        {
            return TParType::clsThis();
        }

        tCIDLib::EMTStates eMTSafe() const final
        {
            return tCIDLib::EMTStates::Safe;
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4WaitMSs) const final
        {
            m_mtxSync.Lock(c4WaitMSs);
        }

        tCIDLib::TVoid Unlock() const final
        {
            m_mtxSync.Unlock();
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mtxSync
        //      We override the MLockable interface and implement them in terms
        //      of this guy.
        // -------------------------------------------------------------------
        TMutex  m_mtxSync;
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
TBinOutStream& operator<<(          TBinOutStream&                  strmOut
                            , const THashMap<TElem, TKey, TKeyOps>& colToStream)
{
    // Don't let it change during this
    TLocker lockrThis(&colToStream);

    //
    //  Stream out a leading stream marker, then the element counts, the
    //  thread safety setting, and our hash modulus.
    //
    //  We got rid of max elements, so we just stream a zero for now
    //
    tCIDLib::TCard4 c4Count     = colToStream.c4ElemCount();

    strmOut     <<  tCIDLib::EStreamMarkers::StartObject
                <<  c4Count
                <<  tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard)
                <<  tCIDLib::TCard4(0)
                <<  colToStream.eMTSafe()
                <<  colToStream.c4HashModulus();

    // If there were any elements, then stream them
    THashMap<TElem, TKey, TKeyOps>::TCursor cursOut(&colToStream);
    while (cursOut.bIsValid())
    {
        const THashMap<TElem, TKey, TKeyOps>::TPair& kobjCur = cursOut.objRCur();
        strmOut << kobjCur.objKey() << kobjCur.objValue();
        ++cursOut;
    }
    strmOut << tCIDLib::EStreamMarkers::EndObject;
    return strmOut;
}


// We cannot lock the collection, since we might delete the mutex!
template <typename TElem, class TKey, class TKeyOps>
TBinInStream& operator>>(TBinInStream&                      strmIn
                        , THashMap<TElem, TKey, TKeyOps>& colToStream)
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
    tCIDLib::EMTStates  eMTSafeDummy;
    strmIn >> c4Count >> c4XORCount >> c4OldMax >> eMTSafeDummy >> c4HashModulus;

    if (c4XORCount != tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard))
        TCollectionBase::BadStoredCount(colToStream.clsIsA());

    //
    //  Set these new attributes on the collection, except for the current
    //  count, which will get set implicitly by our loading the stored elements.
    //
    colToStream.SetModulus(c4HashModulus);

    // If there were any elements, then stream them in
    if (c4Count)
    {
        TKey    objKey;
        TElem   objValue;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strmIn >> objKey >> objValue;
            colToStream.objAdd
            (
                THashMap<TElem, TKey, TKeyOps>::TPair(objKey, objValue)
            );
        }
    }

    strmIn.CheckForMarker(tCIDLib::EStreamMarkers::EndObject, CID_FILE, CID_LINE);
    return strmIn;
}



