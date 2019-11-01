//
// FILE NAME: CIDKernel_ExternalProcess_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//  This file provides the Win32 specific implementation of the class
//  TKrnlExtProcess.
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
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"
#include    <psapi.h>


// ---------------------------------------------------------------------------
//  Define our publically opaque per-platfrom data
// ---------------------------------------------------------------------------
struct TKrnlExtProcess::TPlatData
{
    BOOL        bInCon;
    HANDLE      hIn;
    DWORD       dwInFlags;

    BOOL        bOutCon;
    HANDLE      hOut;
    DWORD       dwOutFlags;
};


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDKernel_ExternalProcess_Win32
{
    // -----------------------------------------------------------------------
    //  We suppor the 'reattachment' to processes by persisting a platform
    //  specific formatted string of attach info for later use. This is used
    //  as a prefix on our platform's version of that string, to help insure
    //  later that we are getting a string we formatted.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const pszAttachInfoPref = L"CIDW32AI";
}



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method is called to save console information around the execution
//  of an external process.
//
static tCIDLib::TVoid QueryConInfo(TKrnlExtProcess::TPlatData& SaveInfo)
{
    //
    //  To prevent the child process from freaking out our console handle
    //  mode settings, save them and restore them.
    //
    SaveInfo.hIn = ::GetStdHandle(STD_INPUT_HANDLE);
    SaveInfo.bInCon = ::GetConsoleMode(SaveInfo.hIn, &SaveInfo.dwInFlags);

    SaveInfo.hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    SaveInfo.bOutCon = ::GetConsoleMode(SaveInfo.hOut, &SaveInfo.dwOutFlags);
}


//
//  This method is called to restore the console mode settings after an
//  external process is run.
//
static tCIDLib::TVoid RestoreConInfo(const TKrnlExtProcess::TPlatData& SaveInfo)
{
    if (SaveInfo.bInCon)
        ::SetConsoleMode(SaveInfo.hIn, SaveInfo.dwInFlags);
    if (SaveInfo.bOutCon)
        ::SetConsoleMode(SaveInfo.hOut, SaveInfo.dwOutFlags);
}


//
//  This method is called to build up an environment block from a list of
//  environment strings. If successful, it allocates a buffer which becomes
//  the property of the caller.
//
static tCIDLib::TCh* pszBuildEnviron(tCIDKernel::TStrList& klistSrc)
{
    //
    //  First lets see how much space we need to create the environment
    //  buffer. It consists of the passed strings plus terminating
    //  nuls, plus an extra terminating nul (so we start the size at
    //  1.)
    //
    tCIDLib::TCard4 c4BufSize = 1;

    tCIDLib::TCard4 c4EnvCount = 0;
    if (klistSrc.bResetCursor())
    {
        TKrnlString* pkstrCur = klistSrc.pobjHead();
        do
        {
            c4BufSize += pkstrCur->c4Length() + 1;
            c4EnvCount++;
        }   while (klistSrc.bNext(pkstrCur));
    }

    // Allocate the buffer
    tCIDLib::TCh* pszEnvBuffer = new tCIDLib::TCh[c4BufSize];

    #if CID_DEBUG_ON
    tCIDLib::TCard4 c4CharsLeft = c4BufSize;
    #endif

    // And now fill it in
    tCIDLib::TCh* pszTmp = pszEnvBuffer;
    if (klistSrc.bResetCursor())
    {
        TKrnlString* pkstrCur = klistSrc.pobjHead();
        do
        {
            // Get the length of this string
            tCIDLib::TCard4 c4Len = pkstrCur->c4Length();

            // And copy it into the buffer
            TRawMem::CopyMemBuf
            (
                pszTmp, pkstrCur->pszValue(), (c4Len + 1) * kCIDLib::c4UniBytes
            );

            // Move the temp pointer past this string's nul
            pszTmp += c4Len + 1;

            // If debug then keep track of chars used so far
            #if CID_DEBUG_ON
            if (c4CharsLeft < c4Len+1)
            {
                delete [] pszEnvBuffer;
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
                return nullptr;
            }

            c4CharsLeft -= c4Len+1;
            #endif

        }   while (klistSrc.bNext(pkstrCur));
    }

    // Add in the second terminating nul for the end of buffer
    #if CID_DEBUG_ON
    if (c4CharsLeft != 1)
    {
        delete [] pszEnvBuffer;
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
        return nullptr;
    }
    #endif

    *pszTmp = kCIDLib::chNull;
    return pszEnvBuffer;
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlExtProcess
//  PREFIX: kextp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlExtProcess: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlExtProcess::TKrnlExtProcess() :

    m_pPlatData(nullptr)
{
    // Allocate the new process handle
    m_hprocThis.m_phprociThis = new TProcessHandleImpl;

    // And allocate our per-platform data structure and init it
    m_pPlatData = new TPlatData;
    m_pPlatData->bInCon = 0;
    m_pPlatData->bOutCon = 0;
    m_pPlatData->hIn = 0;
    m_pPlatData->hOut = 0;
}

TKrnlExtProcess::~TKrnlExtProcess()
{
    // Clean up the platform data
    delete m_pPlatData;
    m_pPlatData = nullptr;

    // Close the process handle if open
    if (m_hprocThis.m_phprociThis->hProcess)
    {
        if (!::CloseHandle(m_hprocThis.m_phprociThis->hProcess))
        {
            //
            //  If it fails and we are debugging, then do a popup. Otherwise
            //  there is not much we can do since we cannot throw out of the
            //  destructor.
            //
            #if CID_DEBUG_ON
            TKrnlError::SetLastHostError(::GetLastError());
            kmodCIDKernel.KrnlErrorPopUp
            (
                TKrnlError::kerrLast()
                , CID_FILE
                , CID_LINE
                , kmodCIDKernel.pszLoadCIDFacMsg(kKrnlErrs::errcGen_CloseHandle)
            );
            #endif
        }
    }
    m_hprocThis.m_phprociThis->hProcess = 0;
    m_hprocThis.m_phprociThis->pidThis = 0;
}


// ---------------------------------------------------------------------------
//  TKrnlExtProcess: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If this object has not been previously set, we can adopt a process and
//  gain access to it, given that the handle to it has been obtained.
//
tCIDLib::TBoolean TKrnlExtProcess::bAdoptProcess(TProcessHandle& hprocToAdopt)
{
    if (m_hprocThis.m_phprociThis->hProcess != 0)
    {
        // This object has already been associated with a process
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);

        // Free handle, since we are adopting it
        ::CloseHandle(hprocToAdopt.m_phprociThis->hProcess);
        return kCIDLib::False;
    }

    // Copy over the process handle info
    m_hprocThis.m_phprociThis->hProcess = hprocToAdopt.m_phprociThis->hProcess;
    m_hprocThis.m_phprociThis->pidThis = hprocToAdopt.m_phprociThis->pidThis;

    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlExtProcess::bEverStarted(tCIDLib::TBoolean& bToFill) const
{
    bToFill = (m_hprocThis.m_phprociThis->hProcess != 0);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bGetPriorityClass(tCIDLib::EPrioClasses& eToFill) const
{
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (!bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }

    const tCIDLib::TCard4 c4Prio = ::GetPriorityClass
    (
        m_hprocThis.m_phprociThis->hProcess
    );

    if (c4Prio == REALTIME_PRIORITY_CLASS)
        eToFill = tCIDLib::EPrioClasses::RealTime;
    else if (c4Prio == HIGH_PRIORITY_CLASS)
        eToFill = tCIDLib::EPrioClasses::High;
    else if (c4Prio == NORMAL_PRIORITY_CLASS)
        eToFill = tCIDLib::EPrioClasses::Normal;
    else if (c4Prio == IDLE_PRIORITY_CLASS)
        eToFill = tCIDLib::EPrioClasses::IdleTime;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_UnknownPrio);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bIsRunning(tCIDLib::TBoolean& bToFill) const
{
    if (!m_hprocThis.m_phprociThis->hProcess)
    {
        bToFill = kCIDLib::False;
        return kCIDLib::True;
    }

    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle
    (
        m_hprocThis.m_phprociThis->hProcess, 0
    );

    // If it succeeded, the process is dead
    if (eRes == tCIDLib::EWaitRes::Success)
    {
        bToFill = kCIDLib::False;
        return kCIDLib::True;
    }

    // If it timed out, then process is alive
    if (eRes == tCIDLib::EWaitRes::Timeout)
    {
        bToFill = kCIDLib::True;
        return kCIDLib::True;
    }

    // Else an error
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlExtProcess::bKill()
{
    tCIDLib::TBoolean bRun;

    if (!bIsRunning(bRun))
        return kCIDLib::False;

    // If it's not running, then just say we are happy
    if (!bRun)
        return kCIDLib::True;

    if (!::TerminateProcess(m_hprocThis.m_phprociThis->hProcess, 0))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Using the handle of the running process, get the full path to it
tCIDLib::TBoolean
TKrnlExtProcess::bQueryFullPath(        tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    // We have to be running for this to work
    if (!m_hprocThis.m_phprociThis->hProcess)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NeverStarted);
        return kCIDLib::False;
    }

    const tCIDLib::TCard4 c4Res = ::GetModuleFileNameExW
    (
        m_hprocThis.m_phprociThis->hProcess, 0, pszToFill, c4MaxChars
    );
    if (!c4Res)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bQueryProcessTimes(tCIDLib::TEncodedTime&      enctStart
                                    , tCIDLib::TEncodedTime&    enctEnd
                                    , tCIDLib::TEncodedTime&    enctInKernel
                                    , tCIDLib::TEncodedTime&    enctInUserMode)
{
    if (!m_hprocThis.m_phprociThis->hProcess)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NeverStarted);
        return kCIDLib::False;
    }

    FILETIME CreateTime;
    FILETIME ExitTime;
    FILETIME KrnlTime;
    FILETIME UserTime;
    if (!::GetProcessTimes
    (
        m_hprocThis.m_phprociThis->hProcess
        , &CreateTime
        , &ExitTime
        , &KrnlTime
        , &UserTime))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_QueryProcTimes);
        return kCIDLib::False;
    }

    //
    //  And put the values into the caller's buffers. Convert the start
    //  and end times to local times, but not the others because they
    //  are just time offsets.
    //
    FILETIME Local;

    ::FileTimeToLocalFileTime(&CreateTime, &Local);
    enctStart = TRawBits::c8From32(Local.dwLowDateTime, Local.dwHighDateTime)
                - TKrnlTimeStamp::enctBaseOfs;

    ::FileTimeToLocalFileTime(&ExitTime, &Local);
    enctEnd = TRawBits::c8From32(Local.dwLowDateTime, Local.dwHighDateTime)
              - TKrnlTimeStamp::enctBaseOfs;

    enctInKernel = TRawBits::c8From32(KrnlTime.dwLowDateTime, KrnlTime.dwHighDateTime);
    enctInUserMode = TRawBits::c8From32(UserTime.dwLowDateTime, UserTime.dwHighDateTime);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bSetPriorityClass(const tCIDLib::EPrioClasses eNewClass)
{
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (!bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4NewPrio;

    if (eNewClass == tCIDLib::EPrioClasses::IdleTime)
        c4NewPrio = IDLE_PRIORITY_CLASS;
    else if (eNewClass == tCIDLib::EPrioClasses::Normal)
        c4NewPrio = NORMAL_PRIORITY_CLASS;
    else if (eNewClass == tCIDLib::EPrioClasses::High)
        c4NewPrio = HIGH_PRIORITY_CLASS;
    else if (eNewClass == tCIDLib::EPrioClasses::RealTime)
        c4NewPrio = REALTIME_PRIORITY_CLASS;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_UnknownPrio);
        return kCIDLib::False;
    }

    if (!::SetPriorityClass(m_hprocThis.m_phprociThis->hProcess, c4NewPrio))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bStart(const   tCIDLib::TCh* const     pszPath
                        , const tCIDLib::TCh*           pszInitPath
                        ,       tCIDKernel::TStrList&   klistParms
                        ,       tCIDKernel::TStrList&   klistEnviron
                        , const tCIDLib::EExtProcFlags  eFlags
                        , const tCIDLib::EExtProcShows  eShow)
{
    //
    //  Ok, first check to make sure that we are not already running. If
    //  we are, then that's this is not legal.
    //
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    //
    //  For this version, we just build up the command line and then call
    //  the other version. So first we need to calculate how long the command
    //  line buffer must be. We start with the path plus three. This leaves
    //  room for a space after it (if there are any parameters) and quote
    //  characters around it (in case it has whitespace in it.)
    //
    tCIDLib::TCard4 c4BufSize = TRawStr::c4StrLen(pszPath) + 3;
    if (klistParms.bResetCursor())
    {
        TKrnlString* pkstrCur = klistParms.pobjHead();
        do
        {
            c4BufSize += pkstrCur->c4Length();
        }   while (klistParms.bNext(pkstrCur));
    }

    // Ok, now lets allocate the buffer and put a janitor on it.
    tCIDLib::TCh* pszCmdLine = new tCIDLib::TCh[c4BufSize + 1];
    TArrayJanitor<tCIDLib::TCh> janCmdLine(pszCmdLine);

    // Start by copying the path into it
    TRawStr::CopyCatStr(pszCmdLine, c4BufSize, L"\"", pszPath, L"\"");

    //
    //  And then if there are any parameters, we need to append them onto
    //  the command line. We prepend a separator space between each one and
    //  put quotes around them. The storage for this was accounted for above.
    //
    if (klistParms.bResetCursor())
    {
        TKrnlString* pkstrCur = klistParms.pobjHead();
        do
        {
            TRawStr::CatStr(pszCmdLine, L" \"");
            TRawStr::CatStr(pszCmdLine, klistParms.pobjHead()->pszValue());
            TRawStr::CatStr(pszCmdLine, L"\"");
        }   while (klistParms.bNext(pkstrCur));
    }


    // And call the other version
    return bStart(pszCmdLine, pszInitPath, klistEnviron, eFlags, eShow);
}

tCIDLib::TBoolean
TKrnlExtProcess::bStart(const   tCIDLib::TCh* const     pszStartString
                        , const tCIDLib::TCh*           pszInitPath
                        ,       tCIDKernel::TStrList&   klistEnviron
                        , const tCIDLib::EExtProcFlags  eFlags
                        , const tCIDLib::EExtProcShows  eShow)
{
    //
    //  Ok, first check to make sure that we are not already running. If
    //  we are, then that's this is not legal.
    //
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    // If there's any previous handle open, then close it
    if (m_hprocThis.m_phprociThis->hProcess)
    {
        ::CloseHandle(m_hprocThis.m_phprociThis->hProcess);
        m_hprocThis.m_phprociThis->hProcess = 0;
    }

    //
    //  We need to break the command line up into individual parameters.
    //  This is done because the canonical command line format is not the
    //  same as any platform formats. So we have it broken up for us by
    //  code that understands the portable format and then we put it back
    //  together in our required format.
    //
    const tCIDLib::TCard4 c4MaxParms = 1024;
    tCIDLib::TCh* apszParms[c4MaxParms];
    const tCIDLib::TCard4 c4ParmCount = c4BreakOutParms
    (
        pszStartString, apszParms, c4MaxParms
    );

    // If no parms, return failure. He already set the last error
    if (!c4ParmCount)
        return kCIDLib::False;

    //
    //  Make a separate copy of the 0th parameter, the program to run, so
    //  that we can modify it if needed.
    //
    tCIDLib::TCh szFullPath[kCIDLib::c4MaxPathLen + 1];
    TRawStr::CopyStr(szFullPath, apszParms[0], kCIDLib::c4MaxPathLen);

    //
    //  See if the program has an extension. If not, then add the default exe
    //  extension. So we find the last path separator and the last extension
    //  separator. If there is no extension separator, or there are are some
    //  but they are before the last path separator (i.e. its some kind of
    //  path like "\Foo.Bar\Program"), we need to add the extension.
    //
    tCIDLib::TCh* pszLastSep = TRawStr::pszFindLastChar(szFullPath, L'\\');
    tCIDLib::TCh* pszLastPer = TRawStr::pszFindLastChar
    (
        szFullPath, L'.'
    );

    if (!pszLastPer || (pszLastPer < pszLastSep))
    {
        // If the last character is the period, don't add two
        if (szFullPath[TRawStr::c4StrLen(szFullPath) - 1] == L'.')
            TRawStr::CatStr(szFullPath, L"Exe", kCIDLib::c4MaxPathLen);
        else
            TRawStr::CatStr(szFullPath, L".Exe", kCIDLib::c4MaxPathLen);
    }

    //
    //  If it does not have any path component, then we have to find it via
    //  the system path.
    //
    if (!TRawStr::pszFindChar(szFullPath, L'\\')
    &&  !TRawStr::pszFindChar(szFullPath, L':'))
    {
        //
        //  We have to have a source copy, because this is going to update
        //  the szFullPath field, which is where our current value is. So make
        //  a temp copy.
        //
        tCIDLib::TCh* pszTmp = TRawStr::pszReplicate(szFullPath);
        TArrayJanitor<tCIDLib::TCh> janTmp(pszTmp);
        if (!TKrnlFileSys::bFindInSysPath(pszTmp, szFullPath, kCIDLib::c4MaxPathLen))
        {
            // Clean up the parms before we exit
            for (tCIDLib::TCard4 c4ParmInd = 0; c4ParmInd < c4ParmCount; c4ParmInd++)
                delete [] apszParms[c4ParmInd];
            return kCIDLib::False;
        }
    }

    //
    //  Figure out how big a buffer we need to create to hold the full
    //  command line.
    //
    tCIDLib::TCard4 c4BufSz = (c4ParmCount * 4) + TRawStr::c4StrLen(szFullPath);

    // We handled the 0th parm above, so start at 1 here!
    tCIDLib::TCard4 c4ParmInd;
    for (c4ParmInd = 1; c4ParmInd < c4ParmCount; c4ParmInd++)
        c4BufSz += TRawStr::c4StrLen(apszParms[c4ParmInd]);

    // Allocate the buffer and put a janitor on it
    tCIDLib::TCh* pszParms = new tCIDLib::TCh[c4BufSz + 1];
    TArrayJanitor<tCIDLib::TCh> janParams(pszParms);

    // Now fill in the buffer, starting with the modified path parm
    pszParms[0] = kCIDLib::chNull;
    TRawStr::CatStr(pszParms, L"\"");
    TRawStr::CatStr(pszParms, szFullPath);
    TRawStr::CatStr(pszParms, L"\"");

    // And now do the rest, quoted (and space separated)
    for (c4ParmInd = 1; c4ParmInd < c4ParmCount; c4ParmInd++)
    {
        TRawStr::CatStr(pszParms, L" \"");
        TRawStr::CatStr(pszParms, apszParms[c4ParmInd]);
        TRawStr::CatStr(pszParms, L"\"");
    }

    // Now we can let the parameters go
    for (c4ParmInd = 0; c4ParmInd < c4ParmCount; c4ParmInd++)
        delete [] apszParms[c4ParmInd];

    //
    //  Ok, we've found it, so now lets load up the image header. Even though
    //  we might not need it below, this will insure we can really find the
    //  file and that its really an exe.
    //
    IMAGE_OPTIONAL_HEADER Header;
    if (!TKrnlWin32::bQueryExeHdr(szFullPath, Header))
        return kCIDLib::False;

    //
    //  Now lets build up the environment buffer, if we have any environment
    //  strings. If not, it just stays zero.
    //
    tCIDLib::TCh* pszEnvBuffer = nullptr;
    if (!klistEnviron.bIsEmpty())
    {
        pszEnvBuffer = pszBuildEnviron(klistEnviron);
        if (!pszEnvBuffer)
            return kCIDLib::False;
    }

    // Put a janitor on it so we know it gets cleaned up
    TArrayJanitor<tCIDLib::TCh> janEnv(pszEnvBuffer);

    //
    //  We need to set up the startup info structure, based on the incoming
    //  flags and show styles, and some stuff that we just do all the time.
    //
    STARTUPINFO Startup = {0};
    Startup.cb = sizeof(STARTUPINFO);
    Startup.dwFlags = STARTF_FORCEONFEEDBACK | STARTF_USESHOWWINDOW;

    switch(eShow)
    {
        case tCIDLib::EExtProcShows::Hidden :
            Startup.wShowWindow = SW_HIDE;
            break;

        case tCIDLib::EExtProcShows::NormNoActivate :
            Startup.wShowWindow = SW_SHOWNA;
            break;

        case tCIDLib::EExtProcShows::Maximized :
            Startup.wShowWindow = SW_SHOWMAXIMIZED;
            break;

        case tCIDLib::EExtProcShows::Minimized :
            Startup.wShowWindow = SW_SHOWMINIMIZED;
            break;

        case tCIDLib::EExtProcShows::MinNoActivate :
            Startup.wShowWindow = SW_SHOWMINNOACTIVE;
            break;

        case tCIDLib::EExtProcShows::Normal :
        default :
            Startup.wShowWindow = SW_SHOWNORMAL;
            break;
    };

    //
    //  If it should inherit our handles, then set that up.
    //
    DWORD bInheritHandles = 0;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EExtProcFlags::UseStdHandles))
    {
        Startup.dwFlags |= STARTF_USESTDHANDLES;
        Startup.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
        Startup.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
        Startup.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
//        bInheritHandles = 1;
    }

    // Translate the flag parameter to the system flags
    tCIDLib::TCard4 c4Flag = 0;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EExtProcFlags::Detached))
        c4Flag |= CREATE_NEW_CONSOLE;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EExtProcFlags::NewProcGroup))
        c4Flag |= CREATE_NEW_PROCESS_GROUP;

    // Add in some other stuff we always do
    c4Flag |= CREATE_DEFAULT_ERROR_MODE
              | CREATE_UNICODE_ENVIRONMENT;

    //
    //  To prevent the child process from freaking out our console handle
    //  mode settings, save them and restore them.
    //
    QueryConInfo(*m_pPlatData);


    //
    //  Ok now we can finally create the process. We store away the process
    //  handle and the process id. In this one we just have a single string
    //  with the program and params together, so we pass zero for the first
    //  parameter.
    //
    PROCESS_INFORMATION ProcInfo = {0};
    if (!::CreateProcess
    (
        szFullPath
        , pszParms
        , 0
        , 0
        , bInheritHandles
        , c4Flag
        , pszEnvBuffer
        , pszInitPath
        , &Startup
        , &ProcInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());

        // Restore the console mode
        RestoreConInfo(*m_pPlatData);

        return kCIDLib::False;
    }

    // Get rid of the thread handle
    ::CloseHandle(ProcInfo.hThread);

    // Store away the process info
    m_hprocThis.m_phprociThis->hProcess = ProcInfo.hProcess;
    m_hprocThis.m_phprociThis->pidThis = ProcInfo.dwProcessId;

    // If asked, then wait for the other program to die
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EExtProcFlags::WaitFor))
    {
        if ((Header.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
        ||  (Header.Subsystem == IMAGE_SUBSYSTEM_OS2_CUI)
        ||  (Header.Subsystem == IMAGE_SUBSYSTEM_POSIX_CUI))
        {
            tCIDLib::TBoolean   bState;
            tCIDLib::EExitCodes eExit;
            if (!bWaitForDeath(bState, eExit, kCIDLib::c4MaxWait) || !bState)
                return kCIDLib::False;

            // Restore the saved console info
            RestoreConInfo(*m_pPlatData);
        }
    }
    return kCIDLib::True;
}


//
//  Start the program by invoking an 'open' operation on some file that it
//  is configured as the editor for.
//
tCIDLib::TBoolean
TKrnlExtProcess::bStartAsOpen(  const   tCIDLib::TCh* const     pszDocPath
                                , const tCIDLib::EExtProcShows  eShow
                                , const tCIDLib::TBoolean       bAdminPrivs)
{
    // Fill in the ingoing info
    SHELLEXECUTEINFO ShellEx = {0};

    ShellEx.cbSize = sizeof(ShellEx);
    ShellEx.fMask = SEE_MASK_NOCLOSEPROCESS
                    | SEE_MASK_CONNECTNETDRV
                    | SEE_MASK_FLAG_NO_UI
                    | SEE_MASK_WAITFORINPUTIDLE;

    ShellEx.lpFile = pszDocPath;

    // Set the verb to 'run as' if it needs admin privs
    if (bAdminPrivs)
        ShellEx.lpVerb = L"runas";

    switch(eShow)
    {
        case tCIDLib::EExtProcShows::Hidden :
            ShellEx.nShow = SW_HIDE;
            break;

        case tCIDLib::EExtProcShows::NormNoActivate :
            ShellEx.nShow = SW_SHOWNA;
            break;

        case tCIDLib::EExtProcShows::Maximized :
            ShellEx.nShow = SW_SHOWMAXIMIZED;
            break;

        case tCIDLib::EExtProcShows::Minimized :
            ShellEx.nShow = SW_SHOWMINIMIZED;
            break;

        case tCIDLib::EExtProcShows::MinNoActivate :
            ShellEx.nShow = SW_SHOWMINNOACTIVE;
            break;

        case tCIDLib::EExtProcShows::Normal :
        default :
            ShellEx.nShow = SW_SHOWNORMAL;
            break;
    };

    BOOL Res = ::ShellExecuteEx(&ShellEx);
    if (Res == FALSE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  Store away the process handle. We don't get a PID back so we have
    //  ask for that separately.
    //
    m_hprocThis.m_phprociThis->hProcess = ShellEx.hProcess;
    m_hprocThis.m_phprociThis->pidThis = ::GetProcessId(ShellEx.hProcess);

    return kCIDLib::True;
}


// Start the program by passing a command line to the system command processor
tCIDLib::TBoolean
TKrnlExtProcess::bSystemEscape( const   tCIDLib::TCh* const     pszCommandLine
                                , const tCIDLib::ESysEscFlags   eFlags)
{
    //
    //  Ok, first check to make sure that we are not already running. If
    //  we are, then that's this is not legal.
    //
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    // If there's any previous handle open, then close it
    if (m_hprocThis.m_phprociThis->hProcess)
        ::CloseHandle(m_hprocThis.m_phprociThis->hProcess);

    //
    //  Find the COMSPEC environment variable setting. This should always be
    //  set to the desired command processor for this workstation. If its
    //  not found, provide a default value.
    //
    tCIDLib::TCh szComSpec[2048];
    if (!::GetEnvironmentVariable(L"COMSPEC", szComSpec, 1023))
        TRawStr::CopyStr(szComSpec, L"cmd.exe");

    //
    //  We need to break the command line up into individual parameters.
    //  This is done because the canonical command line format is not the
    //  same as any platform formats. So we have it broken up for us by
    //  code that understands the portable format and then we put it back
    //  together in our required format.
    //
    const tCIDLib::TCard4 c4MaxParms = 1024;
    tCIDLib::TCh* apszParms[c4MaxParms];
    const tCIDLib::TCard4 c4ParmCount = c4BreakOutParms
    (
        pszCommandLine, apszParms, c4MaxParms
    );

    //
    //  If the command line is not empty but we got back no parms, return
    //  failure. He already set the last error
    //
    if (!c4ParmCount && *pszCommandLine)
        return kCIDLib::False;

    //
    //  Figure out how big a buffer we need to create the whole thing. We
    //  have to hold the comspec, the /C or /K with spaces on either side,
    //  and a space after each parm, and the parms themselves.
    //
    tCIDLib::TCard4 c4BufSz = TRawStr::c4StrLen(szComSpec)
                              + 4
                              + 2
                              + c4ParmCount;

    tCIDLib::TCard4 c4ParmInd;
    for (c4ParmInd = 0; c4ParmInd < c4ParmCount; c4ParmInd++)
        c4BufSz += TRawStr::c4StrLen(apszParms[c4ParmInd]);

    // Allocate the buffer and put a janitor on it
    tCIDLib::TCh* pszParms = new tCIDLib::TCh[c4BufSz + 1];
    TArrayJanitor<tCIDLib::TCh> janParams(pszParms);

    // Copy over the compsec and the correct flag
    TRawStr::CopyStr(pszParms, szComSpec);
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESysEscFlags::StayOpen))
        TRawStr::CatStr(pszParms, L" /K ");
    else
        TRawStr::CatStr(pszParms, L" /C ");

    // If any parms, then add in those as well
    if (c4ParmCount)
    {
        TRawStr::CatStr(pszParms, L"\"");
        for (c4ParmInd = 0; c4ParmInd < c4ParmCount; c4ParmInd++)
        {
            TRawStr::CatStr(pszParms, apszParms[c4ParmInd]);
            TRawStr::CatStr(pszParms, L" ");
        }
        TRawStr::CatStr(pszParms, L"\"");
    }

    // Now we can let the parameters go
    for (c4ParmInd = 0; c4ParmInd < c4ParmCount; c4ParmInd++)
        delete [] apszParms[c4ParmInd];

    // Set up the flags according to the parameter that we got
    tCIDLib::TCard4 c4Flags = CREATE_UNICODE_ENVIRONMENT;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESysEscFlags::NewWindow))
        c4Flags |= CREATE_NEW_CONSOLE;

    //
    //  To prevent the child process from freaking out our console handle
    //  mode settings, save them and restore them.
    //
    QueryConInfo(*m_pPlatData);

    //
    //  And now invoke the process. We pass the command processor as the
    //  program, and provide the environment variables separately.
    //
    PROCESS_INFORMATION ProcInfo = {0};
    STARTUPINFO Startup = {0};
    Startup.cb = sizeof(STARTUPINFO);
    if (!::CreateProcess
    (
        0, pszParms, 0, 0, FALSE, c4Flags, 0, 0, &Startup, &ProcInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());

        // Restore the console mode
        RestoreConInfo(*m_pPlatData);
        return kCIDLib::False;
    }

    // Get rid of the thread handle
    ::CloseHandle(ProcInfo.hThread);

    // Store away the process info
    m_hprocThis.m_phprociThis->hProcess = ProcInfo.hProcess;
    m_hprocThis.m_phprociThis->pidThis = ProcInfo.dwProcessId;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESysEscFlags::Wait))
    {
        tCIDLib::TBoolean   bState;
        tCIDLib::EExitCodes eExit;
        if (!bWaitForDeath(bState, eExit, kCIDLib::c4MaxWait) || !bState)
            return kCIDLib::False;

        // Restore the console mode
        RestoreConInfo(*m_pPlatData);
    }
    return kCIDLib::True;
}


//
//  Given attachment info previous gotten by a FormatAttachData call, this
//  guy will try to find the previously running process and attach this
//  object to it.
//
tCIDLib::TBoolean
TKrnlExtProcess::bTryAttachTo(  const   tCIDLib::TCh* const pszAttachInfo
                                ,       tCIDLib::TBoolean&  bAttached)
{
    // Assume the worst
    bAttached = kCIDLib::False;

    // Catch a bogus passed buffer
    if (!pszAttachInfo || !*pszAttachInfo)
        return kCIDLib::True;

    //
    //  Ok, first check to make sure that we are not already running. If
    //  we are, then that's this is not legal.
    //
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyStarted);
        return kCIDLib::False;
    }

    //
    //  Make a copy of the attach info that we can modify. There is a space
    //  between each token in the string, and we can just replace those
    //  with nulls to break out the info.
    //
    tCIDLib::TCh* const pszInfo = TRawStr::pszReplicate(pszAttachInfo);
    TArrayJanitor<tCIDLib::TCh> janInfo(pszInfo);

    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszInfo);
    tCIDLib::TCh* pszEnd = pszInfo + c4Len;
    tCIDLib::TCh* pszPtr = pszInfo;

    tCIDLib::TCh* pszPref = pszInfo;
    tCIDLib::TCh* pszPID = 0;
    tCIDLib::TCh* pszPath = 0;
    while (pszPtr < pszEnd)
    {
        if (*pszPtr == kCIDLib::chSpace)
        {
            *pszPtr = kCIDLib::chNull;

            if (!pszPID)
                pszPID = pszPtr + 1;
            else if (!pszPath)
                pszPath = pszPtr + 1;
        }

        // If we have the bits, then we are done
        if (pszPID && pszPath)
            break;

        // Else, keep going
        pszPtr++;
    }

    //
    //  If we didn't get all the bits, or the attach prefix is not valid,
    //  then reject it.
    //
    if (!pszPID
    || !pszPath
    || !TRawStr::bCompareStr(pszPref, CIDKernel_ExternalProcess_Win32::pszAttachInfoPref))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_BadAttachInfo);
        return kCIDLib::False;
    }

    // Convert the PID to a numeric value
    tCIDLib::TBoolean bOk;
    tCIDLib::TProcessId pidFind = TRawStr::c4AsBinary(pszPID, bOk, tCIDLib::ERadices::Hex);
    if (!bOk)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_BadAttachInfo);
        return kCIDLib::False;
    }

    // Ok, let's try to find a process with this PID
    HANDLE hprocTar = ::OpenProcess(PROCESS_ALL_ACCESS, kCIDLib::False, pidFind);

    // Oh well, he's no longer around. Not an error, just a failure
    if (!hprocTar)
        return kCIDLib::True;

    //
    //  Make absolutely sure by getting the full path to the process, and
    //  comparing it to the full path we stored in the attach info.
    //
    tCIDLib::TCh achBuf[kCIDLib::c4MaxPathLen + 1];
    const tCIDLib::TCard4 c4Res = ::GetModuleFileNameExW
    (
        hprocTar, 0, achBuf, kCIDLib::c4MaxPathLen
    );
    if (!c4Res)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // <TBD> Is this right?
    if (!TRawStr::bCompareStr(achBuf, pszPath))
    {
        // The PID has been reused. Not an error, just a failure
        ::CloseHandle(hprocTar);
        return kCIDLib::True;
    }

    // Looks like a winner, so attach to this guy
    bAttached = kCIDLib::True;
    m_hprocThis.m_phprociThis->pidThis = pidFind;
    m_hprocThis.m_phprociThis->hProcess = hprocTar;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlExtProcess::bWaitForDeath(         tCIDLib::TBoolean&   bState
                                ,       tCIDLib::EExitCodes& eToFill
                                , const tCIDLib::TCard4      c4WaitFor) const
{
    // If it was never started, just say we are happy
    if (!m_hprocThis.m_phprociThis->hProcess)
    {
        eToFill = tCIDLib::EExitCodes::Normal;
        return kCIDLib::True;
    }

    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle
    (
        m_hprocThis.m_phprociThis->hProcess, c4WaitFor
    );

    //
    //  If a timeout, don't say we failed, but set the last error and set the
    //  state parm to false to say we timed out.
    //
    if (eRes == tCIDLib::EWaitRes::Timeout)
    {
        bState = kCIDLib::False;
        return kCIDLib::True;
    }

    //
    //  Assume its dead at this point, and put back the console mode that
    //  was saved away when it started.
    //
    RestoreConInfo(*m_pPlatData);

    // If it failed, then the handle may be bad or something
    if (eRes == tCIDLib::EWaitRes::Error)
        return kCIDLib::False;

    // Query tbe result code
    tCIDLib::TCard4 c4Code;
    if (!::GetExitCodeProcess(m_hprocThis.m_phprociThis->hProcess, &c4Code))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    bState = kCIDLib::True;
    eToFill = tCIDLib::EExitCodes(c4Code);
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlExtProcess::bWaitInit(const tCIDLib::TCard4 c4WaitFor)
{
    // Make sure it's running
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::False;

    if (!bRun)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotRunning);
        return kCIDLib::False;
    }

    // Looks to be so do the wait
    const tCIDLib::TCard4 c4Res = ::WaitForInputIdle
    (
        m_hprocThis.m_phprociThis->hProcess, c4WaitFor
    );

    //
    //  If it timed out, then return the timeout error, else do the usual
    //  set of the host error.
    //
    if (c4Res == WAIT_TIMEOUT)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
        return kCIDLib::False;
    }

    if (c4Res == WAIT_FAILED)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TVoid
TKrnlExtProcess::FormatAttachData(          tCIDLib::TCh* const pszToFill
                                    , const tCIDLib::TCard4     c4MaxChars)
{
    //
    //  Just format out the process id and path to the executable. This
    //  let's us search for it later if we are asked to re-attach, and to
    //  verify it's the same process (i.e. the PID hasn't just be reused.)
    //
    //  If we aren't running, then don't format anything.
    //
    tCIDLib::TBoolean bRunning;
    if (!bIsRunning(bRunning))
    {
        pszToFill[0] = kCIDLib::chNull;
        return;
    }

    if (bRunning)
    {
        tCIDLib::TCh achBuf[kCIDLib::c4MaxPathLen + 1];

        //
        //  First, put in a little ID, so we can later help insure that
        //  a call to bTryAttachTo() is getting a valid attach data string.
        //
        TRawStr::CopyStr
        (
            pszToFill
            , CIDKernel_ExternalProcess_Win32::pszAttachInfoPref
            , c4MaxChars
        );
        TRawStr::CatStr(pszToFill, L" ", c4MaxChars);

        // Append the process id of the target process
        TRawStr::bFormatVal
        (
            tCIDLib::TCard8(m_hprocThis.m_phprociThis->pidThis)
            , achBuf
            , kCIDLib::c4MaxPathLen
            , tCIDLib::ERadices::Hex
        );
        TRawStr::CatStr(pszToFill, achBuf, c4MaxChars);
        TRawStr::CatStr(pszToFill, L" ", c4MaxChars);

        // Append the full path to the target process exe
        ::GetModuleFileNameExW
        (
            m_hprocThis.m_phprociThis->hProcess
            , 0
            , achBuf
            , kCIDLib::c4MaxPathLen
        );
        achBuf[kCIDLib::c4MaxPathLen] = kCIDLib::chNull;
        TRawStr::CatStr(pszToFill, achBuf, c4MaxChars);
    }
     else
    {
        pszToFill[0] = kCIDLib::chNull;
    }
}


tCIDLib::TProcessId TKrnlExtProcess::pidThis() const
{
    // Get the running state. If it fails, return the invalid handle
    tCIDLib::TBoolean bRun;
    if (!bIsRunning(bRun))
        return kCIDLib::pidInvalid;

    // If not running, also return the invalid handle
    if (!bRun)
        return kCIDLib::pidInvalid;

    // Looks ok, so return the id
    return m_hprocThis.m_phprociThis->pidThis;
}


