//
// FILE NAME: CIDKernel_EventSemaphore_Linux.Cpp
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

    m_pheviThis(nullptr)
{
}

TEventHandle::TEventHandle(const TEventHandle& hevToCopy) :

    m_pheviThis(hevToCopy.m_pheviThis)
{
}

TEventHandle::~TEventHandle()
{
    m_pheviThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TEventHandle& TEventHandle::operator=(const TEventHandle& hevToAssign)
{
    if (this == &hevToAssign)
        return *this;

    m_pheviThis = hevToAssign.m_pheviThis;

    return *this;
}


tCIDLib::TBoolean
TEventHandle::operator==(const TEventHandle& hevToCompare) const
{
    return (m_pheviThis == hevToCompare.m_pheviThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TEventHandle::bIsValid() const
{
    return (m_pheviThis && m_pheviThis->c4RefCount);
}


tCIDLib::TVoid TEventHandle::Clear()
{
    m_pheviThis = 0;
}


tCIDLib::TVoid
TEventHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_pheviThis)
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

    m_pszName(nullptr)
{
}

TKrnlEvent::TKrnlEvent(const tCIDLib::TCh* const pszName) :

    m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
}

TKrnlEvent::~TKrnlEvent()
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
//  TKrnlEvent: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlEvent::bClose()
{
    if (m_hevThis.bIsValid())
    {
        ::pthread_mutex_lock(&m_hevThis.m_pheviThis->mtxThis);

        tCIDLib::TOSErrCode HostErr;
        if (!--m_hevThis.m_pheviThis->c4RefCount)
        {
            if(m_hevThis.m_pheviThis->iSysVSemId != -1)
            {
                if (m_hevThis.m_pheviThis->bSysVOwner)
                {
                    union semun SemUnion;
                    SemUnion.val = 0;

                    if (::semctl(m_hevThis.m_pheviThis->iSysVSemId
                                 , 0
                                 , IPC_RMID
                                 , SemUnion))
                    {
                        m_hevThis.m_pheviThis->c4RefCount++;
                        ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);
                        TKrnlError::SetLastHostError(errno);
                        return kCIDLib::False;
                    }
                }
            }
            else
            {
                HostErr = ::pthread_cond_destroy(&m_hevThis.m_pheviThis->condThis);
                if (HostErr)
                {
                    TKrnlError::SetLastHostError(HostErr);
                    return kCIDLib::False;
                }
            }

            ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);
            HostErr = ::pthread_mutex_destroy(&m_hevThis.m_pheviThis->mtxThis);
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete m_hevThis.m_pheviThis;
        }
        else
        {
            ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);
        }

        m_hevThis.m_pheviThis = nullptr;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEvent::bCreate(const tCIDLib::EEventStates eInitState, const tCIDLib::TBoolean bManual)
{
    if (m_hevThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (m_pszName)
    {
        tCIDLib::TBoolean bCreated;
        return bCreateNamed(eInitState, kCIDLib::True, bManual, bCreated);
    }

    m_hevThis.m_pheviThis = new TEventHandleImpl;
    m_hevThis.m_pheviThis->c4RefCount = 1;
    m_hevThis.m_pheviThis->iSysVSemId = -1;
    m_hevThis.m_pheviThis->bSet = (eInitState == tCIDLib::EEventStates::Reset);

    ::pthread_mutex_init(&m_hevThis.m_pheviThis->mtxThis, 0);

    tCIDLib::TSInt iErr = ::pthread_cond_init(&m_hevThis.m_pheviThis->condThis, 0);
    if (iErr)
    {
        ::pthread_mutex_destroy(&m_hevThis.m_pheviThis->mtxThis);
        delete m_hevThis.m_pheviThis;
        m_hevThis.m_pheviThis = nullptr;
        TKrnlError::SetLastHostError(iErr);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEvent::bOpenOrCreate(  const   tCIDLib::EEventStates   eInitState
                            , const tCIDLib::TBoolean       bManual
                            ,       tCIDLib::TBoolean&      bCreated)
{
    if (m_hevThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    return bCreateNamed(eInitState, kCIDLib::False, bManual, bCreated);
}


tCIDLib::TBoolean TKrnlEvent::bDuplicate(const TKrnlEvent& kevToDup)
{
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] m_pszName;
    m_pszName = nullptr;

    // Recreate the name
    if (kevToDup.m_pszName)
        m_pszName = TRawStr::pszReplicate(kevToDup.m_pszName);

    // Duplicate the handle
    m_hevThis = kevToDup.m_hevThis;
    if (m_hevThis.bIsValid())
    {
        ::pthread_mutex_lock(&m_hevThis.m_pheviThis->mtxThis);
        m_hevThis.m_pheviThis->c4RefCount++;
        ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bOpen()
{
    if (m_hevThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    m_hevThis.m_pheviThis = new TEventHandleImpl;
    m_hevThis.m_pheviThis->c4RefCount = 1;

    tCIDLib::TSInt iFlags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(m_pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmp = ::semget(key, 0, iFlags);
    if (iTmp == -1)
    {
        delete m_hevThis.m_pheviThis;
        m_hevThis.m_pheviThis = 0;
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    ::pthread_mutex_init(&m_hevThis.m_pheviThis->mtxThis, 0);

    m_hevThis.m_pheviThis->iSysVSemId = iTmp;
    m_hevThis.m_pheviThis->bSysVOwner = kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bReset()
{
    if (!m_hevThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (m_hevThis.m_pheviThis->iSysVSemId != -1)
    {
        union semun SemUnion;
        SemUnion.val = 1;

        if (::semctl(m_hevThis.m_pheviThis->iSysVSemId
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
        ::pthread_mutex_lock(&m_hevThis.m_pheviThis->mtxThis);

        m_hevThis.m_pheviThis->bSet = kCIDLib::True;

        ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bSetName(const tCIDLib::TCh* const pszName)
{
    if (m_hevThis.bIsValid())
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


tCIDLib::TBoolean TKrnlEvent::bTrigger()
{
    if (!m_hevThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (m_hevThis.m_pheviThis->iSysVSemId != -1)
    {
        union semun SemUnion;
        SemUnion.val = 0;

        if (::semctl(m_hevThis.m_pheviThis->iSysVSemId
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
        ::pthread_mutex_lock(&m_hevThis.m_pheviThis->mtxThis);

        m_hevThis.m_pheviThis->bSet = kCIDLib::False;
        ::pthread_cond_broadcast(&m_hevThis.m_pheviThis->condThis);

        ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bWaitFor(const tCIDLib::TCard4 c4Wait)
{
    if (!m_hevThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (c4Wait == kCIDLib::c4MaxWait)
    {
        if (m_hevThis.m_pheviThis->iSysVSemId != -1)
        {
            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = 0;
            SemBuf.sem_flg = 0;

            if (::semop(m_hevThis.m_pheviThis->iSysVSemId
                        , &SemBuf
                        , 1))
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            ::pthread_mutex_lock(&m_hevThis.m_pheviThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (m_hevThis.m_pheviThis->bSet && !HostErr)
            {
                HostErr = ::pthread_cond_wait(&m_hevThis.m_pheviThis->condThis
                                              , &m_hevThis.m_pheviThis->mtxThis);
            }

            ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);

            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

        }
    }
    else
    {
        if (m_hevThis.m_pheviThis->iSysVSemId != -1)
        {
            TKrnlLinux::TThreadTimer thtWaitFor(c4Wait);

            struct sembuf SemBuf;
            SemBuf.sem_num = 0;
            SemBuf.sem_op = 0;
            SemBuf.sem_flg = 0;

            if (!thtWaitFor.bBegin())
                return kCIDLib::False;

            if (::semop(m_hevThis.m_pheviThis->iSysVSemId
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

            ::pthread_mutex_lock(&m_hevThis.m_pheviThis->mtxThis);

            tCIDLib::TOSErrCode HostErr = 0;
            while (m_hevThis.m_pheviThis->bSet && !HostErr)
            {
                HostErr = ::pthread_cond_timedwait(&m_hevThis.m_pheviThis->condThis
                                                   , &m_hevThis.m_pheviThis->mtxThis
                                                   , &TimeSpec);
            }

            ::pthread_mutex_unlock(&m_hevThis.m_pheviThis->mtxThis);

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
TKrnlEvent::bCreateNamed(const  tCIDLib::EEventStates eInitState
                        , const tCIDLib::TBoolean     bFailIfExists
                        , const tCIDLib::TBoolean     bManual
                        ,       tCIDLib::TBoolean&    bCreated)
{
    tCIDLib::TSInt iFlags = IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    key_t key = TRawStr::hshHashStr(m_pszName, kCIDLib::i4MaxInt);
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

        // Not created, we opened an existing one
        bCreated = kCIDLib::False;
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

        // We created a new one
        bCreated = kCIDLib::True;        
    }

    m_hevThis.m_pheviThis = new TEventHandleImpl;
    m_hevThis.m_pheviThis->c4RefCount = 1;
    m_hevThis.m_pheviThis->iSysVSemId = iTmp;
    m_hevThis.m_pheviThis->bSysVOwner = bOwner;
    m_hevThis.m_pheviThis->bSet = (eInitState == tCIDLib::EEventStates::Reset);

    ::pthread_mutex_init(&m_hevThis.m_pheviThis->mtxThis, 0);

    return kCIDLib::True;
}
