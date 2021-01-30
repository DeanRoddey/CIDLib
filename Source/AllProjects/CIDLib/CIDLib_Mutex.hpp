//
// FILE NAME: CIDLib_Mutex.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/1993
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
//  This is the header for the CIDLib_Mutex.Cpp file. It implements the TMutex
//  class, which is a mutual exclusion class.
//
//  We also define a very common derivative of MLockable that works in terms of
//  a mutex. This lets things that wnat to implement MLockable and don't need
//  to provide their own locking for any other purpose, just mix in this
//  convenient implementation. It assumes that the mutex will never be copied or
//  assigned, so it just provides empty assign, copy, move stuff.
//
//  As per MLockable, the locking methods are const, because it's so common for
//  mutexes to just be an internal detail and used for locking within both const
//  and non-const methods.
//
//  As per the
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
//  CLASS: TMutex
// PREFIX: mtx
// ---------------------------------------------------------------------------
class CIDLIBEXP TMutex : public TObject, public MFormattable, public MLockable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TMutex
        (
            const   tCIDLib::ELockStates    eInitState = tCIDLib::ELockStates::Unlocked
        );

        TMutex
        (
            const   TResourceName&          rsnToUse
            , const tCIDLib::ELockStates    eInitState = tCIDLib::ELockStates::Unlocked
            , const tCIDLib::ECreateActs    eCreateAct = tCIDLib::ECreateActs::OpenOrCreate
        );

        TMutex
        (
            const   TResourceName&          rsnToUse
            ,       tCIDLib::TBoolean&      bCreated
            , const tCIDLib::ELockStates    eInitState = tCIDLib::ELockStates::Unlocked
            , const tCIDLib::ECreateActs    eCreateAct = tCIDLib::ECreateActs::OpenOrCreate
        );

        TMutex(const TMutex&) = delete;
        TMutex(TMutex&&) = delete;

        ~TMutex();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMutex& operator=(const TMutex&) = delete;
        TMutex& operator=(TMutex&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTryLock
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        )   const final;

        tCIDLib::TVoid Lock
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        )   const final;

        tCIDLib::TVoid Unlock() const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsNamed() const;

        const TResourceName& rsnName() const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP    TTextOutStream&         strmToWriteTo
        )   const   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitNamed
        (
                    tCIDLib::TBoolean&      bCreated
            , const tCIDLib::ELockStates    eInitState
            , const tCIDLib::ECreateActs    eAction
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNamed
        //      Indicates whether this is a named mutex or not.
        //
        //  m_kmtxImpl
        //      This is the kernel mutex object that provides the real
        //      implementation and platform abstraction.
        //
        //  m_rsnThis
        //      The name of the mutex, in a resource name object.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bNamed;
        TKrnlMutex              m_kmtxImpl;
        TResourceName           m_rsnThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMutex,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: MMtxLockable
// PREFIX: mlock
// ---------------------------------------------------------------------------
class CIDLIBEXP MMtxLockable : public MLockable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~MMtxLockable() {}


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTryLock(const tCIDLib::TCard4 c4WaitMSs) const override
        {
            return m_mtxSync.bTryLock(c4WaitMSs);
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4WaitMSs) const override
        {
            m_mtxSync.Lock(c4WaitMSs);
        }

        tCIDLib::TVoid Unlock() const override
        {
            m_mtxSync.Unlock();
        }


    protected :
        // -------------------------------------------------------------------
        //  See the class comments above. We assume the mutex is never copied or
        //  moved or assigned. It's an internal detail, not data of the class.
        // -------------------------------------------------------------------
        MMtxLockable() = default;
        MMtxLockable(const MMtxLockable&) = default;
        MMtxLockable(MMtxLockable&&) = default;
        MMtxLockable& operator=(const MMtxLockable&) = default;
        MMtxLockable& operator=(MMtxLockable&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mtxSync
        //      The mutex we use to implement the lockable interface. Our overrides
        //      above are just forwards to this. It is not moved or assigned or
        //      copied. See the class comments above. It has to be mutable because
        //      the locking methods are const (see the header comments of the
        //      base lockable class.)
        // -------------------------------------------------------------------
        TMutex  m_mtxSync;
};

#pragma CIDLIB_POPPACK
