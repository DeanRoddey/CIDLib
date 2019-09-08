//
// FILE NAME: CIDKernel_CriticalSection_Win32.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/09/1998
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
//  This file the Linux specific implementation of the TKrnlCritSec class.
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

// ---------------------------------------------------------------------------
//   CLASS: TKrnlCritSec
//  PREFIX: kcrs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlCritSec: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlCritSec::TKrnlCritSec() :
    m_pData(0)
{
    TKrnlLinux::TRecursiveMutex* prmtxThis =
        new TKrnlLinux::TRecursiveMutex();
    prmtxThis->iInitialize();
    m_pData = prmtxThis;
}

TKrnlCritSec::~TKrnlCritSec()
{
    TKrnlLinux::TRecursiveMutex* prmtxThis =
        static_cast<TKrnlLinux::TRecursiveMutex*>(m_pData);
    prmtxThis->iDestroy();
    delete prmtxThis;
}


// ---------------------------------------------------------------------------
//  TKrnlCritSec: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlCritSec::Enter() const
{
    TKrnlLinux::TRecursiveMutex* prmtxThis =
        static_cast<TKrnlLinux::TRecursiveMutex*>(m_pData);
    prmtxThis->iLock();
}


tCIDLib::TVoid TKrnlCritSec::Exit() const
{
    TKrnlLinux::TRecursiveMutex* prmtxThis =
        static_cast<TKrnlLinux::TRecursiveMutex*>(m_pData);
    prmtxThis->iUnlock();
}
