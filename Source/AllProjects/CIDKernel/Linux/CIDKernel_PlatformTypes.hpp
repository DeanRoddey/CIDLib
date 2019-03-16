//
// FILE NAME: CIDKernel_PlatformTypes.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/25/1999
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
//  This file is the main types file of the facility. Since this is the
//  core facility, it has all of the fundamental stuff. We use the name
//  space system to make this stuff fit into the CIDLib.Dll facility's
//  name spaces, since this facility is really hidden from end users.
//
// CAVEATS/GOTCHAS:
//
//  1)  This file might be used by some simple, external programs that
//      want to interface to some CIDLib data. So it supports the
//      CIDKRNL_NOCLASSES conditional. If its defined, then only non-class
//      stuff is included here.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Define some development system specific code/data attributes
// ---------------------------------------------------------------------------
#define ASMCALL
#define DLLINITAPI
#define THREADCALL
#define CALLBACKFUNC
#define DLLEXPORT
#define DLLIMPORT
#define THREADVAR
#define WNDPROCAPI



// ---------------------------------------------------------------------------
//  This is the part of the tCIDLib namespace that defines the types for the
//  particular platform. This is where all of the really fundamental type
//  stuff is defined.
// ---------------------------------------------------------------------------
namespace tCIDLib
{
    // -----------------------------------------------------------------------
    //  These are our versions of the fundamental data types. We provide very
    //  explicit sizes for all fundamental types using the number of bytes it
    //  takes.
    // -----------------------------------------------------------------------
    using TBoolean  = bool;             // b
    using TSCh      = char;             // ch
    using TCard1    = unsigned char;    // c1
    using TCard2    = unsigned short;   // c2
    using TCard4    = unsigned long;    // c4
    using TFloat4   = float;            // f4
    using TFloat8   = double;           // f8
    using TInt1     = signed char;      // i1
    using TInt2     = short int;        // i2
    using TInt4     = long int;         // i4
    using TCh       = wchar_t;          // ch


    // -----------------------------------------------------------------------
    //  We occasionaly need these when interfacing to 3rd party code or OS.
    //  They are deprecated in most cases because they are sized according
    //  to platform.
    //
    //  Pefixes are i and u respectively.
    // -----------------------------------------------------------------------
    using TSInt     = int;
    using TUInt     = unsigned;


    // -----------------------------------------------------------------------
    //  Some specially named versions of some fundamental types and their
    //  prefixes.
    // -----------------------------------------------------------------------
    using TEncodedTime  = TFloat8;      // enct
    using TErrCode      = TCard4;       // errc
    using TOSErrCode    = TSInt;        // errc
    using TIPAddr       = TCard4;       // ipa
    using TIPPortNum    = TCard4;       // ippn
    using TProcessId    = TSInt;        // pid
    using TResId        = TCard4;       // rid
    using TMsgId        = TCard4;       // mid
    using TThreadId     = TCard4;       // tid


    // -----------------------------------------------------------------------
    //  These are some structures that must map to the layout of their host
    //  OS equivilents. These must be interopeable with some higher level
    //  structures such as TPoint, TArea and TSize. Having these allows those
    //  higher level modules to avoid importing system headers.
    // -----------------------------------------------------------------------
    #pragma CIDLIB_PACK(1)
    struct  THostRectl
    {
        tCIDLib::TInt4  i4Left;
        tCIDLib::TInt4  i4Top;
        tCIDLib::TInt4  i4Right;
        tCIDLib::TInt4  i4Bottom;
    };

    struct THostPoint
    {
        tCIDLib::TInt4  i4X;
        tCIDLib::TInt4  i4Y;
    };

    struct THostSize
    {
        tCIDLib::TInt4  i4CX;
        tCIDLib::TInt4  i4CY;
    };

    struct TRawRGB
    {
        tCIDLib::TCard1 c1Red;
        tCIDLib::TCard1 c1Green;
        tCIDLib::TCard1 c1Blue;
    };

    struct TCodedRGB
    {
        tCIDLib::TCard1 c1Red;
        tCIDLib::TCard1 c1Green;
        tCIDLib::TCard1 c1Blue;
        tCIDLib::TCard1 c1Dummy;
    };
    #pragma CIDLIB_POPPACK
}
