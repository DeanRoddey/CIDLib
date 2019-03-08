//
// FILE NAME: CIDKernel_RawMemory_Win32.Cpp
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
//  This file provides the Win32 specific implementation for the raw memory
//  interface.
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
#include    <memory.h>



// ---------------------------------------------------------------------------
//  TRawMem functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TRawMem::bCommitPages(          tCIDLib::TVoid*         pPageAdr
                        , const tCIDLib::TCard4         c4PageCount
                        , const tCIDLib::EMemAccFlags   eAccess)
{
    if (!::VirtualAlloc
    (
        pPageAdr
        , c4PageCount * kCIDLib::c4MemPageSize
        , MEM_COMMIT
        , tCIDLib::c4EnumOrd(eAccess)))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TRawMem::bFreeSysMem(const tCIDLib::TVoid* pToFree)
{
    if (!::VirtualFree((tCIDLib::TVoid*)pToFree, 0, MEM_RELEASE))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TRawMem::bAllocSysMem(  const   tCIDLib::TCard4         c4Size
                        , const tCIDLib::EMemAccFlags   eAccess
                        , const TRawMem::EMemAllFlags   eAlloc
                        ,       tCIDLib::TVoid*&        pToFill)
{
    // Try to allocate the buffer
    tCIDLib::TVoid* pBuf = ::VirtualAlloc
    (
        0, c4Size, tCIDLib::c4EnumOrd(eAlloc), tCIDLib::c4EnumOrd(eAccess)
    );

    if (!pBuf)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    pToFill = pBuf;
    return kCIDLib::True;
}


//
//  We do a safe decrement of the passed reference. It cannot be zero or greater than
//  i4MaxInt. If so we return an error. So, if it underflows, we know something is
//  wrong. A ref count cannot go below zero unless the counting is bad.
//
tCIDLib::TBoolean TRawMem::bSafeRefRelease(tCIDLib::TCard4& c4Ref)
{
    if (c4Ref > tCIDLib::TCard4(kCIDLib::i4MaxInt))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_BadRefCntRel);
        return kCIDLib::False;
    }

    if (!c4Ref)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_ZeroRefCntRel);
        return kCIDLib::False;
    }

    LONG lVal = (LONG)c4Ref;
    lVal = ::InterlockedDecrementRelease(&lVal);

    // The ref count management must be wrong
    if (lVal < 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_RefCntUnderflow);
        return kCIDLib::False;
    }

    c4Ref = tCIDLib::TCard4(lVal);
    return kCIDLib::True;
}



tCIDLib::TCard4
TRawMem::c4CompareAndExchange(          tCIDLib::TCard4&    c4ToFill
                                , const tCIDLib::TCard4     c4New
                                , const tCIDLib::TCard4     c4Compare)
{
    return static_cast<tCIDLib::TCard4>
    (
        ::InterlockedCompareExchange
        (
            reinterpret_cast<long*>(&c4ToFill)
            , c4New
            , c4Compare
        )
    );
}


tCIDLib::TCard4
TRawMem::c4Exchange(        tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::TCard4     c4New)
{
    return static_cast<tCIDLib::TCard4>
    (
        ::InterlockedExchange
        (
            reinterpret_cast<long*>(&c4ToFill)
            , c4New
        )
    );
}


tCIDLib::TVoid*
TRawMem::pCompareAndExchangePtr(const   tCIDLib::TVoid* pToFill
                                , const tCIDLib::TVoid* pNew
                                , const tCIDLib::TVoid* pCompare)
{
    // We have to cast because this API doesn't understand const'ness
    return ::InterlockedCompareExchangePointer
    (
        const_cast<tCIDLib::TVoid**>(&pToFill)
        , const_cast<tCIDLib::TVoid*>(pNew)
        , const_cast<tCIDLib::TVoid*>(pCompare)
    );
}


tCIDLib::TVoid* TRawMem::pPageBaseAdr(const tCIDLib::TVoid* pBufContained)
{
    MEMORY_BASIC_INFORMATION RawInfo;
    ::VirtualQuery(pBufContained, &RawInfo, sizeof(MEMORY_BASIC_INFORMATION));
    return RawInfo.BaseAddress;
}


tCIDLib::TVoid*
TRawMem::pQueryMemFlags(const   tCIDLib::TVoid*         pBufToQuery
                        ,       TRawMem::TSysMemInfo&   MemInfo)
{
    MEMORY_BASIC_INFORMATION RawInfo;
    ::VirtualQuery(pBufToQuery, &RawInfo, sizeof(MEMORY_BASIC_INFORMATION));

    MemInfo.c4RegionSize  = RawInfo.RegionSize;
    MemInfo.eAllocAccess  = tCIDLib::EMemAccFlags(RawInfo.AllocationProtect);
    MemInfo.eRegionAccess = tCIDLib::EMemAccFlags(RawInfo.Protect);
    MemInfo.eRegionState  = tCIDLib::EMemStates(RawInfo.State);
    MemInfo.eRegionType   = tCIDLib::EMemTypes(RawInfo.Type);
    MemInfo.pAllocBase    = RawInfo.AllocationBase;

    return RawInfo.BaseAddress;
}


//
//  We do a safe increment of the passed value. Windows provides us with a function
//  to do this.
//
tCIDLib::TVoid TRawMem::SafeRefAcquire(tCIDLib::TCard4& c4Ref)
{
    ::InterlockedIncrementAcquire(&c4Ref);
}
