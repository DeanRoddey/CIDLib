//
// FILE NAME: CIDKernel_Process_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/1998
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
//  This file provides the Win32 specific implementation of the TKrnlProcess
//  namespace.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDKernel_Process_Win32
    {
        struct TFindWndFiltered
        {
            tCIDCtrls::TWndHandle   hwndFound;
            tCIDLib::TProcessId     pidTarget;
            tCIDLib::TBoolean       bOwned;
            tCIDLib::TCard4         c4DontWantStyles;
            tCIDLib::TCard4         c4WantStyles;
            tCIDLib::TCard4         c4DontWantExStyles;
            tCIDLib::TCard4         c4WantExStyles;

            tCIDLib::TCard4         c4FoundStyles;
            tCIDLib::TCard4         c4FoundExStyles;
        };


        //
        //  We provide a mechanism for supporting single instances of applications
        //  being runnable. We create a shared memory buffer and put some info into
        //  it. We need a a little structure for the data in the buffer.
        //
        //  pksmbHold is used to keep a copy around if we created the buffer, so
        //  that we are the active instance until we die, at which time this
        //  buffer will be released by the OS.
        //
        constexpr tCIDLib::TCard4   c4SingleInstVer = 1;
        struct   TSingleInstInfo
        {
            tCIDLib::TCard4         c4Version;
            HANDLE                  hprocSingle;
            tCIDLib::TProcessId     pidSingle;
        };
        TKrnlSharedMemBuf*          pksmbHold;
    }
}



// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static BOOL CALLBACK bEnumWndFiltered(HWND hwndTarget, LPARAM lParam)
{
    // Cast the data parm to its correct type
    CIDKernel_Process_Win32::TFindWndFiltered* pFindData
                            = (CIDKernel_Process_Win32::TFindWndFiltered*)lParam;

    //
    //  If they are filtering for pid, then get the process id of this window
    //  and check it.
    //
    if (pFindData->pidTarget)
    {
        tCIDLib::TCard4 c4Id;
        ::GetWindowThreadProcessId(hwndTarget, &c4Id);
        if (c4Id != pFindData->pidTarget)
            return 1;
    }

    // Make sure it matches the owned/unowned flag
    tCIDCtrls::TWndHandle hwndOwner = ::GetWindow(hwndTarget, GW_OWNER);

    if ((pFindData->bOwned && !hwndOwner)
    ||  (!pFindData->bOwned && hwndOwner))
    {
        return 1;
    }

    // Test the styles
    const tCIDLib::TCard4 c4Styles = ::GetWindowLong(hwndTarget, GWL_STYLE);
    const tCIDLib::TCard4 c4ExStyles = ::GetWindowLong(hwndTarget, GWL_EXSTYLE);

    if (((c4Styles & pFindData->c4WantStyles) == pFindData->c4WantStyles)
    &&  ((c4Styles & pFindData->c4DontWantStyles) == 0)
    &&  ((c4ExStyles & pFindData->c4WantExStyles) == pFindData->c4WantExStyles)
    &&  ((c4ExStyles & pFindData->c4DontWantExStyles) == 0))
    {
        pFindData->c4FoundExStyles = c4ExStyles;
        pFindData->c4FoundStyles = c4Styles;
        pFindData->hwndFound = hwndTarget;
        return 0;
    }
    return 1;
}





// ---------------------------------------------------------------------------
//   CLASS: TProcessHandle
//  PREFIX: hproc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TProcessHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TProcessHandle::TProcessHandle() :

    m_phprociThis(0)
{
    m_phprociThis = new TProcessHandleImpl;
    m_phprociThis->hProcess = 0;
    m_phprociThis->pidThis = 0;
}

TProcessHandle::TProcessHandle(const TProcessHandle& hprocToCopy) :

    m_phprociThis(0)
{
    m_phprociThis = new TProcessHandleImpl;
    m_phprociThis->hProcess = hprocToCopy.m_phprociThis->hProcess;
    m_phprociThis->pidThis = hprocToCopy.m_phprociThis->pidThis;
}

TProcessHandle::TProcessHandle(const TProcessHandleImpl& hprociSrc) :

    m_phprociThis(0)
{
    m_phprociThis = new TProcessHandleImpl;
    m_phprociThis->hProcess = hprociSrc.hProcess;
    m_phprociThis->pidThis = hprociSrc.pidThis;
}

TProcessHandle::~TProcessHandle()
{
    delete m_phprociThis;
    m_phprociThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TProcessHandle& TProcessHandle::operator=(const TProcessHandle& hprocToAssign)
{
    if (this == &hprocToAssign)
        return *this;

    m_phprociThis->hProcess = hprocToAssign.m_phprociThis->hProcess;
    m_phprociThis->pidThis = hprocToAssign.m_phprociThis->pidThis;

    return *this;
}


tCIDLib::TBoolean
TProcessHandle::operator==(const TProcessHandle& hprocToCompare) const
{
    return ((m_phprociThis->hProcess == hprocToCompare.m_phprociThis->hProcess)
    &&      (m_phprociThis->pidThis == hprocToCompare.m_phprociThis->pidThis));
}

tCIDLib::TBoolean
TProcessHandle::operator!=(const TProcessHandle& hprocToCompare) const
{
    return !operator==(hprocToCompare);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TProcessHandle::bIsValid() const
{
    return (m_phprociThis->hProcess != 0);
}

tCIDLib::TVoid TProcessHandle::Clear()
{
    m_phprociThis->hProcess = 0;
}

tCIDLib::TVoid
TProcessHandle::FormatToStr(        tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    tCIDLib::TZStr64    szTmp1;
    tCIDLib::TZStr64    szTmp2;

    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phprociThis->hProcess)
        , szTmp1
        , 64
        , tCIDLib::ERadices::Hex
    );
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phprociThis->pidThis)
        , szTmp2
        , 64
    );

    TRawStr::CopyCatStr(pszToFill, c4MaxChars, szTmp1, L"\\", szTmp2);
}

const TProcessHandleImpl& TProcessHandle::hprociThis() const
{
    return *m_phprociThis;
}




// ---------------------------------------------------------------------------
//  TKrnlProcess functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlProcess::bActivateProcess(const tCIDLib::TProcessId pidToActivate)
{
    //
    //  Enumerate top level windows and look for one in the passed process.
    //  If we find more than one, take the one that is a popup.
    //
    CIDKernel_Process_Win32::TFindWndFiltered FindData = {0};

    FindData.pidTarget = pidToActivate;
    FindData.bOwned = kCIDLib::False;
    FindData.c4DontWantStyles = WS_DISABLED | WS_CHILD;
    FindData.c4WantStyles = WS_TILED | WS_VISIBLE;
    FindData.c4DontWantExStyles = WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

    if (!::EnumWindows(&bEnumWndFiltered, (LPARAM)&FindData) && !FindData.hwndFound)
        return kCIDLib::False;

    if (!FindData.hwndFound)
        return kCIDLib::False;

    //
    //  Get our thread id and that of the target window we want to
    //  activate.
    //
    const tCIDLib::TThreadId tidTar = ::GetWindowThreadProcessId(FindData.hwndFound, 0);
    const tCIDLib::TThreadId tidThis = ::GetCurrentThreadId();

    //
    //  And attach the two threads together, bring the target forward,
    //  and detach.
    //
    ::AttachThreadInput(tidThis, tidTar, 1);

    // If minimized, then restore it
    if (FindData.c4FoundStyles & WS_MINIMIZE)
    {
        WINDOWPLACEMENT WndPlace = {0};
        WndPlace.length = sizeof(WINDOWPLACEMENT);
        ::GetWindowPlacement(FindData.hwndFound, &WndPlace);

        // If minimized, then show normal, else just show in current state
        if (WndPlace.showCmd == SW_SHOWMINIMIZED)
            WndPlace.showCmd = SW_RESTORE;
        else
            WndPlace.showCmd = SW_SHOW;
        ::SetWindowPlacement(FindData.hwndFound, &WndPlace);
    }

    // And bring to the foreground
    ::SetForegroundWindow(FindData.hwndFound);

    ::AttachThreadInput(tidThis, tidTar, 0);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlProcess::bCheckSingleInstanceInfo( const   tCIDLib::TCh* const     pszResName
                                        ,       tCIDLib::TBoolean&      bFound
                                        ,       TKrnlExtProcess* const  pkextpTarget)
{
    tCIDLib::TBoolean bRetVal = kCIDLib::True;
    bFound = kCIDLib::False;
    TKrnlMutex kmtxSync(L"Mutex_CQSL_SingleInstanceInfo");
    try
    {
        //
        //  We use a named mutex to syncrhonize the creation and initialization
        //  of the buffer. Otherwise, two instances could come up and one would
        //  fail because it sees the buffer exists, but the contents have not
        //  been set yet.
        //
        tCIDLib::TBoolean bCreated;
        if (!kmtxSync.bOpenOrCreate(tCIDLib::ELockStates::Locked, bCreated))
            return kCIDLib::False;

        //
        //  Find an existing shared memory buffer with this resource name if
        //  it exists. Not finding it is not an error since we ask to open
        //  only if it exists.
        //
        TKrnlSharedMemBuf ksmbTest;
        bRetVal = ksmbTest.bAlloc
        (
            pszResName
            , 4096
            , tCIDLib::EAllocTypes::Commit
            , tCIDLib::EMemAccFlags::ReadWrite
            , bFound
            , tCIDLib::ECreateActs::OpenIfExists
        );

        //
        //  If no error, flip the found flag since it's really returning
        //  whether or not it created it.
        //
        if (bRetVal)
            bFound = !bFound;

        //
        //  If we found an existing one, and they provided an external process
        //  object, then set up that object.
        //
        if (bRetVal && bFound && pkextpTarget)
        {
            // Look at the memory as the structure it contains
            CIDKernel_Process_Win32::TSingleInstInfo* pInfo
            (
                reinterpret_cast<CIDKernel_Process_Win32::TSingleInstInfo*>(ksmbTest.pData())
            );

            //
            //  And set up a process handle with the process info. We have to dup
            //  the handle because the caller will adopt it.
            //
            HANDLE hNew = 0;
            if (!::DuplicateHandle
            (
                pInfo->hprocSingle
                , TKrnlProcess::hprocThis().hprociThis().hProcess
                , TKrnlProcess::hprocThis().hprociThis().hProcess
                , &hNew
                , 0
                , 0
                , DUPLICATE_SAME_ACCESS))
            {
                TKrnlError::SetLastHostError(::GetLastError());
                bRetVal = kCIDLib::False;
            }

            if (bRetVal)
            {
                TProcessHandleImpl hprociExt;
                hprociExt.hProcess = hNew;
                hprociExt.pidThis = pInfo->pidSingle;

                //
                //  If this fails, he will release the handle since he's
                //  adopting it.
                //
                TProcessHandle hprocExt(hprociExt);
                bRetVal = pkextpTarget->bAdoptProcess(hprocExt);
            }
        }
    }

    catch(...)
    {
        // Fall through with a failure return
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_SystemException);
        bRetVal = kCIDLib::False;

        // MUST FALL through to unlock
    }

    // Release the sync mutex now. It will destruct on the way out
    kmtxSync.bUnlock();

    return bRetVal;
}



tCIDLib::TBoolean
TKrnlProcess::bSetPriorityClass(const tCIDLib::EPrioClasses eClass)
{
    // Convert the enum into the host priority class
    tCIDLib::TCard4 c4Prio;
    if (eClass == tCIDLib::EPrioClasses::IdleTime)
        c4Prio = IDLE_PRIORITY_CLASS;
    else if (eClass == tCIDLib::EPrioClasses::Normal)
        c4Prio = NORMAL_PRIORITY_CLASS;
    else if (eClass == tCIDLib::EPrioClasses::High)
        c4Prio = HIGH_PRIORITY_CLASS;
    else if (eClass == tCIDLib::EPrioClasses::RealTime)
        c4Prio = REALTIME_PRIORITY_CLASS;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_InvalidPriority);
        return kCIDLib::False;
    }

    if (!::SetPriorityClass(::GetCurrentProcess(), c4Prio))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlProcess::bSetSingleInstanceInfo(   const   tCIDLib::TCh* const pszResName
                                        , const tCIDLib::TBoolean   bBringOldForward
                                        ,       tCIDLib::TBoolean&  bFoundOld)
{
    tCIDLib::TBoolean bRetVal = kCIDLib::True;
    bFoundOld = kCIDLib::False;
    TKrnlMutex kmtxSync(L"Mutex_CQSL_SingleInstanceInfo");
    try
    {
        //
        //  We use a named mutex to syncrhonize the creation and initialization
        //  of the buffer. Otherwise, two instances could come up and one would
        //  fail because it sees the buffer exists, but the contents have not
        //  been set yet.
        //
        tCIDLib::TBoolean bCreated;
        if (!kmtxSync.bOpenOrCreate(tCIDLib::ELockStates::Locked, bCreated))
            return kCIDLib::False;

        //
        //  If we didn't create it, then we have to block on it since we
        //  did not get the required initial lock state.
        //
        if (!bCreated)
        {
            //
            //  We should never have to wait more than a split second but
            //  just in case, let it block for a bit before giving up.
            //
            if (!kmtxSync.bLock(5000))
                return kCIDLib::False;
        }

        //
        //  Find an existing shared memory buffer with this resource name, or
        //  create a new one if none exists.
        //
        TKrnlSharedMemBuf ksmbTest;
        tCIDLib::TBoolean bRes = ksmbTest.bAlloc
        (
            pszResName
            , 4096
            , tCIDLib::EAllocTypes::Commit
            , tCIDLib::EMemAccFlags::ReadWrite
            , bFoundOld
            , tCIDLib::ECreateActs::OpenOrCreate
        );

        if (!bRes)
        {
            // Didn't find it and couldn't create it due to some error
            kmtxSync.bUnlock();
            return kCIDLib::False;
        }

        //
        //  The found old flag was passed in as the 'created it' output, so
        //  it's now the opposite of what it should be. Flip it the other
        //  way. If we created it, then we didn't find the old one.
        //
        bFoundOld = !bFoundOld;

        // Look at the memory via our little structure
        CIDKernel_Process_Win32::TSingleInstInfo* pInfo
        (
            reinterpret_cast<CIDKernel_Process_Win32::TSingleInstInfo*>(ksmbTest.pData())
        );

        //
        //  If we didn't find an old one, then we created it, and need to
        //  store ourself as the single instance. Else, if they asked us to,
        //  bring forward the old one.
        //
        if (!bFoundOld)
        {
            pInfo->c4Version = CIDKernel_Process_Win32::c4SingleInstVer;
            pInfo->hprocSingle = ::GetCurrentProcess();
            pInfo->pidSingle = ::GetCurrentProcessId();

            //
            //  And now insure that the shared memory buffer will live as
            //  long as this process by duplicating the handle. When our
            //  object goes out of scope below, we'll be left with this
            //  memory handle holding our info till we die
            //
            CIDKernel_Process_Win32::pksmbHold = new TKrnlSharedMemBuf;
            CIDKernel_Process_Win32::pksmbHold->bDuplicate(ksmbTest);
        }
         else if (bBringOldForward)
        {
            // Make sure the version info is valid
            if (pInfo->c4Version == CIDKernel_Process_Win32::c4SingleInstVer)
            {
                // Call the activation method, return his return status
                bRetVal = bActivateProcess(pInfo->pidSingle);
            }
             else
            {
                // It's not set the return to false and store the error
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcProc_BadSingleInstInfo);
                bRetVal = kCIDLib::False;
            }
        }
    }

    catch(...)
    {
        // Fall through with a failure return
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_SystemException);
        bRetVal = kCIDLib::False;
    }

    // Release the sync mutex now. It will destruct on the way out
    kmtxSync.bUnlock();

    // Return success or failure
    return bRetVal;
}


tCIDLib::TVoid TKrnlProcess::ExitProcess(const tCIDLib::EExitCodes eExitCode)
{
    ::ExitProcess(tCIDLib::c4EnumOrd(eExitCode));
}


// Fault in our process handle
const TProcessHandle& TKrnlProcess::hprocThis()
{
    static TProcessHandle hprocThis
    (
        TProcessHandleImpl(::GetCurrentProcess(), ::GetCurrentProcessId())
    );
    return hprocThis;
}


tCIDLib::TProcessId TKrnlProcess::pidThis()
{
    return ::GetCurrentProcessId();
}
