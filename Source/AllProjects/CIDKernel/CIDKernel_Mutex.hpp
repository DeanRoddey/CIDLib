//
// FILE NAME: CIDKernel_Mutex.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_Mutex.cpp file, which implements
//  the TKrnlMutex class. This class is the platform wrapper class for mutex
//  objects.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Duplicate() will duplicate a mutex. This is the only way to
//      open multiple copies of an unnamed mutex. For named mutexes, you
//      can either create another mutex with the same name, or use the
//      duplicate method.
//
//  2)  bOpen() and bCreateOrOpen() are only valid for named mutexes. Unnamed
//      ones can only be created.
//
//  3)  The lock state must be a mutable attribute (its not a member anyway
//      in most implementations, since its implicit) so the lock/unlock
//      methods are const. This is a requirement of the standard MLockable
//      mixin interface used in TMutex (which uses this class as its
//      implementation.)
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlMutex
//  PREFIX: kmtx
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlMutex
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlMutex();

        TKrnlMutex
        (
            const   tCIDLib::TCh* const     pszName
        );

        TKrnlMutex(const TKrnlMutex&) = delete;

        ~TKrnlMutex();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlMutex& operator=(const TKrnlMutex&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bCreate
        (
            const   tCIDLib::ELockStates    eInitState
        );

        tCIDLib::TBoolean bDuplicate
        (
            const   TKrnlMutex&             kmtxToDup
        );

        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TBoolean bLock
        (
            const   tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TBoolean bNamed() const;

        tCIDLib::TBoolean bOpen();

        tCIDLib::TBoolean bOpenOrCreate
        (
            const   tCIDLib::ELockStates    eInitState
            ,       tCIDLib::TBoolean&      bCreated
        );

        tCIDLib::TBoolean bSetName
        (
            const   tCIDLib::TCh* const     pszNewName
        );

        tCIDLib::TBoolean bUnlock() const;

        const tCIDLib::TCh* pszName() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreateNamed
        (
            const   tCIDLib::ELockStates    eInitState
            , const tCIDLib::TBoolean       bFailIfExists
            ,       tCIDLib::TBoolean&      bCreated
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hmtxThis
        //      This is the handle to the mutex sem.
        //
        //  m_pszName
        //      The name of the semaphore.
        // -------------------------------------------------------------------
        TMutexHandle    m_hmtxThis;
        tCIDLib::TCh*   m_pszName;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean TKrnlMutex::bIsValid() const
{
    return  (m_hmtxThis.bIsValid());
}

inline tCIDLib::TBoolean TKrnlMutex::bNamed() const
{
    return (m_pszName != 0);
}

inline const tCIDLib::TCh* TKrnlMutex::pszName() const
{
    return m_pszName;
}
