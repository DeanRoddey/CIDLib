//
// FILE NAME: CIDKernel_SafeCounter_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 11/21/1998
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
//  This file provides the Linux specific implementation of the classes
//  TKrnlSafeInt4Counter and TKrnlSafeCardCounter.
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
//  TKrnlSafeInt4Counter: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSafeInt4Counter::TKrnlSafeInt4Counter() :

    m_pi4Counter(new tCIDLib::TInt4(0))
    , m_pLockData(nullptr)
{
    m_pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(m_pLockData), 0);
}

TKrnlSafeInt4Counter::TKrnlSafeInt4Counter(const tCIDLib::TInt4 i4InitVal) :

    m_pi4Counter(new tCIDLib::TInt4(i4InitVal))
    , m_pLockData(nullptr)
{
    m_pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(m_pLockData), 0);
}

TKrnlSafeInt4Counter::~TKrnlSafeInt4Counter()
{
    delete m_pi4Counter;
    m_pi4Counter = nullptr;

    pthread_mutex_t* pMtx = static_cast<pthread_mutex_t*>(m_pLockData);
    ::pthread_mutex_destroy(pMtx);
    delete pMtx;
    m_pLockData = nullptr;
}



// ---------------------------------------------------------------------------
//  TKrnlSafeInt4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Dec()
{
    tCIDLib::TInt4 i4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    --(*m_pi4Counter);
    i4Ret = *m_pi4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return i4Ret;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Inc()
{
    tCIDLib::TInt4 i4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    ++(*m_pi4Counter);
    i4Ret = *m_pi4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return i4Ret;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4AddTo(const tCIDLib::TInt4 i4ToAdd)
{
    tCIDLib::TInt4 i4Return;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    m_pi4Counter += i4ToAdd;
    i4Return = *m_pi4Counter;    
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return i4Return;
}

tCIDLib::TInt4
TKrnlSafeInt4Counter::i4CompareAndExchange( const   tCIDLib::TInt4  i4New
                                            , const tCIDLib::TInt4  i4Compare)
{
    tCIDLib::TInt4 i4Return;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    i4Return = *m_pi4Counter;
    if (*m_pi4Counter == i4Compare)
        *m_pi4Counter = i4New;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return i4Return;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Exchange(const tCIDLib::TInt4 i4New)
{
    tCIDLib::TInt4 i4Return;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    i4Return = *m_pi4Counter;
    *m_pi4Counter = i4New;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return i4Return;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4SubFrom(const tCIDLib::TInt4 i4ToSub)
{
    tCIDLib::TInt4 i4Return;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    *m_pi4Counter -= i4ToSub;
    i4Return = *m_pi4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return i4Return;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Value() const
{
    tCIDLib::TInt4 i4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    i4Ret = *m_pi4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return i4Ret;
}





// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSafeCard4Counter::TKrnlSafeCard4Counter() :

    m_pc4Counter(new tCIDLib::TCard4(0))
{
    m_pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(m_pLockData), 0);
}

TKrnlSafeCard4Counter::TKrnlSafeCard4Counter(const tCIDLib::TCard4 c4InitVal) :

    m_pc4Counter(new tCIDLib::TCard4(c4InitVal))
{
    m_pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(m_pLockData), 0);
}

TKrnlSafeCard4Counter::~TKrnlSafeCard4Counter()
{
    pthread_mutex_t* pMtx = static_cast<pthread_mutex_t*>(m_pLockData);
    ::pthread_mutex_destroy(pMtx);
    delete pMtx;
    pMtx = nullptr;

    delete m_pc4Counter;
    m_pc4Counter = nullptr;
}


// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Dec()
{
    tCIDLib::TCard4 c4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    --(*m_pc4Counter);
    c4Ret = *m_pc4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Inc()
{
    tCIDLib::TCard4 c4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    ++(*m_pc4Counter);
    c4Ret = *m_pc4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4AddTo(const tCIDLib::TCard4 c4ToAdd)
{
    tCIDLib::TCard4 c4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    m_pc4Counter += c4ToAdd;
    c4Ret = *m_pc4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return c4Ret;
}

tCIDLib::TCard4
TKrnlSafeCard4Counter::c4CompareAndExchange(const   tCIDLib::TCard4  c4New
                                            , const tCIDLib::TCard4  c4Compare)
{
    tCIDLib::TCard4 c4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    c4Ret = *m_pc4Counter;
    if (*m_pc4Counter == c4Compare)
        *m_pc4Counter = c4New;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Exchange(const tCIDLib::TCard4 c4New)
{
    tCIDLib::TCard4 c4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    c4Ret = *m_pc4Counter;
    *m_pc4Counter = c4New;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4SubFrom(const tCIDLib::TCard4 c4ToSub)
{
    tCIDLib::TCard4 c4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    *m_pc4Counter -= c4ToSub;
    c4Ret = *m_pc4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Value() const
{
    tCIDLib::TCard4 c4Ret;
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(m_pLockData));
    c4Ret = *m_pc4Counter;
    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(m_pLockData));

    return c4Ret;
}
