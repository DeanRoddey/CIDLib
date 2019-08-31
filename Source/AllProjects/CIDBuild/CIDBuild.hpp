//
// FILE NAME: CIDBuild.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This is the main header for the program. It subincludes all the other
//  headers for the program, and it provides the two namespaces for constants
//  and types.
//
// CAVEATS/GOTCHAS:
//
//  1)  There is a little conditional code here, though not much.
//
//  2)  This guy directly brings in some headers from the CIDKernel and
//      CIDWnd facilties, so that it can have access to the format of the
//      message file structures. This is a very special case, but it saves a
//      lot of paranoia about keeping two different versions in sync.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Bring in a little stuff from CIDKernel and CIDWnd that we have to share
// ---------------------------------------------------------------------------
#if defined(PLATFORM_WIN32_WIN7)
    #include    "../CIDKernel/Win32/CIDKernel_PlatformDefines.hpp"
    #include    "../CIDKernel/Win32/CIDKernel_PlatformTypes.hpp"
    #define     CIDBUILD_WIDEMAIN 1
#elif defined(PLATFORM_LINUX)
    #include    "../CIDKernel/Linux/CIDKernel_PlatformDefines.hpp"
    #include    "../CIDKernel/Linux/CIDKernel_PlatformTypes.hpp"
    #define     CIDBUILD_WIDEMAIN 1
#else
    #error Unknown platform define
#endif

#define     CIDKRNL_NOCLASSES
#include    "../CIDKernel/CIDKernel_DevMacros.hpp"
#include    "../CIDKernel/CIDKernel_Type.hpp"
#include    "../CIDKernel/CIDKernel_Constant.hpp"
#include    "../CIDKernel/CIDKernel_Unicode.hpp"

// This one has to see the public types above
#if defined(PLATFORM_WIN32_WIN7)
    #include    "../CIDKernel/Win32/CIDKernel_PlatformConstants.hpp"
#elif defined(PLATFORM_LINUX)
    #include    "../CIDKernel/Linux/CIDKernel_PlatformConstants.hpp"
#else
    #error Unknown platform define
#endif

#define     CIDCTRLS_NOCLASSES
#include    "../CIDKernel/CIDKernel_Type_Ctrls.hpp"



// ---------------------------------------------------------------------------
//  The platform specific header
// ---------------------------------------------------------------------------
#if defined(PLATFORM_WIN32_WIN7)
    #include "Win32\CIDBuild_Platform.hpp"
#elif defined(PLATFORM_LINUX)
    #include "Linux/CIDBuild_Platform.hpp"
#else
    #error("Unknown platform define")
#endif



// ---------------------------------------------------------------------------
//  Program types namespace
// ---------------------------------------------------------------------------
namespace tCIDBuild
{
    // -----------------------------------------------------------------------
    //  These represent the major actions that the program can be told to
    //  do, via the /Actions= parameter.
    // -----------------------------------------------------------------------
    enum class EActions
    {
        Build
        , MakeDeps
        , ShowProjDeps
        , ShowProjSettings
        , CopyHeaders
        , Debug
        , MakeRes
        , MakeBinRelease
        , MakeDevRelease
        , IDLGen
        , Bootstrap
    };


    // -----------------------------------------------------------------------
    //  When there is an option to adopt or not adopt something, this enum
    //  is used.
    // -----------------------------------------------------------------------
    enum class EAdoptModes
    {
        NoAdopt
        , Adopt
    };


    // -----------------------------------------------------------------------
    //  This enum represents the two possible build types we do, either a
    //  production or a development build.
    // -----------------------------------------------------------------------
    enum class EBldModes
    {
        Develop
        , Production
    };


    // -----------------------------------------------------------------------
    //  The possible display types support, which is a per-project setting.
    // -----------------------------------------------------------------------
    enum class EDisplayTypes
    {
        NotApplicable
        , Console
        , GUI
    };


    // -----------------------------------------------------------------------
    //  The set of simple encodings we support
    // -----------------------------------------------------------------------
    enum class EEncodings
    {
        UTF16
        , ASCII
    };


    // -----------------------------------------------------------------------
    //  A simple set of generic errors that are thrown from internal stuff.
    // -----------------------------------------------------------------------
    enum class EErrors
    {
        BadParams
        , CopyFailed
        , FileNotFound
        , FileFormat
        , UnexpectedEOF
        , Internal
        , NotFound
        , AlreadyExists
        , IndexError
        , Full
        , NotImplemented
        , DependError
        , CreateError
        , BuildError
        , NotSupported
        , WriteError
        , ReadError
        , OpenError
        , QueryError
        , SeekError
        , QueryCurDir
        , MakeTmpFile
        , NotADir
    };


    // -----------------------------------------------------------------------
    //  The header dump modes. Full mode does the full recursive dump, whereas
    //  the standard mode just does one per facility, since for the most part
    //  there aren't any differences among the files of a facility, unless a
    //  file brings in a private header.
    // -----------------------------------------------------------------------
    enum class EHdrDmpModes
    {
        None
        , Std
        , Full
    };


    // -----------------------------------------------------------------------
    //  The modes that the project iterator can iterate the tree of project
    //  dependencies.
    //
    //  A minimal iteration never repeats a previously handled project.
    //  Otherwise, the entire tree is iterated.
    // -----------------------------------------------------------------------
    enum class EIterModes : tCIDLib::TCard2
    {
        BottomUp            = 0x0001
        , Minimal           = 0x0002
        , SkipTarget        = 0x0004

        // Some common bit combinations
        , FromBottomMin     = 0x0003
        , FromBottomMax     = 0x0001
        , FromTopMin        = 0x0002
        , FromTopMax        = 0x0000
        , FromBottomMinSkip = 0x0007
    };


    // -----------------------------------------------------------------------
    //  The path modes available when finding file info objects. It can give
    //  back just the base file name found, or the name with the relative path
    //  used to find it, or the full path to the found file.
    // -----------------------------------------------------------------------
    enum class EPathModes
    {
        Base
        , Relative
        , Full
    };


    // -----------------------------------------------------------------------
    //  There are five types of projects. We have SharedLibs, SharedObjs,
    //  StaticLibs, Executables, and Services. On Win32, SharedLibs/SharedObjs
    //  are the same thing, but there is a distinction on Unix. Executables and
    //  Services are both executable programs, but services are ones designed
    //  to run in the background, so sometimes different things need to be
    //  done.
    //
    //  File copy projects just have a source directory and a list of file
    //  copy statements. But they can also have dependent projects, to insure
    //  that they are done after any dependents are completed.
    //
    //  Groups are purely a list of dependent projects, so that we can force the
    //  building of sets of projects.
    // -----------------------------------------------------------------------
    enum class EProjTypes
    {
        SharedLib
        , SharedObj
        , StaticLib
        , Executable
        , Service

        // From here up are non-code type projects
        , FileCopy
        , Group

        , MaxCodeType       = Service
    };


    // -----------------------------------------------------------------------
    //  The ways that a project can ask to link to the C++ RTL. This is not
    //  generally important, since almost everything will use the default
    //  but it can be important for some standalone utilities.
    // -----------------------------------------------------------------------
    enum class ERTLModes
    {
        SingleStatic
        , SingleDynamic
        , MultiStatic
        , MultiDynamic
    };


    // -----------------------------------------------------------------------
    //  These are used to send special chars into text streams. There are
    //  actually short name defines for them below to make it easier.
    // -----------------------------------------------------------------------
    enum class ESpecialChars
    {
        EndLine
    };


    // -----------------------------------------------------------------------
    //  These represent the standard files, and are accepted by the binary
    //  and text file classes.
    // -----------------------------------------------------------------------
    enum class EStdFiles
    {
        Out
        , In
        , Err
    };


    // -----------------------------------------------------------------------
    //  These represent the tokens that can be found in a project or depend
    //  file. The 'get next token' parsing methods return these.
    // -----------------------------------------------------------------------
    enum class ETokens
    {
        None
        , Define
        , UnDef
        , IfDef
        , IfNotDef
        , EndIf
        , Include
    };
};



// ---------------------------------------------------------------------------
//  Internal constants
// ---------------------------------------------------------------------------
namespace kCIDBuild
{
    // -----------------------------------------------------------------------
    //  Shorthand aliases for the special chars
    // -----------------------------------------------------------------------
    const   tCIDBuild::ESpecialChars    EndLn(tCIDBuild::ESpecialChars::EndLine);


    // -----------------------------------------------------------------------
    //  General constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4NotFound      = kCIDLib::c4MaxCard;


    // -----------------------------------------------------------------------
    //  Some project related constants
    //
    //  pszAllProjects
    //      The name of the magic project that represents the "build all"
    //      target.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const pszAllProjects  = L"All";


    // -----------------------------------------------------------------------
    //  The path separators used in the project files. If not the same as the platform
    //  path separator, we have to update the paths as we parse the file.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh      chProjectPathSep = L'\\';


    // -----------------------------------------------------------------------
    //  Flags for TUtils::bExec
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4ExecFlag_None     = 0;
    const tCIDLib::TCard4   c4ExecFlag_Async    = 0x00000001;
    const tCIDLib::TCard4   c4ExecFlag_LowPrio  = 0x00000002;


    // -----------------------------------------------------------------------
    //  Define some mode specific strings
    // -----------------------------------------------------------------------
    #if defined(BLDMODE_DEV)
    const tCIDLib::TCh* const  pszBuildMode   = L"BLDMODE_DEV";
    #elif defined(BLDMODE_PROD)
    const tCIDLib::TCh* const  pszBuildMode   = L"BLDMODE_PROD";
    #else
    #error("Unknown build mode")
    #endif
};



// ---------------------------------------------------------------------------
//  We need an out of line helper for some of the first headers below to be able
//  to log stuff if they are inlined.
// ---------------------------------------------------------------------------
namespace tCIDBuild
{
    tCIDLib::TVoid LogMsg
    (
        const   tCIDLib::TCh* const         pszMsg
    );
}


// ---------------------------------------------------------------------------
//  The rest of the program includes
// ---------------------------------------------------------------------------
#include    "CIDBuild_Janitor.hpp"
#include    "CIDBuild_RawStr.hpp"
#include    "CIDBuild_String.hpp"
#include    "CIDBuild_List.hpp"

namespace tCIDBuild
{
    typedef TList<TBldStr>      TStrList;
}

#include    "CIDBuild_Utils.hpp"
#include    "CIDBuild_BinFile.hpp"
#include    "CIDBuild_TextFile.hpp"


// ---------------------------------------------------------------------------
//  Export global data
//
//  stdOut
//      The standard output text stream that the program uses to output its
//      info.
// ---------------------------------------------------------------------------
extern TTextFile    stdOut;

#include    "CIDBuild_MsgIdInfo.hpp"
#include    "CIDBuild_FindInfo.hpp"
#include    "CIDBuild_KeyValuePair.hpp"

namespace tCIDBuild
{
    typedef TList<TKeyValuePair>    TKVPList;
}

#include    "CIDBuild_LineSpooler.hpp"
#include    "CIDBuild_IDLInfo.hpp"
#include    "CIDBuild_DependGraph.hpp"
#include    "CIDBuild_ProjectInfo.hpp"
#include    "CIDBuild_FileDepend.hpp"
#include    "CIDBuild_ProjectList.hpp"
#include    "CIDBuild_ToolsDriver.hpp"
#include    "CIDBuild_BlkParser.hpp"
#include    "CIDBuild_ResCompiler.hpp"

#include    "CIDBuild_Facility.hpp"



// ---------------------------------------------------------------------------
//  Export global data
//
//  facCIDBuild
//      The facility object for this program.
// ---------------------------------------------------------------------------
extern TFacCIDBuild facCIDBuild;


// ---------------------------------------------------------------------------
//  A macro to allow ++ operators for enums
// ---------------------------------------------------------------------------
#define EnumIncMacro(typ) \
typ operator++(typ& in, int) { in = typ(tCIDLib::TCard4(in) + 1); return in; }


// ---------------------------------------------------------------------------
//  Some global insertion operators to display our enums
// ---------------------------------------------------------------------------
TTextFile& operator<<(TTextFile&, const tCIDBuild::EActions);
TTextFile& operator<<(TTextFile&, const tCIDBuild::EBldModes);
TTextFile& operator<<(TTextFile&, const tCIDBuild::EErrors);
TTextFile& operator<<(TTextFile&, const tCIDBuild::EProjTypes);

template <typename T> tCIDLib::TBoolean bTestBits(const T tLHS, const T tRHS)
{
    return (tCIDLib::TCard4(tLHS) & tCIDLib::TCard4(tRHS)) != 0;
}
