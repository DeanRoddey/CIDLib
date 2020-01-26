//
// FILE NAME: CIDLib_Vector.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/13/1999
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
//  This is the header for the vector collection template. It implements the
//  TVector class template, which provides by value storage of elements in a
//  standard vector manner (i.e. an array to which you can only append and
//  which grows as required.)
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TVector
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem, typename TIndex = tCIDLib::TCard4>
class TVector : public TCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested class type aliases
        // -------------------------------------------------------------------
        using TMyElemType   = TElem;
        using TMyType       = TVector<TElem, TIndex>;
        using TParType      = TCollection<TElem>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"TVector<TElem,TIndex>");
            return clsRet;
        }


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

                    m_i4CurIndex(0)
                    , m_pcolCursoring(nullptr)
                {
                }

                explicit TConstCursor(const TMyType* const pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_i4CurIndex(0)
                    , m_pcolCursoring(pcolToCursor)
                {
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
                        m_i4CurIndex = cursSrc.m_i4CurIndex;
                        m_pcolCursoring = cursSrc.m_pcolCursoring;
                    }
                    return *this;
                }

                tCIDLib::TBoolean operator==(const TConstCursor& cursSrc) const
                {
                    if (!TParent::operator==(cursSrc))
                        return kCIDLib::False;
                    return (m_i4CurIndex == cursSrc.m_i4CurIndex);
                }

                tCIDLib::TBoolean operator!=(const TConstCursor& cursSrc) const
                {
                    return !TConstCursor::operator==(cursSrc);
                }

                TConstCursor<TElem>& operator++()
                {
                    this->bNext();
                    return *this;
                }

                TConstCursor<TElem> operator++(int)
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

                    // Gotta be between -1 and element count
                    return
                    (
                        (m_i4CurIndex > -1)
                        && (m_i4CurIndex < tCIDLib::TInt4(m_pcolCursoring->c4ElemCount()))
                    );
                }

                tCIDLib::TBoolean bNext() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);

                    // It could be -1 here, in which case we just move up to 0
                    if (m_i4CurIndex < tCIDLib::TInt4(m_pcolCursoring->c4ElemCount()))
                        m_i4CurIndex++;
                    return bIsValid();
                }

                tCIDLib::TBoolean bPrevious() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    if (m_i4CurIndex > -1)
                        m_i4CurIndex--;
                    return bIsValid();
                }

                tCIDLib::TBoolean bReset() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    m_i4CurIndex = 0;
                    return bIsValid();
                }

                tCIDLib::TBoolean bSeekToEnd() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    const tCIDLib::TCard4 c4Count = m_pcolCursoring->c4ElemCount();
                    if (c4Count)
                        m_i4CurIndex = tCIDLib::TInt4(c4Count) - 1;
                    else
                        m_i4CurIndex = 0;

                    return bIsValid();
                }

                const TElem& objRCur() const final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return m_pcolCursoring->objAt(TIndex(m_i4CurIndex));
                }



            protected   :
                // -----------------------------------------------------------
                //  Declare our friends
                // -----------------------------------------------------------
                friend class TMyType;


                // -----------------------------------------------------------
                //  Protected, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TCard4 i4CurIndex() const
                {
                    return m_i4CurIndex;
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_i4CurIndex
                //      This is the current index that we are on in our iteration.
                //      We have to use a signed value because this is a bi-directional
                //      cursor. So we have to have an 'invalid' index going in both
                //      directions. So -1 is bad going backwards.
                //
                //  m_pcolCursoring
                //      This is the vector that we are cursoring. It is provided in
                //      the constructor.
                // -----------------------------------------------------------
                tCIDLib::TInt4  m_i4CurIndex;
                const TMyType*  m_pcolCursoring;


                // -----------------------------------------------------------
                //  Do any needed magic macros
                // -----------------------------------------------------------
                TemplateRTTIDefs
                (
                    TMyType::TConstCursor<TElem>, TBiColCursor<TElem>
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
                    return static_cast<TElem&>
                    (
                        m_pcolNCCursoring->objAt(TIndex(this->i4CurIndex()))
                    );
                }

                TElem* operator->() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return &static_cast<TElem&>
                    (
                        m_pcolNCCursoring->objAt(TIndex(this->i4CurIndex()))
                    );
                }

                TNonConstCursor<TElem>& operator++()
                {
                    this->bNext();
                    return *this;
                }

                TNonConstCursor<TElem> operator++(int)
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
                    return static_cast<TElem&>
                    (
                        m_pcolNCCursoring->objAt(TIndex(this->i4CurIndex()))
                    );
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolNCCursoring
                //      This is the vector that we are cursoring. It is
                //      provided in the constructor. We have to keep our own
                //      non-const pointer, and pass it on to our parent who
                //      handles the non-const stuff.
                // -----------------------------------------------------------
                TMyType* m_pcolNCCursoring;


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
        //  Typedefs for our nested cursor types and a comparator function
        // -------------------------------------------------------------------
        using TCursor = TConstCursor<TElem>;
        using TNCCursor = TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TVector(const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollection<TElem>(eMTSafe)
            , m_apElems(nullptr)
            , m_c4CurAlloc(8)
            , m_c4CurCount(0)
        {
            // Allocate the array of elem pointers and zero them
            m_apElems = new TElem*[m_c4CurAlloc];
            TRawMem::SetMemBuf
            (
                m_apElems, tCIDLib::TCard1(0), sizeof(TElem*) * m_c4CurAlloc
            );
        }

        TVector(const   TIndex              tInitAlloc
                , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollection<TElem>(eMTSafe)
            , m_apElems(nullptr)
            , m_c4CurAlloc(tCIDLib::c4EnumOrd(tInitAlloc))
            , m_c4CurCount(0)
        {
            // Watch for a zero sized initial alloc and use 8
            if (!m_c4CurAlloc)
                m_c4CurAlloc = 8;

            // Allocate the array of elem pointers and zero them
            m_apElems = new TElem*[m_c4CurAlloc];
            TRawMem::SetMemBuf
            (
                m_apElems, tCIDLib::TCard1(0), sizeof(TElem*) * m_c4CurAlloc
            );
        }

        TVector(const   TElem* const        pobjInitVals
                , const TIndex              tCount
                , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollection<TElem>(eMTSafe)
            , m_apElems(nullptr)
            , m_c4CurAlloc(tCIDLib::c4EnumOrd(tCount))
            , m_c4CurCount(tCIDLib::c4EnumOrd(tCount))
        {
            // Can't allow zero, else they are all valid otherwise
            if (!m_c4CurAlloc)
                this->ZeroSize(CID_FILE, CID_LINE);

            // Allocate the array of pointers and make copies of the source elements.
            m_apElems = new TElem*[m_c4CurAlloc];
            try
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
                {
                    CIDLib_Suppress(6386)  // alloc and count are the same size
                    m_apElems[c4Index] = new TElem(pobjInitVals[c4Index]);
                }
            }

            catch(TError& errToCatch)
            {
                delete m_apElems;
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            catch(...)
            {
                delete m_apElems;
                throw;
            }
        }

        TVector(const TMyType& colSrc) :

            TCollection<TElem>(colSrc)
            , m_apElems(nullptr)
            , m_c4CurAlloc(0)
            , m_c4CurCount(0)
        {
            // Lock the other collection while we do this
            TLocker lockrThat(&colSrc);

            // Sanity check the source if debugging
            #if CID_DEBUG_ON
            this->CheckAllocAndCount(colSrc.m_c4CurCount, colSrc.m_c4CurAlloc, CID_FILE, CID_LINE);
            #endif

            //
            //  NOTE: We couldn't do this in the initializer list above since
            //  we have to get the source locked first!
            //
            m_c4CurAlloc    = colSrc.m_c4CurAlloc;
            m_c4CurCount    = colSrc.m_c4CurCount;

            //
            //  Allocate the array of pointers and make copies of the
            //  active source elements.
            //
            m_apElems = new TElem*[m_c4CurAlloc];
            try
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
                {
                    CIDLib_Suppress(6386)
                    m_apElems[c4Index] = new TElem(*colSrc.m_apElems[c4Index]);
                }
            }

            catch(TError& errToCatch)
            {
                delete m_apElems;
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            catch(...)
            {
                delete m_apElems;
                throw;
            }

            // Zero the rest, if any
            if (m_c4CurCount < m_c4CurAlloc)
            {
                TRawMem::SetMemBuf
                (
                    &m_apElems[m_c4CurCount]
                    , tCIDLib::TCard1(0)
                    , sizeof(TElem*) * (m_c4CurAlloc - m_c4CurCount)
                );
            }
        }

        //
        //  Set up a minimal valid setup, then call the move operator. If the
        //  source is thread safe, this one will be as well.
        //
        TVector(TMyType&& colSrc) :

            TCollection<TElem>(colSrc)
            , m_apElems(new TElem*[1])
            , m_c4CurAlloc(1)
            , m_c4CurCount(0)
        {
            *this = tCIDLib::ForceMove(colSrc);
        }

        ~TVector()
        {
            // Clean up the elements, then the array itself
            RemoveAllElems();
            delete [] m_apElems;
            m_apElems = nullptr;
            m_c4CurAlloc = 0;
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

                #if CID_DEBUG_ON
                this->CheckAllocAndCount(colSrc.m_c4CurCount, colSrc.m_c4CurAlloc, CID_FILE, CID_LINE);
                #endif

                TParType::operator=(colSrc);

                // Clean up our current stuff
                RemoveAllElems();
                delete [] m_apElems;
                m_c4CurAlloc = 0;

                //
                //  Allocate the new stuff and copy the source elements, zeroing
                //  out any unused pointers.
                //
                m_c4CurAlloc = colSrc.m_c4CurAlloc;
                m_c4CurCount = colSrc.m_c4CurCount;
                m_apElems = new TElem*[m_c4CurAlloc];

                tCIDLib::TCard4 c4Index;
                for (c4Index = 0; c4Index < m_c4CurCount; c4Index++)
                {
                    CIDLib_Suppress(6386)  // We range checked above
                    m_apElems[c4Index] = new TElem(*colSrc.m_apElems[c4Index]);
                }
                for (; c4Index < m_c4CurAlloc; c4Index++)
                {
                    CIDLib_Suppress(6386)  // We range checked above
                    m_apElems[c4Index] = 0;
                }

                this->c4IncSerialNum();
            }
            return *this;
        }

        TMyType& operator=(TMyType&& colSrc)
        {
            if (&colSrc != this)
            {
                // Lock us both if we are thread safe
                TLocker lockrUs(this);
                TLocker lockrSrc(&colSrc);

                //
                //  Call our parent first. This just really bumps the serial numbers
                //  and some some sanity checks.
                //
                TParType::operator=(tCIDLib::ForceMove(colSrc));

                // And now we just swap our few members
                tCIDLib::Swap(m_c4CurCount, colSrc.m_c4CurCount);
                tCIDLib::Swap(m_c4CurAlloc, colSrc.m_c4CurAlloc);
                tCIDLib::Swap(m_apElems, colSrc.m_apElems);

                // Publish reload events for both
                this->PublishReloaded();
                colSrc.PublishReloaded();
            }
            return *this;
        }

        const TElem& operator[](const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrCol(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return *m_apElems[c4Index];
        }

        TElem& operator[](const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrCol(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return *m_apElems[c4Index];
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const final
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TParType::bIsDescendantOf(clsTarget);
        }

        tCIDLib::TBoolean bIsEmpty() const final
        {
            TLocker lockrCol(this);
            return (m_c4CurCount == 0);
        }

        tCIDLib::TCard4 c4ElemCount() const final
        {
            TLocker lockrCol(this);
            return m_c4CurCount;
        }

        const TClass& clsIsA() const final
        {
            return clsThis();
        }

        const TClass& clsParent() const final
        {
            return TParType::clsThis();
        }

        TElem& objAdd(const TElem& objNew) final
        {
            TLocker lockrCol(this);

            if (m_c4CurCount == m_c4CurAlloc)
                ExpandTo(m_c4CurCount + 1);

            //
            //  Create a new copy of the object and store in the next slot,
            //  bumping the counter after we add it.
            //
            m_apElems[m_c4CurCount++] = new TElem(objNew);

            // Invalidate any cursors and return a ref to the new element
            this->c4IncSerialNum();
            return *m_apElems[m_c4CurCount - 1];
        }

        [[nodiscard]] TCursor* pcursNew() const final
        {
            TLocker lockrCol(this);
            return new TCursor(this);
        }

        tCIDLib::TVoid RemoveAll() final
        {
            TLocker lockrCol(this);
            if (!m_c4CurCount)
                return;

            RemoveAllElems();
            this->c4IncSerialNum();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  If not in the list add it. We assume an unsorted list. If it's sorted
        //  you'd do a lot better yourself doing a binary search and adding if not
        //  found.
        //
        template <typename TComp = tCIDLib::TDefEqComp<typename TMyElemType>>
        tCIDLib::TBoolean bAddIfNew(const TElem& objToAdd, TComp pfnComp = TComp())
        {
            TLocker lockrThis(this);

            // See if this element is in teh list
            tCIDLib::TCard4 c4Index = 0;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                if (pfnComp(*m_apElems[c4Index], objToAdd))
                    return kCIDLib::False;
            }

            objAdd(objToAdd);
            return kCIDLib::True;
        }


        //
        //  We can remove an element by looking for one with that value, possibly
        //  removing all such or just the first one. We assume the list is not
        //  sorted. If it is, you can do better by doing a binary search and
        //  removing it yourself.
        //
        template <typename TComp = tCIDLib::TDefEqComp<typename TMyElemType>>
        tCIDLib::TBoolean
        bRemoveIfMember(const TElem& objToRemove, TComp pfnComp = TComp())
        {
            TLocker lockrThis(this);

            // See if this element is in teh list
            tCIDLib::TCard4 c4Index = 0;
            while (c4Index < m_c4CurCount)
            {
                if (pfnComp(*m_apElems[c4Index], objToRemove))
                    break;
                c4Index++;
            }

            // If not found, return false now
            if (c4Index == m_c4CurCount)
                return kCIDLib::False;

            //
            //  Put a janitor on the element that we are going to delete so
            //  that it will delete on the way out. If it throws at that point
            //  due to an error in the dtor, we don't care because we've got
            //  our stuff straight, and it's now the caller's issue.
            //
            TJanitor<TElem> janRemove(m_apElems[c4Index]);
            m_apElems[c4Index] = 0;

            //
            //  Now bump down the count of elements and compact the list if
            //  needed.
            //
            m_c4CurCount--;
            if (c4Index < m_c4CurCount)
            {
                for (tCIDLib::TCard4 c4CompInd = c4Index;
                                        c4CompInd < m_c4CurCount; c4CompInd++)
                {
                    m_apElems[c4CompInd] = m_apElems[c4CompInd + 1];
                }

                // Zero the last one, since it's not used anymore
                m_apElems[m_c4CurCount] = 0;
            }

            // And invalidate cursors
            this->c4IncSerialNum();
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bRemoveIfMember(TElem* const pobjToRemove)
        {
            TLocker lockrThis(this);

            // See if this element is in teh list
            tCIDLib::TCard4 c4Index = 0;
            while (c4Index < m_c4CurCount)
            {
                if (m_apElems[c4Index] == pobjToRemove)
                    break;
                c4Index++;
            }

            // If not found, return false now
            if (c4Index == m_c4CurCount)
                return kCIDLib::False;

            //
            //  Put a janitor on the element that we are going to delete so
            //  that it will delete on the way out. If it throws at that point
            //  due to an error in the dtor, we don't care because we've got
            //  our stuff straight, and it's now the caller's issue.
            //
            TJanitor<TElem> janRemove(m_apElems[c4Index]);
            m_apElems[c4Index] = 0;

            //
            //  Now bump down the count of elements and compact the list if
            //  needed.
            //
            m_c4CurCount--;
            if (c4Index < m_c4CurCount)
            {
                for (tCIDLib::TCard4 c4CompInd = c4Index;
                                        c4CompInd < m_c4CurCount; c4CompInd++)
                {
                    m_apElems[c4CompInd] = m_apElems[c4CompInd + 1];
                }

                // Zero the last one, since it's not used anymore
                m_apElems[m_c4CurCount] = 0;
            }

            // And invalidate cursors
            this->c4IncSerialNum();
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bRemoveLast()
        {
            TLocker lockrCol(this);

            // If no elements, then we don't do anything
            if (!m_c4CurCount)
                return kCIDLib::False;

            // Bump down the count to get the new last element
            m_c4CurCount--;

            //
            //  Put a janitor on the element we are removing so that it will
            //  be deleted on the way out. If it throws due to an error in
            //  the dtor, that's the caller's problem, not ours. We will have
            //  gotten our state correct by then, so it's not going to be a
            //  problem for us. After the janitor has it, zero the element
            //  slot.
            //
            TJanitor<TElem> janRemove(m_apElems[m_c4CurCount]);
            m_apElems[m_c4CurCount] = 0;

            // Now invalidate cursors and return true
            this->c4IncSerialNum();
            return kCIDLib::True;
        }


        tCIDLib::TVoid BlockMove(const  TIndex              tStartIndex
                                , const TIndex              tEndIndex
                                , const tCIDLib::TBoolean   bUp)
        {
            const tCIDLib::TCard4 c4StartIndex = tCIDLib::TCard4(tStartIndex);
            const tCIDLib::TCard4 c4EndIndex = tCIDLib::TCard4(tEndIndex);

            this->CheckIndex(c4StartIndex, m_c4CurCount, CID_FILE, CID_LINE);
            this->CheckIndex(c4EndIndex, m_c4CurCount, CID_FILE, CID_LINE);

            //
            //  Do some basic checking. If no items, then nothing to do obviously
            //  and the values are clearly bogus.
            //
            if (!m_c4CurCount)
                return;

            // If already at the end in the direction, do nothing
            if (bUp)
            {
                if (!c4StartIndex)
                    return;
            }
             else
            {
                if (c4EndIndex == m_c4CurCount - 1)
                    return;
            }

            //
            //  We can do it. If moving up, then orphan the object from before the
            //  range and put it back at the end. If down, then orphan the one after
            //  the range and put it back at the start.
            //
            tCIDLib::TCard4 c4From = 0;
            tCIDLib::TCard4 c4To = 0;
            if (bUp)
            {
                c4From = c4StartIndex - 1;
                c4To = c4EndIndex;
            }
             else
            {
                c4From = c4EndIndex + 1;
                c4To = c4StartIndex;
            }

            TJanitor<TElem> janFrom(pobjOrphanAt(c4From));
            InsertAt(*janFrom.pobjThis(), c4To);

            // And invalidate any cursors
            this->c4IncSerialNum();
        }


        tCIDLib::TCard4 c4CurAlloc() const
        {
            TLocker lockrCol(this);
            return m_c4CurAlloc;
        }

        tCIDLib::TVoid CheckExpansion(const tCIDLib::TCard4 c4NewElems)
        {
            // If the new elements wouldn't fit, then expand
            TLocker lockrCol(this);
            if (m_c4CurCount + c4NewElems >= m_c4CurAlloc)
                ExpandTo(m_c4CurCount + c4NewElems);
        }


        template <typename IterCB> tCIDLib::TBoolean bForEachI(IterCB iterCB) const
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                if (!iterCB(*m_apElems[c4Index], c4Index))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachNC(IterCB iterCB)
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                if (!iterCB(*m_apElems[c4Index]))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachNCI(IterCB iterCB)
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                if (!iterCB(*m_apElems[c4Index], c4Index))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }


        tCIDLib::TVoid InsertAt(const TElem& objToInsert, const TIndex tAt)
        {
            TLocker lockrThis(this);

            // Expand the array if we need to
            if (m_c4CurCount == m_c4CurAlloc)
                ExpandTo(m_c4CurAlloc + 1);

            // If its equal to the count, then its an append. Else check the index
            const tCIDLib::TCard4 c4At = tCIDLib::TCard4(tAt);
            if (c4At == m_c4CurCount)
            {
                m_apElems[m_c4CurCount++] = new TElem(objToInsert);
                return;
            }
             else
            {
                this->CheckIndex(c4At, m_c4CurCount, CID_FILE, CID_LINE);

                //
                //  Its not at the end, so we have to move everything past the
                //  insert index up to make room for the new item.
                //
                for (tCIDLib::TCard4 c4Index = m_c4CurCount; c4Index > c4At; c4Index--)
                    m_apElems[c4Index] = m_apElems[c4Index - 1];

                // Store the new guy and bump the count
                m_apElems[c4At] = new TElem(objToInsert);
                m_c4CurCount++;
            }

            this->c4IncSerialNum();
        }

        template <typename TCompFunc> tCIDLib::TVoid
        InsertSorted(const  TElem&              objToInsert
                    ,       TCompFunc           pfnComp
                    ,       TIndex&             tAt)
        {
            TLocker lockrThis(this);

            //
            //  If we have no current values, then that's the easy one,
            //  we just add the new item and it goes at 0. If we have
            //  one item, then also do a simple check. Else we do the
            //  binary search.
            //
            if (!m_c4CurCount)
            {
                objAdd(objToInsert);
                tAt = TIndex(0);
                return;
            }
             else if (m_c4CurCount == 1)
            {
                if (pfnComp(objToInsert, *m_apElems[0]) == tCIDLib::ESortComps::FirstLess)
                {
                    tAt = TIndex(0);
                    InsertAt(objToInsert, 0);
                }
                 else
                {
                    tAt = TIndex(1);
                    objAdd(objToInsert);
                }
                return;
            }

            //
            //  More than one, so let's do the binary search. So set up the
            //  two end points that are used to subdivide the list.
            //
            tCIDLib::ESortComps eRes = tCIDLib::ESortComps::Equal;
            tCIDLib::TInt4 i4End = tCIDLib::TInt4(m_c4CurCount) - 1;
            tCIDLib::TInt4 i4Begin = 0;
            tCIDLib::TInt4 i4MidPoint = 0;
            while (i4Begin <= i4End)
            {
                // Divide the current range
                i4MidPoint = (i4Begin + i4End) / 2;

                // Check this guy
                eRes = pfnComp(objToInsert, *m_apElems[i4MidPoint]);
                if (eRes == tCIDLib::ESortComps::Equal)
                    break;

                // Didn't find it, so see which way to go and adjust begin/end
                if (eRes == tCIDLib::ESortComps::FirstLess)
                    i4End = i4MidPoint - 1;
                else
                    i4Begin = i4MidPoint + 1;
            }

            if (i4End < 0)
            {
                // We went all the way past the start, so put it at zero
                tAt = TIndex(0);
            }
             else if (tCIDLib::TCard4(i4Begin) >= m_c4CurCount)
            {
                // We went all the way past the end, so put it at the end
                tAt = TIndex(m_c4CurCount);
            }
             else
            {
                // Else either insert before or after where we break out
                switch(eRes)
                {
                    case tCIDLib::ESortComps::Equal :
                    case tCIDLib::ESortComps::FirstLess :
                        tAt = TIndex(i4MidPoint);
                        break;

                    default :
                        tAt = TIndex(i4MidPoint + 1);
                        break;
                };
            }
            InsertAt(objToInsert, tCIDLib::TCard4(tAt));
        }


        const TElem& objAt(const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrCol(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return *m_apElems[c4Index];
        }

        TElem& objAt(const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrCol(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return *m_apElems[c4Index];
        }


        // Construct an element in place
        template <typename... TArgs> TElem& objPlace(TArgs&&... Args)
        {
            TLocker lockrCol(this);

            if (m_c4CurCount == m_c4CurAlloc)
                ExpandTo(m_c4CurCount + 1);

            m_apElems[m_c4CurCount++] = new TElem(tCIDLib::Forward<TArgs>(Args)...);

            // Invalidate any cursors and return a ref to the new element
            this->c4IncSerialNum();
            return *m_apElems[m_c4CurCount - 1];
        }


        template <typename TCompFunc>
        TElem* pobjBinarySearch(const   TElem&          objToFind
                                ,       TCompFunc       pfnComp
                                ,       TIndex&         tAt)
        {
            // Set up the two end points that are used to subdivide the list
            tCIDLib::TInt4 i4End = tCIDLib::TInt4(m_c4CurCount) - 1;
            tCIDLib::TInt4 i4Begin = 0;

            tCIDLib::ESortComps eRes;
            tCIDLib::TInt4 i4MidPoint = 0;
            while (i4Begin <= i4End)
            {
                // Divide the current range
                i4MidPoint = (i4Begin + i4End) / 2;

                // Check this guy. If this is it, then return it
                eRes = pfnComp(objToFind, *m_apElems[i4MidPoint]);
                if (eRes == tCIDLib::ESortComps::Equal)
                {
                    tAt = TIndex(i4MidPoint);
                    return m_apElems[i4MidPoint];
                }

                // Didn't find it, so see which way to go and adjust begin/end
                if (eRes == tCIDLib::ESortComps::FirstLess)
                    i4End = i4MidPoint - 1;
                else
                    i4Begin = i4MidPoint + 1;
            }

            // We never found it but return where such an element would go
            if (i4End < 0)
            {
                // We went all the way past the start, so put it at zero
                tAt = TIndex(0);
            }
             else if (tCIDLib::TCard4(i4Begin) >= m_c4CurCount)
            {
                // We went all the way past the end, so put it at the end
                tAt = TIndex(m_c4CurCount);
            }
             else
            {
                // Else either insert before or after where we break out
                switch(eRes)
                {
                    case tCIDLib::ESortComps::Equal :
                    case tCIDLib::ESortComps::FirstLess :
                        tAt = TIndex(i4MidPoint);
                        break;

                    default :
                        tAt = TIndex(i4MidPoint + 1);
                        break;
                };
            }
            return nullptr;
        }

        template <typename TCompFunc> const TElem*
        pobjBinarySearch(const  TElem&          objToFind
                        ,       TCompFunc       pfnComp
                        ,       TIndex&         tAt) const
        {
            // Set up the two end points that are used to subdivide the list
            tCIDLib::TInt4 i4End = tCIDLib::TInt4(m_c4CurCount) - 1;
            tCIDLib::TInt4 i4Begin = 0;

            tCIDLib::ESortComps eRes;
            tCIDLib::TInt4 i4MidPoint = 0;
            while (i4Begin <= i4End)
            {
                // Divide the current range
                i4MidPoint = (i4Begin + i4End) / 2;

                // Check this guy. If this is it, then return it
                eRes = pfnComp(objToFind, *m_apElems[i4MidPoint]);
                if (eRes == tCIDLib::ESortComps::Equal)
                {
                    tAt = TIndex(i4MidPoint);
                    return m_apElems[i4MidPoint];
                }

                // Didn't find it, so see which way to go and adjust begin/end
                if (eRes == tCIDLib::ESortComps::FirstLess)
                    i4End = i4MidPoint - 1;
                else
                    i4Begin = i4MidPoint + 1;
            }

            // We never found it but return where such an element would go
            if (i4End < 0)
            {
                // We went all the way past the start, so put it at zero
                tAt = TIndex(0);
            }
             else if (tCIDLib::TCard4(i4Begin) >= m_c4CurCount)
            {
                // We went all the way past the end, so put it at the end
                tAt = TIndex(m_c4CurCount);
            }
             else
            {
                // Else either insert before or after where we break out
                switch(eRes)
                {
                    case tCIDLib::ESortComps::Equal :
                    case tCIDLib::ESortComps::FirstLess :
                        tAt = TIndex(i4MidPoint);
                        break;

                    default :
                        tAt = TIndex(i4MidPoint + 1);
                        break;
                };
            }
            return nullptr;
        }

        template <typename K, typename TCompFunc>
        TElem* pobjKeyedBinarySearch(const  K&          Key
                                    ,       TCompFunc   pfnComp
                                    ,       TIndex&     tAt)
        {
            // Set up the two end points that are used to subdivide the list
            tCIDLib::TInt4 i4End = tCIDLib::TInt4(m_c4CurCount) - 1;
            tCIDLib::TInt4 i4Begin = 0;

            tCIDLib::ESortComps eRes;
            tCIDLib::TInt4 i4MidPoint = 0;
            while (i4Begin <= i4End)
            {
                // Divide the current range
                i4MidPoint = (i4Begin + i4End) / 2;

                // Check this guy. If this is it, then return it
                eRes = pfnComp(Key, *m_apElems[i4MidPoint]);
                if (eRes == tCIDLib::ESortComps::Equal)
                {
                    tAt = TIndex(i4MidPoint);
                    return m_apElems[i4MidPoint];
                }

                // Didn't find it, so see which way to go and adjust begin/end
                if (eRes == tCIDLib::ESortComps::FirstLess)
                    i4End = i4MidPoint - 1;
                else
                    i4Begin = i4MidPoint + 1;
            }

            // We never found it but return where such an element would go
            if (i4End < 0)
            {
                // We went all the way past the start, so put it at zero
                tAt = TIndex(0);
            }
             else if (tCIDLib::TCard4(i4Begin) >= m_c4CurCount)
            {
                // We went all the way past the end, so put it at the end
                tAt = TIndex(m_c4CurCount);
            }
             else
            {
                // Else either insert before or after where we break out
                switch(eRes)
                {
                    case tCIDLib::ESortComps::Equal :
                    case tCIDLib::ESortComps::FirstLess :
                        tAt = TIndex(i4MidPoint);
                        break;

                    default :
                        tAt = TIndex(i4MidPoint + 1);
                        break;
                };
            }
            return nullptr;
        }

        template <typename K, typename TCompFunc> const TElem*
        pobjKeyedBinarySearch(  const K&                Key
                                ,     TCompFunc         pfnComp
                                ,     TIndex&           tAt) const
        {
            // Set up the two end points that are used to subdivide the list
            tCIDLib::TInt4 i4End = tCIDLib::TInt4(m_c4CurCount) - 1;
            tCIDLib::TInt4 i4Begin = 0;

            tCIDLib::ESortComps eRes;
            tCIDLib::TInt4 i4MidPoint = 0;
            while (i4Begin <= i4End)
            {
                // Divide the current range
                i4MidPoint = (i4Begin + i4End) / 2;

                // Check this guy. If this is it, then return it
                eRes = pfnComp(Key, *m_apElems[i4MidPoint]);
                if (eRes == tCIDLib::ESortComps::Equal)
                {
                    tAt = TIndex(i4MidPoint);
                    return m_apElems[i4MidPoint];
                }

                // Didn't find it, so see which way to go and adjust begin/end
                if (eRes == tCIDLib::ESortComps::FirstLess)
                    i4End = i4MidPoint - 1;
                else
                    i4Begin = i4MidPoint + 1;
            }

            // We never found it but return where such an element would go
            if (i4End < 0)
            {
                // We went all the way past the start, so put it at zero
                tAt = TIndex(0);
            }
             else if (tCIDLib::TCard4(i4Begin) >= m_c4CurCount)
            {
                // We went all the way past the end, so put it at the end
                tAt = TIndex(m_c4CurCount);
            }
             else
            {
                // Else either insert before or after where we break out
                switch(eRes)
                {
                    case tCIDLib::ESortComps::Equal :
                    case tCIDLib::ESortComps::FirstLess :
                        tAt = TIndex(i4MidPoint);
                        break;

                    default :
                        tAt = TIndex(i4MidPoint + 1);
                        break;
                };
            }
            return nullptr;
        }


        tCIDLib::TVoid
        Reallocate(const tCIDLib::TCard4 c4NewSize, const tCIDLib::TBoolean bKeepOld)
        {
            // If the new elements wouldn't fit, then expand
            TLocker lockrCol(this);

            // If the new size is the same as the current allocation, then optimize
            if (c4NewSize == m_c4CurAlloc)
            {
                // If not keeping the old stuff, zero them out, else nothing changes
                if (!bKeepOld)
                {
                    TRawMem::SetMemBuf
                    (
                        m_apElems, tCIDLib::TCard1(0), sizeof(TElem*) * m_c4CurAlloc
                    );
                    m_c4CurCount = 0;

                    // Invalidate any cursors
                    this->c4IncSerialNum();
                }
            }
             else
            {
                // Check that we aren't doing anything bad here
                this->CheckReallocParms(c4NewSize, m_c4CurCount, bKeepOld, CID_FILE, CID_LINE);

                // Allocate the new array
                TElem** apNew = new TElem*[c4NewSize];

                //
                //  If not keeping any old ones, force current count to zero, so we won't
                //  copy any and will zero all new ones.
                //
                if (!bKeepOld)
                    m_c4CurCount = 0;

                // Copy over any actually used elements
                if (m_c4CurCount)
                    TRawMem::CopyMemBuf(apNew, m_apElems, m_c4CurCount * sizeof(TElem*));

                // Zero out the remainder of the new slots
                if (c4NewSize > m_c4CurCount)
                {
                    TRawMem::SetMemBuf
                    (
                        &apNew[m_c4CurCount]
                        , tCIDLib::TCard1(0)
                        , sizeof(TElem*) * (c4NewSize - m_c4CurCount)
                    );
                }

                //
                //  Delete the old array and store the new info. Note that this deletes
                //  the elements as well, so we don't have to free them.
                //
                delete [] m_apElems;
                m_apElems = apNew;
                m_c4CurAlloc = c4NewSize;

                // Invalidate any cursors
                this->c4IncSerialNum();
            }
        }

        tCIDLib::TVoid
        Reallocate(const tCIDLib::TCard4 c4NewSize, const TElem& objInitVal)
        {
            //
            //  Call the other one to reallocate, don't keep any old values. That
            //  will leave us with zero current elements.
            //
            Reallocate(c4NewSize, kCIDLib::False);

            // And now add new elements up to new size
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4NewSize; c4Index++)
                objAdd(objInitVal);
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TLocker lockrThis(this);

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            //
            //  Use the index from the cursor. We will leave the cursor on
            //  this same index, which will effectively move it ot the next
            //  element when we delete the current one.
            //
            RemoveAt(TIndex(cursAt.m_i4CurIndex));
            cursAt.c4SerialNum(this->c4SerialNum());
        }

        tCIDLib::TVoid RemoveAt(const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrThis(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);

            // Orphan the indicated object and delete it
            delete pobjOrphanAt(c4Index);

            // And invalidate cursors
            this->c4IncSerialNum();
        }

        //
        //  NOTE: This one cannot be done thread safe because it can change the
        //  thread safe state!
        //
        tCIDLib::TVoid Reset(const  tCIDLib::EMTStates  eMTSafe
                            , const tCIDLib::TCard4     c4Init)
        {
            // Flush all elements
            RemoveAllElems();
            delete [] m_apElems;
            m_c4CurAlloc = 0;

            this->SetMTState(eMTSafe);

            //
            //  Allocate the new stuff and copy the source elements, zeroing
            //  out any unused pointers.
            //
            m_c4CurAlloc = c4Init ? c4Init : 32;
            m_apElems = new TElem*[m_c4CurAlloc];
            TRawMem::SetMemBuf
            (
                m_apElems
                , tCIDLib::TCard1(0)
                , sizeof(TElem*) * m_c4CurAlloc
            );
        }

        // Set all elements to a particular value
        tCIDLib::TVoid SetAll(const TElem& objToSet)
        {
            TLocker lockrCol(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
                *m_apElems[c4Index] = objToSet;
        }

        template <typename TCompFunc> tCIDLib::TVoid Sort(TCompFunc pfnComp)
        {
            TLocker lockrCol(this);

            // If one or less, we are done
            if (m_c4CurCount < 2)
                return;

            //
            //  We create a wrapper comparison function that takes the pointers
            //  that we store, and calls the caller's comparison function with those
            //  dereferenced.
            //
            TArrayOps::TSort<TElem*>
            (
                m_apElems
                , m_c4CurCount
                , [pfnComp](const TElem* pobj1, const TElem* pobj2)
                  {return pfnComp(*pobj1, *pobj2); }
            );
        }

        template <typename TCompFunc>
        tCIDLib::TVoid SortRange(       TCompFunc           pfnComp
                                , const TIndex              iStartInd
                                , const tCIDLib::TCard4     c4SCount)
        {
            // If no count, just return, else lock and do it
            if (!c4SCount)
                return;
            TLocker lockrCol(this);

            // If one or less, we are done
            if (m_c4CurCount < 2)
                return;

            // Check the two indices
            const tCIDLib::TCard4 c4StartInd = tCIDLib::TCard4(iStartInd);
            this->CheckIndex(c4StartInd, m_c4CurCount, CID_FILE, CID_LINE);
            this->CheckIndex(c4StartInd + (c4SCount - 1), m_c4CurCount, CID_FILE, CID_LINE);

            //
            //  We create a wrapper comparison function that takes the pointers
            //  that we store, and calls the caller's comparison function with those
            //  dereferenced.
            //
            TArrayOps::TSortSubFile<TElem*>
            (
                m_apElems
                , c4StartInd
                , c4SCount
                , [pfnComp](const TElem* pobj1, const TElem* pobj2)
                    {return pfnComp(*pobj1, *pobj2); }
            );
        }

        tCIDLib::TVoid StealAllFrom(TMyType& colSrc)
        {
            TLocker lockrThis(this);
            TLocker lockrSrc(&colSrc);

            // Remove all our elements
            RemoveAllElems();

            // Expand our allocation if not enough
            if (colSrc.m_c4CurCount > m_c4CurAlloc)
            {
                m_c4CurAlloc = colSrc.m_c4CurAlloc;
                delete [] m_apElems;
                m_apElems = new TElem*[m_c4CurAlloc];

                // Don't have to zero them since we are going to set them all
            }

            // Take the source's current count and then zero it
            m_c4CurCount = colSrc.m_c4CurCount;
            colSrc.m_c4CurCount = 0;

            // Now steal all of his elements
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                CIDLib_Suppress(6386)  // We range checked above
                m_apElems[c4Index] = colSrc.m_apElems[c4Index];
                colSrc.m_apElems[c4Index] = 0;
            }

            // Invalidate cursors on both
            this->c4IncSerialNum();
            colSrc.c4IncSerialNum();
        }

        tCIDLib::TVoid SwapItems(const  TIndex tFirst
                                , const TIndex tSecond)
        {
            const tCIDLib::TCard4 c4First = tCIDLib::TCard4(tFirst);
            const tCIDLib::TCard4 c4Second = tCIDLib::TCard4(tSecond);
            TLocker lockrThis(this);
            this->CheckIndex(c4First, m_c4CurCount, CID_FILE, CID_LINE);
            this->CheckIndex(c4Second, m_c4CurCount, CID_FILE, CID_LINE);

            TElem* pSave = m_apElems[c4First];
            m_apElems[c4First] = m_apElems[c4Second];
            m_apElems[c4Second] = pSave;

            // And invalidate cursors
            this->c4IncSerialNum();
        }


        //
        //  Find something in the list. Depends on a lambda with capture to avoid having
        //  to pass in the value to look for. Not found value defaults to most likely
        //  scenario of a Card4 value with max card meaning not found.
        //
        template <typename TCompCB>
        TIndex tFind(       TCompCB     compCB
                    , const TIndex      tNotFound = TIndex(kCIDLib::c4MaxCard)
                    , const TIndex      tStartAt = TIndex(0)) const
        {
            const tCIDLib::TCard4 c4StartAt = tCIDLib::TCard4(tStartAt);

            TLocker lockrThis(this);

			//
			//	We can allow the start index to be at the item past the end. We just
			//	don't enter the loop in that case and return not found. Since we
			//	don't have a checking helper for that, we'll do it ourself up front.
			//
			if (c4StartAt == m_c4CurCount)
				return tNotFound;

			// Otherwise, it has to be a valid index
            this->CheckIndex(c4StartAt, m_c4CurCount, CID_FILE, CID_LINE);
            for (tCIDLib::TCard4 c4Ind = c4StartAt; c4Ind < m_c4CurCount; c4Ind++)
            {
                if (compCB(*m_apElems[c4Ind]))
                    return TIndex(c4Ind);
            }
            return tNotFound;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  This guy preserves old content, so if this is all that gets done, no
        //  cursors will actually be invalidated, since our cursors don't directly
        //  access the element array. So we don't need to increment the serial
        //  number here.
        //
        tCIDLib::TVoid ExpandTo(const tCIDLib::TCard4 c4ExpandTo)
        {
            //
            //  If the expand to is < existing, then an error. If it's equal,
            //  then we just do nothing. If the new size would exceed the max,
            //  that's an error also.
            //
            if (this->bCheckNewSize(c4ExpandTo, m_c4CurAlloc, CID_FILE, CID_LINE))
            {
                tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4(c4ExpandTo * 1.5);
                if (c4NewSize < m_c4CurAlloc + 32)
                    c4NewSize = m_c4CurAlloc + 32;

                // Allocate the new array
                TElem** apNew = new TElem*[c4NewSize];

                //
                //  Copy over the old elements to the beginning of the new. We only
                //  need to copy actually used elements, which might be far smaller
                //  than the allocation size.
                //
                TRawMem::CopyMemBuf(apNew, m_apElems, m_c4CurCount * sizeof(TElem*));

                // Zero out the remainder of the new slots
                TRawMem::SetMemBuf
                (
                    &apNew[m_c4CurAlloc]
                    , tCIDLib::TCard1(0)
                    , sizeof(TElem*) * (c4NewSize - m_c4CurCount)
                );

                // Delete the old array and store the new info
                delete [] m_apElems;
                m_apElems = apNew;
                m_c4CurAlloc = c4NewSize;
            }
        }


        //
        //  We remove the indicated index from the list and compact it, returning
        //  the removed item.
        //
        [[nodiscard]] TElem* pobjOrphanAt(const tCIDLib::TCard4 c4At)
        {
            // Remember the element we are going to orphan
            TElem* pobjRet = m_apElems[c4At];
            m_apElems[c4At] = nullptr;

            // And compact the list if not the last one in the list
            m_c4CurCount--;
            if (c4At < m_c4CurCount)
            {
                for (tCIDLib::TCard4 c4CompInd = c4At;
                                        c4CompInd < m_c4CurCount; c4CompInd++)
                {
                    m_apElems[c4CompInd] = m_apElems[c4CompInd + 1];
                }

                // Zero the last one, since it's not used anymore
                m_apElems[m_c4CurCount] = nullptr;
            }
            return pobjRet;
        }

        tCIDLib::TVoid RemoveAllElems()
        {

            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                try
                {
                    delete m_apElems[c4Index];
                }

                catch(TError& errToCatch)
                {
                    // Not much we can do other than log
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);
                }

                catch(...)
                {
                    m_apElems[c4Index] = nullptr;
                }
            }

            //
            //  Zero out all the slots now. For safety's sake, don't do the
            //  current count but the current alloc.
            //
            TRawMem::SetMemBuf(m_apElems, tCIDLib::TCard1(0), sizeof(TElem*) * m_c4CurAlloc);

            // And we now have zero elements
            m_c4CurCount = 0;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_apElems
        //      This is the array of pointers to elements that represents the
        //      vector. Elements beyond m_c4CurCount are not in use. Elements
        //      which have not been allocated yet are zeroed.
        //
        //  m_c4CurAlloc
        //      This is the current size of the array of pointers.
        //
        //  m_c4CurCount
        //      This is the current count of used elements. It starts at zero
        //      and goes up as elements are added.

        // -------------------------------------------------------------------
        TElem**             m_apElems;
        tCIDLib::TCard4     m_c4CurAlloc;
        tCIDLib::TCard4     m_c4CurCount;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TMyType)
};


#pragma CIDLIB_POPPACK



//
//  Unlike the usual way that classes support binary streaming, i.e. through
//  the MStreamable mixin interface, we cannot do that here because that would
//  require that all our element types are streamable. So we provide a global
//  operator for those folks who want to use it. This means that collections
//  cannot be streamed polymorphically via the base classes.
//
template <typename TElem, typename TIndex>
TBinOutStream& operator<<(          TBinOutStream&          strmOut
                            , const TVector<TElem, TIndex>& colToStream)
{
    // Don't let it change during this
    TLocker lockrThis(&colToStream);

    //
    //  Stream out a leading stream marker, then the element count, and the
    //  thread safety setting. We got rid of max elements, so just a zero
    //  placeholder.
    //
    const tCIDLib::TCard4 c4Count = colToStream.c4ElemCount();

    strmOut     <<  tCIDLib::EStreamMarkers::StartObject
                <<  c4Count
                <<  tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard)
                <<  tCIDLib::TCard4(0)
                <<  colToStream.eMTSafe();

    // If there were any elements, then stream them
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmOut << colToStream[TIndex(c4Index)];

    strmOut << tCIDLib::EStreamMarkers::EndObject;
    return strmOut;
}

// We cannot lock collection, because we may delete the mutex!
template <typename TElem, typename TIndex> TBinInStream&
operator>>(TBinInStream& strmIn, TVector<TElem, TIndex>& colToStream)
{
    // Flush the collection first
    colToStream.RemoveAll();

    // Make sure we see the stream marker
    strmIn.CheckForMarker(tCIDLib::EStreamMarkers::StartObject, CID_FILE, CID_LINE);

    // Stream in the state of the collection itself
    tCIDLib::TCard4     c4Count;
    tCIDLib::TCard4     c4XORCount;
    tCIDLib::TCard4     c4OldMaxCount;
    tCIDLib::EMTStates  eMTSafe;
    strmIn >> c4Count >> c4XORCount >> c4OldMaxCount >> eMTSafe;

    if (c4XORCount != tCIDLib::TCard4(c4Count ^ kCIDLib::c4MaxCard))
        TCollectionBase::BadStoredCount(colToStream.clsIsA());

    // Update it to at leats hold the new count of elements
    colToStream.Reset(eMTSafe, c4Count);

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

