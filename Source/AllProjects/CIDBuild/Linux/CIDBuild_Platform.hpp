//
// FILE NAME: CIDBuild_Platform.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/23/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the platform specific header for the Linux platform. It allows
//  the platform to define certain things that might be different on each
//  platform.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>

// ---------------------------------------------------------------------------
//  Define our endianness
// ---------------------------------------------------------------------------
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define CIDBUILD_LITTLEENDIAN
#else
#define CIDBUILD_BIGENDIAN
#endif

// ---------------------------------------------------------------------------
//  Define the packing mechanism
// ---------------------------------------------------------------------------
#define CIDBUILD_PACK(v)    pack(push,v)
#define CIDBUILD_POPPACK    pack(pop)

// ---------------------------------------------------------------------------
//  Provide the platform specific part of the types namespace
// ---------------------------------------------------------------------------
namespace tCIDBuild
{
    // -----------------------------------------------------------------------
    //  Define some types that we need to abstract on a per-platform basis.
    // -----------------------------------------------------------------------
    typedef int             TFileHandle;


    // -----------------------------------------------------------------------
    //  Define the local wide character. This should map to the type of char
    //  that the locale wchar_t maps to. If wchar_t is intrinsic, then just
    //  use that here. Otherwise, if it would require the inclusion of RTL
    //  headers to do it because its just a typedef, then just use the
    //  equivalent fundamental type here.
    // -----------------------------------------------------------------------
    typedef wchar_t         TCh;


    // -----------------------------------------------------------------------
    //  And define the local short character type, which is almost always just
    //  the char type.
    // -----------------------------------------------------------------------
    typedef char            TSCh;
}


// ---------------------------------------------------------------------------
//  Provide the platform specific part of the constants namespace
// ---------------------------------------------------------------------------
namespace kCIDBuild
{
    // -----------------------------------------------------------------------
    //  Define some platform specific file name and file path stuff.
    //
    //  pszAllHFiles
    //  pszAllCppFiles
    //      These should be set to the platform wildcard that will find
    //      all Cpp and Hpp/H files.
    //
    //  pszBldMode
    //      A string that represents the current build mode. Its just a
    //      convenience to avoid doing it over and over.
    //
    //  pszDllExt
    //  pszLibExt
    //  pszExeExt
    //  pszObjExt
    //      These are the common output extensions that we use in this
    //      program. The input extensions are predestined by what we named
    //      the files, so the are not a portability problem.
    // -----------------------------------------------------------------------
    const tCIDBuild::TCh* const  pszAllHFiles       = L"*.h*";
    const tCIDBuild::TCh* const  pszAllCppFiles     = L"*.cpp";
    const tCIDBuild::TCh* const  pszDllExt          = L".so";
    const tCIDBuild::TCh* const  pszExeExt          = L"";
    const tCIDBuild::TCh* const  pszHppExt          = L".hpp";
    const tCIDBuild::TCh* const  pszLibExt          = L".a";
    const tCIDBuild::TCh* const  pszObjExt          = L".o";
    const tCIDBuild::TCh         chExtSep           = L'.';



    // -----------------------------------------------------------------------
    //  Define some platform strings
    //
    //  pszPlatformDir
    //      This tells us what the name of the per-platform subdirectory
    //      is. If a project indicates that it has a per-platform sub-
    //      directory, then there will be a sub-directory with this name
    //      under the project directory.
    //
    //  pszTargetPlatform
    //      This is a constant string that contains the text of the target
    //      platform define. This saves from doing this work in all the
    //      places we might need it. Its passed to all compiled code as well
    //      as displayed in some places.
    // -----------------------------------------------------------------------
    const tCIDBuild::TCh* const  pszPlatformDir     = L"Linux";
    const tCIDBuild::TCh* const  pszTargetPlatform  = L"PLATFORM_LINUX";
}
