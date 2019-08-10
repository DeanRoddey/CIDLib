//
// FILE NAME: CIDLib_ObjLocker.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/09/2019
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
//  This is the header for the CIDLib_ObjLocker.Cpp file. It implements the
//  TObjLocker class, which is a templatized class that allows for exclusive
//  access to a contained object. The object is not directly accessible, it
//  can only be gotten by calling objLock(), which will return a TObjLock
//  object that will hold exclusive access to the object until it destructs.
//  The object lock is effectively a unique type pointer, so it can be copied
//  and assigned, but there will only be one at any time and it will retain the
//  lock.
//
//  It doesn't work on an existing object, since that would make it less safe.
//  It constructs the object in place as a member.
//
//  Because of how these guys work, we can't allow recursive locking. So we keep
//  a locked flag and, if we get the lock and then see it's already set, we unlock
//  and throw an already locked error. It has to be the same thread or we couldn't
//  have gotten the lock again, so it's safe to unlock it before we throw.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

 template <typename T> class TObjLocker;

// ---------------------------------------------------------------------------
//  CLASS: TObjLock
// PREFIX: olock
// ---------------------------------------------------------------------------
template <typename T> class TObjLock
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TObjLock() = delete;
        TObjLock(const TObjLock&) = delete;

        TObjLock(TObjLock&& olockSrc) :

            m_pbLocked(nullptr)
            , m_pmtxLock(nullptr)
            , m_pobjLocked(nullptr)
        {
            *this = tCIDLib::ForceMove(olockSrc);
        }

        ~TObjLock()
        {
            // If we stil own it, then unlock
            if (m_pmtxLock)
            {
                // Clear the flag before we unlock
                *m_pbLocked = kCIDLib::False;
                m_pmtxLock->Unlock();
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TObjLock& operator=(const TObjLock&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;

        TObjLock& operator=(TObjLock&& olockSrc)
        {
            if (this != &olockSrc)
            {
                tCIDLib::Swap(m_pbLocked, olockSrc.m_pbLocked);
                tCIDLib::Swap(m_pmtxLock, olockSrc.m_pmtxLock);
                tCIDLib::Swap(m_pobjLocked, olockSrc.m_pobjLocked);
            }
            return *this;
        }

        explicit operator tCIDLib::TBoolean () const
        {
            return (m_pobjLocked != nullptr);
        }

        T& operator*()
        {
            if (!m_pobjLocked)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TObjLock");
            return *m_pobjLocked;
        }

        const T& operator*() const
        {
            if (!m_pobjLocked)
                TSmartPtrHelpers::ThrowNullRef(CID_LINE, L"TObjLock");
            return *m_pobjLocked;
        }

        T* operator->()
        {
            return m_pobjLocked;
        }

        const T* operator->() const
        {
            return m_pobjLocked;
        }

    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        template<typename T> friend class TObjLocker;


        // -------------------------------------------------------------------
        //  A hidden constructor for our owning locker. He's a friend.
        // -------------------------------------------------------------------
        TObjLock(TMutex* const              pmtxLock
                , T* const                  pobjLocked
                , tCIDLib::TBoolean* const  pbLocked) :

            m_pbLocked(pbLocked)
            , m_pmtxLock(pmtxLock)
            , m_pobjLocked(pobjLocked)
        {
        }

    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pbLocked
        //      We clear the locker's locked flag when we release.
        //
        //  m_pmtxLock
        //      A pointer to the mutex that our owning locker is using to lock
        //      access to the object. It's already locked when we get it, we just
        //      have to unlock it on dtor.
        //
        //  m_pobjLocked
        //      A pointer to the object that we have locked
        // -------------------------------------------------------------------
        tCIDLib::TBoolean*  m_pbLocked;
        TMutex*             m_pmtxLock;
        T*                  m_pobjLocked;
};



// ---------------------------------------------------------------------------
//  CLASS: TObjLocker
// PREFIX: olockr
// ---------------------------------------------------------------------------
template <typename T> class TObjLocker
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        template <typename... TArgs> TObjLocker(TArgs&&... Args) :

            m_bLocked(kCIDLib::False)
            , m_objLocked(tCIDLib::Forward<TArgs>(Args)...)
        {
        }

        TObjLocker() = delete;
        TObjLocker(const TObjLocker&) = delete;
        TObjLocker(TObjLocker&&) = delete;

        ~TObjLocker() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TObjLocker& operator=(const TObjLocker&) = delete;
        TObjLocker& operator=(TObjLocker&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  We lock first so we know we have the object and can create the lock object
        //  and return it. The lock object unlocks when the last instance dies.
        //
        //  The try version will return a nulled lobk object if it cannot get the
        //  lock.
        //
        TObjLock<T> olockTryGet(const tCIDLib::TCard4 c4Millis)
        {
            if (m_mtxLock.bTryLock(c4Millis))
            {
                if (m_bLocked)
                {
                    m_mtxLock.Unlock();
                    TSmartPtrHelpers::ThrowAlreadyLocked(CID_LINE, L"TObjLocker");
                }
                m_bLocked = kCIDLib::True;
                return TObjLock<T>(&m_mtxLock, &m_objLocked, &m_bLocked);
            }

            return TObjLock<T>(nullptr, nullptr, nullptr);
        }


        TObjLock<T> olockGet(const tCIDLib::TCard4 c4Millis)
        {
            m_mtxLock.Lock(c4Millis);
            if (m_bLocked)
            {
                m_mtxLock.Unlock();
                TSmartPtrHelpers::ThrowAlreadyLocked(CID_LINE, L"TObjLocker");
            }
            m_bLocked = kCIDLib::True;
            return TObjLock<T>(&m_mtxLock, &m_objLocked, &m_bLocked);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bLocked
        //      This is set while we are locked, so that we can reject recursive
        //      locks.
        //
        //  m_mtxLock
        //      The mutex we use to lock the object.
        //
        //  m_objLocked
        //      The object we contain and are locking.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bLocked;
        TMutex              m_mtxLock;
        T                   m_objLocked;
};

#pragma CIDLIB_POPPACK
