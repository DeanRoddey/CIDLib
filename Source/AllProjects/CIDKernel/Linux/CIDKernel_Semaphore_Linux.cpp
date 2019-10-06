//
// FILE NAME: CIDKernel_Semaphore_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/28/1998
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

    m_phsemiThis(nullptr)
{
}

TSemaphoreHandle::TSemaphoreHandle(const TSemaphoreHandle& hsemToCopy) :

    m_phsemiThis(hsemToCopy.m_phsemiThis)
{
}

TSemaphoreHandle::~TSemaphoreHandle()
{
    m_phsemiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TSemaphoreHandle&
TSemaphoreHandle::operator=(const TSemaphoreHandle& hsemToAssign)
{
    if (this == &hsemToAssign)
        return *this;

    m_phsemiThis = hsemToAssign.m_phsemiThis;

    return *this;
}


tCIDLib::TBoolean
TSemaphoreHandle::operator==(const TSemaphoreHandle& hsemToCompare) const
{
    return (m_phsemiThis == hsemToCompare.m_phsemiThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TSemaphoreHandle::bIsValid() const
{
    return (m_phsemiThis && m_phsemiThis->c4RefCount);
}

tCIDLib::TVoid TSemaphoreHandle::Clear()
{
    m_phsemiThis = 0;
}

tCIDLib::TVoid
TSemaphoreHandle::FormatToStr(        tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phsemiThis)
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

    m_c4MaxCount(c4MaxCount)
    , m_pszName(nullptr)
{
}

TKrnlSemaphore::TKrnlSemaphore( const   tCIDLib::TCh* const pszName
                                , const tCIDLib::TCard4     c4MaxCount) :
    m_c4MaxCount(c4MaxCount)
    , m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
}

TKrnlSemaphore::~TKrnlSemaphore()
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
//  TKrnlSemaphore: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TKrnlSemaphore::bClose()
{
    if (m_hsemThis.bIsValid())
    {
        ::pthread_mutex_lock(&m_hsemThis.m_phsemiThis->mtxThis);

        tCIDLib::TOSErrCode HostErr;

        if (!--m_hsemThis.m_phsemiThis->c4RefCount)
        {
            if(m_hsemThis.m_phsemiThis->iSysVSemId != -1)
            {
                if (m_hsemThis.m_phsemiThis->bSysVOwner)
                {
                    union semun SemUnion;
                    SemUnion.val = 0;

                    if (::semctl(m_hsemThis.m_phsemiThis->iSysVSemId
                                 , 0
                                 , IPC_RMID
                                 , SemUnion))
                    {
                        m_hsemThis.m_phsemiThis->c4RefCount++;
                        ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
                        TKrnlError::SetLastHostError(errno);
                        return kCIDLib::False;
                    }
                }
            }
            else
            {
                HostErr = ::pthread_cond_destroy(&m_hsemThis.m_phsemiThis->condThis);
                if (HostErr)
                {
                    TKrnlError::SetLastHostError(HostErr);
                    return kCIDLib::False;
                }
            }

            ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
            HostErr = ::pthread_mutex_destroy(&m_hsemThis.m_phsemiThis->mtxThis);
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete m_hsemThis.m_phsemiThis;
        }
        else
        {
            ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
        }

        m_hsemThis.m_phsemiThis = 0;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bCreate(const tCIDLib::TCard4 c4InitCount)
{
    if (m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (c4InitCount > m_c4MaxCount)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    if (m_pszName)
    {
        tCIDLib::TBoolean bDummy;
        return bCreateNamed(c4InitCount, kCIDLib::True, bDummy);
    }

    m_hsemThis.m_phsemiThis = new TSemaphoreHandleImpl;
    m_hsemThis.m_phsemiThis->c4RefCount = 1;
    m_hsemThis.m_phsemiThis->iSysVSemId = -1;
    m_hsemThis.m_phsemiThis->c4CurCount = c4InitCount;

    ::pthread_mutex_init(&m_hsemThis.m_phsemiThis->mtxThis, 0);

    tCIDLib::TSInt HostErr = ::pthread_cond_init(&m_hsemThis.m_phsemiThis->condThis, 0);
    if (HostErr)
    {
        ::pthread_mutex_destroy(&m_hsemThis.m_phsemiThis->mtxThis);
        delete m_hsemThis.m_phsemiThis;
        m_hsemThis.m_phsemiThis = 0;
        TKrnlError::SetLastHostError(HostErr);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSemaphore::bOpenOrCreate(tCIDLib::TBoolean& bCreated, const tCIDLib::TCard4 c4InitCount)
{
    if (m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    return bCreateNamed(c4InitCount, kCIDLib::False, bCreated);
}


tCIDLib::TBoolean TKrnlSemaphore::bDuplicate(const TKrnlSemaphore& ksemToDup)
{
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] m_pszName;
    m_pszName = 0;

    // Recreate the name
    if (ksemToDup.m_pszName)
        m_pszName = TRawStr::pszReplicate(ksemToDup.m_pszName);

    // Copy max count
    m_c4MaxCount = ksemToDup.m_c4MaxCount;

    // Duplicate the handle
    m_hsemThis = ksemToDup.m_hsemThis;
    if (m_hsemThis.bIsValid())
    {
        ::pthread_mutex_lock(&m_hsemThis.m_phsemiThis->mtxThis);
        m_hsemThis.m_phsemiThis->c4RefCount++;
        ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bEnter(const tCIDLib::TCard4 c4MilliSecs)
{
    if (!m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (c4MilliSecs == kCIDLib::c4MaxWait)
    {
        if (m_hsemThis.m_phsemiThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (::semop(m_hsemThis.m_phsemiThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            ::pthread_mutex_lock(&m_hsemThis.m_phsemiThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (!m_hsemThis.m_phsemiThis->c4CurCount && !HostErr)
            {
                HostErr = ::pthread_cond_wait(&m_hsemThis.m_phsemiThis->condThis
                                              , &m_hsemThis.m_phsemiThis->mtxThis);
            }

            if (HostErr)
            {
                ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            if (m_hsemThis.m_phsemiThis->c4CurCount)
                m_hsemThis.m_phsemiThis->c4CurCount--;

            ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
        }
    }
    else
    {
        if (m_hsemThis.m_phsemiThis->iSysVSemId != -1)
        {
            TKrnlLinux::TThreadTimer thtWaitFor(c4MilliSecs);

            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = -1;
            SemBuf.sem_flg = 0;

            if (!thtWaitFor.bBegin())
                return kCIDLib::False;

            if (::semop(m_hsemThis.m_phsemiThis->iSysVSemId
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

            ::pthread_mutex_lock(&m_hsemThis.m_phsemiThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (!m_hsemThis.m_phsemiThis->c4CurCount && !HostErr)
            {
                HostErr = ::pthread_cond_timedwait(&m_hsemThis.m_phsemiThis->condThis
                                                   , &m_hsemThis.m_phsemiThis->mtxThis
                                                   , &TimeSpec);
            }

            if (HostErr)
            {
                ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            if (m_hsemThis.m_phsemiThis->c4CurCount)
                m_hsemThis.m_phsemiThis->c4CurCount--;

            ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
        }
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bExit()
{
    if (!m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (m_hsemThis.m_phsemiThis->iSysVSemId != -1)
    {
        struct sembuf SemBuf;
        SemBuf.sem_num = 0;
        SemBuf.sem_op = 1;
        SemBuf.sem_flg = 0;

        if (::semop(m_hsemThis.m_phsemiThis->iSysVSemId, &SemBuf, 1))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }
    else
    {
        ::pthread_mutex_lock(&m_hsemThis.m_phsemiThis->mtxThis);

        if (++m_hsemThis.m_phsemiThis->c4CurCount > m_c4MaxCount)
        {
            m_hsemThis.m_phsemiThis->c4CurCount = m_c4MaxCount;
            ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_TooMany);
            return kCIDLib::False;
        }

        if (m_hsemThis.m_phsemiThis->c4CurCount == 1)
            ::pthread_cond_signal(&m_hsemThis.m_phsemiThis->condThis);

        ::pthread_mutex_unlock(&m_hsemThis.m_phsemiThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bOpen()
{
    if (m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    m_hsemThis.m_phsemiThis = new TSemaphoreHandleImpl;

    tCIDLib::TSInt iFlags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(m_pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmp = ::semget(key, 0, iFlags);
    if (iTmp == -1)
    {
        delete m_hsemThis.m_phsemiThis;
        m_hsemThis.m_phsemiThis = 0;
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    ::pthread_mutex_init(&m_hsemThis.m_phsemiThis->mtxThis, 0);

    m_hsemThis.m_phsemiThis->c4RefCount = 1;
    m_hsemThis.m_phsemiThis->iSysVSemId = iTmp;
    m_hsemThis.m_phsemiThis->bSysVOwner = kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bSetName(const tCIDLib::TCh* const pszName)
{
    if (m_hsemThis.bIsValid())
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


// ---------------------------------------------------------------------------
//  TKrnlSemaphore: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSemaphore::bCreateNamed(const  tCIDLib::TCard4     c4InitCount
                            , const tCIDLib::TBoolean   bFailIfExists
                            ,       tCIDLib::TBoolean&  bCreated)
{
    tCIDLib::TSInt iFlags = IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(m_pszName, kCIDLib::i4MaxInt);
    tCIDLib::TBoolean bOwner = kCIDLib::False;

    // Only need to set if we create it
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
        SemUnion.val = c4InitCount;

        if (::semctl(iTmp, 0, SETVAL, SemUnion))
        {
            TKrnlError::SetLastHostError(errno);
            ::semctl(iTmp, 0, IPC_RMID, SemUnion);
            return kCIDLib::False;
        }

        bCreated = kCIDLib::True;
    }

    m_hsemThis.m_phsemiThis = new TSemaphoreHandleImpl;
    m_hsemThis.m_phsemiThis->c4RefCount = 1;
    m_hsemThis.m_phsemiThis->iSysVSemId = iTmp;
    m_hsemThis.m_phsemiThis->bSysVOwner = bOwner;
    m_hsemThis.m_phsemiThis->c4CurCount = c4InitCount;

    ::pthread_mutex_init(&m_hsemThis.m_phsemiThis->mtxThis, 0);

    return kCIDLib::True;
}
