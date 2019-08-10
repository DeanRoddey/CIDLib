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
//   CLASS: TCntPtr
//  PREFIX: cptr
// ---------------------------------------------------------------------------
template <class T> class TCntPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCntPtr() :

            m_pcdRef(nullptr)
        {
        }

        explicit TCntPtr(T* pobjToAdopt) :

            m_pcdRef(new TCntData(pobjToAdopt))
        {
        }

        TCntPtr(const TCntPtr<T>& cptrSrc) :

            m_pcdRef(nullptr)
        {
            // If the caller has a ref, increment the count and store the pointer
            if (cptrSrc.m_pcdRef)
            {
                TAtomic::c4SafeAcquire(cptrSrc.m_pcdRef->m_c4RefCnt);
                m_pcdRef = cptrSrc.m_pcdRef;
            }
        }

        // Become responsible for the object in a unique pointer
        TCntPtr(TUniquePtr<T>& uptrSrc) :

            m_pcdRef(new TCntData(uptrSrc.pOrphan()))
        {
        }

        ~TCntPtr()
        {
           if (m_pcdRef)
            {
                TCntData* pcdTmp = m_pcdRef;
                m_pcdRef = nullptr;
                try
                {
                    ReleaseRef(kCIDLib::True, pcdTmp);
                }

                catch(...)
                {
                    // There's not much we can do. Even logging in this code is dangerous
                    #if CID_DEBUG_ON
                    TAudio::Beep(880, 100);
                    #endif
                }
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        explicit operator tCIDLib::TBoolean() const
        {
            return ((m_pcdRef != nullptr) && (m_pcdRef->m_pobjData != nullptr));
        }

        tCIDLib::TBoolean operator!() const
        {
            return ((m_pcdRef == nullptr) || (m_pcdRef->m_pobjData == nullptr));
        }

        T* operator->()
        {
            CheckNullRef(CID_LINE);
            return m_pcdRef->m_pobjData;
        }

        const T* operator->() const
        {
            CheckNullRef(CID_LINE);
            return m_pcdRef->m_pobjData;
        }

        T& operator*()
        {
            CheckNullRef(CID_LINE);
            return *m_pcdRef->m_pobjData;
        }

        const T& operator*() const
        {
            CheckNullRef(CID_LINE);
            return *m_pcdRef->m_pobjData;
        }

        //
        //  We need to make sure we can get a ref to the new guy before we commit
        TCntPtr& operator=(const TCntPtr<T>& cptrSrc)
        {
            if (this != &cptrSrc)
            {
                // Get a copy of our current ref
                TCntData* pcdSave = m_pcdRef;

                // If the caller has a ref, bump its count
                if (cptrSrc.m_pcdRef)
                    TAtomic::c4SafeAcquire(cptrSrc.m_pcdRef->m_c4RefCnt);

                //
                //  Now we can take his pointer and release ours, via the copy we
                //  made above.
                //
                m_pcdRef = cptrSrc.m_pcdRef;
                ReleaseRef(kCIDLib::False, pcdSave);
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        // This is only advisory of course
        tCIDLib::TCard4 c4RefCount() const
        {
            if (m_pcdRef)
                return m_pcdRef->m_c4RefCnt;
            return 0;
        }

        tCIDLib::TVoid DropRef()
        {
            TCntData* pcdTmp = m_pcdRef;
            m_pcdRef = nullptr;
            ReleaseRef(kCIDLib::False, pcdTmp);
        }

        T* pobjData()
        {
            if (!m_pcdRef)
                return nullptr;
            return m_pcdRef->m_pobjData;
        }

        const T* pobjData() const
        {
            if (!m_pcdRef)
                return nullptr;
            return m_pcdRef->m_pobjData;
        }

        tCIDLib::TVoid SetPointer(T* const pobjNew)
        {
            //
            //  Make sure we've got the new guy stored away then release our
            //  old ref via the copy we made.
            //
            TCntData* pcdTmp = m_pcdRef;
            m_pcdRef = new TCntData(pobjNew);
            ReleaseRef(kCIDLib::False, pcdTmp);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        struct TCntData
        {
            TCntData(T* pobjAdopt) :

                m_c4RefCnt(1)
                , m_pobjData(pobjAdopt)
            {
            }

            volatile tCIDLib::TCard4    m_c4RefCnt;
            T*                          m_pobjData;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        // Called to make sure the reference is good
        tCIDLib::TVoid CheckNullRef(const tCIDLib::TCard4 c4Line) const
        {
            if ((m_pcdRef == nullptr) || (m_pcdRef->m_pobjData == nullptr))
                TSmartPtrHelpers::ThrowNullRef(c4Line, L"TCntPtr");
        }


        //
        //  This is called to release a reference. This is static so we can't do the wrong
        //  thing. The caller should save his counted data, null the member, then pass the
        //  copy to this to be released. That way, his pointer is already null and we work
        //  purely on the copy. Any time he release a reference he is giving up any claim
        //  on the counted object.
        //
        static tCIDLib::TVoid ReleaseRef(const tCIDLib::TBoolean bDtor, TCntData* pcdToRelease)
        {
            if (!pcdToRelease)
                return;

            //
            //  Make sure we don't have a zero ref count here. The safe release below
            //  will catch this also, but not give a good error context.
            //
            #if CID_DEBUG_ON
            TSmartPtrHelpers::CheckRefNotZero(CID_LINE, pcdToRelease->m_c4RefCnt, L"TCntPtr");
            #endif

            if (!TAtomic::c4SafeRelease(pcdToRelease->m_c4RefCnt))
            {
                TJanitor<T> janObj(pcdToRelease->m_pobjData);
                delete pcdToRelease;
            }
        }

        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcdRef
        //      An instance of our little counted data structure. If it is not null, then
        //      it means we have a valid reference. once the ref count goes zero, we delete
        //      this guy and set this pointer to null.
        // -------------------------------------------------------------------
        TCntData*   m_pcdRef;
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

#pragma CIDLIB_POPPACK

