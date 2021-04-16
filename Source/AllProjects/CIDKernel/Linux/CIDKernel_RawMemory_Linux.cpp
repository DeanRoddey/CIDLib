//
// FILE NAME: CIDKernel_RawMemory_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 09/13/1998
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
//  This file provides the Linux specific implementation for the raw memory
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

// ---------------------------------------------------------------------------
//  Local functions, structures, data and constants
// ---------------------------------------------------------------------------
namespace CIDKernel_RawMemory_Linux
{
    // ---------------------------------------------------------------------------
    //  Local constants
    // ---------------------------------------------------------------------------
    const tCIDLib::TCard4 c4ChunkSize = 10;

    // ---------------------------------------------------------------------------
    //  Local types
    // ---------------------------------------------------------------------------
    class TLinuxMemInfo
    {
        public:
            TLinuxMemInfo();

            const TLinuxMemInfo& operator= (const TLinuxMemInfo& Info);

            tCIDLib::TBoolean       bInUse;
            tCIDLib::TVoid*         pRegion;
            tCIDLib::TCard4         c4Size;
            tCIDLib::EMemAccFlags   eAccess;
    };

    // ---------------------------------------------------------------------------
    //  Local data
    // ---------------------------------------------------------------------------
    TLinuxMemInfo* aLinuxMemInfo = nullptr;
    tCIDLib::TCard4 c4Capacity = 0;

    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    TLinuxMemInfo::TLinuxMemInfo() :
    
        bInUse(kCIDLib::False)
        , pRegion(0)
        , c4Size(0)
        , eAccess(tCIDLib::EMemAccFlags::None)
    {
    }

    const TLinuxMemInfo& TLinuxMemInfo::operator= (const TLinuxMemInfo& Info)
    {
        if (this != &Info)
        {
            bInUse = Info.bInUse;
            pRegion = Info.pRegion;
            c4Size = Info.c4Size;
            eAccess = Info.eAccess;
        }
        return *this;
    }

    tCIDLib::TVoid AddInfo(const TLinuxMemInfo& Info)
    {
        if (!CIDKernel_RawMemory_Linux::c4Capacity)
        {
            CIDKernel_RawMemory_Linux::aLinuxMemInfo = new TLinuxMemInfo[c4ChunkSize];
            CIDKernel_RawMemory_Linux::c4Capacity = c4ChunkSize;
        }
        tCIDLib::TCard4 c4Idx;
        for (c4Idx = 0; c4Idx < CIDKernel_RawMemory_Linux::c4Capacity; c4Idx++)
        {
            if (!CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx].bInUse)
            {
                CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx] = Info;
                CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx].bInUse = kCIDLib::True;
                return;
            }
        }
        TLinuxMemInfo* aNewMemInfo = new TLinuxMemInfo[CIDKernel_RawMemory_Linux::c4Capacity + c4ChunkSize];
        for (c4Idx = 0; c4Idx < CIDKernel_RawMemory_Linux::c4Capacity; c4Idx++)
            aNewMemInfo[c4Idx] = CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx];
        delete [] CIDKernel_RawMemory_Linux::aLinuxMemInfo;
        CIDKernel_RawMemory_Linux::aLinuxMemInfo = aNewMemInfo;
        CIDKernel_RawMemory_Linux::aLinuxMemInfo[CIDKernel_RawMemory_Linux::c4Capacity] = Info;
        CIDKernel_RawMemory_Linux::aLinuxMemInfo[CIDKernel_RawMemory_Linux::c4Capacity].bInUse = kCIDLib::True;
        CIDKernel_RawMemory_Linux::c4Capacity += c4ChunkSize;
    }

    TLinuxMemInfo* FindInfo(const tCIDLib::TVoid* pAddrInRange)
    {
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < CIDKernel_RawMemory_Linux::c4Capacity; c4Idx++)
        {
            if (CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx].bInUse
            &&  (CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx].pRegion <= pAddrInRange)
            &&  (tCIDLib::TCard4(CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx].pRegion)
                + CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx].c4Size > tCIDLib::TCard4(pAddrInRange)))
            {
                return &CIDKernel_RawMemory_Linux::aLinuxMemInfo[c4Idx];
            }
        }
        return 0;
    }

    tCIDLib::TCard4 TranslateMemAccFlags(const tCIDLib::EMemAccFlags eAccess)
    {
        tCIDLib::TCard4 c4Return = 0;
        switch (eAccess)
        {
            case tCIDLib::EMemAccFlags::None:
            case tCIDLib::EMemAccFlags::Guard:        // No such thing as guard pages in Linux
                c4Return |= PROT_NONE;
                break;
            case tCIDLib::EMemAccFlags::ReadOnly:
                c4Return |= PROT_READ;
                break;
            case tCIDLib::EMemAccFlags::ReadWrite:
                c4Return |= PROT_READ | PROT_WRITE;
                break;
            case tCIDLib::EMemAccFlags::Execute:
                c4Return |= PROT_EXEC;
                break;
            case tCIDLib::EMemAccFlags::ExecuteRead:
                c4Return |= PROT_EXEC | PROT_READ;
                break;
            case tCIDLib::EMemAccFlags::ExecuteReadWrite:
                c4Return |= PROT_EXEC | PROT_READ | PROT_WRITE;
                break;
        }
        return c4Return;
    }
}


// ---------------------------------------------------------------------------
//  TRawMem functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TRawMem::bCommitPages(          tCIDLib::TVoid*
                        , const tCIDLib::TCard4
                        , const tCIDLib::EMemAccFlags)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean TRawMem::bFreeSysMem(const tCIDLib::TVoid* pToFree)
{
    CIDKernel_RawMemory_Linux::TLinuxMemInfo* pMemInfo = CIDKernel_RawMemory_Linux::FindInfo(pToFree);
    if (!pMemInfo)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
        return kCIDLib::False;
    }

    if (::munmap(static_cast<tCIDLib::TSCh*>(pMemInfo->pRegion), pMemInfo->c4Size))
    {
        TKrnlError::SetLastError(errno);
        return kCIDLib::False;
    }

    pMemInfo->bInUse = kCIDLib::False;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TRawMem::bAllocSysMem(  const   tCIDLib::TCard4         c4Size
                        , const tCIDLib::EMemAccFlags   eAccess
                        , const TRawMem::EMemAllFlags
                        ,       tCIDLib::TVoid*&        pToFill)
{
    tCIDLib::TVoid* pBuf = ::mmap
    (
        0
        , c4Size
        , CIDKernel_RawMemory_Linux::TranslateMemAccFlags(eAccess)
        , MAP_PRIVATE | MAP_ANON
        , -1
        , 0
    );

    if (!pBuf)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    pToFill = pBuf;

    CIDKernel_RawMemory_Linux::TLinuxMemInfo MemInfo;
    MemInfo.c4Size = c4Size;
    MemInfo.eAccess = eAccess;
    MemInfo.pRegion = pBuf;
    CIDKernel_RawMemory_Linux::AddInfo(MemInfo);

    return kCIDLib::True;
}


tCIDLib::TCard4
TRawMem::c4CompareAndExchange(          tCIDLib::TCard4&    c4ToFill
                                , const tCIDLib::TCard4     c4New
                                , const tCIDLib::TCard4     c4Compare) noexcept
{
    tCIDLib::TCard4 c4Tmp = c4Compare;
    const tCIDLib::TBoolean bRes = __atomic_compare_exchange
    (
        &c4ToFill
        , &c4Tmp
        , &c4New
        , kCIDLib::False
        , __ATOMIC_SEQ_CST
        , __ATOMIC_SEQ_CST
    );

    // If true, then it was equal to compare and so that we the original value
    if (bRes)
        return c4Compare;

    // The original value is in c4Tmp
    return c4Tmp;
}


tCIDLib::TCard4
TRawMem::c4Exchange(        tCIDLib::TCard4&        c4ToFill
                    , const tCIDLib::TCard4         c4New) noexcept
{
    tCIDLib::TCard4 c4Ret;
    __atomic_exchange(&c4ToFill, &c4New, &c4Ret, __ATOMIC_SEQ_CST);
    return c4Ret;
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
    return tCIDLib::TCard4(__sync_add_and_fetch(&c4Ref, 1));
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

    tCIDLib::TInt4 iRes = __sync_sub_and_fetch(&c4Ref, 1);
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
                                    , const tCIDLib::TVoid*     pCompare) noexcept
{
    tCIDLib::TVoid* pTmp = const_cast<tCIDLib::TVoid*>(pCompare);
    const tCIDLib::TBoolean bRes = __atomic_compare_exchange
    (
        ppToFill
        , &pTmp
        , &pNew
        , kCIDLib::False
        , __ATOMIC_SEQ_CST
        , __ATOMIC_SEQ_CST
    );

    // If true, then it was equal to compare and so that we the original value
    if (bRes)
        return const_cast<tCIDLib::TVoid*>(pCompare);

    // The original value is in pTmp
    return pTmp;
}


//
//  Atomically updates the to fill pointer with the new pointer. Returns the
//  original value that was replaced.
//
tCIDLib::TVoid*
TRawMem::pExchangeRawPtr(tCIDLib::TVoid** ppToFill, const tCIDLib::TVoid* pNew) noexcept
{
    tCIDLib::TVoid* pRet = nullptr;
    __atomic_exchange(ppToFill, const_cast<tCIDLib::TVoid**>(&pNew), &pRet, __ATOMIC_SEQ_CST);
    return pRet;
}


tCIDLib::TVoid* TRawMem::pPageBaseAdr(const tCIDLib::TVoid* pBufContained)
{
    CIDKernel_RawMemory_Linux::TLinuxMemInfo* pMemInfo = CIDKernel_RawMemory_Linux::FindInfo(pBufContained);
    return pMemInfo ? pMemInfo->pRegion : nullptr;
}


tCIDLib::TVoid*
TRawMem::pQueryMemFlags(const   tCIDLib::TVoid*         pBufToQuery
                        ,       TRawMem::TSysMemInfo&   MemInfo)
{
    CIDKernel_RawMemory_Linux::TLinuxMemInfo* pLinuxMemInfo
    (
        CIDKernel_RawMemory_Linux::FindInfo(pBufToQuery)
    );
    if (!pLinuxMemInfo)
        return nullptr;

    MemInfo.c4RegionSize = pLinuxMemInfo->c4Size;
    MemInfo.eAllocAccess = pLinuxMemInfo->eAccess;
    MemInfo.eRegionAccess = pLinuxMemInfo->eAccess;
    MemInfo.eRegionState = tCIDLib::EMemStates::Commit;
    MemInfo.eRegionType = tCIDLib::EMemTypes::Private;
    MemInfo.pAllocBase = pLinuxMemInfo->pRegion;

    return MemInfo.pAllocBase;
}
