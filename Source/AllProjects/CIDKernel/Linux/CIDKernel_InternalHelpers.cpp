//
// FILE NAME: CIDKernel_InternalHelpers.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/29/1998
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
//  This file implements the TKrnlLinux namespace. This guy holds some
//  stuff that is for use totally within the Linux kernel driver.
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

#include    <sys/types.h>
#include    <sys/stat.h>
#include    <unistd.h>
#include    <signal.h>


// ---------------------------------------------------------------------------
//  Local variables
// ---------------------------------------------------------------------------
namespace
{
    // Magic number
    //const tCIDLib::TEncodedTime enctTimeAdjustment = 116444736000000000;
    const tCIDLib::TEncodedTime enctIntervalsPerSec = 1000000000 / 100;
    const tCIDLib::TSInt        iThreadTimerSignal = SIGSYS;

    struct TThreadStartupInfo
    {
        tCIDLib::TThreadId        tidToKill;
        TKrnlLinux::TThreadTimer* thtThis;
    };

    struct TTimerInfo
    {
        sigjmp_buf          SigJmpBuf;
        tCIDLib::TBoolean   bJumpOnSignal;
        tCIDLib::TBoolean   bTriggered;
    };

    pthread_key_t __keyTimerInfo;

    tCIDLib::TVoid __ThreadInfoDestructor(tCIDLib::TVoid* pData)
    {
        TKrnlLinux::TKrnlThreadInfo* pThreadInfo =
            static_cast<TKrnlLinux::TKrnlThreadInfo*>(pData);
        delete pThreadInfo;
    }
}

//
// Converts a time_t value to the kind used by CIDLib. CID file
// time values are the number of 100-nanosecond intervals elapsed
// since January 1, 1970, UTC.
//
tCIDLib::TVoid
TKrnlLinux::LinuxFileTimeToCIDFileTime(const time_t LinuxTime
                                       , tCIDLib::TEncodedTime& enctTimeToFill)
{
    enctTimeToFill = tCIDLib::TEncodedTime(LinuxTime) * enctIntervalsPerSec;
}

//
// Converts a CID time value to a Linux time_t.
//
tCIDLib::TVoid
TKrnlLinux::CIDFileTimeToLinuxFileTime(const tCIDLib::TEncodedTime enctCIDTime
                                        , time_t& LinuxTime)
{
    LinuxTime = time_t(enctCIDTime / enctIntervalsPerSec);
}

tCIDLib::TVoid TKrnlLinux::StatBufToInfoFlags(const struct stat& StatBuf
                                              , tCIDLib::EFileInfoFlags& eFlags)
{
    tCIDLib::TCard4 c4Tmp;
    if (S_ISDIR(StatBuf.st_mode))
        c4Tmp = tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::IsDirectory);
    else
        c4Tmp = tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::None);

    if (StatBuf.st_uid == ::getuid())
    {
        if (StatBuf.st_mode & S_IRUSR)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanRead);
        if (StatBuf.st_mode & S_IWUSR)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanWrite);
        if (StatBuf.st_mode & S_IXUSR)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanExecute);
    }
    else if (StatBuf.st_gid == ::getgid())
    {
        if (StatBuf.st_mode & S_IRGRP)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanRead);
        if (StatBuf.st_mode & S_IWGRP)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanWrite);
        if (StatBuf.st_mode & S_IXGRP)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanExecute);
    }
    else
    {
        if (StatBuf.st_mode & S_IROTH)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanRead);
        if (StatBuf.st_mode & S_IWOTH)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanWrite);
        if (StatBuf.st_mode & S_IXOTH)
            c4Tmp |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanExecute);
    }

    eFlags = tCIDLib::EFileInfoFlags(c4Tmp);
}

tCIDLib::TSCh*
TKrnlLinux::pszFindInPath(const tCIDLib::TCh* const pszToFind)
{
    tCIDLib::TSCh* pszCopy = TRawStr::pszConvert(pszToFind);

    if (!TRawStr::pszFindChar(pszToFind, kCIDLib::chPathSep))
    {
        tCIDLib::TSCh* pszPath = ::getenv("PATH");
        if (!pszPath)
            return pszCopy;
        tCIDLib::TSCh* pszPathCopy = TRawStr::pszReplicate(pszPath);
        TArrayJanitor<tCIDLib::TSCh> janPathCopy(pszPathCopy);

        tCIDLib::TSCh szFullPath[kCIDLib::c4MaxPathLen + 1];
        tCIDLib::TSCh* pszSave;
        tCIDLib::TSCh* pszCurPath = ::strtok_r(pszPathCopy, ":", &pszSave);
        while (pszCurPath)
        {
            ::strcpy(szFullPath, pszCurPath);
            if (szFullPath[::strlen(szFullPath) - 1] != '\\')
                ::strcat(szFullPath, "\\");
            ::strcat(szFullPath, pszCopy);
            if (!::access(szFullPath, F_OK))
            {
                delete [] pszCopy;
                return TRawStr::pszReplicate(szFullPath);
            }
            pszCurPath = ::strtok_r(0, ":", &pszSave);
        }
    }

    return pszCopy;
}

TKrnlLinux::TThreadTimer::TThreadTimer(tCIDLib::TCard4 c4MilliSecs) :
    __tidThis(kCIDLib::tidInvalid)
    , __c4MilliSecs(c4MilliSecs)
{
    TTimerInfo* pTimerInfo =
        static_cast<TTimerInfo*>(::pthread_getspecific(__keyTimerInfo));

    if (!pTimerInfo)
    {
        pTimerInfo = new TTimerInfo;
        pTimerInfo->bJumpOnSignal = kCIDLib::False;
        ::pthread_setspecific(__keyTimerInfo, pTimerInfo);
    }
    else
    {
        TKrnlError::ThrowKrnlError(kKrnlErrs::errcGen_AlreadyExists);
    }
}

TKrnlLinux::TThreadTimer::~TThreadTimer()
{
    Cancel();
    Reset();

    TTimerInfo* pTimerInfo =
        static_cast<TTimerInfo*>(::pthread_getspecific(__keyTimerInfo));

    delete pTimerInfo;
    ::pthread_setspecific(__keyTimerInfo, 0);
}

tCIDLib::TBoolean TKrnlLinux::TThreadTimer::bBegin()
{
    sigset_t NewSigSet;

    ::sigemptyset(&NewSigSet);
    ::sigaddset(&NewSigSet, iThreadTimerSignal);
    ::pthread_sigmask(SIG_UNBLOCK, &NewSigSet, &__SavedSigSet);

    struct sigaction NewSigAction;
    ::memset(&NewSigAction, 0, sizeof(NewSigAction));
    NewSigAction.sa_handler = TKrnlLinux::TThreadTimer::__HandleSignal;
    ::sigemptyset(&NewSigAction.sa_mask);
    if (::sigaction(iThreadTimerSignal, &NewSigAction, &__SavedSigAction))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    TThreadStartupInfo* pStartInfo = new TThreadStartupInfo;
    pStartInfo->tidToKill = ::pthread_self();
    pStartInfo->thtThis = this;

    pthread_attr_t ThreadAttr;
    ::pthread_attr_init(&ThreadAttr);
    ::pthread_attr_setdetachstate(&ThreadAttr, 1);

    tCIDLib::TOSErrCode HostErr;
    HostErr = ::pthread_create(&__tidThis
                               , &ThreadAttr
                               , TKrnlLinux::TThreadTimer::__TimerFunc
                               , pStartInfo);
    ::pthread_attr_destroy(&ThreadAttr);
    if (HostErr)
    {
        TKrnlError::SetLastHostError(HostErr);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlLinux::TThreadTimer::bTriggered() const
{
    TTimerInfo* pTimerInfo =
        static_cast<TTimerInfo*>(::pthread_getspecific(__keyTimerInfo));

    return pTimerInfo->bTriggered;
}

tCIDLib::TVoid TKrnlLinux::TThreadTimer::Cancel()
{
    ::pthread_cancel(__tidThis);
}

sigjmp_buf& TKrnlLinux::TThreadTimer::JumpEnvironment() const
{
    TTimerInfo* pTimerInfo =
        static_cast<TTimerInfo*>(::pthread_getspecific(__keyTimerInfo));

    return pTimerInfo->SigJmpBuf;
}

tCIDLib::TVoid  TKrnlLinux::TThreadTimer::JumpOnSignal(tCIDLib::TBoolean bAction)
{
    TTimerInfo* pTimerInfo =
        static_cast<TTimerInfo*>(::pthread_getspecific(__keyTimerInfo));

    pTimerInfo->bJumpOnSignal = bAction;
}

tCIDLib::TVoid TKrnlLinux::TThreadTimer::Reset()
{
    ::sigaction(SIGALRM, &__SavedSigAction, 0);
    ::pthread_sigmask(SIG_SETMASK, &__SavedSigSet, 0);
}

tCIDLib::TVoid* TKrnlLinux::TThreadTimer::__TimerFunc(tCIDLib::TVoid* pParam)
{
    TThreadStartupInfo StartInfo = *static_cast<TThreadStartupInfo*>(pParam);

    delete static_cast<TThreadStartupInfo*>(pParam);

    struct timespec TimeSpec;
    TimeSpec.tv_sec = StartInfo.thtThis->__c4MilliSecs / 1000;
    TimeSpec.tv_nsec = (StartInfo.thtThis->__c4MilliSecs % 1000) * 1000000;

    ::nanosleep(&TimeSpec, 0);
    ::pthread_kill(StartInfo.tidToKill, iThreadTimerSignal);

    return 0;
}

tCIDLib::TVoid TKrnlLinux::TThreadTimer::__HandleSignal(tCIDLib::TSInt)
{
    TTimerInfo* pTimerInfo =
        static_cast<TTimerInfo*>(::pthread_getspecific(__keyTimerInfo));

    pTimerInfo->bTriggered = kCIDLib::True;

    if (pTimerInfo->bJumpOnSignal)
        ::siglongjmp(pTimerInfo->SigJmpBuf, 1);
}

tCIDLib::TBoolean TKrnlLinux::TThreadTimer::bInitTerm(tCIDLib::EInitTerm eInitTerm)
{
    tCIDLib::TOSErrCode HostErr;
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        HostErr = ::pthread_key_create(&__keyTimerInfo, 0);
        if (HostErr)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_TooMany);
            return kCIDLib::False;
        }
    }
    else
    {
        HostErr = ::pthread_key_delete(__keyTimerInfo);
        if (HostErr)
        {
            TKrnlError::SetLastHostError(HostErr);
            return kCIDLib::False;
        }
    }

    return kCIDLib::True;
}

tCIDLib::TSInt TKrnlLinux::TRecursiveMutex::iInitialize()
{
    ::pthread_mutex_init(&__mtx, 0);
    __c4Count = 0;
    __tidOwner = kCIDLib::tidInvalid;
    return 0;
}

tCIDLib::TSInt TKrnlLinux::TRecursiveMutex::iLock()
{
    tCIDLib::TThreadId tidMyself = ::pthread_self();
    if (::pthread_equal(__tidOwner, tidMyself))
    {
        __c4Count++;
    }
    else
    {
        ::pthread_mutex_lock(&__mtx);
        __tidOwner = tidMyself;
        __c4Count = 1;
    }
    return 0;
}

tCIDLib::TSInt TKrnlLinux::TRecursiveMutex::iUnlock()
{
    if (!::pthread_equal(__tidOwner, ::pthread_self()))
    {
        return EPERM;
    }
    if (--__c4Count == 0)
    {
        __tidOwner = kCIDLib::tidInvalid;
        ::pthread_mutex_unlock(&__mtx);
    }
    return 0;
}

pthread_key_t TKrnlLinux::TKrnlThreadInfo::__keyThreadInfo = 0;

TKrnlLinux::TKrnlThreadInfo::TKrnlThreadInfo(TKrnlThread* pKrnlThread, tCIDLib::TCh* pszName)
{
    __pkthrThis = pKrnlThread;
    __pszName = pszName;
    ::pthread_setspecific(__keyThreadInfo, this);
}

TKrnlLinux::TKrnlThreadInfo::~TKrnlThreadInfo()
{
    delete [] __pszName;
}

TKrnlLinux::TKrnlThreadInfo* TKrnlLinux::TKrnlThreadInfo::pkthriInstance()
{
    return static_cast<TKrnlLinux::TKrnlThreadInfo*>
        (::pthread_getspecific(__keyThreadInfo));
}

tCIDLib::TBoolean TKrnlLinux::TKrnlThreadInfo::bInitTerm(tCIDLib::EInitTerm eInitTerm)
{
    tCIDLib::TOSErrCode HostErr;

    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        HostErr = ::pthread_key_create(&__keyThreadInfo, __ThreadInfoDestructor);
        if (HostErr)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_TooMany);
            return kCIDLib::False;
        }
        new TKrnlThreadInfo(0, TRawStr::pszReplicate(kCIDLib::pszPrimaryThrdName));
    }
    else
    {
        delete TKrnlThreadInfo::pkthriInstance();
        HostErr = ::pthread_key_delete(__keyThreadInfo);
        if (HostErr)
        {
            TKrnlError::SetLastHostError(HostErr);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

// ---------------------------------------------------------------------------
//   CLASS: TTermFifo
//  PREFIX: fifo
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTermFifo: Constructor and Destructor
// ---------------------------------------------------------------------------

TKrnlLinux::TTermFifo::TTermFifo(tCIDLib::TCard4 c4Size) :
    __c4Size(c4Size)
    , __c4Head(kCIDLib::c4MaxCard)
    , __c4Tail(0)
    , __c4Peek(0)
{
    __pchFifo = new tCIDLib::TSCh[__c4Size];
}

TKrnlLinux::TTermFifo::~TTermFifo()
{
    Clear();
    delete [] __pchFifo;
}

// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlLinux::TTermFifo::bPush()
{
    if (__c4Tail == kCIDLib::c4MaxCard)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
        return kCIDLib::False;
    }

    tCIDLib::TSCh chTmp;
    tCIDLib::TSInt iRead = ::read(0, &chTmp, 1);
    if (iRead != 1)
    {
        if (iRead == 0)
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NoData);
        else
            TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    __pchFifo[__c4Tail] = chTmp;

    if (__c4Head == kCIDLib::c4MaxCard)
        __c4Head = __c4Peek = __c4Tail;

    if (++__c4Tail == __c4Size)
        __c4Tail = 0;

    if (__c4Tail == __c4Head)
        __c4Tail = kCIDLib::c4MaxCard;

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlLinux::TTermFifo::bIsEmpty() const
{
    return (__c4Head == kCIDLib::c4MaxCard || __c4Tail == __c4Peek);
}

tCIDLib::TSCh TKrnlLinux::TTermFifo::chPeek()
{
    tCIDLib::TSCh chReturn = __pchFifo[__c4Peek];

    if (++__c4Peek == __c4Size)
        __c4Peek = 0;

    return chReturn;
}

tCIDLib::TSCh TKrnlLinux::TTermFifo::chPull()
{
    tCIDLib::TSCh chReturn = __pchFifo[__c4Head];

    tCIDLib::TBoolean bPeekIsHead = __c4Peek == __c4Head;

    if (++__c4Head == __c4Size)
        __c4Head = 0;

    if (__c4Head == __c4Tail)
    {
        __c4Head = kCIDLib::c4MaxCard;
        __c4Tail = 0;
    }

    if (bPeekIsHead)
        __c4Peek = __c4Head;

    return chReturn;
}

tCIDLib::TVoid TKrnlLinux::TTermFifo::Clear()
{
    ::memset(__pchFifo, 0, __c4Size);
    __c4Head = kCIDLib::c4MaxCard;
    __c4Tail = __c4Peek = 0;
}

tCIDLib::TVoid TKrnlLinux::TTermFifo::ClearPeeked()
{
    if (__c4Peek == __c4Tail)
        Clear();
    else
        __c4Head = __c4Peek;
}

tCIDLib::TVoid TKrnlLinux::TTermFifo::ResetPeek()
{
    __c4Peek = __c4Head;
}



// ---------------------------------------------------------------------------
//   CLASS: TTermValueNode
//  PREFIX: node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTermValueNode: Constructor and Destructor
// ---------------------------------------------------------------------------
TKrnlLinux::TTermValueNode::TTermValueNode() :
    pnodeChild(0)
    , pnodeSibling(0)
    , c2Code(0)
    , chThis(0)
{
}

TKrnlLinux::TTermValueNode::~TTermValueNode()
{
    delete pnodeSibling;
    delete pnodeChild;
}

