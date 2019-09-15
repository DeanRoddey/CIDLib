//
// FILE NAME: CIDOrbUC_LogSrvLogger.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/03/2001
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"


// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TLogSrvLogger,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDOrbUC_LogSrvLogger
{
    // -----------------------------------------------------------------------
    //  We'll only keep up to this many events. If we get behind so bad that
    //  we can't keep up, we'll start dropping events. That would be a really
    //  pathological situation since we log to a local file if we can't log
    //  to the log server.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MaxEvents = 128;
};



// ---------------------------------------------------------------------------
//   CLASS: TLogSrvLogger
//  PREFIX: lgr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLogSrvLogger: Constructors and Destructor
// ---------------------------------------------------------------------------
TLogSrvLogger::TLogSrvLogger(const TString& strLocalPath) :

    m_bForceLocal(kCIDLib::False)
    , m_c4DroppedEvents(0)
    , m_colLogQ(tCIDLib::EMTStates::Safe)
    , m_porbcLogger(0)
    , m_strLocalPath(strLocalPath)
    , m_strmBuf(8192)
    , m_thrLogger
      (
        L"TLogSrvLoggerReconnThread"
        , TMemberFunc<TLogSrvLogger>(this, &TLogSrvLogger::eLoggerThread)
      )
{
    // Do one time init
    Initialize();

    //
    //  Start the thread. It will now start trying to connect to the log
    //  server. Generally it will get there before any actual logging is
    //  done after we return. But, if it doesn't, that's no big deal. Any
    //  logged data will be spooled out as soon as it connects anyway.
    //
    m_thrLogger.Start();
}

TLogSrvLogger::~TLogSrvLogger()
{
    Cleanup();
}


// ---------------------------------------------------------------------------
//  TLogSrvLogger: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLogSrvLogger::LogEvent(const TLogEvent& logevToLog)
{
    //
    //  If the queue is full, then we have to drop the new one and not try
    //  to log it.
    //
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < 10; c4Index++)
    {
        // Give it a try
        {
            TMtxLocker mtxlSync(m_colLogQ.pmtxLock());
            if (!m_colLogQ.bIsFull(CIDOrbUC_LogSrvLogger::c4MaxEvents))
            {
                // We got it, so take our slot and get out
                m_colLogQ.objAdd(logevToLog);
                break;
            }
        }

        // <TBD> We should update the queue to handle this more cleanly
        // Be sure the sleep is OUTSIDE the block above!
        TThread::Sleep(5);
    }

    // If we maxed out, then we are backed up bad and had to drop it
    if (c4Index >= 10)
        m_c4DroppedEvents++;
}


// ---------------------------------------------------------------------------
//  TLogSrvLogger: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TLogSrvLogger::bForceLocal() const
{
    return m_bForceLocal;
}

tCIDLib::TBoolean TLogSrvLogger::bForceLocal(const tCIDLib::TBoolean bNew)
{
    //
    //  Sync with the logger thread first. This will insure that the thread
    //  isn't currently logging something via the client proxy, because we'll
    //  know it's at the bottom of it's loop.
    //
    try
    {
        m_thrLogger.WaitSync(10000);
    }

    catch(...)
    {
        // Not much we can do. Set hte flag and go on
        m_bForceLocal = bNew;
        return m_bForceLocal;
    }

    // Set the force local flag
    m_bForceLocal = bNew;
    if (bNew)
    {
        //
        //  We are forcing local log on, so delete the logger and zero
        //  the pointer. This way when we let the logger thread go, we know
        //  he's in local mode and no further access to the orb will occur.
        //
        //  If we let him clean it up, he could still try to delete the
        //  logger after we return.
        //
        try
        {
            delete m_porbcLogger;
            m_porbcLogger = 0;
        }

        catch(TError& errToCatch)
        {
            m_porbcLogger = 0;

            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TError errLog(errToCatch);
                errLog.SetLogged();
                LogEvent(errLog);
            }

            TLogEvent logevFail
            (
                facCIDOrbUC().strName()
                , CID_FILE
                , CID_LINE
                , kOrbUCMsgs::midLgr_FailedFLocal
                , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_FailedFLocal)
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );
            logevFail.SetLogged();
            LogEvent(logevFail);
        }

        catch(...)
        {
            m_porbcLogger = 0;

            TLogEvent logevFail
            (
                facCIDOrbUC().strName()
                , CID_FILE
                , CID_LINE
                , kOrbUCMsgs::midLgr_FailedFLocal
                , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_FailedFLocal)
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );
            logevFail.SetLogged();
            LogEvent(logevFail);
        }
    }

    // And let the thread go
    m_thrLogger.Release();

    // If in verbose mode, drop an object in the queue for this
    if (facCIDOrbUC().bLogInfo())
    {
        TLogEvent logevForce
        (
            facCIDOrbUC().strName()
            , CID_FILE
            , CID_LINE
            , kOrbUCMsgs::midLgr_ForcedLocal
            , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_ForcedLocal)
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );
        logevForce.SetLogged();
        LogEvent(logevForce);
    }
    return m_bForceLocal;
}


tCIDLib::TCard4 TLogSrvLogger::c4DroppedEvents() const
{
    return m_c4DroppedEvents;
}


tCIDLib::TVoid TLogSrvLogger::Cleanup()
{
    // Stop the reconnect thread, and wait for it to completely die
    try
    {
        // Give it a bit to flush out any last messages
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 10; c4Index++)
        {
            {
                TMtxLocker mtxlSync(m_colLogQ.pmtxLock());
                if (m_colLogQ.bIsEmpty())
                    break;
            }
            TThread::Sleep(50);
        }
        m_thrLogger.ReqShutdownSync(10000);
        m_thrLogger.eWaitForDeath(5000);
    }

    catch(...)
    {
        //
        //  It's not really safe to log anything here. We are the logger and
        //  we are obviously having problems.
        //
    }
}


// ---------------------------------------------------------------------------
//  TLogSrvLogger: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes
TLogSrvLogger::eLoggerThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  If the log server isn't there, we don't want to keep trying over
    //  and over if the log events are coming in fairly quickly. So we time
    //  throttle the retries.
    //
    tCIDLib::TEncodedTime enctLast = 0;

    //
    //  And now loop until we are asked to shut down. If we see the client
    //  proxy object pointer go zero, then periodically try to reconnect.
    //
    TLogEvent logevCur;
    while (kCIDLib::True)
    {
        try
        {
            //
            //  Wait a while for content to arrive. We wake up periodically
            //  to check for shutdown requests
            //
            if (m_colLogQ.bGetNextMv(logevCur, 250, kCIDLib::False))
            {
                //
                //  If if the logger pointer is null and it's been more than 5
                //  seconds since we last tried and we aren't in forced local
                //  mode, then see if we can reconnect.
                //
                if (!m_porbcLogger && !m_bForceLocal)
                {
                    tCIDLib::TEncodedTime enctNow = TTime::enctNow();
                    if ((enctNow - enctLast) > (kCIDLib::enctOneSecond * 5))
                    {
                        enctLast = enctNow;

                        // This won't throw, it'll just return a null on failure
                        m_porbcLogger = porbcMakeClient();

                        if (m_porbcLogger)
                        {
                            // And spool any locally saved stuff if required
                            if (m_flLog.bIsOpen())
                                m_flLog.Close();
                            SpoolLocal(*m_porbcLogger);
                        }
                    }
                }

                //
                //  If in forced local mode, then just unconditionally do a
                //  local log. If we don't have a logger object, then also
                //  just log locally because we would have just tried above
                //  to create it if that was appropriate.
                //
                if (m_bForceLocal || !m_porbcLogger)
                {
                    LogLocal(logevCur);
                }
                 else
                {
                    // We may have a logger, so try it. It might fail
                    tCIDLib::TBoolean bLogged = kCIDLib::False;
                    if (m_porbcLogger)
                    {
                        try
                        {
                            m_porbcLogger->LogSingle(logevCur);
                            bLogged = kCIDLib::True;
                        }

                        catch(...)
                        {
                            // Oh well, we failed, so clean up the logger
                            delete m_porbcLogger;
                            m_porbcLogger = 0;

                            // And try a local log
                            LogLocal(logevCur);

                            // If in verbose logging mode, then log it
                            if (facCIDOrbUC().bLogWarnings())
                            {
                                TLogEvent logevFallback
                                (
                                    facCIDOrbUC().strName()
                                    , CID_FILE
                                    , CID_LINE
                                    , kOrbUCMsgs::midLgr_FallingBack
                                    , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_FallingBack)
                                    , tCIDLib::ESeverities::Warn
                                    , tCIDLib::EErrClasses::AppStatus
                                );
                                logevFallback.SetLogged();
                                LogEvent(logevFallback);
                            }
                        }
                    }
                }
            }
        }

        catch(...)
        {
            // Just eat it
        }

        //
        //  See if we've been asked to exit. This will also serve to sync
        //  with any syncing thread. This happens in the 'force local' call,
        //  since he has to make sure we aren't currently logging something
        //  to the log server before he exits, else somene can force local
        //  and return and stop the ORB while we are using the client proxy.
        //
        //  Note that he will delete our logger, so by the time he lets us
        //  go, we are fully in local mode with no logger object.
        //
        if (thrThis.bCheckShutdownRequest())
            break;
    }

    //
    //  We've been asked to exit. If we have a logger, clean it up now before
    //  we go. If the local log file is open, close it.
    //
    if (m_porbcLogger)
    {
        try
        {
            delete m_porbcLogger;
            m_porbcLogger = 0;
        }

        catch(...)
        {
            // Not much we can do here, we are the logger and we are done
        }
    }

    if (m_flLog.bIsOpen())
    {
        try
        {
            m_flLog.Close();
        }

        catch(...)
        {
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::TVoid TLogSrvLogger::Initialize()
{
    //
    //  Set the root part of the local file name, for convenience we pre-calc
    //  it. Its the process name and the process id to deal with multiple
    //  instances of the same process running at the same time.
    //
    m_strLocalRoot = TProcess::strProcessName();
    m_strLocalRoot.Append(L"_");
    m_strLocalRoot.AppendFormatted(TProcess::pidThis());
    m_strLocalRoot.Append(L"_");

    //
    //  Set up the unique name object. We add the extension and replaceable
    //  token for this version.
    //
    m_unamFiles.strText(m_strLocalRoot + L"%(1,5,0).LocalLog");
}


tCIDLib::TVoid TLogSrvLogger::LogLocal(const TLogEvent& logevToDump)
{
    // Make sure the local log file is open
    try
    {
        if (!m_flLog.bIsOpen())
            OpenMostRecent();
    }

    catch(...)
    {
        // <TBD> Can we really do anything here?
        return;
    }

    // Reset our memory stream, and format the object to it
    m_strmBuf.Reset();
    m_strmBuf   << tCIDLib::EStreamMarkers::Frame
                << logevToDump
                << kCIDLib::FlushIt;

    // Now write out the bufffered object
    m_flLog.c4WriteBuffer(m_strmBuf.mbufData(), m_strmBuf.c4CurPos());

    //
    //  If the file has reached the roll over size, then lets close down
    //  this one and open up the next one.
    //
    if (m_flLog.c8CurSize() > kCIDOrbUC::c8LogFlRolloverSz)
    {
        m_flLog.Close();
        m_pathCurLogFile = m_strLocalPath;
        m_pathCurLogFile.AddLevel(m_unamFiles.strQueryNewName());
        m_flLog.strName(m_pathCurLogFile);
        m_flLog.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );
    }
}


TCIDLogSrvClientProxy* TLogSrvLogger::porbcMakeClient()
{
    TCIDLogSrvClientProxy* porbcRet = 0;
    try
    {
        // Use a short timeout since we only want to do this if it is there
        porbcRet = facCIDOrbUC().porbcMakeClient<TCIDLogSrvClientProxy>
        (
            TCIDLogSrvClientProxy::strBinding, 100
        );
    }

    catch(...)
    {
        // Just fall through
    }
    return porbcRet;
}


tCIDLib::TVoid TLogSrvLogger::OpenMostRecent()
{
    // Reset the sequence on the unique name member
    m_unamFiles.c4SetSequenceNum(0);

    // Set up a wild card that will match our process' local log files.
    TPathStr pathSearch(m_strLocalPath);
    pathSearch.AddLevel(m_strLocalRoot + TString(L"*.LocalLog"));

    // See if find any at all
    TDirIter    diterLogs;
    TFindBuf    fndbCur;
    if (!diterLogs.bFindFirst(pathSearch, fndbCur))
    {
        //
        //  There aren't any, so just gen up the first unique file name and
        //  return. Make sure the path exists before we try to create the
        //  file.
        //
        m_pathCurLogFile = m_strLocalPath;
        TFileSys::MakePath(m_pathCurLogFile);
        m_pathCurLogFile.AddLevel(m_unamFiles.strQueryNewName());

        m_flLog.strName(m_pathCurLogFile);
        m_flLog.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateIfNew
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );
        return;
    }

    //
    //  There are some files, so keep looking until we find the one with
    //  the biggest number. We don't assume that the file system is sorted,
    //  or that if it is that the collation would make what we want to happen
    //  happen. So we pull out the number of each and convert the number to
    //  binary for the comparison.
    //
    tCIDLib::TCard4 c4Biggest = 0;
    tCIDLib::TCard8 c8Biggest = 0;
    tCIDLib::TCard4 c4CurValue;
    do
    {
        // Pull the number out of the file name
        fndbCur.pathFileName().bQueryName(pathSearch);
        pathSearch.Cut(0, m_strLocalRoot.c4Length());

        try
        {
            c4CurValue = pathSearch.c4Val(tCIDLib::ERadices::Dec);
        }

        catch(...)
        {
            // Wasn't legal for some reason, just keep going
            continue;
        }

        //
        //  If its bigger than the biggest, then store it as the new biggest.
        //  And save the file size as well, for later use.
        //
        if (c4CurValue > c4Biggest)
        {
            c4Biggest = c4CurValue;
            c8Biggest  = fndbCur.c8Size();
        }

    }   while (diterLogs.bFindNext(fndbCur));

    //
    //  Lets see now big this one is. If its less than re-open size, then
    //  lets keep it. Else, we'll just move on to the next one.
    //
    if (c8Biggest < kCIDOrbUC::c8LogFlReopenSz)
    {
        //
        //  Set the unique name to the current biggest so it'll create its
        //  first name as that biggest numbered file.
        //
        m_unamFiles.c4SetSequenceNum(c4Biggest);

        // Create the name of the current biggest file
        m_pathCurLogFile = m_strLocalPath;
        m_pathCurLogFile.AddLevel(m_unamFiles.strQueryNewName());

        // And now make this file the current output stream
        m_flLog.strName(m_pathCurLogFile);
        m_flLog.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );

        // Seek to the end for writing
        m_flLog.SetFilePos(m_flLog.c8CurSize());
    }
     else
    {
        //
        //  Set the unique name to the current biggest plus 1, so it'll start
        //  creating names after the current biggest numbered one.
        //
        m_unamFiles.c4SetSequenceNum(c4Biggest + 1);

        // Create the name of the new file
        m_pathCurLogFile = m_strLocalPath;
        m_pathCurLogFile.AddLevel(m_unamFiles.strQueryNewName());

        // And now create this new file
        m_flLog.strName(m_pathCurLogFile);
        m_flLog.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateIfNew
            , tCIDLib::EFilePerms::AllOwnerAccess
            , tCIDLib::EFileFlags::SequentialScan
        );
    }
}


tCIDLib::TVoid TLogSrvLogger::SpoolLocal(TCIDLogSrvClientProxy& orbcToUse)
{
    //
    //  We will spool up old files from any instances of our own process, not
    //  just own own. And some of them might be from previous instances of
    //  a singleton app, since we change PIDs each time.
    //
    TPathStr pathSearch = m_strLocalPath;
    pathSearch.AddLevel(TProcess::strProcessName());
    pathSearch.Append(L"_*.LocalLog");

    //
    //  Create a bag to hold the files we find. Make it a sorted by, sorted
    //  by last modify time, ascending. If there are more than a few, we'll
    //  delete our way up through the list till we get to the lastest ones,
    //  then spool them out from oldest to most recent.
    //
    TSortedBag<TFindBuf> colFiles
    (
        TFindBuf::eCompByLastMod
        , tCIDLib::ESortDirs::Ascending
        , tCIDLib::EMTStates::Unsafe
    );

    // If there aren't any, then return
    if (!TFileSys::c4SearchDir(pathSearch, colFiles, tCIDLib::EDirSearchFlags::NormalFiles))
        return;

    // If in verbose logging mode, then log this
    if (facCIDOrbUC().bLogInfo())
    {
        facCIDOrbUC().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbUCMsgs::midLgr_SpoolingLocal
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );
    }

    try
    {
        //
        //  Iterate the collection and spool up each one, then delete it. If
        //  there are more than 3, then delete the older ones and just spool
        //  the last three.
        //
        tCIDLib::TCard4 c4Count = colFiles.c4ElemCount();
        TSortedBag<TFindBuf>::TCursor cursFiles(&colFiles);
        for (; cursFiles; ++cursFiles)
        {
            const TString& strName = cursFiles->pathFileName();
            if (c4Count > 3)
            {
                //
                //  The delete could fail, if this is an active local
                //  log from another instance of this program. We just
                //  ignore the error in that case.
                //
                try
                {
                    TFileSys::DeleteFile(strName);
                }

                catch(...)
                {
                }
            }
             else
            {
                // We are down to the last three, so spool it out
                SpoolOut(orbcToUse, strName);
            }
            c4Count--;
        }
    }

    catch(...)
    {
        // Too dangerous to try to log here, just throw our own error
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcLSLgr_SpoolErr
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }

    //
    //  And now get ready for any subsequent local logging by finding again
    //  the local file to use. If we failed anywhere above, then we'll probably
    //  just end up on the file we were on to start with, unless its at the
    //  rollover size, in which case we'll move up one.
    //
    OpenMostRecent();
}


tCIDLib::TVoid
TLogSrvLogger::SpoolOut(        TCIDLogSrvClientProxy&  orbcToUse
                        , const TString&                strSrcFile)
{
    //
    //  Create an input stream for this file. This might fail becuase the
    //  file could be one that is open in another instance. If so, then just
    //  leave it. We try to open it in the same mode, so that we are sure to
    //  get rejected if it's open.
    //
    try
    {
        TBinFileInStream strmSrc
        (
            strSrcFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_ReadWrite
        );

        // And call the other version
        SpoolOut(orbcToUse, strmSrc);
    }

    catch(...)
    {
    }
}


tCIDLib::TVoid
TLogSrvLogger::SpoolOut(TCIDLogSrvClientProxy&  orbcToUse
                        , TBinFileInStream&     strmSrc)
{
    const tCIDLib::TCard8 c8Size = strmSrc.c8CurSize();

    TLogEvent logevCur;
    while (strmSrc.c8CurPos() < c8Size)
    {
        // There should be a frame marker between them
        strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);

        // And now stream in the even itself
        strmSrc >> logevCur;

        // Now send it to the log server
        orbcToUse.LogSingle(logevCur);
    }

    //
    //  And we've successfully spooled this one, so delete it. Get the name
    //  before we close it!
    //
    const TString strToDelete = strmSrc.strFileName();
    strmSrc.Close();
    TFileSys::DeleteFile(strToDelete);
}

