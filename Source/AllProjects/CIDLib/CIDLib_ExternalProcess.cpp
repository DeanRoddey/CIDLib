//
// FILE NAME: CIDLib_ExternalProcess.cpp
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
//  This file implements the TExternalProcess class, which supports the
//  execution and control of other processes.
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
RTTIDecls(TExternalProcess,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TExternalProcess
//  PREFIX: extp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TExternalProcess: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TExternalProcess::c4BreakOutParms(  const   TString&                strToParse
                                    ,       tCIDLib::TStrCollect&   colToFill)
{
    //
    //  Parameter parsing is platform specific so we have to use the CIDKernel
    //  services for this. It will fill in a raw array for us. We have to
    //  guess what the max would be and try again if it fails, but it seldom
    //  will becuase we'll use an initial max that should be more than
    //  sufficient.
    //
    const tCIDLib::TCard4 c4MaxParms = 256;
    tCIDLib::TCh* apszParms[c4MaxParms];
    tCIDLib::TCard4 c4ParmsFound = TKrnlExtProcess::c4BreakOutParms
    (
        strToParse.pszBuffer()
        , apszParms
        , c4MaxParms
    );

    //
    //  Flush the caller's collection and update it with what we found. We
    //  clean up the strings as we go.
    //
    colToFill.RemoveAll();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ParmsFound; c4Index++)
    {
        colToFill.objAdd(TString(apszParms[c4Index]));
        delete [] apszParms[c4Index];
    }
    return c4ParmsFound;
}


// ---------------------------------------------------------------------------
//  TExternalProcess: Constructors and Destructor
// ---------------------------------------------------------------------------
TExternalProcess::TExternalProcess()
{
}

TExternalProcess::~TExternalProcess()
{
}


// ---------------------------------------------------------------------------
//  TExternalProcess: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TExternalProcess::bEverStarted() const
{
    tCIDLib::TBoolean bRet;
    if (!m_kextpThis.bEverStarted(bRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_TestState
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return bRet;
}


tCIDLib::TBoolean TExternalProcess::bIsRunning() const
{
    tCIDLib::TBoolean bRet;
    if (!m_kextpThis.bIsRunning(bRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_TestState
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return bRet;
}


//
//  See if we can attach to the process using the passed attach info (which
//  should have come from a previous call on some external process' object's
//  FormatAttachData() method.
//
tCIDLib::TBoolean
TExternalProcess::bTryAttachTo(const TString& strAttachInfo)
{
    tCIDLib::TBoolean bAttached;
    if (!m_kextpThis.bTryAttachTo(strAttachInfo.pszBuffer(), bAttached))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_Attach
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return bAttached;
}


tCIDLib::TBoolean
TExternalProcess::bWaitForDeath(        tCIDLib::EExitCodes&    eToFill
                                , const tCIDLib::TCard4         c4MilliSecs) const
{
    tCIDLib::TBoolean bState;
    if (!m_kextpThis.bWaitForDeath(bState, eToFill, c4MilliSecs))
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_WaitFor
            , kerrRes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return bState;
}


tCIDLib::EPrioClasses TExternalProcess::ePriorityClass() const
{
    if (!bIsRunning())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_NotRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    tCIDLib::EPrioClasses eRet;
    if (!m_kextpThis.bGetPriorityClass(eRet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_GetPriority
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return eRet;
}

tCIDLib::EPrioClasses
TExternalProcess::ePriorityClass(const tCIDLib::EPrioClasses eNewClass)
{
    if (!bIsRunning())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_NotRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    if (!m_kextpThis.bSetPriorityClass(eNewClass))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_SetPriority
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return eNewClass;
}


tCIDLib::EExitCodes
TExternalProcess::eWaitForDeath(const tCIDLib::TCard4 c4MilliSecs) const
{
    tCIDLib::TBoolean   bState;
    tCIDLib::EExitCodes eRet;
    if (!m_kextpThis.bWaitForDeath(bState, eRet, c4MilliSecs) || !bState)
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();

        // Handle a timeout specially
        tCIDLib::TErrCode       errcToThrow = kCIDErrs::errcExtP_WaitFor;
        tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::CantDo;
        if (!bState)
        {
            errcToThrow = kCIDErrs::errcExtP_Timeout;
            eClass = tCIDLib::EErrClasses::Timeout;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcToThrow
            , kerrRes
            , tCIDLib::ESeverities::Failed
            , eClass
        );
    }
    return eRet;
}


tCIDLib::TVoid TExternalProcess::FormatAttachData(TString& strToFill)
{
    const tCIDLib::TCard4 c4BufSz = 2048;
    tCIDLib::TCh achBuf[c4BufSz + 1];
    m_kextpThis.FormatAttachData(achBuf, c4BufSz);
    strToFill = achBuf;
}


// Expose our kernel level object for internal use
TKrnlExtProcess& TExternalProcess::kextpThis()
{
    return m_kextpThis;
}

const TKrnlExtProcess& TExternalProcess::kextpThis() const
{
    return m_kextpThis;
}


// Attempt a hard kill on the targe tprocess
tCIDLib::TVoid TExternalProcess::Kill()
{
    if (!m_kextpThis.bKill())
    {
        tCIDLib::TErrCode       errcToThrow = kCIDErrs::errcExtP_Kill;
        tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::CantDo;

        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        if (kerrRes.errcId() == kKrnlErrs::errcGen_NotRunning)
        {
            errcToThrow = kCIDErrs::errcExtP_NotRunning;
            eClass = tCIDLib::EErrClasses::NotReady;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcToThrow
            , kerrRes
            , tCIDLib::ESeverities::Failed
            , eClass
        );
    }
}


tCIDLib::TProcessId TExternalProcess::pidThis() const
{
    // It will return kCIDLib::pidInvalid if we aren't running
    return m_kextpThis.pidThis();
}


// Get the full path to the original process file
tCIDLib::TVoid TExternalProcess::QueryFullPath(TString& strToFill)
{
    tCIDLib::TCh achBuf[kCIDLib::c4MaxPathLen + 1];
    if (!m_kextpThis.bQueryFullPath(achBuf, kCIDLib::c4MaxPathLen))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_QueryFullPath
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strToFill = achBuf;
}



tCIDLib::TVoid
TExternalProcess::QueryProcessTimes(TTime&      tmStart
                                    , TTime&    tmEnd
                                    , TTime&    tmKernelTime
                                    , TTime&    tmUserTime)
{
    tCIDLib::TEncodedTime enctStart, enctEnd, enctKernel, enctUser;
    if (!m_kextpThis.bQueryProcessTimes(enctStart, enctEnd, enctKernel, enctUser))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_QueryTimes
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Fill in the caller's time objects
    tmStart = enctStart;
    tmEnd = enctEnd;
    tmKernelTime = enctKernel;
    tmUserTime = enctUser;
}


tCIDLib::TVoid
TExternalProcess::Start(const   TString&                strCmdLine
                        , const TString&                strInitPath
                        , const tCIDLib::EExtProcFlags  eFlag
                        , const tCIDLib::EExtProcShows  eShow)
{
    // Just call the other version with a null environment
    Start(strCmdLine, strInitPath, TEnvironment::Nul_TEnvironment(), eFlag, eShow);
}

tCIDLib::TVoid
TExternalProcess::Start(const   TString&                strCmdLine
                        , const TString&                strInitPath
                        , const TEnvironment&           envToUse
                        , const tCIDLib::EExtProcFlags  eFlag
                        , const tCIDLib::EExtProcShows  eShow)
{
    // It cannot be running already
    if (bIsRunning())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_NotWhileRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    // Watch for obvious problem of an empty command line
    if (strCmdLine.bIsEmpty())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_EmptyPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Ok, so lets build up the environment strings that the kernel needs to
    //  do its thing.
    //
    tCIDKernel::TStrList klistEnv;
    try
    {
        if (!TEnvironment::bIsNullObject(envToUse))
        {
            TString strTmp;
            TEnvironment::TCursor cursEnv = envToUse.cursThis();

            tCIDLib::TCard4 c4Index = 0;
            for (; cursEnv; ++cursEnv)
            {
                strTmp = cursEnv->strKey().pszBuffer();
                strTmp.Append(kCIDLib::chEquals);
                strTmp.Append(cursEnv->strValue().pszBuffer());

                klistEnv.pobjAddNew(new TKrnlString(strTmp.pszBuffer()));
            }
        }

        //
        //  Start it by calling the kernel impl object. We pass along the
        //  command line and any environment info we have.
        //
        const tCIDLib::TCh* pszInitPath = nullptr;
        if (!strInitPath.bIsEmpty())
            pszInitPath = strInitPath.pszBuffer();

        if (!m_kextpThis.bStart(strCmdLine.pszBuffer(), pszInitPath, klistEnv, eFlag, eShow))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcExtP_CantStart
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strCmdLine
            );
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid
TExternalProcess::Start(const   TString&                strPath
                        , const TString&                strInitPath
                        , const tCIDLib::TStrCollect&   colParms
                        , const tCIDLib::EExtProcFlags  eFlag
                        , const tCIDLib::EExtProcShows  eShow)
{
    Start(strPath, strInitPath, colParms, TEnvironment::Nul_TEnvironment(), eFlag, eShow);
}

tCIDLib::TVoid
TExternalProcess::Start(const   TString&                strPath
                        , const TString&                strInitPath
                        , const tCIDLib::TStrCollect&   colParms
                        , const TEnvironment&           envToUse
                        , const tCIDLib::EExtProcFlags  eFlag
                        , const tCIDLib::EExtProcShows  eShow)
{
    if (bIsRunning())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_NotWhileRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    //
    //  If we ended up with an empty path, then its obviously not been set
    //  up correctly.
    //
    if (strPath.bIsEmpty())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_EmptyPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Ok, so lets build up the raw parameter and environment strings that
    //  the kernel needs to do its thing.
    //
    tCIDKernel::TStrList    klistEnv;
    tCIDKernel::TStrList    klistParms;
    TString                 strTmp;
    try
    {
        if (!TEnvironment::bIsNullObject(envToUse))
        {
            TEnvironment::TCursor cursEnv = envToUse.cursThis();

            tCIDLib::TCard4 c4Index = 0;
            for (; cursEnv; ++cursEnv)
            {
                strTmp = cursEnv->strKey().pszBuffer();
                strTmp.Append(kCIDLib::chEquals);
                strTmp.Append(cursEnv->strValue().pszBuffer());

                klistEnv.pobjAddNew(new TKrnlString(strTmp.pszBuffer()));
            }
        }

        if (colParms.c4ElemCount())
        {
            // Get a new cursor from the collection
            TColCursor<TString>& cursParms = *colParms.pcursNew();
            TJanitor<TColCursor<TString>> janCurs(&cursParms);

            // And create a list of kernel strings from those
            for (; cursParms; ++cursParms)
            {
                klistParms.pobjAddNew(new TKrnlString(cursParms->pszBuffer()));
            }
        }

        //
        //  Ok, lets start up this puppy. We just call our kernel object
        //  and pass along this info we've set up.
        //
        const tCIDLib::TCh* pszInitPath = nullptr;
        if (!strInitPath.bIsEmpty())
            pszInitPath = strInitPath.pszBuffer();
        if (!m_kextpThis.bStart(strPath.pszBuffer()
                                , pszInitPath
                                , klistParms
                                , klistEnv
                                , eFlag
                                , eShow))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcExtP_CantStart
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strPath
            );
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::TVoid
TExternalProcess::StartAsOpen(  const   TString&                strDocFile
                                , const tCIDLib::EExtProcShows  eShow
                                , const tCIDLib::TBoolean       bAdminPrivs)
{
    if (!m_kextpThis.bStartAsOpen(strDocFile.pszBuffer(), eShow, bAdminPrivs))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_CantStart
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strDocFile
        );
    }
}


tCIDLib::TVoid
TExternalProcess::SystemEscape( const   TString&                strCmdLine
                                , const tCIDLib::ESysEscFlags   eFlags)
{
    if (bIsRunning())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_NotWhileRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    // Its not already running, so lets start it
    if (!m_kextpThis.bSystemEscape(strCmdLine.pszBuffer(), eFlags))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_CantStart
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strCmdLine
        );
    }
}


tCIDLib::TVoid TExternalProcess::WaitInit(const tCIDLib::TCard4 c4WaitFor)
{
    // It has to be running for this to work
    if (!bIsRunning())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_NotRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    if (!m_kextpThis.bWaitInit(c4WaitFor))
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();

        // For a timeout, throw a specific error, else do the general one
        if (kerrRes.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcExtP_TimeoutInit
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Timeout
            );
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcExtP_WaitInit
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

