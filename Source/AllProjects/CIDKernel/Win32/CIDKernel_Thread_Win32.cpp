//
// FILE NAME: CIDKernel_Thread_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation of the TKrnlThread
//  classes.
//
// CAVEATS/GOTCHAS:
//
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
#include    <process.h>


// ---------------------------------------------------------------------------
//  Local types
//
//  TStartUpData
//      This is used locally to pass the startup data from the bBeginThread
//      method to the internal thread start method. It is used to pass in
//      the actual CIDLib level method that will be called, and the user data
//      to be passed along.
// ---------------------------------------------------------------------------
struct TStartUpData
{
    TKrnlThread::TCallBack  pfnCallBack;
    tCIDLib::TVoid*         pUserData;
    TThread*                pthrStarting;
    TKrnlThread*            pkthrStarting;
    tCIDLib::TCh*           pszName;
};


namespace CIDKernel_Thread_Win32
{
    // -----------------------------------------------------------------------
    //  Local data
    //
    //  tidPrimary
    //      Used to save the primary thread id. This is the thread that our
    //      initialization occurs on, so we just store away the calling thread
    //      id during it.
    // -----------------------------------------------------------------------
    tCIDLib::TThreadId  tidPrimary;


    // -----------------------------------------------------------------------
    //  A per thread slot id for our own internal use
    // -----------------------------------------------------------------------
    tCIDLib::TCard4     c4SlotId;


    // -----------------------------------------------------------------------
    //  Our per-thread info structure
    // -----------------------------------------------------------------------
    struct  TPerThreadInfo
    {
        tCIDLib::TBoolean   bIsGUIThread;
    };
}



// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------

//
//  This method is the exception filter for the OS level exception handler placed on
//  every new thread. We used to implement some stack dump stuff, really complicated
//  and heavy, but that's been deprecated. But we keep this in case we want to use
//  it in the future, and of course breakpoints can be placed here to handle really low
//  level problems that might occur.
//
static tCIDLib::TInt4
i4ExceptFilter(         EXCEPTION_POINTERS* const   pExceptInfo
                , const tCIDLib::TCh* const         pszThreadName)
{

    return EXCEPTION_CONTINUE_SEARCH;
}


//
//  This is the function that is really started up when a new thread is started. It sets
//  up a low level exception handler then it calls the real CIDLib level thread function,
//  passing along the information it needs.
//
tCIDLib::TUInt __stdcall uThreadStart(tCIDLib::TVoid* pStartUp)
{
    // Cast it back to the startup data structure
    TStartUpData* pData = static_cast<TStartUpData*>(pStartUp);

    // Do some standard init on this thread
    TKrnlThread::KrnlThreadInit();

    //
    //  Install a try block and call the callback, passing back the thread
    //  start data we were given.
    //
    tCIDLib::EExitCodes eRet = tCIDLib::EExitCodes::Normal;
    __try
    {
        eRet = pData->pfnCallBack(pData->pthrStarting, pData->pUserData);
    }

    __except(i4ExceptFilter(GetExceptionInformation(), pData->pszName))
    {
        // Just fall through
    }

    // Clean up COM on this thread
    ::OleUninitialize();

    // Clean up the passed user data
    delete [] pData->pszName;
    delete pData;

    // Clean up the per-thread data structure slot
    delete ::TlsGetValue(CIDKernel_Thread_Win32::c4SlotId);

    return tCIDLib::TUInt(eRet);
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
        // Store away the calling thread, which is the primary thread id
        CIDKernel_Thread_Win32::tidPrimary = ::GetCurrentThreadId();

        //
        //  Allocate a per-thread slot that we can use to store some info
        //  that is of use to us. For every thread that is created it will
        //  allocate a structure and set it on this slot.
        //
        CIDKernel_Thread_Win32::c4SlotId = ::TlsAlloc();
    }
    return kCIDLib::True;
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
    m_phthriThis->hThread = 0;
    m_phthriThis->tidThis = kCIDLib::tidInvalid;
}

TThreadHandle::TThreadHandle(const TThreadHandle& hthrToCopy) :

    m_phthriThis(nullptr)
{
    m_phthriThis = new TThreadHandleImpl;
    m_phthriThis->hThread = hthrToCopy.m_phthriThis->hThread;
    m_phthriThis->tidThis = hthrToCopy.m_phthriThis->tidThis;
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

    m_phthriThis->hThread = hthrToAssign.m_phthriThis->hThread;
    m_phthriThis->tidThis = hthrToAssign.m_phthriThis->tidThis;

    return *this;
}


tCIDLib::TBoolean
TThreadHandle::operator==(const TThreadHandle& hthrToCompare) const
{
    return ((m_phthriThis->hThread == hthrToCompare.m_phthriThis->hThread)
    &&      (m_phthriThis->tidThis == hthrToCompare.m_phthriThis->tidThis));
}

tCIDLib::TBoolean
TThreadHandle::operator!=(const TThreadHandle& hthrToCompare) const
{
    return !operator==(hthrToCompare);
}


// -------------------------------------------------------------------
//  TThreadHandle: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TThreadHandle::bIsValid() const
{
    return (m_phthriThis->hThread != 0);
}

tCIDLib::TVoid TThreadHandle::Clear()
{
    m_phthriThis->hThread = 0;
    m_phthriThis->tidThis = kCIDLib::tidInvalid;
}

tCIDLib::TVoid
TThreadHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    tCIDLib::TZStr64    szTmp1;
    tCIDLib::TZStr64    szTmp2;

    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phthriThis->hThread), szTmp1, 64, tCIDLib::ERadices::Hex
    );
    TRawStr::bFormatVal(tCIDLib::TCard4(m_phthriThis->tidThis), szTmp2, 64);
    TRawStr::CopyCatStr(pszToFill, c4MaxChars, szTmp1, L"\\", szTmp2);
}


const TThreadHandleImpl& TThreadHandle::hthriThis() const
{
    return *m_phthriThis;
}

TThreadHandleImpl& TThreadHandle::hthriThis()
{
    return *m_phthriThis;
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

// Return true if the calling thread is the passed thread
tCIDLib::TBoolean TKrnlThread::bIsCaller(const TKrnlThread& kthrToTest)
{
    return
    (
        kthrToTest.m_hthrThis.m_phthriThis->tidThis == ::GetCurrentThreadId()
    );
}


// Return whether the callng thread is a GUI marked thread
tCIDLib::TBoolean TKrnlThread::bIsCallerGUIThread()
{
    CIDKernel_Thread_Win32::TPerThreadInfo* pInfo =
    (
        (CIDKernel_Thread_Win32::TPerThreadInfo*)::TlsGetValue(CIDKernel_Thread_Win32::c4SlotId)
    );
    if (pInfo)
        return pInfo->bIsGUIThread;
    return kCIDLib::False;
}


// Return teh priority of the passed thread
tCIDLib::TBoolean
TKrnlThread::bPriorityOf(const  TKrnlThread&            kthrToQuery
                        ,       tCIDLib::EPrioLevels&   eToFill)
{
    // If this thread is not running, then this is not legal
    if (kthrToQuery.m_hthrThis.m_phthriThis->tidThis == kCIDLib::tidInvalid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }

    // Get the current host priority level for the thread
    const tCIDLib::TInt4 i4Level = ::GetThreadPriority
    (
        kthrToQuery.m_hthrThis.m_phthriThis->hThread
    );

    if (i4Level == THREAD_PRIORITY_ERROR_RETURN)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert the system value to our enum type
    if (i4Level == THREAD_PRIORITY_LOWEST)
        eToFill = tCIDLib::EPrioLevels::Lowest;
    else if (i4Level == THREAD_PRIORITY_BELOW_NORMAL)
        eToFill = tCIDLib::EPrioLevels::BelowNormal;
    else if (i4Level == THREAD_PRIORITY_NORMAL)
        eToFill = tCIDLib::EPrioLevels::Normal;
    else if (i4Level == THREAD_PRIORITY_ABOVE_NORMAL)
        eToFill = tCIDLib::EPrioLevels::AboveNormal;
    else if (i4Level == THREAD_PRIORITY_HIGHEST)
        eToFill = tCIDLib::EPrioLevels::Highest;
    else
    {
        //
        //  Someone has changed it outside of our control so its not one
        //  of our standard values. So lets find out what its closest to
        //  and return that.
        //
        if (i4Level < THREAD_PRIORITY_BELOW_NORMAL)
            eToFill = tCIDLib::EPrioLevels::Lowest;
        else if (i4Level < THREAD_PRIORITY_NORMAL)
            eToFill = tCIDLib::EPrioLevels::BelowNormal;
        else if (i4Level < THREAD_PRIORITY_ABOVE_NORMAL)
            eToFill = tCIDLib::EPrioLevels::Normal;
        else
            eToFill = tCIDLib::EPrioLevels::Highest;
    }
    return kCIDLib::True;
}


// Almost never used but available to force down a thread
tCIDLib::TVoid TKrnlThread::Exit(const tCIDLib::EExitCodes eExitCode)
{
    _endthreadex(tCIDLib::TUInt(eExitCode));
}


//
//  When the caller doesn't have the actual thread object, it can call this to generically
//  sleep for a given time.
//
//  Note the special case for GUI threads. Strangely, we just actually block. If we don't
//  then things can happen while we sleep and that can cause all kinds of wierdiness.
//  Of course in actual fact this should almost never be called from a GUI thread, but if
//  so we need to avoid calamity.
//
//  Otherwise, again strangely due to how windows works, we have to do a loop where we
//  we pump messages, because if we don't some COM stuff won't work right since it requires
//  msg pumping.
//
tCIDLib::TVoid TKrnlThread::Sleep(const tCIDLib::TCard4 c4WaitMillis)
{
    tCIDLib::TBoolean bIsGUIThread = kCIDLib::False;
    CIDKernel_Thread_Win32::TPerThreadInfo* pInfo =
    (
        (CIDKernel_Thread_Win32::TPerThreadInfo*)::TlsGetValue(CIDKernel_Thread_Win32::c4SlotId)
    );
    if (pInfo)
        bIsGUIThread = pInfo->bIsGUIThread;

    // If this is the GUI thread, just do a sleep and return. Else the other thing
    if (bIsGUIThread)
    {
        ::Sleep(c4WaitMillis);
        return;
    }


    tCIDLib::TEncodedTime enctCur = TKrnlTimeStamp::enctNow();
    const tCIDLib::TEncodedTime enctEnd
    (
        (c4WaitMillis == INFINITE) ? kCIDLib::c8MaxCard
                                   : enctCur + (kCIDLib::enctOneMilliSec * c4WaitMillis)
    );

    while (enctCur < enctEnd)
    {
        const tCIDLib::TCard4 c4CurMillis = tCIDLib::TCard4
        (
            (enctEnd - enctCur) / kCIDLib::enctOneMilliSec
        );

        DWORD Res = ::MsgWaitForMultipleObjectsEx
        (
            0
            , 0
            , c4CurMillis
            , QS_SENDMESSAGE | QS_POSTMESSAGE
            , MWMO_INPUTAVAILABLE
        );

        if (Res == WAIT_OBJECT_0)
        {
            // The thread has died, so exit
            break;
        }

        if (Res == WAIT_OBJECT_0 + 1)
        {
            // Pump messages, then keep going.
            MSG Msg;
            if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
            {
                if (Msg.message == WM_QUIT)
                {
                    PostQuitMessage((int)Msg.wParam);
                    return;
                }
                 else
                {
                    TranslateMessage(&Msg);
                    DispatchMessage(&Msg);
                }
            }
        }
         else if (Res == WAIT_TIMEOUT)
        {
            // We used up all remaining time, so we timed out
            break;
        }
         else
        {
            // It was a failure of some sort
            break;
        }
        enctCur = TKrnlTimeStamp::enctNow();
    }
}



// Return the thread if of the primary thread (the first one started)
tCIDLib::TThreadId TKrnlThread::tidPrimary()
{
    return CIDKernel_Thread_Win32::tidPrimary;
}


//
//  Return the thread id of the calling thread, which is useful if the caller doesn't
//  have or need to pay the cost of getting the thread object for the calling thread.
//
tCIDLib::TThreadId TKrnlThread::tidCaller()
{
    return ::GetCurrentThreadId();
}


//
//  Handle per-thread initializiation. This is broken out since, in some special cases
//  we might need to call it other than in the new thread callback above. It does COM
//  init, and creates per-thread data storage for this thread, initially setting that
//  to indicate a non-GUI thread (which may be updated later.)
//
tCIDLib::TVoid TKrnlThread::KrnlThreadInit()
{
    ::OleInitialize(0);

    CIDKernel_Thread_Win32::TPerThreadInfo* pInfo
    (
        new CIDKernel_Thread_Win32::TPerThreadInfo
    );
    pInfo->bIsGUIThread = kCIDLib::False;
    ::TlsSetValue(CIDKernel_Thread_Win32::c4SlotId, pInfo);
}


// ---------------------------------------------------------------------------
//  TKrnlThread: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlThread::TKrnlThread()
{
}

TKrnlThread::~TKrnlThread()
{
    // Close the handle if its open
    if (m_hthrThis.m_phthriThis->hThread)
    {
        //
        //  If it fails and we are debugging, then do a popup to warn the
        //  developer. Otherwise, there is not much we can do at this fundamental
        //  level.
        //
        if (!::CloseHandle(m_hthrThis.m_phthriThis->hThread))
        {
            #if CID_DEBUG_ON
            kmodCIDKernel.ErrorPopUp
            (
                kKrnlErrs::errcGen_CloseHandle
                , CID_FILE
                , CID_LINE
                , L"~TKrnlThread"
            );
            #endif
        }
    }
}


// ---------------------------------------------------------------------------
//  TKrnlThread: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  In some rare cases we need to wrap a thread we obtained from somewhere else. This
//  will wrap the calling thread and we adopt it.
//
tCIDLib::TBoolean TKrnlThread::bAdoptCaller()
{
    if (m_hthrThis.m_phthriThis->hThread)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadySet);
        return kCIDLib::False;
    }

    // Query the required handle info about the calling thread
    m_hthrThis.m_phthriThis->hThread = ::GetCurrentThread();
    m_hthrThis.m_phthriThis->tidThis = ::GetCurrentThreadId();

    return kCIDLib::True;
}


// Return whether we are marked as the GUI thread, which is in the per-thread data
tCIDLib::TBoolean TKrnlThread::bIsGUIThread() const
{
    CIDKernel_Thread_Win32::TPerThreadInfo* pInfo =
    (
        (CIDKernel_Thread_Win32::TPerThreadInfo*)::TlsGetValue(CIDKernel_Thread_Win32::c4SlotId)
    );
    if (pInfo)
        return pInfo->bIsGUIThread;
    return kCIDLib::False;
}


// Return whether we are running or not
tCIDLib::TBoolean TKrnlThread::bIsRunning(tCIDLib::TBoolean& bToFill) const
{
    // If the thread id has been set to invalid, then not running
    if (m_hthrThis.m_phthriThis->tidThis == kCIDLib::tidInvalid)
    {
        bToFill = kCIDLib::False;
        return kCIDLib::True;
    }

    //
    //  Seems to be running, but check it by waiting on the handle. We don't
    //  need to bother with message pumping here since we do a zero time
    //  wait.
    //
    const tCIDLib::TCard4 c4Res = ::WaitForSingleObject
    (
        m_hthrThis.m_phthriThis->hThread, 0
    );

    if (c4Res == WAIT_OBJECT_0)
    {
        // The thread has ended normally
        bToFill = kCIDLib::False;
        TRawMem::c4Exchange(m_hthrThis.m_phthriThis->tidThis, kCIDLib::tidInvalid);
        return kCIDLib::True;
    }

    // If it timed out, the thread is still running
    if (c4Res == WAIT_TIMEOUT)
    {
        bToFill = kCIDLib::True;
        return kCIDLib::True;
    }

    // It's an error
    TKrnlError::SetLastHostError(::GetLastError());
    bToFill = kCIDLib::False;
    return kCIDLib::False;
}


//
//  The CIDLib level thread stuff calls this to start a new thread. We get the callback
//  to call, and some startup info.
//
tCIDLib::TBoolean
TKrnlThread::bBeginThread(  const   TKrnlThread::TCallBack  pfnFunc
                            , const tCIDLib::TCard4         c4StackSize
                            ,       tCIDLib::TVoid* const   pData
                            , const tCIDLib::EThreadFlags
                            ,       TThread* const          pthrStart
                            , const tCIDLib::TCh* const     pszThreadName)
{
    // Make sure this isn't a double start
    if (m_hthrThis.m_phthriThis->tidThis != kCIDLib::tidInvalid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    HANDLE          hTmp = 0;
    tCIDLib::TUInt  tidTmp = 0;

    //
    //  Allocate a new thread startup data structure and fill it in with
    //  the info we want to pass to the internal thread startup.
    //
    TStartUpData* pStartUp = new TStartUpData;
    pStartUp->pfnCallBack = pfnFunc;
    pStartUp->pUserData = pData;
    pStartUp->pthrStarting = pthrStart;
    pStartUp->pkthrStarting = this;
    pStartUp->pszName = TRawStr::pszReplicate(pszThreadName);

    //
    //  Start the thread. Note that we start it blocked so that we have time to store
    //  away its info before it could ask for it. We start a local function first,
    //  which then calls the CIDLib level callback.
    //
    hTmp = (tCIDLib::TVoid*)_beginthreadex
    (
        0
        , c4StackSize
        , uThreadStart
        , pStartUp
        , CREATE_SUSPENDED
        , (tCIDLib::TUInt*)&tidTmp
    );

    if (!hTmp)
    {
        // We need to clean up the startup data ourself in this case
        delete [] pStartUp->pszName;
        delete pStartUp;

        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Store away the thread info
    TRawMem::c4Exchange(m_hthrThis.m_phthriThis->tidThis, tidTmp);
    TKrnlWin32::AtomicHandleSet(m_hthrThis.m_phthriThis->hThread, hTmp);

    // Now we can let the thread go
    if (::ResumeThread(hTmp) == -1)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  In some cases a thread needs to block himself, to later be awakened by another thread.
//  So typically he would put himself on some list and then block himself.
//
tCIDLib::TBoolean TKrnlThread::bBlockMyself()
{
    // Make sure its this thread calling
    if (m_hthrThis.m_phthriThis->tidThis != ::GetCurrentThreadId())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_NotThisThread);
        return kCIDLib::False;
    }

    if (::SuspendThread(m_hthrThis.m_phthriThis->hThread) == 0xFFFFFFFF)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Not generally used but needed. We only support a fairly limited set of priorities,
//  which may still be too many for some platforms.
//
tCIDLib::TBoolean TKrnlThread::bPriority(tCIDLib::EPrioLevels& eToFill) const
{
    // Just call the static one and pass ourself as the thread
    return TKrnlThread::bPriorityOf(*this, eToFill);
}

tCIDLib::TBoolean TKrnlThread::bSetPriority(const tCIDLib::EPrioLevels eLevel)
{
    // Convert the enum into the host priority class
    tCIDLib::TInt4 i4Prio;
    if (eLevel == tCIDLib::EPrioLevels::Lowest)
        i4Prio = THREAD_PRIORITY_LOWEST;
    else if (eLevel == tCIDLib::EPrioLevels::BelowNormal)
        i4Prio = THREAD_PRIORITY_BELOW_NORMAL;
    else if (eLevel == tCIDLib::EPrioLevels::Normal)
        i4Prio = THREAD_PRIORITY_NORMAL;
    else if (eLevel == tCIDLib::EPrioLevels::AboveNormal)
        i4Prio = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (eLevel == tCIDLib::EPrioLevels::Highest)
        i4Prio = THREAD_PRIORITY_HIGHEST;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_InvalidPriority);
        return kCIDLib::False;
    }

    if (!::SetThreadPriority(m_hthrThis.m_phthriThis->hThread, i4Prio))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Once a thread has exited, the exit code can be queried. We return it as our exit
//  code enum, though it may not actually be one of the defined values! If the thread
//  isn't done yet, the return is false.
//
tCIDLib::TBoolean
TKrnlThread::bQueryExitCode(tCIDLib::EExitCodes& eToFill) const
{
    tCIDLib::TCard4 c4Code;
    if (!::GetExitCodeThread(m_hthrThis.m_phthriThis->hThread, &c4Code))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (c4Code == STILL_ACTIVE)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_StillRunning);
        return kCIDLib::False;
    }

    eToFill = tCIDLib::EExitCodes(c4Code);
    return kCIDLib::True;
}


//
//  Unblock a thread that called bBlockMyself() above. Obviously only anothre thread
//  could correctly call this since the thread to be unblocked couldn't do it.
//
tCIDLib::TBoolean TKrnlThread::bUnblock()
{
    if (::ResumeThread(m_hthrThis.m_phthriThis->hThread) == 0xFFFFFFFF)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Wait for up to a given time for this thread to die. If the return is true, then
//  the bDied parameter is updated with the died or not state.
//
//  This one has to do a two-object wait, for either this thread to die, or for this
//  thread to post the passed event, which is a very useful thing to b eable to do.
//
tCIDLib::TBoolean
TKrnlThread::bWaitEvOrDeath(        TKrnlEvent&         kevWait
                            ,       tCIDLib::TBoolean&  bDied
                            , const tCIDLib::TCard4     c4MilliSecs)
{
    HANDLE ahWait[2];
    ahWait[0] = m_hthrThis.m_phthriThis->hThread;
    ahWait[1] = kevWait.hevThis().heviThis().hEvent;

    tCIDLib::TCard4 c4Which;
    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandles
    (
        ahWait, 2, c4Which, c4MilliSecs
    );

    // If we timed out, say it didn't die and return timeout error
    if (eRes == tCIDLib::EWaitRes::Timeout)
    {
        bDied = kCIDLib::False;
        return kCIDLib::False;
    }

    // If it was the thread, then set the died flag and return success
    if ((eRes == tCIDLib::EWaitRes::Success) && (c4Which == 0))
    {
        bDied = kCIDLib::True;
        return kCIDLib::True;
    }

    // If the event, then it didn't die, so return success but false
    if ((eRes == tCIDLib::EWaitRes::Success) && (c4Which == 1))
    {
        bDied = kCIDLib::False;
        return kCIDLib::True;
    }

    // Else it was some other system error, so not sure if it did and error state
    bDied = kCIDLib::False;
    return kCIDLib::False;
}


//
//  A simpler version of the above, which only waits for this thread to terminate. If
//  we timeout on our handle wait, then it didn't die. If the return is true, then
//  bState holds the died state. If that is true, then eToFill holds the exit code.
//
tCIDLib::TBoolean
TKrnlThread::bWaitForDeath(         tCIDLib::TBoolean&      bState
                            ,       tCIDLib::EExitCodes&    eToFill
                            , const tCIDLib::TCard4         c4MilliSecs) const
{
    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle
    (
         m_hthrThis.m_phthriThis->hThread, c4MilliSecs
    );

    //
    //  It'd died appropriately, so we can return the exit code, and also clear our
    //  thread id right now. This thread hasn't full died yet, since we need to get
    //  back to the original callback and all that. But we want it to seem dead as
    //  of return from this call.
    //
    if (eRes == tCIDLib::EWaitRes::Success)
    {
        bState = kCIDLib::True;
        TRawMem::c4Exchange(m_hthrThis.m_phthriThis->tidThis, kCIDLib::tidInvalid);
        return bQueryExitCode(eToFill);
    }

    //
    //  If we timed out, then its still running after our wait period. Don't
    //  return false, but set the state flag to false.
    //
    if (eRes == tCIDLib::EWaitRes::Timeout)
    {
        bState = kCIDLib::False;
        return kCIDLib::True;
    }

    // Else it was some other system error
    bState = kCIDLib::False;
    return kCIDLib::False;
}


//
//  Sets the GUI thread marker flag. This is set in a per thread memory location so that
//  we can get back to it without the outside world having to pass it in to all of the
//  handle waiting methods.
//
tCIDLib::TVoid TKrnlThread::MarkAsGUIThread()
{
    CIDKernel_Thread_Win32::TPerThreadInfo* pInfo =
    (
        (CIDKernel_Thread_Win32::TPerThreadInfo*)::TlsGetValue(CIDKernel_Thread_Win32::c4SlotId)
    );
    if (pInfo)
        pInfo->bIsGUIThread = kCIDLib::True;
}


//
//  Orphans the thread associated with this thread object so that this object
//  can go away without the thread having to end. Generally should not be
//  used. It's mostly for internal purposes if we have to attach to an
//  existing thread for some reason, then let it go again.
//
tCIDLib::TVoid TKrnlThread::Orphan()
{
    TRawMem::c4Exchange(m_hthrThis.m_phthriThis->tidThis, kCIDLib::tidInvalid);
    TKrnlWin32::AtomicHandleSet(m_hthrThis.m_phthriThis->hThread, 0);
}


//
//  Marks this thread as no longer running, in our case by storing an invalid
//  thread id.
//
tCIDLib::TVoid TKrnlThread::SetNotRunning()
{
    //
    //  Note that we DO NOT clear the handle, since its required to get the
    //  exit code.
    //
    TRawMem::c4Exchange(m_hthrThis.m_phthriThis->tidThis, kCIDLib::tidInvalid);
}


tCIDLib::TThreadId TKrnlThread::tidThis() const
{
    return m_hthrThis.m_phthriThis->tidThis;
}

