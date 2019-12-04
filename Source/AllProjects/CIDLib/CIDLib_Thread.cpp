//
// FILE NAME: CIDLib_Thread.cpp
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
//  This file implements the TThread class, which provides process
//  control for all CIDLib based applications. It maintains a list of all
//  currently existing thread objects. It also provides the real thread
//  startup function which just calls the client's function. This way we
//  can install exception handlers for unhandled exceptions.
//
//  This is a very complex file, which is just a side effect of the complexity
//  of threading, of the problems with bootstrapping the system without
//  getting into a recursive freakout if something goes wrong during primal
//  init, of the interelationship between error handling and threading, ect...
//
// CAVEATS/GOTCHAS:
//
//  1)  The local thread list manipulation functions assume that the
//      publically visible methods calling them have locked the access
//      critical section if that is required.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//  Forward prototypes
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
//  Do our class' RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TThread,TObject)
RTTIDecls(TThreadFunc,TObject)
RTTIDecls(TGlobalThreadFunc,TThreadFunc)
RTTIDecls(TThreadPrioJan,TObject)
RTTIDecls(TThreadSyncJan,TObject)



namespace CIDLib_Thread
{
    namespace
    {
        // ---------------------------------------------------------------------------
        //  An array of these is use to keep track of all of the running threads.
        //  It is a small associative array so the .tidThread field is used to
        //  find the right entry, and the other field is the thread object. This
        //  prevents having to dereference each object to get to the id during the
        //  search. If pthrThis is zero, then that entry is not used. Otherwise,
        //  the id is kCIDLib::tidInvalid if the thread is inactive, and some
        //  other number if it is active.
        //
        //  The thread list is faulted in lazily because we have no way to force
        //  it to be already allocated and initialized. Making it a global array
        //  would set the tidThread to zero, which might not be the right value
        //  for kCIDLib::tidInvalid on some platforms.
        // ---------------------------------------------------------------------------
        struct  TThreadItem
        {
            TThread*                pthrThis;
            tCIDLib::TThreadId      tidThread;
        };


        // ---------------------------------------------------------------------------
        //  A mtex to protect access to the thread list
        // ---------------------------------------------------------------------------
        static TMutex* pmtxListAccess()
        {
            static TMutex mtxListAccess;
            return &mtxListAccess;
        }


        // ---------------------------------------------------------------------------
        //  Constant data for local use
        //
        //  c4MaxThreads
        //      The maximum number of threads supported per-process.
        // ---------------------------------------------------------------------------
        const tCIDLib::TCard4    c4MaxThreads   = 1024;


        // ---------------------------------------------------------------------------
        //  Variables for local use
        //
        //  c4ThreadCount
        //      The count of thread objects currently defined, i.e. the number of
        //      active entries in the thread list. They are not all necessarily
        //      running, just created and added to the list.
        //
        //  pthrPrimary
        //      This is set to the first thread started by the user's Exe. When
        //      this thread is seen exiting (even normally), then the program is
        //      ended. The magic start up code (see CIDLib.Hpp) macros will call into
        //      here to set this to the primary thread object.
        // ---------------------------------------------------------------------------
        tCIDLib::TCard4      c4ThreadCount;
        TThread*             pthrPrimary;


        // -----------------------------------------------------------------------
        //  Some stats cache items we maintain
        // -----------------------------------------------------------------------
        TStatsCacheItem     m_sciThreadCnt;
    }
}



// ---------------------------------------------------------------------------
//  Functions for local use only
// ---------------------------------------------------------------------------

//
//  This method handles the lazy initialization of the thread list. It returns
//  a pointer to the thread list array. This is unfortunate, but there is no
//  way to get it intialized early enough to let it be global.
//
//  WE also use it to fault in our stats cache item for the number of threads,
//  since it could never change until the thread list is created anyway.
//
static CIDLib_Thread::TThreadItem* athriList()
{
    static CIDLib_Thread::TThreadItem   athriList[CIDLib_Thread::c4MaxThreads];
    static TAtomicFlag                  atomDone;

    if (!atomDone)
    {
        // Lock the list first
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        // Always check again to make sure we weren't beat to the punch
        if (!atomDone)
        {
            // Zero the pointer and invalidate the thread id for each entry
            for (tCIDLib::TCard4 c4Ind = 0; c4Ind < CIDLib_Thread::c4MaxThreads; c4Ind++)
            {
                athriList[c4Ind].pthrThis = nullptr;
                athriList[c4Ind].tidThread = kCIDLib::tidInvalid;
            }
        }

        // Initialize our stats cache item
        TStatsCache::RegisterItem
        (
            kCIDLib::pszStat_Core_ThreadCount
            , tCIDLib::EStatItemTypes::Counter
            , CIDLib_Thread::m_sciThreadCnt
        );

        atomDone.Set();
    }
    return athriList;
}


//
//  This method allows a thread to be added at a particular slot and with
//  an existing thread id. The index must be available. AddThread(), below,
//  selects an empty slot and then calls this version (which is ok since
//  these local methods assume the thread list is already locked.)
//
tCIDLib::TVoid AddThreadAt(TThread* const pthrNew, const tCIDLib::TCard4 c4Index)
{
    CIDAssert(c4Index < CIDLib_Thread::c4MaxThreads, L"Invalid thread index");

    // Get a pointer to the indicated index.
    CIDLib_Thread::TThreadItem* pthriCur = &athriList()[c4Index];

    // If debugging, make sure its not already in use
    #if CID_DEBUG_ON
    if (pthriCur->tidThread != kCIDLib::tidInvalid)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_CannotAddAt
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Index)
        );
    }
    #endif

    // Store the thread pointer, set the tid field, and bump up the count
    pthriCur->pthrThis  = pthrNew;
    pthriCur->tidThread = pthrNew->kthrThis().tidThis();
    CIDLib_Thread::c4ThreadCount++;

    // Bump the stats counter
    TStatsCache::c8IncCounter(CIDLib_Thread::m_sciThreadCnt);

    //
    //  Store the thread list index in the thread object so that we have a
    //  link the other direction. This avoids the need to search the list
    //  to find a thread's list entry.
    //
    pthriCur->pthrThis->m_c4ListInd = c4Index;
}


//
//  This method is called when a new thread is to be added to the list, and
//  the exact index is not important. It will find the first available slot
//  in the thread list and add the thread there. It will abort if the thread
//  count exceeds the maximum.
//
tCIDLib::TVoid AddThread(TThread* const pthrNew)
{
    // Make sure we've not filled the thread list
    if (CIDLib_Thread::c4ThreadCount == CIDLib_Thread::c4MaxThreads)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_TooManyThreads
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(CIDLib_Thread::c4MaxThreads)
        );
    }

    //
    //  Scan for an empty entry in the list. If we don't find one, then
    //  there is a major screw up because the thread count is wrong. Also,
    //  count how many are taken while we look. If it exceeds
    //  CIDLib_Thread::c4ThreadCount, then that is an error too.
    //
    tCIDLib::TCard4  c4Ind, c4Count = 0;
    for (c4Ind = 0; c4Ind < CIDLib_Thread::c4MaxThreads; c4Ind++)
    {
        if (!athriList()[c4Ind].pthrThis)
            break;
        c4Count++;
    }

    // If debugging, report any inconsistencies
    #if CID_DEBUG_ON
    if ((c4Count > CIDLib_Thread::c4ThreadCount)
    ||  (c4Ind == CIDLib_Thread::c4MaxThreads))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_ThreadCount
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(CIDLib_Thread::c4ThreadCount)
            , TCardinal(c4Count)
        );
    }
    #endif

    //
    //  And add it at the free slot. Note that this is ok, because we assume
    //  the thread list is locked. So no one is going to steal this slot
    //  before we get this stored.
    //
    AddThreadAt(pthrNew, c4Ind);
}


//
//  These methods will find the thread in the list with the passed id or
//  name and then reset the list entry and bump down the thread count.
//
static tCIDLib::TBoolean bRemoveName(const TString& strName)
{
    // If debugging, check for count underflow. This would be way bad
    if (!CIDLib_Thread::c4ThreadCount)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_ThreadCount_0
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }

    // Get the list pointer and search it for this id
    CIDLib_Thread::TThreadItem* pthriCur = athriList();
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < CIDLib_Thread::c4MaxThreads; c4Ind++)
    {
        if (pthriCur->pthrThis)
        {
            if (pthriCur->pthrThis->strName() == strName)
            {
                // We found it so dec the count and clear the entry
                CIDLib_Thread::c4ThreadCount--;
                pthriCur->pthrThis = 0;
                pthriCur->tidThread = kCIDLib::tidInvalid;

                // Decrement the stats counter
                TStatsCache::c8DecCounter(CIDLib_Thread::m_sciThreadCnt);

                return kCIDLib::True;
            }
        }
        pthriCur++;
    }

    // Didn't find it, oh well...
    return kCIDLib::False;
}


//
//  This method will find the thread in the list with the passed id. It will
//  return the TThread object if found, else a null pointer.
//
static TThread* pthrFindId(const tCIDLib::TThreadId tidThread)
{
    // Make sure incoming id is not kCIDLib::tidInvalid
    if (tidThread == kCIDLib::tidInvalid)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_InvalidTID
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    // Get a pointer to the list and search it
    CIDLib_Thread::TThreadItem* pthriCur = athriList();
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < CIDLib_Thread::c4MaxThreads; c4Ind++)
    {
        if (pthriCur->tidThread == tidThread)
            return pthriCur->pthrThis;

        pthriCur++;
    }
    return nullptr;
}


//
//  This method will find the named thread in the list. It will return the
//  TThread object pointer or a null pointer if not found.
//
static TThread* pthrFindName(const TString& strName)
{
    CIDLib_Thread::TThreadItem* pthriCur = athriList();
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < CIDLib_Thread::c4MaxThreads; c4Ind++)
    {
        if (pthriCur->pthrThis)
        {
            if (pthriCur->pthrThis->strName() == strName)
                return pthriCur->pthrThis;
        }
        pthriCur++;
    }
    return 0;
}


//
//  This is the internal thread startup callback function. When the kernel
//  thread object is asked to start the thread, we pass it this function.
//  This is not actually started first though. The platform driver will start
//  an internal method which will call us back on this function. So, at this
//  point, we are now running with the context of the new thread.
//
tCIDLib::EExitCodes eThreadStart(TThread* const pthrStarting, tCIDLib::TVoid* pData)
{
    // It won't actually return false, it'll throw, but this is for code analysis purposes
    if (!bCIDPreCond(pthrStarting != nullptr))
        return tCIDLib::EExitCodes::InitFailed;

    //
    //  Add a new node to the thread list. We can't add us to the list until
    //  we are running, since our thread id isn't set until then. We don't
    //  get removed until the thread object actually destructs though, since
    //  we have to still be there until the actual thread code exits, and
    //  the code that is managing that thread cleans us up.
    //
    //  We may already be in the list, if this thread object is being
    //  started up again, so we have to check that first. We check by pointer
    //  since it would have to be the same object literally.
    //
    {
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        // See if this thread object is already in the list
        CIDLib_Thread::TThreadItem* pthriCur = athriList();
        TThread* pthrOld = nullptr;
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < CIDLib_Thread::c4MaxThreads; c4Ind++)
        {
            if (pthriCur->pthrThis == pthrStarting)
            {
                pthrOld = pthrStarting;
                break;
            }
            pthriCur++;
        }

        // If not, then add it
        if (!pthrOld)
            AddThread(pthrStarting);

        //
        //  Set our copy of the thread's tid. This is redundant in a way, since
        //  its already in our kernel thread object. But having a separate version
        //  in the list directly allows us to search the list by id without having
        //  to dereference the thread objects to do it.
        //
        athriList()[pthrStarting->m_c4ListInd].tidThread = TKrnlThread::tidCaller();
    }

    //
    //  Install a C++ exception handler around the user's thread. We catch
    //  any exceptions coming out of here and clean up.
    //
    tCIDLib::EExitCodes eRet = tCIDLib::EExitCodes::Normal;
    try
    {
        //
        //  Call the init method of the starting thread. This must be done
        //  in the context of the thread itself, so we do it here before
        //  calling the user's init method.
        //
        pthrStarting->Init();

        try
        {
            //
            //  If we have a thread function object, then invoke it. Otherwise,
            //  assume they have overridden eProcess() and invoke that. In the
            //  second case we don't have to pass anything because its implicit
            //  with the thread object.
            //
            if (pthrStarting->m_ptfuncToRun)
                eRet = pthrStarting->m_ptfuncToRun->eRun(*pthrStarting, pData);
             else
                eRet = pthrStarting->eProcess();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);

            // Do the shutdown stuff to clean up, then rethrow
            pthrStarting->ShutdownProcessing();
            throw;
        }

        // Do the shutdown process stuff
        pthrStarting->ShutdownProcessing();
    }

    catch (TError& errToCatch)
    {
        // Set the exit code to indicate a runtime error
        eRet = tCIDLib::EExitCodes::RuntimeError;

        //
        //  Try to log this message. Make sure to use status class so it always gets
        //  to the log file. Catch any exceptions out of here. We also log an
        //  unhandled exception error as well.
        //
        try
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , TString(kCIDLib_::pszUnhandledRTE)
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
                , pthrStarting->strName()
            );
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
            // Not much we can do but eat this
        }
    }

    catch(...)
    {
        // Set the exit code to indicate a runtime error
        eRet = tCIDLib::EExitCodes::RuntimeError;

        try
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , TString(kCIDLib_::pszUnhandledSE)
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , pthrStarting->strName()
            );
        }

        catch(...)
        {
            // Not much we can do but eat this
        }
    }

    //
    //  If this was the primary thread, then its the main thread that was
    //  first started by the user and we want to die now. So exit the
    //  process using the return code we got back.
    //
    if (pthrStarting == CIDLib_Thread::pthrPrimary)
    {
        CIDLib_Thread::pthrPrimary = nullptr;
        TProcess::ExitProcess(eRet);
    }

    // Return the thread's return value
    return eRet;
}



// ---------------------------------------------------------------------------
//  Intrafacility methods
// ---------------------------------------------------------------------------

//
//  This function will get the thread id of the current thread, lock the
//  thread list, look up the thread, and return a pointer to the thread
//  object.
//
//  NOTE:   If the thread is not found, then this is a major error, so we log
//          an error and abort. Note also that we cannot log from this call
//          because we will get into a big circle jerk with the logging
//          mechanism and overflow the stack.
//
//  NOTE:   This function faults in a thread object for the main thread, since
//          it means that something went wrong during fundamental init and we
//          are dying. So faulting in a dummy primary thread object will make
//          the termination go cleanly.
//
TThread* pthrCurrent()
{
    TThread*            pthrRet = nullptr;
    tCIDLib::TThreadId  tidCaller = TKrnlThread::tidCaller();

    // Dummy scope so we don't stay locked if popup occurs below!
    {
        // Get access to the thread list
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        // Look for this thread id
        pthrRet = pthrFindId(tidCaller);
    }

    //
    //  If we did get a nul node back, then a thread with that id does not
    //  exist. This is a major error, so we need to abort.
    //
    if (!pthrRet)
    {
        static tCIDLib::TBoolean bMainThreadAdded = kCIDLib::False;

        //
        //  If the main thread has not been added, then an error occured
        //  very early and we are basically dying. So fault in a thread for
        //  the main thread so that termination can complete cleanly.
        //
        if (!bMainThreadAdded && (tidCaller == TKrnlThread::tidPrimary()))
        {
            //
            //  Note that the def ctor is hidden and only friends can call
            //  it. It will adopt the calling thread, which is what we
            //  want to happen here.
            //
            TThread* pthrMain = new TThread();
            AddThread(pthrMain);
            bMainThreadAdded = kCIDLib::True;
            return pthrMain;
        }

        //
        //  Do an error popup since this a very bad error and we cannot log
        //  from this method, because that code will call back into here and
        //  we will recurse until the stack overflows.
        //
        #if CID_DEBUG_ON
        facCIDLib().MsgPopUp
        (
            facCIDLib().strMsg
            (
                kCIDErrs::errcPrc_ThreadIdNotFound
                , TCardinal(tidCaller)
            )
            , CID_FILE
            , CID_LINE
        );
        #endif

        // And get outa here
        TProcess::ExitProcess(tCIDLib::EExitCodes::FatalError);
    }

    // We did find the node, so return the thread object in the node
    return pthrRet;
}


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------

//
//  This is called by the magic startup code that is generated into the
//  user's Exe. It stores the address of the primary thread, which is the
//  first user thread started. When that thread ends, the program is
//  terminated.
//
tCIDLib::TVoid CIDLib_MakePrimary(TThread* const pthrPrimary)
{
    if (CIDLib_Thread::pthrPrimary)
    {
        facCIDLib().ErrorPopUp
        (
            kCIDMsgs::midThrd_MultipleCalls
            , CID_FILE
            , CID_LINE
        );
    }
    CIDLib_Thread::pthrPrimary = pthrPrimary;
}



// ---------------------------------------------------------------------------
//  TThreadFunc: Destructor
// ---------------------------------------------------------------------------
TThreadFunc::~TThreadFunc()
{
}


// ---------------------------------------------------------------------------
//  TThreadFunc: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  For now, this is nothing but a passthrough to the virtual implementation method.
//  But it gives us future flexibility to catch thread function type threads starting
//  up.
//
tCIDLib::EExitCodes TThreadFunc::eRun(TThread& thrThis, tCIDLib::TVoid* const pData)
{
    return eRunImpl(thrThis, pData);
}


// ---------------------------------------------------------------------------
//  TThreadFunc: Hidden constructors and operators
// ---------------------------------------------------------------------------
TThreadFunc::TThreadFunc()
{
}

TThreadFunc::TThreadFunc(const TThreadFunc&)
{
}

TThreadFunc& TThreadFunc::operator=(const TThreadFunc&)
{
    return *this;
}


// ---------------------------------------------------------------------------
//  TGlobalThreadFunc: Constructors and destructor
// ---------------------------------------------------------------------------
TGlobalThreadFunc::TGlobalThreadFunc() :

    m_pfnThreadFunc(nullptr)
{
}

TGlobalThreadFunc::TGlobalThreadFunc(const tCIDLib::TThreadFuncPtr pfnThreadFunc) :

    m_pfnThreadFunc(pfnThreadFunc)
{
}

TGlobalThreadFunc::TGlobalThreadFunc(const TGlobalThreadFunc& tfuncToCopy) :

    m_pfnThreadFunc(tfuncToCopy.m_pfnThreadFunc)
{
}

TGlobalThreadFunc::~TGlobalThreadFunc()
{
}

// ---------------------------------------------------------------------------
//  TGlobalThreadFunc: Public operators
// ---------------------------------------------------------------------------
TGlobalThreadFunc&
TGlobalThreadFunc::operator=(const TGlobalThreadFunc& tfuncToAssign)
{
    if (this != &tfuncToAssign)
    {
        m_pfnThreadFunc = tfuncToAssign.m_pfnThreadFunc;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TGlobalThreadFunc: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGlobalThreadFunc::bValidMethod() const
{
    return (m_pfnThreadFunc != nullptr);
}


// ---------------------------------------------------------------------------
//  TGlobalThreadFunc: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes
TGlobalThreadFunc::eRunImpl(TThread& thrThis, tCIDLib::TVoid* const pData)
{
    return m_pfnThreadFunc(thrThis, pData);
}



// ---------------------------------------------------------------------------
//   CLASS: TThreadPrioJan
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TThread: Constructors and Destructor
// ---------------------------------------------------------------------------

// We always work on the thread we are created in the context of
TThreadPrioJan::TThreadPrioJan(const tCIDLib::EPrioLevels eLevel) :

    m_eLevel(tCIDLib::EPrioLevels::Count)
    , m_pthrTarget(nullptr)
{
    //
    //  Get the priority of this thread now. The class and level were set to
    //  invalid values above so that, if this fails, they will cause an error
    //  when restored to the thread.
    //
    m_pthrTarget = pthrCurrent();
    m_eLevel = m_pthrTarget->ePriority();

    // Set the new priority
    m_pthrTarget->SetPriority(eLevel);
}

TThreadPrioJan::~TThreadPrioJan()
{
    // If it got changed, then set the old one back
    if (m_pthrTarget && (m_eLevel != tCIDLib::EPrioLevels::Count))
        m_pthrTarget->SetPriority(m_eLevel);
}


// ---------------------------------------------------------------------------
//   CLASS: TThreadSyncJan
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TThreadSyncJan: Constructors and destructor
// ---------------------------------------------------------------------------
TThreadSyncJan::TThreadSyncJan(         TThread* const  pthrTarget
                                , const tCIDLib::TCard4 c4Millis) :

    m_pthrTarget(pthrTarget)
{
    // Do the sync request
    pthrTarget->WaitSync(c4Millis);
}

TThreadSyncJan::~TThreadSyncJan()
{
    // And release the thread
    m_pthrTarget->Release();
}



// ---------------------------------------------------------------------------
//   CLASS: TThread
//  PREFIX: thr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TThread: Constructors and Destructor
// ---------------------------------------------------------------------------
TThread::TThread(   const   TString&            strName
                    , const TThreadFunc&        tfuncToRun
                    , const tCIDLib::TCard4     c4StackSz
                    , const tCIDLib::TBoolean   bSelfPrio) :

    m_bSelfPrio(bSelfPrio)
    , m_bShutdownRequest(kCIDLib::False)
    , m_bSyncRequest(kCIDLib::False)
    , m_c4StackSz(c4StackSz)
    , m_kthrThis()
    , m_ptfuncToRun(nullptr)
    , m_pfnOnExit(nullptr)
    , m_strName(strName)
    , m_tidSyncReq(kCIDLib::tidInvalid)
{
    //
    //  Duplicate the passed thread function object and store it away. We
    //  use polymorphic duplication here, since it could be any derivative
    //  of the base class.
    //
    m_ptfuncToRun = ::pDupObject<TThreadFunc>(tfuncToRun);

    //
    //  Call the common initialization method. Do this last since it sync
    //  and force data sync on MP systems.
    //
    CommonInit();
}

TThread::TThread(   const   TString&                strName
                    , const tCIDLib::TThreadFuncPtr pfnToRun
                    , const tCIDLib::TCard4         c4StackSz
                    , const tCIDLib::TBoolean       bSelfPrio) :

    m_bSelfPrio(bSelfPrio)
    , m_bShutdownRequest(kCIDLib::False)
    , m_bSyncRequest(kCIDLib::False)
    , m_c4StackSz(c4StackSz)
    , m_kthrThis()
    , m_ptfuncToRun(nullptr)
    , m_pfnOnExit(nullptr)
    , m_strName(strName)
    , m_tidSyncReq(kCIDLib::tidInvalid)
{
    // Create a thread function object for the global function
    m_ptfuncToRun = new TGlobalThreadFunc(pfnToRun);

    //
    //  Call the common initialization method. Do this last since it sync
    //  and force data sync on MP systems.
    //
    CommonInit();
}

TThread::TThread(   const   TString&            strName
                    , const tCIDLib::TCard4     c4StackSz
                    , const tCIDLib::TBoolean   bSelfPrio) :

    m_bSelfPrio(bSelfPrio)
    , m_bShutdownRequest(kCIDLib::False)
    , m_bSyncRequest(kCIDLib::False)
    , m_c4StackSz(c4StackSz)
    , m_kthrThis()
    , m_ptfuncToRun(nullptr)
    , m_pfnOnExit(nullptr)
    , m_strName(strName)
    , m_tidSyncReq(kCIDLib::tidInvalid)
{
    //
    //  Call the common initialization method. Do this last since it sync
    //  and force data sync on MP systems.
    //
    CommonInit();
}

TThread::~TThread()
{
    //
    //  We cannot do this while the thread is running. So if it looks to
    //  be running, let's at least try to get it to shut down ourself.
    //
    if (bIsRunning())
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcPrc_StillRunning
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotReady
                , m_strName
            );
        }

        // Set the flag and wait for it to exit
        m_bShutdownRequest = kCIDLib::True;
        tCIDLib::TBoolean   bState;
        tCIDLib::EExitCodes eRes;
        if (!m_kthrThis.bWaitForDeath(bState, eRes, 5000))
        {
            // This is really bad, so proc fatal error to restart us
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcPrc_CantStop
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , m_strName
            );
        }
    }

    // Clean up our event objects
    if (!m_kevSync.bClose())
    {
        facCIDLib().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TString(L"m_kevSync")
        );
    }

    if (!m_kevResponse.bClose())
    {
        facCIDLib().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TString(L"m_kevResponse")
        );
    }

    //
    //  Remove this thread from the list. Do this last cause if an error
    //  occurs above, the error code will try to look up this thread in the
    //  list. Use a faux scope block to minimize list lock time.
    //
    //  It may not be there. If the thread was never started, it would not
    //  have been added to the list. So we don't consider it an error not
    //  to find it.
    //
    {
        // Get access to the thread list
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());
        bRemoveName(m_strName);
    }

    // Clean up the thread function object
    delete m_ptfuncToRun;
    m_ptfuncToRun = nullptr;
}


// ---------------------------------------------------------------------------
//  TThread: Public, static methods
// ---------------------------------------------------------------------------
TThread* TThread::pthrCaller()
{
    //
    //  Just delegate to internal method. Unless somehow some external thread
    //  calls into some of our code (which shouldn't be happening except in
    //  some rare cases at a low level where the OS does callbacks, then the
    //  caller has to be inherently in the list.
    //
    //  If not, this guy will force a non-adopting thread object into the
    //  list, just to let us keep moving forward. But the thread will get
    //  stuck in the list because no one owns it. OTOH, we are not responsible
    //  for it either. It'll just say there until the app closes.
    //
    return pthrCurrent();
}


//
//  A non-conditional sleep. See the bSleep() method for usually a better option if
//  you have access to the thread object.
//
tCIDLib::TVoid TThread::Sleep(const tCIDLib::TCard4 c4MilliSecs)
{
    TKrnlThread::Sleep(c4MilliSecs);
}


tCIDLib::TThreadId TThread::tidCaller()
{
    return TKrnlThread::tidCaller();
}



// ---------------------------------------------------------------------------
//  TThread: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Sometimes a shutdown request needs to be cleared after it's seen. if it's
//  use to break out of a layer of processing but not the whole thread. This
//  is not encouraged, but sometimes it's the only practical way to do such
//  a thing since this is the only globally available way to break out of any
//  of the blocking methods in CIDLib and other CIDLib based code.
//
tCIDLib::TVoid TThread::AckShutdownRequest()
{
    m_bShutdownRequest = kCIDLib::False;
}


//
//  Tells the caller if the shutdown request flag is set. If another thread
//  is asking us to sync (a common thing in some cases, because it wants to
//  know we have seen the request), then we will sync up.
//
//  This can only be called by thread on itself. The Sync call, even if it
//  doesn't have to do a sync, will check this and throw if not.
//
tCIDLib::TBoolean TThread::bCheckShutdownRequest()
{
    //
    //  If another thread is asking us to sync, then do so. If not, this
    //  won't do anything.
    //
    Sync();

    // And return our flag state
   return m_bShutdownRequest;
}



// Return the GUI thread mark for this thread
tCIDLib::TBoolean TThread::bIsGUIThread() const
{
    return m_kthrThis.bIsGUIThread();
}


// Return whether we are running or nog
tCIDLib::TBoolean TThread::bIsRunning() const
{
    tCIDLib::TBoolean bRet;
    if (!m_kthrThis.bIsRunning(bRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_Running
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return bRet;
}


//
//  Sleeps for either the indicated time, or until a shutdown request is received.
//
//  If the full sleep period is done, we return true, else we return false to
//  indicate that we did not sleep the whole time due to a shutdown.
//
tCIDLib::TBoolean TThread::bSleep(const tCIDLib::TCard4 c4MilliSecs)
{
    //
    //  If zero time, just do a shutdown request check. If there's a request
    //  return false, to indicate we didn't wake up due to timeout, so negate
    //  the return.
    //
    if (!c4MilliSecs)
        return !bCheckShutdownRequest();

    //
    //  If it's more than 500ms, then we will do it in chunks, checking for
    //  a shutdown request each time around. This allows user code to do
    //  sleeps without having to worry about it holding up a shutdown
    //  request.
    //
    tCIDLib::TEncodedTime enctCur = TKrnlTimeStamp::enctNow();
    const tCIDLib::TEncodedTime enctEnd
    (
        enctCur + (c4MilliSecs * kCIDLib::enctOneMilliSec)
    );

    while (enctCur < enctEnd)
    {
        // Calc the millis left
        tCIDLib::TCard4 c4ThisTime = tCIDLib::TCard4
        (
            (enctEnd - enctCur) / kCIDLib::enctOneMilliSec
        );

        if (c4ThisTime > 500)
            c4ThisTime = 500;
        TKrnlThread::Sleep(c4ThisTime);

        // Sync with any waiting thread and then check the shutdown flag
        Sync();
        if (m_bShutdownRequest)
            return kCIDLib::False;

        // Haven't been asked to shutdown, so prep for another sleep
        enctCur = TKrnlTimeStamp::enctNow();
    }
    return kCIDLib::True;
}


//
//  Waits for this thread to die and returns any exit code, with an optional
//  timeout so that the caller doesn't block forever if the thread doesn't
//  exit. A thread cannot call this on himself.
//
tCIDLib::TBoolean
TThread::bWaitForDeath(         tCIDLib::EExitCodes&    eToFill
                        , const tCIDLib::TCard4         c4Timeout) const
{
    // Make sure we don't try to wait on ourself
    if (TKrnlThread::bIsCaller(m_kthrThis))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_WaitForSelf
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , m_strName
        );
    }

    tCIDLib::TBoolean bState;
    if (!m_kthrThis.bWaitForDeath(bState, eToFill, c4Timeout))
    {
        const TKrnlError& kerrLast = TKrnlError::kerrLast();
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_ThreadWait
            , kerrLast
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    // Return the state flag, which indicates whether it was dead or not
    return bState;
}


//
//  This is used in some cases where a thread just wants to suspend himself
//  until he's awakened again later by some other thread.
//
tCIDLib::TVoid TThread::BlockMyself()
{
    CheckCallerIsSelf
    (
        CID_LINE, facCIDLib().pszLoadCIDMsg(kCIDMsgs::midThrd_BlockSelf)
    );

    if (!m_kthrThis.bBlockMyself())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_Block
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , m_strName
        );
    }
}


// Retursn the current set priorty level for this thread
tCIDLib::EPrioLevels TThread::ePriority() const
{
    tCIDLib::EPrioLevels eRet;
    if (!m_kthrThis.bPriority(eRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_QueryThreadPrio
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return eRet;
}


//
//  Retursn the exist code of this thread. IT really has no meaning if the
//  thread hasn't yet ended.
//
tCIDLib::EExitCodes TThread::eTermCode() const
{
    tCIDLib::EExitCodes eExitCode;
    if (!m_kthrThis.bQueryExitCode(eExitCode))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_QueryExitCode
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , facCIDLib().strMsg(kCIDMsgs::midThrd_Thread)
            , m_strName
        );
    }
    return eExitCode;
}


//
//  Waits for up to a given time for this thread to die, and returns the
//  exit code returned by the thread.
//
tCIDLib::EExitCodes
TThread::eWaitForDeath(const tCIDLib::TCard4 c4Timeout) const
{
    // If debugging, make sure we don't try to wait on ourself
    if (TKrnlThread::bIsCaller(m_kthrThis))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_WaitForSelf
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , m_strName
        );
    }

    tCIDLib::EExitCodes eRet;
    tCIDLib::TBoolean bState;
    if (!m_kthrThis.bWaitForDeath(bState, eRet, c4Timeout) || !bState)
    {
        const TKrnlError& kerrLast = TKrnlError::kerrLast();

        tCIDLib::TErrCode errcLog = kCIDErrs::errcPrc_ThreadWait;
        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::CantDo;

        // If the error is a timeout, handle that separately
        if (!bState)
        {
            errcLog = kCIDErrs::errcPrc_Timeout;
            eClass = tCIDLib::EErrClasses::Timeout;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcLog
            , kerrLast
            , tCIDLib::ESeverities::Failed
            , eClass
            , m_strName
        );
    }
    return eRet;
}


//
//  Provides access to the kernel level thread object that represents this
//  thread.
//
const TKrnlThread& TThread::kthrThis() const
{
    return m_kthrThis;
}

TKrnlThread& TThread::kthrThis()
{
    return m_kthrThis;
}


// Mark the underlying thread as the GUI thread
tCIDLib::TVoid TThread::MarkAsGUIThread()
{
    m_kthrThis.MarkAsGUIThread();
}


//
//  Lets the outside world set an onexit callback on us. This will be called
//  when the as the thread is exiting.
//
tCIDLib::TExitFuncPtr TThread::pfnSetOnExit(const tCIDLib::TExitFuncPtr pfnNew)
{
    tCIDLib::TExitFuncPtr pfnTmp = m_pfnOnExit;
    m_pfnOnExit = pfnNew;
    return pfnTmp;
}


//
//  Used to release a thread that has been sycned to.
//
tCIDLib::TVoid TThread::Release()
{
    //
    //  Lock the thread list while we do this. This effectively makes this
    //  operation atomic, so that the data cannot change in the meantime.
    //  This is kind of cheesy, but it works and avoids having to have a
    //  mutex in every thread object just for this stuff.
    //
    {
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        // Make sure the caller is the one that did the sync
        #if CID_DEBUG_ON
        if (TKrnlThread::tidCaller() != m_tidSyncReq)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcPrc_NotSyncReq
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , m_strName
            );
        }
        #endif

        // If there is no outstanding sync request, then log a warning
        if (!m_bSyncRequest)
        {
            if (facCIDLib().bLogWarnings())
            {
                facCIDLib().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcPrc_NoSyncRequest
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::AppError
                    , m_strName
                );
            }
        }

        // Clear the sync request flag now that we are being let go
        m_bSyncRequest = kCIDLib::False;
        m_tidSyncReq = kCIDLib::tidInvalid;

        // Reset the reply again
        if (!m_kevResponse.bReset())
        {
            // Process fatal error, so always log
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Reset
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TString(m_kevResponse.pszName())
            );
        }

        // Post the sync event to let the thread that is releasing us go
        if (!m_kevSync.bTrigger())
        {
            // Process fatal error, so always log
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Trigger
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TString(m_kevSync.pszName())
            );
        }
    }

    //
    //  And wait till he indicates he's going again or dies, since either
    //  could possibly happen. It doesn't matter which, but we just want to
    //  come back as soon as either does, and not just timeout if he goes
    //  away.
    //
    //  IMPORTANT:  The reason for this last wait is that if we don't
    //              do this, we can get around and call WaitSync again before
    //              he ever sees the sync trigger, in which case we are
    //              screwed and deadlocked since he'll stay asleep.
    //
    tCIDLib::TBoolean bDied;
    if (!m_kthrThis.bWaitEvOrDeath(m_kevResponse, bDied, 60000))
    {
        //
        //  Any failure here is bad. We are also checking for death, so if
        //  we time out, it couldn't be because he died, he has to be alive
        //  but hasn't signaled.
        //
        //  We don't get a false return if it dies, only if a timeout or
        //  some other error.
        //
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_ReleaseErr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Timeout
            , m_strName
        );
    }
}


//
//  Other threads call this in order to ask this thread to shut down.
//
//  In many cases the other thread wants to make sure that this thread is at
//  a particular place when this request is made, so they want to do a sync.
//  Other times they don't, because they are going to come back later and
//  wait for that process to complete, so we support both schemes.
//
//  If syncing, the caller provides a timeout that's the longest they'll wait
//  for this to happen. If we return successfully, then the thread has acked
//  the request and is on the way down.
//
//  If not syncing, we just set the shutdown request thread. The caller
//  doesn't expect the thread to be going down right now, but that it will
//  as soon as it sees the request flag.
//
tCIDLib::TVoid TThread::ReqShutdownNoSync()
{
    m_bShutdownRequest = kCIDLib::True;
}

tCIDLib::TVoid TThread::ReqShutdownSync(const tCIDLib::TCard4 c4TimeOut)
{
    // If its not running, then we are done already
    if (!bIsRunning())
        return;

    //
    //  We are sync'ing, so let's wait for this thread to sync with us,
    //  we are the caller thread remember in this case.
    //
    try
    {
        WaitSync(c4TimeOut);
    }

    catch(TError& errToCatch)
    {
        // If it's that he is already stopped, we are ok with that
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcPrc_NotRunning))
        {
            return;
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // This thread is at the Sync call now. Set the flag and release him
    try
    {
        m_bShutdownRequest = kCIDLib::True;
        Release();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // It failed, so be safe and reset the flag
        m_bShutdownRequest = kCIDLib::False;
        throw;
    }
}


// Return a ref to the name of this thread
const TString& TThread::strName() const
{
    return m_strName;
}


// Set the thread priority for this thread.
tCIDLib::TVoid TThread::SetPriority(const tCIDLib::EPrioLevels ePrioLev)
{
    // If debugging, then make sure the level is reasonable
    #if CID_DEBUG_ON
    if ((ePrioLev < tCIDLib::EPrioLevels::Min)
    ||  (ePrioLev > tCIDLib::EPrioLevels::Max))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_BadPrioLevel
            , facCIDLib().pszLoadCIDMsg(kCIDMsgs::midThrd_ClippedPrio)
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(ePrioLev))
            , TCardinal(tCIDLib::c4EnumOrd(tCIDLib::EPrioLevels::Max))
        );
    }
    #endif

    //
    //  If this thread is in SelfPrio mode, then only it can set its own
    //  priority.
    //
    if (m_bSelfPrio)
    {
        // Abort if the caller is not this thread
        CheckCallerIsSelf
        (
            CID_LINE
            , facCIDLib().pszLoadCIDMsg(kCIDMsgs::midThrd_SelfPrioMode)
        );
    }

    if (!m_kthrThis.bSetPriority(ePrioLev))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_SetThreadPrio
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}


//
//  No matter how the thread is pointed at some entry point, this is called to start
//  the thread running. They can pass a pointer to some data. They still own that data,
//  the new thread must get a copy out before calling sync.
//
tCIDLib::TVoid TThread::Start(          tCIDLib::TVoid* const   pData
                                , const tCIDLib::EThreadFlags   eFlags)
{
    //
    //  To try to help catch starting up a null thread object, we provide
    //  this check when debugging. It does happen sometimes and the results
    //  can be a mess to diagnose.
    //
    #if CID_DEBUG_ON
    if (this == 0)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_NullThreadObj
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  Note that this method is *always* called from another thread, since
    //  a thread cannot start itself, since its not running in order to do
    //  so. So we don't have to check that.
    //

    // Can start a running thread
    if (bIsRunning())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_AlreadyRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , m_strName
        );
    }

    // Make sure some flags are set correctly for our initial startup
    m_bShutdownRequest = kCIDLib::False;

    // Reset the sync semaphore so it won't go until we tell it to
    if (!m_kevSync.bReset())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Reset
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TString(m_kevSync.pszName())
        );
    }

    //
    //  We act as though a sync request is already outstanding as soon as it
    //  starts. This is so we can block until it syncs up, at which time the
    //  calling thread can be sure its started up (and gotten any passed data
    //  before the calling thread lets go of it, etc...)
    //
    if (!m_kevResponse.bReset())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Reset
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TString(m_kevResponse.pszName())
        );
    }

    m_bSyncRequest = kCIDLib::True;
    m_tidSyncReq = TKrnlThread::tidCaller();

    //
    //  And ask the kernel to start the thread up. It will really start up a local,
    //  platform specific thread function. That guy will set up a platform specific,
    //  low level exception handler and then call back on our eThreadStart function.
    //
    if (!m_kthrThis.bBeginThread(eThreadStart
                                , m_c4StackSz
                                , pData
                                , eFlags
                                , this
                                , m_strName.pszBuffer()))
    {
        m_bSyncRequest = kCIDLib::False;
        m_tidSyncReq = kCIDLib::tidInvalid;

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_ThreadStartFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    //
    //  So now we need to wait for the new thread to call Sync() for the
    //  first time. At that point, we can return to the caller who knows
    //  that the new thread is up and going. It of course may die before it
    //  does that, which we wait for as well.
    //
    //  When debugging, make the wait pretty long so that its possible to
    //  trace through the other thread's startup without the parent thread
    //  timing out.
    //
    //  We wait for it to either trigger the event, or die, which could
    //  happen if it gets an exception during init before it syncs.
    //
    #if CID_DEBUG_ON
    const tCIDLib::TCard4 c4TimeOut = 480000;
    #else
    const tCIDLib::TCard4 c4TimeOut = 30000;
    #endif

    tCIDLib::TBoolean bDied;
    if (!m_kthrThis.bWaitEvOrDeath(m_kevResponse, bDied, c4TimeOut))
    {
        // Something has gone fundamentally wrong, bu tlet's try to clean up
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        m_bSyncRequest = kCIDLib::False;
        m_tidSyncReq = kCIDLib::tidInvalid;

        const TKrnlError& kerrLast = TKrnlError::kerrLast();

        tCIDLib::TErrCode errcLog = kCIDErrs::errcEv_Wait;
        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::CantDo;

        // If the error is a timeout, handle that separately
        if (kerrLast.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            errcLog = kCIDErrs::errcEv_Timeout;
            eClass = tCIDLib::EErrClasses::Timeout;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcLog
            , kerrLast
            , tCIDLib::ESeverities::ProcFatal
            , eClass
            , TString(m_kevResponse.pszName())
        );
    }

    //
    //  Ok, he is up and waiting in his Sync() call, so we can let him go
    //  now, if he didn't die
    //
    if (!bDied)
        Release();
}


//
//  This is used by this thread to check if another thread has requested that it
//  sync up. If there is an outstanding sync request, we need to let the waiting
//  thread know and block ourselves. He has to subsequently call Release to let us
//  go again.
//
//  Typically this is used when another thread has to know that this thread is at a
//  particular place in his processing before doing something. So this thread will
//  call Sync() at that spot. If no request is outstanding the overhead is tiny.
//
//  This is NOT for use to keep this thread blocked. It's just to make sure we are
//  at a particular place so that he can update some data or whatever. We will only
//  wait up to 60 seconds to be released. If this thread needs to block, then some
//  other ad hoc mechanism should be used.
//
tCIDLib::TVoid TThread::Sync()
{
    // This can only be called by this thread
    CheckCallerIsSelf(CID_LINE);

    //
    //  If there is a sync request, then deal with it. What we do is first
    //  check the flag. If it's not set, we just return. If it is set, then
    //  we lock and check again.
    //
    //  This way, we avoid the overhead of a mutex lock 99.999% of the time,
    //  but we still do the right thing and lock before really committing
    //  to reacting to the sync request, so that we don't get into a race
    //  condition.
    //
    if (m_bSyncRequest)
    {
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());
        if (!m_bSyncRequest)
            return;

        // Post the response semaphore to indicate that we are now waiting
        if (!m_kevResponse.bTrigger())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Trigger
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::CantDo
                , TString(m_kevResponse.pszName())
            );
        }
    }
     else
    {
        // No request, just return
        return;
    }

    // Wait for the caller to either post the sync event, or die, either one.
    tCIDLib::TBoolean bDied;
    if (!m_kthrThis.bWaitEvOrDeath(m_kevSync, bDied, 60000))
    {
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        const TKrnlError& kerrLast = TKrnlError::kerrLast();
        tCIDLib::TErrCode errcLog = kCIDErrs::errcEv_Wait;
        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::CantDo;

        // If the error is a timeout, handle that separately
        if (kerrLast.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            errcLog = kCIDErrs::errcEv_Timeout;
            eClass = tCIDLib::EErrClasses::Timeout;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcLog
            , kerrLast
            , tCIDLib::ESeverities::ProcFatal
            , eClass
            , TString(m_kevSync.pszName())
        );
    }

    //
    //  We are awake again so the syncing thread has released us. So let him
    //  know we're going again. If he died, then it doesn't really make any
    //  difference but we do it anyway.
    //
    if (!m_kevResponse.bTrigger())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Trigger
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::CantDo
            , TString(m_kevResponse.pszName())
        );
    }
}


tCIDLib::TThreadId TThread::tidThis() const
{
    return m_kthrThis.tidThis();
}


//
//  This is called to unblock this thread after it has previous blocked itself. In
//  theory this thread could call this on itself, but it would just have no effect.
//  If it actually is unblocking this thread, obviously this thread cannot
//  accidentally unblock itself since it's not running.
//
tCIDLib::TVoid TThread::Unblock()
{
    if (!m_kthrThis.bUnblock())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_Unblock
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , m_strName
        );
    }
}


//
//  This is called to make a sync request on another thread. That thread will see that
//  request the next time it comes back around its loop and calls Sync(). We
//  need to wait for it to acknowledge this and to know that it is at a sync point.
//  When that happens, this will return and he will be blocked until we Release() him.
//  If it doesn't happen within the timeout period, then we throw.
//
//  This is not for long term blocking. He will only wait up to 60 seconds after syncing.
//  This is just to sync, update some stuff, and then release.
//
tCIDLib::TVoid TThread::WaitSync(const tCIDLib::TCard4 c4Timeout)
{
    // Get the the id of the calling thread
    const tCIDLib::TThreadId tidCaller = TKrnlThread::tidCaller();

    // A thread cannot wait to sync on itself
    #if CID_DEBUG_ON
    if (m_kthrThis.tidThis() == tidCaller)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_SyncWithSelf
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , m_strName
        );
    }
    #endif

    // A faux block to minimize thread list lock time
    try
    {
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        // If there is already a request, then we can't do it
        if (m_bSyncRequest)
        {
            TThread* pthrSyncer = pthrFindId(m_tidSyncReq);

            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcPrc_AlreadySynced
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Already
                , m_strName
                , pthrSyncer->m_strName
            );
        }

        //
        //  Reset the sync semaphore to cause the thread to block the
        //  next time it calls Sync().
        //
        if (!m_kevSync.bReset())
        {
            m_kevResponse.bTrigger();
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Reset
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TString(m_kevSync.pszName())
            );
        }

        // Reset our response event that we are going to block on
        if (!m_kevResponse.bReset())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Reset
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TString(m_kevResponse.pszName())
            );
        }

        // Remember the syncer's id for later
        m_tidSyncReq = tidCaller;

        // Set the flag now to make the other thread sync with us
        m_bSyncRequest = kCIDLib::True;
    }

    catch(TError& errToCatch)
    {
        // Lock and clean up then rethrow
        {
            TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());
            m_bSyncRequest = kCIDLib::False;
            m_tidSyncReq = kCIDLib::tidInvalid;
            m_kevResponse.bTrigger();
            m_kevSync.bTrigger();
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    //
    //  Wait for either the thread to die or the response event to be trigged by
    //  the thread. If we get an error in the wait, or the thread died instead of
    //  responding, then handle that. This could particularly happen if it was on
    //  the way out as we were setting up the above to get ready.
    //
    tCIDLib::TBoolean bDied;
    if (!m_kthrThis.bWaitEvOrDeath(m_kevResponse, bDied, c4Timeout))
    {
        // Get the error before we do anything else
        TKrnlError kerrLast = TKrnlError::kerrLast();

        // Lock again so we can clean up
        TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

        // Clear the flags back out
        m_bSyncRequest = kCIDLib::False;
        m_tidSyncReq = kCIDLib::tidInvalid;

        // Just in case the thread got blocked, let him go
        if (!m_kevSync.bTrigger())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Trigger
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TString(m_kevResponse.pszName())
            );
        }

        // If the error is a timeout, handle that separately
        if (kerrLast.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcPrc_TimeOnSync
                , kerrLast
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Timeout
                , m_strName
            );
        }
         else
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Wait
                , kerrLast
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TString(m_kevResponse.pszName())
            );
        }
    }

    // If it died, then throw a not running error
    if (bDied)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_NotRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , m_strName
        );
    }
}



// ---------------------------------------------------------------------------
//  TThread: Hidden constructors
// ---------------------------------------------------------------------------

//
//  This one is protected. It it used to create a thread object for the primary
//  thread, which it will just adopt. It sets the name to a prefab primary
//  thread name.
//
TThread::TThread() :

    m_bSelfPrio(kCIDLib::True)
    , m_bShutdownRequest(kCIDLib::False)
    , m_bSyncRequest(kCIDLib::False)
    , m_c4ListInd(CIDLib_Thread::c4MaxThreads)
    , m_c4StackSz(0)
    , m_kthrThis()
    , m_ptfuncToRun(0)
    , m_pfnOnExit(0)
    , m_tidSyncReq(kCIDLib::tidInvalid)
    , m_strName(kCIDLib::pszPrimaryThrdName)
{
    if (!m_kthrThis.bAdoptCaller())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_Adopt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// ---------------------------------------------------------------------------
//  TThread: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method will check to make sure that the calling thread is this
//  thread. If not, it will throw an exception
//
tCIDLib::TVoid
TThread::CheckCallerIsSelf(const   tCIDLib::TCard4     c4LineNum
                            , const tCIDLib::TCh* const pszAuxText) const
{
    if (m_kthrThis.tidThis() != TKrnlThread::tidCaller())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4LineNum
            , kCIDErrs::errcPrc_NotThisThread
            , pszAuxText ? pszAuxText : kCIDLib::pszEmptyZStr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Authority
            , this->m_strName
        );
    }
}


//
//  This method is called from the thread startup code when the thread is
//  ending normally. It handles doing all of the work that needs to be done
//  when the thread ends. If the thread exists due to an unhandled exception,
//  then this is not called, and it doesn't matter since the program is going
//  to die anyway, and the thread might not be in a sane enough state to deal
//  with the cleanup either.
//
tCIDLib::TVoid TThread::ShutdownProcessing()
{
    // Lock while we clean up
    TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

    //
    //  Call the on-exit function to let the thread object clean up. This
    //  is for user code since it does not require a separate derivation to
    //  do.
    //
    if (m_pfnOnExit)
        m_pfnOnExit(*this);

    //
    //  Call the virtual terminate method to let derived thread classes do
    //  internal cleanup. This must be done in the context of the thread.
    //
    Terminate();

    //
    //  And mark it inactive in our thread list. It will not be actually
    //  removed from the list until the thread object is destroyed (since
    //  it might just be restarted.)
    //
    CIDAssert(m_c4ListInd < CIDLib_Thread::c4MaxThreads, L"Thread index is invalid");
    athriList()[m_c4ListInd].tidThread = kCIDLib::tidInvalid;

    // Clean up any per-thread data for this thread
    TKrnlPerThreadData::CleanUpThreadData();
}


// ---------------------------------------------------------------------------
//  TThread: Protected, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TThread::Init()
{
    //
    //  Default, do-nothing implementation. Derived class must call us before
    //  its processing though, in case we need to add processing.
    //
}


tCIDLib::EExitCodes TThread::eProcess()
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcPrc_NoProcessOverride
        , tCIDLib::ESeverities::ProcFatal
        , tCIDLib::EErrClasses::AppError
    );

    // Make the compiler happy
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::TVoid TThread::Terminate()
{
    //
    //  Default, do-nothing implementation. Derived class must call us after
    //  its processing though, in case we need to add processing.
    //
}


// ---------------------------------------------------------------------------
//  TThread: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TThread::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo   << facCIDLib().strMsg(kCIDMsgs::midThrd_ThreadEq)
                    << m_strName
                    << facCIDLib().strMsg(kCIDMsgs::midThrd_RunningEq)
                    << bIsRunning()
                    << facCIDLib().strMsg
                       (
                        kCIDMsgs::midThrd_IdEq
                        , TCardinal(m_kthrThis.tidThis())
                       );
}


// ---------------------------------------------------------------------------
//  TThread: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TThread::CommonInit()
{
    // Get access to the thread list
    TMtxLocker mtxlAccess(CIDLib_Thread::pmtxListAccess());

    //
    //  Check the thread list and see if there is already a thread with
    //  this name.
    //
    TThread* pthrStarting = pthrFindName(m_strName);

    //
    //  If we get a thread back, then a thread with that name already exists.
    //  If so, and it's not this thread, that's a problem.
    //
    if (pthrStarting && (pthrStarting != this))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_AlreadyExists
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , m_strName
        );
    }

    //
    //  Make sure that the stack size is not less than 64K, unless it is
    //  zero which means let the system decide.
    //
    if (m_c4StackSz && (m_c4StackSz < 0xFFFF))
        m_c4StackSz = 0xFFFF;

    // Create this thread's sync/response semaphores
    if (!m_kevSync.bCreate(tCIDLib::EEventStates::Triggered, kCIDLib::True))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TString(L"m_kevSync")
        );
    }

    if (!m_kevResponse.bCreate(tCIDLib::EEventStates::Triggered, kCIDLib::True))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TString(L"m_kevResponse")
        );
    }
}

