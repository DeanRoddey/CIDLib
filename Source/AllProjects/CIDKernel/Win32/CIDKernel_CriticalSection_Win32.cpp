//
// FILE NAME: CIDKernel_CriticalSection_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
//  TKrnlCritSec: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlCritSec::TKrnlCritSec() :

    m_pData(0)
{
    m_pData = static_cast<CRITICAL_SECTION*>(::malloc(sizeof(CRITICAL_SECTION)));
    ::InitializeCriticalSection((CRITICAL_SECTION*)m_pData);
}

TKrnlCritSec::~TKrnlCritSec()
{
    if (m_pData)
    {
        ::DeleteCriticalSection((CRITICAL_SECTION*)m_pData);
        ::free(m_pData);

        m_pData = 0;
    }
}


// ---------------------------------------------------------------------------
//  TKrnlCritSec: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlCritSec::Enter() const
{
    ::EnterCriticalSection(static_cast<CRITICAL_SECTION*>(m_pData));
}


tCIDLib::TVoid TKrnlCritSec::Exit() const
{
    ::LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(m_pData));
}



// ---------------------------------------------------------------------------
//  Custom allocators to ensure alignment
// ---------------------------------------------------------------------------
void* TKrnlCritSec::operator new(size_t t)
{
    return ::_aligned_malloc(t, 32);
}

void TKrnlCritSec::operator delete(void* ptr)
{
    ::_aligned_free(ptr);
}


