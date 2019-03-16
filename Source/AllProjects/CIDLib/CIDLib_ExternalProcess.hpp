//
// FILE NAME: CIDLib_ExternalProcess.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1997
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
//  This is the header for the CIDLib_ExternalProcess.Cpp file. This file
//  implements the TExternalProcess class. This class allows you to execute
//  other processes and control them.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward declarations
// ---------------------------------------------------------------------------
template <class TString> class TCollection;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TExternalProcess
//  PREFIX: extp
// ---------------------------------------------------------------------------
class CIDLIBEXP TExternalProcess : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard4 c4BreakOutParms
        (
            const   TString&                strToParse
            ,       tCIDLib::TStrCollect&   colToFill
        );


        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TExternalProcess();

        TExternalProcess(const TExternalProcess&) = delete;

        ~TExternalProcess();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TExternalProcess& operator=(const TExternalProcess&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEverStarted() const;

        tCIDLib::TBoolean bIsRunning() const;

        tCIDLib::TBoolean bTryAttachTo
        (
            const   TString&                strAttachInfo
        );

        tCIDLib::TBoolean bWaitForDeath
        (
                    tCIDLib::EExitCodes&    eToFill
            , const tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::EPrioClasses ePriorityClass() const;

        tCIDLib::EPrioClasses ePriorityClass
        (
            const   tCIDLib::EPrioClasses   eNewClass
        );

        tCIDLib::EExitCodes eWaitForDeath
        (
            const   tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TVoid FormatAttachData
        (
                    TString&                strToFill
        );

        TKrnlExtProcess& kextpThis();

        const TKrnlExtProcess& kextpThis() const;

        tCIDLib::TVoid Kill();

        tCIDLib::TProcessId pidThis() const;

        tCIDLib::TVoid QueryFullPath
        (
                    TString&                strToFill
        );

        tCIDLib::TVoid QueryProcessTimes
        (
                    TTime&                  tmStart
            ,       TTime&                  tmEnd
            ,       TTime&                  tmKernelTime
            ,       TTime&                  tmUserTime
        );

        tCIDLib::TVoid Start
        (
            const   TString&                strCmdLine
            , const TString&                strInitPath
            , const tCIDLib::EExtProcFlags  eFlag = tCIDLib::EExtProcFlags::NewProcGroup
            , const tCIDLib::EExtProcShows  eShow = tCIDLib::EExtProcShows::Normal
        );

        tCIDLib::TVoid Start
        (
            const   TString&                strCmdLine
            , const TString&                strInitPath
            , const TEnvironment&           envToUse
            , const tCIDLib::EExtProcFlags  eFlag = tCIDLib::EExtProcFlags::NewProcGroup
            , const tCIDLib::EExtProcShows  eShow = tCIDLib::EExtProcShows::Normal
        );

        tCIDLib::TVoid Start
        (
            const   TString&                strPath
            , const TString&                strInitPath
            , const tCIDLib::TStrCollect&   colParms
            , const tCIDLib::EExtProcFlags  eFlag = tCIDLib::EExtProcFlags::NewProcGroup
            , const tCIDLib::EExtProcShows  eShow = tCIDLib::EExtProcShows::Normal
        );

        tCIDLib::TVoid Start
        (
            const   TString&                strPath
            , const TString&                strInitPath
            , const tCIDLib::TStrCollect&   colParms
            , const TEnvironment&           envToUse
            , const tCIDLib::EExtProcFlags  eFlag = tCIDLib::EExtProcFlags::NewProcGroup
            , const tCIDLib::EExtProcShows  eShow = tCIDLib::EExtProcShows::Normal
        );

        tCIDLib::TVoid StartAsOpen
        (
            const   TString&                strDocFile
            , const tCIDLib::EExtProcShows  eShow = tCIDLib::EExtProcShows::Normal
            , const tCIDLib::TBoolean       bAdminPrivs = kCIDLib::False
        );

        tCIDLib::TVoid SystemEscape
        (
            const   TString&                strCmdLine
            , const tCIDLib::ESysEscFlags   eFlags = tCIDLib::ESysEscFlags::DropOut
        );

        tCIDLib::TVoid WaitInit
        (
            const   tCIDLib::TCard4         c4WaitFor
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kextpThis
        //      This is the kernel abstraction for an external process we
        //      do most of our work via this object. It keeps platform
        //      details down in the kernel where it belongs.
        // -------------------------------------------------------------------
        TKrnlExtProcess     m_kextpThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TExternalProcess, TObject)
};

#pragma CIDLIB_POPPACK

