//
// FILE NAME: CIDOrbUC_LogSrvLogger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/03/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TLogSrvLogger
//  PREFIX: lgr
// ---------------------------------------------------------------------------
class CIDORBUCEXP TLogSrvLogger : public TObject, public MLogger
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLogSrvLogger() = delete;

        TLogSrvLogger
        (
            const   TString&                strLocalPath
        );

        TLogSrvLogger(const TLogSrvLogger&) = delete;

        ~TLogSrvLogger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLogSrvLogger& operator=(const TLogSrvLogger&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LogEvent
        (
            const   TLogEvent&              logevToLog
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bForceLocal() const;

        tCIDLib::TBoolean bForceLocal
        (
            const   tCIDLib::TBoolean       bNew
        );

        tCIDLib::TCard4 c4DroppedEvents() const;

        tCIDLib::TVoid Cleanup();


    private :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TLogQ = TQueue<TLogEvent>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eLoggerThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid Initialize();

        tCIDLib::TVoid LogLocal
        (
            const   TLogEvent&              logevToDump
        );

        tCIDLib::TVoid OpenMostRecent();

        TCIDLogSrvClientProxy* porbcMakeClient();

        tCIDLib::TVoid SpoolLocal
        (
                TCIDLogSrvClientProxy&      orbcToUse
        );

        tCIDLib::TVoid SpoolOut
        (
                    TCIDLogSrvClientProxy&  orbcToUse
            , const TString&                strSrcFile
        );

        tCIDLib::TVoid SpoolOut
        (
                    TCIDLogSrvClientProxy&  orbcToUse
            ,       TBinFileInStream&       strmSrc
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bForceLocal
        //      Once our host process is about to terminate the Orb support,
        //      it should force us into local log mode. This way we won't
        //      log a bogus 'fall back to local log' message. We'll just
        //      fall back voluntarily. Any messages logged after that will
        //      go to the local log and will be spooled up on the next start.
        //
        //  m_c4DroppedEvents
        //      We keep a count of events we drop because the queue is full.
        //      Shouldn't happen but we want to be able to check.
        //
        //  m_colLogQ
        //      The queue in which events are put till the logger thread gets
        //      them out. It's thread safe and provides all the
        //      synchronization.
        //
        //  m_flLog
        //      The local file to which we are currently logging if we can't
        //      connect to the log server. We format them first into
        //      m_strmBuf, then write them to the file as a chunk.
        //
        //  m_pathCurLogFile
        //      The current local log file we are logging to, if any. If none,
        //      then its empty. Hopefully its always empty, since we are
        //      connected ok to the log server and don't need to do any local
        //      logging.
        //
        //  m_porbcLogger
        //      A pointer to the log server proxy that the logging thread
        //      is using. He will delete/create it as required.
        //
        //  m_strLocalPath
        //      Where we should put our local log files when we can't get to
        //      the log server.
        //
        //  m_strLocalRoot
        //      The root part of the local file name. Its built from the
        //      process name and process id. To avoid having to rebuild it
        //      up again and again, we just store it locally. The full name
        //      includes the replaceable token part, but that's handled by
        //      the m_unamFiles guy.
        //
        //  m_strmBuf
        //      A memory buffer output stream to use to pre-flatten the
        //      log events before writing them locally. This way we can just
        //      dump the whole thing at once to the local file.
        //
        //  m_thrLogger
        //      Events are queued and then spooled out by a background
        //      thread. This guy keeps trying to get us conencted to the log
        //      server, and when connected it pulls items off the m_colLogQ
        //      queue and sends them.
        //
        //  m_unamFiles
        //      We use the standard unique name class to generate new file
        //      names for the local log files we use if we can't get to the
        //      log server.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bForceLocal;
        tCIDLib::TCard4         m_c4DroppedEvents;
        TLogQ                   m_colLogQ;
        TBinaryFile             m_flLog;
        TPathStr                m_pathCurLogFile;
        TCIDLogSrvClientProxy*  m_porbcLogger;
        TString                 m_strLocalPath;
        TPathStr                m_strLocalRoot;
        TBinMBufOutStream       m_strmBuf;
        TThread                 m_thrLogger;
        TUniqueName             m_unamFiles;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TLogSrvLogger,TObject)
};

#pragma CIDLIB_POPPACK

