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
//   CLASS: TLogSrvLogger
//  PREFIX: lgr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLogSrvLogger: Constructors and Destructor
// ---------------------------------------------------------------------------
TLogSrvLogger::TLogSrvLogger(const TString& strLocalPath) :

    m_bForceLocal(kCIDLib::False)
    , m_enctLast(0)
    , m_porbcLogger(nullptr)
    , m_strLocalPath(strLocalPath)
    , m_strmBuf(8192)
{
    // Do one time init
    Initialize();
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
    try
    {
        //
        //  If if the logger pointer is null and it's been more than 5
        //  seconds since we last tried and we aren't in forced local
        //  mode, then see if we can reconnect.
        //
        if (!m_porbcLogger && !m_bForceLocal)
        {
            tCIDLib::TEncodedTime enctNow = TTime::enctNow();
            if ((enctNow - m_enctLast) > (kCIDLib::enctOneSecond * 5))
            {
                m_enctLast = enctNow;

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
            LogLocal(logevToLog);
        }
        else
        {
            // We may have a logger, so try it. It might fail
            tCIDLib::TBoolean bLogged = kCIDLib::False;
            if (m_porbcLogger)
            {
                try
                {
                    m_porbcLogger->LogSingle(logevToLog);
                    bLogged = kCIDLib::True;
                }

                catch(...)
                {
                    // Oh well, we failed, so clean up the logger
                    delete m_porbcLogger;
                    m_porbcLogger = nullptr;

                    // And try a local log
                    LogLocal(logevToLog);

                    // If in verbose logging mode, then log it
                    if (facCIDOrbUC().bLogWarnings())
                    {
                        facCIDOrbUC().LogMsg
                        (
                            CID_FILE
                            , CID_LINE
                            , kOrbUCMsgs::midLgr_FallingBack
                            , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_FallingBack)
                            , tCIDLib::ESeverities::Warn
                            , tCIDLib::EErrClasses::AppStatus
                        );
                    }
                }
            }
        }
    }

    catch(...)
    {
    }
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
            m_porbcLogger = nullptr;
        }

        catch(TError& errToCatch)
        {
            m_porbcLogger = nullptr;

            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TError errLog(errToCatch);
                errLog.SetLogged();
                LogEvent(errLog);
            }

            facCIDOrbUC().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbUCMsgs::midLgr_FailedFLocal
                , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_FailedFLocal)
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );
        }

        catch(...)
        {
            m_porbcLogger = nullptr;
            facCIDOrbUC().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbUCMsgs::midLgr_FailedFLocal
                , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_FailedFLocal)
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );
        }
    }

    // If in verbose mode, log this info
    if (facCIDOrbUC().bLogInfo())
    {
        facCIDOrbUC().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbUCMsgs::midLgr_ForcedLocal
            , facCIDOrbUC().strMsg(kOrbUCMsgs::midLgr_ForcedLocal)
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );
    }
    return m_bForceLocal;
}


tCIDLib::TVoid TLogSrvLogger::Cleanup()
{
}


// ---------------------------------------------------------------------------
//  TLogSrvLogger: Private, non-virtual methods
// ---------------------------------------------------------------------------
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
    TCIDLogSrvClientProxy* porbcRet = nullptr;
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
    //  Create a bag to hold the files we find. Make it a sorted bag, sorted
    //  by last modify time, ascending. If there are more than a few, we'll
    //  delete our way up through the list till we get to the lastest ones,
    //  then spool them out from oldest to most recent.
    //
    TSortedBag<TFindBuf> colFiles
    (
        TFindBuf::eCompByLastMod, tCIDLib::ESortDirs::Ascending
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
        for (auto cursFiles = colFiles.cursThis(); cursFiles; ++cursFiles)
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

