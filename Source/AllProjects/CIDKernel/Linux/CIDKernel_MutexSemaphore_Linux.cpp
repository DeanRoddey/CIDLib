//
// FILE NAME: CIDKernel_MutexSemaphore_Linux.Cpp
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
    __phmtxiThis(0)
{
}

TMutexHandle::TMutexHandle(const TMutexHandle& hmtxToCopy) :
    __phmtxiThis(hmtxToCopy.__phmtxiThis)
{
}

TMutexHandle::~TMutexHandle()
{
    __phmtxiThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TMutexHandle& TMutexHandle::operator=(const TMutexHandle& hmtxToAssign)
{
    if (this == &hmtxToAssign)
        return *this;

    __phmtxiThis = hmtxToAssign.__phmtxiThis;

    return *this;
}


tCIDLib::TBoolean
TMutexHandle::operator==(const TMutexHandle& hmtxToCompare) const
{
    return (__phmtxiThis == hmtxToCompare.__phmtxiThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TMutexHandle::bValid() const
{
    return (__phmtxiThis && __phmtxiThis->c4RefCount);
}

tCIDLib::TVoid TMutexHandle::Clear()
{
    __phmtxiThis = 0;
}

tCIDLib::TVoid
TMutexHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(__phmtxiThis)
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
    __pszName(0)
{
}

TKrnlMutex::TKrnlMutex(const tCIDLib::TCh* const pszName):
    __pszName(0)
{
    if (pszName)
        __pszName = TRawStr::pszReplicate(pszName);
}

TKrnlMutex::~TKrnlMutex()
{
    if (__pszName)
    {
        delete [] __pszName;
        __pszName = 0;
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
tCIDLib::TBoolean TKrnlMutex::bValid() const
{
    return __hmtxThis.bValid();
}


tCIDLib::TBoolean TKrnlMutex::bClose()
{
    if (__hmtxThis.bValid())
    {
        __hmtxThis.__phmtxiThis->prmtxThis->iLock();

        if (!--__hmtxThis.__phmtxiThis->c4RefCount)
        {
            if((__hmtxThis.__phmtxiThis->iSysVSemId != -1)
               && __hmtxThis.__phmtxiThis->bSysVOwner)
            {
                union semun SemUnion;
                SemUnion.val = 0;

                if (::semctl(__hmtxThis.__phmtxiThis->iSysVSemId
                             , 0
                             , IPC_RMID
                             , SemUnion))
                {
                    __hmtxThis.__phmtxiThis->c4RefCount++;
                    __hmtxThis.__phmtxiThis->prmtxThis->iUnlock();
                    TKrnlError::SetLastHostError(errno);
                    return kCIDLib::False;
                }
            }

            __hmtxThis.__phmtxiThis->prmtxThis->iUnlock();

            tCIDLib::TOSErrCode HostErr =
                __hmtxThis.__phmtxiThis->prmtxThis->iDestroy();
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete __hmtxThis.__phmtxiThis->prmtxThis;
            delete __hmtxThis.__phmtxiThis;
        }
        else
        {
            __hmtxThis.__phmtxiThis->prmtxThis->iUnlock();
        }

        __hmtxThis.__phmtxiThis = 0;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlMutex::bCreate(const tCIDLib::ELockStates eInitState)
{
    if (__hmtxThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (__pszName)
        return __bCreateNamed(eInitState, kCIDLib::True);

    __hmtxThis.__phmtxiThis = new TMutexHandleImpl;
    __hmtxThis.__phmtxiThis->c4RefCount = 1;
    __hmtxThis.__phmtxiThis->iSysVSemId = -1;

    __hmtxThis.__phmtxiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    __hmtxThis.__phmtxiThis->prmtxThis->iInitialize();

    if (eInitState == tCIDLib::ELockStates::Locked)
        __hmtxThis.__phmtxiThis->prmtxThis->iLock();

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlMutex::bCreateOrOpen(const tCIDLib::ELockStates eInitState)
{
    if (__hmtxThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!__pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    return __bCreateNamed(eInitState, kCIDLib::False);
}


tCIDLib::TBoolean TKrnlMutex::bLock(const tCIDLib::TCard4 c4MilliSecs) const
{
    union semun SemUnion;
    SemUnion.val = 0;

    if (!__hmtxThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (__hmtxThis.__phmtxiThis->iSysVSemId != -1
        && !::semctl(__hmtxThis.__phmtxiThis->iSysVSemId
                     , 0
                     , GETVAL
                     , SemUnion)
        && (::semctl(__hmtxThis.__phmtxiThis->iSysVSemId
                     , 0
                     , GETPID
                     , SemUnion) == ::getpid()))
    {
        __hmtxThis.__phmtxiThis->c4LockCount++;
        return kCIDLib::True;
    }

    if (c4MilliSecs == kCIDLib::c4MaxWait)
    {
        if (__hmtxThis.__phmtxiThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (::semop(__hmtxThis.__phmtxiThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }

            __hmtxThis.__phmtxiThis->c4LockCount = 1;
        }
        else
        {
            __hmtxThis.__phmtxiThis->prmtxThis->iLock();
        }
    }
    else
    {
        TKrnlLinux::TThreadTimer thtWaitFor(c4MilliSecs);

        if (__hmtxThis.__phmtxiThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (!thtWaitFor.bBegin())
                return kCIDLib::False;

            if (::semop(__hmtxThis.__phmtxiThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                if (errno == EINTR)
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
                else
                    TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }

            __hmtxThis.__phmtxiThis->c4LockCount = 1;
        }
        else
        {
            if (!thtWaitFor.bTriggered())
            {
                if (!thtWaitFor.bBegin())
                    return kCIDLib::False;

                __hmtxThis.__phmtxiThis->prmtxThis->iLock();

                thtWaitFor.Cancel();
            }
            else
            {
                __hmtxThis.__phmtxiThis->prmtxThis->iUnlock();
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
    delete [] __pszName;
    __pszName = 0;

    // Recreate the name
    if (kmtxToDup.__pszName)
        __pszName = TRawStr::pszReplicate(kmtxToDup.__pszName);

    // Duplicate the handle
    __hmtxThis = kmtxToDup.__hmtxThis;
    if (__hmtxThis.bValid())
    {
        __hmtxThis.__phmtxiThis->prmtxThis->iLock();
        __hmtxThis.__phmtxiThis->c4RefCount++;
        __hmtxThis.__phmtxiThis->prmtxThis->iUnlock();
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bOpen()
{
    if (__hmtxThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!__pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    __hmtxThis.__phmtxiThis = new TMutexHandleImpl;
    __hmtxThis.__phmtxiThis->c4RefCount = 1;

    tCIDLib::TSInt iFlags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(__pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmp = ::semget(key, 0, iFlags);
    if (iTmp == -1)
    {
        delete __hmtxThis.__phmtxiThis;
        __hmtxThis.__phmtxiThis = 0;
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    __hmtxThis.__phmtxiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    __hmtxThis.__phmtxiThis->prmtxThis->iInitialize();

    __hmtxThis.__phmtxiThis->iSysVSemId = iTmp;
    __hmtxThis.__phmtxiThis->bSysVOwner = kCIDLib::False;
    __hmtxThis.__phmtxiThis->c4LockCount = 0;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bSetName(const tCIDLib::TCh* const pszName)
{
    if (__hmtxThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    delete [] __pszName;
    __pszName = TRawStr::pszReplicate(pszName);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bUnlock() const
{
    if (!__hmtxThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (__hmtxThis.__phmtxiThis->iSysVSemId != -1)
    {
        union semun SemUnion;
        SemUnion.val = 0;

        if (!::semctl(__hmtxThis.__phmtxiThis->iSysVSemId
                      , 0
                      , GETVAL
                      , SemUnion))
        {
            if (::semctl(__hmtxThis.__phmtxiThis->iSysVSemId
                         , 0
                         , GETPID
                         , SemUnion) == ::getpid())
            {
                if (!--__hmtxThis.__phmtxiThis->c4LockCount)
                {
                    struct sembuf SemBuf;
                    SemBuf.sem_num = 0;
                    SemBuf.sem_op = 1;
                    SemBuf.sem_flg = 0;

                    if (::semop(__hmtxThis.__phmtxiThis->iSysVSemId, &SemBuf, 1))
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
        __hmtxThis.__phmtxiThis->prmtxThis->iUnlock();
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlMutex: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlMutex::__bCreateNamed( const   tCIDLib::ELockStates    eState
                            , const tCIDLib::TBoolean       bFailIfExists)
{
    tCIDLib::TSInt iFlags = IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(__pszName, kCIDLib::i4MaxInt);
    tCIDLib::TBoolean bOwner = kCIDLib::False;

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
    }

    __hmtxThis.__phmtxiThis = new TMutexHandleImpl;
    __hmtxThis.__phmtxiThis->c4RefCount = 1;
    __hmtxThis.__phmtxiThis->iSysVSemId = iTmp;
    __hmtxThis.__phmtxiThis->bSysVOwner = bOwner;

    __hmtxThis.__phmtxiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    __hmtxThis.__phmtxiThis->prmtxThis->iInitialize();

    if (eState == tCIDLib::ELockStates::Locked)
        return bLock(kCIDLib::c4MaxWait);

    return kCIDLib::True;
}
