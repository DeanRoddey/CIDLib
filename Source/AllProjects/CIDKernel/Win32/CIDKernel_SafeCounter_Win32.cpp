//
// FILE NAME: CIDKernel_SafeCounter_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
//  TKrnlSafeInt4Counter: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSafeInt4Counter::TKrnlSafeInt4Counter() :

    m_pi4Counter(0)
    , m_pLockData(0)
{
    // On this platform we don't use the lock data
    m_pi4Counter = (tCIDLib::TInt4*)::_aligned_malloc(sizeof(tCIDLib::TInt4), 32);
    *m_pi4Counter = 0;
}

TKrnlSafeInt4Counter::TKrnlSafeInt4Counter(const tCIDLib::TInt4 i4InitVal) :

    m_pi4Counter(0)
    , m_pLockData(0)
{
    // On this platform we don't use the lock data
    m_pi4Counter = (tCIDLib::TInt4*)::_aligned_malloc(sizeof(tCIDLib::TInt4), 32);

    // Store the initial value
    *m_pi4Counter = i4InitVal;
}

TKrnlSafeInt4Counter::~TKrnlSafeInt4Counter()
{
    // On this platform we don't use the lock data
    ::_aligned_free((void*)m_pi4Counter);
}



// ---------------------------------------------------------------------------
//  TKrnlSafeInt4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSafeInt4Counter::bDec()
{
    tCIDLib::TBoolean           bRet = kCIDLib::True;
    volatile tCIDLib::TInt4*    pi4Update = m_pi4Counter;
    __asm
    {
        mov         ebx, pi4Update          ; Get address of the counter loaded
        lock dec    [ebx]                   ; Do an interlocked decrement
        jz          Done                    ; If zero result, then return True
        mov         bRet, 0                 ; Otherwise, set up a false return
    Done:
    }
    return bRet;
}


tCIDLib::TBoolean TKrnlSafeInt4Counter::bInc()
{
    tCIDLib::TBoolean           bRet = kCIDLib::True;
    volatile tCIDLib::TInt4*    pi4Update = m_pi4Counter;
    __asm
    {
        mov         ebx, pi4Update          ; Get address of the counter loaded
        lock inc    [ebx]                   ; Do an interlocked increment
        jz          Done                    ; If zero, then return true
        mov         bRet, 0                 ; Otherwise, set up a false return
    Done:
    }
    return bRet;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4AddTo(const tCIDLib::TInt4 i4ToAdd)
{
    tCIDLib::TInt4           i4Ret;
    volatile tCIDLib::TInt4* pi4Update = m_pi4Counter;
    __asm
    {
        mov         ebx, pi4Update
        mov         eax, i4ToAdd            ; Load the value to add
        lock xadd   [ebx], eax              ; And do an interlocked add
        mov         i4Ret, eax              ; Return the result
    }
    return i4Ret;
}

tCIDLib::TInt4
TKrnlSafeInt4Counter::i4CompareAndExchange( const   tCIDLib::TInt4  i4New
                                            , const tCIDLib::TInt4  i4Compare)
{
    tCIDLib::TInt4           i4Ret;
    volatile tCIDLib::TInt4* pi4Update = m_pi4Counter;
    __asm
    {
        mov         ebx, pi4Update
        mov         eax, i4Compare          ; Load the value to compare
        mov         edx, i4New              ; And the value to exchange
        lock cmpxchg [ebx], edx             ; Do the locked cmp/xch
        mov         i4Ret, eax              ; Return the result
    }
    return i4Ret;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Exchange(const tCIDLib::TInt4 i4New)
{
    tCIDLib::TInt4           i4Ret;
    volatile tCIDLib::TInt4* pi4Update = m_pi4Counter;
    __asm
    {
        mov         ebx, pi4Update          ; Load address of the counter
        mov         eax, i4New              ; Load the value to swap in
        lock xchg   [ebx], eax              ; And do an interlocked exchange
        mov         i4Ret, eax              ; Return the result
    }
    return i4Ret;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4SubFrom(const tCIDLib::TInt4 i4ToSub)
{
    tCIDLib::TInt4           i4Ret;
    volatile tCIDLib::TInt4* pi4Update = m_pi4Counter;
    __asm
    {
        mov         ebx, pi4Update          ; Load address of the counter
        mov         eax, i4ToSub            ; Load the value to sub
        neg         eax                     ; Negate it so we can add it
        lock xadd   [ebx], eax              ; And do an interlocked add
        mov         i4Ret, eax              ; Put the value back for return
    }
    return i4Ret;
}


tCIDLib::TInt4 TKrnlSafeInt4Counter::i4Value() const
{
    tCIDLib::TInt4                 i4Ret;
    volatile const tCIDLib::TInt4* pi4Update = m_pi4Counter;
    __asm
    {
        mov         ebx, pi4Update          ; Load address of the counter
        xor         eax, eax                ; Add zero as a way to get value
        lock xadd   [ebx], eax              ; And do an interlocked add
        mov         i4Ret, eax              ; Put the value back for return
    }
    return i4Ret;
}





// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSafeCard4Counter::TKrnlSafeCard4Counter() :

    m_pc4Counter(0)
    , m_pLockData(0)
{
    // On this platform we don't use the lock data
    m_pc4Counter = (tCIDLib::TCard4*)::_aligned_malloc(sizeof(tCIDLib::TCard4), 32);
    *m_pc4Counter = 0;

}

TKrnlSafeCard4Counter::TKrnlSafeCard4Counter(const tCIDLib::TCard4 c4InitVal) :

    m_pc4Counter(0)
    , m_pLockData(0)
{
    // On this platform we don't use the lock data
    m_pc4Counter = (tCIDLib::TCard4*)::_aligned_malloc(sizeof(tCIDLib::TCard4), 32);
    *m_pc4Counter = c4InitVal;
}

TKrnlSafeCard4Counter::~TKrnlSafeCard4Counter()
{
    // On this platform we don't use the lock data
    ::_aligned_free((void*)m_pc4Counter);
}


// ---------------------------------------------------------------------------
//  TKrnlSafeCard4Counter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSafeCard4Counter::bDec()
{
    tCIDLib::TBoolean           bRet = kCIDLib::True;
    volatile tCIDLib::TCard4*   pc4Update = m_pc4Counter;
    __asm
    {
        mov         ebx, pc4Update          ; Get address of the counter loaded
        lock dec    [ebx]                   ; Do an interlocked decrement
        jz          Done                    ; If zero, then return true
        mov         bRet, 0                 ; Otherwise, set up a false return
    Done:
    }
    return bRet;
}


tCIDLib::TBoolean TKrnlSafeCard4Counter::bInc()
{
    tCIDLib::TBoolean           bRet = kCIDLib::True;
    volatile tCIDLib::TCard4*   pc4Update = m_pc4Counter;
    __asm
    {
        mov         ebx, pc4Update          ; Get address of the counter loaded
        lock inc    [ebx]                   ; Do an interlocked increment
        jz          Done                    ; If zero, then return true
        mov         bRet, 0                 ; Otherwise, set up a false return
    Done:
    }
    return bRet;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4AddTo(const tCIDLib::TCard4 c4ToAdd)
{
    tCIDLib::TCard4             c4Ret;
    volatile tCIDLib::TCard4*   pc4Update = m_pc4Counter;
    __asm
    {
        mov         ebx, pc4Update
        mov         eax, c4ToAdd            ; Load the value to add
        lock xadd   [ebx], eax              ; And do an interlocked add
        mov         c4Ret, eax              ; Return result
    }
    return c4Ret;
}

tCIDLib::TCard4
TKrnlSafeCard4Counter::c4CompareAndExchange(const   tCIDLib::TCard4  c4New
                                            , const tCIDLib::TCard4  c4Compare)
{
    tCIDLib::TCard4             c4Ret;
    volatile tCIDLib::TCard4*   pc4Update = m_pc4Counter;
    __asm
    {
        mov         ebx, pc4Update          ; Load EA of counter
        mov         eax, c4Compare          ; Load the value to compare
        mov         edx, c4New              ; And the value to exchange
        lock cmpxchg [ebx], edx             ; Do the locked cmp/xch
        mov         c4Ret, eax              ; Return result
    }
    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Exchange(const tCIDLib::TCard4 c4New)
{
    tCIDLib::TCard4           c4Ret;
    volatile tCIDLib::TCard4* pc4Update = m_pc4Counter;
    __asm
    {
        mov         ebx, pc4Update          ; Load address of the counter
        mov         eax, c4New              ; Load the value to swap in
        lock xchg   [ebx], eax              ; And do an interlocked exchange
        mov         c4Ret, eax              ; Return the result
    }
    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4SubFrom(const tCIDLib::TCard4 c4ToSub)
{
    tCIDLib::TCard4           c4Ret;
    volatile tCIDLib::TCard4* pc4Update = m_pc4Counter;
    __asm
    {
        mov         ebx, pc4Update          ; Load address of the counter
        mov         eax, c4ToSub            ; Load the value to sub
        neg         eax                     ; Negate it so we can add it
        lock xadd   [ebx], eax              ; And do an interlocked add
        mov         c4Ret, eax              ; Put the value back for return
    }
    return c4Ret;
}


tCIDLib::TCard4 TKrnlSafeCard4Counter::c4Value() const
{
    tCIDLib::TCard4                 c4Ret;
    volatile const tCIDLib::TCard4* pc4Update = m_pc4Counter;
    __asm
    {
        mov         ebx, pc4Update          ; Load address of the counter
        xor         eax, eax                ; Add zero as a way to get value
        lock xadd   [ebx], eax              ; And do an interlocked add
        mov         c4Ret, eax              ; Put the value back for return
    }
    return c4Ret;
}


