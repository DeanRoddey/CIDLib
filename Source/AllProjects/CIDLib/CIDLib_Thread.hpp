//
// FILE NAME: CIDLib_Thread.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/16/1993
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
//  This file provides the thread control class, TThread. Threads are named
//  for easy identification in log file messages and such.
//
//  Also implemented here is a simple thread priority janitor class,
//  TThreadPrioJan, which makes it safe to apply new priorities to threads in an
//  exception based system. Not often used but available.
//
//  We define a TThreadFunc class which represents a thread callback function that
//  maps to something. Sometimes we want to map it to a global function. Sometimes
//  to a method of a class. So we need some way to hold the target info and dispatch
//  the thread to that target once it comes out on the other side of being started
//  up and is running in its new thread context.
//
//  We have two derivatives of it that handle the two scenarios mentioned above. The
//  latter has to be templatized since it has to create a pointer to a member of
//  the target class.
//
//  Note that threads can also be run completely standalone. You can derive from
//  TThread and override eProcess (and also Init() and Terminate() if want.) So it
//  depends on what kind of thread you want. For things like a thread far, having them
//  start on a method of a containing class is very convenient since they all
//  automatically get a shared data context.
//
// GOTCHAS/CAVEATS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TThread;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TThreadFunc
//  PREFIX: tfunc
// ---------------------------------------------------------------------------
class CIDLIBEXP TThreadFunc : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        ~TThreadFunc();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bValidMethod() const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eRun
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid* const   pData
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::EExitCodes eRunImpl
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid* const   pData
        )   = 0;


        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TThreadFunc();
        TThreadFunc(const TThreadFunc&);
        TThreadFunc& operator=(const TThreadFunc&);


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TThreadFunc,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TGlobalThreadFunc
//  PREFIX: tfunc
// ---------------------------------------------------------------------------
class CIDLIBEXP TGlobalThreadFunc : public TThreadFunc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGlobalThreadFunc();

        TGlobalThreadFunc
        (
                    tCIDLib::TThreadFuncPtr pfnThreadFunc
        );

        TGlobalThreadFunc(const TGlobalThreadFunc&);

        ~TGlobalThreadFunc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGlobalThreadFunc& operator=(const TGlobalThreadFunc&);


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValidMethod() const override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eRunImpl
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid* const   pData
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGlobalThreadFunc,TThreadFunc)
        DefPolyDup(TGlobalThreadFunc)


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pfnThreadFunc
        //      The global thread function that we run. We use the same
        //      function type alias that TThread itself does in this case.
        // -------------------------------------------------------------------
        tCIDLib::TThreadFuncPtr     m_pfnThreadFunc;
};



// ---------------------------------------------------------------------------
//   CLASS: TMemberFunc
//  PREFIX: tfunc
// ---------------------------------------------------------------------------
template <typename T> class TMemberFunc : public TThreadFunc
{
    public  :
        // -------------------------------------------------------------------
        //  Class specific types
        // -------------------------------------------------------------------
        using TMemFunc = tCIDLib::EExitCodes (T::*)(TThread&, tCIDLib::TVoid*);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMemberFunc() :

            m_pfnMemberFunc(nullptr)
            , m_pobjTarget(nullptr)
        {
        }

        TMemberFunc(T* const pobjTarget, TMemFunc pfnMemberFunc) :

            m_pfnMemberFunc(pfnMemberFunc)
            , m_pobjTarget(pobjTarget)
        {
        }

        TMemberFunc(const TMemberFunc<T>& tfuncSrc) :

            m_pfnMemberFunc(tfuncSrc.m_pfnMemberFunc)
            , m_pobjTarget(tfuncSrc.m_pobjTarget)
        {
        }

        ~TMemberFunc()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMemberFunc& operator=(const TMemberFunc<T>& tfuncSrc)
        {
            if (this != &tfuncSrc)
            {
                m_pfnMemberFunc = tfuncSrc.m_pfnMemberFunc;
                m_pobjTarget = tfuncSrc.m_pobjTarget;
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValidMethod() const override
        {
            return (m_pfnMemberFunc != nullptr);
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eRunImpl(TThread& thrThis, tCIDLib::TVoid* const pData) override
        {
            return (m_pobjTarget->*m_pfnMemberFunc)(thrThis, pData);
        }


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TMemberFunc<T>,TThreadFunc)
        DefPolyDup(TMemberFunc<T>)


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pfnMemberFunc
        //      The class member method that we run.
        //
        //  m_pobjTarget
        //      The target object whose method we are to run.
        // -------------------------------------------------------------------
        TMemFunc    m_pfnMemberFunc;
        T*          m_pobjTarget;
};



// ---------------------------------------------------------------------------
//   CLASS: TThread
//  PREFIX: thr
// ---------------------------------------------------------------------------
class CIDLIBEXP TThread : public TObject, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static TThread* pthrCaller();

        static tCIDLib::TVoid Sleep
        (
            const   tCIDLib::TCard4         c4MilliSecs
        );

        static tCIDLib::TThreadId tidCaller();



        // -------------------------------------------------------------------
        //  Contructors and Destructor
        // -------------------------------------------------------------------
        TThread
        (
            const   TString&                strName
            , const tCIDLib::TThreadFuncPtr pfnToRun
            , const tCIDLib::TCard4         c4StackSz = 0
            , const tCIDLib::TBoolean       bSelfPrio = kCIDLib::False
        );

        TThread
        (
            const   TString&                strName
            , const tCIDLib::TCard4         c4StackSz = 0
            , const tCIDLib::TBoolean       bSelfPrio = kCIDLib::False
        );

        TThread
        (
            const   TString&                strName
            , const TThreadFunc&            tfuncToRun
            , const tCIDLib::TCard4         c4StackSz = 0
            , const tCIDLib::TBoolean       bSelfPrio = kCIDLib::False
        );

        TThread(const TThread&) = delete;

        TThread
        (
                    TThread&&               thrSrc
        );

        ~TThread();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TThread& operator=(const TThread&) = delete;

        TThread& operator=
        (
                    TThread&&               thrSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AckShutdownRequest();

        tCIDLib::TBoolean bCheckShutdownRequest();

        tCIDLib::TBoolean bIsGUIThread() const;

        tCIDLib::TBoolean bIsRunning() const;

        tCIDLib::TBoolean bSleep
        (
            const   tCIDLib::TCard4         c4Timeout
        );

        tCIDLib::TBoolean bWaitForDeath
        (
                    tCIDLib::EExitCodes&    eToFill
            , const tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TVoid BlockMyself();

        tCIDLib::EExitCodes eTermCode() const;

        tCIDLib::EPrioLevels ePriority() const;

        tCIDLib::EExitCodes eWaitForDeath
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        )   const;

        const TKrnlThread& kthrThis() const;

        TKrnlThread& kthrThis();

        tCIDLib::TVoid MarkAsGUIThread();

        tCIDLib::TExitFuncPtr pfnSetOnExit
        (
            const   tCIDLib::TExitFuncPtr   pfnNew
        );

        tCIDLib::TVoid Release();

        tCIDLib::TVoid ReqShutdownSync
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        );

        tCIDLib::TVoid ReqShutdownNoSync();

        tCIDLib::TVoid SetPriority
        (
            const   tCIDLib::EPrioLevels    ePrioLev
        );

        tCIDLib::TVoid Start
        (
                    tCIDLib::TVoid* const   pData = nullptr
            , const tCIDLib::EThreadFlags   eFlags = tCIDLib::EThreadFlags::None
        );

        const TString& strName() const;

        tCIDLib::TVoid Sync();

        tCIDLib::TThreadId tidThis() const;

        tCIDLib::TVoid Unblock();

        tCIDLib::TVoid WaitSync
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        );


    protected   :
        // -------------------------------------------------------------------
        //  Declare friends
        //
        //  This class has many more that usual because of its very primal nature.
        // -------------------------------------------------------------------
        friend tCIDLib::TVoid AddThread
        (
                    TThread* const          pthrNew
        );

        friend tCIDLib::TVoid AddThreadAt
        (
                    TThread* const          pthrNew
            , const tCIDLib::TCard4         c4Index
        );

        friend tCIDLib::EExitCodes eThreadStart
        (
                    TThread* const          pthrToStart
            ,       tCIDLib::TVoid* const   pUserData
        );

        friend TThread* pthrCurrent();


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TThread();


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Init();

        virtual tCIDLib::TVoid Terminate();

        virtual tCIDLib::EExitCodes eProcess();


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const final;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckCallerIsSelf
        (
            const   tCIDLib::TCard4         c4Line
            , const tCIDLib::TCh* const     pszAuxText = nullptr
        )   const;

        tCIDLib::TVoid ShutdownProcessing();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CommonInit();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSelfPrio
        //      This flag (when set) prevents any other thread from setting
        //      this thread's priority.
        //
        //  m_bShutdownRequest
        //      This is set to indicate that another is making a request to
        //      us to shutdown. The usual sync actions also take place, but
        //      this flag is set for use by the bCheckShutdownRequest()
        //      method.
        //
        //  m_bSyncRequest
        //      This is set to indicate that another thread has called
        //      WaitSync(). It is also used to make sure that two threads
        //      don't try to sync at the same time. We store the tid of the
        //      syncing thread to be able to insure that only he releases
        //      the sync (in m_tidSyncRequester.)
        //
        //  m_c4ListInd
        //      This is our object's index into the internal list of threads.
        //      This is an optimization. It is set during construction and
        //      left alone.
        //
        //  m_c4StackSz
        //      The size of the stack to give to the thread. Passed in by the
        //      creator.
        //
        //  m_kevSync
        //      This event semaphore is used by the Sync()/WaitSync() methods
        //      to allow the thread to sync with another thread. It is also
        //      used during thread startup to sync.
        //
        //  m_kevResponse
        //      This event semaphore is used by the Sync()/WaitSync() methods
        //      to allow the thread to sync with another thread.
        //
        //  m_kthrThis
        //      The kernel thread object for this thread. This gives us the
        //      basic control and info that we need for threads. Its set to
        //      0 when this thread is not running.
        //
        //  m_pfnOnExit
        //      This is the address of an optional on-exit function that
        //      may be provided by the client code. It will be called when
        //      the thread is shutting down. If the client does not set the
        //      function, it is 0.
        //
        //  m_tfuncToRun
        //      This is the thread function object that holds the thing
        //      (global function, method, etc...) that provides the code path
        //      for this thread. It is called from the .Start() method. If
        //      this guy's function pointer is zero, then its assumed that
        //      the user has overridden the eProcess() method.
        //
        //  m_tidSyncReq
        //      When a sync request is made, we store the id of the thread
        //      that is syncing. This allows us to make sure that only that
        //      thread releases the sync.
        //
        //  m_szName
        //      The name of the thread. This was passed in by the caller in
        //      the constructor.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bSelfPrio;
        tCIDLib::TBoolean       m_bShutdownRequest;
        tCIDLib::TBoolean       m_bSyncRequest;
        tCIDLib::TCard4         m_c4ListInd;
        tCIDLib::TCard4         m_c4StackSz;
        TKrnlEvent              m_kevSync;
        TKrnlEvent              m_kevResponse;
        TKrnlThread             m_kthrThis;
        tCIDLib::TExitFuncPtr   m_pfnOnExit;
        TThreadFunc*            m_ptfuncToRun;
        tCIDLib::TThreadId      m_tidSyncReq;
        TString                 m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TThread,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TThreadPrioJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TThreadPrioJan : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TThreadPrioJan() = delete;

        TThreadPrioJan
        (
            const   tCIDLib::EPrioLevels    eLevel
        );

        TThreadPrioJan(const TThreadPrioJan&) = delete;

        ~TThreadPrioJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TThreadPrioJan& operator=(const TThreadPrioJan&) = delete;
        tCIDLib::TVoid* operator new(size_t) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eLevel
        //      This is used to store the priority of the thread when the
        //      janitor is created.
        //
        //  m_pthrTarget
        //      This is the thread we were created under. It is used to make
        //      sure we get back to the original thread and not some other
        //      one.
        // -------------------------------------------------------------------
        tCIDLib::EPrioLevels    m_eLevel;
        TThread*                m_pthrTarget;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TThreadPrioJan,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TThreadSyncJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TThreadSyncJan : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TThreadSyncJan() = delete;

        TThreadSyncJan
        (
                    TThread* const          pthrTarget
            , const tCIDLib::TCard4         c4Millis
        );

        TThreadSyncJan(const TThreadSyncJan&) = delete;

        ~TThreadSyncJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TThreadSyncJan& operator=(const TThreadSyncJan&) = delete;
        tCIDLib::TVoid* operator new(size_t) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pthrTarget
        //      This is the target thread to sync with, and to unsync with
        //      on the way out.
        // -------------------------------------------------------------------
        TThread*    m_pthrTarget;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TThreadSyncJan,TObject)
};

#pragma CIDLIB_POPPACK


