//
// FILE NAME: CIDKernel_PlatformDefines.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/08/1998
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
//  This file provides the platform dependent core defines for the system.
//  The core platform independent stuff is in CIDKernel_DevMacros.hpp in the
//  CIDKernel directory.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Win32 platform is always little endian. Big endian platforms will instead
//  define the CIDLIB_BIGENDIAN token.
// ---------------------------------------------------------------------------
#define CIDLIB_LITTLEENDIAN



// ---------------------------------------------------------------------------
//  Use the CPU defines provided by the compiler to drive the CIDLib CPU
//  define that is used by all CIDLib code.
//
//  When porting to another development environment that does not provide
//  these, you might have to do a conditional here based on compiler and
//  do a separate set of these.
// ---------------------------------------------------------------------------
#if defined(_M_IX86)
#define CIDLIB_CPU_X86
#elif defined(_M_PPC)
#define CIDLIB_CPU_PPC
#elif defined(_M_ALPHA)
#define CIDLIB_CPU_ALPHA
#elif defined(_M_MRX000)
#define CIDLIB_CPU_MIPS
#endif



// ---------------------------------------------------------------------------
//  Define the import/export keywords for the Win32 platform.
//
//  CID_DLLIMPORT
//  CID_DLLEXPORT
//      These are used in headers to import/export classes.
//
//  CID_FNEXPORT
//      This is used in cpp files to unconditionally export a function. In
//      most cases this is used to export 'factory functions' from driver
//      style DLL/SharedLibs. In the Win32 case, its the same as the other
//      export scheme.
// ---------------------------------------------------------------------------
#define CID_DLLIMPORT   DLLIMPORT
#define CID_DLLEXPORT   DLLEXPORT
#define CID_FNEXPORT    DLLEXPORT



// ---------------------------------------------------------------------------
//  Define a packing scheme for this platform. Most of the compilers for
//  Win32 will support the push/pop scheme, but you can conditionalize this
//  on particular compiler if not.
//
//  CIDLIBPACK is the default packing that is used around classes.
// ---------------------------------------------------------------------------
#define CIDLIBPACK  4

#define CIDLIB_PACK(v)  pack(push,v)

#define CIDLIB_POPPACK  pack(pop)



// ---------------------------------------------------------------------------
//  This one is used to force local data to be correctly cache aligned, which
//  is required if it is going to be interlocked exchanged.
// ---------------------------------------------------------------------------
#define CID_CACHEALIGN __declspec(align(32))



// ---------------------------------------------------------------------------
//  If our local wide character type (tCIDLib::TCh) is the same as a UTF-16
//  Unicode character, then define this. Else don't define it. This will allow
//  some streaming code in CIDLib to be more efficient.
// ---------------------------------------------------------------------------
#define CIDLIB_WCISUTF16


// ---------------------------------------------------------------------------
//  Define our version of the magic main module macro. We don't need to do
//  anything platform specific here particularly, other than using the needed
//  wide character entry point.
//
//  We just call the CIDLib init stuff, then create a new thread using the
//  passed constructor macro text, make it the primary thread for the program,
//  and then start it and wait forever for it to end, returning its exit code
//  as the return for the program.
// ---------------------------------------------------------------------------
#define CIDLib_MainModule(thrCtor) \
extern "C" int wmain(); \
int wmain() \
{ \
    TThread* pthrToStart = new thrCtor; \
    CIDLib_MakePrimary(pthrToStart); \
    CIDLib_Init(); \
    pthrToStart->Start(); \
    return int(pthrToStart->eWaitForDeath()); \
}


// ---------------------------------------------------------------------------
//  An alternative main for programs that want to be Win32 services. In this
//  case we don't start the client main thread. A callback on the user's
//  handler will be made to tell him to start it.
//
//  In this case we also call InitKrnlModule to set the 'we are in a service'
//  flag. This is the only place where it's really convenient to set such
//  a flag.
// ---------------------------------------------------------------------------
#define CIDLib_MainService(pszName,psrvhCtor,debug) \
extern "C" int wmain(); \
int wmain() \
{ \
    TCIDKrnlModule::InitKrnlModule(kCIDLib::True); \
    CIDLib_Init(); \
    TWin32ServiceHandler* psrvhNew = new psrvhCtor; \
    if (!TKrnlWin32Service::bInit(pszName, psrvhNew, debug)) \
        return int(tCIDLib::EExitCodes::InitFailed); \
    return int(tCIDLib::EExitCodes::Normal); \
}



// ---------------------------------------------------------------------------
//  This macro is used to check for debug being on or off. This is done for
//  flexibility, instead of doing the ifdef(x) everywhere. This is driven by
//  the standard platform define for debug mode.
// ---------------------------------------------------------------------------
#if     defined(_DEBUG)
#define CID_DEBUG_ON        1
#define CID_DEBUG_OFF       0
#else
#define CID_DEBUG_ON        0
#define CID_DEBUG_OFF       1
#endif


// ---------------------------------------------------------------------------
//  We suppress the warning that VC++ kicks out when you use a template member
//  of an exported class. When the C++ spec deals with exports of templates,
//  we'll deal with it completely.
// ---------------------------------------------------------------------------
#pragma warning(disable : 4251 4121)


