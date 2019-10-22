//
// FILE NAME: CIDKernel_CriticalSection_Win32.Cpp
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
//  This file the Win32 specific implementation of the TKrnlCritSec class.
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


// ---------------------------------------------------------------------------
//   CLASS: TKrnlCritSec
//  PREFIX: kcrs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlCritSec: Public data types
// ---------------------------------------------------------------------------
struct TKrnlCritSec::TPlatData
{
    alignas(kCIDLib::c4CacheAlign) CRITICAL_SECTION CritSec;
};


// ---------------------------------------------------------------------------
//  TKrnlCritSec: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlCritSec::TKrnlCritSec() :

    m_pPlatData(new TPlatData)
{
    ::InitializeCriticalSection(&m_pPlatData->CritSec);
}

TKrnlCritSec::~TKrnlCritSec()
{
    if (m_pPlatData)
    {
        ::DeleteCriticalSection(&m_pPlatData->CritSec);

        delete m_pPlatData;
        m_pPlatData = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TKrnlCritSec: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlCritSec::Enter() const
{
    ::EnterCriticalSection(&m_pPlatData->CritSec);
}


tCIDLib::TVoid TKrnlCritSec::Exit() const
{
    ::LeaveCriticalSection(&m_pPlatData->CritSec);
}

