//
// FILE NAME: CIDLib_FixedSizePool.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/18/2019
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
//  This file implements the TFixedSizePool. It is very similar to the TSimplePool
//  class, so see the comments there. But the difference is that the elements are
//  all fixed size and so interchangeable. So it can be a lot more efficient when
//  being used for pools of such elements, because it knows that it can just stick
//  new or returned ones on the end of their respective lists, and take elements
//  off the end of the lists.
//
//  The thread safety issues are exactly the same as the simple pool class so see
//  it for details.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

template <typename TElem> class TFixedSizePool;


// ---------------------------------------------------------------------------
//  CLASS: TFixedPoolPtr
// PREFIX: spptr
//
//  This is a smart pointer for objects in a pool. It gets an object out of a pool
//  and makes sure it gets given back. It will reference copy the object, so it knows
//  when the last ref is gone.
// ---------------------------------------------------------------------------
template <typename TElem> class TFixedPoolPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TFixedPoolPtr(const TFixedPoolPtr& spptrSrc) :

            m_percThis(nullptr)
        {
            if (&spptrSrc != this)
                TakeSource(spptrSrc);
        }

        TFixedPoolPtr(TFixedPoolPtr&& spptrSrc) : TFixedPoolPtr()
        {
            *this = tCIDLib::ForceMove(spptrSrc);
        }

        virtual ~TFixedPoolPtr()
        {
            DecRefCnt();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFixedPoolPtr& operator=(const TFixedPoolPtr& spptrSrc)
        {
            if (&spptrSrc != this)
                TakeSource(spptrSrc);
            return *this;
        }

        TFixedPoolPtr& operator=(TFixedPoolPtr&& spptrSrc)
        {
            if (&spptrSrc != this)
                tCIDLib::Swap(m_percThis, spptrSrc.m_percThis);
            return *this;
        }

        operator tCIDLib::TBoolean () const
        {
            return (m_percThis != nullptr);
        }

        tCIDLib::TBoolean operator!() const
        {
            return (m_percThis == nullptr);
        }

        TElem* operator->()
        {
            return pobjData();
        }

        const TElem* operator->() const
        {
            return pobjData();
        }

        TElem& operator*()
        {
            return *pobjData();
        }

        const TElem& operator*() const
        {
            return *pobjData();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TElem* pobjData() const
        {
            CIDAssert(m_percThis, L"The pool pointer doesn't have an object");
            return m_percThis->m_pobjElem;
        }

        TElem* pobjData()
        {
            CIDAssert(m_percThis, L"The pool pointer doesn't have an object");
            return m_percThis->m_pobjElem;
        }

        // Release it early back to the pool if we still have it
        tCIDLib::TVoid Release()
        {
            DecRefCnt();
        }


    protected :
        // -------------------------------------------------------------------
        //  The pool has to be a friend to call our protected ctors
        // -------------------------------------------------------------------
        friend class TFixedSizePool<TElem>;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TFixedPoolPtr() :

            m_percThis(nullptr)
        {
        }

        TFixedPoolPtr(TFixedSizePool<TElem>* const psplSrc) :

            m_percThis(nullptr)
        {
            //
            //  If we got a pool, then get an object, and set the initial ref
            //  count to 1. Note no sync required here. If the pool is thread
            //  safe then the object reservation is.
            //
            if (psplSrc)
            {
                TElem* pobjElem = psplSrc->pobjReserveElem();
                m_percThis = new TElemRefCnt;
                m_percThis->m_c4RefCnt = 1;
                m_percThis->m_psplSrc = psplSrc;
                m_percThis->m_pobjElem = pobjElem;
            }
        }




    private :
        // -------------------------------------------------------------------
        //  A little structure to let us wrap and ref count the objects
        // -------------------------------------------------------------------
        struct TElemRefCnt
        {
            tCIDLib::TCard4         m_c4RefCnt;
            TElem*                  m_pobjElem;
            TFixedSizePool<TElem>*  m_psplSrc;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  Do the stuff required to release our object if we have one. If we are
        //  the last, then release the object back to the pool, and clean up our
        //  structure pointer.
        //
        tCIDLib::TVoid DecRefCnt()
        {
            if (!m_percThis)
                return;

            try
            {
                // We shouldn't have a pointer if the ref count is zero, but check
                CIDAssert(m_percThis->m_c4RefCnt, L"Simple pool ptr ref cnt underflow");

                if (m_percThis->m_c4RefCnt && (--m_percThis->m_c4RefCnt == 0))
                {
                    m_percThis->m_psplSrc->ReleaseElem(m_percThis->m_pobjElem);
                    delete m_percThis;
                }

                // We drop our pointer either way
                m_percThis = nullptr;
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }
        }


        //
        //  Do the stuff for us to take on another pointer's object, decrementing
        //  and releasing ours as required, and incrementing his. We know the source
        //  cannot change other than the ref count (which cannot go to zero while we
        //  are here since the copy we have is keeping it alive.) So we just have to
        //  sync the ref counting.
        //
        tCIDLib::TVoid TakeSource(const TFixedPoolPtr& spptrSrc)
        {
            // Decrement ours first, releasing if needed
            try
            {
                DecRefCnt();
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            // And now take the source's and bump the count
            if (spptrSrc.m_percThis)
            {
                m_percThis = spptrSrc.m_percThis;
                m_percThis->m_c4RefCnt++;
            }
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_percThis
        //      If this is non-null we have a ref to an object. This is what we
        //      copy/assign and ref count.
        // -------------------------------------------------------------------
        TElemRefCnt*    m_percThis;
};



// ---------------------------------------------------------------------------
//  CLASS: TFixedSizePool
// PREFIX: spl
// ---------------------------------------------------------------------------
template <typename TElem> class TFixedSizePool : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TElemList = TRefVector<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TFixedSizePool(const TFixedSizePool&) = delete;

        TFixedSizePool(TFixedSizePool&&) = delete;

        ~TFixedSizePool()
        {
            {
                // Lock if we are MT safe
                TLocker lockrSync(m_pmtxSync);

                //
                //  Neither our free or used lists are adopting, so we have to
                //  explicitly free them.
                //
                const tCIDLib::TCard4 c4FreeCnt = m_colFreeList.c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4FreeCnt; c4Index++)
                {
                    try
                    {
                        delete m_colFreeList[c4Index];
                    }

                    catch(...) {}
                }

                const tCIDLib::TCard4 c4UsedCnt = m_colUsedList.c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4UsedCnt; c4Index++)
                {
                    try
                    {
                        delete m_colUsedList[c4Index];
                    }

                    catch(...) {}
                }

                // Now remove the references
                m_colFreeList.RemoveAll();
                m_colUsedList.RemoveAll();
            }

            // Clean up the mutex if we created it.
            delete m_pmtxSync;
        }



        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFixedSizePool& operator=(const TFixedSizePool&) = delete;
        TFixedSizePool& operator=(TFixedSizePool&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  Return the overall count of elements. We have to lock here since we
        //  have to get the current size of both lists.
        //
        tCIDLib::TCard4 c4ElemCount() const
        {
            TLocker lockrSync(m_pmtxSync);
            return m_colFreeList.c4ElemCount() + m_colUsedList.c4ElemCount();
        }

        //
        //  Return the max count we could have, minus the number already in the
        //  used list.
        //
        tCIDLib::TCard4 c4ElemsAvail() const
        {
            TLocker lockrSync(m_pmtxSync);
            return (m_c4MaxPoolSize - m_colUsedList.c4ElemCount());
        }


        // Return the maximum poll expansion size
        tCIDLib::TCard4 c4MaxElemCount() const
        {
            return m_c4MaxPoolSize;
        }


        //
        //  Reserves an element from the pool, creating a new one if necessary. You
        //  are responsible for returning it to the pool yourself in this case.
        //
        //  See spptrReserveElem() below for some possibly safer methods.
        //
        [[nodiscard]] TElem* pobjReserveElem()
        {
            // Lock if we are MT safe
            TLocker lockrSync(m_pmtxSync);

            // Make sure that we can reserve another one
            const tCIDLib::TCard4 c4FreeCnt = m_colFreeList.c4ElemCount();
            const tCIDLib::TCard4 c4UsedCnt = m_colUsedList.c4ElemCount();
            if (c4UsedCnt >= m_c4MaxPoolSize)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcSPool_PoolIsFull
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::OutResource
                    , m_strName
                );
            }

            // Make sure combined count hasn't gone past max
            CIDAssert
            (
                (c4FreeCnt + c4UsedCnt) <= m_c4MaxPoolSize
                , L"Combined free/used lists are larger than max pool size"
            );

            // If we have any free ones to select from, then take one, else create one
            TElem* pelemRet = nullptr;
            if (c4FreeCnt)
                pelemRet = m_colFreeList.pobjOrphanLast();
            else
                pelemRet = pelemMakeNew();

            // Insert it into the used list by address
            tCIDLib::TCard4 c4InsertAt;
            m_colUsedList.pobjBinarySearch(*pelemRet, eCompAddr, c4InsertAt);
            m_colUsedList.InsertAt(pelemRet, c4InsertAt);

            //
            //  Let the derived class prep it and return it. The caller is responsible
            //  for returning it to us.
            //
            PrepElement(*pelemRet);
            return pelemRet;
        }


        // In case someone needs to do more than atomic ops
        TMutex* pmtxSync()
        {
            return m_pmtxSync;
        }


        // Return the current sizes of our used and free lists
        tCIDLib::TVoid QueryListSizes(tCIDLib::TCard4& c4Used, tCIDLib::TCard4& c4Free)
        {
            // Lock if we are MT safe
            TLocker lockrSync(m_pmtxSync);

            c4Used = m_colUsedList.c4ElemCount();
            c4Free = m_colFreeList.c4ElemCount();
        }


        //
        //  Only call this when no one is using the pool! Otherwise you are going
        //  make a mess of things. We can't really double check here since the purpose
        //  of this is to allow them to all be released after the fact though the
        //  threads that had reserved them never gave them back formally.
        //
        tCIDLib::TVoid ReleaseAll()
        {
            // Lock if we are MT safe
            TLocker lockrSync(m_pmtxSync);

            //
            //  Take all of the elements from the used list and put them back into
            //  the free list, then sort it.
            //
            if (!m_colUsedList.bIsEmpty())
            {
                const tCIDLib::TCard4 c4Count = m_colUsedList.c4ElemCount();

                // We have to have room to put them all back to the free list
                CIDAssert
                (
                    (c4Count + m_colFreeList.c4ElemCount()) <= m_c4MaxPoolSize
                    , L"Combined free/used lists are larger than max pool size"
                );
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                    m_colFreeList.Add(m_colUsedList[c4Index]);

                // Let them go out of the used list now
                m_colUsedList.RemoveAll();
            }
        }


        //  Releases the passed object back to the free list
        tCIDLib::TVoid ReleaseElem(TElem* const pobjToRelease)
        {
            // Lock if we are MT safe
            TLocker lockrSync(m_pmtxSync);

            // Find this guy in the used list, which is sorted by address
            tCIDLib::TCard4 c4AtUsed;
            if (!m_colUsedList.pobjBinarySearch(*pobjToRelease, eCompAddr, c4AtUsed))
            {
                //
                //  This is bad. Try to recover by seeing if it is in the free list.
                //  If so, just leave it alone, though we log an error. If not in
                //  the free list, delete it as the least worst scenario.
                //
                const tCIDLib::TCard4 c4FreeCnt = m_colFreeList.c4ElemCount();
                tCIDLib::TCard4 c4Index = 0;
                for (; c4Index < c4FreeCnt; c4Index++)
                {
                    if (m_colFreeList[c4Index] == pobjToRelease)
                        break;
                }

                if (c4Index == c4FreeCnt)
                {
                    try
                    {
                        delete pobjToRelease;
                    }

                    catch(TError& errToCatch)
                    {
                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        TModule::LogEventObj(errToCatch);
                    }
                }

                // It isn't a legal pool object that's in use
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcSPool_NotReservedElem
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , m_strName
                );
            }

            //
            //  Let's put it back into the free list and orphan from the used list.
            //  We have to suppress the nodiscard warning from the orphan call, which
            //  we can do by just pass that to Add instead of the incoming value.
            //  They are the same object.
            //
            m_colFreeList.Add(m_colUsedList.pobjOrphanAt(c4AtUsed));

            // Make sure combined count hasn't gone past max
            CIDAssert
            (
                (m_colUsedList.c4ElemCount() + m_colFreeList.c4ElemCount()) <= m_c4MaxPoolSize
                , L"Combined free/used lists are larger than max pool size"
            );
        }


        // Returns an empty pool pointer that you can set later
        TFixedPoolPtr<TElem> spptrEmpty()
        {
            return TFixedPoolPtr<TElem>();
        }


        //
        //  Create a pool pointer which will reserve an element for you and return it
        //  automatically when the last reference is destroyed.
        //
        TFixedPoolPtr<TElem> spptrReserveElem()
        {
            return TFixedPoolPtr<TElem>(this);
        }


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TFixedSizePool( const   tCIDLib::TCard4     c4MaxAlloc
                        , const TString&            strName
                        , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            m_c4MaxPoolSize(c4MaxAlloc ? c4MaxAlloc : 8192)
            , m_colFreeList
              (
                tCIDLib::EAdoptOpts::NoAdopt, tCIDLib::MinVal(m_c4MaxPoolSize / 8, 128UL)
              )
            , m_colUsedList
              (
                tCIDLib::EAdoptOpts::NoAdopt, tCIDLib::MinVal(m_c4MaxPoolSize / 8, 128UL)
              )
            , m_pmtxSync(nullptr)
            , m_strName(strName)
        {
            // If MT safe, then allocate the critical section
            if (eMTSafe == tCIDLib::EMTStates::Safe)
                m_pmtxSync = new TMutex;
         }


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] virtual TElem* pelemMakeNew() = 0;

        virtual tCIDLib::TVoid PrepElement
        (
                    TElem&                  objTar
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  We can provide a static search/sort comparitor for the reserved list,
        //  which is always done on the address of the elements.
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eCompAddr(const  TElem&  elemFirst
                                            , const TElem&  elemSec)
        {
            if (&elemFirst < &elemSec)
                return tCIDLib::ESortComps::FirstLess;
            else if (&elemFirst > &elemSec)
                return tCIDLib::ESortComps::FirstGreater;
            return tCIDLib::ESortComps::Equal;
        }



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxPoolSize
        //      The max number of elements that we can hold in the pool.
        //
        //  m_colFreeList
        //      The list of available elements, sorted by the user data
        //      attribute of the elements. This is a non-adopting list since
        //      we need to regularly move elements to the used list.
        //
        //  m_colUsed
        //      The list of elements that have been reserved, sorted by the
        //      address of the elements.  This is a non-adopting list since
        //      we need to regularly move elements to the free list.
        //
        //  m_pmtxSync
        //      If the outside world wants to do synchronized access to us, we
        //      need to lock while we access the list. They indicate if they
        //      want synchronized access via the eMTSafe parameter to the
        //      ctor. We only allocate this if they indicate we should be
        //      thread safe.
        //
        //  m_strName
        //      The client provides a name which we just use in any errors
        //      we throw or messsages we log.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4MaxPoolSize;
        TElemList                   m_colFreeList;
        TElemList                   m_colUsedList;
        TMutex*                     m_pmtxSync;
        TString                     m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TFixedSizePool,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TFixedSizePoolJan
// PREFIX: jan
//
//  This guy is for getting pool elements on a scoped basis. It gets one on ctor
//  and release on dtor. It doesn't allow any copying or moviing.
// ---------------------------------------------------------------------------
template <typename TElem> class TFixedSizePoolJan
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TPoolType = TFixedSizePool<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFixedSizePoolJan() = delete;

        TFixedSizePoolJan(TPoolType* const psplSrc) :

            m_pobjGotten(nullptr)
            , m_psplSrc(psplSrc)
        {
            // Ask for the object and store it
            if (m_psplSrc)
                m_pobjGotten = psplSrc->pobjReserveElem();
        }

        // They already have one and want to insure it gets given back on exit
        TFixedSizePoolJan(TPoolType* const psplSrc, TElem* const pobjToReturn) :

            m_pobjGotten(pobjToReturn)
            , m_psplSrc(psplSrc)
        {
        }

        TFixedSizePoolJan(const TFixedSizePoolJan&) = delete;
        TFixedSizePoolJan(TFixedSizePoolJan&&) = delete;

        ~TFixedSizePoolJan()
        {
            // Release the element that we got, if we still have it
            if (m_psplSrc && m_pobjGotten)
                m_psplSrc->ReleaseElem(m_pobjGotten);
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFixedSizePoolJan& operator=(const TFixedSizePoolJan&) = delete;
        TFixedSizePoolJan& operator=(TFixedSizePoolJan&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        explicit operator tCIDLib::TBoolean() const
        {
            return m_psplSrc != nullptr;
        }

        const TElem* operator->() const
        {
            return m_pobjGotten;
        }

        TElem* operator->()
        {
            return m_pobjGotten;
        }

        const TElem& operator*() const
        {
            return *m_pobjGotten;
        }

        TElem& operator*()
        {
            return *m_pobjGotten;
        }


        //
        //  The caller becomes responsible for return to the pool. Could be null
        //  if already released or orphaned.
        //
        [[nodiscard]] TElem* pobjOrphan()
        {
            TElem* pobjRet = m_pobjGotten;
            if (m_psplSrc)
            {
                m_pobjGotten = nullptr;
                m_psplSrc = nullptr;
            }
            return pobjRet;
        }


        // Release it early back to the pool
        tCIDLib::TVoid Release()
        {
            if (m_psplSrc)
            {
                m_psplSrc->ReleaseElem(m_pobjGotten);
                m_pobjGotten = nullptr;
                m_psplSrc = nullptr;
            }
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pobjGotten
        //      The object we got and need to give back.
        //
        //  m_psplSrc
        //      The source pool that we got the object from. If this is non-null
        //      m_pobjGotten has to be valid.
        // -------------------------------------------------------------------
        TElem*      m_pobjGotten;
        TPoolType*  m_psplSrc;
};

#pragma CIDLIB_POPPACK

