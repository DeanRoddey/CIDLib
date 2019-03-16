//
// FILE NAME: CIDLib_Mutex.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/20/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the TMutex class, which is a mutual exclusion
//  semaphore. Only one thread at a time can own it.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TMutex,TObject)




// ---------------------------------------------------------------------------
//  CLASS: TMtxLocker
// PREFIX: lock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMtxLocker: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMtxLocker::CheckLocked(const tCIDLib::TBoolean bToCheck)
{
    // If we already have it locked/unlocked, this is illegal
    if (m_bLocked == bToCheck)
    {
        if (m_bLocked)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcLock_AlreadyLocked
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Already
            );
        }
         else
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcLock_NotLocked
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
            );
        }
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TMutex
//  PREFIX: mtx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMutex: Constructors and Destructor
// ---------------------------------------------------------------------------
TMutex::TMutex(const tCIDLib::ELockStates eInitState) :

    m_bNamed(kCIDLib::False)
{
    if (!m_kmtxImpl.bCreate(eInitState))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMtx_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
        );
    }
}

TMutex::TMutex( const   TResourceName&          rsnToUse
                , const tCIDLib::ELockStates    eInitState
                , const tCIDLib::ECreateActs eAction) :

    m_bNamed(kCIDLib::True)
    , m_kmtxImpl
      (
        rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Mutex).pszBuffer()
      )
    , m_rsnThis(rsnToUse)
{
    tCIDLib::TBoolean bCreated;
    InitNamed(bCreated, eInitState, eAction);
}

TMutex::TMutex( const   TResourceName&          rsnToUse
                ,       tCIDLib::TBoolean&      bCreated
                , const tCIDLib::ELockStates    eInitState
                , const tCIDLib::ECreateActs eAction) :

    m_bNamed(kCIDLib::True)
    , m_kmtxImpl
      (
        rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Mutex).pszBuffer()
      )
    , m_rsnThis(rsnToUse)
{
    InitNamed(bCreated, eInitState, eAction);
}

TMutex::~TMutex()
{
    if (!m_kmtxImpl.bClose())
    {
        //
        //  If we get an error, log it but don't throw out of the destructor
        //  so we use a warning message.
        //
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMtx_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
        );
    }
}


// ---------------------------------------------------------------------------
//  TMutex: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMutex::bIsNamed() const
{
    return m_bNamed;
}


tCIDLib::TBoolean TMutex::bTryLock(const tCIDLib::TCard4 c4Timeout) const
{
    if (!m_kmtxImpl.bLock(c4Timeout))
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        if (kerrRes.errcId() != kKrnlErrs::errcGen_Timeout)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMtx_LockError
                , kerrRes
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Timeout
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TVoid TMutex::Lock(const tCIDLib::TCard4 c4Timeout) const
{
    if (!m_kmtxImpl.bLock(c4Timeout))
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();

        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::Unknown;
        tCIDLib::TErrCode errcLog = kCIDErrs::errcMtx_LockError;
        if (kerrRes.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            eClass = tCIDLib::EErrClasses::Timeout;
            errcLog = kCIDErrs::errcMtx_Timeout;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcLog
            , kerrRes
            , tCIDLib::ESeverities::Failed
            , eClass
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
        );
    }
}


const TResourceName& TMutex::rsnName() const
{
    return m_rsnThis;
}


tCIDLib::TVoid TMutex::Unlock() const
{
    if (!m_kmtxImpl.bUnlock())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMtx_Unlock
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
        );
    }
}


// ---------------------------------------------------------------------------
//  TMutex: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMutex::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex);
}



// ---------------------------------------------------------------------------
//  TMutex: Private, non-virtual methods
// ---------------------------------------------------------------------------

// A common helper to avoid redundant code in some ctors
tCIDLib::TVoid
TMutex::InitNamed(          tCIDLib::TBoolean&      bCreated
                    , const tCIDLib::ELockStates    eInitState
                    , const tCIDLib::ECreateActs eCreateAct)
{
    if (eCreateAct == tCIDLib::ECreateActs::CreateIfNew)
    {
        if (!m_kmtxImpl.bCreate(eInitState))
        {
            const TKrnlError& kerrLast = TKrnlError::kerrLast();
            tCIDLib::TErrCode errcToThrow;
            if (kerrLast.errcId() == kKrnlErrs::errcGen_AlreadyExists)
                errcToThrow = kCIDErrs::errcMtx_AlreadyExists;
            else
                errcToThrow = kCIDErrs::errcMtx_Create;

            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMtx_Create
                , kerrLast
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
            );
        }

        // It's always created (or fails) in this case
        bCreated = kCIDLib::True;
    }
     else if (eCreateAct == tCIDLib::ECreateActs::OpenOrCreate)
    {
        if (!m_kmtxImpl.bOpenOrCreate(eInitState, bCreated))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMtx_OpenOrCreate
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
            );
        }
    }
     else if (eCreateAct == tCIDLib::ECreateActs::OpenIfExists)
    {
        if (!m_kmtxImpl.bOpen())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMtx_Open
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
            );
        }

        // It's never created in this case
        bCreated = kCIDLib::False;
    }
     else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadCreateAct
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eCreateAct))
            , TString(L"mutex")
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Mutex)
        );
    }
}

