//
// FILE NAME: CIDKernel_RawMemory.Cpp
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
//  This file provides the platform independent implementation for the
//  TRawMem namespace. The platform dependent stuff is implmented in each
//  per-platform subdirectory in the CIDKernel_RawMemory_Win32.Cpp file.
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
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDKernel_RawMemory
{
    // -----------------------------------------------------------------------
    //  We fault in the CRC table for the 3309 CRC algorithm, upon first use
    // -----------------------------------------------------------------------
    tCIDLib::TCard4             ac43309Table[256];
    volatile tCIDLib::TBoolean  b3309Init = kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  TRawMem methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TRawMem::c4PageRounded(const tCIDLib::TCard4 c4ToRound)
{
    if (c4ToRound < kCIDLib::c4MemPageSize)
        return 1;

    //
    //  Get the base value, which will be rounded down if any partial. If any
    //  partial, then we need another page.
    //
    const tCIDLib::TCard4 c4Mod = (c4ToRound % kCIDLib::c4MemPageSize);
    tCIDLib::TCard4 c4Ret = c4ToRound - c4Mod;
    if (c4Mod)
        c4Ret += kCIDLib::c4MemPageSize;
    return c4Ret;
}


tCIDLib::TCard4 TRawMem::c4PagesCovered(const tCIDLib::TCard4 c4Size)
{
    if (!c4Size)
        return 0;
    else if (c4Size < kCIDLib::c4MemPageSize)
        return 1;

    tCIDLib::TCard4 c4Ret = c4Size / kCIDLib::c4MemPageSize;
    if (c4Size % kCIDLib::c4MemPageSize)
        c4Ret++;
    return c4Ret;
}


tCIDLib::TVoid
TRawMem::CopyMemBuf(        tCIDLib::TVoid* const   pDest
                    , const tCIDLib::TVoid* const   pSrc
                    , const tCIDLib::TCard4         c4Count)
{
    memcpy(pDest, pSrc, c4Count);
}


tCIDLib::ESortComps
TRawMem::eCompareMemBuf(const   tCIDLib::TVoid* const   p1
                        , const tCIDLib::TVoid* const   p2
                        , const tCIDLib::TCard4         c4Count)
{
     tCIDLib::TInt4 i4Res = memcmp(p1, p2, c4Count);
    if (i4Res < 0)
        return tCIDLib::ESortComps::FirstLess;
    else if (i4Res > 0)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;

}


//
//  Do a standard CIDLib hahs on a buffer. This is the hash used internally
//  by CIDLib classes that need to hash buffers for hashed collections and
//  so forth. It does a fairly standard CRC based hash that is fast and
//  efficient. We use a standard, constant 16 bit CRC table.
//
//  We have one version that does incremental hashs of disjoint buffers and
//  accumulates a hash by passing in the previous one to start off with. And
//  we have another that just does a calculation on a single buffer by
//  calling the incremental one with a zeroed starting hash.
//
tCIDLib::THashVal
TRawMem::hshHashBuffer( const   tCIDLib::THashVal       hshLast
                        , const tCIDLib::TVoid* const   pBuf
                        , const tCIDLib::TCard4         c4Modulus
                        , const tCIDLib::TCard4         c4BufSz)
{
    // Start the hash out as the last hash value
    tCIDLib::TCard4 c4Ret = hshLast;

    // Get a temp pointer that we can run upwards as we go
    const tCIDLib::TCard1* pc1Buf = reinterpret_cast<const tCIDLib::TCard1*>(pBuf);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BufSz; c4Index++)
    {
        // Create a temporary value, shifted down version of the current hash
        const tCIDLib::TCard4 c4Tmp = c4Ret >> 24;

        //
        //  Now scramble the bits. The value we add in for the current value
        //  is actually using the current byte to index a CRC table. This
        //  helps scramble things more than a single byte addition.
        //
        c4Ret += (c4Ret * 37) + c4Tmp
                    + tCIDLib::TCard4(kCIDLib::ac2CRCTable[*pc1Buf++]);
    }

    // Divide by the modulus for return
    return tCIDLib::THashVal(c4Ret % c4Modulus);
}

tCIDLib::THashVal
TRawMem::hshHashBuffer( const   tCIDLib::TVoid* const   pBuf
                        , const tCIDLib::TCard4         c4Modulus
                        , const tCIDLib::TCard4         c4BufSz)
{
    // Just call the incremental version once to do the whole buffer
    return hshHashBuffer(0, pBuf, c4Modulus, c4BufSz);
}


//
//  Do an ISO-3309 type CRC calculation on buffers. We have one version that
//  is for use on ongoing fragments, accumulating them into a single hash,
//  and one that takes a full buffer and just calls the other to do the whole
//  thing and complete the CRC.
//
tCIDLib::THashVal
TRawMem::hshHashBuffer3309( const   tCIDLib::THashVal       hshLast
                            , const tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4Bytes)
{
    // If we've not initialized the CRC table, do it now
    if (!CIDKernel_RawMemory::b3309Init)
    {
        TBaseLock lockInit;
        if (!CIDKernel_RawMemory::b3309Init)
        {
            tCIDLib::TCard4 c4Cur;
            for (tCIDLib::TCard4 c4EntryInd = 0; c4EntryInd < 256; c4EntryInd++)
            {
                c4Cur = c4EntryInd;
                for (tCIDLib::TCard4 c4Loop = 0; c4Loop < 8; c4Loop++)
                {
                    if (c4Cur & 0x1)
                        c4Cur = 0xEDB88320L ^ (c4Cur >> 1);
                    else
                        c4Cur >>= 1;
                }
                CIDKernel_RawMemory::ac43309Table[c4EntryInd] = c4Cur;
            }

            // And lastly indicate we've faulted it in
            CIDKernel_RawMemory::b3309Init = kCIDLib::True;
        }
    }

    // Get a byte based temp pointer that we can run upwards as we go
    const tCIDLib::TCard1* pc1Buf = reinterpret_cast<const tCIDLib::TCard1*>(pBuf);

    // Get a copy of the last hash as a starting poin
    tCIDLib::TCard4 c4Ret = hshLast;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Bytes; c4Index++)
    {
        c4Ret = CIDKernel_RawMemory::ac43309Table[(c4Ret ^ pc1Buf[c4Index]) & 0xFF]
                ^ (c4Ret >> 8);
    }
    return c4Ret;
}

tCIDLib::THashVal
TRawMem::hshHashBuffer3309( const   tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4Bytes)
{
    return hshHashBuffer3309(kCIDLib::c4MaxCard, pBuf, c4Bytes) ^ kCIDLib::c4MaxCard;
}


//
//  Do an Adler32 hash calculation on a buffer.
//
tCIDLib::THashVal
TRawMem::hshHashBufferAdler32(  const   tCIDLib::THashVal       hshAdler
                                , const tCIDLib::TVoid* const   pBuf
                                , const tCIDLib::TCard4         c4Bytes)
{
    // These are used to generated the inner loop stuff compactly
    #define Do1(buf,i1)  {c4S1 += buf[i1]; c4S2 += c4S1;}
    #define Do2(buf,i2)  Do1(buf,i2); Do1(buf,i2+1);
    #define Do4(buf,i4)  Do2(buf,i4); Do2(buf,i4+2);
    #define Do8(buf,i8)  Do4(buf,i8); Do4(buf,i8+4);
    #define Do16(buf)    Do8(buf,0); Do8(buf,8);

    // Some constants uses in the calculation. Base is the largest 16 bit prime
    const tCIDLib::TCard4 c4Base(65521);
    const tCIDLib::TCard4 c4NMax(5552);

    // If an empty buffer, return 1
    if (!c4Bytes)
        return 1;

    // Get a byte based temp pointer that we can run upwards as we go
    const tCIDLib::TCard1* pc1Buf = reinterpret_cast<const tCIDLib::TCard1*>(pBuf);

    tCIDLib::TCard4 c4S1 = hshAdler & 0xFFFF;
    tCIDLib::TCard4 c4S2 = (hshAdler >> 16) & 0xFFFF;

    tCIDLib::TCard4 c4Counter = c4Bytes;
    while (c4Counter > 0)
    {
        tCIDLib::TSInt sLoop = c4Counter < c4NMax ? tCIDLib::TSInt(c4Counter) : c4NMax;
        c4Counter -= sLoop;
        while (sLoop >= 16)
        {
            Do16(pc1Buf);
            pc1Buf += 16;
            sLoop -= 16;
        }

        if (sLoop != 0)
        {
            do
            {
                c4S1 += *pc1Buf++;
                c4S2 += c4S1;
            }   while (--sLoop);

            c4S1 %= c4Base;
            c4S2 %= c4Base;
        }
        if (sLoop != 0)
        {
            do
            {
                c4S1 += *pc1Buf++;
                c4S2 += c4S1;
            } while (--sLoop);
        }

        c4S1 %= c4Base;
        c4S2 %= c4Base;
    }
    return (c4S2 << 16) | c4S1;
}


tCIDLib::TVoid
TRawMem::MoveMemBuf(        tCIDLib::TVoid* const   pDest
                    ,       tCIDLib::TVoid* const   pSrc
                    , const tCIDLib::TCard4         c4Count)
{
    memmove(pDest, pSrc, c4Count);
}


//
//  Handles our commonly needed chore of rounding a pointer up or down
//  to the next/prev page boundary.
//
const tCIDLib::TVoid* TRawMem::pNextPageAdr(const tCIDLib::TVoid* pAddr)
{
    if (!pAddr)
        return 0;

    const tCIDLib::TCard4 c4Org = tCIDLib::TCard4(pAddr);
    tCIDLib::TCard4 c4Tmp = c4Org / kCIDLib::c4MemPageSize;
    c4Tmp *= kCIDLib::c4MemPageSize;
    if (!c4Tmp)
        c4Tmp = kCIDLib::c4MemPageSize;
    else if (c4Org % kCIDLib::c4MemPageSize)
        c4Tmp += kCIDLib::c4MemPageSize;
    return reinterpret_cast<const tCIDLib::TVoid*>(c4Tmp);
}

tCIDLib::TVoid* TRawMem::pNextPageAdr(tCIDLib::TVoid* pAddr)
{
    if (!pAddr)
        return 0;

    const tCIDLib::TCard4 c4Org = tCIDLib::TCard4(pAddr);
    tCIDLib::TCard4 c4Tmp = c4Org / kCIDLib::c4MemPageSize;
    c4Tmp *= kCIDLib::c4MemPageSize;
    if (!c4Tmp)
        c4Tmp = kCIDLib::c4MemPageSize;
    else if (c4Org % kCIDLib::c4MemPageSize)
        c4Tmp += kCIDLib::c4MemPageSize;
    return reinterpret_cast<tCIDLib::TVoid*>(c4Tmp);
}


const tCIDLib::TVoid* TRawMem::pPrevPageAdr(const tCIDLib::TVoid* pAddr)
{
    // If less than one page, then return 0
    if (pAddr < (tCIDLib::TVoid*)kCIDLib::c4MemPageSize)
        return 0;

    tCIDLib::TCard4 c4Tmp = tCIDLib::TCard4(pAddr) / kCIDLib::c4MemPageSize;
    c4Tmp *= kCIDLib::c4MemPageSize;
    return reinterpret_cast<const tCIDLib::TVoid*>(c4Tmp);
}

tCIDLib::TVoid* TRawMem::pPrevPageAdr(tCIDLib::TVoid* pAddr)
{
    // If less than one page, then return 0
    if (pAddr < (tCIDLib::TVoid*)kCIDLib::c4MemPageSize)
        return 0;

    tCIDLib::TCard4 c4Tmp = tCIDLib::TCard4(pAddr) / kCIDLib::c4MemPageSize;
    c4Tmp *= kCIDLib::c4MemPageSize;
    return reinterpret_cast<tCIDLib::TVoid*>(c4Tmp);
}


tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TCard1         c1Fill
                    , const tCIDLib::TCard4         c4Count)
{
    memset(pMem, c1Fill, c4Count);
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TInt1          i1Fill
                    , const tCIDLib::TCard4         c4Count)
{
    memset(pMem, i1Fill, c4Count);
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TCard2         c2Fill
                    , const tCIDLib::TCard4         c4Count)
{
    //
    //  Optimize if the high and low bytes are the same, since we can just
    //  do a memset. Otherwise, have to do it manually.
    //
    if (((c2Fill & 0xFF00) >> 8) == (c2Fill & 0x00FF))
    {
        memset(pMem, tCIDLib::TCard1(c2Fill & 0x00FF), c4Count * 2);
        return;
    }

    tCIDLib::TCard2* pc2Buf = (tCIDLib::TCard2*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pc2Buf++ = c2Fill;
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TInt2          i2Fill
                    , const tCIDLib::TCard4         c4Count)
{
    //
    //  Optimize if the high and low bytes are the same, since we can just
    //  do a memset. Otherwise, have to do it manually.
    //
    if (((i2Fill & 0xFF00) >> 8) == (i2Fill & 0x00FF))
    {
        memset(pMem, tCIDLib::TCard1(i2Fill & 0x00FF), c4Count * 2);
        return;
    }

    tCIDLib::TInt2* pi2Buf = (tCIDLib::TInt2*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pi2Buf++ = i2Fill;
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TCard4         c4Fill
                    , const tCIDLib::TCard4         c4Count)
{
    // Optimize if the fill is 0. Otherwise, have to do it manually.
    if (!c4Fill)
    {
        memset(pMem, 0, c4Count * 4);
        return;
    }

    tCIDLib::TCard4* pc4Buf = (tCIDLib::TCard4*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pc4Buf++ = c4Fill;
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TInt4          i4Fill
                    , const tCIDLib::TCard4         c4Count)
{
    // Optimize if the fill is 0. Otherwise, have to do it manually.
    if (!i4Fill)
    {
        memset(pMem, 0, c4Count * 4);
        return;
    }

    tCIDLib::TInt4* pi4Buf = (tCIDLib::TInt4*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pi4Buf++ = i4Fill;
}


tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TSCh           chFill
                    , const tCIDLib::TCard4         c4Count)
{
    memset(pMem, chFill, c4Count);
}


tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TCh            chFill
                    , const tCIDLib::TCard4         c4Count)
{
    // Optimize if the fill is 0. Otherwise, have to do it manually.
    if (!chFill)
    {
        memset(pMem, 0, c4Count * sizeof(chFill));
        return;
    }

    tCIDLib::TCh* pchBuf = (tCIDLib::TCh*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pchBuf++ = chFill;
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TFloat4        f4Fill
                    , const tCIDLib::TCard4         c4Count)
{
    // Optimize if the fill is 0. Otherwise, have to do it manually.
    if (!f4Fill)
    {
        memset(pMem, 0, c4Count * sizeof(f4Fill));
        return;
    }

    tCIDLib::TFloat4* pf4Buf = (tCIDLib::TFloat4*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pf4Buf++ = f4Fill;
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TFloat8        f8Fill
                    , const tCIDLib::TCard4         c4Count)
{
    // Optimize if the fill is 0. Otherwise, have to do it manually.
    if (!f8Fill)
    {
        memset(pMem, 0, c4Count * sizeof(f8Fill));
        return;
    }

    tCIDLib::TFloat8* pf8Buf = (tCIDLib::TFloat8*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pf8Buf++ = f8Fill;
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TSInt          iFill
                    , const tCIDLib::TCard4         c4Count)
{
    // Optimize if the fill is 0. Otherwise, have to do it manually.
    if (!iFill)
    {
        memset(pMem, 0, c4Count * 4);
        return;
    }

    tCIDLib::TSInt* piBuf = (tCIDLib::TSInt*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *piBuf++ = iFill;
}

tCIDLib::TVoid
TRawMem::SetMemBuf(         tCIDLib::TVoid* const   pMem
                    , const tCIDLib::TUInt          uFill
                    , const tCIDLib::TCard4         c4Count)
{
    // Optimize if the fill is 0. Otherwise, have to do it manually.
    if (!uFill)
    {
        memset(pMem, 0, c4Count * 4);
        return;
    }

    tCIDLib::TUInt* puBuf = (tCIDLib::TUInt*)pMem;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *puBuf++ = uFill;
}

