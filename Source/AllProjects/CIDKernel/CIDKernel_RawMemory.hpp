//
// FILE NAME: CIDKernel_RawMemory.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/13/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_RawMemory.Cpp file, which provides
//  some simple APIs for manipulating raw memory buffers. All of this stuff
//  is placed in the TRawMem namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TRawMem
{
    #pragma CIDLIB_PACK(CIDLIBPACK)

    // ------------------------------------------------------------------------
    //  A small structure that is returned by pQueryMemFlags()
    // ------------------------------------------------------------------------
    struct  TSysMemInfo
    {
        tCIDLib::TCard4         c4RegionSize;
        tCIDLib::EMemAccFlags   eAllocAccess;
        tCIDLib::EMemAccFlags   eRegionAccess;
        tCIDLib::EMemStates     eRegionState;
        tCIDLib::EMemTypes      eRegionType;
        tCIDLib::TVoid*         pAllocBase;
    };

    #pragma CIDLIB_POPPACK


    // ------------------------------------------------------------------------
    //  Some primarily internally used flags for dealing with allocation
    //  and commitment of system memory buffers.
    // ------------------------------------------------------------------------
    enum class EMemAllFlags : tCIDLib::TCard4
    {
        Commit                  = 0x00001000UL
        , Reserve               = 0x00002000UL
    };


    KRNLEXPORT tCIDLib::TBoolean bAllocSysMem
    (
        const   tCIDLib::TCard4         c4Size
        , const tCIDLib::EMemAccFlags   eAccess
        , const TRawMem::EMemAllFlags   eAlloc
        ,       tCIDLib::TVoid*&        pToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bCommitPages
    (
                tCIDLib::TVoid*         pPageAdr
        , const tCIDLib::TCard4         c4PageCount
        , const tCIDLib::EMemAccFlags   eAccess = tCIDLib::EMemAccFlags::ReadWrite
    );

    KRNLEXPORT tCIDLib::TBoolean bFreeSysMem
    (
        const   tCIDLib::TVoid* const   pBuf
    );

    KRNLEXPORT tCIDLib::TBoolean bSafeRefRelease
    (
                tCIDLib::TCard4&        c4Ref
    );

    KRNLEXPORT tCIDLib::TCard4 c4CompareAndExchange
    (
                tCIDLib::TCard4&        c4ToFill
        , const tCIDLib::TCard4         c4New
        , const tCIDLib::TCard4         c4Compare
    );

    KRNLEXPORT tCIDLib::TCard4 c4Exchange
    (
                tCIDLib::TCard4&        c4ToFill
        , const tCIDLib::TCard4         c4New
    );

    KRNLEXPORT tCIDLib::TCard4 c4PageRounded
    (
        const   tCIDLib::TCard4         c4ToRound
    );

    KRNLEXPORT tCIDLib::TCard4 c4PagesCovered
    (
        const   tCIDLib::TCard4         c4Size
    );

    KRNLEXPORT tCIDLib::ESortComps eCompareMemBuf
    (
        const   tCIDLib::TVoid* const   p1
        , const tCIDLib::TVoid* const   p2
        , const tCIDLib::TCard4         c4Count
    );

    inline tCIDLib::TBoolean
    bCompareMemBuf( const   tCIDLib::TVoid* const   p1
                    , const tCIDLib::TVoid* const   p2
                    , const tCIDLib::TCard4         c4Count)
    {
        return eCompareMemBuf(p1, p2, c4Count) == tCIDLib::ESortComps::Equal;
    }

    KRNLEXPORT tCIDLib::THashVal hshHashBuffer
    (
        const   tCIDLib::THashVal       hshLast
        , const tCIDLib::TVoid* const   pBuf
        , const tCIDLib::TCard4         c4Modulus
        , const tCIDLib::TCard4         c4Bytes
    );

    KRNLEXPORT tCIDLib::THashVal hshHashBuffer
    (
        const   tCIDLib::TVoid* const   pBuf
        , const tCIDLib::TCard4         c4Modulus
        , const tCIDLib::TCard4         c4Bytes
    );

    KRNLEXPORT tCIDLib::THashVal hshHashBuffer3309
    (
        const   tCIDLib::THashVal       hshLast
        , const tCIDLib::TVoid* const   pBuf
        , const tCIDLib::TCard4         c4Bytes
    );

    KRNLEXPORT tCIDLib::THashVal hshHashBuffer3309
    (
        const   tCIDLib::TVoid* const   pBuf
        , const tCIDLib::TCard4         c4Bytes
    );

    KRNLEXPORT tCIDLib::THashVal hshHashBufferAdler32
    (
        const   tCIDLib::THashVal       hshAdler
        , const tCIDLib::TVoid* const   pBuf
        , const tCIDLib::TCard4         c4Bytes
    );

    KRNLEXPORT tCIDLib::TVoid CopyMemBuf
    (
                tCIDLib::TVoid* const   pDest
        , const tCIDLib::TVoid* const   pSrc
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid MoveMemBuf
    (
                tCIDLib::TVoid* const   pDest
        ,       tCIDLib::TVoid* const   pSrc
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid* pCompareAndExchangePtr
    (
        const   tCIDLib::TVoid*         pToFill
        , const tCIDLib::TVoid*         pNew
        , const tCIDLib::TVoid*         pCompare
    );

    KRNLEXPORT const tCIDLib::TVoid* pNextPageAdr
    (
        const   tCIDLib::TVoid*         pAddr
    );

    KRNLEXPORT tCIDLib::TVoid* pNextPageAdr
    (
                tCIDLib::TVoid*         pAddr
    );

    KRNLEXPORT const tCIDLib::TVoid* pPrevPageAdr
    (
        const   tCIDLib::TVoid*         pAddr
    );

    KRNLEXPORT tCIDLib::TVoid* pPrevPageAdr
    (
                tCIDLib::TVoid*         pAddr
    );

    KRNLEXPORT tCIDLib::TVoid* pPageBaseAdr
    (
        const   tCIDLib::TVoid*         pBufContained
    );

    KRNLEXPORT tCIDLib::TVoid* pQueryMemFlags
    (
        const   tCIDLib::TVoid*         pBufToQuery
        ,       TSysMemInfo&            MemInfo
    );

    KRNLEXPORT tCIDLib::TVoid SafeRefAcquire
    (
                tCIDLib::TCard4&        c4Ref
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TCard1         c1Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TCard2         c2Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TCard4         c4Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TSCh           chFill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TCh            chFill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TInt1          i1Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TInt2          i2Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TInt4          i4Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TFloat4        f4Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TFloat8        f8Fill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TSInt          iFill
        , const tCIDLib::TCard4         c4Count
    );

    KRNLEXPORT tCIDLib::TVoid SetMemBuf
    (
                tCIDLib::TVoid* const   pMemToFill
        , const tCIDLib::TUInt          uFill
        , const tCIDLib::TCard4         c4Count
    );


    //
    //  These are some templates that make the business of doing compare
    //  and exchanges on pointers typesafe and convenient.
    //
    template <class T> T* pExchangePtr(T*& pToFill, const T* const pNew)
    {
        return reinterpret_cast<T*>
        (
            c4Exchange
            (
                reinterpret_cast<tCIDLib::TCard4&>(pToFill)
                , reinterpret_cast<tCIDLib::TCard4>(pNew)
            )
        );
    }

    template <class T> T* pCompareAndExchangePtr(       T*&         pToFill
                                                , const T* const    pNew
                                                , const T* const    pCompare)
    {
        return reinterpret_cast<T*>
        (
            c4CompareAndExchange
            (
                *reinterpret_cast<tCIDLib::TCard4*>(&pToFill)
                , reinterpret_cast<tCIDLib::TCard4>(pNew)
                , reinterpret_cast<tCIDLib::TCard4>(pCompare)
            )
        );
    }


    // ---------------------------------------------------------------------------
    //  Calculates the offset between two addresses. Keeps the caller from having
    //  to do the casting. p2 should be higher in memory.
    // ---------------------------------------------------------------------------
    inline tCIDLib::TCard4 c4AdrDiff(const  tCIDLib::TVoid* const p1
                                    , const tCIDLib::TVoid* const p2)
    {
        return ((tCIDLib::TCard1*)p2) - ((tCIDLib::TCard1*)p1);
    }
}
