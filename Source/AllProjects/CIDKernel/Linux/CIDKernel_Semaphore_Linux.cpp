//
// FILE NAME: CIDKernel_Semaphore_Linux.Cpp
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
//  TKrnlSemaphore.
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
//   CLASS: TSemaphoreHandle
//  PREFIX: hsem
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSemaphoreHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TSemaphoreHandle::TSemaphoreHandle() :
    __phsemiThis(0)
{
}

TSemaphoreHandle::TSemaphoreHandle(const TSemaphoreHandle& hsemToCopy) :
    __phsemiThis(hsemToCopy.__phsemiThis)
{
}

TSemaphoreHandle::~TSemaphoreHandle()
{
    __phsemiThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TSemaphoreHandle&
TSemaphoreHandle::operator=(const TSemaphoreHandle& hsemToAssign)
{
    if (this == &hsemToAssign)
        return *this;

    __phsemiThis = hsemToAssign.__phsemiThis;

    return *this;
}


tCIDLib::TBoolean
TSemaphoreHandle::operator==(const TSemaphoreHandle& hsemToCompare) const
{
    return (__phsemiThis == hsemToCompare.__phsemiThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TSemaphoreHandle::bValid() const
{
    return (__phsemiThis && __phsemiThis->c4RefCount);
}

tCIDLib::TVoid TSemaphoreHandle::Clear()
{
    __phsemiThis = 0;
}

tCIDLib::TVoid
TSemaphoreHandle::FormatToStr(        tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(__phsemiThis)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlSemaphore
//  PREFIX: ksem
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSemaphore: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSemaphore::TKrnlSemaphore(const tCIDLib::TCard4 c4MaxCount) :
    __c4MaxCount(c4MaxCount)
    , __pszName(0)
{
}

TKrnlSemaphore::TKrnlSemaphore( const   tCIDLib::TCh* const pszName
                                , const tCIDLib::TCard4     c4MaxCount) :
    __c4MaxCount(c4MaxCount)
    , __pszName(0)
{
    if (pszName)
        __pszName = TRawStr::pszReplicate(pszName);
}

TKrnlSemaphore::~TKrnlSemaphore()
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
//  TKrnlSemaphore: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSemaphore::bValid() const
{
    return __hsemThis.bValid();
}


tCIDLib::TBoolean TKrnlSemaphore::bClose()
{
    if (__hsemThis.bValid())
    {
        ::pthread_mutex_lock(&__hsemThis.__phsemiThis->mtxThis);

        tCIDLib::TOSErrCode HostErr;

        if (!--__hsemThis.__phsemiThis->c4RefCount)
        {
            if(__hsemThis.__phsemiThis->iSysVSemId != -1)
            {
                if (__hsemThis.__phsemiThis->bSysVOwner)
                {
                    union semun SemUnion;
                    SemUnion.val = 0;

                    if (::semctl(__hsemThis.__phsemiThis->iSysVSemId
                                 , 0
                                 , IPC_RMID
                                 , SemUnion))
                    {
                        __hsemThis.__phsemiThis->c4RefCount++;
                        ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
                        TKrnlError::SetLastHostError(errno);
                        return kCIDLib::False;
                    }
                }
            }
            else
            {
                HostErr = ::pthread_cond_destroy(&__hsemThis.__phsemiThis->condThis);
                if (HostErr)
                {
                    TKrnlError::SetLastHostError(HostErr);
                    return kCIDLib::False;
                }
            }

            ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
            HostErr = ::pthread_mutex_destroy(&__hsemThis.__phsemiThis->mtxThis);
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete __hsemThis.__phsemiThis;
        }
        else
        {
            ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
        }

        __hsemThis.__phsemiThis = 0;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bCreate(const tCIDLib::TCard4 c4InitCount)
{
    if (__hsemThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (c4InitCount > __c4MaxCount)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    if (__pszName)
        return __bCreateNamed(c4InitCount, kCIDLib::True);

    __hsemThis.__phsemiThis = new TSemaphoreHandleImpl;
    __hsemThis.__phsemiThis->c4RefCount = 1;
    __hsemThis.__phsemiThis->iSysVSemId = -1;
    __hsemThis.__phsemiThis->c4CurCount = c4InitCount;

    ::pthread_mutex_init(&__hsemThis.__phsemiThis->mtxThis, 0);

    tCIDLib::TSInt HostErr = ::pthread_cond_init(&__hsemThis.__phsemiThis->condThis, 0);
    if (HostErr)
    {
        ::pthread_mutex_destroy(&__hsemThis.__phsemiThis->mtxThis);
        delete __hsemThis.__phsemiThis;
        __hsemThis.__phsemiThis = 0;
        TKrnlError::SetLastHostError(HostErr);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSemaphore::bCreateOrOpen(const tCIDLib::TCard4 c4InitCount)
{
    if (__hsemThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!__pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    return __bCreateNamed(c4InitCount, kCIDLib::False);
}


tCIDLib::TBoolean TKrnlSemaphore::bDuplicate(const TKrnlSemaphore& ksemToDup)
{
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] __pszName;
    __pszName = 0;

    // Recreate the name
    if (ksemToDup.__pszName)
        __pszName = TRawStr::pszReplicate(ksemToDup.__pszName);

    // Copy max count
    __c4MaxCount = ksemToDup.__c4MaxCount;

    // Duplicate the handle
    __hsemThis = ksemToDup.__hsemThis;
    if (__hsemThis.bValid())
    {
        ::pthread_mutex_lock(&__hsemThis.__phsemiThis->mtxThis);
        __hsemThis.__phsemiThis->c4RefCount++;
        ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bEnter(const tCIDLib::TCard4 c4MilliSecs)
{
    if (!__hsemThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (c4MilliSecs == kCIDLib::c4MaxWait)
    {
        if (__hsemThis.__phsemiThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (::semop(__hsemThis.__phsemiThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            ::pthread_mutex_lock(&__hsemThis.__phsemiThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (!__hsemThis.__phsemiThis->c4CurCount && !HostErr)
            {
                HostErr = ::pthread_cond_wait(&__hsemThis.__phsemiThis->condThis
                                              , &__hsemThis.__phsemiThis->mtxThis);
            }

            if (HostErr)
            {
                ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            if (__hsemThis.__phsemiThis->c4CurCount)
                __hsemThis.__phsemiThis->c4CurCount--;

            ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
        }
    }
    else
    {
        if (__hsemThis.__phsemiThis->iSysVSemId != -1)
        {
            TKrnlLinux::TThreadTimer thtWaitFor(c4MilliSecs);

            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (!thtWaitFor.bBegin())
                return kCIDLib::False;

            if (::semop(__hsemThis.__phsemiThis->iSysVSemId
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
            TimeSpec.tv_sec = ::time(0) + (c4MilliSecs / 1000);
            TimeSpec.tv_nsec = (c4MilliSecs % 1000) * 1000000;

            ::pthread_mutex_lock(&__hsemThis.__phsemiThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (!__hsemThis.__phsemiThis->c4CurCount && !HostErr)
            {
                HostErr = ::pthread_cond_timedwait(&__hsemThis.__phsemiThis->condThis
                                                   , &__hsemThis.__phsemiThis->mtxThis
                                                   , &TimeSpec);
            }

            if (HostErr)
            {
                ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            if (__hsemThis.__phsemiThis->c4CurCount)
                __hsemThis.__phsemiThis->c4CurCount--;

            ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
        }
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bExit()
{
    if (!__hsemThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (__hsemThis.__phsemiThis->iSysVSemId != -1)
    {
        struct sembuf SemBuf;
        SemBuf.sem_num = 0;
        SemBuf.sem_op = 1;
        SemBuf.sem_flg = 0;

        if (::semop(__hsemThis.__phsemiThis->iSysVSemId, &SemBuf, 1))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }
    else
    {
        ::pthread_mutex_lock(&__hsemThis.__phsemiThis->mtxThis);

        if (++__hsemThis.__phsemiThis->c4CurCount > __c4MaxCount)
        {
            __hsemThis.__phsemiThis->c4CurCount = __c4MaxCount;
            ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_TooMany);
            return kCIDLib::False;
        }

        if (__hsemThis.__phsemiThis->c4CurCount == 1)
            ::pthread_cond_signal(&__hsemThis.__phsemiThis->condThis);

        ::pthread_mutex_unlock(&__hsemThis.__phsemiThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bOpen()
{
    if (__hsemThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!__pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    __hsemThis.__phsemiThis = new TSemaphoreHandleImpl;

    tCIDLib::TSInt iFlags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(__pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmp = ::semget(key, 0, iFlags);
    if (iTmp == -1)
    {
        delete __hsemThis.__phsemiThis;
        __hsemThis.__phsemiThis = 0;
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    ::pthread_mutex_init(&__hsemThis.__phsemiThis->mtxThis, 0);

    __hsemThis.__phsemiThis->c4RefCount = 1;
    __hsemThis.__phsemiThis->iSysVSemId = iTmp;
    __hsemThis.__phsemiThis->bSysVOwner = kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bSetName(const tCIDLib::TCh* const pszName)
{
    if (__hsemThis.bValid())
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


// ---------------------------------------------------------------------------
//  TKrnlSemaphore: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSemaphore::__bCreateNamed( const   tCIDLib::TCard4     c4InitCount
                                , const tCIDLib::TBoolean   bFailIfExists)
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
        SemUnion.val = c4InitCount;

        if (::semctl(iTmp, 0, SETVAL, SemUnion))
        {
            TKrnlError::SetLastHostError(errno);
            ::semctl(iTmp, 0, IPC_RMID, SemUnion);
            return kCIDLib::False;
        }
    }

    __hsemThis.__phsemiThis = new TSemaphoreHandleImpl;
    __hsemThis.__phsemiThis->c4RefCount = 1;
    __hsemThis.__phsemiThis->iSysVSemId = iTmp;
    __hsemThis.__phsemiThis->bSysVOwner = bOwner;
    __hsemThis.__phsemiThis->c4CurCount = c4InitCount;

    ::pthread_mutex_init(&__hsemThis.__phsemiThis->mtxThis, 0);

    return kCIDLib::True;
}
