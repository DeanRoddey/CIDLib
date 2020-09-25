//
// FILE NAME: CIDKernel_SignalHandler_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 07/25/1999
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
//  This file provides the Linux specific signal handling stuff.
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


namespace
{
    // ---------------------------------------------------------------------------
    //  Local constants
    // ---------------------------------------------------------------------------
    const tCIDLib::TCard4 c4SigWatchInterval = 500;

    // ---------------------------------------------------------------------------
    //  Local types
    // ---------------------------------------------------------------------------
    struct TSigMap
    {
        tCIDLib::ESignals   eSignal;
        tCIDLib::TSInt      iHostSignal;
        sig_atomic_t        PendingCount;
    };

    // ---------------------------------------------------------------------------
    //  Local data
    // ---------------------------------------------------------------------------
    TSigMap aSigMap[] =
    {
          { tCIDLib::ESignals::Break    ,   SIGINT  ,   0   }
        , { tCIDLib::ESignals::Shutdown ,   SIGTERM ,   0   }
    };

    const tCIDLib::TSInt aProgramErrors[] =
    {
        SIGFPE
        , SIGILL
        , SIGSEGV
        , SIGBUS
        , SIGABRT
        , SIGIOT
    };

    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    tCIDLib::TVoid SignalHandler(tCIDLib::TSInt iSignal)
    {
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < tCIDLib::c4ArrayElems(aSigMap); c4Idx++)
        {
            if (iSignal == aSigMap[c4Idx].iHostSignal)
            {
                aSigMap[c4Idx].PendingCount++;
                break;
            }
        }
    }

    //
    // Here is where we trap all program error signals. They are logged, then the
    // default action of the signal is taken. The default action is always termination
    // for these signals.
    // The signals handled here are: SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT, SIGIOT,
    //                               SIGEMT, SIGSYS
    // Note: SIGTRAP is not handled on purpose.
    //
    tCIDLib::TVoid ProgramErrorHandler(tCIDLib::TSInt iSignal)
    {
        // First, dump this signal to a log
        TKrnlLinux::TKrnlThreadInfo* pThreadInfo = TKrnlLinux::TKrnlThreadInfo::pkthriInstance();

        if (pThreadInfo)
        {
            // <TBD> Anything we can do with this?
            /*
            TKrnlThread::DumpException(*pThreadInfo->pkthrThis()
                                       , pThreadInfo->pszName()
                                       , (tCIDLib::TVoid*)iSignal);
            */
        }

        //
        // This is tricky. Reset the signal action to the default, then raise the
        // signal again. This has two effects: (1) It will cause a core dump to be
        // created if the signal would normally create one, (2) It will set the exit
        // code of the program properly.
        //
        struct sigaction DefaultAction;
        DefaultAction.sa_handler = SIG_DFL;
        ::sigemptyset(&DefaultAction.sa_mask);
        DefaultAction.sa_flags = 0;
        ::sigaction(iSignal, &DefaultAction, 0);
        ::raise(iSignal);
    }

    tCIDLib::TVoid* pSignalWatch(tCIDLib::TVoid*)
    {
        const tCIDLib::TCard4 c4Seconds = c4SigWatchInterval / 1000;
        const tCIDLib::TCard4 c4NanoSeconds = (c4SigWatchInterval % 1000) * 1000000;

        struct timespec TimeRequested;
        struct timespec TimeRemaining;

        while (kCIDLib::True)
        {
            TimeRequested.tv_sec = c4Seconds;
            TimeRequested.tv_nsec = c4NanoSeconds;

            // Spend most of the time sleeping
            // Here there are two possible errno values. One is a program
            // error, which I'm going to assume will not happen since this
            // code is completely hidden. Therefore, I'm only handling
            // EINTR, which says that it was interrupted and there's time
            // remaining.
            while (::nanosleep(&TimeRequested, &TimeRemaining))
            {
                TimeRequested.tv_sec = TimeRemaining.tv_sec;
                TimeRequested.tv_nsec = TimeRemaining.tv_nsec;
            }

            // Now check to see if a signal was delivered
            for (tCIDLib::TCard4 c4Idx = 0; c4Idx < tCIDLib::c4ArrayElems(aSigMap); c4Idx++)
            {
                // For each pending signal, call the handler list
                while (aSigMap[c4Idx].PendingCount)
                {
                    TKrnlSignals::bCallHandlers(aSigMap[c4Idx].eSignal);
                    aSigMap[c4Idx].PendingCount--;
                }
            }
        }
    }
}


// ---------------------------------------------------------------------------
//  TKrnlSignals methods
//
//  The platform independent section calls this during its initialization.
//  At this point, the platform independent handler system infrastructure is
//  set up.
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlSignals::PlatformInit()
{
    // First kick off the thread that will watch for signals that
    // use chains of handlers
    pthread_attr_t ThreadAttr;
    ::pthread_attr_init(&ThreadAttr);
    ::pthread_attr_setdetachstate(&ThreadAttr, 1);

    tCIDLib::TThreadId tidTmp;
    tCIDLib::TOSErrCode HostErr;
    HostErr = ::pthread_create(&tidTmp
                               , &ThreadAttr
                               , pSignalWatch
                               , 0);
    ::pthread_attr_destroy(&ThreadAttr);
    if (HostErr)
    {
        TKrnlError::SetLastHostError(HostErr);
        const TKrnlError& kerrLast = TKrnlError::kerrLast();
        TKrnlPopup::Show(CID_FILE
                         , CID_LINE
                         , L"Platform Signal Initialization"
                         , 0
                         , 0
                         , kerrLast.errcId()
                         , kerrLast.errcHostId()
                         , L"Could not create signal watching thread");
    }

    // Now set up the signal handler for the chain users
    struct sigaction SigAction;
    ::memset(&SigAction, 0, sizeof(SigAction));
    SigAction.sa_handler = SignalHandler;
    ::sigemptyset(&SigAction.sa_mask);
    for (tCIDLib::TCard4 c4Idx = 0; c4Idx < tCIDLib::c4ArrayElems(aSigMap); c4Idx++)
    {
        if (::sigaction(aSigMap[c4Idx].iHostSignal, &SigAction, 0))
        {
            TKrnlError::SetLastHostError(errno);
            const TKrnlError& kerrLast = TKrnlError::kerrLast();
            TKrnlPopup::Show(CID_FILE
                             , CID_LINE
                             , L"Platform Signal Initialization"
                             , 0
                             , 0
                             , kerrLast.errcId()
                             , kerrLast.errcHostId()
                             , L"Could not set signal handler");
        }
    }

    // Now deal with program errors
    // These don't use a chain because program errors almost always
    // should result in termination.
    ::memset(&SigAction, 0, sizeof(SigAction));
    SigAction.sa_handler = ProgramErrorHandler;
    SigAction.sa_flags = 0;

    // It's very important to block out all the signals from the handler so
    // that only one at a time gets in. This is because the log file could
    // become severely confused if two signals are in the handler at the same
    // time. We can't use a mutex to protect the log file because you can't
    // use any mutex functions inside a handler.
    ::sigemptyset(&SigAction.sa_mask);
    for (tCIDLib::TCard4 c4Idx = 0; c4Idx < tCIDLib::c4ArrayElems(aProgramErrors); c4Idx++)
    {
        ::sigaddset(&SigAction.sa_mask, aProgramErrors[c4Idx]);
    }
    // Now actually set the action for these guys
    for (tCIDLib::TCard4 c4Idx = 0; c4Idx < tCIDLib::c4ArrayElems(aProgramErrors); c4Idx++)
    {
        ::sigaction(aProgramErrors[c4Idx], &SigAction, 0);
    }
}


tCIDLib::TVoid TKrnlSignals::GenerateCtrlCSignal()
{
    ::raise(SIGINT);
}

