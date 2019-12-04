//
// FILE NAME: CIDLib_SimplePool.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/16/1999
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
//  This file implements the TSimplePool template class, and various related class.
//  The pool implements a commonly used scheme where a simple, dynamically growable
//  pool of some type of resizable object are desired and the usage is such that
//  creating/destroying them for each use would be a performance issue. Objects in
//  the pool can be requested for temporary use and then given back to the pool.
//
//  Efficiency is a big issue here since the pools can get sort of large, and
//  the rate at which elements are added and removed might be high. If
//  we had to do a linear search of the list to find a free element or to
//  find an element in order to free it, it would pig out in a major way.
//
//  So we maintain two lists, one of free elements and one of reserved elements.
//  The reserved elements list we keep sorted by the address of the elements, so
//  that we can quickly find an object being released.
//
//  The free list must be sorted by the size of the elements. The derived class
//  provides us with a method that will extract the size from the elements, and we
//  use that for sorting.
//
//  When an element is reserved, a minimum required size is provided. If there are
//  free elements, we find a best match in the pool to use. Else we ask the derived
//  class to create a new one of that size (though he can adjust upwards from the
//  requested size.)
//
//  Element Prep
//
//  When an element is selected from the free list upon reservation, PrepElement
//  is called to allow the derived class to reset it and/or adjust it as required.
//  It can expand it or whatever. The requested size is passed along for that
//  purpose.
//
//  Janitors/Pointers
//
//  To make getting and giving back elements safe on a scoped basis, a small
//  janitor class is provided. It's ctor will get an element from the pool. Its
//  dtor will give it back.
//
//  We also provide a smart pointer as well. You can either create a derivative of
//  it and use that, or you can ask the pool to create instances of them. The former
//  is convenient if your derivative has access to the pool, which may be some internal
//  implementation detail. Your derived class' constructor can just self-reserve an
//  element for itself. So you just need to declare an instance of the smart pointer
//  to get an element.
//
//  String/Heap Buffer Pool
//
//  It will be fairly common to use pools of strings or heap buffers, so we provide
//  varations that will likely work for most such situations. The client provides an
//  expansion increment we will use if we need to expand one, so we don't just expand
//  up to the requested size, we go there plus the increment.

//  And he can provide a 'big threshold'. Sometimes occasional big elements are
//  allocated but we don't want to keep them around wasting memory. If any element is
//  beyond this threshold, it will be reallocated back down to something small during
//  element prep.
//
//  Fixed Size Elements
//
//  If the elements are all the same size, use the TFixedSizedPool class instead which
//  is much more efficient for such things.
//
//  Thread Safety
//
//  Pools can be marked as thread safe or not. If you are going to have multiple
//  threads getting objects out, then it must be marked as asafe.
//
//  The smart pointers are NOT thread safe, and you shouldn't have multiple thread
//  sharing a single object from the pool anyway. The whole point of the pool is to
//  let them all get their own objects. Multiple threads can create their own to get
//  their own objects from a single pool, as long as that pool itself is marked
//  thread safe. You can pass these pointers from one thread to another if you are
//  using some sort of thread safe transfer mechanism to do so.
//
//  The janitors are not, inherently so since they are for scoped access.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

template<typename TElem> class TSimplePool;


// ---------------------------------------------------------------------------
//  CLASS: TSimplePoolPtr
// PREFIX: spptr
//
//  This is a smart pointer for objects in a simple pool. It gets an object out
//  of a pool and makes sure it gets given back. It will reference copy the object,
//  so it knows when the last ref is gone.
//
//  Though in theory these are thread safe, you should never actually be sharing
//  them between threads. The ref counting is just to let you pass them around
//  and use them as by value objects conveniently. See the class comments above.
// ---------------------------------------------------------------------------
template <typename TElem> class TSimplePoolPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TSimplePoolPtr(const TSimplePoolPtr& spptrSrc) :

            m_percThis(nullptr)
        {
            if (&spptrSrc != this)
                TakeSource(spptrSrc);
        }

        TSimplePoolPtr(TSimplePoolPtr&& spptrSrc) :

            TSimplePoolPtr()
        {
            *this = tCIDLib::ForceMove(spptrSrc);
        }

        virtual ~TSimplePoolPtr()
        {
            DecRefCnt();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSimplePoolPtr& operator=(const TSimplePoolPtr& spptrSrc)
        {
            if (&spptrSrc != this)
                TakeSource(spptrSrc);
            return *this;
        }

        TSimplePoolPtr& operator=(TSimplePoolPtr&& spptrSrc)
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
        friend class TSimplePool<TElem>;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TSimplePoolPtr() :

            m_percThis(nullptr)
        {
        }

        TSimplePoolPtr(TSimplePool<TElem>* const psplSrc, const tCIDLib::TCard4 c4Size) :

            m_percThis(nullptr)
        {
            //
            //  If we got a pool, then get an object, and set the initial ref
            //  count to 1. Note no sync required here. If the pool is thread
            //  safe then the object reservation is.
            //
            if (psplSrc)
            {
                TElem* pobjElem = psplSrc->pobjReserveElem(c4Size);
                m_percThis = new TElemRefCnt;
                m_percThis->m_c4RefCnt = 1;
                m_percThis->m_psplSrc = psplSrc;
                m_percThis->m_pobjElem = pobjElem;
            }
        }


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        TSimplePool<TElem>* psplOwner()
        {
            if (!m_percThis)
                return nullptr;
            return m_percThis->m_psplSrc;
        }


    private :
        // -------------------------------------------------------------------
        //  A little structure to let us wrap and ref count the objects
        // -------------------------------------------------------------------
        struct TElemRefCnt
        {
            tCIDLib::TCard4     m_c4RefCnt;
            TElem*              m_pobjElem;
            TSimplePool<TElem>* m_psplSrc;
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
        //  are here since the copy we are taking from is keeping it alive.)
        //
        tCIDLib::TVoid TakeSource(const TSimplePoolPtr& spptrSrc)
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
//  CLASS: TSimplePool
// PREFIX: spl
// ---------------------------------------------------------------------------
template<typename TElem> class TSimplePool : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TElemList = TRefVector<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TSimplePool(const TSimplePool&) = delete;
        TSimplePool(TSimplePool&&) = delete;

        ~TSimplePool()
        {
            {
                // Lock if we are MT safe
                TMtxLocker mtxlSync(m_pmtxSync);

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

                    catch(...)
                    {
                    }
                }

                const tCIDLib::TCard4 c4UsedCnt = m_colUsedList.c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4UsedCnt; c4Index++)
                {
                    try
                    {
                        delete m_colUsedList[c4Index];
                    }

                    catch(...)
                    {
                    }
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
        TSimplePool& operator=(const TSimplePool&) = delete;
        TSimplePool& operator=(TSimplePool&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  Return the overall count of elements. We have to lock here since we
        //  have to get the current size of both lists.
        //
        tCIDLib::TCard4 c4ElemCount() const
        {
            TMtxLocker mtxlSync(m_pmtxSync);
            return m_colFreeList.c4ElemCount() + m_colUsedList.c4ElemCount();
        }

        //
        //  Return the max count we could have, minus the number already in the
        //  used list.
        //
        tCIDLib::TCard4 c4ElemsAvail() const
        {
            TMtxLocker mtxlSync(m_pmtxSync);
            return (m_c4MaxPoolSize - m_colUsedList.c4ElemCount());
        }


        //
        //  Return the number of elements in use
        //
        tCIDLib::TCard4 c4ElemsUsed() const
        {
            TMtxLocker mtxlSync(m_pmtxSync);
            return m_colUsedList.c4ElemCount();
        }


        // Return the maximum poll expansion size
        tCIDLib::TCard4 c4MaxElemCount() const
        {
            return m_c4MaxPoolSize;
        }


        // Reserves an element from the pool, creating a new one if necessary
        [[nodiscard]] TElem* pobjReserveElem(const tCIDLib::TCard4 c4Size)
        {
            // Lock if we are MT safe
            TMtxLocker mtxlSync(m_pmtxSync);

            // Make sure that we can add another used object
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

            //
            //  Remember if we can create a new one if the derived class doesn't
            //  pick a candidate. Even if they don't, once we max out the total
            //  node count we can't create another we have to use something in the
            //  free list.
            //
            const tCIDLib::TCard4 c4FreeCnt = m_colFreeList.c4ElemCount();
            const tCIDLib::TBoolean bCanMakeNew((c4FreeCnt + c4UsedCnt) < m_c4MaxPoolSize);

            // Make sure combined count hasn't gone past max
            CIDAssert
            (
                (c4FreeCnt + c4UsedCnt) <= m_c4MaxPoolSize
                , L"Combined free/used lists are larger than max pool size"
            );

            // If we have any free ones to select from, then try that first
            TElem* pelemRet = nullptr;
            if (c4FreeCnt)
            {
                pelemRet = pobjFindCandidate(c4Size);
                if (!pelemRet)
                {
                    //
                    //  He didn't select one. If we can't make a new one, then we
                    //  have failed.
                    //
                    if (!bCanMakeNew)
                    {
                        facCIDLib().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kCIDErrs::errcSPool_CouldNotReserve
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::OutResource
                            , m_strName
                        );
                    }
                }
            }

            // If no free one to select from
            if (!pelemRet)
            {
                CIDAssert(bCanMakeNew, L"Should be able to make new pool element");
                pelemRet = pelemMakeNew(c4Size);
                CIDAssert(pelemRet != nullptr, L"Derived class did not create a new element");
            }

            //
            //  We need to get it moved to it's sorted place in the used list. We
            //  know there is space for it since we checked above.
            //
            tCIDLib::TCard4 c4InsertAt;
            CIDLib_Suppress(6011) // It's NOT ever null here, we create one above if needed
            m_colUsedList.pobjBinarySearch(*pelemRet, eCompAddr, c4InsertAt);
            m_colUsedList.InsertAt(pelemRet, c4InsertAt);

            // OK, finally. The caller is responsible for returning it to the pool
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
            TMtxLocker mtxlSync(m_pmtxSync);

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
            TMtxLocker mtxlSync(m_pmtxSync);

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

                // Resort the free list now
                m_colFreeList.Sort
                (
                    [this](const TElem& obj1, const TElem& obj2)
                    {
                        const tCIDLib::TCard4 c4Sz1 = this->c4ElemSize(obj1);
                        const tCIDLib::TCard4 c4Sz2 = this->c4ElemSize(obj2);
                        if (c4Sz1 < c4Sz2)
                            return tCIDLib::ESortComps::FirstLess;
                        else if (c4Sz1 > c4Sz2)
                            return tCIDLib::ESortComps::FirstGreater;
                        return tCIDLib::ESortComps::Equal;
                    }
                );
            }
        }


        //  Releases the passed object back to the free list
        tCIDLib::TVoid ReleaseElem(TElem* const pobjToRelease)
        {
            // Lock if we are MT safe
            TMtxLocker mtxlSync(m_pmtxSync);

            // Find this guy in the used list, which is by address
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
            //  Let's put it back into the free list at its size sorted position,
            //  and orphan from the used list. We have to suppress the nodiscard
            //  warning from the orphan call, so we pass it to AddToFreeList()
            //  instead of the original parameter version.
            //
            AddToFreeList(m_colUsedList.pobjOrphanAt(c4AtUsed));

            // Make sure combined count hasn't gone past max
            CIDAssert
            (
                (m_colUsedList.c4ElemCount() + m_colFreeList.c4ElemCount()) <= m_c4MaxPoolSize
                , L"Combined free/used lists are larger than max pool size"
            );
        }


        // Returns an empty pool pointer that you can set later
        TSimplePoolPtr<TElem> spptrEmpty()
        {
            return TSimplePoolPtr<TElem>();
        }


        //
        //  Create a pool pointer which will reserve an element for you and return it
        //  automatically when the last reference is destroyed.
        //
        TSimplePoolPtr<TElem> spptrReserveElem(const tCIDLib::TCard4 c4Size)
        {
            return TSimplePoolPtr<TElem>(this, c4Size);
        }


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TSimplePool(const   tCIDLib::TCard4     c4MaxAlloc
                    , const TString&            strName
                    , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            m_c4MaxPoolSize(c4MaxAlloc ? c4MaxAlloc : 8192)
            , m_colFreeList
              (
                tCIDLib::EAdoptOpts::NoAdopt
                , tCIDLib::MinVal(m_c4MaxPoolSize / 8, 128UL)
              )
            , m_colUsedList
              (
                tCIDLib::EAdoptOpts::NoAdopt
                , tCIDLib::MinVal(m_c4MaxPoolSize / 8, 128UL)
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
        virtual tCIDLib::TCard4 c4ElemSize
        (
            const   TElem&                  objSrc
        ) = 0;

        [[nodiscard]] virtual TElem* pelemMakeNew
        (
            const   tCIDLib::TCard4         c4Size
        ) = 0;

        virtual tCIDLib::TVoid PrepElement
        (
                    TElem&                  objTar
            , const tCIDLib::TCard4         c4Size
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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        // WE find the correct insert point for it based on size and put it back
        tCIDLib::TVoid AddToFreeList(TElem* const pobjAdd)
        {
            const tCIDLib::TCard4 c4Sz = c4ElemSize(*pobjAdd);

            // It will return the insert position even if not found
            tCIDLib::TCard4 c4At;
            pobjSearchFreeList(c4Sz, c4At);
            m_colFreeList.InsertAt(pobjAdd, c4At);
        }


        TElem* pobjFindCandidate(const tCIDLib::TCard4 c4Size)
        {
            const tCIDLib::TCard4 c4FreeCnt = m_colFreeList.c4ElemCount();
            tCIDLib::TCard4 c4At;
            TElem* pobjRet = pobjSearchFreeList(c4Size, c4At);

            //
            //  If no actual match, then let's find one that's best. It returns
            //  the final position even if nothing is found.
            //
            if (!pobjRet)
            {
                if (c4At >= c4FreeCnt)
                {
                    // Nothing is big enough, so take the last one
                    c4At = c4FreeCnt - 1;
                }
                 else
                {
                    //
                    //  If the target is smaller than the size, then look forward
                    //  for one that is larger. We usually will be between two that
                    //  are smaller and larger (respectively) but we could be in a
                    //  block of ones of the same size. So we look forward possibly
                    //  to the end.
                    //
                    //  If we find one bigger, then we'll take it, else we stick
                    //  with the one the search ended on.
                    //
                    tCIDLib::TCard4 c4TarSz = c4ElemSize(*m_colFreeList[c4At]);
                    if (c4TarSz < c4Size)
                    {
                        tCIDLib::TCard4 c4FIndex = c4At + 1;
                        while (c4FIndex < c4FreeCnt)
                        {
                            tCIDLib::TCard4 c4NextSz = this->c4ElemSize(*m_colFreeList[c4FIndex]);
                            if (c4NextSz > c4Size)
                            {
                                c4At = c4FIndex;
                                c4TarSz = c4NextSz;
                                break;
                            }
                            c4FIndex++;
                        }
                    }
                }

                // OK, so let's get a pointer to the one we selected
                pobjRet = m_colFreeList[c4At];
            }

            //
            //  Let the derived class prep it. If that fails, then nothing
            //  has been done. We only found one, it's not been removed from
            //  the free list yet, so we can throw.
            //
            try
            {
                PrepElement(*pobjRet, c4Size);
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            // It survived so orphan from the free list
            return m_colFreeList.pobjOrphanAt(c4At);
        }


        // The binary search returns the insert point if it doesn't get a match
        TElem* pobjSearchFreeList(const tCIDLib::TCard4 c4Sz, tCIDLib::TCard4& c4At)
        {
            TElem* pobjRet = m_colFreeList.pobjKeyedBinarySearch
            (
                c4Sz
                , [this](const tCIDLib::TCard4& c4Key, const TElem& obj2)
                  {
                    const tCIDLib::TCard4 c4Sz2 = this->c4ElemSize(obj2);
                    if (c4Key < c4Sz2)
                        return tCIDLib::ESortComps::FirstLess;
                    else if (c4Key > c4Sz2)
                        return tCIDLib::ESortComps::FirstGreater;
                    return tCIDLib::ESortComps::Equal;
                  }
                , c4At
            );
            return pobjRet;
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
        TemplateRTTIDefs(TSimplePool,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TSimplePoolJan
// PREFIX: jan
//
//  This guy is for getting pool elements on a scoped basis. It gets one on ctor
//  and release on dtor. It doesn't allow any copying or moviing.
// ---------------------------------------------------------------------------
template <typename TElem> class TSimplePoolJan
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TPoolType = TSimplePool<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSimplePoolJan() = delete;

        TSimplePoolJan(TPoolType* const psplSrc, const tCIDLib::TCard4 c4Size) :

            m_pobjGotten(nullptr)
            , m_psplSrc(psplSrc)
        {
            // Ask for the object and store it
            if (m_psplSrc)
                m_pobjGotten = psplSrc->pobjReserveElem(c4Size);
        }

        // They already have one and want to insure it gets given back on exit
        TSimplePoolJan(TPoolType* const psplSrc, TElem* const pobjToReturn) :

            m_pobjGotten(pobjToReturn)
            , m_psplSrc(psplSrc)
        {
        }

        TSimplePoolJan(const TSimplePoolJan&) = delete;
        TSimplePoolJan(TSimplePoolJan&&) = delete;

        ~TSimplePoolJan()
        {
            // Release the element that we got, if we still have it
            if (m_psplSrc && m_pobjGotten)
                m_psplSrc->ReleaseElem(m_pobjGotten);
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSimplePoolJan& operator=(const TSimplePoolJan&) = delete;
        TSimplePoolJan& operator=(TSimplePoolJan&&) = delete;


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
        TElem* pobjOrphan()
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



// ---------------------------------------------------------------------------
//  CLASS: TStringPool
// PREFIX: spl
// ---------------------------------------------------------------------------
class TStringPool : public TSimplePool<TString>
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TElemJan = TSimplePoolJan<TString>;
        using TElemPtr = TSimplePoolPtr<TString>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStringPool(const   TString&            strName
                    , const tCIDLib::TCard4     c4MaxElems
                    , const tCIDLib::TCard4     c4ExpIncrement = 0
                    , const tCIDLib::TCard4     c4BigThreshold = kCIDLib::c4MaxCard) :

            TSimplePool(c4MaxElems, strName)
            , m_c4BigThreshold(c4BigThreshold)
            , m_c4ExpIncrement(c4ExpIncrement)
        {
        }

        TStringPool(const TStringPool&) = delete;
        TStringPool(TStringPool&&) = delete;

        ~TStringPool() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringPool& operator=(const TStringPool&) = delete;
        TStringPool& operator=(TStringPool&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BigThreshold() const
        {
            return m_c4BigThreshold;
        }

        tCIDLib::TCard4 c4BigThreshold(const tCIDLib::TCard4 c4ToSet)
        {
            m_c4BigThreshold = c4ToSet;
            return m_c4BigThreshold;
        }


    protected :
        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ElemSize(const TString& strSrc) override
        {
            return strSrc.c4BufChars();
        }

        // Create a new string with the requested size
        [[nodiscard]] TString* pelemMakeNew(const tCIDLib::TCard4 c4Size) override
        {
            return new TString(c4Size);
        }

        //
        //  If smaller the size, then expand it up to that plus the expansion
        //  increment. If not, we just clear the string to get it ready for the
        //  next use.
        //
        tCIDLib::TVoid PrepElement(TString& strTar, const tCIDLib::TCard4 c4Size) override
        {
            //
            //  If bigger than the big threshold, and the big threshold is larger
            //  than the requested size, or the new guy is smaller than the requested
            //  size, then reallocate to the size plus expansion increment.
            //
            //  Else just clear for reuse.
            //
            if (((strTar.c4BufChars() > m_c4BigThreshold) && (m_c4BigThreshold >= c4Size))
            ||  (strTar.c4BufChars() < c4Size))
            {
                strTar.Reallocate(c4Size + m_c4ExpIncrement, kCIDLib::False);
            }
             else
            {
                strTar.Clear();
            }
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BigThreshold
        //      When putting an entry back into the free list, if it's bigger than
        //      this, we will reallocate it back down to this so that really big
        //      ones that might only be used occasionally don't stick around. We
        //      default it to maxcard so it doesn't do anything.
        //
        //  m_c4ExpIncrement
        //      If we need to expand a string capacity, we reallocate up to the
        //      size  size plus this expansion increment.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4BigThreshold;
        tCIDLib::TCard4     m_c4ExpIncrement;
};



// ---------------------------------------------------------------------------
//  CLASS: THeapBufPool
// PREFIX: spl
//
//  The free list is sorted by the buffer size of the buffers, so we can quickly
//  find one of the right size.
// ---------------------------------------------------------------------------
class THeapBufPool : public TSimplePool<THeapBuf>
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TElemJan = TSimplePoolJan<THeapBuf>;
        using TElemPtr = TSimplePoolPtr<THeapBuf>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THeapBufPool(const   TString&           strName
                    , const tCIDLib::TCard4     c4MaxElems
                    , const tCIDLib::TCard4     c4ExpIncrement = 0
                    , const tCIDLib::TCard4     c4BigThreshold = kCIDLib::c4MaxCard) :

            TSimplePool(c4MaxElems, strName)
            , m_c4BigThreshold(c4BigThreshold)
            , m_c4ExpIncrement(c4ExpIncrement)
        {
        }

        THeapBufPool(const THeapBufPool&) = delete;

        ~THeapBufPool() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THeapBufPool& operator=(const THeapBufPool&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BigThreshold() const
        {
            return m_c4BigThreshold;
        }

        tCIDLib::TCard4 c4BigThreshold(const tCIDLib::TCard4 c4ToSet)
        {
            m_c4BigThreshold = c4ToSet;
            return m_c4BigThreshold;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ElemSize(const THeapBuf& mbufSrc) override
        {
            return mbufSrc.c4Size();
        }

        // Create a new string with the requested size
        [[nodiscard]] THeapBuf* pelemMakeNew(const tCIDLib::TCard4 c4Size) override
        {
            return new THeapBuf(c4Size);
        }

        //
        //  If smaller the size, then expand it up to that plus the expansion
        //  increment. If not, we just clear the string to get it ready for the
        //  next use.
        //
        tCIDLib::TVoid PrepElement(THeapBuf& mbufTar, const tCIDLib::TCard4 c4Size) override
        {
            if (mbufTar.c4Size() < c4Size)
                mbufTar.Reallocate(c4Size + m_c4ExpIncrement, kCIDLib::False);

            //
            //  If bigger than the big threshold, and the big threshold is larger
            //  than the requested size, or the new guy is smaller than the requested
            //  size, then reallocate to the size plus expansion increment.
            //
            if (((mbufTar.c4Size() > m_c4BigThreshold) && (m_c4BigThreshold >= c4Size))
            ||  (mbufTar.c4Size() < c4Size))
            {
                mbufTar.Reallocate(c4Size + m_c4ExpIncrement, kCIDLib::False);
            }
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BigThreshold
        //      When putting an entry back into the free list, if it's bigger than
        //      this, we will reallocate it back down to this so that really big
        //      ones that might only be used occasionally don't stick around. We
        //      default it to maxcard so it doesn't do anything.
        //
        //  m_c4ExpIncrement
        //      If we need to expand a string capacity, we reallocate up to the
        //      size size plus this expansion increment.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4BigThreshold;
        tCIDLib::TCard4     m_c4ExpIncrement;
};

#pragma CIDLIB_POPPACK

