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

    m_phprociThis(nullptr)
{
    m_phprociThis = new TProcessHandleImpl;
    m_phprociThis->pidThis = 0;
    m_phprociThis->eExit = tCIDLib::EExitCodes::Normal;
    m_phprociThis->bAlreadyClean = kCIDLib::False;
}

TProcessHandle::TProcessHandle(const TProcessHandle& hprocSrc) :

    m_phprociThis(nullptr)
{
    m_phprociThis = new TProcessHandleImpl;
    m_phprociThis->pidThis = hprocSrc.m_phprociThis->pidThis;
    m_phprociThis->eExit = hprocSrc.m_phprociThis->eExit;
    m_phprociThis->bAlreadyClean = hprocSrc.m_phprociThis->bAlreadyClean;
}

TProcessHandle::TProcessHandle(const TProcessHandleImpl& hprociSrc) :

    m_phprociThis(nullptr)
{
    m_phprociThis = new TProcessHandleImpl;
    m_phprociThis->pidThis = hprociSrc.pidThis;
    m_phprociThis->eExit = hprociSrc.eExit;
    m_phprociThis->bAlreadyClean = hprociSrc.bAlreadyClean;
}

TProcessHandle::~TProcessHandle()
{
    delete m_phprociThis;
    m_phprociThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TProcessHandle& TProcessHandle::operator=(const TProcessHandle& hprocToAssign)
{
    if (this == &hprocToAssign)
        return *this;

    m_phprociThis->pidThis = hprocToAssign.m_phprociThis->pidThis;
    m_phprociThis->eExit = hprocToAssign.m_phprociThis->eExit;
    m_phprociThis->bAlreadyClean = hprocToAssign.m_phprociThis->bAlreadyClean;

    return *this;
}


tCIDLib::TBoolean
TProcessHandle::operator==(const TProcessHandle& hprocToCompare) const
{
    return (m_phprociThis->pidThis == hprocToCompare.m_phprociThis->pidThis);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TProcessHandle::bIsValid() const
{
    return (m_phprociThis->pidThis != 0);
}

tCIDLib::TVoid TProcessHandle::Clear()
{
    m_phprociThis->pidThis = 0;
    m_phprociThis->eExit = tCIDLib::EExitCodes::Normal;
    m_phprociThis->bAlreadyClean = kCIDLib::False;
}

tCIDLib::TProcessId TProcessHandle::pidThis() const
{
    return m_phprociThis->pidThis;
}

tCIDLib::TVoid
TProcessHandle::FormatToStr(        tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal(tCIDLib::TCard4(m_phprociThis->pidThis), pszToFill, c4MaxChars);
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
    ::exit(tCIDLib::c4EnumOrd(eExitCode));
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

            TRawMem::pExchangeRawPtr
            (
                reinterpret_cast<void**>(&phprocThis), new TProcessHandle(hprociTmp)
            );
        }
    }
    return *phprocThis;
}


tCIDLib::TProcessId TKrnlProcess::pidThis()
{
    return hprocThis().pidThis();
}
