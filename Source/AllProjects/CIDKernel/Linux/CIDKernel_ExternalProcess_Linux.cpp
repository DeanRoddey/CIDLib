//
// FILE NAME: CIDKernel_ExternalProcess_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/19/1998
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
//  TKrnlExtProcess.
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
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"
#include    "CIDKernel_PlatformStrOps.hpp"


namespace CIDKernel_ExtProc_Linux
{
    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------

    // This class is used to store a growable array of TProcessHandle objects.
    // The purpose is to be able to look up the process when the parent receives
    // a SIGCHLD, then to store termination information in the handle.
    class TChildProcs
    {
        public:
            TChildProcs();
            ~TChildProcs();

            TProcessHandle** ahprocChildProcs();
            tCIDLib::TVoid AddChildProc(TProcessHandle* phprocChild);
            tCIDLib::TCard4 c4Size();
            tCIDLib::TVoid RemoveChildProc(TProcessHandle* phprocChild);

        private:
            static const tCIDLib::TCard4 c4DefaultChunkSize = 10;

            TProcessHandle** m_aphprocChildProcs;
            tCIDLib::TCard4  m_c4Size;
            TKrnlLinux::TRecursiveMutex m_rmtxLock;
    };

    TChildProcs::TChildProcs()
    {
        m_c4Size = 0;
        m_aphprocChildProcs = nullptr;
        m_rmtxLock.iInitialize();
    }

    TChildProcs::~TChildProcs()
    {
        m_rmtxLock.iLock();
        delete [] m_aphprocChildProcs;
        m_rmtxLock.iUnlock();
        m_rmtxLock.iDestroy();
    }

    tCIDLib::TVoid TChildProcs::AddChildProc(TProcessHandle* phprocChild)
    {
        m_rmtxLock.iLock();
        tCIDLib::TCard4 c4Idx;
        for (c4Idx = 0; c4Idx < m_c4Size; c4Idx++)
        {
            if (m_aphprocChildProcs[c4Idx] == 0)
            {
                m_aphprocChildProcs[c4Idx] = phprocChild;
                m_rmtxLock.iUnlock();
                return;
            }
        }
        tCIDLib::TCard4 c4OldSize = m_c4Size;
        m_c4Size += c4DefaultChunkSize;
        TProcessHandle** aphprocTmp = new TProcessHandle*[m_c4Size];
        for (c4Idx = 0; c4Idx < c4OldSize; c4Idx++)
            aphprocTmp[c4Idx] = m_aphprocChildProcs[c4Idx];
        aphprocTmp[c4OldSize] = phprocChild;
        for (c4Idx = c4OldSize + 1; c4Idx < m_c4Size; c4Idx++)
            aphprocTmp[c4Idx] = nullptr;
        delete [] m_aphprocChildProcs;
        m_aphprocChildProcs = aphprocTmp;
        m_rmtxLock.iUnlock();
    }

    inline TProcessHandle** TChildProcs::ahprocChildProcs()
    {
        return m_aphprocChildProcs;
    }

    inline tCIDLib::TCard4 TChildProcs::c4Size()
    {
        return m_c4Size;
    }

    tCIDLib::TVoid TChildProcs::RemoveChildProc(TProcessHandle* phprocChild)
    {
        m_rmtxLock.iLock();
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < m_c4Size; c4Idx++)
        {
            if (m_aphprocChildProcs[c4Idx] == phprocChild)
            {
                m_aphprocChildProcs[c4Idx] = nullptr;
                m_rmtxLock.iUnlock();
                return;
            }
        }
        m_rmtxLock.iUnlock();
    }

    TChildProcs ChildProcs;



    // This function does the work of collecting termination information about child
    // processes. It processes SIGCHLD signals and stores appropriate information in
    // the process handle.
    //
    tCIDLib::TVoid ChildSignalHandler(tCIDLib::TSInt iSignal)
    {
        // The time gathered here will be used as the end time of the child process,
        // since the kernel does not track that information.
        struct timeval TimeEntered;
        ::gettimeofday(&TimeEntered, 0);

        // Now perform the wait. We use the WNOHANG flag because we know that there's
        // a process terminating or we wouldn't be in this function in the first place.
        tCIDLib::TSInt iStatus;
        struct rusage ResourceUsage;
        pid_t pid = ::wait4(WAIT_ANY, &iStatus, WNOHANG, &ResourceUsage);
        if (pid == -1)
            return;

        // Now look up the child process
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < CIDKernel_ExtProc_Linux::ChildProcs.c4Size(); c4Idx++)
        {
            TProcessHandle* phprocCur = CIDKernel_ExtProc_Linux::ChildProcs.ahprocChildProcs()[c4Idx];
            if (phprocCur && phprocCur->hprociThis().pidThis == pid)
            {
                TProcessHandleImpl& hprociThis = const_cast<TProcessHandleImpl&>(phprocCur->hprociThis());

                if (WIFEXITED(iStatus))
                    hprociThis.eExit = tCIDLib::EExitCodes(WEXITSTATUS(iStatus));
                else
                    hprociThis.eExit = tCIDLib::EExitCodes::SystemException;

                hprociThis.UserTime.tv_sec = ResourceUsage.ru_utime.tv_sec;
                hprociThis.UserTime.tv_usec = ResourceUsage.ru_utime.tv_usec;
                hprociThis.KernelTime.tv_sec = ResourceUsage.ru_stime.tv_sec;
                hprociThis.KernelTime.tv_usec = ResourceUsage.ru_stime.tv_usec;
                hprociThis.EndTime.tv_sec = TimeEntered.tv_sec;
                hprociThis.EndTime.tv_usec = TimeEntered.tv_usec;

                hprociThis.bAlreadyClean = kCIDLib::True;

                return;
            }
        }
    }

    // This function looks up the boot time in the /proc file system.
    tCIDLib::TBoolean bQueryBootTime(tCIDLib::TCard4& c4ToFill)
    {
        static tCIDLib::TCard4 c4BootTime = 0;

        if (c4BootTime == 0)
        {
            FILE* KrnlStat = ::fopen("/proc/stat", "r");
            if (!KrnlStat)
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }

            tCIDLib::TSCh szLine[129];
            for (;;)
            {
                if (!::fgets(szLine, 129, KrnlStat))
                    break;

                if (!::strncmp(szLine, "btime", 5))
                {
                    if (::sscanf(szLine, "%*s %lu", &c4BootTime) != 1)
                        continue;
                    break;
                }
            }

            ::fclose(KrnlStat);

            if (!c4BootTime)
            {
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
                return kCIDLib::False;
            }
        }

        c4ToFill = c4BootTime;

        return kCIDLib::True;
    }

    // This function queries the start time of a running process.
    tCIDLib::TBoolean bQueryRawStartTime(pid_t pidToQuery, tCIDLib::TCard4& c4RawStartTime)
    {
        tCIDLib::TSCh szFileName[kCIDLib::c4MaxPathLen + 1];
        ::sprintf(szFileName, "/proc/%d/stat", pidToQuery);
        FILE* StatFile = ::fopen(szFileName, "r");
        if (!StatFile)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TSInt iRet = ::fscanf
        (
            StatFile
            , "%*u %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %*u %lu"
            , &c4RawStartTime
        );

        ::fclose(StatFile);

        if (iRet != 1)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        return kCIDLib::True;
    }

    // This function queries interesting timing information of a currently
    // running process.
    tCIDLib::TBoolean bQueryRawTimes(   pid_t pidToQuery
                                       , tCIDLib::TCard4& c4RawUserTime
                                       , tCIDLib::TCard4& c4RawKernelTime)
    {
        tCIDLib::TSCh szStatPath[kCIDLib::c4MaxPathLen + 1];
        ::sprintf(szStatPath, "/proc/%d/stat", pidToQuery);

        FILE* StatFile = ::fopen(szStatPath, "r");
        if (!StatFile)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TSInt iRet = ::fscanf(StatFile,
                                       "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                                       &c4RawUserTime,
                                       &c4RawKernelTime);

        ::fclose(StatFile);

        if (iRet != 3)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        return kCIDLib::True;
    }
}


//
// Here is where the SIGCHLD handler is registered. We need to handle this signal
// so that child process can be appropriately cleaned up.
//
tCIDLib::TBoolean TKrnlLinux::bInitTermExtProcess(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        struct sigaction SigAction;
        SigAction.sa_handler = CIDKernel_ExtProc_Linux::ChildSignalHandler;
        ::sigemptyset(&SigAction.sa_mask);
        SigAction.sa_flags = 0;
        ::sigaction(SIGCHLD, &SigAction, 0);
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlExtProcess
//  PREFIX: kextp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlExtProcess: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlExtProcess::TKrnlExtProcess()
{
}

TKrnlExtProcess::~TKrnlExtProcess()
{
    CIDKernel_ExtProc_Linux::ChildProcs.RemoveChildProc(&m_hprocThis);
}


// ---------------------------------------------------------------------------
//  TKrnlExtProcess: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If this object has not been previously set, we can adopt a process and
//  gain access to it, given that the handle to it has been obtained.
//
tCIDLib::TBoolean TKrnlExtProcess::bAdoptProcess(TProcessHandle& hprocToAdopt)
{
    if (m_hprocThis.m_phprociThis->pidThis != 0)
    {
        // This object has already been associated with a process
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);

        //
        //  There's nothing we can do with the passed handle. It's just a process
        //  id so we don't close it or anything. It just gets dropped.
        //
        return kCIDLib::False;
    }

    // Copy over the process handle info
    m_hprocThis.m_phprociThis->pidThis = hprocToAdopt.m_phprociThis->pidThis;

    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlExtProcess::bEverStarted(tCIDLib::TBoolean& bToFill) const
{
    bToFill = (m_hprocThis.m_phprociThis->pidThis != 0)
              && !m_hprocThis.m_phprociThis->bAlreadyClean;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bGetPriorityClass(tCIDLib::EPrioClasses& eToFill) const
{
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (!bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }

    tCIDLib::TSInt iPolicy = ::sched_getscheduler(m_hprocThis.m_phprociThis->pidThis);
    if (iPolicy == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    // The assumption is that the only reason to switch from the default scheduler
    // is to do real time.
    if (iPolicy == SCHED_FIFO || iPolicy == SCHED_RR)
        eToFill = tCIDLib::EPrioClasses::RealTime;
    else
        eToFill = tCIDLib::EPrioClasses::Normal;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bIsRunning(tCIDLib::TBoolean& bToFill) const
{
    if (!m_hprocThis.m_phprociThis->pidThis
    ||  m_hprocThis.m_phprociThis->bAlreadyClean)
    {
        bToFill = kCIDLib::False;
    }
    else
    {
        bToFill = kCIDLib::True;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlExtProcess::bKill()
{
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (!bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }

    // Do a SIGTERM instead of SIGKILL so that the process can clean up.
    if (::kill(m_hprocThis.m_phprociThis->pidThis, SIGTERM))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bQueryFullPath(        tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    // We have to be running for this to work
    tCIDLib::TBoolean bState;
    if (!bIsRunning(bState) || !bState)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NeverStarted);
        return kCIDLib::False;
    }

    char pszSym[32];
    char pszPath[PATH_MAX + 1] = {0};

    sprintf(pszSym, "/proc/%d/exe", m_hprocThis.m_phprociThis->pidThis);
    if (::readlink(pszSym, pszPath, PATH_MAX) == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;    
    }

    tCIDLib::TCard4 c4OutChars;
    return CIDStrOp_MBToWC(pszToFill, c4MaxChars, pszPath, c4OutChars);
}


tCIDLib::TBoolean
TKrnlExtProcess::bQueryProcessTimes(tCIDLib::TEncodedTime&     enctStart
                                    , tCIDLib::TEncodedTime&   enctEnd
                                    , tCIDLib::TEncodedTime&   enctInKernel
                                    , tCIDLib::TEncodedTime&   enctInUserMode)
{
    tCIDLib::TCard4 c4BootTime;
    if (!CIDKernel_ExtProc_Linux::bQueryBootTime(c4BootTime))
        return kCIDLib::False;

    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    // First set the start time
    TKrnlLinux::LinuxFileTimeToCIDFileTime(c4BootTime, enctStart);
    enctStart += tCIDLib::TEncodedTime(m_hprocThis.m_phprociThis->c4JiffyStartTime) * 100000;

    // We need to use a different method of getting the time if the process is still
    // running.
    if (bRun)
    {
        tCIDLib::TCard4 c4JiffyUserTime;
        tCIDLib::TCard4 c4JiffyKernelTime;

        if (!CIDKernel_ExtProc_Linux::bQueryRawTimes(m_hprocThis.m_phprociThis->pidThis
                                                    , c4JiffyUserTime
                                                    , c4JiffyKernelTime))
        {
            return kCIDLib::False;
        }

        // Now user time
        enctInUserMode = tCIDLib::TEncodedTime(c4JiffyUserTime) * 100000;

        // And kernel time
        enctInKernel = tCIDLib::TEncodedTime(c4JiffyKernelTime) * 100000;

        // End time is not relevant
        enctEnd = 0;
    }
    else
    {
        // Now user time
        tCIDLib::TEncodedTime enctSec = m_hprocThis.m_phprociThis->UserTime.tv_sec;
        tCIDLib::TEncodedTime enctUSec = m_hprocThis.m_phprociThis->UserTime.tv_usec;
        enctInUserMode = (enctSec * 1000000 + enctUSec) * 10;

        // And kernel time
        enctSec = m_hprocThis.m_phprociThis->KernelTime.tv_sec;
        enctUSec = m_hprocThis.m_phprociThis->KernelTime.tv_usec;
        enctInKernel = (enctSec * 1000000 + enctUSec) * 10;

        // Now end time
        TKrnlLinux::LinuxFileTimeToCIDFileTime(m_hprocThis.m_phprociThis->EndTime.tv_sec, enctEnd);
        enctEnd += tCIDLib::TEncodedTime(m_hprocThis.m_phprociThis->EndTime.tv_usec) * 10;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bSetPriorityClass(const tCIDLib::EPrioClasses eNewClass)
{
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (!bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }

    // We only support Normal and RealTime
    if (eNewClass != tCIDLib::EPrioClasses::Normal
        && eNewClass != tCIDLib::EPrioClasses::RealTime)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    struct sched_param SchedParam;
    tCIDLib::TSInt iPolicy;

    if (eNewClass == tCIDLib::EPrioClasses::Normal)
    {
        iPolicy = SCHED_OTHER;
        SchedParam.sched_priority = 0;
    }
    else
    {
        iPolicy = SCHED_RR;
        SchedParam.sched_priority = ::sched_get_priority_min(SCHED_RR);
    }

    if (::sched_setscheduler(m_hprocThis.m_phprociThis->pidThis
                             , iPolicy
                             , &SchedParam))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}




tCIDLib::TBoolean
TKrnlExtProcess::bStart(const   tCIDLib::TCh* const     pszPath
                        , const tCIDLib::TCh* const     pszInitPath
                        ,       tCIDKernel::TStrList&   klistParms
                        ,       tCIDKernel::TStrList&   klistEnviron
                        , const tCIDLib::EExtProcFlags  eFlag
                        , const tCIDLib::EExtProcShows  eShow)
{
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    //
    // Make the array for arguments. Add two to the size because the first argument
    // should be the name of the file to execute and the last should be zero.
    //
    tCIDLib::TSCh** apszParamCopy = new tCIDLib::TSCh*[klistParms.c4ElemCount() + 2];

    tCIDLib::TCh szNameExt[kCIDLib::c4MaxPathLen + 1];
    TKrnlPathStr::bQueryNameExt(pszPath, szNameExt, c4MaxBufChars(szNameExt));
    tCIDLib::TCard4 c4ArrayIndex = 0;
    
    apszParamCopy[c4ArrayIndex++] = TRawStr::pszConvert(szNameExt);

    if (klistParms.bResetCursor())
    {
        TKrnlString* pkstrCur = klistParms.pobjHead();
        do
        {
            apszParamCopy[c4ArrayIndex++] = TRawStr::pszConvert(pkstrCur->pszValue());            
        }   while (klistParms.bNext(pkstrCur));
    }
    apszParamCopy[c4ArrayIndex++] = 0;

    //
    //  Now convert the file name.
    //
    //  Search for it manually in the path for simplicity. The other option
    //  would be to call "execvp", but that would only work if there is no
    //  environment passed in. It doesn't make sense to introduce the extra
    //  complexity.
    //
    tCIDLib::TSCh* pszFileName = TKrnlLinux::pszFindInPath(pszPath);
    TArrayJanitor<tCIDLib::TSCh> janPath(pszFileName);

    // Only worry about the environment if there is one.
    tCIDLib::TSCh** apszEnvironCopy = nullptr;
    if (!klistEnviron.bIsEmpty())
    {
        apszEnvironCopy = new tCIDLib::TSCh*[klistEnviron.c4ElemCount() + 1];
        c4ArrayIndex = 0;
        if (klistEnviron.bResetCursor())
        {
            TKrnlString* pkstrCur = klistEnviron.pobjHead();
            do
            {
                apszEnvironCopy[c4ArrayIndex++] = TRawStr::pszConvert(pkstrCur->pszValue());            
            }   while (klistEnviron.bNext(pkstrCur));
        }
        apszEnvironCopy[c4ArrayIndex] = 0;
    }

    // Before we make the new process, we need to block child signals.
    // We do this in order to set up the handle before the child gets
    // a chance to exit.
    // The danger is that the process end time will be slightly off if
    // another child sends its SIGCHLD while we're blocked, but that's a
    // small risk.
    sigset_t NewSigSet;
    sigset_t OldSigSet;
    ::sigemptyset(&NewSigSet);
    ::sigaddset(&NewSigSet, SIGCHLD);
    ::sigprocmask(SIG_BLOCK, &NewSigSet, &OldSigSet);

    // Make a new process
    pid_t pidTmp = ::fork();

    // -1 is the error value for fork()
    if (pidTmp == -1)
    {
        // Restore the old mask
        ::sigprocmask(SIG_SETMASK, &OldSigSet, 0);

        // Clean up the strings we converted
        c4ArrayIndex = 0;
        while (apszParamCopy[c4ArrayIndex])
            delete [] apszParamCopy[c4ArrayIndex++];
        delete [] apszParamCopy;

        if (apszEnvironCopy)
        {
            c4ArrayIndex = 0;
            while (apszEnvironCopy[c4ArrayIndex])
                delete [] apszEnvironCopy[c4ArrayIndex++];
            delete [] apszEnvironCopy;
        }
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    //
    // pidTmp equals zero if we're in the child process. In that case
    // we need to execute the file.
    //
    if (pidTmp == 0)
    {
        if (apszEnvironCopy)
            ::execve(pszFileName, apszParamCopy, apszEnvironCopy);
        else
            ::execv(pszFileName, apszParamCopy);

        // We only get here if exec failed.
        tCIDLib::EExitCodes eExit;
        if (errno == ENOENT)
            eExit = tCIDLib::EExitCodes::NotFound;
        else if (errno == EPERM || errno == EACCES)
            eExit = tCIDLib::EExitCodes::PermissionLevel;
        else if (errno == E2BIG || errno == ENAMETOOLONG || errno == ENOTDIR)
            eExit = tCIDLib::EExitCodes::BadParameters;
        else
            eExit = tCIDLib::EExitCodes::ResourceAccess;

        // Call _exit() instead of exit() to avoid flushing buffered streams
        // that actually belong to the parent.
        ::_exit(tCIDLib::c4EnumOrd(eExit));
    }

    // We're in the parent process
    // Get its start time. Since SIGCHLD is blocked, we can be sure
    // that this data is available.
    if (!CIDKernel_ExtProc_Linux::bQueryRawStartTime(pidTmp, m_hprocThis.m_phprociThis->c4JiffyStartTime))
        m_hprocThis.m_phprociThis->c4JiffyStartTime = 0;

    // Set up the handle
    m_hprocThis.m_phprociThis->pidThis = pidTmp;

    // Add it to the list of processes for which we're waiting. This handle will be
    // removed from the list when this object is destroyed.
    CIDKernel_ExtProc_Linux::ChildProcs.AddChildProc(&m_hprocThis);

    // Now it's safe to restore the old signal mask, which should unblock
    // the SIGCHLD signal.
    // If it doesn't, then I did something wrong. :)
    ::sigprocmask(SIG_SETMASK, &OldSigSet, 0);

    // Delete the strings we converted
    c4ArrayIndex = 0;
    while (apszParamCopy[c4ArrayIndex])
        delete [] apszParamCopy[c4ArrayIndex++];
    delete [] apszParamCopy;

    if (apszEnvironCopy)
    {
        c4ArrayIndex = 0;
        while (apszEnvironCopy[c4ArrayIndex])
            delete [] apszEnvironCopy[c4ArrayIndex++];
        delete [] apszEnvironCopy;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bStart(const   tCIDLib::TCh* const     pszStartString
                        , const tCIDLib::TCh* const     pszInitPath
                        ,       tCIDKernel::TStrList&   klistEnviron
                        , const tCIDLib::EExtProcFlags  eFlag
                        , const tCIDLib::EExtProcShows  eShow)
{
    // Break out the start string into a program to run plus parameters
    

    return kCIDLib::False;
}                        


tCIDLib::TBoolean
TKrnlExtProcess::bStartAsOpen(  const   tCIDLib::TCh* const     pszDocPath
                                , const tCIDLib::EExtProcShows  eShow
                                , const tCIDLib::TBoolean       bAdminPrivs)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}                                



tCIDLib::TBoolean
TKrnlExtProcess::bSystemEscape(const    tCIDLib::TCh* const     pszCommandLine
                               , const  tCIDLib::ESysEscFlags   eFlags)
{
    //
    //  This guy just needs to create a new string which runs the system
    //  command shell and passes the passed command line to it as a
    //  set of parameters.
    //
    const tCIDLib::TCh* const   pszPrefix = L"/bin/sh ";
    const tCIDLib::TCard4       c4BufSize = 4095;
    tCIDLib::TCh                szCmdLine[c4BufSize + 1];

    const tCIDLib::TCh* pszShellSwitches;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESysEscFlags::StayOpen))
        pszShellSwitches = L"-i -c ";
    else
        pszShellSwitches = L"-c ";

    TRawStr::CopyCatStr(szCmdLine
                        , c4MaxBufChars(szCmdLine)
                        , pszPrefix
                        , pszShellSwitches
                        , pszCommandLine);

    // And now just call the appropriate start command, run it non-detached
    if (!bStart(szCmdLine, L"", tCIDLib::EExtProcFlags::None, tCIDLib::EExtProcShows::Normal))
        return kCIDLib::False;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESysEscFlags::Wait))
    {
        tCIDLib::TBoolean bState;
        tCIDLib::EExitCodes eExit;
        return bWaitForDeath(bState, eExit, kCIDLib::c4MaxWait);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bTryAttachTo(  const   tCIDLib::TCh* const pszAttachInfo
                                ,       tCIDLib::TBoolean&  bAttached)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlExtProcess::bWaitForDeath(         tCIDLib::TBoolean&      bState
                                ,       tCIDLib::EExitCodes&    eToFill
                                , const tCIDLib::TCard4         c4MilliSecs) const
{
    if (!m_hprocThis.m_phprociThis->pidThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NeverStarted);
        return kCIDLib::False;
    }

    // If it's already clean, then we're off the hook.
    if (m_hprocThis.m_phprociThis->bAlreadyClean)
    {
        eToFill = m_hprocThis.m_phprociThis->eExit;
        return kCIDLib::True;
    }

    struct timespec TimeRequested;
    struct timespec TimeRemaining;

    TimeRequested.tv_sec = c4MilliSecs / 1000;
    TimeRequested.tv_nsec = (c4MilliSecs % 1000) * 1000000;

    // All we need to do is wait. If the SIGCHLD is delivered while we're waiting,
    // then the nanosleep will be interrupted. It's so convenient!
    while (::nanosleep(&TimeRequested, &TimeRemaining))
    {
        if (errno != EINTR)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        if (m_hprocThis.m_phprociThis->bAlreadyClean)
        {
            eToFill = m_hprocThis.m_phprociThis->eExit;
            return kCIDLib::True;
        }

        TimeRequested.tv_sec = TimeRemaining.tv_sec;
        TimeRequested.tv_nsec = TimeRemaining.tv_nsec;
    }

    if (m_hprocThis.m_phprociThis->bAlreadyClean)
    {
        eToFill = m_hprocThis.m_phprociThis->eExit;
        return kCIDLib::True;
    }

    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
    return kCIDLib::False;
}


