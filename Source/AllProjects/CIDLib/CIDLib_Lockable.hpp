//
// FILE NAME: CIDLib_Lockable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/25/2019
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
//  This is the header for the CIDLib_Lockable.Cpp file. This file defines a
//  mixin interface that allows generic locking and unlocking of things. So
//  thinkgs like mutexes and critical sections implement this.
//
//  We also define a scoped locker that can lock anything that impelments the
//  lockable interface. It is tolerant of the lockable object being null, so that
//  we can support optional locking.
//
//  In the CIDLib_Mutex.hpp file it defines an incredibly common implementation
//  that uses a mutex.
//
//
//  Because locking is so often just an internal detail and not part of the actual
//  data of the containing class, and therefore often has to be done from
//  inside const methods. We just make the locking methods const.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TClass;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: MLockable
// PREFIX: mlock
// ---------------------------------------------------------------------------
class CIDLIBEXP MLockable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        virtual ~MLockable() = default;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bTryLock
        (
            const   tCIDLib::TCard4         c4WaitMSs
        )   const = 0;

        virtual tCIDLib::TBoolean bIsDescendantOf
        (
            const   TClass&                 clsTarget
        )   const = 0;

        virtual const TClass& clsIsA() const = 0;

        virtual const TClass& clsParent() const = 0;

        virtual tCIDLib::TVoid Lock
        (
            const   tCIDLib::TCard4         c4WaitMSs
        )   const = 0;

        virtual tCIDLib::TVoid Unlock() const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators, so that including classes can still
        //  have default copy/assignment
        // -------------------------------------------------------------------
        MLockable() = default;
        MLockable(const MLockable&) = default;
        MLockable(MLockable&&) = default;
        MLockable& operator=(const MLockable&) = default;
        MLockable& operator=(MLockable&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        NulObject(MLockable)
};



// ---------------------------------------------------------------------------
//  CLASS: TLocker
// PREFIX: lockr
// ---------------------------------------------------------------------------
class CIDLIBEXP TLocker
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TLocker() = delete;

        TLocker(    const   MLockable* const    pmlockTar
                    , const tCIDLib::TCard4     c4Timeout = kCIDLib::c4MaxWait) :

            m_bLocked(kCIDLib::False)
            , m_pmlockTar(pmlockTar)
        {
            if (m_pmlockTar)
            {
                m_pmlockTar->Lock(c4Timeout);
                m_bLocked = kCIDLib::True;
            }
        }

        TLocker(    const   MLockable* const    pmlockTar
                    , const tCIDLib::TBoolean   bLockIt
                    , const tCIDLib::TCard4     c4Timeout = kCIDLib::c4MaxWait) :

            m_bLocked(kCIDLib::False)
            , m_pmlockTar(pmlockTar)
        {
            if (pmlockTar && bLockIt)
            {
                m_pmlockTar->Lock(c4Timeout);
                m_bLocked = kCIDLib::True;
            }
        }

        TLocker(const TLocker&) = delete;
        TLocker(TLocker&&) = delete;

        ~TLocker()
        {
            // If we have it locked, then unlock it
            if (m_bLocked && m_pmlockTar)
            {
                // This can throw, though it's highly unlikely. If so, we die
                #pragma warning(suppress : 26447)
                m_pmlockTar->Unlock();
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocker& operator=(const TLocker&) = delete;
        TLocker& operator=(TLocker&&) = delete;
        tCIDLib::TVoid* operator new(size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLock(const tCIDLib::TCard4 c4Timeout)
        {
            if (m_pmlockTar)
            {
                CheckLocked(kCIDLib::True);
                m_bLocked = m_pmlockTar->bTryLock(c4Timeout);
            }
             else
            {
                // We don't have a mutex, so just say it's locked
                m_bLocked = kCIDLib::True;
            }
            return m_bLocked;
        }

        tCIDLib::TBoolean bLocked() const
        {
            return m_bLocked;
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4Timeout = kCIDLib::c4MaxWait)
        {
            if (m_pmlockTar)
            {
                CheckLocked(kCIDLib::True);
                m_pmlockTar->Lock(c4Timeout);
            }
            m_bLocked = kCIDLib::True;
        }

        tCIDLib::TVoid Orphan()
        {
            m_pmlockTar = nullptr;
            m_bLocked = kCIDLib::False;
        }

        tCIDLib::TVoid Release()
        {
            if (m_pmlockTar)
            {
                CheckLocked(kCIDLib::False);
                m_pmlockTar->Unlock();
            }
            m_bLocked = kCIDLib::False;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckLocked
        (
            const   tCIDLib::TBoolean       bToCheck
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bLocked
        //      This indicates whether we currently have it locked or not.
        //
        //  m_pmlockTar
        //      This is a pointer to the target mutex we will release upon
        //      destruction. It may be zero. We are designed to be tolerant
        //      of this so that optional scope based locking can be done. See
        //      the file notes above.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bLocked;
        const MLockable*    m_pmlockTar;
};

#pragma CIDLIB_POPPACK
