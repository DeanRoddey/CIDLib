//
// FILE NAME: CIDKernel_EventSemaphore_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/28/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Linux specific implementation for the class
//  TKrnlEvent.
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
//   CLASS: TEventHandle
//  PREFIX: hev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEventHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TEventHandle::TEventHandle() :
    __pheviThis(0)
{
}

TEventHandle::TEventHandle(const TEventHandle& hevToCopy) :
    __pheviThis(hevToCopy.__pheviThis)
{
}

TEventHandle::~TEventHandle()
{
    __pheviThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TEventHandle& TEventHandle::operator=(const TEventHandle& hevToAssign)
{
    if (this == &hevToAssign)
        return *this;

    __pheviThis = hevToAssign.__pheviThis;

    return *this;
}


tCIDLib::TBoolean
TEventHandle::operator==(const TEventHandle& hevToCompare) const
{
    return (__pheviThis == hevToCompare.__pheviThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TEventHandle::bValid() const
{
    return (__pheviThis && __pheviThis->c4RefCount);
}


tCIDLib::TVoid TEventHandle::Clear()
{
    __pheviThis = 0;
}


tCIDLib::TVoid
TEventHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(__pheviThis)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlEvent
//  PREFIX: kev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlEvent: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlEvent::TKrnlEvent() :
    __pszName(0)
{
}

TKrnlEvent::TKrnlEvent(const tCIDLib::TCh* const pszName) :
    __pszName(0)
{
    if (pszName)
        __pszName = TRawStr::pszReplicate(pszName);
}

TKrnlEvent::~TKrnlEvent()
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
//  TKrnlEvent: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlEvent::bClose()
{
    if (__hevThis.bValid())
    {
        ::pthread_mutex_lock(&__hevThis.__pheviThis->mtxThis);

        tCIDLib::TOSErrCode HostErr;
        if (!--__hevThis.__pheviThis->c4RefCount)
        {
            if(__hevThis.__pheviThis->iSysVSemId != -1)
            {
                if (__hevThis.__pheviThis->bSysVOwner)
                {
                    union semun SemUnion;
                    SemUnion.val = 0;

                    if (::semctl(__hevThis.__pheviThis->iSysVSemId
                                 , 0
                                 , IPC_RMID
                                 , SemUnion))
                    {
                        __hevThis.__pheviThis->c4RefCount++;
                        ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);
                        TKrnlError::SetLastHostError(errno);
                        return kCIDLib::False;
                    }
                }
            }
            else
            {
                HostErr = ::pthread_cond_destroy(&__hevThis.__pheviThis->condThis);
                if (HostErr)
                {
                    TKrnlError::SetLastHostError(HostErr);
                    return kCIDLib::False;
                }
            }

            ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);
            HostErr = ::pthread_mutex_destroy(&__hevThis.__pheviThis->mtxThis);
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete __hevThis.__pheviThis;
        }
        else
        {
            ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);
        }

        __hevThis.__pheviThis = 0;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bCreate(const tCIDLib::EEventStates eInitState)
{
    if (__hevThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (__pszName)
        return __bCreateNamed(eInitState, kCIDLib::True);

    __hevThis.__pheviThis = new TEventHandleImpl;
    __hevThis.__pheviThis->c4RefCount = 1;
    __hevThis.__pheviThis->iSysVSemId = -1;
    __hevThis.__pheviThis->bSet = (eInitState == tCIDLib::EEventStates::Reset);

    ::pthread_mutex_init(&__hevThis.__pheviThis->mtxThis, 0);

    tCIDLib::TSInt iErr = ::pthread_cond_init(&__hevThis.__pheviThis->condThis, 0);
    if (iErr)
    {
        ::pthread_mutex_destroy(&__hevThis.__pheviThis->mtxThis);
        delete __hevThis.__pheviThis;
        __hevThis.__pheviThis = 0;
        TKrnlError::SetLastHostError(iErr);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEvent::bCreateOrOpen(const tCIDLib::EEventStates eInitState)
{
    if (__hevThis.bValid())
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


tCIDLib::TBoolean TKrnlEvent::bDuplicate(const TKrnlEvent& kevToDup)
{
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] __pszName;
    __pszName = 0;

    // Recreate the name
    if (kevToDup.__pszName)
        __pszName = TRawStr::pszReplicate(kevToDup.__pszName);

    // Duplicate the handle
    __hevThis = kevToDup.__hevThis;
    if (__hevThis.bValid())
    {
        ::pthread_mutex_lock(&__hevThis.__pheviThis->mtxThis);
        __hevThis.__pheviThis->c4RefCount++;
        ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bOpen()
{
    if (__hevThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!__pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    __hevThis.__pheviThis = new TEventHandleImpl;
    __hevThis.__pheviThis->c4RefCount = 1;

    tCIDLib::TSInt iFlags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(__pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmp = ::semget(key, 0, iFlags);
    if (iTmp == -1)
    {
        delete __hevThis.__pheviThis;
        __hevThis.__pheviThis = 0;
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    ::pthread_mutex_init(&__hevThis.__pheviThis->mtxThis, 0);

    __hevThis.__pheviThis->iSysVSemId = iTmp;
    __hevThis.__pheviThis->bSysVOwner = kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bReset()
{
    if (!__hevThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (__hevThis.__pheviThis->iSysVSemId != -1)
    {
        union semun SemUnion;
        SemUnion.val = 1;

        if (::semctl(__hevThis.__pheviThis->iSysVSemId
                     , 0
                     , SETVAL
                     , SemUnion))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }
    else
    {
        ::pthread_mutex_lock(&__hevThis.__pheviThis->mtxThis);

        __hevThis.__pheviThis->bSet = kCIDLib::True;

        ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bSetName(const tCIDLib::TCh* const pszName)
{
    if (__hevThis.bValid())
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


tCIDLib::TBoolean TKrnlEvent::bTrigger()
{
    if (!__hevThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (__hevThis.__pheviThis->iSysVSemId != -1)
    {
        union semun SemUnion;
        SemUnion.val = 0;

        if (::semctl(__hevThis.__pheviThis->iSysVSemId
                     , 0
                     , SETVAL
                     , SemUnion))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }
    else
    {
        ::pthread_mutex_lock(&__hevThis.__pheviThis->mtxThis);

        __hevThis.__pheviThis->bSet = kCIDLib::False;
        ::pthread_cond_broadcast(&__hevThis.__pheviThis->condThis);

        ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bWaitFor(const tCIDLib::TCard4 c4Wait)
{
    if (!__hevThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (c4Wait == kCIDLib::c4MaxWait)
    {
        if (__hevThis.__pheviThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = 0;
            SemBuf.sem_flg = 0;

            if (::semop(__hevThis.__pheviThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            ::pthread_mutex_lock(&__hevThis.__pheviThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (__hevThis.__pheviThis->bSet && !HostErr)
            {
                HostErr = ::pthread_cond_wait(&__hevThis.__pheviThis->condThis
                                              , &__hevThis.__pheviThis->mtxThis);
            }

            ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);

            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

        }
    }
    else
    {
        if (__hevThis.__pheviThis->iSysVSemId != -1)
        {
            TKrnlLinux::TThreadTimer thtWaitFor(c4Wait);

            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = 0;
            SemBuf.sem_flg = 0;

            if (!thtWaitFor.bBegin())
                return kCIDLib::False;

            if (::semop(__hevThis.__pheviThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                if (errno == EINTR)
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
                else
                    TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            struct timespec TimeSpec;
            TimeSpec.tv_sec = ::time(0) + (c4Wait / 1000);
            TimeSpec.tv_nsec = (c4Wait % 1000) * 1000000;

            ::pthread_mutex_lock(&__hevThis.__pheviThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (__hevThis.__pheviThis->bSet && !HostErr)
            {
                HostErr = ::pthread_cond_timedwait(&__hevThis.__pheviThis->condThis
                                                   , &__hevThis.__pheviThis->mtxThis
                                                   , &TimeSpec);
            }

            ::pthread_mutex_unlock(&__hevThis.__pheviThis->mtxThis);

            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }
        }
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlEvent: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
// FUNCTION/METHOD NAME: __bCreateNamed
//
// DESCRIPTION:
//
//  These guys will create mutex, using the different rules that named
//  resources require.
// -------------------------------------
//   INPUT: eInitState indicates the initial lock state.
//          bFailIfExists indicates whether this is a create operation
//              or a create w/ optional open.
//
//  OUTPUT: None
//
//  RETURN: kCIDLib::True if successful, else kCIDLib::False.
//
tCIDLib::TBoolean
TKrnlEvent::__bCreateNamed( const   tCIDLib::EEventStates eInitState
                            , const tCIDLib::TBoolean     bFailIfExists)
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
        SemUnion.val = eInitState == tCIDLib::EEventStates::Reset ? 1 : 0;

        if (::semctl(iTmp, 0, SETVAL, SemUnion))
        {
            TKrnlError::SetLastHostError(errno);
            ::semctl(iTmp, 0, IPC_RMID, SemUnion);
            return kCIDLib::False;
        }
    }

    __hevThis.__pheviThis = new TEventHandleImpl;
    __hevThis.__pheviThis->c4RefCount = 1;
    __hevThis.__pheviThis->iSysVSemId = iTmp;
    __hevThis.__pheviThis->bSysVOwner = bOwner;
    __hevThis.__pheviThis->bSet = (eInitState == tCIDLib::EEventStates::Reset);

    ::pthread_mutex_init(&__hevThis.__pheviThis->mtxThis, 0);

    return kCIDLib::True;
}
