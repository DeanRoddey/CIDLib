//
// FILE NAME: CIDLib_FileLogger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1996
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
//  This is the header file for the CIDLib_FileLogger.Cpp file. This
//  file provides a simple implementation of MLogger, and supports logging
//  to a binary output file. It optionally supports a named mutex to
//  control logging by multiple processes. It does not have to worry about
//  multiple threads within the same process because calls to LogError()
//  are already synchronized by the calling thread in TModule.
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
//  CLASS: TFileLogger
// PREFIX: lgr
// ---------------------------------------------------------------------------
class CIDLIBEXP TFileLogger : public TObject, public MLogger
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFileLogger();

        TFileLogger
        (
            const   tCIDLib::TCh* const     pszFile
            , const TResourceName&          rsnMutex = TResourceName::Nul_TResourceName()
        );

        TFileLogger
        (
            const   TString&                strFile
            , const TResourceName&          rsnMutex = TResourceName::Nul_TResourceName()
        );

        TFileLogger(const TFileLogger&) = delete;

        ~TFileLogger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFileLogger& operator=(const TFileLogger&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LogEvent
        (
            const   TLogEvent&              logevToLog
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TString&                strFile
            , const TResourceName&          rsnMutex
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_flTarget
        //      This is a file object for the target file.
        //
        //  m_pmtxSync
        //      This is the optional named mutex that is used to sync
        //      multiple processes logging to the same file. If one was not
        //      provided, then no cross process synchronization is used.
        // -------------------------------------------------------------------
        TBinaryFile     m_flTarget;
        TMutex*         m_pmtxSync;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFileLogger,TObject)
};

#pragma CIDLIB_POPPACK


