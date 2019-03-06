//
// FILE NAME: CIDLib_Mutex.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_Mutex.Cpp file. It implements the
//  TMutex class, which is a mutual exclusion class. We also define a
//  lock janitor for the mutex, so that scope based locking can be done
//  very conveniently.
//
// CAVEATS/GOTCHAS:
//
//  1)  The lock janitor is intended to be tolerant of a null mutex pointer
//      so that we can do optional locking even while getting the benefits
//      of a lock janitor. The code can either have a mutex or or not and
//      can still use the janitor.
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
class CIDLIBEXP TMutex : public TObject, public MFormattable
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

        ~TMutex();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMutex& operator=(const TMutex&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsNamed() const;

        tCIDLib::TBoolean bTryLock
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TVoid Lock
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TVoid Unlock() const;

        const TResourceName& rsnName() const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const;


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
//  CLASS: TMtxLocker
// PREFIX: lock
// ---------------------------------------------------------------------------
class CIDLIBEXP TMtxLocker
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TMtxLocker() = delete;

        TMtxLocker( const   TMutex* const       pmtxToLock
                    , const tCIDLib::TCard4     c4Timeout = kCIDLib::c4MaxWait) :

            m_bLocked(kCIDLib::False)
            , m_pmtxToLock(pmtxToLock)
        {
            if (m_pmtxToLock)
            {
                m_pmtxToLock->Lock(c4Timeout);
                m_bLocked = kCIDLib::True;
            }
        }

        TMtxLocker( const   TMutex* const       pmtxToLock
                    , const tCIDLib::TBoolean   bLockIt
                    , const tCIDLib::TCard4     c4Timeout = kCIDLib::c4MaxWait) :

            m_bLocked(kCIDLib::False)
            , m_pmtxToLock(pmtxToLock)
        {
            if (pmtxToLock && bLockIt)
            {
                m_pmtxToLock->Lock(c4Timeout);
                m_bLocked = kCIDLib::True;
            }
        }

        TMtxLocker(const TMtxLocker&) = delete;

        ~TMtxLocker()
        {
            // If we have it locked, then unlock it
            if (m_bLocked && m_pmtxToLock)
                m_pmtxToLock->Unlock();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMtxLocker& operator=(const TMtxLocker&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLock(const tCIDLib::TCard4 c4Timeout)
        {
            if (m_pmtxToLock)
            {
                CheckLocked(kCIDLib::True);
                m_bLocked = m_pmtxToLock->bTryLock(c4Timeout);
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
            if (m_pmtxToLock)
            {
                CheckLocked(kCIDLib::True);
                m_pmtxToLock->Lock(c4Timeout);
            }
            m_bLocked = kCIDLib::True;
        }

        tCIDLib::TVoid Orphan()
        {
            m_pmtxToLock = 0;
            m_bLocked = kCIDLib::False;
        }

        tCIDLib::TVoid Release()
        {
            if (m_pmtxToLock)
            {
                CheckLocked(kCIDLib::False);
                m_pmtxToLock->Unlock();
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
        //  m_pmtxToLock
        //      This is a pointer to the target mutex we will release upon
        //      destruction. It may be zero. We are designed to be tolerant
        //      of this so that optional scope based locking can be done. See
        //      the file notes above.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bLocked;
        const TMutex*       m_pmtxToLock;
};

#pragma CIDLIB_POPPACK


