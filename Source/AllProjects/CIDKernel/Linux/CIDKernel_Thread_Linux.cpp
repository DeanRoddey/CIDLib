//
// FILE NAME: CIDKernel_Thread_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 11/21/1998
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
//  This file provides the Linux specific implementation of the TKrnlThread
//  classes.
//
// CAVEATS/GOTCHAS:
//
//  1)  Make sure that what we use here as an invalid thread id, is the same
//      as what we defined kCIDLib::tidInvalid to be!
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

namespace CIDKernel_Thread_Linux
{
    // ---------------------------------------------------------------------------
    //  Local types
    //
    //  TStartupData
    //      This is used locally to pass the startup data from the bBeginThread
    //      method to the internal thread start method. It is used to pass in
    //      the actual CIDLib level method that will be called, and the user data
    //      to be passed along.
    // ---------------------------------------------------------------------------
    struct TStartupData
    {
        TKrnlThread::TCallBack  pfnCallBack;
        tCIDLib::TVoid*         pUserData;
        TThread*                pthrStarting;
        TKrnlThread*            pkthrStarting;
        pthread_mutex_t*        pmtxStart;
        tCIDLib::TCh*           pszName;
    };

    // ---------------------------------------------------------------------------
    //  Local data
    //
    //  CIDKernel_Thread_Linux::tidPrimary
    //      Used to save the primary thread id. This is the thread that our
    //      initialization occurs on, so we just store away the calling thread
    //      id during it.
    // ---------------------------------------------------------------------------
    tCIDLib::TThreadId  tidPrimary;


    // -----------------------------------------------------------------------
    //  Our per-thread info structure and storage per thread
    // -----------------------------------------------------------------------
    struct  TPerThreadInfo
    {
        tCIDLib::TBoolean   bIsGUIThread = kCIDLib::False;
    };

    thread_local TPerThreadInfo ptiThread;


    // ---------------------------------------------------------------------------
    //  Local methods
    // ---------------------------------------------------------------------------

    //
    //  This is the function that is really started up when a new thread is
    //  started. First it waits until the calling thread is done initializing
    //  the thread handle. Then it calls the real CIDLib level thread function,
    //  passing along the information it needs.
    //
    tCIDLib::TVoid* ThreadStart(tCIDLib::TVoid* pStartUp)
    {
        TStartupData* pData = static_cast<TStartupData*>(pStartUp);

        //
        // We're waiting for the parent thread to initialize the thread
        // handle.
        //
        ::pthread_mutex_lock(pData->pmtxStart);

        // Ok, it's done. Now clean up the mutex.
        ::pthread_mutex_unlock(pData->pmtxStart);
        ::pthread_mutex_destroy(pData->pmtxStart);

        //
        //  Set the thread info for the program error signals' benefit. This isn't a
        //  leak. This guy sets himself as the per-thread info for this thread.
        //
        new TKrnlLinux::TKrnlThreadInfo(pData->pkthrStarting, pData->pszName);

        // Now fire it up
        const tCIDLib::EExitCodes eRet = pData->pfnCallBack(pData->pthrStarting, pData->pUserData);

        delete pData;

        return (tCIDLib::TVoid*)eRet;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TCIDKrnlModule
//  PREFIX: hmod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermThread(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        CIDKernel_Thread_Linux::tidPrimary = ::pthread_self();
    }

    return
    (
        TKrnlLinux::TThreadTimer::bInitTerm(eState)
        && TKrnlLinux::TKrnlThreadInfo::bInitTerm(eState)
    );
}


// ---------------------------------------------------------------------------
//   CLASS: TThreadHandle
//  PREFIX: hthr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TThreadHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TThreadHandle::TThreadHandle() :

    m_phthriThis(nullptr)
{
    m_phthriThis = new TThreadHandleImpl;
    m_phthriThis->tidThis = kCIDLib::tidInvalid;
    m_phthriThis->eExit = tCIDLib::EExitCodes::Normal;
    m_phthriThis->bJoined = kCIDLib::False;
}

TThreadHandle::TThreadHandle(const TThreadHandle& hthrToCopy) :

    m_phthriThis(nullptr)
{
    m_phthriThis->tidThis = hthrToCopy.m_phthriThis->tidThis;
    m_phthriThis->eExit = hthrToCopy.m_phthriThis->eExit;
    m_phthriThis->bJoined = hthrToCopy.m_phthriThis->bJoined;
}

TThreadHandle::~TThreadHandle()
{
    delete m_phthriThis;
    m_phthriThis = nullptr;
}


// -------------------------------------------------------------------
//  TThreadHandle: Public operators
// -------------------------------------------------------------------
TThreadHandle& TThreadHandle::operator=(const TThreadHandle& hthrToAssign)
{
    if (this == &hthrToAssign)
        return *this;

    m_phthriThis->tidThis = hthrToAssign.m_phthriThis->tidThis;
    m_phthriThis->eExit = hthrToAssign.m_phthriThis->eExit;
    m_phthriThis->bJoined = hthrToAssign.m_phthriThis->bJoined;

    return *this;
}


tCIDLib::TBoolean
TThreadHandle::operator==(const TThreadHandle& hthrToCompare) const
{
    return ::pthread_equal
    (
        m_phthriThis->tidThis, hthrToCompare.m_phthriThis->tidThis
    );
}



// -------------------------------------------------------------------
//  TThreadHandle: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TThreadHandle::bIsValid() const
{
    return (m_phthriThis->tidThis != kCIDLib::tidInvalid);
}

tCIDLib::TVoid TThreadHandle::Clear()
{
    m_phthriThis->tidThis = kCIDLib::tidInvalid;
    m_phthriThis->eExit = tCIDLib::EExitCodes::Normal;
    m_phthriThis->bJoined = kCIDLib::False;
}

tCIDLib::TVoid
TThreadHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    tCIDLib::TZStr64    szTmp;
    TRawStr::bFormatVal(tCIDLib::TCard4(m_phthriThis->tidThis), szTmp, 64);
    TRawStr::CopyStr(pszToFill, szTmp, c4MaxChars);
}


tCIDLib::TThreadId TThreadHandle::tidThis() const
{
    return m_phthriThis->tidThis;
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlThread
//  PREFIX: kthr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlThread: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlThread::bIsCaller(const TKrnlThread& kthrToTest)
{
    return
    (
        ::pthread_equal
        (
            kthrToTest.m_hthrThis.m_phthriThis->tidThis, ::pthread_self()
        )
    );
}


// Return whether the callng thread is a GUI marked thread
tCIDLib::TBoolean TKrnlThread::bIsCallerGUIThread()
{
    return CIDKernel_Thread_Linux::ptiThread.bIsGUIThread;
}


tCIDLib::TBoolean
TKrnlThread::bPriorityOf(const  TKrnlThread&            kthrToQuery
                        ,       tCIDLib::EPrioLevels&   eToFill)
{
    // First, figure out which scheduler we're using
    struct sched_param sch;
    tCIDLib::TSInt iPolicy;
    tCIDLib::TOSErrCode errc = ::pthread_getschedparam
    (
        kthrToQuery.m_hthrThis.m_phthriThis->tidThis, &iPolicy, &sch
    );
    if (errc)
    {
        TKrnlError::SetLastHostError(errc);
        return kCIDLib::False;
    }

    if (iPolicy == SCHED_OTHER)
    {
        // We're using the default scheduler
        tCIDLib::TInt4 i4Prio = ::getpriority(PRIO_PROCESS, 0);

        // The range is -20 to 20, but most threads are at 0
        // It's goofy, but lower values are higher
        if (i4Prio < -10)
            eToFill = tCIDLib::EPrioLevels::Highest;
        else if (i4Prio < 0)
            eToFill = tCIDLib::EPrioLevels::AboveNormal;
        else if (i4Prio == 0)
            eToFill = tCIDLib::EPrioLevels::Normal;
        else if (i4Prio < 10)
            eToFill = tCIDLib::EPrioLevels::BelowNormal;
        else
            eToFill = tCIDLib::EPrioLevels::Lowest;
    }
    else
    {
        // This is probably a real-time app because it's
        // using either the FIFO or round-robin scheduler
        // First, figure out the range
        // It should be 0 to 99, but we need to keep the code flexible
        errno = 0;
        tCIDLib::TInt4 i4MinPrio = ::sched_get_priority_min(iPolicy);
        if (i4MinPrio == -1 && errno)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
        errno = 0;
        tCIDLib::TInt4 i4MaxPrio = ::sched_get_priority_max(iPolicy);
        if (i4MaxPrio == -1 && errno)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TInt4 i4Middle = (i4MaxPrio - i4MinPrio) / 2;

        // Now calculate the relative priority of the value for this thread's
        // priority that we already retrieved
        // For these schedulers, lower values are actually lower
        // These values are based on what would be set by the CID set priority
        // function.
        if (sch.sched_priority == i4MinPrio)
            eToFill = tCIDLib::EPrioLevels::Lowest;
        else if (sch.sched_priority < i4MinPrio + i4Middle)
            eToFill = tCIDLib::EPrioLevels::BelowNormal;
        else if (sch.sched_priority == i4MinPrio + i4Middle)
            eToFill = tCIDLib::EPrioLevels::Normal;
        else if (sch.sched_priority < i4MaxPrio)
            eToFill = tCIDLib::EPrioLevels::AboveNormal;
        else
            eToFill = tCIDLib::EPrioLevels::Highest;
    }

    return kCIDLib::True;
}


tCIDLib::TVoid TKrnlThread::Exit(const tCIDLib::EExitCodes eExitCode)
{
    ::pthread_exit(reinterpret_cast<tCIDLib::TVoid*>(eExitCode));
}


tCIDLib::TVoid TKrnlThread::Sleep(const tCIDLib::TCard4 c4MilliSeconds)
{
    struct timespec TimeRequested;
    struct timespec TimeRemaining;
    TimeRequested.tv_sec = c4MilliSeconds / 1000;
    TimeRequested.tv_nsec = (c4MilliSeconds % 1000) * 1000000;
    while (::nanosleep(&TimeRequested, &TimeRemaining) == EINTR)
    {
        TimeRequested.tv_sec = TimeRemaining.tv_sec;
        TimeRequested.tv_nsec = TimeRemaining.tv_nsec;
    }
}


tCIDLib::TThreadId TKrnlThread::tidCaller()
{
    return ::pthread_self();
}


tCIDLib::TThreadId TKrnlThread::tidPrimary()
{
    return CIDKernel_Thread_Linux::tidPrimary;
}


//
//  Handle per-thread initializiation. We may need to call this for a special
//  case other than from the internal thread startup method above. For now we
//  don't have any.
//
tCIDLib::TVoid TKrnlThread::KrnlThreadInit()
{

}



// ---------------------------------------------------------------------------
//  TKrnlThread: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlThread::TKrnlThread()
{
}

TKrnlThread::~TKrnlThread()
{
    Orphan();
}


// ---------------------------------------------------------------------------
//  TKrnlThread: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlThread::bAdoptCaller()
{
    if (m_hthrThis.m_phthriThis->tidThis != kCIDLib::tidInvalid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadySet);
        return kCIDLib::False;
    }

    m_hthrThis.m_phthriThis->tidThis = ::pthread_self();
    m_hthrThis.m_phthriThis->eExit = tCIDLib::EExitCodes::Normal;
    m_hthrThis.m_phthriThis->bJoined = kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlThread::bIsRunning(tCIDLib::TBoolean& bToFill) const
{
    bToFill = (::pthread_kill(m_hthrThis.m_phthriThis->tidThis, 0) == 0);
    return kCIDLib::True;
}


// Return whether we are marked as the GUI thread, which is in the per-thread data
tCIDLib::TBoolean TKrnlThread::bIsGUIThread() const
{
    return CIDKernel_Thread_Linux::ptiThread.bIsGUIThread;
}


tCIDLib::TBoolean
TKrnlThread::bBeginThread(  const   TKrnlThread::TCallBack  pfnFunc
                            , const tCIDLib::TCard4         c4StackSize
                            ,       tCIDLib::TVoid* const   pData
                            , const tCIDLib::EThreadFlags   eFlags
                            ,       TThread* const          pthrStart
                            , const tCIDLib::TCh* const     pszThreadName)
{
    if (m_hthrThis.m_phthriThis->tidThis != kCIDLib::tidInvalid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    pthread_mutex_t mtxStart = PTHREAD_MUTEX_INITIALIZER;

    pthread_t tidTmp = kCIDLib::tidInvalid;
    CIDKernel_Thread_Linux::TStartupData* pStartupData = new CIDKernel_Thread_Linux::TStartupData;
    pStartupData->pfnCallBack = pfnFunc;
    pStartupData->pUserData = pData;
    pStartupData->pthrStarting = pthrStart;
    pStartupData->pmtxStart = &mtxStart;
    pStartupData->pszName = TRawStr::pszReplicate(pszThreadName);
    pStartupData->pkthrStarting = this;

    ::pthread_mutex_lock(&mtxStart);

    tCIDLib::TOSErrCode HostErr = ::pthread_create
    (
        &tidTmp
        , 0
        , CIDKernel_Thread_Linux::ThreadStart
        , pStartupData
    );

    if (HostErr)
    {
        ::pthread_mutex_unlock(&mtxStart);
        ::pthread_mutex_destroy(&mtxStart);
        delete [] pStartupData->pszName;
        delete pStartupData;
        TKrnlError::SetLastHostError(HostErr);
        return kCIDLib::False;
    }

    m_hthrThis.m_phthriThis->tidThis = tidTmp;

    // The thread we just started is waiting on this, so it will
    // destroy the mutex.
    ::pthread_mutex_unlock(&mtxStart);

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlThread::bBlockMyself()
{
    if (m_hthrThis.m_phthriThis->tidThis != ::pthread_self())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_NotThisThread);
        return kCIDLib::False;
    }
    ::pthread_kill(m_hthrThis.m_phthriThis->tidThis, SIGSTOP);
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlThread::bPriority(tCIDLib::EPrioLevels& eToFill) const
{
    // Just call the static one and pass ourself as the thread
    return TKrnlThread::bPriorityOf(*this, eToFill);
}


tCIDLib::TBoolean TKrnlThread::bSetPriority(const tCIDLib::EPrioLevels eLevel)
{
    // First, figure out which scheduler we're using
    struct sched_param sch;
    tCIDLib::TSInt iPolicy;
    tCIDLib::TOSErrCode errc = ::pthread_getschedparam(m_hthrThis.m_phthriThis->tidThis, &iPolicy, &sch);
    if (errc)
    {
        TKrnlError::SetLastHostError(errc);
        return kCIDLib::False;
    }

    if (iPolicy == SCHED_OTHER)
    {
        // We're using the default scheduler

        tCIDLib::TInt4 i4Prio;
        // The range is -20 to 20, but most threads are at 0
        switch (eLevel)
        {
        case tCIDLib::EPrioLevels::Lowest:
            i4Prio = 20;
            break;
        case tCIDLib::EPrioLevels::BelowNormal:
            i4Prio = 10;
            break;
        case tCIDLib::EPrioLevels::Normal:
            i4Prio = 0;
            break;
        case tCIDLib::EPrioLevels::AboveNormal:
            i4Prio = -10;
            break;
        case tCIDLib::EPrioLevels::Highest:
            i4Prio = -20;
            break;
        }

        // This could easily fail, because only the superuser
        // can lower (read "raise") a thread's priority
        if (::setpriority(PRIO_PROCESS, 0, i4Prio))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }
    else
    {
        // This is probably a real-time app because it's
        // using either the FIFO or round-robin scheduler
        // First, figure out the range
        // It should be 0 to 99, but we need to keep the code flexible
        errno = 0;
        tCIDLib::TInt4 i4MinPrio = ::sched_get_priority_min(iPolicy);
        if (i4MinPrio == -1 && errno)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        // If the minimum is zero, then bump it up
        if (i4MinPrio == 0)
            i4MinPrio++;

        errno = 0;
        tCIDLib::TInt4 i4MaxPrio = ::sched_get_priority_max(iPolicy);
        if (i4MaxPrio == -1 && errno)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        const tCIDLib::TInt4 i4ChunkSize
        (
            (i4MaxPrio - i4MinPrio + 1) / tCIDLib::c4EnumOrd(tCIDLib::EPrioLevels::Count)
        );

        tCIDLib::TInt4 i4Prio;
        switch (eLevel)
        {
        case tCIDLib::EPrioLevels::Lowest:
            i4Prio = i4MinPrio;
            break;
        case tCIDLib::EPrioLevels::BelowNormal:
            i4Prio = i4MinPrio + i4ChunkSize;
            break;
        case tCIDLib::EPrioLevels::Normal:
            i4Prio = (i4MaxPrio - i4MinPrio) / 2;
            break;
        case tCIDLib::EPrioLevels::AboveNormal:
            i4Prio = (i4MaxPrio - i4MinPrio) / 2 + i4ChunkSize;
            break;
        case tCIDLib::EPrioLevels::Highest:
            i4Prio = i4MaxPrio;
            break;
        }

        sch.sched_priority = i4Prio;
        errc = ::pthread_setschedparam(m_hthrThis.m_phthriThis->tidThis, iPolicy, &sch);
        if (errc)
        {
            TKrnlError::SetLastHostError(errc);
            return kCIDLib::False;
        }
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlThread::bQueryExitCode(tCIDLib::EExitCodes& eToFill) const
{
    if (!::pthread_kill(m_hthrThis.m_phthriThis->tidThis, 0))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_StillRunning);
        return kCIDLib::False;
    }

    if (!m_hthrThis.m_phthriThis->bJoined)
    {
        tCIDLib::TVoid* pHostExit = 0;
        tCIDLib::TOSErrCode HostErr = ::pthread_join(m_hthrThis.m_phthriThis->tidThis
                                                     , &pHostExit);
        if (HostErr)
        {
            TKrnlError::SetLastHostError(HostErr);
            return kCIDLib::False;
        }

        m_hthrThis.m_phthriThis->eExit = tCIDLib::EExitCodes(tCIDLib::TSInt(pHostExit));
        m_hthrThis.m_phthriThis->bJoined = kCIDLib::True;
        m_hthrThis.m_phthriThis->tidThis = kCIDLib::tidInvalid;
    }

    eToFill = m_hthrThis.m_phthriThis->eExit;

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlThread::bUnblock()
{
    if (::pthread_kill(m_hthrThis.m_phthriThis->tidThis, SIGCONT))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// <TBD> Deal with new bState parameter
tCIDLib::TBoolean
TKrnlThread::bWaitForDeath(         tCIDLib::TBoolean&      bState
                            ,       tCIDLib::EExitCodes&    eToFill
                            , const tCIDLib::TCard4         c4MilliSeconds) const
{
    if (m_hthrThis.m_phthriThis->bJoined)
    {
        eToFill = m_hthrThis.m_phthriThis->eExit;
        return kCIDLib::True;
    }

    TKrnlLinux::TThreadTimer thtWaitFor(c4MilliSeconds);
    tCIDLib::TVoid* pHostExit = 0;

    thtWaitFor.JumpOnSignal(kCIDLib::True);

    if (!::sigsetjmp(thtWaitFor.JumpEnvironment(), 1))
    {
        if (c4MilliSeconds != kCIDLib::c4MaxWait)
        {
            if (!thtWaitFor.bBegin())
                return kCIDLib::False;
        }

        tCIDLib::TOSErrCode HostErr = ::pthread_join(m_hthrThis.m_phthriThis->tidThis
                                                     , &pHostExit);

        thtWaitFor.Cancel();

        if (HostErr)
        {
            TKrnlError::SetLastHostError(HostErr);
            return kCIDLib::False;
        }
    }
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
        return kCIDLib::False;
    }

    m_hthrThis.m_phthriThis->eExit = tCIDLib::EExitCodes(tCIDLib::TSInt(pHostExit));
    m_hthrThis.m_phthriThis->bJoined = kCIDLib::True;
    m_hthrThis.m_phthriThis->tidThis = kCIDLib::tidInvalid;

    eToFill = m_hthrThis.m_phthriThis->eExit;

    return kCIDLib::True;
}


//
//  Sets the GUI thread marker flag. This is set in a per thread memory location so that
//  we can get back to it without the outside world having to pass it in to all of the
//  handle waiting methods.
//
tCIDLib::TVoid TKrnlThread::MarkAsGUIThread()
{
    CIDKernel_Thread_Linux::ptiThread.bIsGUIThread = kCIDLib::True;
}


tCIDLib::TVoid TKrnlThread::Orphan()
{
    if (m_hthrThis.bIsValid()
    && !::pthread_kill(m_hthrThis.m_phthriThis->tidThis, 0))
        ::pthread_detach(m_hthrThis.m_phthriThis->tidThis);
}


tCIDLib::TVoid TKrnlThread::SetNotRunning()
{
}

tCIDLib::TThreadId TKrnlThread::tidThis() const
{
    return m_hthrThis.m_phthriThis->tidThis;
}
