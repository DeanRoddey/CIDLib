//
// FILE NAME: CIDLib_Module.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/05/1997
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
//  TModule is the basic public class that represents a loadable file,
//  a library or executable module.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TModule,TObject)


namespace
{
    namespace CIDLib_Module
    {
        // -----------------------------------------------------------------------
        //  Local static data
        //
        //  atomInitMsgs
        //      A flag to control whether we've done local static initialization
        //      yet.
        //
        //  eAdoptLogger
        //      Indicates whether we own the installed logger (if any) or not.
        //
        //  plgrCurrent
        //      This is the current logger that we send all errors and messages
        //      logged through this process to. We don't own it. We may own it or not
        //      depending on the eAdoptLogger value.
        //
        //  pszTitle1
        //  pszTitle2
        //      The two title strings used in standard popups, support for which
        //      is provided here. They are defaulted so something is there until
        //      they get loaded from translatable text.
        //
        //  pszExceptDuringLog
        //  pszRecursiveError
        //  pszStrLoadFailure
        //      Errors displayed when things go wrong during fundamental ops
        //      provided here. They are defaulted so something is there until
        //      they get loaded from translatable text.
        // -----------------------------------------------------------------------
        TAtomicFlag          atomInitMsgs;
        const tCIDLib::TCh*  pszTitle1 = kCIDLib_::pszTitle1;
        const tCIDLib::TCh*  pszTitle2 = kCIDLib_::pszTitle2;
        const tCIDLib::TCh*  pszExceptDuringLog = kCIDLib_::pszExceptDuringLog;
        const tCIDLib::TCh*  pszRecursiveError = kCIDLib_::pszRecursiveError;
        const tCIDLib::TCh*  pszStrLoadFailure = kCIDLib_::pszStrLoadFailure;


        // -----------------------------------------------------------------------
        //  m_bInitStats
        //  m_sciStartTime
        //  m_sciDroppedLogEvs
        //  m_sciLogErrors
        //      We maintain some stats cache values. This is the storage for those
        //      and a lazy init flag to fault them in.
        // -----------------------------------------------------------------------
        TAtomicFlag      atomInitStats;
        TStatsCacheItem  sciStartTime;
        TStatsCacheItem  sciDroppedLogEvs;
        TStatsCacheItem  sciLogErrors;

        //
        //  We need a little structure we use to maintain a list of log event objects
        //  to be spooled out by our spooling thread. We don't want to use any
        //  high level stuff since it may log messages and get us into a circular
        //  freakout. So we just maintain a simple linked list, and all we ever do
        //  with it is add at the start and remove at the end. The spooling thread
        //  keeps a head and tail pointer (new ones are added at the head and removed
        //  at from the tail by the thread.
        //
        //  If the events aren't time stamped and marked, we do that
        //
        struct TLogQEvent
        {
            TLogQEvent(const TLogEvent& logevSrc) :

                logevData(logevSrc)
            {
                logevData.SetLogged();
            }

            TLogQEvent(TLogEvent&& logevSrc) :

                logevData(tCIDLib::ForceMove(logevSrc))
            {
                logevData.SetLogged();
            }

            // For emplacement via the log msg variations
            template <typename... TArgs> TLogQEvent(TArgs&&... Args) :

                logevData(tCIDLib::Forward<TArgs>(Args)...)
            {
                logevData.SetLogged();
            }

            TLogQEvent*     plogqevPrev = nullptr;
            TLogQEvent*     plogqevNext = nullptr;
            TLogEvent       logevData;
        };

        //
        //  We need a mutex for local sync. Because module stuff can be called
        //  early in the process, we use a lazy faulting in scheme here, to be
        //  sure it'll be available when needed.
        //
        TMutex* pmtxLogSync()
        {
            static TMutex mtxLogSync(tCIDLib::ELockStates::Unlocked);
            return &mtxLogSync;
        }
    }
}




// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TVoid InitMsgs(const TModule& modSrc)
{
    if (!CIDLib_Module::atomInitMsgs)
    {
        TBaseLock lockInit;
        if (!CIDLib_Module::atomInitMsgs)
        {
            tCIDLib::TBoolean   bOk;
            const tCIDLib::TCh* pszTmp;

            pszTmp = modSrc.pszLoadCIDMsg(kCIDMsgs::midMod_ExceptDuringLog, bOk);
            if (bOk)
                CIDLib_Module::pszExceptDuringLog = pszTmp;

            pszTmp = modSrc.pszLoadCIDMsg(kCIDMsgs::midMod_RecursiveErr, bOk);
            if (bOk)
                CIDLib_Module::pszRecursiveError = pszTmp;

            pszTmp = modSrc.pszLoadCIDMsg(kCIDMsgs::midMod_DefStrLoad, bOk);
            if (bOk)
                CIDLib_Module::pszStrLoadFailure = pszTmp;

            CIDLib_Module::atomInitMsgs.Set();
        }
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TLogSpoolThread
//  PREFIX: thr
//
//  This is our internal log even spooler thread. It is spun up upon first need.
// ---------------------------------------------------------------------------
class TLogSpoolThread : public TThread
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TLogSpoolThread() :

            TThread(L"CIDLibIntLogSpoolerThread")
            , m_bTriedDefault(kCIDLib::False)
            , m_c4QueueSize(0)
            , m_eAdopt(tCIDLib::EAdoptOpts::NoAdopt)
            , m_eAdoptNew(tCIDLib::EAdoptOpts::NoAdopt)
            , m_plgrNew(nullptr)
            , m_plgrTarget(nullptr)
            , m_plogqevHead(nullptr)
            , m_plogqevTail(nullptr)
        {
            //
            //  Check for a default logger defined in the environment and store that
            //  info if so. After this we won't have to keep checking.
            //
            if (TSysInfo::pszLogInfo())
                m_kstrDefLoggerInfo = TSysInfo::pszLogInfo();
            else
                TKrnlEnvironment::bFind(kCIDLib_::pszLocalLog, m_kstrDefLoggerInfo);
        }

        ~TLogSpoolThread() {}


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  This is just advisory for the code below to not bother creating an object
        //  if there's no logger set. It might get created the next clock tick but
        //  that's fine.
        //
        tCIDLib::TBoolean bHaveLogger()
        {
            //
            //  If we don't have one, and there's default info available, and we've not
            //  tried to create that guy already, then try to fault it in.
            //
            if (!m_plgrTarget && !m_bTriedDefault && !m_kstrDefLoggerInfo.bIsEmpty())
                SetDefaultLogger();

            return m_plgrTarget != nullptr;
        }

        tCIDLib::TVoid SetLogger(       MLogger* const          plgrNew
                                , const tCIDLib::EAdoptOpts     eAdopt)
        {
            TLocker lockrSync(CIDLib_Module::pmtxLogSync());

            // If there is one that hasn't been gotten yet we have to deal with it
            if (m_plgrNew && (m_eAdoptNew == tCIDLib::EAdoptOpts::Adopt))
            {
                try
                {
                    delete m_plgrNew;
                }

                catch(...)
                {
                    TStatsCache::c8IncCounter(CIDLib_Module::sciLogErrors);
                }
            }

            m_eAdoptNew = eAdopt;
            m_plgrNew = plgrNew;
        }

        // We have to just copy it
        tCIDLib::TVoid QueueEvent(const TLogEvent& logevSrc)
        {
            QueueEvent(new CIDLib_Module::TLogQEvent(logevSrc));
        }

        // We can move it
        tCIDLib::TVoid QueueEvent(TLogEvent&& logevSrc)
        {
            QueueEvent(new CIDLib_Module::TLogQEvent(tCIDLib::ForceMove(logevSrc)));
        }

        // For TModule to create directly, mostl for emplacement scenarios
        tCIDLib::TVoid QueueEvent(CIDLib_Module::TLogQEvent* const plogqevNew)
        {
            TLocker lockrSync(CIDLib_Module::pmtxLogSync());

            //
            //  Make sure there is space available. If not, then we need to reject this
            //  guy.
            //
            if (m_c4QueueSize >= c4MaxLogQSize)
            {
                try
                {
                    delete plogqevNew;
                }

                catch(...)
                {
                    TStatsCache::c8IncCounter(CIDLib_Module::sciLogErrors);
                }
                TStatsCache::c8IncCounter(CIDLib_Module::sciDroppedLogEvs);
                return;
            }

            if (!m_plogqevTail && !m_plogqevHead)
            {
                //
                //  Special case #1 of an empty list, just point the head and tail at
                //  this guy
                //
                m_plogqevHead = plogqevNew;
                m_plogqevTail = plogqevNew;

                // Make sure prev/next are both null
                m_plogqevHead->plogqevPrev = nullptr;
                m_plogqevHead->plogqevNext = nullptr;
            }
             else if (m_plogqevHead == m_plogqevTail)
            {
                //
                //  Special case #2, we have a single node currently. So we just
                //  set this guy as the head (the tail stays the same), and we
                //  point them at each other.
                //
                m_plogqevHead = plogqevNew;
                m_plogqevHead->plogqevPrev = nullptr;
                m_plogqevHead->plogqevNext = m_plogqevTail;
                m_plogqevTail->plogqevPrev = m_plogqevHead;
            }
             else
            {
                //
                //  The standard case, we have to work the new one into the head of
                //  the list. The tail is not directly used, though it could be the
                //  Next node of the head if we only have two nodes.
                //
                CIDLib_Module::TLogQEvent* plogqevOldHead = m_plogqevHead;

                // Make the new guy the head (with no previous)
                m_plogqevHead = plogqevNew;
                m_plogqevHead->plogqevPrev = nullptr;

                // Make the old head his next and him the old guy's previous
                m_plogqevHead->plogqevNext = plogqevOldHead;
                plogqevOldHead->plogqevPrev = m_plogqevHead;
            }
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eProcess() final;


    private :
        // -------------------------------------------------------------------
        //  Private class constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4DefLogMaxChars   = 512;
        static const tCIDLib::TCard4 c4MaxLogQSize      = 8192;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetDefaultLogger();


        // -------------------------------------------------------------------
        //  Private members
        //
        //  This stuff is protected by the mutex that is faulted in in the local
        //  namespace above.
        //
        //  m_bTriedDefault
        //      WE can fault in a commandline/env driven logger, but it could be
        //      bad info and we don't want to continously try to do that, so we
        //      set this once we've go through that.
        //
        //  m_c4QueueSize
        //      We can't let the queue run wild if the logger somehow starts really
        //      getting slow to process events. So we keep up with the size and
        //      will start rejecting once we hit c4MaxLogQSize.
        //
        //  m_eAdopt
        //  m_eAdoptNew
        //      We may or may not adopt the logger, depending on what the caller that
        //      gave it to us says. We need one for the logger and the new logger.
        //
        //  m_plgrNew
        //  m_plgrTarget
        //      This is the target we send the messages to. We don't have to lock
        //      across calls to this. If the outside world sets a new one, it will
        //      be put into m_plgrNew while locked, and we'll locked and store that
        //      new pointer. If they happened to do two before we store the first,
        //      the first one would never get used.
        //
        //  m_plogqevHead
        //  m_plogqevTail
        //      These are our head and tail nodes. New ones are added at the head and
        //      the spooling thread pulls old ones from the tail. If both are null
        //      the list is empty. If both point to the same node, there's only one
        //      node. Else, it's a standard doubly linked list scenario.
        //
        //  m_kstrDefLoggerInfo
        //      In the ctor we see if a default logger is defined in the environment.
        //      If so, we will try to fault one of those in if bHasLogger() is called
        //      and we've not tried it already.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bTriedDefault;
        tCIDLib::TCard4             m_c4QueueSize;
        tCIDLib::EAdoptOpts         m_eAdopt;
        tCIDLib::EAdoptOpts         m_eAdoptNew;
        TKrnlString                 m_kstrDefLoggerInfo;
        MLogger*                    m_plgrNew;
        MLogger*                    m_plgrTarget;
        CIDLib_Module::TLogQEvent*  m_plogqevHead = nullptr;
        CIDLib_Module::TLogQEvent*  m_plogqevTail = nullptr;
};


tCIDLib::EExitCodes TLogSpoolThread::eProcess()
{
    while (!bCheckShutdownRequest())
    {
        try
        {
            // If there's a new logger, let's get that
            if (m_plgrNew)
            {
                TLocker lockrSync(CIDLib_Module::pmtxLogSync());
                if (m_plgrNew)
                {
                    // Clean up any current one if we adopted it
                    if (m_plgrTarget && (m_eAdopt == tCIDLib::EAdoptOpts::Adopt))
                    {
                        try
                        {
                            delete m_plgrTarget;
                        }

                        catch(...)
                        {
                            TStatsCache::c8IncCounter(CIDLib_Module::sciLogErrors);
                        }
                    }

                    m_eAdopt = m_eAdoptNew;
                    m_plgrTarget = m_plgrNew;

                    m_eAdoptNew = tCIDLib::EAdoptOpts::NoAdopt;
                    m_plgrNew = nullptr;
                }
            }

            //
            //  Do a quick check to see if the tail is non-null before we bother
            //  locking. Once set only we can null it again, so this is safe to
            //  do.
            //
            if (m_plogqevTail)
            {
                CIDLib_Module::TLogQEvent* plogqevCur = nullptr;
                {
                    TLocker lockrSync(CIDLib_Module::pmtxLogSync());

                    // We have some special cases to deal with.
                    if (m_plogqevHead == m_plogqevTail)
                    {
                        //
                        //  If head and tail are the same, then we have one node, so
                        //  we take it and set both to null since the list is empty
                        //  now.
                        //
                        plogqevCur = m_plogqevTail;
                        m_plogqevTail = nullptr;
                        m_plogqevHead = nullptr;
                    }
                     else if (m_plogqevHead->plogqevNext == m_plogqevTail)
                    {
                        //
                        //  We have two so they are pointing at each other. We take
                        //  take the tail and store the current head in the tail
                        //  pointer, so we are back to the single node. The head
                        //  now has no next node.
                        //
                        plogqevCur = m_plogqevTail;
                        m_plogqevTail = m_plogqevHead;
                        m_plogqevHead->plogqevNext = nullptr;
                    }
                     else
                    {
                        //
                        //  It's the standard scenario, so take the tail and store
                        //  it's previous node in the tail ptr, setting the new tail's
                        //  next to null.
                        //
                        plogqevCur = m_plogqevTail;
                        m_plogqevTail = plogqevCur->plogqevPrev;
                        m_plogqevTail->plogqevNext = nullptr;
                    }
                }

                try
                {
                    //
                    //  Let's send this one to the logger if we have one. Just to be
                    //  safe make sure we really ended up with a node.
                    //
                    if (m_plgrTarget)
                        m_plgrTarget->LogEvent(plogqevCur->logevData);
                }

                catch(...)
                {
                    TStatsCache::c8IncCounter(CIDLib_Module::sciLogErrors);
                }

                // And clean up the event object
                try
                {
                    delete plogqevCur;
                }

                catch(...)
                {
                    TStatsCache::c8IncCounter(CIDLib_Module::sciLogErrors);
                }
            }
             else
            {
                // Else sleep a bit, breaking out if asked to stop
                if (!bSleep(1000))
                    break;
            }
        }

        catch(...)
        {
            TStatsCache::c8IncCounter(CIDLib_Module::sciLogErrors);
        }
    }

    //  Dump any remaining events from our list.
    //  <TBD>

    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::TVoid TLogSpoolThread::SetDefaultLogger()
{
    //
    //  We have to lock while we do this since the default may create named
    //  resources that could fail if multiple threads tried to do it. And we
    //  ultimately need to set the logger pointer.
    //
    TLocker lockrSync(CIDLib_Module::pmtxLogSync());

    //
    //  Some one could have beaten us to it. We don't want to lock every time
    //  just to test that stuff that lead us here.
    //
    if (m_plgrTarget || m_plgrNew || m_bTriedDefault)
        return;

    // Before we do anything remember we've tried this in case it goes wrong
    m_bTriedDefault = kCIDLib::True;

    const tCIDLib::TCard4 c4MaxBufSz = 1024;

    #pragma warning(suppress : 26494) // We don't need to initialize these
    tCIDLib::TCh szFileName[c4MaxBufSz + 1];
    #pragma warning(suppress : 26494) // We don't need to initialize these
    tCIDLib::TCh szMutexName[c4MaxBufSz + 1];

    szFileName[0] = kCIDLib::chNull;
    szMutexName[0] = kCIDLib::chNull;

    //
    //  Preload the failure strings. If they fail to load, then put
    //  in a default english messages.
    //
    tCIDLib::TBoolean bOk;
    const tCIDLib::TCh* pszBadFormat = facCIDLib().pszLoadCIDMsg
    (
        kCIDErrs::errcMod_BadLogInfoFmt, bOk
    );
    if (!bOk)
        pszBadFormat = kCIDLib_::pszBadLocalLog;

    const tCIDLib::TCh* pszErrOpenLgr = facCIDLib().pszLoadCIDMsg
    (
        kCIDErrs::errcMod_ErrOpenLgr, bOk
    );
    if (!bOk)
        pszErrOpenLgr = kCIDLib_::pszErrCreatingLgr;

    //
    //  The local log information is a blob that we have to parse out.
    //  It is in this format:
    //
    //      filename;format;mutexname
    //
    //  The file name is the name of the file to log to. It is
    //  followed by an optional format and mutex name. The format must
    //  be either Ascii or Unicode. The mutex name is optional and
    //  should be just the last component of a standard CIDLib
    //  named resource name. If the format is not provided but the
    //  mutex is, then it should be filename;;mutexname.
    //
    TTextConverter* ptcvtToUse = nullptr;
    tCIDLib::TCh* pszCtx = nullptr;
    tCIDLib::TCh* pszTokenize = m_kstrDefLoggerInfo.pszReplicate();
    TJanitor<tCIDLib::TCh> janTokenize(pszTokenize);
    tCIDLib::TCh* pszTmp = TRawStr::pszStrTokenize(pszTokenize, L";", &pszCtx);
    if (pszTmp)
    {
        TRawStr::CopyStr(szFileName, pszTmp, c4MaxBufSz);

        //
        //  The next two are optional so we don't fail if they are not
        //  there. But the format string can be malformed.
        //
        pszTmp = TRawStr::pszStrTokenize(nullptr, L";", &pszCtx);
        if (pszTmp)
        {
            if (pszTmp[0])
            {
                if (TRawStr::bCompareStrI(pszTmp, L"UTF-8"))
                    ptcvtToUse = new TUTFConverter(TUTFConverter::EEncodings::UTF8);
                else if (TRawStr::bCompareStrI(pszTmp, L"UTF-16"))
                    ptcvtToUse = new TUTFConverter(TUTFConverter::EEncodings::Def16);
            }

            // And check for a mutex name
            pszTmp = TRawStr::pszStrTokenize(nullptr, L";", &pszCtx);
            if (pszTmp)
                TRawStr::CopyStr(szMutexName, pszTmp, c4MaxBufSz);
        }

        // If not explicit converter, then use UTF8
        if (!ptcvtToUse)
            ptcvtToUse = new TUTFConverter(TUTFConverter::EEncodings::UTF8);

        //
        //  According to whether we have a mutex name, lets create the
        //  resource name object or not.
        //
        TResourceName rsnTmp(L"CharmedQuark", L"CIDLib", szMutexName);
        const TResourceName* prsnMutex
        (
            szMutexName[0] ? &rsnTmp : &TResourceName::Nul_TResourceName()
        );

        try
        {
            //
            //  If no mutex, then assume it's for exclusive use and set
            //  the access to exclusive out stream, which lets us write
            //  and others read, but only we can write.
            //
            TTextFileLogger* plgrNew = new TTextFileLogger
            (
                szFileName
                , szMutexName[0] ? tCIDLib::EAccessModes::Write
                                : tCIDLib::EAccessModes::Excl_OutStream
                , ptcvtToUse
                , *prsnMutex
            );

            // Set the rollover size to something reasonable
            plgrNew->c8RolloverSize(0x10000);

            //
            //  We have to do this (somewhat redundantly) ourself. We cannot
            //  call SetLogger() because we would deadlock, and we can't release
            //  the lock until we have the new logger set.
            //
            //  But we know there's no current logger or we wouldn't have gotten
            //  called here, so we don't have to release one.
            //
            //  However, there's a remote possibility that there was a new one
            //  set that wasn't stored yet, so deal with that.
            //
            if (m_plgrNew)
            {
                if (m_eAdoptNew == tCIDLib::EAdoptOpts::Adopt)
                {
                    m_eAdoptNew = tCIDLib::EAdoptOpts::NoAdopt;
                    try { delete m_plgrNew; } catch(...) {}
                }
                m_plgrNew = nullptr;
            }

            m_plgrTarget = plgrNew;
            m_eAdopt = tCIDLib::EAdoptOpts::Adopt;
        }

        catch(...)
        {
        }
    }
}




// And now we need a lazy faulting in method for our spooling thread
static TLogSpoolThread* pthrSpooler()
{
    static TLogSpoolThread* pthrSpooler = nullptr;
    if (!pthrSpooler)
    {
        TLocker lockrSync(CIDLib_Module::pmtxLogSync());
        if (!pthrSpooler)
        {
            pthrSpooler = new TLogSpoolThread();
            pthrSpooler->Start();
        }
    }
    return pthrSpooler;
}



// ---------------------------------------------------------------------------
//   CLASS: TModule
//  PREFIX: mod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TModule: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TModule::bLoadOSMsg(const tCIDLib::TOSErrCode errcId, TString& strMsgBuf)
{
    const tCIDLib::TCard4 c4BufSz = 1023;
    tCIDLib::TCh achMsg[c4BufSz + 1];

    if (!TKrnlError::bQuerySysErrMsg(errcId, achMsg, c4BufSz))
    {
        strMsgBuf = CIDLib_Module::pszStrLoadFailure;
        strMsgBuf.eReplaceToken(TCardinal(errcId), L'1');
        strMsgBuf.eReplaceToken(TCardinal(TKrnlError::kerrLast().errcId()), L'2');
        return kCIDLib::False;
    }

    // Copy the text back to the string
    strMsgBuf = achMsg;

    return kCIDLib::True;
}


tCIDLib::TVoid
TModule::BuildModName(  const   TStringView&        strvBaseName
                        , const tCIDLib::TCard4     c4MajVersion
                        , const tCIDLib::TCard4     c4MinVersion
                        , const tCIDLib::EModTypes  eModType
                        ,       TString&            strPortable
                        ,       TString&            strLoadable)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently.
    //
    TKrnlString kstrPortable;
    TKrnlString kstrLoadable;

    if (!TKrnlModule::bBuildModNames(strvBaseName.pszBuffer()
                                    , kstrPortable
                                    , kstrLoadable
                                    , c4MajVersion
                                    , c4MinVersion
                                    , eModType))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(strvBaseName)
        );
    }

    strPortable = kstrPortable.pszValue();
    strLoadable = kstrLoadable.pszValue();
}

tCIDLib::TVoid
TModule::BuildModName(  const   TCIDModuleInfo&     modiSrc
                        ,       TString&            strPortable
                        ,       TString&            strLoadable)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently.
    //
    TKrnlString kstrPortable;
    TKrnlString kstrLoadable;
    if (!TKrnlModule::bBuildModNames(modiSrc.strBaseName().pszBuffer()
                                    , kstrPortable
                                    , kstrLoadable
                                    , modiSrc.c4MajVersion()
                                    , modiSrc.c4MinVersion()
                                    , modiSrc.eModType()))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , modiSrc.strBaseName()
        );
    }

    // Just copy the result back to the caller's string
    strPortable = kstrPortable.pszValue();
    strLoadable = kstrLoadable.pszValue();
}


tCIDLib::TVoid
TModule::BuildModName(  const   TStringView&        strvBaseName
                        , const tCIDLib::TCard4     c4MajVersion
                        , const tCIDLib::TCard4     c4MinVersion
                        , const tCIDLib::EModTypes  eModType
                        ,       TString&            strPortable
                        ,       TString&            strLoadable
                        ,       TString&            strPrefLangMsgs
                        ,       TString&            strDefLangMsgs)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently.
    //
    TKrnlString kstrPortable;
    TKrnlString kstrLoadable;
    if (!TKrnlModule::bBuildModNames(strvBaseName.pszBuffer()
                                    , kstrPortable
                                    , kstrLoadable
                                    , c4MajVersion
                                    , c4MinVersion
                                    , eModType))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(strvBaseName)
        );
    }

    // Copy the result back to the caller's string
    strPortable = tCIDLib::ForceMove(kstrPortable);
    strLoadable = tCIDLib::ForceMove(kstrLoadable);

    //
    //  Now lets fill in the prefered and default language versions of the
    //  associated message file name. It uses the portable base name, plus the
    //  language suffix and the CIDMsg extension.
    //
    TPathStr pathTmp = strPortable;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffix());
    pathTmp.AppendExt(L"CIDMsgs");
    strPrefLangMsgs = pathTmp;

    pathTmp = strPortable;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strDefLanguageSuffix());
    pathTmp.AppendExt(L"CIDMsgs");
    strDefLangMsgs = pathTmp;
}

tCIDLib::TVoid
TModule::BuildModName(  const   TCIDModuleInfo& modiSrc
                        ,       TString&        strPortable
                        ,       TString&        strLoadable
                        ,       TString&        strPrefLangMsgs
                        ,       TString&        strDefLangMsgs)
{
    // Just call the other version
    BuildModName
    (
        modiSrc.strBaseName()
        , modiSrc.c4MajVersion()
        , modiSrc.c4MinVersion()
        , modiSrc.eModType()
        , strPortable
        , strLoadable
        , strPrefLangMsgs
        , strDefLangMsgs
    );
}

tCIDLib::TVoid
TModule::BuildModName(  const   TStringView&        strvBaseName
                        , const tCIDLib::TCard4     c4MajVersion
                        , const tCIDLib::TCard4     c4MinVersion
                        , const tCIDLib::EModTypes  eModType
                        , const tCIDLib::ELanguages ePrefLanguage
                        , const tCIDLib::ELanguages eDefLanguage
                        ,       TString&            strPortable
                        ,       TString&            strLoadable
                        ,       TString&            strPrefLangMsgs
                        ,       TString&            strDefLangMsgs
                        ,       TString&            strEnLangMsgs)
{
    //
    //  We have to pass this on to the CIDKernel, because each platform
    //  will do this differently.
    //
    TKrnlString kstrPortable;
    TKrnlString kstrLoadable;
    if (!TKrnlModule::bBuildModNames(strvBaseName.pszBuffer()
                                    , kstrPortable
                                    , kstrLoadable
                                    , c4MajVersion
                                    , c4MinVersion
                                    , eModType))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_CantBuildName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(strvBaseName)
        );
    }

    // Copy the result back to the caller's string
    strPortable = kstrPortable.pszValue();
    strLoadable = kstrLoadable.pszValue();

    //
    //  Now lets fill in the prefered, default, and english language
    //  versions of the associated message file name. It uses the portable
    //  module name, plus the language suffix and the CIDMsg extension.
    //
    TPathStr pathTmp = strPortable;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffixFor(ePrefLanguage));
    pathTmp.AppendExt(L"CIDMsg");
    strPrefLangMsgs = pathTmp;

    pathTmp = strPortable;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffixFor(eDefLanguage));
    pathTmp.AppendExt(L"CIDMsg");
    strDefLangMsgs = pathTmp;

    pathTmp = strPortable;
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_");
    pathTmp.Append(TLocale::strLanguageSuffixFor(tCIDLib::ELanguages::English));
    pathTmp.AppendExt(L"CIDMsg");
    strEnLangMsgs = pathTmp;
}


tCIDLib::TVoid
TModule::BuildModName(  const   TCIDModuleInfo&     modiSrc
                        , const tCIDLib::ELanguages ePrefLanguage
                        , const tCIDLib::ELanguages eDefLanguage
                        ,       TString&            strPortable
                        ,       TString&            strLoadable
                        ,       TString&            strPrefLangMsgs
                        ,       TString&            strDefLangMsgs
                        ,       TString&            strEnLangMsgs)
{
    // Just call the other version
    BuildModName
    (
        modiSrc.strBaseName()
        , modiSrc.c4MajVersion()
        , modiSrc.c4MinVersion()
        , modiSrc.eModType()
        , ePrefLanguage
        , eDefLanguage
        , strPortable
        , strLoadable
        , strPrefLangMsgs
        , strDefLangMsgs
        , strEnLangMsgs
    );
}


tCIDLib::TCard8
TModule::c8ParseVersionStr( const   TStringView&        strvToParse
                            , const tCIDLib::TBoolean   bFullFormat)
{
    // Call the other version to get the values out
    tCIDLib::TCard4 c4Maj = 0;
    tCIDLib::TCard4 c4Min = 0;
    tCIDLib::TCard4 c4Revision = 0;
    if (bFullFormat)
        ParseVersionStr(strvToParse, c4Maj, c4Min, c4Revision);
     else
        ParseVersionStr(strvToParse, c4Maj, c4Min);

    tCIDLib::TCard8 c8Ver = c4Maj;
    c8Ver <<= 16;
    c8Ver |= c4Min;
    c8Ver <<= 32;
    c8Ver |= c4Revision;

    return c8Ver;
}


//
//  We just drop the logger. If we adopted the existing one (if there is one) we will
//  clean it up.
//
tCIDLib::TVoid TModule::OrphanLogger()
{
    TLocker lockrSync(CIDLib_Module::pmtxLogSync());
    TLogSpoolThread* pthrTar = pthrSpooler();
    pthrTar->SetLogger(nullptr, tCIDLib::EAdoptOpts::NoAdopt);
}


// Set a new logger on the spooler thread
tCIDLib::TVoid
TModule::InstallLogger(MLogger* const plgrToSet, const tCIDLib::EAdoptOpts eAdopt)
{
    TLocker lockrSync(CIDLib_Module::pmtxLogSync());
    pthrSpooler()->SetLogger(plgrToSet, eAdopt);
}



tCIDLib::TVoid TModule::LogEventObj(const TLogEvent& logevToLog)
{
    // Get the severity out since its used a lot below
    tCIDLib::ESeverities eSev = logevToLog.eSeverity();

    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        logevToLog.SetLogged();
        try
        {
            pthrTar->QueueEvent(logevToLog);
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSev);
}


tCIDLib::TVoid TModule::LogEventObj(TLogEvent&& logevToLog)
{
    // Get the severity out since its used a lot below
    tCIDLib::ESeverities eSev = logevToLog.eSeverity();

    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        logevToLog.SetLogged();
        try
        {
            pthrTar->QueueEvent(tCIDLib::ForceMove(logevToLog));
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSev);
}

//
//  This method is designed to log bulk events, generally when they recieved
//  by a logging server from clients. So we don't do much of the stuff that
//  the singular version of this method does. We just log them, but never throw
//  or exit the program or do any runtime dumps, since that wouldn't make any
//  sense in the context of a server handling errors from a remote client. The
//  client is the one who needs to do all of that stuff, in its own context.
//
//  NOTE that the elements are moved to the queue, so the collection will be
//  full of empty elements on return. This is slightly more efficient than just
//  making copies.
//
#pragma warning(suppress : 26460) // Parm can't be const because we move elements from it
tCIDLib::TVoid TModule::LogEventObjs(TCollection<TLogEvent>& colToLog)
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        // Get a cursor for the collection and iterate through it
        TColCursor<TLogEvent>& cursErrs = *colToLog.pcursNew();
        TJanitor<TColCursor<TLogEvent> > janCursor(&cursErrs);

        for (; cursErrs; ++cursErrs)
        {
            cursErrs->SetLogged();
            try
            {
                pthrTar->QueueEvent(tCIDLib::ForceMove(*cursErrs));
            }

            catch(...)
            {
            }
        }
    }
}


tCIDLib::TVoid TModule::ParseVersionStr(const   TStringView&        strvToParse
                                        ,       tCIDLib::TCard4&    c4MajVer
                                        ,       tCIDLib::TCard4&    c4MinVer)
{
    // Find the period, which separates the two parts
    tCIDLib::TCard4 c4Ofs;
    if (!strvToParse.bFirstOccurrence(kCIDLib::chPeriod, c4Ofs))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(strvToParse)
        );
    }

    // Copy out the two parts to a temp and convert to binary
    try
    {
        TString strTmp;
        strvToParse.CopyOutSubStr(strTmp, 0, c4Ofs);
        c4MajVer = strTmp.c4Val();
        strvToParse.CopyOutSubStr(strTmp, c4Ofs + 1, kCIDLib::c4MaxCard);
        c4MinVer = strTmp.c4Val();
    }

    catch(...)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(strvToParse)
        );
    }
}

tCIDLib::TVoid TModule::ParseVersionStr(const   TStringView&        strvToParse
                                        ,       tCIDLib::TCard4&    c4MajVer
                                        ,       tCIDLib::TCard4&    c4MinVer
                                        ,       tCIDLib::TCard4&    c4Revision)
{
    // Find the periods, which separates the threeparts
    tCIDLib::TCard4 c4Ofs1 = 0;
    tCIDLib::TCard4 c4Ofs2 = 0;
    if (!strvToParse.bFirstOccurrence(kCIDLib::chPeriod, c4Ofs1)
    ||  !strvToParse.bLastOccurrence(kCIDLib::chPeriod, c4Ofs2)
    ||  ((c4Ofs2 - c4Ofs1) < 1))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(strvToParse)
        );
    }

    // Copy out the two parts to a temp and convert to binary
    try
    {
        TString strTmp;
        strvToParse.CopyOutSubStr(strTmp, 0, c4Ofs1);
        c4MajVer = strTmp.c4Val();
        strvToParse.CopyOutSubStr(strTmp, c4Ofs1 + 1, (c4Ofs2 - c4Ofs1) - 1);
        c4MinVer = strTmp.c4Val();
        strvToParse.CopyOutSubStr(strTmp, c4Ofs2 + 1, kCIDLib::c4MaxCard);
        c4Revision= strTmp.c4Val();
    }

    catch(...)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_BadVersionStr2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(strvToParse)
        );
    }
}



// ---------------------------------------------------------------------------
//  TModule: Constructors and Destructor
// ---------------------------------------------------------------------------
TModule::TModule(const  TCIDModuleInfo&     modiSrc
                , const tCIDLib::TBoolean   bLoad) :

    m_kmodThis()
    , m_pc1Res(nullptr)
{
    DoInit
    (
        modiSrc.strBaseName()
        , modiSrc.c4MajVersion()
        , modiSrc.c4MinVersion()
        , modiSrc.eModType()
        , modiSrc.eFlags()
        , bLoad
        , nullptr
    );
}

TModule::TModule(   const   TStringView&        strvName
                    , const tCIDLib::EModTypes  eModType
                    , const tCIDLib::TCard4     c4MajVer
                    , const tCIDLib::TCard4     c4MinVer
                    , const tCIDLib::EModFlags  eFlags
                    , const tCIDLib::TBoolean   bLoad) :

    m_kmodThis()
    , m_pc1Res(nullptr)
{
    DoInit(strvName, c4MajVer, c4MinVer, eModType, eFlags, bLoad, nullptr);
}

TModule::TModule(   const   TStringView&        strvName
                    , const TStringView&        strFromPath
                    , const tCIDLib::EModTypes  eModType
                    , const tCIDLib::TCard4     c4MajVer
                    , const tCIDLib::TCard4     c4MinVer
                    , const tCIDLib::EModFlags  eFlags) :

    m_kmodThis()
    , m_pc1Res(nullptr)
{
    // Create our other names, then do common init. This is always a load
    DoInit
    (
        strvName, c4MajVer, c4MinVer, eModType, eFlags, kCIDLib::True, &strFromPath
    );
}


//
//  Load an external module on a specific path. This is not for CIDLib facilities
//  but to load other modules that we may need to mess with. They can pass just the
//  name or the full path, as required.
//
TModule::TModule(const  TStringView&        strvPath
                , const tCIDLib::TBoolean   bLoad) :

    m_kmodThis()
    , m_pc1Res(nullptr)
{
    // If not already done, initialize statistics
    InitStats();

    // Load or query from the original full path
    tCIDLib::TBoolean bRes = kCIDLib::False;
    if (bLoad)
        bRes = m_kmodThis.bLoadExternal(strvPath.pszBuffer());
    else
        bRes = m_kmodThis.bQueryExternal(strvPath.pszBuffer());

    if (!bRes)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , bLoad ? kCIDErrs::errcMod_LoadByName : kCIDErrs::errcMod_QueryByName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TString(strvPath)
        );
    }
}


TModule::~TModule()
{
    // Clean up our resource buffer if we loaded one
    try
    {
        delete [] m_pc1Res;
        m_pc1Res = nullptr;
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TModule: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TModule::bHasMessageFile() const
{
    // Just delegate it to the implementation object
    return m_kmodThis.bHasMessageFile();
}


tCIDLib::TBoolean
TModule::bLoadCIDMsg(const  tCIDLib::TMsgId midToLoad
                    ,       TString&        strMsgBuf) const
{
    //
    //  We don't have to replicate this loaded message because assigning it
    //  to the string object effectively does that inside the string ojbect.
    //
    tCIDLib::TBoolean bRet;
    strMsgBuf = m_kmodThis.pszLoadCIDFacMsg(midToLoad, bRet);
    return bRet;
}

tCIDLib::TBoolean
TModule::bLoadCIDMsg(const  tCIDLib::TMsgId     midToLoad
                    ,       TString&            strMsgBuf
                    , const MFormattable&       fmtblToken1
                    , const MFormattable&       fmtblToken2
                    , const MFormattable&       fmtblToken3
                    , const MFormattable&       fmtblToken4) const
{
    //
    //  Lload up the string. If it failed, then we got a default string, so don't
    //  try to do token replacement in that case.
    //
    tCIDLib::TBoolean bRet;
    strMsgBuf = m_kmodThis.pszLoadCIDFacMsg(midToLoad, bRet);

    if (bRet)
    {
        if (!MFormattable::bIsNullObject(fmtblToken1))
            strMsgBuf.eReplaceToken(fmtblToken1, L'1');
        if (!MFormattable::bIsNullObject(fmtblToken2))
            strMsgBuf.eReplaceToken(fmtblToken2, L'2');
        if (!MFormattable::bIsNullObject(fmtblToken3))
            strMsgBuf.eReplaceToken(fmtblToken3, L'3');
        if (!MFormattable::bIsNullObject(fmtblToken4))
            strMsgBuf.eReplaceToken(fmtblToken4, L'4');
    }
    return bRet;
}


tCIDLib::EModTypes TModule::eModType() const
{
    return m_kmodThis.eModType();
}


tCIDLib::TVoid
TModule::ErrorPopUp(const   tCIDLib::TErrCode   errcToDisplay
                    , const TString&            strFileName
                    , const tCIDLib::TCard4     c4Line
                    , const TString&            strAuxMsg)
{
    //
    //  See if we can load the text for the message. If not, then we get
    //  a default message.
    //
    TString strMsg(kCIDLib::pszEmptyZStr, 512);
    bLoadCIDMsg(errcToDisplay, strMsg);

    //
    //  Get a pointer to the correct raw aux message. It is an optional
    //  parameter, so check it for the nul object.
    //
    const tCIDLib::TCh* pszAux = kCIDLib::pszEmptyZStr;
    if (!TString::bIsNullObject(strAuxMsg))
        pszAux = strAuxMsg.pszBuffer();

    TKrnlPopup::Show
    (
        strFileName.pszBuffer()
        , c4Line
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , 0
        , errcToDisplay
        , 0
        , strMsg.pszBuffer()
        , pszAux
    );
}

tCIDLib::TVoid TModule::ErrorPopUp(const TLogEvent& logevToShow)
{
    TKrnlPopup::Show
    (
        logevToShow.strFileName().pszBuffer()
        , logevToShow.c4LineNum()
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , logevToShow.errcId()
        , logevToShow.errcKrnlId()
        , logevToShow.errcHostId()
        , logevToShow.strErrText().pszBuffer()
        , logevToShow.strAuxText().pszBuffer()
    );
}


tCIDLib::TVoid
TModule::KrnlErrorPopUp(const   TString&                strFile
                        , const tCIDLib::TCard4         c4Line
                        , const tCIDLib::TErrCode       errcToDisplay
                        , const TKrnlError&             kerrToDisplay)
{
    // Load the CIDLib error text
    TString strMsg(kCIDLib::pszEmptyZStr, 512);
    bLoadCIDMsg(errcToDisplay, strMsg);

    // Load the kernel error text
    const tCIDLib::TCh* pszKrnlMsg = kmodCIDKernel.pszLoadCIDFacMsg
    (
        kerrToDisplay.errcId()
    );

    TKrnlPopup::Show
    (
        strFile.pszBuffer()
        , c4Line
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , errcToDisplay
        , kerrToDisplay.errcId()
        , kerrToDisplay.errcHostId()
        , strMsg.pszBuffer()
        , pszKrnlMsg
    );
}


const TKrnlModule& TModule::kmodThis() const
{
    return m_kmodThis;
}


tCIDLib::TVoid
TModule::LogKrnlErr(const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToLog
                    , const TKrnlError&             klogevToLog
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
            TString strMsg(kCIDLib::pszEmptyZStr, 128);

            //
            //  Load up the host OS message, if this was caused by an underlying
            //  host error. Note that it sets the string to a default message if
            //  this fails, so we don't do anything if so.
            //
            if (klogevToLog.errcHostId())
                bLoadOSMsg(klogevToLog.errcHostId(), strKrnlMsg);

            // Load the CIDLib error message
            bLoadCIDMsg(errcToLog, strMsg);

            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , errcToLog
                    , klogevToLog
                    , strMsg
                    , strKrnlMsg
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::LogKrnlErr(const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToLog
                    , const TKrnlError&             klogevToLog
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass
                    , const MFormattable&           fmtblToken1
                    , const MFormattable&           fmtblToken2
                    , const MFormattable&           fmtblToken3
                    , const MFormattable&           fmtblToken4) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
            TString strText(kCIDLib::pszEmptyZStr, 256);

            //
            //  Load up the host OS message, if this was caused by an underlying
            //  host error. Note that it sets the string to a default message if
            //  this fails, so we don't do anything if so.
            //
            if (klogevToLog.errcHostId())
                bLoadOSMsg(klogevToLog.errcHostId(), strKrnlMsg);

            // Get a copy of the err text and do the token replacement
            bLoadCIDMsg
            (
                errcToLog
                , strText
                , fmtblToken1
                , fmtblToken2
                , fmtblToken3
                , fmtblToken4
            );

            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , errcToLog
                    , klogevToLog
                    , strText
                    , strKrnlMsg
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}


tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const TString&                strMsg
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , strMsg
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const TString&                strMsg
                , const TString&                strAuxText
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , strMsg
                    , strAuxText
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            TString strMsg(kCIDLib::pszEmptyZStr, 256);
            bLoadCIDMsg(midToLog, strMsg);

            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , strMsg
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const TString&                strAuxText
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            TString strMsg(kCIDLib::pszEmptyZStr, 256);
            bLoadCIDMsg(midToLog, strMsg);

            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , strMsg
                    , strAuxText
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const TString&                strMsg
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass
                , const MFormattable&           fmtblToken1
                , const MFormattable&           fmtblToken2
                , const MFormattable&           fmtblToken3
                , const MFormattable&           fmtblToken4) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            TString strTmp(strMsg, 256);

            // Do parameter replacement
            if (!MFormattable::bIsNullObject(fmtblToken1))
                strTmp.eReplaceToken(fmtblToken1, L'1');
            if (!MFormattable::bIsNullObject(fmtblToken2))
                strTmp.eReplaceToken(fmtblToken2, L'2');
            if (!MFormattable::bIsNullObject(fmtblToken3))
                strTmp.eReplaceToken(fmtblToken3, L'3');
            if (!MFormattable::bIsNullObject(fmtblToken4))
                strTmp.eReplaceToken(fmtblToken4, L'4');

            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , strTmp
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass
                , const MFormattable&           fmtblToken1
                , const MFormattable&           fmtblToken2
                , const MFormattable&           fmtblToken3
                , const MFormattable&           fmtblToken4) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            TString strMsg(kCIDLib::pszEmptyZStr, 256);
            bLoadCIDMsg
            (
                midToLog
                , strMsg
                , fmtblToken1
                , fmtblToken2
                , fmtblToken3
                , fmtblToken4
            );
            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , strMsg
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::LogMsg(const   TString&                strFileName
                , const tCIDLib::TCard4         c4LineNum
                , const tCIDLib::TMsgId         midToLog
                , const TString&                strAuxText
                , const tCIDLib::ESeverities    eSeverity
                , const tCIDLib::EErrClasses    eClass
                , const MFormattable&           fmtblToken1
                , const MFormattable&           fmtblToken2
                , const MFormattable&           fmtblToken3
                , const MFormattable&           fmtblToken4) const
{
    TLogSpoolThread* pthrTar = pthrSpooler();
    if (pthrTar->bHaveLogger())
    {
        try
        {
            TString strMsg(kCIDLib::pszEmptyZStr, 256);
            bLoadCIDMsg
            (
                midToLog, strMsg, fmtblToken1, fmtblToken2, fmtblToken3, fmtblToken4
            );

            pthrTar->QueueEvent
            (
                new CIDLib_Module::TLogQEvent
                (
                    m_strName
                    , strFileName
                    , c4LineNum
                    , strMsg
                    , strAuxText
                    , eSeverity
                    , eClass
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ShowLogFailure(errToCatch);
        }

        catch(...)
        {
            ShowLogFailure(CID_FILE, CID_LINE);
        }
    }

    CheckFatal(eSeverity);
}

tCIDLib::TVoid
TModule::MsgPopUp(  const   TString&        strMsg
                    , const TString&        strFileName
                    , const tCIDLib::TCard4 c4Line
                    , const TString&        strAuxMsg)
{
    const tCIDLib::TCh* pszAux = kCIDLib::pszEmptyZStr;
    if (!TString::bIsNullObject(strAuxMsg))
        pszAux = strAuxMsg.pszBuffer();

    TKrnlPopup::Show
    (
        strFileName.pszBuffer()
        , c4Line
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , 0
        , 0
        , 0
        , strMsg.pszBuffer()
        , pszAux
    );
}


//
//  Locate the raw data for a resource of a given type and/or resource id, within our
//  pre-loaded resource data and return a pointer to it, and it's size. In the first
//  one, we have to match both id resource type and id. In the other one, we just look
//  for the id, and return what resource type it is.
//
//  We we giving back a pointer into our loaded content, so the caller doesn't have to
//  clean it up.
//
const tCIDLib::TCard1*
TModule::pc1FindRes(        tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::TResId     ridToFind
                    , const tCIDLib::EResTypes  eType
                    , const tCIDLib::TBoolean   bErrIfNotFound) const
{
    // If we don't have any resources, then just return a failure
    if (!m_pc1Res)
        return nullptr;

    // Look at the buffer as a resource header
    const tCIDLib::TResHeader* prhFind
                    = reinterpret_cast<const tCIDLib::TResHeader*>(m_pc1Res);

    const tCIDLib::TResItemMap* primFind =
        reinterpret_cast<const tCIDLib::TResItemMap*>(m_pc1Res + sizeof(tCIDLib::TResHeader));

    // Search all of the map items
    for (tCIDLib::TCard4 c4Index = 0; c4Index < prhFind->c4ResCount; c4Index++)
    {
        //
        //  If we find it, then check the type. If the right type, return it,
        //  else keep looking.
        //
        if ((primFind->ridRes == ridToFind) && (primFind->eType == eType))
        {
            c4ToFill = primFind->c4Size;
            return m_pc1Res + primFind->c4At;
        }
        primFind++;
    }

    // Oh well, never found it. If that's bad, then throw here
    if (bErrIfNotFound)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_ResNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strName()
            , TCardinal(ridToFind)
        );
    }
    return nullptr;
}


const tCIDLib::TCard1*
TModule::pc1FindRes(        tCIDLib::EResTypes& eType
                    ,       tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::TResId     ridToFind
                    , const tCIDLib::TBoolean   bErrIfNotFound) const
{
    // If we don't have any resources, then just return a failure
    if (!m_pc1Res)
        return nullptr;

    // Look at the buffer as a resource header
    const tCIDLib::TResHeader* prhFind
                    = reinterpret_cast<const tCIDLib::TResHeader*>(m_pc1Res);

    const tCIDLib::TResItemMap* primFind =
        reinterpret_cast<const tCIDLib::TResItemMap*>(m_pc1Res + sizeof(tCIDLib::TResHeader));

    // Search all of the map items
    for (tCIDLib::TCard4 c4Index = 0; c4Index < prhFind->c4ResCount; c4Index++)
    {
        // If we find it, then give them back the type and pionter
        if (primFind->ridRes == ridToFind)
        {
            eType = primFind->eType;
            c4ToFill = primFind->c4Size;
            return m_pc1Res + primFind->c4At;
        }
        primFind++;
    }

    // Oh well, never found it. If that's bad, then throw here
    if (bErrIfNotFound)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_ResNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strName()
            , TCardinal(ridToFind)
        );
    }
    return nullptr;
}


//
//  Get a function pointer within this module. Used mostly when dynamically loading
//  a module, where we need to get a pointer to a function inside it to call for
//  initialization purposes.
//
tCIDLib::FuncPtr
TModule::pfnGetFuncPtr(const tCIDLib::TSCh* const pszFuncName) const
{
    tCIDLib::FuncPtr pRet;
    if (!m_kmodThis.bGetFuncPtr(pszFuncName, pRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMod_GetFuncPtr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(pszFuncName)
        );
    }
    return pRet;
}


//
//  Load message text in various ways. For the raw pointer type ones, it just reutrns
//  a pointer to the in-place text, the caller is not responsible for cleaning it up.
//
const tCIDLib::TCh*
TModule::pszLoadCIDMsg( const   tCIDLib::TMsgIndex& miSrc
                        , const tCIDLib::TMsgId     midToLoad
                        ,       tCIDLib::TBoolean&  bLoaded) const
{
    return m_kmodThis.pszLoadCIDMsg(miSrc, midToLoad, bLoaded);
}


const tCIDLib::TCh*
TModule::pszLoadCIDMsg( const   tCIDLib::TMsgId     midToLoad
                        ,       tCIDLib::TBoolean&  bLoaded) const
{
    return m_kmodThis.pszLoadCIDFacMsg(midToLoad, bLoaded);
}

const tCIDLib::TCh*
TModule::pszLoadCIDMsg(const tCIDLib::TMsgId midToLoad) const
{
    // Just call the other version with a dummy status
    tCIDLib::TBoolean bDummy;
    return pszLoadCIDMsg(midToLoad, bDummy);
}


TString
TModule::strMsg(const   tCIDLib::TMsgId     midToLoad
                , const tCIDLib::TCard4     c4ExtraSpace) const
{
    // Get the text, put it into string object, and return that by value
    tCIDLib::TBoolean bRet;
    const tCIDLib::TCh* pszMsg = m_kmodThis.pszLoadCIDFacMsg(midToLoad, bRet);
    return TString(pszMsg, c4ExtraSpace);
}

TString TModule::strMsg(const   tCIDLib::TMsgId midToLoad
                        , const MFormattable&   fmtblToken1
                        , const MFormattable&   fmtblToken2
                        , const MFormattable&   fmtblToken3
                        , const MFormattable&   fmtblToken4) const
{
    //
    //  Same as above, but leave some room for tokens. It might still not be enough
    //  and the string has to expand, but we can prevent a lot of expansions without
    //  making the string a lot bigger than it needs to be.
    //
    tCIDLib::TBoolean bRet;
    const tCIDLib::TCh* pszMsg = m_kmodThis.pszLoadCIDFacMsg(midToLoad, bRet);
    TString strRet(pszMsg, 64);

    // And now do token replacement if we got the real msg
    if (bRet)
    {
        if (!MFormattable::bIsNullObject(fmtblToken1))
            strRet.eReplaceToken(fmtblToken1, L'1');
        if (!MFormattable::bIsNullObject(fmtblToken2))
            strRet.eReplaceToken(fmtblToken2, L'2');
        if (!MFormattable::bIsNullObject(fmtblToken3))
            strRet.eReplaceToken(fmtblToken3, L'3');
        if (!MFormattable::bIsNullObject(fmtblToken4))
            strRet.eReplaceToken(fmtblToken4, L'4');
    }
    return tCIDLib::ForceMove(strRet);
}


TString TModule::strLoadableName() const
{
    return TString(m_kmodThis.kstrLoadableName().pszValue());
}


TString TModule::strPath() const
{
    return TString(m_kmodThis.kstrSrcPath().pszValue());
}


// This one may not be set if this is not a CIDLib facility type module
TString TModule::strPortableName() const
{
    if (m_kmodThis.kstrPortableName().pszValue())
        return TString(m_kmodThis.kstrPortableName().pszValue());

    // We just return the loadable name instead
    return TString(m_kmodThis.kstrLoadableName().pszValue());
}


//
//  Throw errors in various ways.
//
tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg(errcToThrow, strTmp);

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , kCIDLib::pszEmptyZStr
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
 }

tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass
                    , const MFormattable&           fmtblToken1
                    , const MFormattable&           fmtblToken2
                    , const MFormattable&           fmtblToken3
                    , const MFormattable&           fmtblToken4) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg
    (
        errcToThrow
        , strTmp
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , kCIDLib::pszEmptyZStr
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}

tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const TString&                strAuxText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg(errcToThrow, strTmp);

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , strAuxText
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}

tCIDLib::TVoid
TModule::ThrowErr(  const   TString&                strFileName
                    , const tCIDLib::TCard4         c4LineNum
                    , const tCIDLib::TErrCode       errcToThrow
                    , const TString&                strAuxText
                    , const tCIDLib::ESeverities    eSeverity
                    , const tCIDLib::EErrClasses    eClass
                    , const MFormattable&           fmtblToken1
                    , const MFormattable&           fmtblToken2
                    , const MFormattable&           fmtblToken3
                    , const MFormattable&           fmtblToken4) const
{
    TString  strTmp(kCIDLib::pszEmptyZStr, 128);

    // Load up the message
    bLoadCIDMsg
    (
        errcToThrow
        , strTmp
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , strTmp
        , strAuxText
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}


tCIDLib::TVoid
TModule::ThrowKrnlErr(  const   TString&                strFileName
                        , const tCIDLib::TCard4         c4LineNum
                        , const tCIDLib::TErrCode       errcToThrow
                        , const TKrnlError&             klogevToThrow
                        , const tCIDLib::ESeverities    eSeverity
                        , const tCIDLib::EErrClasses    eClass) const
{
    TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
    TString strMsg(kCIDLib::pszEmptyZStr, 128);

    //
    //  Load up the host OS message, if this was caused by an underlying
    //  host error. Note that it sets the string to a default message if
    //  this fails, so we don't do anything if so.
    //
    if (klogevToThrow.errcHostId())
        bLoadOSMsg(klogevToThrow.errcHostId(), strKrnlMsg);

    // Load the CIDLib error message
    bLoadCIDMsg(errcToThrow, strMsg);

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , klogevToThrow
        , strMsg
        , strKrnlMsg
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}


tCIDLib::TVoid
TModule::ThrowKrnlErr(  const   TString&                strFileName
                        , const tCIDLib::TCard4         c4LineNum
                        , const tCIDLib::TErrCode       errcToThrow
                        , const TKrnlError&             klogevToThrow
                        , const tCIDLib::ESeverities    eSeverity
                        , const tCIDLib::EErrClasses    eClass
                        , const MFormattable&           fmtblToken1
                        , const MFormattable&           fmtblToken2
                        , const MFormattable&           fmtblToken3
                        , const MFormattable&           fmtblToken4) const
{
    TString strKrnlMsg(kCIDLib::pszEmptyZStr, 128);
    TString strText(kCIDLib::pszEmptyZStr, 256);

    //
    //  Load up the host OS message, if this was caused by an underlying
    //  host error. Note that it sets the string to a default message if
    //  this fails, so we don't do anything if so.
    //
    if (klogevToThrow.errcHostId())
        bLoadOSMsg(klogevToThrow.errcHostId(), strKrnlMsg);

    // Get a copy of the err text and do the token replacement
    bLoadCIDMsg
    (
        errcToThrow
        , strText
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );

    // Create the event object, and throw it
    TLogEvent logevToThrow
    (
        m_strName
        , strFileName
        , c4LineNum
        , errcToThrow
        , klogevToThrow
        , strText
        , strKrnlMsg
        , eSeverity
        , eClass
    );

    //
    //  If its above the trace threshold and not a status message, then
    //  log it now.
    //
    if ((eSeverity != tCIDLib::ESeverities::Status) && bTraceErrs())
        LogEventObj(logevToThrow);

    throw logevToThrow;
}


// ---------------------------------------------------------------------------
//  TModule: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  In some cases, we want to load a third party DLL which isn't one of our facilities
//  so we provide a default that just says yes, log errors, so that we can create
//  instances of this class.
//
tCIDLib::TBoolean TModule::bTraceErrs() const
{
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TModule: Private, static methods
// ---------------------------------------------------------------------------

// Called after each logged event, with the severity, to see if we need to abort
tCIDLib::TVoid TModule::CheckFatal(const tCIDLib::ESeverities eSev)
{
    //
    //  If its at or above the process fatal level, then we need to exit the
    //  program now. We use the standard runtime error code.
    //
    //  If we are in testing mode, then we don't exit since the host process
    //  is just probing to insure that errors are caught and wants to continue
    //  processing.
    //
    if ((eSev >= tCIDLib::ESeverities::ProcFatal)
    &&  (eSev != tCIDLib::ESeverities::Status)
    &&  !TSysInfo::bTestMode())
    {
        TProcess::ExitProcess(tCIDLib::EExitCodes::RuntimeError);
    }
}


// Called if we fail to queue up an event
tCIDLib::TVoid
TModule::ShowLogFailure(const tCIDLib::TCh* const pszFile, const tCIDLib::TCard4 c4Line)
{
    #if CID_DEBUG_ON
    TKrnlPopup::Show
    (
        pszFile
        , c4Line
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszTitle2
        , 0
        , 0
        , 0
        , CIDLib_Module::pszExceptDuringLog
        , kCIDLib::pszEmptyZStr
    );
    #endif
}

tCIDLib::TVoid TModule::ShowLogFailure(const TLogEvent& logevShow)
{
    #if CID_DEBUG_ON
    TKrnlPopup::Show
    (
        logevShow.strFileName().pszBuffer()
        , logevShow.c4LineNum()
        , CIDLib_Module::pszTitle1
        , CIDLib_Module::pszExceptDuringLog
        , logevShow.errcId()
        , 0
        , 0
        , logevShow.strErrText().pszBuffer()
        , logevShow.strAuxText().pszBuffer()
    );
    #endif
}


// ---------------------------------------------------------------------------
//  TModule: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Ths is the common init needed for all of the CIDLib facility loading type
//  ctors. We have all the names and any path stored at this point.
//
tCIDLib::TVoid
TModule::DoInit(const   TStringView&        strvBaseName
                , const tCIDLib::TCard4     c4MajVer
                , const tCIDLib::TCard4     c4MinVer
                , const tCIDLib::EModTypes  eModType
                , const tCIDLib::EModFlags  eFlags
                , const tCIDLib::TBoolean   bLoad
                , const TStringView* const  pstrSrcPath)
{
    tCIDLib::TBoolean bRes = kCIDLib::False;

    // Initialize our stats if not alreayd done
    InitStats();

    //
    //  Init the module handle data member. According to the bLoad parameter
    //  we either need to query or load by name.
    //
    if (bLoad)
    {
        if (pstrSrcPath)
        {
            bRes = m_kmodThis.bLoadFromPath
            (
                strvBaseName.pszBuffer()
                , c4MajVer
                , c4MinVer
                , pstrSrcPath->pszBuffer()
                , eModType
                , eFlags
            );
        }
         else
        {
            bRes = m_kmodThis.bLoadFromName
            (
                strvBaseName.pszBuffer(), c4MajVer, c4MinVer, eModType, eFlags
            );
        }
    }
     else
    {
        // This only uses the name since we are querying something already loaded
        bRes = m_kmodThis.bQueryFromName
        (
            strvBaseName.pszBuffer(), c4MajVer, c4MinVer, eModType, eFlags
        );
    }

    if (!bRes)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , bLoad ? kCIDErrs::errcMod_LoadByName : kCIDErrs::errcMod_QueryByName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TString(strvBaseName)
        );
    }

    //
    //  It worked, so store away the base name. We store our own copy of this because
    //  we use it a lot.
    //
    m_strName = strvBaseName;

    //
    //  Ok, lets try to load up the local strings that are preloaded and
    //  cached. These are attached to the CIDLib module, so we have to make
    //  sure that's what is loading. But it should always be the first module
    //  that loads and uses this class.
    //
    if (!CIDLib_Module::atomInitMsgs)
    {
        if (strvBaseName == L"CIDLib")
            InitMsgs(*this);
    }


        // If it has a resource file, we need to load that
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EModFlags::HasResFile))
    {
        //
        //  All the associated resources are based on the portable name. That should
        //  always be set here since this is only called for CIDLib facility type
        //  modules.
        //
        TPathStr pathResFile(m_kmodThis.kstrSrcPath().pszValue());
        pathResFile.AddLevel(m_kmodThis.kstrPortableName().pszValue());
        pathResFile.AppendExt(L"CIDRes");

        // And load up the resources
        LoadRes(pathResFile);
    }
}


// Initialize our stats if not done already
tCIDLib::TVoid TModule::InitStats()
{
    // If we've not faulted in our stats cache stuff yet, then do that.
    if (!CIDLib_Module::atomInitStats)
    {
        TBaseLock lockInit;
        if (!CIDLib_Module::atomInitStats)
        {
            //
            //  The start time of the app. We just set it here and it
            //  says that way for ever. Any app has to initialize a module
            //  some way or how to do anything useful, so this serves as
            //  start time stamp.
            //
            TStatsCache::RegisterItem
            (
                kCIDLib::pszStat_AppInfo_UpSince
                , tCIDLib::EStatItemTypes::Time
                , CIDLib_Module::sciStartTime
            );
            TStatsCache::SetValue(CIDLib_Module::sciStartTime, TTime::enctNow());

            TStatsCache::RegisterItem
            (
                kCIDLib::pszStat_AppInfo_DroppedLogEvs
                , tCIDLib::EStatItemTypes::Counter
                , CIDLib_Module::sciDroppedLogEvs
            );

            TStatsCache::RegisterItem
            (
                kCIDLib::pszStat_AppInfo_LogErrors
                , tCIDLib::EStatItemTypes::Counter
                , CIDLib_Module::sciLogErrors
            );

            CIDLib_Module::atomInitStats.Set();
        }
    }
}


//
//  Loads up the passed resource file and stores a pointer to the allocated
//  buffer.
//
tCIDLib::TVoid TModule::LoadRes(const TStringView& strvResFile)
{
    TCritSecLocker lockSync(&m_crsSync);
    if (!m_pc1Res)
    {
        // Open a binary file for the resource file
        TBinaryFile flRes(strvResFile);
        flRes.Open
        (
            tCIDLib::EAccessModes::Multi_Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
        );

        const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(flRes.c8CurSize());
        m_pc1Res = new tCIDLib::TCard1[c4Size];
        if (flRes.c4ReadBuffer(m_pc1Res, c4Size) != c4Size)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMod_NoResFile
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , TString(strvResFile)
            );
        }
    }
}



