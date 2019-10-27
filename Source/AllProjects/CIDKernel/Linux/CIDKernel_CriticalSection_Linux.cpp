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
//  TKrnlCritSec: Public data types
// ---------------------------------------------------------------------------

// Define our own version of the platform data
struct TKrnlCritSec::TPlatData
{
    TKrnlLinux::TRecursiveMutex*  prmtxThis;
};


// ---------------------------------------------------------------------------
//  TKrnlCritSec: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlCritSec::TKrnlCritSec() :

    m_pPlatData(new TPlatData)
{
    TKrnlLinux::TRecursiveMutex* prmtxThis = new TKrnlLinux::TRecursiveMutex();
    prmtxThis->iInitialize();
    m_pPlatData->prmtxThis = prmtxThis;
}

TKrnlCritSec::~TKrnlCritSec()
{
    m_pPlatData->prmtxThis->iDestroy();
    delete m_pPlatData;
}


// ---------------------------------------------------------------------------
//  TKrnlCritSec: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlCritSec::Enter() const
{
    m_pPlatData->prmtxThis->iLock();
}


tCIDLib::TVoid TKrnlCritSec::Exit() const
{
    m_pPlatData->prmtxThis->iUnlock();
}
