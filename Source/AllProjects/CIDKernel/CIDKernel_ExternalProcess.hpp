//
// FILE NAME: CIDKernel_ExternalProcess.hpp
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
//  This is the header for the CIDKernel_ExternalProcess.Cpp file. This
//  file implements the TKrnlExtProcess class. This class is an abstraction
//  for manipulating external processes that have been started from this
//  process.
//
//  The CIDLib layer class TExternalProcess uses one of these objects to
//  do its work.
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
//   CLASS: TKrnlExtProcess
//  PREFIX: kextp
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlExtProcess
{
    public  :
        // -------------------------------------------------------------------
        //  Forward declare our per-platform internal structure
        // -------------------------------------------------------------------
        struct TPlatData;


        // -------------------------------------------------------------------
        //  Public, static methods
        //
        //  This method understands how to parse a standard CIDLib command
        //  line format, and break it into separate parameters. This format
        //  is how programs can programmatically execute external processes,
        //  and be sure that parameters are delimited in a standard way. The
        //  rules are:
        //
        //  1)  Whitespace is a delimiter for parameters, so:
        //          myprogram foo bar
        //      will be broken into three parameters
        //
        //  2)  Use single or double quotes if a parameter has spaces, so:
        //          myprogram "parm one" parmtwo
        //      will be broken into three parameters
        //
        //  3)  Quotes are only delimiters, so they only are special if space
        //      is used before and after, so:
        //          myprogram Parm'one two
        //      will only have two paramters, which are "Parm'one" and "two"
        //
        //  4)  To have a quote literally around a parameter, surround it with
        //      the other type of quote, like this:
        //          myprogram '"A quoted String"'
        //
        //  You provide a command line, and you provide an array of pointers
        //  to chars, and its size. It will allocate strings for each param
        //  and store them in apszList[]. It will return the number of entries
        //  it stored. You must clean up these allocated strings when you are
        //  done with them.
        //
        //  It will return zero if the command line is not valid.
        // -------------------------------------------------------------------
        static tCIDLib::TCard4 c4BreakOutParms
        (
            const   tCIDLib::TCh* const     pszCmdLine
            ,       tCIDLib::TCh*           apszList[]
            , const tCIDLib::TCard4         c4MaxParms
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlExtProcess();

        TKrnlExtProcess(const TKrnlExtProcess&) = delete;

        ~TKrnlExtProcess();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TKrnlExtProcess& operator=(const TKrnlExtProcess&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAdoptProcess
        (
                    TProcessHandle&         hprocToAdopt
        );

        tCIDLib::TBoolean bEverStarted
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bGetPriorityClass
        (
                    tCIDLib::EPrioClasses&  eToFill
        )   const;

        tCIDLib::TBoolean bFindByName
        (
            const   tCIDLib::TCh* const     pszProcName
        );

        tCIDLib::TBoolean bIsRunning
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bKill();

        tCIDLib::TBoolean bQueryFullPath
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        tCIDLib::TBoolean bQueryProcessTimes
        (
                    tCIDLib::TEncodedTime&  enctStart
            ,       tCIDLib::TEncodedTime&  enctEnd
            ,       tCIDLib::TEncodedTime&  enctInKernel
            ,       tCIDLib::TEncodedTime&  enctInUserMode
        );

        tCIDLib::TBoolean bSetPriorityClass
        (
            const   tCIDLib::EPrioClasses   eNewClass
        );

        tCIDLib::TBoolean bStart
        (
            const   tCIDLib::TCh* const     pszPath
            , const tCIDLib::TCh* const     pszInitPath
            , const tCIDLib::EExtProcFlags  eFlag
            , const tCIDLib::EExtProcShows  eShow
        );

        tCIDLib::TBoolean bStart
        (
            const   tCIDLib::TCh* const     pszPath
            , const tCIDLib::TCh* const     pszInitPath
            ,       tCIDKernel::TStrList&   klistParms
            ,       tCIDKernel::TStrList&   klistEnviron
            , const tCIDLib::EExtProcFlags  eFlag
            , const tCIDLib::EExtProcShows  eShow
        );

        tCIDLib::TBoolean bStart
        (
            const   tCIDLib::TCh* const     pszStartString
            , const tCIDLib::TCh* const     pszInitPath
            ,       tCIDKernel::TStrList&   klistEnviron
            , const tCIDLib::EExtProcFlags  eFlag
            , const tCIDLib::EExtProcShows  eShow
        );

        tCIDLib::TBoolean bStartAsOpen
        (
            const   tCIDLib::TCh* const     pszDocPath
            , const tCIDLib::EExtProcShows  eShow
            , const tCIDLib::TBoolean       bAdminPrivs
        );

        tCIDLib::TBoolean bSystemEscape
        (
            const   tCIDLib::TCh* const     pszCommandLine
            , const tCIDLib::ESysEscFlags   eFlags = tCIDLib::ESysEscFlags::StartNew
        );

        tCIDLib::TBoolean bTryAttachTo
        (
            const   tCIDLib::TCh* const     pszAttachInfo
            ,       tCIDLib::TBoolean&      bAttached
        );

        tCIDLib::TBoolean bWaitForDeath
        (
                    tCIDLib::TBoolean&      bState
            ,       tCIDLib::EExitCodes&    eExitCodeToFill
            , const tCIDLib::TCard4         c4WaitFor = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TBoolean bWaitInit
        (
            const   tCIDLib::TCard4         c4WaitFor
        );

        tCIDLib::TVoid FormatAttachData
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        tCIDLib::TProcessId pidThis() const;

        const TProcessHandle& hprocThis() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hprocThis
        //      This is the handle to the process.
        //
        //  m_pPlatData
        //      This can be used by per-platform drivers to hold extra info
        //      if they need to.
        // -------------------------------------------------------------------
        TProcessHandle  m_hprocThis;
        TPlatData*      m_pPlatData;
};

#pragma CIDLIB_POPPACK

