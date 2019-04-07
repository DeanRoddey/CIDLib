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


// ---------------------------------------------------------------------------
//  Public functions
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TRawBits::c4RotateLeft( const   tCIDLib::TCard4 c4Value
                                        , const tCIDLib::TCard4 c4RotateCount)
{
    tCIDLib::TCard1 c1Rot = tCIDLib::TCard1(c4RotateCount % 32);
    tCIDLib::TCard4 c4Ret = c4Value;

    _asm
    {
        PUSH    ecx
        mov     cl, c1Rot
        ROL     c4Ret, cl
        POP     ecx
    }
    return c4Ret;
}

tCIDLib::TCard4 TRawBits::c4RotateRight(const   tCIDLib::TCard4 c4Value
                                        , const tCIDLib::TCard4 c4RotateCount)
{
    tCIDLib::TCard1 c1Rot = tCIDLib::TCard1(c4RotateCount % 32);
    tCIDLib::TCard4 c4Ret = c4Value;

    _asm
    {
        PUSH    ecx
        mov     cl, c1Rot
        ROR     c4Ret, cl
        POP     ecx
    }
    return c4Ret;
}




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
    if (bIsLittle)
    {
        // <TBD>
    }
    return f4ToSwap;
}

tCIDLib::TFloat8
TRawBits::f8SwapBytes(  const   tCIDLib::TFloat8    f8ToSwap
                        , const tCIDLib::TBoolean   bIsLittle)
{
    if (bIsLittle)
    {
        // <TBD>
    }
    return f8ToSwap;
}



// These we can optimize a lot
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
TRawBits::SwapInt4Array(        tCIDLib::TInt4* const   pi4Data
                        , const tCIDLib::TCard4         c4Count)
{
    // Just call the other one and cast the type
    SwapCard4Array(reinterpret_cast<tCIDLib::TCard4*>(pi4Data), c4Count);
}
