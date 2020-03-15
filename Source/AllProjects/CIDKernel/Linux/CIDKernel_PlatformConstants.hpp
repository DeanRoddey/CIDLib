//
// FILE NAME: CIDKernel_PlatformConstants.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/25/1998
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
//  This file provides the platform dependent constants of the kCIDLib
//  namespace. It is the platform dependent sibling of the file
//  CIDKernel_Constant.Hpp, which has all of the platform independent core
//  constants.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


namespace kCIDLib
{
    // -----------------------------------------------------------------------
    //  File system constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4MaxPathLen        = 255;
    constexpr tCIDLib::TCh          chMultiPathSep      = L':';
    const tCIDLib::TCh* const       pszMultiPathSep     = L":";
    constexpr tCIDLib::TCh          chDefParmSep        = L'-';
    const tCIDLib::TCh* const       pszAllFilesSpec     = L"*";
    const tCIDLib::TCh* const       pszAllDirsSpec      = L"*";
    const tCIDLib::TCh* const       szWhitespace        = L"\r\n\t\a\f\v ";
    constexpr tCIDLib::TCh          chExtSeparator      = L'.';
    const tCIDLib::TCh* const       szExtSeparator      = L".";
    const tCIDLib::TCh* const       szExeExtension      = L"";
    const tCIDLib::TCh* const       szLibExtension      = L".so";
    const tCIDLib::TCh* const       szMultiPathSep      = L":";
    constexpr tCIDLib::TCh          chPathSep           = L'/';
    const tCIDLib::TCh* const       pszPathSep          = L"/";
    const tCIDLib::TCh* const       szSysExePath        = L"PATH";
    const tCIDLib::TCh* const       szSysLibPath        = L"LD_LIBRARY_PATH";


    // -----------------------------------------------------------------------
    //  Memory related constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4MemPageSize   = 4096;
    constexpr tCIDLib::TCard4       c4CacheAlign    = 4;


    // -----------------------------------------------------------------------
    //  Our native character encoding
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const       pszNativeEncoding = L"UTF-32";


    // -----------------------------------------------------------------------
    //  Process and thread related constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TProcessId   pidInvalid      = 0;
    constexpr tCIDLib::TThreadId    tidInvalid      = 0;


    // -----------------------------------------------------------------------
    //  Socket oriented constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TIPAddr      ipaLoopBack     = 0x0100007F;
    constexpr tCIDLib::TIPAddr      ipaAny          = 0;
    constexpr tCIDLib::TIPAddr      ipaBroadcast    = 0xFFFFFFFF;


    // -----------------------------------------------------------------------
    //  Character oriented constants. Here, we are responsible for the short
    //  and long char types. The Unicode char type is defined in a platform
    //  independent way.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4SCharBytes    = 1;
    constexpr tCIDLib::TSCh         schMaxSChar     = char(0x7F);
    constexpr tCIDLib::TCard4       c4CharBytes     = sizeof(wchar_t);
    constexpr tCIDLib::TCh          chMaxChar       = wchar_t(0xFFFFFFFF);


    // -----------------------------------------------------------------------
    //  Max handles we can wait on at once. It's one less than the max so
    //  that we can have a slot for our own internal use.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4MaxWaitHandles = 63;
}


namespace kCIDSock
{
    // -----------------------------------------------------------------------
    //  Socket related constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4MaxSelect     = 64;
}
