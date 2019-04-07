//
// FILE NAME: CIDKernel_Process_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/21/1998
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
//  This file provides the Linux specific implementation of the TKrnlProcess
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
//   CLASS: TProcessHandle
//  PREFIX: hproc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TProcessHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TProcessHandle::TProcessHandle() :
    __phprociThis(0)
{
    __phprociThis = new TProcessHandleImpl;
    __phprociThis->pidThis = 0;
    __phprociThis->eExit = tCIDLib::EExitCodes::Normal;
    __phprociThis->bAlreadyClean = kCIDLib::False;
}

TProcessHandle::TProcessHandle(const TProcessHandle& hprocToCopy) :
    __phprociThis(0)
{
    __phprociThis = new TProcessHandleImpl;
    __phprociThis->pidThis = hprocToCopy.__phprociThis->pidThis;
    __phprociThis->eExit = hprocToCopy.__phprociThis->eExit;
    __phprociThis->bAlreadyClean = hprocToCopy.__phprociThis->bAlreadyClean;
}

TProcessHandle::TProcessHandle(const TProcessHandleImpl& hprociSrc) :
    __phprociThis(0)
{
    __phprociThis = new TProcessHandleImpl;
    __phprociThis->pidThis = hprociSrc.pidThis;
    __phprociThis->eExit = hprociSrc.eExit;
    __phprociThis->bAlreadyClean = hprociSrc.bAlreadyClean;
}

TProcessHandle::~TProcessHandle()
{
    delete __phprociThis;
    __phprociThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TProcessHandle& TProcessHandle::operator=(const TProcessHandle& hprocToAssign)
{
    if (this == &hprocToAssign)
        return *this;

    __phprociThis->pidThis = hprocToAssign.__phprociThis->pidThis;
    __phprociThis->eExit = hprocToAssign.__phprociThis->eExit;
    __phprociThis->bAlreadyClean = hprocToAssign.__phprociThis->bAlreadyClean;

    return *this;
}


tCIDLib::TBoolean
TProcessHandle::operator==(const TProcessHandle& hprocToCompare) const
{
    return (__phprociThis->pidThis == hprocToCompare.__phprociThis->pidThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TProcessHandle::bValid() const
{
    return (__phprociThis->pidThis != 0);
}

tCIDLib::TVoid TProcessHandle::Clear()
{
    __phprociThis->pidThis = 0;
    __phprociThis->eExit = tCIDLib::EExitCodes::Normal;
    __phprociThis->bAlreadyClean = kCIDLib::False;
}

tCIDLib::TProcessId TProcessHandle::pidThis() const
{
    return __phprociThis->pidThis;
}

tCIDLib::TVoid
TProcessHandle::FormatToStr(        tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(__phprociThis->pidThis)
        , pszToFill
        , c4MaxChars
    );
}



// ---------------------------------------------------------------------------
//  TKrnlProcess functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlProcess::bSetPriorityClass(const tCIDLib::EPrioClasses eClass)
{
    if (eClass != tCIDLib::EPrioClasses::Normal
        && eClass != tCIDLib::EPrioClasses::RealTime)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    struct sched_param SchedParam;
    tCIDLib::TSInt iPolicy;

    if (eClass == tCIDLib::EPrioClasses::Normal)
    {
        iPolicy = SCHED_OTHER;
        SchedParam.sched_priority = 0;
    }
    else
    {
        iPolicy = SCHED_RR;
        SchedParam.sched_priority = ::sched_get_priority_min(SCHED_RR);
    }

    if (::sched_setscheduler(hprocThis().pidThis(),
                             iPolicy,
                             &SchedParam))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TVoid TKrnlProcess::ExitProcess(const tCIDLib::EExitCodes eExitCode)
{
    ::exit(eExitCode);
}


const TProcessHandle& TKrnlProcess::hprocThis()
{
    static TProcessHandle* phprocThis = 0;

    //
    //  If the process handle object has not been created yet, then create it
    //  with appropriate synchronization.
    //
    if (!phprocThis)
    {
        TBaseLock lockInit;
        if (!phprocThis)
        {
            TProcessHandleImpl hprociTmp;
            hprociTmp.pidThis = ::getpid();

            ::InterlockedExchange
            (
                &phprocThis, new TProcessHandle(hprociTmp)
            );
        }
    }
    return *phprocThis;
}


tCIDLib::TProcessId TKrnlProcess::pidThis()
{
    return hprocThis().pidThis();
}
