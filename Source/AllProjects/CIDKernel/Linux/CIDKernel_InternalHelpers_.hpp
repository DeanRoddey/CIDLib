//
// FILE NAME: CIDKernel_InternalHelpers_.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/29/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_Internal.Cpp module. This module
//  provides some internal Linux helper methods that are not part of any
//  externally provided services but are used in more than one module
//  internally. These methods are wrapped in the TKrnlLinux namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


namespace TKrnlLinux
{
    // -----------------------------------------------------------------------
    //  The helper functions
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean bInitTermExtProcess(const tCIDLib::EInitTerm eState);

    tCIDLib::TVoid LinuxFileTimeToCIDFileTime(const time_t LinuxTime
                                              , tCIDLib::TEncodedTime& enctToFill);

    tCIDLib::TVoid CIDFileTimeToLinuxFileTime(const tCIDLib::TEncodedTime enctCIDTime
                                              , time_t& LinuxTime);

    tCIDLib::TVoid StatBufToInfoFlags(const struct stat& StatBuf
                                     , tCIDLib::EFileInfoFlags& eFlags);

    tCIDLib::TSCh* pszFindInPath(const tCIDLib::TCh* const pszToFind);

    class TThreadTimer
    {
    public:
        TThreadTimer(tCIDLib::TCard4 c4MilliSecs);
        ~TThreadTimer();

        tCIDLib::TBoolean bBegin();
        tCIDLib::TBoolean bTriggered() const;
        tCIDLib::TVoid    Cancel();
        sigjmp_buf&       JumpEnvironment() const;
        tCIDLib::TVoid    JumpOnSignal(tCIDLib::TBoolean bAction);
        tCIDLib::TVoid    Reset();

        static tCIDLib::TBoolean bInitTerm(tCIDLib::EInitTerm eInitTerm);

    private:
        static tCIDLib::TVoid* __TimerFunc(tCIDLib::TVoid* pParam);
        static tCIDLib::TVoid  __HandleSignal(tCIDLib::TSInt);

        tCIDLib::TThreadId   __tidThis;
        tCIDLib::TCard4      __c4MilliSecs;
        struct sigaction     __SavedSigAction;
        sigset_t             __SavedSigSet;
    };

    class TRecursiveMutex
    {
    public:
        tCIDLib::TSInt  iDestroy()
        {
            return ::pthread_mutex_destroy(&__mtx);
        }

        tCIDLib::TSInt  iInitialize();
        tCIDLib::TSInt  iLock();
        tCIDLib::TSInt  iUnlock();

        const pthread_mutex_t* pPthreadMutex() const
        {
            return &__mtx;
        }

    private:
        pthread_mutex_t     __mtx;
        tCIDLib::TCard4     __c4Count;
        tCIDLib::TThreadId  __tidOwner;
    };

    class TKrnlThreadInfo
    {
    public:
        TKrnlThreadInfo(TKrnlThread*    pKrnlThread
                        , tCIDLib::TCh* pszName);
        ~TKrnlThreadInfo();

        tCIDLib::TCh*   pszName() const
        {
            return __pszName;
        }

        TKrnlThread*    pkthrThis() const
        {
            return __pkthrThis;
        }

        static TKrnlThreadInfo*     pkthriInstance();
        static tCIDLib::TBoolean    bInitTerm(tCIDLib::EInitTerm eInitTerm);

    private:
        static pthread_key_t    __keyThreadInfo;
        TKrnlThread*            __pkthrThis;
        tCIDLib::TCh*           __pszName;
    };

    class TTermFifo
    {
    public:
        TTermFifo(tCIDLib::TCard4 c4Size);
        ~TTermFifo();

        tCIDLib::TBoolean   bIsEmpty() const;
        tCIDLib::TBoolean   bPush();
        tCIDLib::TSCh       chPeek();
        tCIDLib::TSCh       chPull();
        tCIDLib::TVoid      Clear();
        tCIDLib::TVoid      ClearPeeked();
        tCIDLib::TVoid      ResetPeek();

    private:
        tCIDLib::TSCh*  __pchFifo;
        tCIDLib::TCard4 __c4Size;
        tCIDLib::TCard4 __c4Head;
        tCIDLib::TCard4 __c4Tail;
        tCIDLib::TCard4 __c4Peek;
    };

    class TTermValueNode
    {
    public:
        TTermValueNode();
        ~TTermValueNode();

        TTermValueNode*     pnodeChild;
        TTermValueNode*     pnodeSibling;
        tCIDLib::TCard2     c2Code;
        tCIDLib::TSCh       chThis;
    };
}
