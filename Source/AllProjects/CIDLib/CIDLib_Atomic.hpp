//
// FILE NAME: CIDLib_Atomic.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/20/2019
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
//  This name space provides some more convenient wrappers around some of the
//  low level atomic operations in TRawMem. Those cannot throw exceptions
//  because they don't know about such things. Really low level code should use
//  the CIDKernel versions. But other stuff can use these.
//
//  We also define a singleton class that will lazily fault in an object. By
//  default it can only default construct the object, but you can do a derivative
//  if needed to allocate it yourself.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace TAtomic
{
    inline tCIDLib::TCard4
    c4CompareAndExchange(       tCIDLib::TCard4&    c4ToFill
                        , const tCIDLib::TCard4     c4New
                        , const tCIDLib::TCard4     c4Compare)
    {
        return TRawMem::c4CompareAndExchange(c4ToFill, c4New, c4Compare);
    }

    inline tCIDLib::TCard4
    c4Exchange(tCIDLib::TCard4& c4ToFill, const tCIDLib::TCard4 c4New)
    {
        return TRawMem::c4Exchange(c4ToFill, c4New);
    }

    CIDLIBEXP tCIDLib::TCard4 c4SafeAcquire
    (
                    tCIDLib::TCard4&    c4Target
    );

    CIDLIBEXP tCIDLib::TCard4 c4SafeRelease
    (
                    tCIDLib::TCard4&    c4Target
    );


    template <typename T> T* pExchangePtr(T** ppToFill, T* const pNew)
    {
        return reinterpret_cast<T*>
        (
            TRawMem::pExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(ppToFill), pNew
            )
        );
    }

    template <typename T> T* pCompareAndExchangePtr(T**        ppToFill
                                                , T* const  pNew
                                                , T* const  pCompare)
    {
        return reinterpret_cast<T*>
        (
            TRawMem::pCompareAndExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(ppToFill), pNew, pCompare
            )
        );
    }


    //
    //  These play a trick with compare and exchange, so that we attomically read the value
    //  if it is set, else we store a null, replacing the null that was already there.
    //
    template <typename T> T* pFencedGet(T** pToGet)
    {
        // Sometimes the value is a void pointer which causes an unnecessary reinterpret warning
        CIDLib_Suppress(26471)
        return reinterpret_cast<T*>
        (
            TRawMem::pCompareAndExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(pToGet), nullptr, nullptr
            )
        );
    }

    template <typename T> T* pFencedGet(const T** pToGet)
    {
        // Sometimes the value is a void pointer which causes an unnecessary reinterpret warning
        CIDLib_Suppress(26471)
        return reinterpret_cast<T*>
        (
            TRawMem::pCompareAndExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(pToGet), nullptr, nullptr
            )
        );
    }

    //
    //  Atomically swap in a new value to the target. Returns the previous value
    template <typename T> T* pFencedSet(T** pTarget, T* const pToSet)
    {
        // Sometimes the value is a void pointer which causes an unnecessary reinterpret warning
        CIDLib_Suppress(26471)
        return reinterpret_cast<T*>
        (
            TRawMem::pExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(pTarget), pToSet
            )
        );
    }

    template <typename T> T* pFencedSet(const T** pTarget, T* const pToSet)
    {
        // Sometimes the value is a void pointer which causes an unnecessary reinterpret warning
        CIDLib_Suppress(26471)
        return reinterpret_cast<T*>
        (
            TRawMem::pExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(pTarget), pToSet
            )
        );
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TSingleton
//  PREFIX: single
// ---------------------------------------------------------------------------
template <typename T> class TSingleton
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TSingleton() : m_ptInstance(nullptr)
        {
        }

        TSingleton(const TSingleton&) = delete;
        TSingleton(TSingleton&&) = delete;
        TSingleton& operator=(const TSingleton&) = delete;
        TSingleton& operator=(TSingleton&&) = delete;

        virtual ~TSingleton()
        {
            //
            //  We don't destroy it. These are used for globals and order of
            //  destruction isn't under our control.
            //
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        const T* operator->() const
        {
            if (!TAtomic::pFencedGet(&m_ptInstance))
                FaultIn();
            return m_ptInstance;
        }

        T* operator->()
        {
            if (!TAtomic::pFencedGet(&m_ptInstance))
                FaultIn();
            return m_ptInstance;
        }

        const T& operator*() const
        {
            if (!TAtomic::pFencedGet(&m_ptInstance))
                FaultIn();
            return *m_ptInstance;
        }

        T& operator*()
        {
            // If alreayd set, we are done
            if (!TAtomic::pFencedGet(&m_ptInstance))
                FaultIn();
            return *m_ptInstance;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------

        // If not overridden, then just default construct a T
        virtual T* ptMakeIt()
        {
            return new T{};
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  We lock our sync section, then check again if it exists. If not, then
        //  default construct one and store it.
        //
        tCIDLib::TVoid FaultIn()
        {
            TKrnlCritSecLocker kcrslSync(pkcrsGetLock());
            if (!TAtomic::pFencedGet(&m_ptInstance))
                TAtomic::pFencedSet(&m_ptInstance, this->ptMakeIt());
        }


        //
        //  We have to have a way to sync the faulting in of the object because it may
        //  not be kosher to create more than one and a lockless scheme would require
        //  speculative creation of an object that might not end up being usuable. This
        //  at least we fault in a lockless way to provide a bootstrapping mechanism.
        //
        static TKrnlCritSec* pkcrsGetLock()
        {
            if (!TAtomic::pFencedGet(&s_pkcrsLock))
            {
                TKrnlCritSec* pkcrsNew = new TKrnlCritSec;

                // If someone else beat us to it, destroy ours
                if ( TRawMem::pCompareAndExchangePtr(&s_pkcrsLock, pkcrsNew, nullptr) != nullptr);
                    delete pkcrsNew;
            }
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ptInstance
        //      The pointer to our instance object we fault in. It has to be cache
        //      aligned. Has to be mutable so we can fault it in from const methods.
        // -------------------------------------------------------------------
        mutable T* alignas(kCIDLib::c4CacheAlign) m_ptInstance;

        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_pkcrsLock
        //      See the comments on pkcrsGetLock() above.
        // -------------------------------------------------------------------
        static TKrnlCritSec*    s_pkcrsLock;
};

