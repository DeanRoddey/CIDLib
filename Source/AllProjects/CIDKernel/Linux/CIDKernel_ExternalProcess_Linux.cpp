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


namespace
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

        TProcessHandle** __aphprocChildProcs;
        tCIDLib::TCard4  __c4Size;
        TKrnlLinux::TRecursiveMutex __rmtxLock;
    };

    TChildProcs::TChildProcs()
    {
        __c4Size = 0;
        __aphprocChildProcs = 0;
        __rmtxLock.iInitialize();
    }

    TChildProcs::~TChildProcs()
    {
        __rmtxLock.iLock();
        delete [] __aphprocChildProcs;
        __rmtxLock.iUnlock();
        __rmtxLock.iDestroy();
    }

    tCIDLib::TVoid TChildProcs::AddChildProc(TProcessHandle* phprocChild)
    {
        __rmtxLock.iLock();
        tCIDLib::TCard4 c4Idx;
        for (c4Idx = 0; c4Idx < __c4Size; c4Idx++)
        {
            if (__aphprocChildProcs[c4Idx] == 0)
            {
                __aphprocChildProcs[c4Idx] = phprocChild;
                __rmtxLock.iUnlock();
                return;
            }
        }
        tCIDLib::TCard4 c4OldSize = __c4Size;
        __c4Size += c4DefaultChunkSize;
        TProcessHandle** aphprocTmp = new TProcessHandle*[__c4Size];
        for (c4Idx = 0; c4Idx < c4OldSize; c4Idx++)
            aphprocTmp[c4Idx] = __aphprocChildProcs[c4Idx];
        aphprocTmp[c4OldSize] = phprocChild;
        for (c4Idx = c4OldSize + 1; c4Idx < __c4Size; c4Idx++)
            aphprocTmp[c4Idx] = 0;
        delete [] __aphprocChildProcs;
        __aphprocChildProcs = aphprocTmp;
        __rmtxLock.iUnlock();
    }

    inline TProcessHandle** TChildProcs::ahprocChildProcs()
    {
        return __aphprocChildProcs;
    }

    inline tCIDLib::TCard4 TChildProcs::c4Size()
    {
        return __c4Size;
    }

    tCIDLib::TVoid TChildProcs::RemoveChildProc(TProcessHandle* phprocChild)
    {
        __rmtxLock.iLock();
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < __c4Size; c4Idx++)
        {
            if (__aphprocChildProcs[c4Idx] == phprocChild)
            {
                __aphprocChildProcs[c4Idx] = 0;
                __rmtxLock.iUnlock();
                return;
            }
        }
        __rmtxLock.iUnlock();
    }

    static TChildProcs __ChildProcs;

    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------

    // This function does the work of collecting termination information about child
    // processes. It processes SIGCHLD signals and stores appropriate information in
    // the process handle.
    tCIDLib::TVoid __ChildSignalHandler(tCIDLib::TSInt iSignal)
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
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < __ChildProcs.c4Size(); c4Idx++)
        {
            TProcessHandle* phprocCur = __ChildProcs.ahprocChildProcs()[c4Idx];
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
    tCIDLib::TBoolean __bQueryBootTime(tCIDLib::TCard4& c4ToFill)
    {
        static tCIDLib::TCard4 __c4BootTime = 0;

        if (__c4BootTime == 0)
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
                    if (::sscanf(szLine, "%*s %d", &__c4BootTime) != 1)
                        continue;
                    break;
                }
            }

            ::fclose(KrnlStat);

            if (!__c4BootTime)
            {
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
                return kCIDLib::False;
            }
        }

        c4ToFill = __c4BootTime;

        return kCIDLib::True;
    }

    // This function queries the start time of a running process.
    tCIDLib::TBoolean __bQueryRawStartTime(pid_t pidToQuery, tCIDLib::TCard4& c4RawStartTime)
    {
        tCIDLib::TSCh szFileName[kCIDLib::c4MaxPathLen + 1];
        ::sprintf(szFileName, "/proc/%d/stat", pidToQuery);
        FILE* StatFile = ::fopen(szFileName, "r");
        if (!StatFile)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TSInt iRet = ::fscanf(StatFile,
                                       "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %*u %d",
                                       &c4RawStartTime);

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
    tCIDLib::TBoolean __bQueryRawTimes(pid_t pidToQuery
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
                                       "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %d %d",
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

// Here is where the SIGCHLD handler is registered. We need to handle this signal
// so that child process can be appropriately cleaned up.
tCIDLib::TBoolean TKrnlLinux::bInitTermExtProcess(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        struct sigaction SigAction;
        SigAction.sa_handler = __ChildSignalHandler;
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
    __ChildProcs.RemoveChildProc(&__hprocThis);
}


// ---------------------------------------------------------------------------
//  TKrnlExtProcess: Public, non-virtual methods
// ---------------------------------------------------------------------------
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

    tCIDLib::TSInt iPolicy = ::sched_getscheduler(__hprocThis.__phprociThis->pidThis);
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
    if (!__hprocThis.__phprociThis->pidThis
        || __hprocThis.__phprociThis->bAlreadyClean)
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
    if (::kill(__hprocThis.__phprociThis->pidThis, SIGTERM))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
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

    if (::sched_setscheduler(__hprocThis.__phprociThis->pidThis
                             , iPolicy
                             , &SchedParam))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bQueryProcessTimes(tCIDLib::TEncodedTime&     enctStart
                                    , tCIDLib::TEncodedTime&   enctEnd
                                    , tCIDLib::TEncodedTime&   enctInKernel
                                    , tCIDLib::TEncodedTime&   enctInUserMode)
{
    tCIDLib::TCard4 c4BootTime;
    if (!__bQueryBootTime(c4BootTime))
        return kCIDLib::False;

    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    // First set the start time
    TKrnlLinux::LinuxFileTimeToCIDFileTime(c4BootTime, enctStart);
    enctStart += tCIDLib::TEncodedTime(__hprocThis.__phprociThis->c4JiffyStartTime) * 100000;

    // We need to use a different method of getting the time if the process is still
    // running.
    if (bRun)
    {
        tCIDLib::TCard4 c4JiffyUserTime;
        tCIDLib::TCard4 c4JiffyKernelTime;

        if (!__bQueryRawTimes(__hprocThis.__phprociThis->pidThis
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
        tCIDLib::TEncodedTime enctSec = __hprocThis.__phprociThis->UserTime.tv_sec;
        tCIDLib::TEncodedTime enctUSec = __hprocThis.__phprociThis->UserTime.tv_usec;
        enctInUserMode = (enctSec * 1000000 + enctUSec) * 10;

        // And kernel time
        enctSec = __hprocThis.__phprociThis->KernelTime.tv_sec;
        enctUSec = __hprocThis.__phprociThis->KernelTime.tv_usec;
        enctInKernel = (enctSec * 1000000 + enctUSec) * 10;

        // Now end time
        TKrnlLinux::LinuxFileTimeToCIDFileTime(__hprocThis.__phprociThis->EndTime.tv_sec, enctEnd);
        enctEnd += tCIDLib::TEncodedTime(__hprocThis.__phprociThis->EndTime.tv_usec) * 10;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bStart(const   tCIDLib::TCh* const     pszPath
                        ,       tCIDLib::TCh**          apszParms
                        , const tCIDLib::TCard4         c4ParmCount
                        ,       tCIDLib::TCh**          apszEnviron
                        , const tCIDLib::TCard4         c4EnvironCount
                        , const tCIDLib::EExtProcFlags  eFlag)
{
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    // Make the array for arguments. Add two to the size because the first argument
    // should be the name of the file to execute and the last should be zero.
    tCIDLib::TCard4 c4Counter;
    tCIDLib::TSCh** apszParamCopy = new tCIDLib::TSCh*[c4ParmCount + 2];

    tCIDLib::TCh szNameExt[kCIDLib::c4MaxPathLen + 1];
    TKrnlPathStr::bQueryNameExt(pszPath, szNameExt, c4MaxBufChars(szNameExt));
    apszParamCopy[0] = TRawStr::pszConvert(szNameExt);

    for (c4Counter = 1; c4Counter <= c4ParmCount; c4Counter++)
        apszParamCopy[c4Counter] = TRawStr::pszConvert(apszParms[c4Counter - 1]);
    apszParamCopy[c4Counter] = 0;

    // Now convert the file name.
    // Search for it manually in the path for simplicity. The other option
    // would be to call "execvp", but that would only work if there is no
    // environment passed in. It doesn't make sense to introduce the extra
    // complexity.
    tCIDLib::TSCh* pszFileName = TKrnlLinux::pszFindInPath(pszPath);
    TArrayJanitor<tCIDLib::TSCh> janPath(pszFileName);

    // Only worry about the environment if there is one.
    tCIDLib::TSCh** apszEnvironCopy = 0;
    if (c4EnvironCount)
    {
        apszEnvironCopy = new tCIDLib::TSCh*[c4EnvironCount + 1];
        for (c4Counter = 0; c4Counter < c4ParmCount; c4Counter++)
            apszEnvironCopy[c4Counter] = TRawStr::pszConvert(apszEnviron[c4Counter]);
        apszEnvironCopy[c4Counter] = 0;
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
        c4Counter = 0;
        while (apszParamCopy[c4Counter])
            delete [] apszParamCopy[c4Counter++];
        delete [] apszParamCopy;
        if (c4EnvironCount)
        {
            c4Counter = 0;
            while (apszEnvironCopy[c4Counter])
                delete [] apszEnvironCopy[c4Counter++];
            delete [] apszEnvironCopy;
        }
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    // pidTmp equals zero if we're in the child process. In that case
    // we need to execute the file.
    if (pidTmp == 0)
    {
        if (c4EnvironCount)
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
        ::_exit(eExit);
    }

    // We're in the parent process
    // Get its start time. Since SIGCHLD is blocked, we can be sure
    // that this data is available.
    if (!__bQueryRawStartTime(pidTmp, __hprocThis.__phprociThis->c4JiffyStartTime))
        __hprocThis.__phprociThis->c4JiffyStartTime = 0;

    // Set up the handle
    __hprocThis.__phprociThis->pidThis = pidTmp;

    // Add it to the list of processes for which we're waiting. This handle will be
    // removed from the list when this object is destroyed.
    __ChildProcs.AddChildProc(&__hprocThis);

    // Now it's safe to restore the old signal mask, which should unblock
    // the SIGCHLD signal.
    // If it doesn't, then I did something wrong. :)
    ::sigprocmask(SIG_SETMASK, &OldSigSet, 0);

    // Delete the strings we converted
    c4Counter = 0;
    while (apszParamCopy[c4Counter])
        delete [] apszParamCopy[c4Counter++];
    delete [] apszParamCopy;
    if (c4EnvironCount)
    {
        c4Counter = 0;
        while (apszEnvironCopy[c4Counter])
            delete [] apszEnvironCopy[c4Counter++];
        delete [] apszEnvironCopy;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bStart(const   tCIDLib::TCh* const     pszStartString
                        ,       tCIDLib::TCh**          apszEnviron
                        , const tCIDLib::TCard4         c4EnvironCount
                        , const tCIDLib::EExtProcFlags  eFlag)
{
    //  Ok, first check to make sure that we are not already running. If
    //  we are, then that's this is not legal.
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    //  Parse the command line into its components. This both makes sure that
    //  its legal, and allows us to separate out the program part of the
    //  command line (so that we can see that type of program it is.)
    const tCIDLib::TCard4 c4MaxParmCnt = 512;
    tCIDLib::TCh** apszParms = new tCIDLib::TCh*[c4MaxParmCnt];

    tCIDLib::TCard4 c4ActualParmCnt =
        c4BreakOutParms(pszStartString, apszParms, c4MaxParmCnt);

    tCIDLib::TBoolean bRet;
    if (c4ActualParmCnt != 0)
    {
        bRet = bStart(apszParms[0]
                      , &apszParms[1]
                      , c4ActualParmCnt - 1
                      , apszEnviron
                      , c4EnvironCount
                      , eFlag);
    }
    else
    {
        bRet = kCIDLib::False;
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcExtP_EmptyCmdLine);
    }

    // Clean up the params
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ActualParmCnt; c4Index++)
        delete [] apszParms[c4Index];
    delete [] apszParms;

    return bRet;
}


tCIDLib::TBoolean
TKrnlExtProcess::bSystemEscape(const tCIDLib::TCh* const pszCommandLine
                               , const tCIDLib::ESysEscFlags   eFlags)
{
    //
    //  This guy just needs to create a new string which runs the system
    //  command shell and passes the passed command line to it as a
    //  set of parameters.
    //
    const tCIDLib::TCh* const   pszPrefix = L"/bin/sh ";
    const tCIDLib::TCard4       c4BufSize = 4095;
    tCIDLib::TCh                szCmdLine[c4BufSize + 1];

    tCIDLib::TCh* pszShellSwitches;
    if (eFlags & tCIDLib::ESysEscFlags::StayOpen)
        pszShellSwitches = L"-i -c ";
    else
        pszShellSwitches = L"-c ";

    TRawStr::CopyCatStr(szCmdLine
                        , c4MaxBufChars(szCmdLine)
                        , pszPrefix
                        , pszShellSwitches
                        , pszCommandLine);

    // And now just call the appropriate start command, run it non-detached
    if (!bStart(szCmdLine, tCIDLib::EExtProcFlags::NonDetached))
        return kCIDLib::False;

    if (eFlags & tCIDLib::ESysEscFlags::Wait)
    {
        tCIDLib::EExitCodes eExit;
        return bWaitForDeath(eExit);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bWaitForDeath(         tCIDLib::EExitCodes& eToFill
                                , const tCIDLib::TCard4      c4MilliSecs) const
{
    if (!__hprocThis.__phprociThis->pidThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NeverStarted);
        return kCIDLib::False;
    }

    // If it's already clean, then we're off the hook.
    if (__hprocThis.__phprociThis->bAlreadyClean)
    {
        eToFill = __hprocThis.__phprociThis->eExit;
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

        if (__hprocThis.__phprociThis->bAlreadyClean)
        {
            eToFill = __hprocThis.__phprociThis->eExit;
            return kCIDLib::True;
        }

        TimeRequested.tv_sec = TimeRemaining.tv_sec;
        TimeRequested.tv_nsec = TimeRemaining.tv_nsec;
    }

    if (__hprocThis.__phprociThis->bAlreadyClean)
    {
        eToFill = __hprocThis.__phprociThis->eExit;
        return kCIDLib::True;
    }

    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
    return kCIDLib::False;
}
