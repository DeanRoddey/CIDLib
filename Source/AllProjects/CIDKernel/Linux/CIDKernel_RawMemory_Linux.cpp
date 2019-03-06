//
// FILE NAME: CIDKernel_RawMemory_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 09/13/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
namespace
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
    pthread_mutex_t __mtxCmpXchg = PTHREAD_MUTEX_INITIALIZER;
    TLinuxMemInfo* __aLinuxMemInfo = 0;
    tCIDLib::TCard4 __c4Capacity = 0;

    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    TLinuxMemInfo::TLinuxMemInfo()
        : bInUse(kCIDLib::False)
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

    tCIDLib::TVoid __AddInfo(const TLinuxMemInfo& Info)
    {
        if (!__c4Capacity)
        {
            __aLinuxMemInfo = new TLinuxMemInfo[c4ChunkSize];
            __c4Capacity = c4ChunkSize;
        }
        tCIDLib::TCard4 c4Idx;
        for (c4Idx = 0; c4Idx < __c4Capacity; c4Idx++)
        {
            if (!__aLinuxMemInfo[c4Idx].bInUse)
            {
                __aLinuxMemInfo[c4Idx] = Info;
                __aLinuxMemInfo[c4Idx].bInUse = kCIDLib::True;
                return;
            }
        }
        TLinuxMemInfo* aNewMemInfo = new TLinuxMemInfo[__c4Capacity + c4ChunkSize];
        for (c4Idx = 0; c4Idx < __c4Capacity; c4Idx++)
            aNewMemInfo[c4Idx] = __aLinuxMemInfo[c4Idx];
        delete [] __aLinuxMemInfo;
        __aLinuxMemInfo = aNewMemInfo;
        __aLinuxMemInfo[__c4Capacity] = Info;
        __aLinuxMemInfo[__c4Capacity].bInUse = kCIDLib::True;
        __c4Capacity += c4ChunkSize;
    }

    TLinuxMemInfo* __FindInfo(const tCIDLib::TVoid* pAddrInRange)
    {
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < __c4Capacity; c4Idx++)
        {
            if (__aLinuxMemInfo[c4Idx].bInUse
            &&  (__aLinuxMemInfo[c4Idx].pRegion <= pAddrInRange)
            &&  (tCIDLib::TCard4(__aLinuxMemInfo[c4Idx].pRegion)
                + __aLinuxMemInfo[c4Idx].c4Size > tCIDLib::TCard4(pAddrInRange)))
            {
                return &__aLinuxMemInfo[c4Idx];
            }
        }
        return 0;
    }

    tCIDLib::TCard4 __TranslateMemAccFlags(const tCIDLib::EMemAccFlags eAccess)
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
    TLinuxMemInfo* pMemInfo = __FindInfo(pToFree);
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
    tCIDLib::TVoid* pBuf = ::mmap(0,
                                  c4Size,
                                  __TranslateMemAccFlags(eAccess),
                                  MAP_PRIVATE | MAP_ANON,
                                  -1,
                                  0);

    if (!pBuf)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    pToFill = pBuf;

    TLinuxMemInfo MemInfo;
    MemInfo.c4Size = c4Size;
    MemInfo.eAccess = eAccess;
    MemInfo.pRegion = pBuf;
    __AddInfo(MemInfo);

    return kCIDLib::True;
}


tCIDLib::TVoid* TRawMem::pPageBaseAdr(const tCIDLib::TVoid* pBufContained)
{
    TLinuxMemInfo* pMemInfo = __FindInfo(pBufContained);
    return pMemInfo ? pMemInfo->pRegion : 0;
}


tCIDLib::TCard4
TRawMem::c4CompareAndExchange(           tCIDLib::TCard4&        c4ToFill
                                , const tCIDLib::TCard4         c4New
                                , const tCIDLib::TCard4         c4Compare)
{
    ::pthread_mutex_lock(&__mtxCmpXchg);

    tCIDLib::TCard4 c4Return = c4ToFill;

    if (c4ToFill == c4Compare)
    {
        c4ToFill = c4New;
    }

    ::pthread_mutex_unlock(&__mtxCmpXchg);

    return c4Return;
}

tCIDLib::TCard4
TRawMem::c4Exchange(        tCIDLib::TCard4&        c4ToFill
                    , const tCIDLib::TCard4         c4New)
{
    ::pthread_mutex_lock(&__mtxCmpXchg);

    tCIDLib::TCard4 c4Return = c4ToFill;
    c4ToFill = c4New;

    ::pthread_mutex_unlock(&__mtxCmpXchg);

    return c4Return;
}


tCIDLib::TVoid*
TRawMem::pQueryMemFlags(const   tCIDLib::TVoid*         pBufToQuery
                        ,       TRawMem::TSysMemInfo&   MemInfo)
{
    TLinuxMemInfo* pLinuxMemInfo = __FindInfo(pBufToQuery);
    if (!pLinuxMemInfo)
        return 0;

    MemInfo.c4RegionSize = pLinuxMemInfo->c4Size;
    MemInfo.eAllocAccess = pLinuxMemInfo->eAccess;
    MemInfo.eRegionAccess = pLinuxMemInfo->eAccess;
    MemInfo.eRegionState = tCIDLib::EMemStates::Commit;
    MemInfo.eRegionType = tCIDLib::EMemTypes::Private;
    MemInfo.pAllocBase = pLinuxMemInfo->pRegion;

    return MemInfo.pAllocBase;
}
