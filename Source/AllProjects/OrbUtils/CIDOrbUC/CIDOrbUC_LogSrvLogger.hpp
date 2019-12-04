//
// FILE NAME: CIDOrbUC_LogSrvLogger.hpp
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
//  This file implements a logger object (implements the MLogger interface)
//  that logs to the CIDLib Log Server via its ORB interface.
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
        TLogSrvLogger(TLogSrvLogger&&) = delete;

        ~TLogSrvLogger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLogSrvLogger& operator=(const TLogSrvLogger&) = delete;
        TLogSrvLogger& operator=(TLogSrvLogger&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LogEvent
        (
            const   TLogEvent&              logevToLog
        )   final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bForceLocal() const;

        tCIDLib::TBoolean bForceLocal
        (
            const   tCIDLib::TBoolean       bNew
        );

        tCIDLib::TVoid Cleanup();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
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
        //  m_enctLast
        //      If the log server isn't there, we don't want to keep trying over
        //      and over if the log events are coming in fairly quickly. So we time
        //      throttle the retries.
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
        //  m_unamFiles
        //      We use the standard unique name class to generate new file
        //      names for the local log files we use if we can't get to the
        //      log server.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bForceLocal;
        tCIDLib::TEncodedTime   m_enctLast;
        TBinaryFile             m_flLog;
        TPathStr                m_pathCurLogFile;
        TCIDLogSrvClientProxy*  m_porbcLogger;
        TString                 m_strLocalPath;
        TPathStr                m_strLocalRoot;
        TBinMBufOutStream       m_strmBuf;
        TUniqueName             m_unamFiles;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TLogSrvLogger,TObject)
};

#pragma CIDLIB_POPPACK

