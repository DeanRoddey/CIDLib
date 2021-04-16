//
// FILE NAME: CIDKernel_SafeCounter_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1998
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
//  This file provides the Win32 specific implementation of the classes
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
//   CLASS: TKrnlSafeCard4Counter
//  PREFIX: scnt
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSafeCard4Counter::TKrnlSafeCard4Counter() :

    m_c4Counter(0)
    , m_pLockData(nullptr)
{
}

TKrnlSafeCard4Counter::TKrnlSafeCard4Counter(const tCIDLib::TCard4 c4InitVal) :

    m_c4Counter(c4InitVal)
    , m_pLockData(nullptr)
{
}

TKrnlSafeCard4Counter::~TKrnlSafeCard4Counter()
{
}


// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TKrnlSafeCard4Counter::c4AddTo(const tCIDLib::TCard4 c4ToAdd)
{
    return ::InterlockedAdd((LONG*)&m_c4Counter, LONG(c4ToAdd));
}

tCIDLib::TCard4
TKrnlSafeCard4Counter::c4CompareAndExchange(const   tCIDLib::TCard4  c4New
                                            , const tCIDLib::TCard4  c4Compare)
{
    return tCIDLib::TCard4
    (
        ::InterlockedCompareExchange((LONG*)&m_c4Counter, (LONG)c4New, (LONG)c4Compare)
    );
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Dec()
{
    return tCIDLib::TCard4(::InterlockedDecrement((LONG*)&m_c4Counter));
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Exchange(const tCIDLib::TCard4 c4New)
{
    return tCIDLib::TCard4(::InterlockedExchange((LONG*)&m_c4Counter, (LONG)c4New));
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Inc()
{
    return tCIDLib::TCard4(::InterlockedIncrement((LONG*)&m_c4Counter));
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4SubFrom(const tCIDLib::TCard4 c4ToSub)
{
    // There's no substract to add a negative value
    return  ::InterlockedAdd((LONG*)&m_c4Counter, LONG(c4ToSub) * -1);
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Value() const
{
    // Do an interlocked add of zero to get the current value
    return ::InterlockedAdd((LONG*)&m_c4Counter, 0);
}





// ---------------------------------------------------------------------------
//   CLASS: TKrnlSafeInt4Counter
//  PREFIX: scnt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSafeInt4Counter: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSafeInt4Counter::TKrnlSafeInt4Counter() :

    m_i4Counter(0)
    , m_pLockData(nullptr)
{
}

TKrnlSafeInt4Counter::TKrnlSafeInt4Counter(const tCIDLib::TInt4 i4InitVal) :

    m_i4Counter(i4InitVal)
    , m_pLockData(nullptr)
{
}

TKrnlSafeInt4Counter::~TKrnlSafeInt4Counter()
{
}



// ---------------------------------------------------------------------------
//  TKrnlSafeInt4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Dec()
{
    return ::InterlockedDecrement(&m_i4Counter);
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Inc()
{
    return ::InterlockedIncrement(&m_i4Counter);
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4AddTo(const tCIDLib::TInt4 i4ToAdd)
{
    return ::InterlockedAdd(&m_i4Counter, i4ToAdd);
}

tCIDLib::TInt4
TKrnlSafeInt4Counter::i4CompareAndExchange( const   tCIDLib::TInt4  i4New
                                            , const tCIDLib::TInt4  i4Compare)
{
    return ::InterlockedCompareExchange(&m_i4Counter, i4New, i4Compare);
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Exchange(const tCIDLib::TInt4 i4New)
{
    return ::InterlockedExchange(&m_i4Counter, i4New);
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4SubFrom(const tCIDLib::TInt4 i4ToSub)
{
    return ::InterlockedAdd(&m_i4Counter, -i4ToSub);
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Value() const
{
    //
    //  Do an add of zero to get the value. We have to cast the const'ness off
    //  to do this.
    //
    return ::InterlockedAdd
    (
        const_cast<tCIDLib::TInt4*>(&m_i4Counter), 0
    );
}

