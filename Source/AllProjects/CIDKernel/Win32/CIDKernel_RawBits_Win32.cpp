//
// FILE NAME: CIDKernel_RawBits_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/22/1997
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
//  This file provides a set of methods for manipulating raw bit fields
//  inside various fundamental types. Most of them are inline and are
//  platform independent. This file provides the Win32 specific implementation
//  of a couple of them that require system services.
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
#include    <intrin.h>


//
//  As long as we are on the Intel world or CPUs that use the Intel IEEE format,
//  we just return the values as is. That's our canonical format.
//
//  We will have to address this if that situation changes.
//
//  In theory this would only get called if the system endianness if different from
//  that of the passed value. But we cannot assume that. So we make the caller
//  pass the endianness of the passed value. We don't use it yet, but this is just
//  planning ahead.
//
tCIDLib::TFloat4
TRawBits::f4SwapBytes(  const   tCIDLib::TFloat4    f4ToSwap
                        , const tCIDLib::TBoolean   bIsLittle)
{
    if (!bIsLittle)
    {
        // <TBD>
    }
    return f4ToSwap;
}

tCIDLib::TFloat8
TRawBits::f8SwapBytes(  const   tCIDLib::TFloat8    f8ToSwap
                        , const tCIDLib::TBoolean   bIsLittle)
{
    if (!bIsLittle)
    {
        // <TBD>
    }
    return f8ToSwap;
}



//
//  Do bulk byte swapping. On some platforms this might be able to get
//  various types of boosts. For the 32 bit one, where we have a built in
//  CPU command for the swap, we just do inline. For the others we use
//  intrinsics.
//
tCIDLib::TVoid
TRawBits::SwapCard2Array(       tCIDLib::TCard2* const  pc2Data
                        , const tCIDLib::TCard4         c4Count)
{
    tCIDLib::TCard2* pc2Cur = pc2Data;
    const tCIDLib::TCard2* pc2End = pc2Data + c4Count;
    while (pc2Cur < pc2End)
    {
        *pc2Cur = _byteswap_ushort(*pc2Cur);
        pc2Cur++;
    }
}

tCIDLib::TVoid
TRawBits::SwapCard4Array(       tCIDLib::TCard4* const  pc4Data
                        , const tCIDLib::TCard4         c4Count)
{
    _asm
    {
        PUSH    eax
        PUSH    ecx
        PUSH    edi
        PUSH    esi

        MOV     edi, pc4Data
        MOV     esi, pc4Data
        MOV     ecx, c4Count

        CountLoop:

        LODS    eax
        BSWAP   eax
        STOS    eax

        LOOP    CountLoop

        POP     esi
        POP     edi
        POP     ecx
        POP     eax
    }
}

tCIDLib::TVoid
TRawBits::SwapCard8Array(       tCIDLib::TCard8* const  pc8Data
                        , const tCIDLib::TCard4         c4Count)
{
    tCIDLib::TCard8* pc8Cur = pc8Data;
    const tCIDLib::TCard8* pc8End = pc8Data + c4Count;
    while (pc8Cur < pc8End)
    {
        *pc8Cur = _byteswap_uint64(*pc8Cur);
        pc8Cur++;
    }
}


// These just call the unsigned versions above and cast the type
tCIDLib::TVoid
TRawBits::SwapInt2Array(        tCIDLib::TInt2* const   pi2Data
                        , const tCIDLib::TCard4         c4Count)
{
    SwapCard2Array(reinterpret_cast<tCIDLib::TCard2*>(pi2Data), c4Count);
}

tCIDLib::TVoid
TRawBits::SwapInt4Array(        tCIDLib::TInt4* const   pi4Data
                        , const tCIDLib::TCard4         c4Count)
{
    SwapCard4Array(reinterpret_cast<tCIDLib::TCard4*>(pi4Data), c4Count);
}

tCIDLib::TVoid
TRawBits::SwapInt8Array(        tCIDLib::TInt8* const   pi8Data
                        , const tCIDLib::TCard4         c4Count)
{
    SwapCard8Array(reinterpret_cast<tCIDLib::TCard8*>(pi8Data), c4Count);
}
