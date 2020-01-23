//
// FILE NAME: CIDLib_HashSet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/02/1997
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
//  This is the public header for the THashSet class. This class provides an
//  unordered collection of elements. Unlike TKeyedHashSet, this collection
//  does not extract key fields from elements. Instead, the entire content of
//  the elements are the key. Therefore it has one less template instantiation
//  parameter than TKeyedHashSet, because it does not need to indicate the
//  key field type.
//
//  This class is often used for a hashed set of TString objects
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
struct  THashSetList;
template <typename TElem, class TKeyOps> class THashSet;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: THashSetNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <typename TElem> class THashSetNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THashSetNode(const TElem& objData, THashSetNode<TElem>* pnodeNext) :

            m_objData(objData)
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
        template <typename... TArgs> THashSetNode(const EForceCall, TArgs&&... Args) :

            m_objData(tCIDLib::Forward<TArgs>(Args)...)
            , m_pnodeNext(nullptr)
        {
        }

        THashSetNode(const THashSetNode<TElem>&) = delete;

        ~THashSetNode() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THashSetNode<TElem>& operator=(const THashSetNode<TElem>&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TElem& objData()
        {
            return m_objData;
        }

        const TElem& objData() const
        {
            return m_objData;
        }

        TElem& objData(const TElem& objToSet)
        {
            m_objData = objToSet;
            return m_objData;
        }

        THashSetNode<TElem>* pnodeNext()
        {
            return m_pnodeNext;
        }

        THashSetNode<TElem>* pnodeNext(THashSetNode<TElem>* const pnodeToSet)
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
        //      user's data.
        //
        //  m_pnodeNext
        //      This is a pointer to the next node in the bucket that this
        //      node is part of.
        // -------------------------------------------------------------------
        TElem                   m_objData;
        THashSetNode<TElem>*    m_pnodeNext;
};



// ---------------------------------------------------------------------------
//   CLASS: THashSet
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem, class TKeyOps> class THashSet

    : public TCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested aliases for the node type used by a hash table and the
        //  user provided object equality function.
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = THashSet<TElem, TKeyOps>;
        using TNode = THashSetNode<TElem>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"THashSet<TElem,TKeyOps>");
            return clsRet;
        }


        // -------------------------------------------------------------------
        //  Our nested cursor classes
        // -------------------------------------------------------------------
        template <typename TElem, class TKeyOps> class TConstCursor

            : public TBiColCursor<TElem>
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
                        if (!pclsThis)
                            pclsThis = new TClass(L"THashSet::TConstCursor<TElem,TKeyOps>");
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
                    , m_pnodeCur(nullptr)
                    , m_pcolCursoring(nullptr)
                {
                }

                TConstCursor(const TMyType* pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_hshBadFirst(tCIDLib::THashVal(-1))
                    , m_hshBadLast(pcolToCursor->c4HashModulus())
                    , m_hshCurBucket(0)
                    , m_pnodeCur(nullptr)
                    , m_pcolCursoring(pcolToCursor)
                {
                    m_pnodeCur = m_pcolCursoring->pnodeFindFirst(m_hshCurBucket);
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadLast;
                }

                // We have to lock first, so we can't use member init!
                TConstCursor(const TConstCursor& cursSrc) :

                    m_hshBadFirst(tCIDLib::THashVal(-1))
                {
                    TConstCursor::operator=(cursSrc);
                }

                ~TConstCursor() {}


                // -----------------------------------------------------------
                //  Public Operators
                // -----------------------------------------------------------
                TConstCursor& operator=(const TConstCursor& cursSrc)
                {
                    // Don't have to copy bad first, it's the same for everyone
                    if (this != &cursSrc)
                    {
                        // Lock the collection if it's been set and is thread safe
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


                // -----------------------------------------------------------
                //  Public, inherited methods
                // -----------------------------------------------------------
                tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
                {
                    if (clsTarget == clsThis())
                        return kCIDLib::True;
                    return TParent::bIsDescendantOf(clsTarget);
                }

                tCIDLib::TBoolean bIsValid() const final
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

                tCIDLib::TBoolean bNext() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    m_pnodeCur = m_pcolCursoring->pnodeFindNext(m_pnodeCur, m_hshCurBucket);

                    // If not found, set is invalid in the forward direction
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadLast;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bPrevious() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(bIsValid(), CID_FILE, CID_LINE);
                    m_pnodeCur = m_pcolCursoring->pnodeFindPrevious(m_pnodeCur, m_hshCurBucket);

                    // If not found, set our bucket to bad first
                    if (!m_pnodeCur)
                        m_hshCurBucket = m_hshBadFirst;
                    return (m_pnodeCur != nullptr);
                }

                tCIDLib::TBoolean bReset() final
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

                tCIDLib::TBoolean bSeekToEnd() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    m_pnodeCur = m_pcolCursoring->pnodeFindLast(m_hshCurBucket);

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

                const TElem& objRCur() const final
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
                //      is the hash modulus.
                //
                //  m_hshCurBucket
                //      The index of the bucket within the table that we are
                //      cursoring. The hash table has a bucket per possible hash so we
                //      have to keep up with the one we are working on.
                //
                //      The node pointer isn't used for validity checking. It's just
                //      used to do next/previous. If this guy is valid, then the node
                //      must be valid.
                //
                //  m_pcolCursoring
                //      A pointer to the hash map collection that we are cursoring
                //
                //  m_pnodeCur
                //      The current node in the hash table. If m_hshCurBucket is valid
                //      this must be pointing to a valid current node.
                // -----------------------------------------------------------
                const tCIDLib::THashVal m_hshBadFirst;
                tCIDLib::THashVal       m_hshBadLast;
                tCIDLib::THashVal       m_hshCurBucket;
                const TMyType*          m_pcolCursoring;
                TNode*                  m_pnodeCur;
        };


        template <typename TElem, class TKeyOps> class TNonConstCursor

            : public TConstCursor<TElem, TKeyOps>
        {
            public  :
                // -----------------------------------------------------------
                //  Public types
                // -----------------------------------------------------------
                using TParent = TConstCursor<TElem, TKeyOps>;


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
                            pclsThis = new TClass(L"THashSet::TNonConstCursor<TElem,TKeyOps>");
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

                TNonConstCursor(TMyType* pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_pcolNCCursoring(pcolToCursor)
                {
                }

                // We have to lock first, so we can't use member init!
                TNonConstCursor(const TNonConstCursor& cursSrc)
                {
                    TNonConstCursor::operator=(cursSrc);
                }

                ~TNonConstCursor() {}


                // -----------------------------------------------------------
                //  Public Operators
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
                //  m_pcolCursoring
                //      A pointer to the hash map collection that we are
                //      cursoring. We need our own non-const pointer.
                // -----------------------------------------------------------
                TMyType*  m_pcolNCCursoring;
        };


        // -------------------------------------------------------------------
        //  Aliases our cursor classes
        // -------------------------------------------------------------------
        using TCursor = TConstCursor<TElem, TKeyOps>;
        using TNCCursor = TNonConstCursor<TElem, TKeyOps>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THashSet<TElem,TKeyOps>() = delete;

        THashSet(   const   tCIDLib::TCard4     c4Modulus
                    , const TKeyOps&            kopsToUse
                    , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollection<TElem>(eMTSafe)
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(c4Modulus)
            , m_kopsToUse(kopsToUse)
        {
            // Allocate and initialize the bucket table
            m_apBuckets = new TNode*[c4Modulus];
            TRawMem::SetMemBuf
            (
                m_apBuckets, tCIDLib::TCard1(0), sizeof(tCIDLib::TVoid*) * c4Modulus
            );
        }

        THashSet(const TMyType& colSrc) :

            TCollection<TElem>(colSrc)
            , m_apBuckets(nullptr)
            , m_c4CurElements(0)
            , m_c4HashModulus(colSrc.m_c4HashModulus)
            , m_kopsToUse(colSrc.m_kopsToUse)
        {
            try
            {
                // Allocate and initialize the list table
                m_apBuckets = new TNode*[m_c4HashModulus];
                TRawMem::SetMemBuf
                (
                    m_apBuckets, tCIDLib::TCard1(0), sizeof(tCIDLib::TVoid*) * m_c4HashModulus
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

        THashSet(TMyType&&) = delete;

        ~THashSet()
        {
            // Flush the collection
            RemoveAll();

            // And delete the hash bucket pointer array
            delete [] m_apBuckets;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& colSrc)
        {
            // Check for assignment to self
            if (this == &colSrc)
                return *this;

            TLocker lockrUs(this);
            TLocker lockrSrc(&colSrc);

            // Call our parent first
            TCollection<TElem>::operator=(colSrc);

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

            // And now take his current elements value and copy the key ops
            m_c4CurElements = colSrc.m_c4CurElements;
            m_kopsToUse = colSrc.m_kopsToUse;

            // Invalidate any cursors
            this->c4IncSerialNum();

            return *this;
        }

        // We don't swap key ops, just element content
        TMyType& operator=(TMyType&&) = delete;

        const TElem& operator[](const TString& strToFind) const
        {
            return this->objFind(strToFind);
        }

        TElem& operator[](const TString& strToFind)
        {
            return this->objFind(strToFind);
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

        tCIDLib::TVoid RemoveAll() final
        {
            TLocker lockrSync(this);
            if (!m_c4CurElements)
                return;

            // For each bucket, iterator it and delete its elements.
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4HashModulus; c4Index++)
            {
                if (!m_apBuckets[c4Index])
                    continue;

                TNode* pnodeCur = m_apBuckets[c4Index];
                while (pnodeCur)
                {
                    TNode* pnodeNext = pnodeCur->pnodeNext();
                    delete pnodeCur;
                    pnodeCur = pnodeNext;
                }
                m_apBuckets[c4Index] = 0;
            }

            // And we now have zero elements
             m_c4CurElements = 0;
            this->c4IncSerialNum();
        }

        TElem& objAdd(const TElem& objToAdd) final
        {
            TLocker lockrSync(this);

            // See if this element is already in the collection
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(objToAdd, hshElem);

            // If so, we cannot allow it
            if (pnodeCheck)
                this->DuplicateElem(CID_FILE, CID_LINE);

            //
            //  Add it to the appropriate bucket. We just put it at the head
            //  since the order does not matter. We just construct the
            //  node and pass it the current head, which it will make its
            //  next node.
            //
            m_apBuckets[hshElem] = new TNode(objToAdd, m_apBuckets[hshElem]);

            // Bump up the element count
            m_c4CurElements++;
            this->c4IncSerialNum();

            return m_apBuckets[hshElem]->objData();
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
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(janNode.pobjThis()->objData(), hshElem);

            // If so, we cannot allow it
            if (pnodeCheck)
                this->DuplicateElem(CID_FILE, CID_LINE);

            //
            //  Add it to the appropriate bucket. We just put it at the head
            //  since the order does not matter. We just construct the
            //  node and pass it the current head, which it will make its
            //  next node. We orphan it out of the janiotr now that we know we
            //  are keeping it. We have to set the next node on it after the
            //  fact.
            //
            janNode.pobjThis()->pnodeNext(m_apBuckets[hshElem]);
            m_apBuckets[hshElem] = janNode.pobjOrphan();

            // Bump up the element count
            m_c4CurElements++;
            this->c4IncSerialNum();

            return m_apBuckets[hshElem]->objData();
        }

        [[nodiscard]] TCursor* pcursNew() const final
        {
            TLocker lockrSync(this);
            return new TCursor(this);
        }

        [[nodiscard]] TObject* pobjDuplicate() const final
        {
            return new THashSet<TElem,TKeyOps>(*this);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasElement(const TElem& objToCheck) const
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshElem;
            return (pnodeFind(objToCheck, hshElem) != nullptr);
        }

        tCIDLib::TBoolean bRemove(  const   TElem&              objToRemove
                                    , const tCIDLib::TBoolean   bThrowIfNot = kCIDLib::True)
        {
            TLocker lockrSync(this);

            // Get the hash of the element
            const tCIDLib::THashVal hshElem = m_kopsToUse.hshKey(objToRemove, m_c4HashModulus);

            // Search the bucket if its not empty
            TNode* pnodeFind = m_apBuckets[hshElem];
            while (pnodeFind)
            {
                // If this object matches, then break out
                if (m_kopsToUse.bCompKeys(pnodeFind->objData(), objToRemove))
                    break;
                pnodeFind = pnodeFind->pnodeNext();
            }

            // If not found, return false or throw
            if (!pnodeFind)
            {
                if (bThrowIfNot)
                    this->NodeNotFound(CID_FILE, CID_LINE);
                return kCIDLib::False;
            }

            // We have to find the previous node for the target node.
            TNode* const pnodePrev = pnodeFindPrev(pnodeFind, hshElem);

            //
            //  If the previous node is 0, then this is the head node, else we
            //  have to point the previous node out the find node's next node.
            //
            if (!pnodePrev)
                m_apBuckets[hshElem] = pnodeFind->pnodeNext();
            else
                pnodePrev->pnodeNext(pnodeFind->pnodeNext());

            // Bump the serial number to invalidate cursors
            this->c4IncSerialNum();

            // So we can toast the found node and reduce the element count
            delete pnodeFind;
            m_c4CurElements--;

            return kCIDLib::True;
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
        TCursor cursFind(const TElem& objToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objToFind, hshKey);
            if (!pnodeRet)
                hshKey = m_c4HashModulus;
            return TCursor(this, hshKey, pnodeRet);
        }

        TNCCursor cursFind(const TElem& objToFind)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objToFind, hshKey);
            if (!pnodeRet)
                hshKey = m_c4HashModulus;
            return TNCCursor(this, hshKey, pnodeRet);
        }


        // Add the object if new, and return whether we added or not
        TElem& objAddIfNew( const   TElem&              objToAdd
                            ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(objToAdd, hshElem);

            if (pnodeCheck)
            {
                bAdded = kCIDLib::False;
                return pnodeCheck->objData();
            }

            bAdded = kCIDLib::True;
            return objAdd(objToAdd);
        }

        TElem& objAddOrUpdate(const TElem& objToAdd, tCIDLib::TBoolean& bAdded)
        {
            TLocker lockrSync(this);

            // See if the element exists
            tCIDLib::THashVal hshElem;
            TNode* pnodeCheck = pnodeFind(objToAdd, hshElem);

            // If found, then update it and indcate we didn't add
            if (pnodeCheck)
            {
                bAdded = kCIDLib::False;
                pnodeCheck->objData(objToAdd);
                return pnodeCheck->objData();
            }

            // Indicate that we added, and add this new object
            bAdded = kCIDLib::True;
            return objAdd(objToAdd);
        }

        TElem& objFind(const TElem& objToFind)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objToFind, hshKey);

            // Not found so throw an exception
            if (!pnodeRet)
                this->NodeNotFound(CID_FILE, CID_LINE);

            // we found it so return the object data
            return pnodeRet->objData();
        }

        const TElem& objFind(const TElem& objToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            const TNode* pnodeRet = pnodeFind(objToFind, hshKey);

            // Not found so throw an exception
            if (!pnodeRet)
                this->NodeNotFound(CID_FILE, CID_LINE);

            // we found it so return the object data
            CIDLib_Suppress(6011)  // We null checked above
            return pnodeRet->objData();
        }

        TElem& objFindOrAdd(const   TElem&              objToFindOrAdd
                            ,       tCIDLib::TBoolean&  bAdded)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshElem;
            TNode* pnodeRet = pnodeFind(objToFindOrAdd, hshElem);

            // Set the added parameter
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
                pnodeRet = new TNode(objToFindOrAdd, m_apBuckets[hshElem]);
                m_apBuckets[hshElem] = pnodeRet;

                // Bump the serial number to invalidate cursors
                this->c4IncSerialNum();

                // Bump up the element count
                m_c4CurElements++;
            }
            return pnodeRet->objData();
        }

        TElem* pobjFind(const TElem& objToFind)
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            TNode* pnodeRet = pnodeFind(objToFind, hshKey);
            if (!pnodeRet)
                return nullptr;
            return &pnodeRet->objData();
        }

        const TElem* pobjFind(const TElem& objToFind) const
        {
            TLocker lockrSync(this);

            tCIDLib::THashVal hshKey;
            const TNode* pnodeRet = pnodeFind(objToFind, hshKey);
            if (!pnodeRet)
                return 0;
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
            //  We have to get the bucket for the current element and find the
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

            // So we can now toast the node and decrement the elem count
            delete pnodeToRemove;
            m_c4CurElements--;

            // Bump the serial number to invalidate other cursors
            this->c4IncSerialNum();

            // Get the cursor back into sync.
            cursAt.c4SerialNum(this->c4SerialNum());
        }

        tCIDLib::TVoid Reset(const  tCIDLib::EMTStates  eToSet
                            , const tCIDLib::TCard4     c4HashModulus)
        {
            TLocker lockrSync(this);

            // First we have to remove all elements from the collection
            RemoveAll();

            this->SetMTState(eToSet);

            // Reallocate the buckets if the hash mod changed
            if (c4HashModulus != m_c4HashModulus)
            {
                delete [] m_apBuckets;
                m_apBuckets = new TNode*[m_c4HashModulus];
                m_c4HashModulus = c4HashModulus;
                TRawMem::SetMemBuf
                (
                    m_apBuckets, tCIDLib::TCard1(0), sizeof(tCIDLib::TVoid*) * m_c4HashModulus
                );
            }
        }


    protected  :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TConstCursor<TElem,TKeyOps>;
        friend class TNonConstCursor<TElem,TKeyOps>;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        TNode* pnodeFindFirst(tCIDLib::THashVal& hshToFill) const
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4HashModulus; c4Index++)
            {
                if (m_apBuckets[c4Index])
                {
                    hshToFill = c4Index;
                    return m_apBuckets[c4Index];
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
        TNode* pnodeFindNext( TNode* const        pnodeLast
                            , tCIDLib::THashVal&  hshToUpdate) const
        {
            // Move up to the next node
            TNode* m_pnodeCur = pnodeLast->pnodeNext();
            if (m_pnodeCur)
                return m_pnodeCur;

            // Search subsequent buckets for non-empty one
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
        TNode* pnodeFind(const TElem& objToFind, tCIDLib::THashVal& hshElem) const
        {
            // Get the hash of the element
            hshElem = m_kopsToUse.hshKey(objToFind, m_c4HashModulus);

            // If this bucket is empty, then obviously not here
            TNode* pnodeCur = nullptr;
            if (m_apBuckets[hshElem])
            {
                // The bucket is not empty so check it
                pnodeCur = m_apBuckets[hshElem];
                while (pnodeCur)
                {
                    if (m_kopsToUse.bCompKeys(pnodeCur->objData(), objToFind))
                        break;
                    pnodeCur = pnodeCur->pnodeNext();
                }
            }
            return pnodeCur;
        }


        // Find the previous node in the passed bucket, null if at already at the first
        TNode* pnodeFindPrev( const   TNode* const        pnodeToFind
                              , const tCIDLib::THashVal   hshBucket) const
        {
            // Search this bucket for the target node and return its previous
            TNode* pnodeCur = m_apBuckets[hshBucket];

            // Watch for passed node being the bucket head or it being null
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


        tCIDLib::TVoid RepNodes(const THashSet<TElem,TKeyOps>& colSrc)
        {
            //
            //  Loop through all of the entries in the source collection's
            //  hash table and, for any that have entries, replicate them
            //  to the same place in the destination.
            //
            tCIDLib::TCard4 c4Index;
            for (c4Index = 0; c4Index < colSrc.m_c4HashModulus; c4Index++)
            {
                // If empty, do the next one
                if (!colSrc.m_apBuckets[c4Index])
                    continue;

                THashSet<TElem,TKeyOps>::TNode* pnodeCur = nullptr;
                THashSet<TElem,TKeyOps>::TNode* pnodeLast = nullptr;
                THashSet<TElem,TKeyOps>::TNode* pnodeSrc = colSrc.m_apBuckets[c4Index];
                while (pnodeSrc)
                {
                    // Replicate this bucket's nodes
                    pnodeCur = new TNode(pnodeSrc->objData(), 0);

                    // Set last node's next to this one
                    if (pnodeLast)
                        pnodeLast->pnodeNext(pnodeCur);

                    //
                    //  Set last node's next to this one, if there was a last.
                    //  Else, save this as the head node in the dest.
                    //
                    if (pnodeLast)
                        pnodeLast->pnodeNext(pnodeCur);
                    else
                        m_apBuckets[c4Index] = pnodeCur;

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
        //      This is the array of hash table buckets. Each bucket holds the
        //      elements that hash to that index. It is allocated to the size
        //      of the hash modulus, since it must have a bucket per possible
        //      hash value.
        //
        //  m_c4CurElements
        //      The current number of elements that the table has.
        //
        //  m_c4HashModulus
        //      The modulus divisor for the hash. This is also the number
        //      elements allocated for the m_apBuckets array.
        //
        //  m_kopsToUse
        //      The key ops object to use. It handles hashing and comparing
        //      the 'key' field, which is the whole object in this collection.
        // -------------------------------------------------------------------
        TNode**             m_apBuckets;
        tCIDLib::TCard4     m_c4CurElements;
        tCIDLib::TCard4     m_c4HashModulus;
        TKeyOps             m_kopsToUse;
};

#pragma CIDLIB_POPPACK


//
//  Unlike the usual way that classes support binary streaming, i.e. through
//  the MStreamable mixin interface, we cannot do that here because that would
//  require that all our element types are streamable. So we provide a global
//  operator for those folks who want to use it. This means that collections
//  cannot be streamed polymorphically via the base classes.
//
template <typename TElem, class TKeyOps>
TBinOutStream& operator<<(          TBinOutStream&              strmOut
                            , const THashSet<TElem,TKeyOps>&    colToStream)
{
    // Don't let it change during this
    TLocker lockrThis(&colToStream);

    //
    //  Stream out a leading stream marker, then the element counts, the
    //  thread safety setting, and our hash modulus. We got rid of the max
    //  elements so just a zero place holder.
    //
    tCIDLib::TCard4 c4Count     = colToStream.c4ElemCount();

    strmOut     <<  tCIDLib::EStreamMarkers::StartObject
                <<  c4Count
                <<  tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard)
                <<  tCIDLib::TCard4(0)
                <<  colToStream.eMTSafe()
                <<  colToStream.c4HashModulus();

    // If there were any elements, then stream them
    THashSet<TElem,TKeyOps>::TCursor cursOut(&colToStream);
    for (; cursOut; ++cursOut)
        strmOut << *cursOut;

    strmOut << tCIDLib::EStreamMarkers::EndObject;
    return strmOut;
}


// We cannot lock the collection, since we might destroy the mutex!
template <typename TElem, class TKeyOps>
TBinInStream& operator>>(TBinInStream& strmIn, THashSet<TElem,TKeyOps>& colToStream)
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

