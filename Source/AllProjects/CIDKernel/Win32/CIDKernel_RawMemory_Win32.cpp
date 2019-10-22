//
// FILE NAME: CIDKernel_RawMemory_Win32.Cpp
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



tCIDLib::TCard4
TRawMem::c4CompareAndExchange(              tCIDLib::TCard4&    c4ToFill
                                , const     tCIDLib::TCard4     c4New
                                , const     tCIDLib::TCard4     c4Compare)
{
    return static_cast<tCIDLib::TCard4>
    (
        ::InterlockedCompareExchange
        (
            reinterpret_cast<long*>(&c4ToFill), c4New, c4Compare
        )
    );
}


tCIDLib::TCard4
TRawMem::c4Exchange(        tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::TCard4     c4New)
{
    return static_cast<tCIDLib::TCard4>
    (
        ::InterlockedExchange(reinterpret_cast<long*>(&c4ToFill), c4New)
    );
}


//
//  We do a safe inc/dec of the passed reference. Just to be safe we don't allow ref
//  counts beyond i4MaxCard, since the interlocked stuff really works on signed
//  values. It would be psychotic to expect such a thing anyway. And we check for an
//  underflow in the release since that means something went really wrong.
//
tCIDLib::TCard4
TRawMem::c4SafeRefAcquire(tCIDLib::TCard4& c4Ref, tCIDLib::TBoolean& bRes)
{
    if (c4Ref > tCIDLib::TCard4(kCIDLib::i4MaxInt))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_BadRefCntRel);
        bRes = kCIDLib::False;
        return 0;
    }
    bRes = kCIDLib::True;
    return tCIDLib::TCard4(::InterlockedIncrement(&c4Ref));
}

tCIDLib::TCard4
TRawMem::c4SafeRefRelease(tCIDLib::TCard4& c4Ref, tCIDLib::TBoolean& bRes)
{
    if (c4Ref > tCIDLib::TCard4(kCIDLib::i4MaxInt))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_BadRefCntRel);
        bRes = kCIDLib::False;
        return 0;
    }

    if (!c4Ref)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_ZeroRefCntRel);
        bRes = kCIDLib::False;
        return 0;
    }

    LONG iRes = ::InterlockedDecrement(&c4Ref);
    if (iRes < 0)
    {
        // The ref count management must be wrong
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_RefCntUnderflow);
        bRes = kCIDLib::False;
        return 0;
    }
    bRes = kCIDLib::True;
    return tCIDLib::TCard4(iRes);
}


//
//  Returns the original value in ppToFill. If that's the same as pCompare,
//  then the exchange occurred.
//
tCIDLib::TVoid*
TRawMem::pCompareAndExchangeRawPtr(         tCIDLib::TVoid**    ppToFill
                                    , const tCIDLib::TVoid*     pNew
                                    , const tCIDLib::TVoid*     pCompare)
{
    // We have to cast because this API doesn't understand const'ness
    return ::InterlockedCompareExchangePointer
    (
        const_cast<tCIDLib::TVoid**>(ppToFill)
        , const_cast<tCIDLib::TVoid*>(pNew)
        , const_cast<tCIDLib::TVoid*>(pCompare)
    );
}


//
//  Atomically updates the to fill pointer with the new pointer. Returns the
//  original value that was replaced.
//
tCIDLib::TVoid*
TRawMem::pExchangeRawPtr(tCIDLib::TVoid** ppToFill, const tCIDLib::TVoid* pNew)
{
    return ::InterlockedExchangePointer
    (
        const_cast<tCIDLib::TVoid**>(ppToFill), const_cast<tCIDLib::TVoid*>(pNew)
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

