//
// FILE NAME: CIDKernel_Mutex_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/25/1998
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
//  This file provides the Linux specific implementation of the class
//  TKrnlMutex class.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TMutexHandle
//  PREFIX: hmtx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMutexHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TMutexHandle::TMutexHandle() :

    m_phmtxiThis(nullptr)
{
}

TMutexHandle::TMutexHandle(const TMutexHandle& hmtxToCopy) :

    m_phmtxiThis(hmtxToCopy.m_phmtxiThis)
{
}

TMutexHandle::~TMutexHandle()
{
    m_phmtxiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TMutexHandle& TMutexHandle::operator=(const TMutexHandle& hmtxToAssign)
{
    if (this == &hmtxToAssign)
        return *this;

    m_phmtxiThis = hmtxToAssign.m_phmtxiThis;
    return *this;
}


tCIDLib::TBoolean
TMutexHandle::operator==(const TMutexHandle& hmtxToCompare) const
{
    return (m_phmtxiThis == hmtxToCompare.m_phmtxiThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TMutexHandle::bIsValid() const
{
    return (m_phmtxiThis && m_phmtxiThis->c4RefCount);
}

tCIDLib::TVoid TMutexHandle::Clear()
{
    m_phmtxiThis = 0;
}

tCIDLib::TVoid
TMutexHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phmtxiThis)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlMutex
//  PREFIX: kmtx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlMutex: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlMutex::TKrnlMutex() :

    m_pszName(nullptr)
{
}

TKrnlMutex::TKrnlMutex(const tCIDLib::TCh* const pszName):

    m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
}

TKrnlMutex::~TKrnlMutex()
{
    if (m_pszName)
    {
        delete [] m_pszName;
        m_pszName = nullptr;
    }

    if (!bClose())
    {
        //
        //  If it fails and we are debugging, then do a popup. Otherwise
        //  there is not much we can do.
        //
        #if CID_DEBUG_ON
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
            , kmodCIDKernel.pszLoadCIDFacMsg(kKrnlErrs::errcGen_CloseHandle)
        );
        #endif
    }
}


// ---------------------------------------------------------------------------
//  TKrnlMutex: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlMutex::bClose()
{
    if (m_hmtxThis.bIsValid())
    {
        m_hmtxThis.m_phmtxiThis->prmtxThis->iLock();

        if (!--m_hmtxThis.m_phmtxiThis->c4RefCount)
        {
            if((m_hmtxThis.m_phmtxiThis->iSysVSemId != -1)
               && m_hmtxThis.m_phmtxiThis->bSysVOwner)
            {
                union semun SemUnion;
                SemUnion.val = 0;

                if (::semctl(m_hmtxThis.m_phmtxiThis->iSysVSemId
                             , 0
                             , IPC_RMID
                             , SemUnion))
                {
                    m_hmtxThis.m_phmtxiThis->c4RefCount++;
                    m_hmtxThis.m_phmtxiThis->prmtxThis->iUnlock();
                    TKrnlError::SetLastHostError(errno);
                    return kCIDLib::False;
                }
            }

            m_hmtxThis.m_phmtxiThis->prmtxThis->iUnlock();

            tCIDLib::TOSErrCode HostErr =
                m_hmtxThis.m_phmtxiThis->prmtxThis->iDestroy();
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete m_hmtxThis.m_phmtxiThis->prmtxThis;
            delete m_hmtxThis.m_phmtxiThis;
        }
        else
        {
            m_hmtxThis.m_phmtxiThis->prmtxThis->iUnlock();
        }

        m_hmtxThis.m_phmtxiThis = 0;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlMutex::bCreate(const tCIDLib::ELockStates eInitState)
{
    if (m_hmtxThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (m_pszName)
    {
        tCIDLib::TBoolean bDummy;
        return bCreateNamed(eInitState, kCIDLib::True, bDummy);
    }

    m_hmtxThis.m_phmtxiThis = new TMutexHandleImpl;
    m_hmtxThis.m_phmtxiThis->c4RefCount = 1;
    m_hmtxThis.m_phmtxiThis->iSysVSemId = -1;

    m_hmtxThis.m_phmtxiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    m_hmtxThis.m_phmtxiThis->prmtxThis->iInitialize();

    if (eInitState == tCIDLib::ELockStates::Locked)
        m_hmtxThis.m_phmtxiThis->prmtxThis->iLock();

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlMutex::bOpenOrCreate(  const   tCIDLib::ELockStates    eInitState
                            ,       tCIDLib::TBoolean&      bCreated)
{
    if (m_hmtxThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    return bCreateNamed(eInitState, kCIDLib::False, bCreated);
}


tCIDLib::TBoolean TKrnlMutex::bLock(const tCIDLib::TCard4 c4MilliSecs) const
{
    union semun SemUnion;
    SemUnion.val = 0;

    if (!m_hmtxThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (m_hmtxThis.m_phmtxiThis->iSysVSemId != -1
        && !::semctl(m_hmtxThis.m_phmtxiThis->iSysVSemId
                     , 0
                     , GETVAL
                     , SemUnion)
        && (::semctl(m_hmtxThis.m_phmtxiThis->iSysVSemId
                     , 0
                     , GETPID
                     , SemUnion) == ::getpid()))
    {
        m_hmtxThis.m_phmtxiThis->c4LockCount++;
        return kCIDLib::True;
    }

    if (c4MilliSecs == kCIDLib::c4MaxWait)
    {
        if (m_hmtxThis.m_phmtxiThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (::semop(m_hmtxThis.m_phmtxiThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }

            m_hmtxThis.m_phmtxiThis->c4LockCount = 1;
        }
        else
        {
            m_hmtxThis.m_phmtxiThis->prmtxThis->iLock();
        }
    }
    else
    {
        TKrnlLinux::TThreadTimer thtWaitFor(c4MilliSecs);

        if (m_hmtxThis.m_phmtxiThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (!thtWaitFor.bBegin())
                return kCIDLib::False;

            if (::semop(m_hmtxThis.m_phmtxiThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                if (errno == EINTR)
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
                else
                    TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }

            m_hmtxThis.m_phmtxiThis->c4LockCount = 1;
        }
        else
        {
            if (!thtWaitFor.bTriggered())
            {
                if (!thtWaitFor.bBegin())
                    return kCIDLib::False;

                m_hmtxThis.m_phmtxiThis->prmtxThis->iLock();

                thtWaitFor.Cancel();
            }
            else
            {
                m_hmtxThis.m_phmtxiThis->prmtxThis->iUnlock();
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
                return kCIDLib::False;
            }
        }
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bDuplicate(const TKrnlMutex& kmtxToDup)
{
    // Close it if open
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] m_pszName;
    m_pszName = 0;

    // Recreate the name
    if (kmtxToDup.m_pszName)
        m_pszName = TRawStr::pszReplicate(kmtxToDup.m_pszName);

    // Duplicate the handle
    m_hmtxThis = kmtxToDup.m_hmtxThis;
    if (m_hmtxThis.bIsValid())
    {
        m_hmtxThis.m_phmtxiThis->prmtxThis->iLock();
        m_hmtxThis.m_phmtxiThis->c4RefCount++;
        m_hmtxThis.m_phmtxiThis->prmtxThis->iUnlock();
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bOpen()
{
    if (m_hmtxThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    m_hmtxThis.m_phmtxiThis = new TMutexHandleImpl;
    m_hmtxThis.m_phmtxiThis->c4RefCount = 1;

    tCIDLib::TSInt iFlags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(m_pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmp = ::semget(key, 0, iFlags);
    if (iTmp == -1)
    {
        delete m_hmtxThis.m_phmtxiThis;
        m_hmtxThis.m_phmtxiThis = 0;
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    m_hmtxThis.m_phmtxiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    m_hmtxThis.m_phmtxiThis->prmtxThis->iInitialize();

    m_hmtxThis.m_phmtxiThis->iSysVSemId = iTmp;
    m_hmtxThis.m_phmtxiThis->bSysVOwner = kCIDLib::False;
    m_hmtxThis.m_phmtxiThis->c4LockCount = 0;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bSetName(const tCIDLib::TCh* const pszName)
{
    if (m_hmtxThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    delete [] m_pszName;
    m_pszName = TRawStr::pszReplicate(pszName);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bUnlock() const
{
    if (!m_hmtxThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (m_hmtxThis.m_phmtxiThis->iSysVSemId != -1)
    {
        union semun SemUnion;
        SemUnion.val = 0;

        if (!::semctl(m_hmtxThis.m_phmtxiThis->iSysVSemId
                      , 0
                      , GETVAL
                      , SemUnion))
        {
            if (::semctl(m_hmtxThis.m_phmtxiThis->iSysVSemId
                         , 0
                         , GETPID
                         , SemUnion) == ::getpid())
            {
                if (!--m_hmtxThis.m_phmtxiThis->c4LockCount)
                {
                    struct sembuf SemBuf;
                    SemBuf.sem_num = 0;
                    SemBuf.sem_op = 1;
                    SemBuf.sem_flg = 0;

                    if (::semop(m_hmtxThis.m_phmtxiThis->iSysVSemId, &SemBuf, 1))
                    {
                        TKrnlError::SetLastHostError(errno);
                        return kCIDLib::False;
                    }
                }
                return kCIDLib::True;
            }
        }

        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotOwner);
        return kCIDLib::False;
    }
    else
    {
        m_hmtxThis.m_phmtxiThis->prmtxThis->iUnlock();
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlMutex: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlMutex::bCreateNamed(const  tCIDLib::ELockStates    eState
                        , const tCIDLib::TBoolean       bFailIfExists
                        ,       tCIDLib::TBoolean&      bCreated)
{
    tCIDLib::TSInt iFlags = IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(m_pszName, kCIDLib::i4MaxInt);
    tCIDLib::TBoolean bOwner = kCIDLib::False;

    // We only need to set if we create it
    bCreated = kCIDLib::False;

    tCIDLib::TSInt iTmp = ::semget(key, 1, iFlags);
    if (iTmp == -1)
    {
        if (errno == EEXIST)
        {
            if (bFailIfExists)
            {
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
                return kCIDLib::False;
            }

            iFlags &= ~(IPC_CREAT | IPC_EXCL);

            iTmp = ::semget(key, 1, iFlags);
            if (iTmp == -1)
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }
    else
    {
        bOwner = kCIDLib::True;

        union semun SemUnion;
        SemUnion.val = 1;

        if (::semctl(iTmp, 0, SETVAL, SemUnion))
        {
            TKrnlError::SetLastHostError(errno);
            ::semctl(iTmp, 0, IPC_RMID, SemUnion);
            return kCIDLib::False;
        }

        // We created it
        bCreated = kCIDLib::True;
    }

    m_hmtxThis.m_phmtxiThis = new TMutexHandleImpl;
    m_hmtxThis.m_phmtxiThis->c4RefCount = 1;
    m_hmtxThis.m_phmtxiThis->iSysVSemId = iTmp;
    m_hmtxThis.m_phmtxiThis->bSysVOwner = bOwner;

    m_hmtxThis.m_phmtxiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    m_hmtxThis.m_phmtxiThis->prmtxThis->iInitialize();

    if (eState == tCIDLib::ELockStates::Locked)
        return bLock(kCIDLib::c4MaxWait);

    return kCIDLib::True;
}
