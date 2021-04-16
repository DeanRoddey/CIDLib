//
// FILE NAME: CIDKernel_PlatformTypes.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
#pragma once

// ---------------------------------------------------------------------------
//  Define some development system specific code/data attributes
// ---------------------------------------------------------------------------
#define ASMCALL         __stdcall
#define CALLBACKFUNC    __stdcall
#define DLLEXPORT       __declspec(dllexport)
#define DLLIMPORT       __declspec(dllimport)
#define THREADVAR       __declspec(thread)
#define WNDPROCAPI      __stdcall



// ---------------------------------------------------------------------------
//  This is the part of the tCIDLib namespace that defines the types for the
//  particular platform. This is where all of the really fundamental type
//  stuff is defined.
// ---------------------------------------------------------------------------
namespace tCIDLib
{
    // -----------------------------------------------------------------------
    //  An alias for a returned function pointer (in the generic sense,
    //  prior to casting to its real type.
    // -----------------------------------------------------------------------
    using FuncPtr = int (__stdcall *)();


    // -----------------------------------------------------------------------
    //  These are our versions of the fundamental data types. We provide very
    //  explicit sizes for all fundamental types using the number of bytes it
    //  takes. The exception is the TCh native wide character type.
    // -----------------------------------------------------------------------
    using TBoolean  = bool;                 // b
    using TSCh      = char;                 // ch
    using TCard1    = unsigned char;        // c1
    using TCard2    = unsigned short;       // c2
    using TCard4    = unsigned long;        // c4
    using TCard8    = unsigned long long;   // c8
    using TFloat4   = float;                // f4
    using TFloat8   = double;               // f8
    using TInt1     = signed char;          // i1
    using TInt2     = short int;            // i2
    using TInt4     = long int;             // i4
    using TInt8     = long long;            // i8
    using TCh       = wchar_t;              // ch


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
    using TEncodedTime  = TCard8;           // enct
    using TErrCode      = TCard4;           // errc
    using TOSErrCode    = TCard4;           // errc
    using TIPPortNum    = TCard4;           // ippn
    using TProcessId    = TCard4;           // pid
    using TResId        = TCard4;           // rid
    using TMsgId        = TCard4;           // mid
    using TThreadId     = TCard4;           // tid


    // -----------------------------------------------------------------------
    //  A type we can internally use to bit manipulate enum values. This isn't what
    //  they are stored as (they are stored as Int4 value) this is what we can cast
    //  them to to do masking operations.
    // -----------------------------------------------------------------------
    using TEnumMaskType = TCard4;


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
        tCIDLib::TCard1 c1Alpha;
    };
    #pragma CIDLIB_POPPACK
}


// ----------------------------------------------------------------------------
//  These are some types that are used by the graphics oriented facility
// ----------------------------------------------------------------------------
struct HACCEL__;
struct HBITMAP__;
struct HBRUSH__;
struct HDC__;
struct HFONT__;
struct HICON__;
struct HINSTANCE__;
struct HMENU__;
struct HPEN__;
struct HRGN__;
struct HWND__;
struct tagLOGFONTW;

namespace tCIDGraphDev
{
    using TBmpHandle    = HBITMAP__*;       // hbmp
    using TBrushHandle  = HBRUSH__*;        // hbr
    using TDeviceHandle = HDC__*;           // hdev
    using TFontHandle   = HFONT__*;         // hfont
    using TIconHandle   = HICON__*;         // hico
    using THandle       = void*;            // h
    using TModHandle    = HINSTANCE__*;     // hmod
    using TPenHandle    = HPEN__*;          // hpen
    using TRegionHandle = HRGN__*;          // hrgn
}


// ----------------------------------------------------------------------------
//  These are some types that are used by control window oriented facilities
// ----------------------------------------------------------------------------
namespace tCIDCtrls
{
    // -----------------------------------------------------------------------
    //  Some types we want to be able to control
    // -----------------------------------------------------------------------
    using TAccelHandle      = HACCEL__*;        // haccel
    using TImgListHandle    = void*;            // himgl
    using TLParam           = long;
    using TMenuHandle       = HMENU__*;         // hmenu
    using TWndCoord         = tCIDLib::TInt4;   // wcr
    using TWndSize          = tCIDLib::TCard4;  // wsz
    using TWndMsg           = unsigned int;     // wmsg
    using TMsgResult        = long;             // mres
    using TTimerId          = tCIDLib::TCard4;  // tmid
    using TWndId            = tCIDLib::TCard4;  // wid
    using TWndHandle        = HWND__*;          // hwnd
    using TWParam           = unsigned int;

    using TDispatchFunc = tCIDCtrls::TMsgResult (WNDPROCAPI *)
    (
        const   tCIDCtrls::TWndHandle  hwndThis
        , const tCIDCtrls::TWndMsg     wmsgMsg
        , const tCIDCtrls::TWParam     wParam
        , const tCIDCtrls::TLParam     iParam
    );
}

