//
// FILE NAME: CIDLib_RefVector.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/03/2000
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
//  This is the header for the by reference vector collection template. It implements
//  the TRefVector class template, which provides by value storage of elements in a
//  standard vector manner (i.e. an array to which you can only append and which
//  grows as required.)
//
//  Publish/Subscribe
//
//  This collection implements publish/subscribe so you can subscribe to changes. See
//  the base collection class' header comments.
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
//   CLASS: TRefVector
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem, typename TIndex = tCIDLib::TCard4>
class TRefVector : public TRefCollection<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Class type aliases
        // -------------------------------------------------------------------
        using TMyElemType   = TElem;
        using TMyType       = TRefVector<TElem, TIndex>;
        using TParType      = TRefCollection<TElem>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"TRefVector<TElem,TIndex>");
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

                    // Have to be between 0 and count minus 1
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
                        this->m_i4CurIndex++;
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
                    return kCIDLib::True;
                }

                tCIDLib::TBoolean bReset() final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    this->c4SerialNum(m_pcolCursoring->c4SerialNum());
                    m_i4CurIndex = 0;
                    return bIsValid();
                }

                const TElem& objRCur() const final
                {
                    this->CheckInitialized(CID_FILE, CID_LINE);

                    TLocker lockrCol(m_pcolCursoring);
                    this->CheckSerialNum(m_pcolCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return *m_pcolCursoring->pobjAt(TIndex(m_i4CurIndex));
                }


            protected   :
                // -----------------------------------------------------------
                //  Declare our friends
                // -----------------------------------------------------------
                friend class TMyType;


                // -----------------------------------------------------------
                //  Protected, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TInt4 i4CurIndex() const
                {
                    return m_i4CurIndex;
                }


                // -----------------------------------------------------------
                //  Protected data members
                //
                //  m_i4CurIndex
                //      This is the current index that we are on in our iteration.
                //      We have to use a signed value because we need to be able to
                //      have an invalid state at either end of the range, because
                //      we are a bi-directional cursor. So -1 is the invalid end
                //      going backwards.
                // -----------------------------------------------------------
                tCIDLib::TInt4  m_i4CurIndex;


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolCursoring
                //      This is the vector that we are cursoring. It is provided in
                //      the constructor.
                // -----------------------------------------------------------
                const TMyType*    m_pcolCursoring;


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

                explicit  TNonConstCursor(TMyType* const pcolToCursor) :

                    TParent(pcolToCursor)
                    , m_pcolNCCursoring(pcolToCursor)
                {
                }

                // We have to lock first, so we can't use member init!
                TNonConstCursor(const TNonConstCursor<TElem>& cursSrc)
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
                    return *static_cast<TElem*>
                    (
                        m_pcolNCCursoring->pobjAt(TIndex(this->i4CurIndex()))
                    );
                }

                TElem* operator->() const
                {
                    TLocker lockrCol(m_pcolNCCursoring);
                    this->CheckSerialNum(m_pcolNCCursoring->c4SerialNum(), CID_FILE, CID_LINE);
                    this->CheckValid(this->bIsValid(), CID_FILE, CID_LINE);
                    return static_cast<TElem*>
                    (
                        m_pcolNCCursoring->pobjAt(TIndex(this->i4CurIndex()))
                    );
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
                    return *static_cast<TElem*>
                    (
                        m_pcolNCCursoring->pobjAt(TIndex(this->i4CurIndex()))
                    );
                }


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_pcolNCCursoring
                //      We have to keep our own non-const pointer.
                // -----------------------------------------------------------
                TMyType*  m_pcolNCCursoring;


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
        //  Aliases for our nested cursor classes
        // -------------------------------------------------------------------
        using TCursor   = TConstCursor<TElem>;
        using TNCCursor = TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefVector() = delete;

        TRefVector(const tCIDLib::EAdoptOpts eAdopt) :

            TParType()
            , m_apElems(nullptr)
            , m_c4CurAlloc(8)
            , m_c4CurCount(0)
            , m_eAdopt(eAdopt)
        {
            //
            //  Allocate the array of elem pointers. For sanity's sake, we
            //  zero them out, though the current count should indicate which
            //  ones are valid or not.
            //
            m_apElems = new TElem*[m_c4CurAlloc];
            TRawMem::SetMemBuf
            (
                m_apElems, tCIDLib::TCard1(0), sizeof(TElem*) * m_c4CurAlloc
            );
        }

        TRefVector( const   tCIDLib::EAdoptOpts eAdopt
                    , const TIndex              tInitAlloc) :

            TParType()
            , m_apElems(nullptr)
            , m_c4CurAlloc(tCIDLib::c4EnumOrd(tInitAlloc))
            , m_c4CurCount(0)
            , m_eAdopt(eAdopt)
        {
            // Watch for a zero sized initial alloc and use 8
            if (!m_c4CurAlloc)
                m_c4CurAlloc = 8;

            //
            //  Allocate the array of elem pointers. For sanity's sake, we
            //  zero them out, though the current count should indicate which
            //  ones are valid or not.
            //
            m_apElems = new TElem*[m_c4CurAlloc];
            TRawMem::SetMemBuf
            (
                m_apElems, tCIDLib::TCard1(0), sizeof(TElem*) * m_c4CurAlloc
            );
        }

        TRefVector(const TMyType&) = delete;
        TRefVector(TMyType&&) = delete;

        ~TRefVector()
        {
            // If we adopted, then clean up the elements in use
            RemoveAllElems(m_eAdopt);

            // And then clean up the array of element pointers
            delete [] m_apElems;
            m_apElems = nullptr;
            m_c4CurAlloc = 0;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        const TElem* operator[](const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrThis(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return m_apElems[c4Index];
        }

        // USE WITH CARE! This bypasses any reporting of changes via publish!
        TElem* operator[](const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrThis(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return m_apElems[c4Index];
        }

        // We don't allow copy, only move
        TMyType& operator=(const TMyType&) = delete;
        TMyType& operator=(TMyType&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add(TElem* const pobjNew) final
        {
            // Just delegate to a non-virtual which does what we want already
            c4Append(pobjNew);
        }

        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TParType::bIsDescendantOf(clsTarget);
        }

        tCIDLib::TBoolean bIsEmpty() const final
        {
            TLocker lockrThis(this);
            return (m_c4CurCount == 0);
        }

        tCIDLib::TCard4 c4ElemCount() const final
        {
            TLocker lockrThis(this);
            return m_c4CurCount;
        }

        const TClass& clsIsA() const override
        {
            return clsThis();
        }

        const TClass& clsParent() const override
        {
            return TParType::clsThis();
        }

        tCIDLib::EAdoptOpts eAdopt() const final
        {
            return m_eAdopt;
        }

        tCIDLib::TVoid RemoveAll() final
        {
            TLocker lockrThis(this);
            if (!m_c4CurCount)
                return;

            RemoveAllElems(m_eAdopt);
        }

        tCIDLib::TVoid GiveAllTo(TParType& colTarget) final
        {
            // Lock and add all of our items to the target
            TLocker lockrThis(this);
            TLocker lockrTar(&colTarget);

            //
            //  Just in case its set up for pub/sub, use the block mode janitor. We
            //  don't know if it's an ordered collection or if its sorted or something,
            //  so pass max card for a non-contiguous load.
            //
            {
                TColBlockModeJan janBlock(&colTarget, kCIDLib::True, kCIDLib::c4MaxCard);
                for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
                    colTarget.Add(m_apElems[c4Index]);
            }

            //
            //  And now we can remove all of ours, forcing non-adopting mode, since
            //  we've given them to the other guy.
            //
            RemoveAllElems(tCIDLib::EAdoptOpts::NoAdopt);
        }

        tCIDLib::TVoid OrphanElem(TElem* const pnodeToOrphan) final
        {
            // Just lock and call the helper than pulls an element out
            TLocker lockrThis(this);
            bPullOutElem(pnodeToOrphan, kCIDLib::True);
        }

        [[nodiscard]] TCursor* pcursNew() const final
        {
            TLocker lockrThis(this);
            return new TCursor(this);
        }

        tCIDLib::TVoid RemoveElem(TElem* const pnodeToRemove) final
        {
            TLocker lockrThis(this);

            // Pull the element out of the list
            bPullOutElem(pnodeToRemove, kCIDLib::True);

            // If we are adopting, then delete it
            if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete pnodeToRemove;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  We can add if not already in the list literally by pointer or if the
        //  new object's value is not in the list. For the latter, it must support
        //  copy construction since we have to make a copy to add. If we don't add
        //  it, the caller is still responsible for it.
        //
        tCIDLib::TBoolean bAddIfNew(TElem* const pnodeToAdd)
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                // If already in the list, return false
                if (m_apElems[c4Index] == pnodeToAdd)
                    return kCIDLib::False;
            }

            c4Append(pnodeToAdd);
            return kCIDLib::True;
        }

        template <typename TComp = tCIDLib::TDefEqComp<typename TMyElemType>>
        tCIDLib::TBoolean bAddIfNew(const TElem& objToAdd, TComp pfnComp = TComp())
        {
            TLocker lockrThis(this);

            // See if this element is in teh list
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                if (pfnComp(*m_apElems[c4Index], objToAdd))
                    return kCIDLib::False;
            }

            c4Append(new TElem(objToAdd));
            return kCIDLib::True;
        }


        tCIDLib::TBoolean bIsMember(const TElem* const pnodeToCheck)
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                if (m_apElems[c4Index] == pnodeToCheck)
                    return kCIDLib::True;
            }
            return kCIDLib::False;
        }


        //
        //  We can remove an element by pointer comparison or by looking for the
        //  first one that matches a value. We assume the list is not sorted. If
        //  it is, you can do much better by doing a binary search and removing it
        //  yourself by located index.
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

            // If we are adopting, then delete it
            TElem* pobjAt = pobjPullOutElemAt(c4Index);
            if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete pobjAt;

            return kCIDLib::True;
        }

        tCIDLib::TBoolean bRemoveIfMember(TElem* const pnodeToRemove)
        {
            TLocker lockrThis(this);

            // Pull the element out of the list if it's a member
            if (bPullOutElem(pnodeToRemove, kCIDLib::False))
            {
                // If we are adopting, then delete it
                if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
                    delete pnodeToRemove;
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }

        tCIDLib::TBoolean bRemoveLast()
        {
            TLocker lockrThis(this);

            // If no elements, then we don't do anything
            if (!m_c4CurCount)
                return kCIDLib::False;

            // Publish first so that sync subs can see it before we kill it
            this->PublishRemove(m_c4CurCount - 1);

            // Decrement the count
            m_c4CurCount--;

            //
            //  If we own the elements, delete the element. WE do it by putting a
            //  janitor on it, so it will delete after we get our state correct. If it's
            //  dtor throws, that is the caller's problem, not ours.
            //
            TJanitor<TElem> janRemove
            (
                (m_eAdopt == tCIDLib::EAdoptOpts::Adopt) ? m_apElems[m_c4CurCount] : nullptr
            );
            m_apElems[m_c4CurCount] = nullptr;

            // Invalidate cursors
            this->c4IncSerialNum();

            return kCIDLib::True;
        }

        tCIDLib::TCard4 c4Append(TElem* const pobjNew)
        {
            if (!pobjNew)
                this->NullNodeAdded(CID_FILE, CID_LINE);
            TLocker lockrThis(this);

            // Expand the array if we need to (and we can)
            if (m_c4CurCount == m_c4CurAlloc)
                this->ExpandTo(m_c4CurCount + 1);

            // Store this element and bump the count
            const tCIDLib::TCard4 c4Ret = m_c4CurCount;
            m_apElems[m_c4CurCount++] = pobjNew;
            this->c4IncSerialNum();

            this->PublishAdd(c4Ret);
            return c4Ret;
        }

        tCIDLib::TVoid CheckExpansion(const tCIDLib::TCard4 c4NewElems)
        {
            // If the new elements wouldn't fit, then expand
            TLocker lockrCol(this);
            if (m_c4CurCount + c4NewElems >= m_c4CurAlloc)
                ExpandTo(m_c4CurCount + c4NewElems);
        }

        // Removes and destroys all elements, regardless of the adopt setting
        tCIDLib::TVoid DestroyAllElems()
        {
            RemoveAllElems(tCIDLib::EAdoptOpts::Adopt);
        }

        //
        //  Puts all our pointers into the passed collection. This must
        //  be explcitly called instead of doing a copy ctor, in order to
        //  accidental issues with ownership. Either this collection or
        //  the target must be non-adopting.
        //
        tCIDLib::TVoid DupPointers(TParType& colTarget) const
        {
            TLocker lockrThis(this);
            TLocker lockrTar(&colTarget);

            // One of us must be non-adopting
            this->CheckOneNonAdopting(eAdopt(), colTarget.eAdopt(), CID_FILE, CID_LINE);

            //
            //  Just in case its set up for pub/sub, use the block mode janitor. We
            //  don't know if it's an ordered collection or if its sorted or something,
            //  so pass max card for a non-contiguous load. His remove all method will
            //  do the same.
            //
            colTarget.RemoveAll();
            TColBlockModeJan janBlock(&colTarget, kCIDLib::True, kCIDLib::c4MaxCard);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
                colTarget.Add(m_apElems[c4Index]);
        }

        // Enable publish/subscribe on this collection
        tCIDLib::TVoid EnablePubSub(const TString& strDesiredPath, TString& strActualPath)
        {
            // Just pass to our parent
            this->EnablePublish(strDesiredPath, strActualPath);
        }

        tCIDLib::TVoid EnablePubSub(const TString& strTopicPath)
        {
            // Just pass to our parent
            this->EnablePublish(strTopicPath);
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


        // Inserts the item at the indicated index
        tCIDLib::TVoid InsertAt(        TElem* const        pobjToInsert
                                , const TIndex              tAt
                                , const tCIDLib::TBoolean   bNoPublish = kCIDLib::False)
        {
            if (!pobjToInsert)
                this->NullNodeAdded(CID_FILE, CID_LINE);
            TLocker lockrThis(this);

            // If its equal to the count, then its an append. Else check the index
            const tCIDLib::TCard4 c4At = tCIDLib::TCard4(tAt);
            if (c4At == m_c4CurCount)
            {
                c4Append(pobjToInsert);
                return;
            }
            this->CheckIndex(c4At, m_c4CurCount, CID_FILE, CID_LINE);

            // Expand the array if we need to
            if (m_c4CurCount == m_c4CurAlloc)
                ExpandTo(m_c4CurAlloc + 1);

            //
            //  Its not at the end, so we have to move everything past the
            //  insert index up to make room for the new item.
            //
            for (tCIDLib::TCard4 c4Index = m_c4CurCount; c4Index > c4At; c4Index--)
                m_apElems[c4Index] = m_apElems[c4Index - 1];

            // Store the new guy and bump the count
            m_apElems[c4At] = pobjToInsert;
            m_c4CurCount++;

            // They can suppress publishing if it's part of a larger operation
            if (!bNoPublish)
                this->PublishAdd(c4At);
            this->c4IncSerialNum();
        }

        template <typename TCompFunc>
        tCIDLib::TVoid
        InsertSorted(       TElem* const        pobjToInsert
                    ,       TCompFunc           pfnComp
                    ,       TIndex&             tAt)
        {
            if (!pobjToInsert)
            {
                this->NullNodeAdded(CID_FILE, CID_LINE);

                // Won't happen but makes analyzer happy
                return;
            }

            TLocker lockrThis(this);

            //
            //  If we have no current values, then that's the easy one,
            //  we just add the new item and it goes at 0. If we have
            //  one item, then also do a simple check. Else we do the
            //  binary search.
            //
            tCIDLib::ESortComps eRes = tCIDLib::ESortComps::Equal;
            if (!m_c4CurCount)
            {
                Add(pobjToInsert);
                tAt = TIndex(0);
                return;
            }
             else if (m_c4CurCount == 1)
            {
                eRes = pfnComp(*pobjToInsert, *m_apElems[0]);
                if (eRes == tCIDLib::ESortComps::FirstLess)
                {
                    tAt = TIndex(0);
                    InsertAt(pobjToInsert, 0);
                }
                 else
                {
                    tAt = TIndex(1);
                    Add(pobjToInsert);
                }
                return;
            }

            //
            //  More than one, so let's do the binary search. So set up the
            //  two end points that are used to subdivide the list.
            //
            tCIDLib::TInt4 i4End = tCIDLib::TInt4(m_c4CurCount) - 1;
            tCIDLib::TInt4 i4Begin = 0;
            tCIDLib::TInt4 i4MidPoint = 0;
            while (i4Begin <= i4End)
            {
                // Divide the current range
                i4MidPoint = (i4Begin + i4End) / 2;

                eRes = pfnComp(*pobjToInsert, *m_apElems[i4MidPoint]);
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
                switch(eRes)
                {
                    // Else either insert before or after where we break out
                    case tCIDLib::ESortComps::Equal :
                    case tCIDLib::ESortComps::FirstLess :
                        tAt = TIndex(i4MidPoint);
                        break;

                    default :
                        tAt = TIndex(i4MidPoint + 1);
                        break;
                };
            }
            InsertAt(pobjToInsert, tAt);

        }

        // Move an item from one index to another
        tCIDLib::TVoid MoveItem(const   TIndex      tFrom
                                , const TIndex      tTo)
        {
            // Do nothing if the same index for both
            const tCIDLib::TCard4 c4From = tCIDLib::TCard4(tFrom);
            const tCIDLib::TCard4 c4To = tCIDLib::TCard4(tTo);
            if (c4From == c4To)
                return;

            TLocker lockrThis(this);
            this->CheckIndex(c4From, m_c4CurCount, CID_FILE, CID_LINE);

            //
            //  DO THE FROM index check above before going into this
            //  block! See comments below.
            //
            //  Handle the special case of moving it to the end. We have to
            //  let them pass in the current count in the to in order to allow
            //  them to move something to the end. It would complicate the
            //  stuff below, so we special case it.
            //
            if (c4To == m_c4CurCount)
            {
                //
                //  Only bother doing this if they aren't trying to move the
                //  already last one to the end, which wouldn't have been
                //  caught above since we allow the to to be past the end. We
                //  have to have at least one or the from index check above
                //  would have failed, so the current count minus one cannot
                //  wrap around.
                //
                if (c4From != m_c4CurCount - 1)
                {
                    //
                    //  Remove it from the from position and then put it in the
                    //  last position (now emptied by pulling out the lower
                    //  indexed from position) and bump the count back up.
                    //
                    //  Supress publishing from the pull out, and we will do a
                    //  move below.
                    //
                    TElem* pobjTmp = pobjPullOutElemAt(c4From, kCIDLib::True);
                    const tCIDLib::TCard4 c4At = m_c4CurCount;
                    m_apElems[c4At] = pobjTmp;
                    m_c4CurCount++;

                    this->PublishMoved(c4From, m_c4CurCount);
                }
                return;
            }

            // Not the special case, so the to has to be a normal valid index
            this->CheckIndex(c4To, m_c4CurCount, CID_FILE, CID_LINE);

            //
            //  If the from and two are adjacent, we can jut do a swap and
            //  avoid compacting and expanding the list.
            //
            if ((c4From == (c4To + 1)) || (c4To == (c4From + 1)))
            {
                TElem* pSave = m_apElems[c4From];
                m_apElems[c4From] = m_apElems[c4To];
                m_apElems[c4To] = pSave;

                // And invalidate cursors
                this->c4IncSerialNum();
                this->PublishSwap(c4From, c4To);
            }
             else
            {
                //
                //  Oh well, have to orphan and insert. The insert operation
                //  will bump the serial number for us. We suppress publishing in
                //  the intermediate steps and do a single publish moved.
                //
                //  If the from index is before the to, then we have to drop
                //  the to index to get to the right place, because we've
                //  changed the indices by removing it first.
                //
                TElem* pobjTmp = pobjPullOutElemAt(c4From, kCIDLib::True);
                if (c4From < c4To)
                    this->InsertAt(pobjTmp, TIndex(c4To - 1), kCIDLib::True);
                else
                    this->InsertAt(pobjTmp, TIndex(c4To), kCIDLib::True);

                this->PublishMoved(c4From, c4To);
            }
        }

        tCIDLib::TVoid OrphanElemAt(const tCIDLib::TCard4 c4At)
        {
            //
            //  Just lock and call the helper than pulls an element out at a particular
            //  index. Cast to avoid to prevent a nodiscard warning.
            //
            TLocker lockrThis(this);
            static_cast<void>(pobjPullOutElemAt(c4At));
        }

        [[nodiscard]] TMyType* pcolMakeNewOf() const
        {
            // Make a copy with same characteristics, but not content!
            TLocker lockrThis(this);
            return new TMyType(m_eAdopt, m_c4CurCount);
        }

        const TElem* pobjAt(const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrThis(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return m_apElems[c4Index];
        }

        TElem* pobjAt(const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            TLocker lockrThis(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);
            return m_apElems[c4Index];
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
                // Else either before or after where we break out
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

        template <typename TCompFunc>
        const TElem* pobjBinarySearch(  const   TElem&              objToFind
                                        ,       TCompFunc           pfnComp
                                        ,       TIndex&             tAt) const
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

            tCIDLib::ESortComps eRes = tCIDLib::ESortComps::Equal;
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
        pobjKeyedBinarySearch(  const   K&              Key
                                ,       TCompFunc       pfnComp
                                ,       TIndex&         tAt) const
        {
            // Set up the two end points that are used to subdivide the list
            tCIDLib::TInt4 i4End = tCIDLib::TInt4(m_c4CurCount) - 1;
            tCIDLib::TInt4 i4Begin = 0;

            tCIDLib::ESortComps eRes = tCIDLib::ESortComps::Equal;
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

        const TElem* pobjLast() const
        {
            TLocker lockrThis(this);

            // See if there are any nodes. If not, throw an exception
            if (!m_c4CurCount)
                this->ColIsEmpty(CID_FILE, CID_LINE);
            return m_apElems[m_c4CurCount - 1];
        }

        TElem* pobjLast()
        {
            TLocker lockrThis(this);

            // See if there are any nodes. If not, throw an exception
            if (!m_c4CurCount)
                this->ColIsEmpty(CID_FILE, CID_LINE);
            return m_apElems[m_c4CurCount - 1];
        }

        [[nodiscard]] TElem* pobjOrphanAt(const TIndex tAt)
        {
            TLocker lockrThis(this);
            return pobjPullOutElemAt(tCIDLib::TCard4(tAt));
        }

        [[nodiscard]] TElem* pobjOrphanAt(TNCCursor& cursAt)
        {
            TLocker lockrThis(this);

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            //
            //  Use the index from the cursor. We will leave the cursor on
            //  this same index, which will effectively move it to the next
            //  element when we delete the current one.
            //
            TElem* pobjRet = pobjPullOutElemAt(TIndex(cursAt.m_i4CurIndex));
            cursAt.c4SerialNum(this->c4SerialNum());
            return pobjRet;
        }

        [[nodiscard]] TElem* pobjOrphanLast()
        {
            TLocker lockrThis(this);

            // If no elements, then we don't do anything
            if (!m_c4CurCount)
                return nullptr;

            // Publish first so sync subs can see it before we kill it
            this->PublishRemove(m_c4CurCount - 1);

            //
            //  Else bump the count down and return the element at that
            //  index, clearing it out of our list
            //
            m_c4CurCount--;
            TElem* pobjRet = m_apElems[m_c4CurCount];
            m_apElems[m_c4CurCount] = nullptr;
            this->c4IncSerialNum();

            return pobjRet;
        }

        tCIDLib::TVoid RemoveAt(TCursor& cursAt)
        {
            TLocker lockrThis(this);

            // Make sure the cursor is valid and belongs to this collection
            this->CheckCursorValid(cursAt, CID_FILE, CID_LINE);
            if (!cursAt.bIsCursoring(*this))
                this->NotMyCursor(cursAt.clsIsA(), clsIsA(), CID_FILE, CID_LINE);

            //
            //  Use the index from the cursor. We will leave the cursor on this
            //  same index, which will effectively move it to the next element
            //  when we delete the current one. That may invalidate it, it may not.
            //
            RemoveAt(TIndex(cursAt.m_i4CurIndex));
            cursAt.c4SerialNum(this->c4SerialNum());
        }

        tCIDLib::TVoid RemoveAt(const tCIDLib::TCard4 c4Index)
        {
            TLocker lockrThis(this);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);

            //
            //  Publish FIRST in this case, so that any sync subscribers can get info
            //  from this one before it gets whacked.
            //
            this->PublishRemove(c4Index);

            //
            //  Now bump down the count. If we were on the last element, then
            //  that's all we need to do. Else we have to compact the list.
            //
            m_c4CurCount--;

            //
            //  If we are adopting, then clean up this element. So put a janitor
            //  on it, so that it will destruct after we have done all of our
            //  work and are consistent again. If it throws from it's dtor, that
            //  is the caller's problem, not ours.
            //
            TJanitor<TElem> janRemote
            (
                (m_eAdopt == tCIDLib::EAdoptOpts::Adopt) ? m_apElems[c4Index] : nullptr
            );
            m_apElems[c4Index] = nullptr;

            // If that wasn't the last element, then compact
            if (c4Index < m_c4CurCount)
            {
                for (tCIDLib::TCard4 c4CompInd = c4Index;
                                        c4CompInd < m_c4CurCount; c4CompInd++)
                {
                    m_apElems[c4CompInd] = m_apElems[c4CompInd + 1];
                }

                // Zero that last one out
                m_apElems[m_c4CurCount] = nullptr;
            }

            // And invalidate cursors
            this->c4IncSerialNum();
        }

        tCIDLib::TVoid SetAt(TElem* const pobjNew, const TIndex tIndex)
        {
            TLocker lockrThis(this);
            if (!pobjNew)
                this->NullNodeAdded(CID_FILE, CID_LINE);

            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            this->CheckIndex(c4Index, m_c4CurCount, CID_FILE, CID_LINE);

            // If its the same element, then do nothing
            if (m_apElems[c4Index] == pobjNew)
                return;

            //
            //  If adopting, delete the guy we are overwritting. Do it by
            //  putting a janitor on it, so that it happens after we get our
            //  state adjusted.
            //
            TJanitor<TElem> janDelete
            (
                (m_eAdopt == tCIDLib::EAdoptOpts::Adopt) ? m_apElems[c4Index] : nullptr
            );
            m_apElems[c4Index] = pobjNew;

            // And invalidate cursors
            this->c4IncSerialNum();

            this->PublishElemChanged(c4Index);
        }

        template <typename TCompFunc> tCIDLib::TVoid Sort(TCompFunc pfnComp)
        {
            TLocker lockrThis(this);

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

            this->c4IncSerialNum();
            this->PublishReorder();
        }

        template <typename TCompFunc>
        tCIDLib::TVoid SortRange(       TCompFunc           pfnComp
                                , const TIndex              tStartInd
                                , const tCIDLib::TCard4     c4SCount)
        {
            // If no count, just return, else lock and do it
            if (!c4SCount)
                return;
            TLocker lockrCol(this);

            // If one or less, we are done
            if (m_c4CurCount < 2)
                return;

            const tCIDLib::TCard4 c4StartInd(tStartInd);

            // Check the two indices
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
                  { return pfnComp(*pobj1, *pobj2); }
            );

            this->c4IncSerialNum();
            this->PublishReorder();
        }

        tCIDLib::TVoid SwapItems(const  TIndex  tFirst
                                , const TIndex  tSecond)
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
            this->PublishSwap(c4First, c4Second);
        }


        //
        //  Find something in the list. Depends on a lambda with capture to avoid having
        //  to pass in the value to look for. Not found value defaults to mo likely
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
        tCIDLib::TBoolean
        bPullOutElem(const TElem* pobjToPull, const tCIDLib::TBoolean bThrowIfNot)
        {
            // Look for this element in our list
            tCIDLib::TCard4 c4Index;
            for (c4Index = 0; c4Index < m_c4CurCount; c4Index++)
            {
                if (m_apElems[c4Index] == pobjToPull)
                    break;
            }
            if (c4Index == m_c4CurCount)
            {
                if (bThrowIfNot)
                    this->NotMemberNode(CID_FILE, CID_LINE);
                return kCIDLib::False;
            }

            // We have to supress the no dicard error here by cast to void
            static_cast<void>(pobjPullOutElemAt(c4Index));
            return kCIDLib::True;
        }

        tCIDLib::TVoid ExpandTo(const tCIDLib::TCard4 c4ExpandTo)
        {
            //
            //  If the expand to is < existing, then an error. If it's equal,
            //  then we just do nothing.
            //
            if (this->bCheckNewSize(c4ExpandTo, m_c4CurAlloc, CID_FILE, CID_LINE))
            {
                tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4(c4ExpandTo * 1.5);
                if (c4NewSize < m_c4CurAlloc + 32)
                    c4NewSize = m_c4CurAlloc + 32;

                // Allocate the new array and copy over the old
                TElem** apNew = new TElem*[c4NewSize];
                TRawMem::CopyMemBuf(apNew, m_apElems, m_c4CurAlloc * sizeof(TElem*));

                // Delete the old array and store the new info
                delete [] m_apElems;
                m_apElems = apNew;
                m_c4CurAlloc = c4NewSize;
            }
        }

        [[nodiscard]] TElem*
        pobjPullOutElemAt(  const   tCIDLib::TCard4     c4At
                            , const tCIDLib::TBoolean   bNoPublish = kCIDLib::False)
        {
            // Check for bounds error
            this->CheckIndex(c4At, m_c4CurCount, CID_FILE, CID_LINE);

            // Remember this guy for return
            TElem* pobjRet = m_apElems[c4At];

            //
            //  Sometimes this is done in the context of a higher level operation that
            //  is published for the overall operation, so they want to suppress this.
            //
            //  Do it before we kill the element so sync subs can see it before we do.
            //
            if (!bNoPublish)
                this->PublishRemove(c4At);

            //
            //  Index looks ok so bump down the count. If we were on the last element,
            //  then that's all we need to do. Else we have to compact the list.
            //
            m_c4CurCount--;

            if (c4At < m_c4CurCount)
            {
                for (tCIDLib::TCard4 c4CompInd = c4At;
                                        c4CompInd < m_c4CurCount; c4CompInd++)
                {
                    m_apElems[c4CompInd] = m_apElems[c4CompInd + 1];
                }

                // Zero that last one out for sanity's sake
                m_apElems[m_c4CurCount] = nullptr;
            }

            // And invalidate cursors
            this->c4IncSerialNum();

            return pobjRet;
        }


        tCIDLib::TVoid RemoveAllElems(const tCIDLib::EAdoptOpts eAdopt)
        {
            // If no elements, then do nothing
            if (!m_c4CurCount)
                return;

            if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurCount; c4Index++)
                {
                    try
                    {
                        delete m_apElems[c4Index];
                    }

                    catch(TError& errToCatch)
                    {
                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        TModule::LogEventObj(errToCatch);
                    }

                    catch(...)
                    {
                    }
                }
            }

            // For sanity's sake, zero out the old pointers
            TRawMem::SetMemBuf
            (
                m_apElems, tCIDLib::TCard1(0), sizeof(TElem*) * m_c4CurAlloc
            );

            // And we have no elements now
            m_c4CurCount = 0;
            this->c4IncSerialNum();

            this->PublishClear();
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
        //
        //  m_eAdopt
        //      This is set during construction and indicates if we own our
        //      elements or just reference them.
        // -------------------------------------------------------------------
        TElem**             m_apElems;
        tCIDLib::TCard4     m_c4CurAlloc;
        tCIDLib::TCard4     m_c4CurCount;
        tCIDLib::EAdoptOpts m_eAdopt;
};



// ---------------------------------------------------------------------------
//   CLASS: TSafeRefVector
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem, typename TIndex = tCIDLib::TCard4>
class TSafeRefVector : public TRefVector<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested class type aliases
        // -------------------------------------------------------------------
        using TMyType       = TSafeRefVector<TElem, TIndex>;
        using TParType      = TRefVector<TElem,TIndex>;

        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"TSafeRefVector<TElem,TIndex>");
            return clsRet;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeRefVector() = delete;

        TSafeRefVector(const tCIDLib::EAdoptOpts eAdopt) :

            TParType(eAdopt)
        {
        }

        TSafeRefVector( const   tCIDLib::EAdoptOpts eAdopt
                        , const TIndex              tInitAlloc) :

            TParType(eAdopt, tInitAlloc)
        {
        }

        TSafeRefVector(const TMyType&) = delete;
        TSafeRefVector(TMyType&&) = delete;

        ~TSafeRefVector()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType&) = delete;
        TMyType& operator=(TMyType&& colSrc) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const final
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TParType::bIsDescendantOf(clsTarget);
        }

        tCIDLib::TBoolean bTryLock(const tCIDLib::TCard4 c4WaitMS) const override
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

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4WaitMSs) const override
        {
            m_mtxSync.Lock(c4WaitMSs);
        }

        tCIDLib::TVoid Unlock() const override
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

