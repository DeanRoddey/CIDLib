//
// FILE NAME: CIDLib_TextFileLogger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1996
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
//  This is the header file for the CIDLib_TextFileLogger.Cpp file. This
//  file provides a simple implementation of MLogger, and supports logging
//  to a text output file. It optionally supports a named semaphore to
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


class TTextFileOutStream;

// ---------------------------------------------------------------------------
//  CLASS: TTextFileLogger
// PREFIX: lgr
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextFileLogger : public TObject, public MLogger
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TTextFileLogger();

        TTextFileLogger
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::EAccessModes   eAccess
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
            , const TResourceName&          rsnMutex = TResourceName::Nul_TResourceName()
        );

        TTextFileLogger
        (
            const   TString&                strFile
            , const tCIDLib::EAccessModes   eAccess
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
            , const TResourceName&          rsnMutex = TResourceName::Nul_TResourceName()
        );

        TTextFileLogger(const TTextFileLogger&) = delete;

        ~TTextFileLogger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextFileLogger& operator=(const TTextFileLogger&) = delete;


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
        tCIDLib::TCard8 c8RolloverSize() const;

        tCIDLib::TCard8 c8RolloverSize
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::TCh* const     pszFile
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
            , const TResourceName&          rsnMutex = TResourceName::Nul_TResourceName()
        );

        tCIDLib::TVoid Open
        (
            const   TString&                strFile
            ,       TTextConverter* const   ptcvtToAdopt = nullptr
            , const TResourceName&          rsnMutex = TResourceName::Nul_TResourceName()
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNeedsBOM
        //      We check the converter's encoding and remember if we need to
        //      add a BOM to new files.
        //
        //  m_c8RolloverSize
        //      The size at which the current file should be closed and a
        //      new one started. This only works for non-shared files, since
        //      it will try to rename the current file to a backup name and
        //      start a new file. So, by default, it is set to the maximum
        //      file size so that rollover won't happen.
        //
        //  m_eAccess
        //      The file access requested. We need this because if we do file
        //      rollover, we have to create a new file and want to give it
        //      the same access.
        //
        //  m_pstrmTarget
        //      This is a text stream object for the target file. Errors
        //      are formatted to this stream.
        //
        //  m_pmtxSync
        //      This is the optional named mutex that is used to sync
        //      multiple processes logging to the same file. If one was not
        //      provided, then no cross process synchronization is used.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bNeedsBOM;
        tCIDLib::TCard8         m_c8RolloverSize;
        tCIDLib::EAccessModes   m_eAccess;
        TTextFileOutStream*     m_pstrmTarget;
        TMutex*                 m_pmtxSync;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextFileLogger,TObject)
};

#pragma CIDLIB_POPPACK

