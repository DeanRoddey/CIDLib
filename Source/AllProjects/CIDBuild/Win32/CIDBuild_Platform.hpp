//
// FILE NAME: CIDBuild_Platform.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This is the platform specific header for the Win32 platform. It allows
//  the platform to define certain things that might be different on each
//  platform.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  If we are under MS VC++, then this section handles all of the stuff that
//  is specific to it.
// ---------------------------------------------------------------------------
#if defined(_MSC_VER)

#if defined(PLATFORM_WIN32_WIN7)
#define UNICODE
#define _UNICODE
#else
#error Unknown target platform for VC++
#endif

#else

#error Unknown target platform

#endif


// ---------------------------------------------------------------------------
//  Define our endianness
// ---------------------------------------------------------------------------
#define CIDBUILD_LITTLEENDIAN


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
    using TFileHandle = void*;


    // -----------------------------------------------------------------------
    //  Define the local wide character. This should map to the type of char
    //  that the locale wchar_t maps to. If wchar_t is intrinsic, then just
    //  use that here. Otherwise, if it would require the inclusion of RTL
    //  headers to do it because its just a typedef, then just use the
    //  equivalent fundamental type here.
    // -----------------------------------------------------------------------
    using TCh = wchar_t;


    // -----------------------------------------------------------------------
    //  And define the local short character type, which is almost always just
    //  the char type.
    // -----------------------------------------------------------------------
    using TSCh = char;
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
    //      all C/Cpp and Hpp/H files.
    //
    //  pszDllExt
    //  pszLibExt
    //  pszExeExt
    //  pszObjExt
    //      These are the common output extensions that we use in this
    //      program. The input extensions are predestined by what we named
    //      the files, so the are not a portability problem.
    //
    //  chExtSep
    //  chPathSep
    //  pszPathSep
    //  chPathCompSep
    //      Some separator characters that are platform specific. The Comp
    //      one is for path component separators, such as in PATH=x;y; and so
    //      forth.
    // -----------------------------------------------------------------------
    const tCIDBuild::TCh* const  pszAllHFiles       = L"*.h";
    const tCIDBuild::TCh* const  pszAllHppFiles     = L"*.hpp";
    const tCIDBuild::TCh* const  pszAllCFiles       = L"*.c";
    const tCIDBuild::TCh* const  pszAllCppFiles     = L"*.cpp";
    const tCIDBuild::TCh* const  pszDllExt          = L".dll";
    const tCIDBuild::TCh* const  pszExeExt          = L".exe";
    const tCIDBuild::TCh* const  pszHppExt          = L".hpp";
    const tCIDBuild::TCh* const  pszLibExt          = L".lib";
    const tCIDBuild::TCh* const  pszObjExt          = L".obj";
    const tCIDBuild::TCh         chExtSep           = L'.';
    const tCIDBuild::TCh         chPathCompSep      = L';';



    // -----------------------------------------------------------------------
    //  Define some platform strings
    //
    //  pszPlatformDir
    //      This tells us what the name of the per-platform subdirectory
    //      is. If a project indicates that it has a per-platform sub-
    //      directory, then there will be a sub-directory with this name
    //      under the project directory.
    // -----------------------------------------------------------------------
    #if defined(PLATFORM_WIN32_WIN7)
    const tCIDBuild::TCh* const  pszPlatformDir     = L"Win32";
    const tCIDBuild::TCh* const  pszBasePlatform    = L"Win32";
    const tCIDBuild::TCh* const  pszFullPlatform    = L"Win32_Win7";
    #else
    #error Unknown target platform
    #endif
}

