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
//  1. Counted/Weak - These are analgous to the shared/weak pointers of the standard
//  libraries, and work similarly. You can provide a custom deleter to the counted
//  pointer (the default is to just call delete.)
//
//  2. Managed - The are just simple, non-counting wrappers that don't delete. They
//  just let you pass around a pointer in a value semantics sort of way, but ultimately
//  someone else has to clean the data up.
//
//  3. Member - These are for classes that need to allocate members but otherwise
//  want to treat them as by value objects. So this type of pointer imposes copy,
//  assign, and move semantics on the pointer. The default is to use delete and the
//  copy constructor, but you can provide a custom deleter and copier.
//
//  4. Unique = Similar to the standard library unique pointers. They don't allow copy
//  or assign, just move and give away the pointer to the next one in line. The last
//  one that has when it destructs will clean up the object. The default is to just
//  call delete but you can provide a custom deleter.
//
//
//  The latter ones self contained but the counted/weak ones need to share a couple
//  helper classes. There is a base class that manages the object pointer and the
//  weak/strong reference counts. It provides methods for inc/dec of either count,
//  and tells the caller when it's time to delete the helper object (when there are
//  no more weak or strong pointers.) It has a derivative that takes a custom deleter
//  object, but weak/strong ptr classes always look at it via the base class so that
//  they are not tied to the deleter type.
//
//  The refs are stored in a single 32 bit value so that we can do all our all ref
//  count management atomically. The high 16 bits is the weak count and the low 16 is
//  the strong count. That means a max of 64K refs of either type, but that's pretty
//  reasonable for the benefits and simplicity this scheme provides.
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

    CIDLIBEXP tCIDLib::TVoid RefCountRange
    (
        const   tCIDLib::TCard4         c4Line
        , const tCIDLib::TBoolean       bStrong
        , const tCIDLib::TBoolean       bOver
        , const tCIDLib::TCh* const     pszPtrType
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

    CIDLIBEXP tCIDLib::TVoid ThrowRefCountUnderflow
    (
        const   tCIDLib::TCard4         c4Line
        , const tCIDLib::TCh* const     pszType
    );


    // Default deleter and copier handlers
    template <typename T> struct TDefDeleter
    {
        tCIDLib::TVoid operator()(T* pT)
        {
            delete pT;
        }
    };

    template <typename T> struct TDefCopier
    {
        T* operator()(T* pT)
        {
            return new T(*pT);
        }
    };
}

template <typename T> class TWeakPtr;


// ---------------------------------------------------------------------------
//   CLASS: TUniquePtr
//  PREFIX: uptr
// ---------------------------------------------------------------------------
template <typename T> class TUniquePtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TUniquePtr() :

            m_pdpInfo(new TDataPtr<T>(nullptr))
        {
        }

        explicit TUniquePtr(T* const pobjAdopt) :

            m_pdpInfo(new TDataPtr<T>(pobjAdopt))
        {
        }

        template <typename TDeleter> TUniquePtr(T* const pobjAdopt, TDeleter fnDel) :

            m_pdpInfo(new TDataPtrDel<T, TDeleter>(pobjAdopt, fnDel))
        {
        }

        TUniquePtr(const TUniquePtr<T>&) = delete;

        // We gen up a default data pointer object with a null user object to give away
        TUniquePtr(TUniquePtr<T>&& uptrSrc) :

            m_pdpInfo(new TDataPtr<T>(nullptr))
        {
            *this = tCIDLib::ForceMove(uptrSrc);
        }

        ~TUniquePtr()
        {
            if (m_pdpInfo)
            {
                // Destroy the user data if we still have it, then our data
                TJanitor<TDataPtr<T>> janData(m_pdpInfo);
                m_pdpInfo = nullptr;
                janData.pobjThis()->DestroyData();
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUniquePtr<T>& operator=(const TUniquePtr<T>&) = delete;

        TUniquePtr<T>& operator=(TUniquePtr<T>&& uptrSrc)
        {
            if (&uptrSrc != this)
                tCIDLib::Swap(m_pdpInfo, uptrSrc.m_pdpInfo);
            return *this;
        }

        operator tCIDLib::TBoolean() const
        {
            return (m_pdpInfo->m_pobjData != nullptr);
        }

        tCIDLib::TBoolean operator!() const
        {
            return (m_pdpInfo->m_pobjData == nullptr);
        }

        T* operator->()
        {
            return m_pdpInfo->m_pobjData;
        }

        const T* operator->() const
        {
            return m_pdpInfo->m_pobjData;
        }

        const T& operator*() const
        {
            if (!m_pdpInfo->m_pobjData)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TUniquePtr");
            return *m_pdpInfo->m_pobjData;
        }

        T& operator*()
        {
            if (!m_pdpInfo->m_pobjData)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TUniquePtr");
            return *m_pdpInfo->m_pobjData;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DropRef()
        {
            //
            //  If we still have a valid pointer, then we own it. We replace it with a
            //  default data pointer object with a null user data pointer.
            //
            if (m_pdpInfo->m_pobjData)
            {
                TJanitor<TDataPtr<T>> janData(m_pdpInfo);
                m_pdpInfo = new TDataPtr<T>(nullptr);

                janData.pobjThis()->DestroyData();
            }
        }

        //
        //  We give away our object and set a null in its place. Note that the caller
        //  must be aware of custom deleter issues here. If it was set with a customer
        //  deleter and he gets it out and calls delete on it, it might not work.
        //
        [[nodiscard]] T* pOrphan()
        {
            T* pobjRet = m_pdpInfo->m_pobjData;
            m_pdpInfo->m_pobjData = nullptr;
            return pobjRet;
        }


    private :
        // -------------------------------------------------------------------
        //  Private types
        //
        //  We need a base class to hold our object and a derived class that can hold
        //  a deleter. Our base class is the default and just calls delete on it.
        // -------------------------------------------------------------------
        template <typename T2> struct TDataPtr
        {
            public :
                TDataPtr(T2* const pobjAdopt) : m_pobjData(pobjAdopt) {}
                virtual ~TDataPtr() {}

                virtual tCIDLib::TVoid DestroyData()
                {
                    delete m_pobjData;
                }

                T2*  m_pobjData;
        };

        template <typename T2, typename TDeleter> struct TDataPtrDel : public TDataPtr<T>
        {
            TDataPtrDel(T2* const pobjAdopt, TDeleter fnDelete) :

                TDataPtr<T2>(pobjAdopt)
                , m_fnDeleter(fnDelete)
            {
            }

            tCIDLib::TVoid DestroyData() final
            {
                m_fnDeleter(this->m_pobjData);
            }

            TDeleter    m_fnDeleter;
        };


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pdpInfo
        //      This will be either our base pointer or the derived deleter oriented
        //      one, depending on how we were constructed. It is never null. If we
        //      drop our ref, we create a new base one that holds a null pointer.
        // -------------------------------------------------------------------
        TDataPtr<T>*    m_pdpInfo;
};





// ---------------------------------------------------------------------------
//  This is really just for internal use, but it needs to be shared by both counted
//  and weak pointers.
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

        virtual ~TCntPtrData()
        {
            //
            //  It's an error if this goes away without a zero count or a nulled data
            //  pointer. It means someone didn't do what they are supposed to.
            //
            CIDAssert(m_c4RefCnt == 0, L"The pointer ref count was not zero at data dtor");
            CIDAssert(m_pobjData == nullptr, L"The pointer was not zero at data dtor");
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
                if ((c4Comp >> 16) == 0xFFFF)
                    TSmartPtrHelpers::RefCountRange(CID_LINE, kCIDLib::False, kCIDLib::True, L"weak");

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
        //  true if we got the ref, which means the data still exists and will now
        //  not go away until the caller releases. If the data is already gone, we return
        //  false.
        //
        //  If we return false here, then a counted pointer being copied or assigned to
        //  from another strong pointer, or a weak pointer being turned into a strong pointer,
        //  will set themselves up with a default pointer data with a null user object, so
        //  this guy cannot get strong owned again once it's released the object.
        //
        tCIDLib::TBoolean bAcquireStrongRef()
        {
            tCIDLib::TCard4 c4Comp = 0, c4Result = 0, c4New = 0;
            while (kCIDLib::True)
            {
                c4Comp = m_c4RefCnt;

                //
                //  If the strong count has gone zero, we are done. Once it goes zero it
                //  stays zero, so no more sync is required to do this.
                //
                //  NOTE that we cannot assert the pointer is null here since it might not
                //  have happened yet, the thread that zeroed the count could still be
                //  working on that.
                //
                if ((c4Comp & 0xFFFF) == 0)
                    return kCIDLib::False;

                // Make sure we aren't overflowing the counter
                if ((c4Comp & 0xFFFF) == 0xFFFF)
                    TSmartPtrHelpers::RefCountRange(CID_LINE, kCIDLib::True, kCIDLib::True, L"counted");

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
        //  If the strong count goes zero, we free the user object. We return false if there
        //  are no more refs of either type, which means the caller should free us as well.
        //  If no refs at all, then there can be no way anyone else could do any more
        //  operations on this data unless something is very wrong. The caller is the last
        //  man standing.
        //
        tCIDLib::TBoolean bReleaseStrongRef()
        {
            tCIDLib::TCard4 c4Comp = 0, c4Result = 0, c4New = 0;
            while (kCIDLib::True)
            {
                // Get the current value and make sure we aren't going to underflow
                c4Comp = m_c4RefCnt;
                if ((c4Comp & 0xFFFF) == 0)
                    TSmartPtrHelpers::RefCountRange(CID_LINE, kCIDLib::True, kCIDLib::False, L"counted");

                // Create a new value with a strong count one smaller
                c4New = (c4Comp & 0xFFFF0000) | ((c4Comp & 0xFFFF) - 1);
                c4Result = TAtomic::c4CompareAndExchange(m_c4RefCnt, c4New, c4Comp);

                // If the value didn't change behind our back, we are good
                if (c4Result == c4Comp)
                    break;
            }

            //
            //  If the new value has a zero strong count, the user object is done.
            //  Once it goes zero it stays zero, so no need to sync anymore here.
            //  It couldn't have already been zero, since we would have gotten an
            //  underflow error above.
            //
            if (!(c4New & 0xFFFF))
            {
                try
                {
                    DestroyUserData(m_pobjData);
                    m_pobjData = nullptr;
                }

                catch(...)
                {
                    m_pobjData = nullptr;
                }
            }

            //
            //  If no refs left at all, return false, in which case the caller should
            //  free this pointer data.
            //
            return (c4New > 0);
        }

        //
        //  Drops the weak count. If both the weak and strong pointers are zero, then
        //  the caller should delete us. We return false if there are no refs at all.
        //  Otherwise, there's still someone hanging on.
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

                if ((c4Comp >> 16) == 0)
                    TSmartPtrHelpers::RefCountRange(CID_LINE, kCIDLib::False, kCIDLib::False, L"weak");

                // Create a new value with a weak count one smaller
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
            c4Ret &= 0xFFFF;
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


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid DestroyUserData(T* const pobjDestroy) = 0;


    private :
        //
        //  To avoid redundancy in the methods above that need to get the current count
        //  values out and aren't themslves doing a c/x, we provide this helper, which
        //  insures they get a clean copy of the ref count member.
        //
        tCIDLib::TCard4 c4RefCount() const
        {
            //
            //  We have to make sure we get a clean value so we get the value and do a
            //  compare and swap with itself until we get the same value back.
            //
            tCIDLib::TCard4 c4Ret = 0;
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

        mutable tCIDLib::TCard4 m_c4RefCnt;
        T*                      m_pobjData;
};

template <typename T, typename TDeleter> class TCntPtrDataDel : public TCntPtrData<T>
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCntPtrDataDel() = delete;

        TCntPtrDataDel(         T* const            pobjAdopt
                        , const tCIDLib::TBoolean   bStrongType
                        ,       TDeleter            fnDeleter) :

            TCntPtrData<T>(pobjAdopt, bStrongType)
            , m_fnDeleter(fnDeleter)
        {
        }

        TCntPtrDataDel(const TCntPtrDataDel<T,TDeleter>&) = delete;

        ~TCntPtrDataDel() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCntPtrDataDel<T,TDeleter>& operator=(const TCntPtrDataDel<T,TDeleter>&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DestroyUserData(T* const pobjDestroy) final
        {
            m_fnDeleter(pobjDestroy);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_fnDeleter
        //      A custom deleter provided by the client code.
        // -------------------------------------------------------------------
        TDeleter    m_fnDeleter;
};

template <typename T> class TCntPtrDataDefDel : public TCntPtrData<T>
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCntPtrDataDefDel() = delete;

        TCntPtrDataDefDel(T* const pobjAdopt, const tCIDLib::TBoolean bStrongType) :

            TCntPtrData<T>(pobjAdopt, bStrongType)
        {
        }

        TCntPtrDataDefDel(const TCntPtrDataDefDel<T>&) = delete;

        ~TCntPtrDataDefDel() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCntPtrDataDefDel<T>& operator=(const TCntPtrDataDefDel<T>&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DestroyUserData(T* const pobjDestroy) final
        {
            m_fnDeleter(pobjDestroy);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_fnDeleter
        //      We just have a fixed default deleter, which just calls delete on the
        //      user object.
        // -------------------------------------------------------------------
        TSmartPtrHelpers::TDefDeleter<T>    m_fnDeleter;
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

            m_pcdRef(new TCntPtrDataDefDel<T>(nullptr, kCIDLib::True))
        {
        }

        explicit TCntPtr(T* pobjToAdopt) :

            m_pcdRef(new TCntPtrDataDefDel<T>(pobjToAdopt, kCIDLib::True))
        {
        }

        template <typename TDeleter> TCntPtr(T* pobjToAdopt, TDeleter fnDeleter) :

            m_pcdRef(new TCntPtrDataDel<T,TDeleter>(pobjToAdopt, kCIDLib::True, fnDeleter))
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
            m_pcdRef = new TCntPtrDataDefDel<T>(nullptr, kCIDLib::True);

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
            m_pcdRef = new TCntPtrDataDefDel<T>(pobjNew, kCIDLib::True);

            // If no more refs of either type left, then clean up the pointer data
            if (!pcdTmp->bReleaseStrongRef())
                delete pcdTmp;
        }

        template <typename TDeleter>
        tCIDLib::TVoid SetPointer(T* const pobjNew, TDeleter fnDeleter)
        {
            //
            //  Make sure we've got the new guy stored away then release our
            //  old ref via the copy we made.
            //
            TCntPtrData<T>* pcdTmp = m_pcdRef;
            m_pcdRef = new TCntPtrDataDel<T,TDeleter>(pobjNew, kCIDLib::True, fnDeleter);

            // If no more refs of either type left, then clean up the pointer data
            if (!pcdTmp->bReleaseStrongRef())
                delete pcdTmp;
        }


    protected :
        // -------------------------------------------------------------------
        //  Friend declaration
        // -------------------------------------------------------------------
        template <typename T2> friend class TWeakPtr;


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
        //      An instance of our little counted data structure. It is never null.
        //      If the old one is dropped we allocate a new, default one with a
        //      null data pointer in it.
        //
        //      We store it via the base class because we cannot be tied to the
        //      deleter type.
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

        // Just set up an already dead null pointer with a single weak ref
        TWeakPtr() :

            m_pcdRef(new TCntPtrDataDefDel<T>(nullptr, kCIDLib::False))
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

        TWeakPtr(TWeakPtr<T>&& wptrSrc) :

            m_pcdRef(new TCntPtrDataDefDel<T>(nullptr, kCIDLib::False))
        {
            *this = tCIDLib::ForceMove(wptrSrc);
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

        TWeakPtr<T>& operator=(TWeakPtr<T>&& wptrSrc)
        {
            if (&wptrSrc != this)
                tCIDLib::Swap(m_pcdRef, wptrSrc.m_pcdRef);
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
            m_pcdRef = new TCntPtrDataDefDel<T>(nullptr, kCIDLib::False);

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
template <typename T> class TMngPtr
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


// ---------------------------------------------------------------------------
//   CLASS: TMemberPtr
//  PREFIX: mbptr
// ---------------------------------------------------------------------------
template <typename T> class TMemberPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TMemberPtr() :

            m_pdpInfo(new TDataPtrDefDC<T>(nullptr))
        {
        }

        explicit TMemberPtr(T* const pobjAdopt) :

            m_pdpInfo(new TDataPtrDefDC<T>(pobjAdopt))
        {
        }

        // Let them set custom copy/delete handlers
        template <typename TCopy, typename TDel>
        TMemberPtr(T* const pobjAdopt, TCopy fnCopy, TDel fnDelete) :

            m_pdpInfo(new TDataPtrCustDC<T,TCopy,TDel>(pobjAdopt, fnCopy, fnDelete))
        {
        }

        //
        //  Duplicate the caller's pointer data, which gets us the data and the
        //  copy/delete handlers.
        //
        TMemberPtr(const TMemberPtr<T>& mbptrSrc) :

            m_pdpInfo(mbptrSrc.m_pdpInfo->pdpDuplicate())
        {
        }

        // We just gen up a default one with null data to give away
        TMemberPtr(TMemberPtr<T>&& mbptrSrc) :

            m_pdpInfo(new TDataPtrDefDC<T>(nullptr))
        {
            *this = tCIDLib::ForceMove(mbptrSrc);
        }

        ~TMemberPtr()
        {
            ReleaseData(kCIDLib::False);
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------

        //
        //  Drop our current data and duplicate the caller's pointer data, which
        //  gets us the data and the copy/delete handlers.
        //
        TMemberPtr<T>& operator=(const TMemberPtr<T>& mbptrSrc)
        {
            if (&mbptrSrc != this)
            {
                ReleaseData(kCIDLib::False);
                m_pdpInfo = mbptrSrc->pdpDuplicate();
            }
            return *this;
        }

        TMemberPtr<T>& operator=(TMemberPtr<T>&& mbptrSrc)
        {
            if (&mbptrSrc != this)
                tCIDLib::Swap(m_pdpInfo, mbptrSrc.m_pdpInfo);
            return *this;
        }

        operator tCIDLib::TBoolean() const
        {
            return (m_pdpInfo->m_pobjData != nullptr);
        }

        tCIDLib::TBoolean operator!() const
        {
            return (m_pdpInfo->m_pobjData == nullptr);
        }

        T* operator->()
        {
            return m_pdpInfo->m_pobjData;
        }

        const T* operator->() const
        {
            return m_pdpInfo->m_pobjData;
        }


        const T& operator*() const
        {
            if (!m_pdpInfo->m_pobjData)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TMemberPtr");
            return *m_pdpInfo->m_pobjData;
        }

        T& operator*()
        {
            if (!m_pdpInfo->m_pobjData)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TMemberPtr");
            return *m_pdpInfo->m_pobjData;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DropRef()
        {
            //
            //  Release our current data if we have any, telling it to allocate a
            //  dummy new one with null user data.
            //
            ReleaseData(kCIDLib::True);
        }

        [[nodiscard]] T* pOrphan()
        {
            T* pobjRet = m_pdpInfo->m_pobjData;
            m_pdpInfo->m_pobjData = nullptr;
            return pobjRet;
        }

        // Drop any current data and create a new default handler for the new data
        tCIDLib::TVoid SetPointer(T* const pobjAdopt)
        {
            ReleaseData(kCIDLib::False);
            m_pdpInfo = new TDataPtrDefDC<T>(pobjAdopt);
        }

        // For this one, let them set their own copy/delete handlers
        template <typename TCopy, typename TDel>
        tCIDLib::TVoid SetPointer(T* const pobjAdopt, TCopy fnCopy, TDel fnDelete)
        {
            ReleaseData(kCIDLib::False);
            m_pdpInfo = new TDataPtrCustDC<T,TCopy,TDel>(pobjAdopt, fnCopy, fnDelete);
        }


    private :
        // -------------------------------------------------------------------
        //  Private types
        //
        //  We need a base class to hold our object and a derived class that can hold
        //  a deleter. Our base class is the default and just called delete on it.
        // -------------------------------------------------------------------
        template <typename T2> struct TDataPtr
        {
            TDataPtr(T2* const pobjAdopt) : m_pobjData(pobjAdopt) {}
            virtual TDataPtr<T2>* pdpDuplicate() = 0;
            virtual tCIDLib::TVoid DestroyData() = 0;

            T2*  m_pobjData;
        };

        template <typename T2>
        struct TDataPtrDefDC : public TDataPtr<T2>
        {
            TDataPtrDefDC(T2* const pobjAdopt) : TDataPtr<T2>(pobjAdopt)
            {
            }

            // Duplicate ourself and our contained data
            [[nodiscard]] TDataPtr<T2>* pdpDuplicate() final
            {
                return new TDataPtrDefDC<T2>
                (
                    this->m_pobjData ? new T2(*this->m_pobjData) : nullptr
                );
            }

            tCIDLib::TVoid DestroyData() final
            {
                delete this->m_pobjData;
            }
        };

        template <typename T2, typename TCopier, typename TDeleter>
        struct TDataPtrCustDC : public TDataPtr<T2>
        {
            TDataPtrCustDC(T2* const pobjAdopt, TCopier fnCopy, TDeleter fnDelete) :

                TDataPtr<T2>(pobjAdopt)
                , m_fnCopier(fnCopy)
                , m_fnDeleter(fnDelete)
            {
            }

            // Duplicate ourself and our contained data
            [[nodiscard]] TDataPtr<T2>* pdpDuplicate() final
            {
                return new TDataPtrCustDC<T2, TCopier, TDeleter>
                (
                    this->m_pobjData ? m_fnCopier(this->m_pobjData) : nullptr
                    , m_fnCopier
                    , m_fnDeleter
                );
            }

            tCIDLib::TVoid DestroyData() final
            {
                m_fnDeleter(this->m_pobjData);
            }

            TCopier     m_fnCopier;
            TDeleter    m_fnDeleter;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ReleaseData(const tCIDLib::TBoolean bSetNull)
        {
            if (m_pdpInfo)
            {
                try
                {
                    // Destroy the user data if we still have it, then our data
                    m_pdpInfo->DestroyData();
                    delete m_pdpInfo;

                    if (bSetNull)
                        m_pdpInfo = new TDataPtrDefDC<T>(nullptr);
                    else
                        m_pdpInfo = nullptr;
                }

                catch(...)
                {
                }
            }
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pdpInfo
        //      This will be either our base pointer or the derived deleter oriented
        //      one, depending on how we were constructed. It is never null. If we
        //      drop our ref, ew create a new one that holds a null pointer.
        // -------------------------------------------------------------------
        TDataPtr<T>*    m_pdpInfo;
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

