//
// FILE NAME: CIDKernel_SafeCounter_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 11/21/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

    __i4Counter(0)
{
    __pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(__pLockData), 0);
}

TKrnlSafeInt4Counter::TKrnlSafeInt4Counter(const tCIDLib::TInt4 i4InitVal) :

    __i4Counter(i4InitVal)
{
    __pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(__pLockData), 0);
}

TKrnlSafeInt4Counter::~TKrnlSafeInt4Counter()
{
    pthread_mutex_t* pMtx = static_cast<pthread_mutex_t*>(__pLockData);
    ::pthread_mutex_destroy(pMtx);
    delete pMtx;
}



// ---------------------------------------------------------------------------
//  TKrnlSafeInt4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSafeInt4Counter::bDec()
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TBoolean bResult = kCIDLib::True;
    if (--__i4Counter)
        bResult = kCIDLib::False;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return bResult;
}


tCIDLib::TBoolean TKrnlSafeInt4Counter::bInc()
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TBoolean bResult = kCIDLib::True;
    if (++__i4Counter)
        bResult = kCIDLib::False;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return bResult;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4AddTo(const tCIDLib::TInt4 i4ToAdd)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TInt4 i4Return = __i4Counter;
    __i4Counter += i4ToAdd;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return i4Return;
}

tCIDLib::TInt4
TKrnlSafeInt4Counter::i4CompareAndExchange( const   tCIDLib::TInt4  i4New
                                            , const tCIDLib::TInt4  i4Compare)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TInt4 i4Return = __i4Counter;

    if (__i4Counter == i4Compare)
        __i4Counter = i4New;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return i4Return;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Exchange(const tCIDLib::TInt4 i4New)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TInt4 i4Return = __i4Counter;
    __i4Counter = i4New;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return i4Return;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4SubFrom(const tCIDLib::TInt4 i4ToSub)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TInt4 i4Return = __i4Counter;
    __i4Counter -= i4ToSub;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return i4Return;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Value() const
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TInt4 i4Return = __i4Counter;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return i4Return;
}





// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSafeCard4Counter::TKrnlSafeCard4Counter() :

    __c4Counter(0)
{
    __pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(__pLockData), 0);
}

TKrnlSafeCard4Counter::TKrnlSafeCard4Counter(const tCIDLib::TCard4 c4InitVal) :

    __c4Counter(c4InitVal)
{
    __pLockData = new pthread_mutex_t;
    ::pthread_mutex_init(static_cast<pthread_mutex_t*>(__pLockData), 0);
}

TKrnlSafeCard4Counter::~TKrnlSafeCard4Counter()
{
    pthread_mutex_t* pMtx = static_cast<pthread_mutex_t*>(__pLockData);
    ::pthread_mutex_destroy(pMtx);
    delete pMtx;
}


// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSafeCard4Counter::bDec()
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TBoolean bResult = kCIDLib::True;
    if (--__c4Counter)
        bResult = kCIDLib::False;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return bResult;
}


tCIDLib::TBoolean TKrnlSafeCard4Counter::bInc()
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TBoolean bResult = kCIDLib::True;
    if (++__c4Counter)
        bResult = kCIDLib::False;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return bResult;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4AddTo(const tCIDLib::TCard4 c4ToAdd)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TCard4 c4Return = __c4Counter;
    __c4Counter += c4ToAdd;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return c4Return;
}

tCIDLib::TCard4
TKrnlSafeCard4Counter::c4CompareAndExchange(const   tCIDLib::TCard4  c4New
                                            , const tCIDLib::TCard4  c4Compare)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TCard4 c4Return = __c4Counter;

    if (__c4Counter == c4Compare)
        __c4Counter = c4New;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return c4Return;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Exchange(const tCIDLib::TCard4 c4New)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TCard4 c4Return = __c4Counter;
    __c4Counter = c4New;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return c4Return;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4SubFrom(const tCIDLib::TCard4 c4ToSub)
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TCard4 c4Return = __c4Counter;
    __c4Counter -= c4ToSub;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return c4Return;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Value() const
{
    ::pthread_mutex_lock(static_cast<pthread_mutex_t*>(__pLockData));

    tCIDLib::TCard4 c4Return = __c4Counter;

    ::pthread_mutex_unlock(static_cast<pthread_mutex_t*>(__pLockData));

    return c4Return;
}
