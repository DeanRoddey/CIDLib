//
// FILE NAME: CIDLib_SmartPointer.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
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
//  This file defines some smart pointer classes for general usage.
//
//  There is TCntPtr which provides a thread safe counted pointer, using lockless
//  reference counting.

//  We also provide TMngPtr from the base class, which just provides a non-counted
//  pointer. It purely exists to give 'by value' semantics to a pointer. It's assumed
//  that someone owns it and will clean it up. So this class just copies the pointer
//  as required.
//
//  Ane we provide TUniquePtr, which uses move semantics and disables assignment and
//  copy ctor, to insure that only one instance of it will ever have the pointer
//  that it contains (unless you of course explictly set two of them to the same
//  pointer, which you should definitely not do.) It is not thread safe, but it intended
//  to allow you to pass around a pointer and insure that only the last copy of it has
//  the original pointer, and that you can't accidentally assign it to another one.
//
//  But of course be careful you don't assign away the one that you mean to keep.
//
//  To keep some of the functionality out of line, to reduce rebuilds and code footprint
//  we have a little helper namespace with some functions they can call. These are also
//  used by some other smart pointer'ish classes.
//
//  At the bottom we add some smart pointer related helpers to the tCIDLib namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace TSmartPtrHelpers
{
    CIDLIBEXP tCIDLib::TVoid CantAcquireStrongRef
    (
        const   tCIDLib::TCard4         c4Line
    );

    CIDLIBEXP tCIDLib::TVoid CheckRefNotZero
    (
        const   tCIDLib::TCard4         c4Line
        , const tCIDLib::TCard4         c4ToCheck
        , const tCIDLib::TCh* const     pszType
    );

    CIDLIBEXP tCIDLib::TVoid ThrowAlreadyLocked
    (
        const   tCIDLib::TCard4         c4Line
        , const tCIDLib::TCh* const     pszType
    );

    CIDLIBEXP tCIDLib::TVoid ThrowNullRef
    (
        const   tCIDLib::TCard4         c4Line
        , const tCIDLib::TCh* const     pszType
    );
}

template <typename T> class TWeakPtr;


// ---------------------------------------------------------------------------
//   CLASS: TUniquePtr
//  PREFIX: uptr
// ---------------------------------------------------------------------------
template <class T> class TUniquePtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TUniquePtr() :

            m_pData(nullptr)
        {
        }

        explicit TUniquePtr(T* const pAdopt) :

            m_pData(pAdopt)
        {
        }

        TUniquePtr(const TUniquePtr<T>&) = delete;

        TUniquePtr(TUniquePtr<T>&& uptrSrc) :

            m_pData(nullptr)
        {
            *this = tCIDLib::ForceMove(uptrSrc);
        }

        ~TUniquePtr()
        {
            if (m_pData)
            {
                try
                {
                    delete m_pData;
                    m_pData = nullptr;
                }

                catch(...)
                {
                }
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUniquePtr<T>& operator=(const TUniquePtr<T>&) = delete;

        TUniquePtr<T>& operator=(TUniquePtr<T>&& uptrSrc)
        {
            if (&uptrSrc != this)
                tCIDLib::Swap(m_pData, uptrSrc.m_pData);
            return *this;
        }

        operator tCIDLib::TBoolean() const
        {
            return (m_pData != nullptr);
        }

        tCIDLib::TBoolean operator!() const
        {
            return (m_pData == nullptr);
        }

        T* operator->()
        {
            return m_pData;
        }

        const T* operator->() const
        {
            return m_pData;
        }

        const T& operator*() const
        {
            if (!m_pData)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TUniquePtr");
            return *m_pData;
        }

        T& operator*()
        {
            if (!m_pData)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TUniquePtr");
            return *m_pData;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DropRef()
        {
            // If we still have a valid pointer, then we own it
            if (m_pData)
            {
                T* pOld = m_pData;
                m_pData = nullptr;
                delete pOld;
            }
        }

        [[nodiscard]] T* pOrphan()
        {
            T* pRet = m_pData;
            m_pData = nullptr;
            return pRet;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      The data object we were given to handle. It can be null of course.
        // -------------------------------------------------------------------
        T*  m_pData;
};


// ---------------------------------------------------------------------------
//  This is really just for internal use, but it needs to be shared by both counted
//  and weak pointers.
//
//  The count is a double count with the high 16 bits used for weak ref counts and
//  the low 16 for strong ref counts. This means no more than 64K references of each
//  type but that's not much of a limitation and it lets us do our thing atomically
//  without locks. We can just use compare and swap.
//
//  We provide the methods for managing the ref counts cleanly, so the callers don't
//  have to redundantly do that. We tell the caller when he should release this data
//  object.
//
//  Note that we can get a null pointer to manage, so the strong count being non-zero
//  doesn't require that the pointer be non-null. But a zero strong count requires
//  that the pointer be cleaned up and nulled.
//
//  The caller tells our ctor whether this is an initial strong or weak ref and set
//  the counts accordingly.
// ---------------------------------------------------------------------------
template <typename T> class TCntPtrData
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCntPtrData() = delete;

        TCntPtrData(T* const pobjAdopt, const tCIDLib::TBoolean bStrongType) :

            m_c4RefCnt(bStrongType ? 1 : 0x10000)
            , m_pobjData(pobjAdopt)
        {
        }

        TCntPtrData(const TCntPtrData<T>&) = delete;

        ~TCntPtrData()
        {
            //
            //  It's an error if this goes away without a zero count. It means someone
            //  didn't do what they are supposed to.
            //
            CIDAssert(m_c4RefCnt == 0, L"The pointer ref was not zero at data dtor");
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCntPtrData<T>& operator=(const TCntPtrData<T>&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  We bump the weak reference. Nothing else has to happen here but to bump it.
        //  Whether the object still exists at this point is not important. It will become
        //  so later if they try to get a strong ref (when they get a counted pointer from
        //  the weak pointer.) They could still be copying a weak pointer even if the data
        //  is already gone (no strong pointers left), so we'd still need to bump the weak
        //  ref, which keeps this pointer data alive anyway.
        //
        tCIDLib::TVoid AcquireWeakRef()
        {
            tCIDLib::TCard4 c4Comp, c4Result, c4New;
            while (kCIDLib::True)
            {
                c4Comp = m_c4RefCnt;
                CIDAssert((c4Comp >> 16) < kCIDLib::c2MaxCard, L"Weak ref count overflow");

                // Create a new value with a bumped weak count and try it
                c4New = (c4Comp & 0xFFFF) | (((c4Comp >> 16) + 1) << 16);
                c4Result = TAtomic::c4CompareAndExchange(m_c4RefCnt, c4New, c4Comp);

                // If the value didn't change behind our back, we are good
                if (c4Result == c4Comp)
                    break;
            }
        }

        //
        //  We increment the strong ref count if the data is not already gone. We return
        //  the true if we got the ref, which means the data still exists and will now
        //  not go away until the caller releases. If the data is already gone, we return
        //  false.
        //
        //  If we return false here, then a counted pointer being copied or assigned to
        //  from another strong pointer, or a weak pointer being turned into a strong pointer,
        //  will set themselves up with a different pointer data with a null user object, so
        //  this guy cannot get strong owned again once it's released the object.
        //
        tCIDLib::TBoolean bAcquireStrongRef()
        {
            tCIDLib::TCard4 c4Comp, c4Result, c4New;
            while (kCIDLib::True)
            {
                c4Comp = m_c4RefCnt;

                //
                //  If the strong count has gone zero, we are done. Once it goes zero it
                //  stays zero, so no more sync is required to do this. Not that we cannot
                //  assert the pointer is null here since it might not have happened yet,
                //  the thread that zeroed the count could still be working on that.
                //
                if (!(c4Comp & 0xFFFF))
                {
                    return kCIDLib::False;
                }

                // Create a new value with a bumped strong count and try it
                c4New = (c4Comp & 0xFFFF0000) | ((c4Comp & 0xFFFF) + 1);
                c4Result = TAtomic::c4CompareAndExchange(m_c4RefCnt, c4New, c4Comp);

                // If the value didn't change behind our back, we are good
                if (c4Result == c4Comp)
                    break;
            }
            return kCIDLib::True;
        }


        //
        //  If the strong count goes zero, we free the object. We return false if there are
        //  no more refs of either type, which means the caller should free this pointer
        //  data member. If no refs at all, then there can be no way anyone else could do
        //  any more operations on this data unless something is very wrong.
        //
        tCIDLib::TBoolean bReleaseStrongRef()
        {
            tCIDLib::TCard4 c4Comp, c4Result, c4New;
            while (kCIDLib::True)
            {
                // Get the current value
                c4Comp = m_c4RefCnt;
                CIDAssert(c4Comp & 0xFFFF, L"Strong ref count underflow");

                // Create a new value with a strong count one smaller
                c4New = (c4Comp & 0xFFFF0000) | ((c4Comp & 0xFFFF) - 1);
                c4Result = TAtomic::c4CompareAndExchange(m_c4RefCnt, c4New, c4Comp);

                // If the value didn't change behind our back, we are good
                if (c4Result == c4Comp)
                    break;
            }

            //
            //  If the new value has a zero strong count, this guy is done. Once it
            //  goes zero it stays zero, so no need to sync anymore here.
            //
            if (!(c4New & 0xFFFF))
            {
                delete m_pobjData;
                m_pobjData = nullptr;
            }

            // Return true if any counts left
            return (c4New > 0);
        }

        //
        //  Drops the weak count. If both the weak and strong pointers are zero, then
        //  the caller should delete us. We return false if there are no refs at all.
        //
        //  Otherwise, there are still strong refs which will keep both the object and
        //  this pointer data alive.
        //
        //  If both counts are zero there should be no one left to do anything to this
        //  pointer data except the caller.
        //
        tCIDLib::TBoolean bReleaseWeakRef()
        {
            tCIDLib::TCard4 c4Comp, c4Result, c4New;
            while (kCIDLib::True)
            {
                // Get the current value
                c4Comp = m_c4RefCnt;

                // Create a new value with a weak count one smaller
                CIDAssert(c4Comp >> 16, L"Weak ref count underflow");

                c4New = (c4Comp & 0xFFFF) | (((c4Comp >> 16) - 1) << 16);
                c4Result = TAtomic::c4CompareAndExchange(m_c4RefCnt, c4New, c4Comp);

                // If the value didn't change behind our back, we are good
                if (c4Result == c4Comp)
                    break;
            }

            //
            //  If no refs left at all, return false, in which case the caller should
            //  free this pointer data.
            //
            return (c4New > 0);
        }


        //
        //  This is only advisory of course. It could change before the caller does anything
        //  with the value. The weak count is the low 16 bits.
        //
        tCIDLib::TCard4 c4StrongCount() const
        {
            tCIDLib::TCard4 c4Ret = c4RefCount();
            c4Ret &= kCIDLib::c2MaxCard;
            return c4Ret;
        }

        //
        //  This is only advisory of course. It could change before the caller does anything
        //  with the value. The weak count is the high 16 bits.
        //
        tCIDLib::TCard4 c4WeakCount() const
        {
            tCIDLib::TCard4 c4Ret = c4RefCount();
            c4Ret >>= 16;
            return c4Ret;
        }


        T* pobjData()
        {
            return m_pobjData;
        }

        const T* pobjData() const
        {
            return m_pobjData;
        }


    private :
        //
        //  To avoid redundancy in the two methods above that return the strong and weak
        //  counts. They first need to get a clean copy of the current combined value.
        //
        tCIDLib::TCard4 c4RefCount() const
        {
            //
            //  We have to make sure we get a clean value so we get the value and do a
            //  compare and swap with itself until we get the same value back.
            //
            tCIDLib::TCard4 c4Ret;
            while (kCIDLib::True)
            {
                const tCIDLib::TCard4 c4Comp = m_c4RefCnt;
                if (TAtomic::c4CompareAndExchange(m_c4RefCnt, c4Comp, c4Comp) == c4Comp)
                {
                    c4Ret = c4Comp;
                    break;
                }
            }
            return c4Ret;
        }


        volatile mutable tCIDLib::TCard4    m_c4RefCnt;
        T*                                  m_pobjData;
};



// ---------------------------------------------------------------------------
//   CLASS: TCntPtr
//  PREFIX: cptr
// ---------------------------------------------------------------------------
template <typename T> class TCntPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCntPtr() :

            m_pcdRef(new TCntPtrData<T>(nullptr, kCIDLib::True))
        {
        }

        explicit TCntPtr(T* pobjToAdopt) :

            m_pcdRef(new TCntPtrData<T>(pobjToAdopt, kCIDLib::True))
        {
        }

        TCntPtr(const TCntPtr<T>& cptrSrc) :

            m_pcdRef(nullptr)
        {
            CIDAssert(cptrSrc.m_pcdRef != nullptr, L"Source cnt pointer has no pointer data");

            // Acquire a strong ref and take his pointer data
            if (!cptrSrc.m_pcdRef->bAcquireStrongRef())
                TSmartPtrHelpers::CantAcquireStrongRef(CID_LINE);
            m_pcdRef = cptrSrc.m_pcdRef;
        }

        // Become responsible for the object in a unique pointer
        TCntPtr(TUniquePtr<T>& uptrSrc) :

            m_pcdRef(new TCntPtrData<T>(uptrSrc.pOrphan(), kCIDLib::True))
        {
        }

        ~TCntPtr()
        {
            CIDAssert(m_pcdRef != nullptr, L"The cnt pointer dtor has no pointer data");

            TCntPtrData<T>* pcdTmp = m_pcdRef;
            m_pcdRef = nullptr;
            try
            {
                //
                //  Release a strong reference. If that leaves no references then we
                //  delete the pointer data.
                //
                if (!pcdTmp->bReleaseStrongRef())
                    delete pcdTmp;
            }

            catch(...)
            {
                // There's not much we can do. Even logging in this code is dangerous
                #if CID_DEBUG_ON
                TAudio::Beep(880, 100);
                #endif
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        explicit operator tCIDLib::TBoolean() const
        {
            return (m_pcdRef->pobjData() != nullptr);
        }

        tCIDLib::TBoolean operator!() const
        {
            return (m_pcdRef->pobjData() == nullptr);
        }

        T* operator->()
        {
            CheckNullRef(CID_LINE);
            return m_pcdRef->pobjData();
        }

        const T* operator->() const
        {
            CheckNullRef(CID_LINE);
            return m_pcdRef->pobjData();
        }

        T& operator*()
        {
            CheckNullRef(CID_LINE);
            return *m_pcdRef->pobjData();
        }

        const T& operator*() const
        {
            CheckNullRef(CID_LINE);
            return *m_pcdRef->pobjData();
        }

        TCntPtr& operator=(const TCntPtr<T>& cptrSrc)
        {
            if (this != &cptrSrc)
            {
                // Make sure we can get a ref to his before we commit
                if (!cptrSrc.m_pcdRef->bAcquireStrongRef())
                    TSmartPtrHelpers::CantAcquireStrongRef(CID_LINE);

                // Release our strong ref and free it if no more refs
                if (!m_pcdRef->bReleaseStrongRef())
                {
                    delete m_pcdRef;
                    m_pcdRef = nullptr;
                }

                // Now we can store his pointer as ours
                m_pcdRef = cptrSrc.m_pcdRef;
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        // This is only advisory of course
        tCIDLib::TCard4 c4StrongCount() const
        {
            return m_pcdRef->c4StrongCount();
        }

        tCIDLib::TCard4 c4WeakCount() const
        {
            return m_pcdRef->c4WeakCount();
        }

        tCIDLib::TVoid DropRef()
        {
            TCntPtrData<T>* pcdTmp = m_pcdRef;
            m_pcdRef = new TCntPtrData<T>(nullptr, kCIDLib::True);

            // If no more refs of either type left, then clean up the pointer data
            if (!pcdTmp->bReleaseStrongRef())
                delete pcdTmp;
        }

        T* pobjData()
        {
            return m_pcdRef->pobjData();
        }

        const T* pobjData() const
        {
            return m_pcdRef->pobjData();
        }

        tCIDLib::TVoid SetPointer(T* const pobjNew)
        {
            //
            //  Make sure we've got the new guy stored away then release our
            //  old ref via the copy we made.
            //
            TCntPtrData<T>* pcdTmp = m_pcdRef;
            m_pcdRef = new TCntPtrData(pobjNew, kCIDLib::True);

            // If no more refs of either type left, then clean up the pointer data
            if (!pcdTmp->bReleaseStrongRef())
                delete pcdTmp;
        }


    protected :
        // -------------------------------------------------------------------
        //  Friend declaration
        // -------------------------------------------------------------------
        template <typename T> friend class TWeakPtr;

        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------

        //
        //  The weak pointer calls this if he successfully gets a strong ref to
        //  his data. We need to make it our own, and the strong ref he got is
        //  now ours to release.
        //
        //  We take the data via a reference instead of a pointer else it would
        //  conflict with the main public ctor.
        //
        TCntPtr<T>(TCntPtrData<T>& cdToAdopt) :

            m_pcdRef(&cdToAdopt)
        {
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        // Called to make sure the reference is good
        tCIDLib::TVoid CheckNullRef(const tCIDLib::TCard4 c4Line) const
        {
            if ((m_pcdRef == nullptr) || (m_pcdRef->pobjData() == nullptr))
                TSmartPtrHelpers::ThrowNullRef(c4Line, L"TCntPtr");
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcdRef
        //      An instance of our little counted data structure. If it is not null, then
        //      it means we have a valid reference. once the ref count goes zero, we delete
        //      this guy and set this pointer to null.
        // -------------------------------------------------------------------
        TCntPtrData<T>* m_pcdRef;
};


// ---------------------------------------------------------------------------
//   CLASS: TWeakPtr
//  PREFIX: wptr
// ---------------------------------------------------------------------------
template <typename T> class TWeakPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWeakPtr() :

            m_pcdRef(new TCntPtrData<T>(nullptr, kCIDLib::False))
        {
        }

        // We take a weak ref. Doesn't matter if the data is already gone or not
        TWeakPtr(const TCntPtr<T>& cptrSrc) :

            m_pcdRef(nullptr)
        {
            cptrSrc.m_pcdRef->AcquireWeakRef();
            m_pcdRef = cptrSrc.m_pcdRef;
        }

        TWeakPtr(const TWeakPtr<T>& wptrSrc) :

            m_pcdRef(nullptr)
        {
            // Bump the weak ref count and take on his data pointer
            wptrSrc.m_pcdRef->AcquireWeakRef();
            m_pcdRef = wptrSrc.m_pcdRef;
        }

        ~TWeakPtr()
        {
            // Release our weak ref. If no more refs of either type, destroy the pointer data
            if (!m_pcdRef->bReleaseWeakRef())
            {
                delete m_pcdRef;
                m_pcdRef = nullptr;
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWeakPtr<T>& operator=(const TCntPtr<T>& cptrSrc)
        {
            TCntPtrData<T>* pcdOld = m_pcdRef;

            cptrSrc.m_pcdRef->AcquireWeakRef();
            m_pcdRef = cptrSrc.m_pcdRef;

            // Drop our old ref and delete if we were the last
            if (!pcdOld->bReleaseWeakRef())
                delete pcdOld;

            return *this;
        }

        TWeakPtr<T>& operator=(const TWeakPtr<T>& wptrSrc)
        {
            TCntPtrData<T>* pcdOld = m_pcdRef;

            wptrSrc.m_pcdRef->AcquireWeakRef();
            m_pcdRef = wptrSrc.m_pcdRef;

            // Drop our old ref and delete if we were the last
            if (!pcdOld->bReleaseWeakRef())
                delete pcdOld;

            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        // If the strong count is zero, the data is gone
        tCIDLib::TBoolean bExpired() const
        {
            return (m_pcdRef->c4StrongCount() == 0);
        }

        tCIDLib::TCard4 c4StrongCount() const
        {
            return m_pcdRef->c4StrongCount();
        }

        TCntPtr<T> cptrGet()
        {
            //
            //  Try to get a strong ref to the data. If that fails, we just return
            //  a dummy nulled pointer data. Else we give the counted pointer a pointer
            //  to our pointer data.
            //
            if (m_pcdRef->bAcquireStrongRef())
            {
                //
                //  This is a protected method but we are a friend. We pass via a ref
                //  so that it doesn't conflict with the main public ctor.
                //
                return TCntPtr<T>(*m_pcdRef);
            }
            return TCntPtr<T>(nullptr);
        }

        tCIDLib::TVoid DropRef()
        {
            TCntPtrData<T>* pcdOld = m_pcdRef;
            m_pcdRef = nullptr;

            // Drop the old weak ref and delete if we were the last
            if (!pcdOld->bReleaseWeakRef())
                delete pcdOld;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcdRef
        //      Our weak referenced pointer to the pointer data. It may just be to
        //      a dummy one if we were default ctored or have been asked to drop our
        //      reference. Otherwise, we are sharing it with potentially some counted
        //      pointers if they have not all dropped the reference.
        // -------------------------------------------------------------------
        TCntPtrData<T>*     m_pcdRef;
};


// ---------------------------------------------------------------------------
//   CLASS: TMngPtr
//  PREFIX: mptr
// ---------------------------------------------------------------------------
template <class T> class TMngPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMngPtr() :

            m_pData(nullptr)
        {
        }

        explicit TMngPtr(T* const pData) :

            m_pData(pData)
        {
        }

        TMngPtr(const TMngPtr<T>& mptrSrc) :

            m_pData(nullptr)
        {
            m_pData = mptrSrc.m_pData;
        }

        ~TMngPtr()
        {
            // Nothing to do, we just reference the data
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        operator tCIDLib::TBoolean() const
        {
            return (m_pData != nullptr);
        }

        tCIDLib::TBoolean operator!() const
        {
            return (m_pData == nullptr);
        }

        T* operator->()
        {
            return m_pData;
        }

        const T* operator->() const
        {
            return m_pData;
        }

        const T& operator*() const
        {
            CheckNullRef(CID_LINE);
            return *m_pData;
        }

        T& operator*()
        {
            CheckNullRef(CID_LINE);
            return *m_pData;
        }

        TMngPtr& operator=(const TMngPtr<T>& mptrSrc)
        {
            if (this != &mptrSrc)
                m_pData = mptrSrc.m_pData;
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        T& objData()
        {
            CheckNullRef(CID_LINE);
            return *m_pData;
        }

        const T& objData() const
        {
            CheckNullRef(CID_LINE);
            return *m_pData;
        }

        T* pobjData()
        {
            return m_pData;
        }

        const T* pobjData() const
        {
            return m_pData;
        }

        tCIDLib::TVoid SetPointer(T* const pToSet)
        {
            m_pData = pToSet;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtula methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckNullRef(const tCIDLib::TCard4 c4Line) const
        {
            if (!m_pData)
                TSmartPtrHelpers::ThrowNullRef(c4Line, L"TMngPtr");
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      This the pointer to the data
        // -------------------------------------------------------------------
        T*  m_pData;
};


namespace tCIDLib
{
    // -----------------------------------------------------------------------
    //  Helpers to create counted and unique pointers in place
    // -----------------------------------------------------------------------
    template <typename T, typename... TArgs> TCntPtr<T> cptrMakeNew(TArgs&& ...Args)
    {
        return TCntPtr<T>(new T(tCIDLib::Forward<TArgs>(Args)...));
    }

    template <typename T, typename... TArgs> TUniquePtr<T> uptrMakeNew(TArgs&& ...Args)
    {
        return TUniquePtr<T>(new T(tCIDLib::Forward<TArgs>(Args)...));
    }
}

#pragma CIDLIB_POPPACK

