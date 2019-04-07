//
// FILE NAME: CIDKernel_PlatformDefines.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 10/11/1998
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
//  This file provides the platform dependent core defines for the system.
//  The core platform independent stuff is in CIDKernel_DevMacros.Hpp in the
//  CIDKernel directory.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define CIDLIB_LITTLEENDIAN
#else
#define CIDLIB_BIGENDIAN
#endif

// ---------------------------------------------------------------------------
//  Use the CPU defines provided by the compiler to drive the CIDLib CPU
//  define that is used by all CIDLib code.
//
//  When porting to another development environment that does not provide
//  these, you might have to do a conditional here based on compiler and
//  do a separate set of these.
// ---------------------------------------------------------------------------
#if defined(__i386__)
#define CIDLIB_CPU_X86
#elif defined(__ppc__)
#define CIDLIB_CPU_PPC
#elif defined(__alpha__)
#define CIDLIB_CPU_ALPHA
#elif defined(__mips__)
#define CIDLIB_CPU_MIPS
#endif


// ---------------------------------------------------------------------------
//  Define the import/export keywords as blanks
// ---------------------------------------------------------------------------
#define CID_DLLIMPORT
#define CID_DLLEXPORT


// ---------------------------------------------------------------------------
//  Define a packing scheme for this platform. Most of the compilers for
//  Win32 will support the push/pop scheme, but you can conditionalize this
//  on particular compiler if not.
//
//  CIDLIBPACK is the default packing that is used around classes.
// ---------------------------------------------------------------------------
#define CIDLIBPACK  4

#define CIDLIB_PACK(v)  pack(push, v)
#define CIDLIB_POPPACK  pack(pop)

// ---------------------------------------------------------------------------
//  Define our version of the magic main module macro. We need to grab argc
//  argv here, so that when the TKrnlFileSys command-line related functions
//  are called, the result is consistent with what people would expect if they
//  had grabbed argc and argv themselves.
// ---------------------------------------------------------------------------
#define CIDLib_MainModule(thrCtor) \
int main(int argc, char** argv) \
{ \
    extern int CIDKernel_SystemInfo_Linux_argc; \
    extern char** CIDKernel_SystemInfo_Linux_argv; \
    CIDKernel_SystemInfo_Linux_argc = argc; \
    CIDKernel_SystemInfo_Linux_argv = argv; \
    CIDLib_Init(); \
    TThread* pthrToStart = new thrCtor; \
    CIDLib_MakePrimary(pthrToStart); \
    pthrToStart->Start(); \
    return pthrToStart->eWaitForDeath(); \
}
