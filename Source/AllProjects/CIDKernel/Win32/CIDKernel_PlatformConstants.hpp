//
// FILE NAME: CIDKernel_PlatformConstants.hpp
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
//  This file provides the platform dependent constants of the kCIDLib
//  namespace. It is the platform dependent sibling of the file
//  CIDKernel_Constant.hpp, which has all of the platform independent core
//  constants.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace kCIDLib
{
    // -----------------------------------------------------------------------
    //  File system constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4               c4MaxPathLen    = 260;
    constexpr tCIDLib::TCh                  chDefParmSep    = L'/';
    constexpr tCIDLib::TCh                  chMultiPathSep  = L';';
    constexpr tCIDLib::TCh                  chPathSep       = L'\\';
    const tCIDLib::TCh* const               pszAllFilesSpec = L"*";
    const tCIDLib::TCh* const               pszAllDirsSpec  = L"*";
    const tCIDLib::TCh* const               szWhitespace    = L"\r\n\t\a\f\v ";
    const tCIDLib::TCh* const               szExeExtension  = L".Exe";
    const tCIDLib::TCh* const               szLibExtension  = L".DLL";
    const tCIDLib::TCh* const               szMultiPathSep  = L";";
    const tCIDLib::TCh* const               szPathSep       = L"\\";
    const tCIDLib::TCh* const               szDefParmSep    = L"/";
    const tCIDLib::TCh* const               szSysExePath    = L"PATH";
    const tCIDLib::TCh* const               szSysLibPath    = L"PATH";


    // -----------------------------------------------------------------------
    //  Memory related constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4               c4MemPageSize   = 4096;
    constexpr tCIDLib::TCard4               c4CacheAlign    = 8;


    // -----------------------------------------------------------------------
    //  Our native character encoding
    // -----------------------------------------------------------------------
    constexpr const tCIDLib::TCh* const     pszNativeEncoding = L"UTF-16LE";


    // -----------------------------------------------------------------------
    //  Process and thread related constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TProcessId           pidInvalid      = 0;
    constexpr tCIDLib::TThreadId            tidInvalid      = 0;


    // -----------------------------------------------------------------------
    //  Character oriented constants. Here, we are responsible for the short
    //  and long char types. The Unicode char type is defined in a platform
    //  independent way.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4               c4SCharBytes    = 1;
    constexpr tCIDLib::TSCh                 schMaxSChar     = static_cast<char>(0x7F);
    constexpr tCIDLib::TCard4               c4CharBytes     = 2;
    constexpr tCIDLib::TCh                  chMaxChar       = 0xFFFF;


    // -----------------------------------------------------------------------
    //  Max handles we can wait on at once. It's one less than the max so
    //  that we can have a slot for our own internal use.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4               c4MaxWaitHandles = 63;

    // -----------------------------------------------------------------------
    //  Since we currently allow each platform to use its own wide character
    //  format in memory, we need to have this
    // -----------------------------------------------------------------------
    #if defined(CIDLIB_LITTLEENDIAN)
    constexpr tCIDLib::EBaseTextFmts        eWCCharFmt = tCIDLib::EBaseTextFmts::TwoByte_LE;
    #else
    constexpr tCIDLib::EBaseTextFmts        eWCCharFmt = tCIDLib::EBaseTextFmts::TwoByte_BE;
    #endif
}


namespace kCIDSock
{
    // -----------------------------------------------------------------------
    //  Socket related constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4               c4MaxSelect     = 64;
}


namespace kCIDGraphDev
{

    // ------------------------------------------------------------------------
    //  These are the invalid values for some graphics oriented types
    // ------------------------------------------------------------------------
    constexpr tCIDGraphDev::TBmpHandle      hbmpInvalid     = nullptr;
    constexpr tCIDGraphDev::TBrushHandle    hbrInvalid      = nullptr;
    constexpr tCIDGraphDev::TDeviceHandle   hdevInvalid     = nullptr;
    constexpr tCIDGraphDev::TFontHandle     hfontInvalid    = nullptr;
    constexpr tCIDGraphDev::TIconHandle     hicoInvalid     = nullptr;
    constexpr tCIDGraphDev::TPenHandle      hpenInvalid     = nullptr;
    constexpr tCIDGraphDev::TRegionHandle   hrgnInvalid     = nullptr;
}


namespace kCIDCtrls
{
    // ------------------------------------------------------------------------
    //  Invalid handle values
    // ------------------------------------------------------------------------
    constexpr tCIDCtrls::TAccelHandle       haccelInvalid   = nullptr;
    constexpr tCIDCtrls::TMenuHandle        hmenuInvalid    = nullptr;
    constexpr tCIDCtrls::TWndHandle         hwndInvalid     = nullptr;
    constexpr tCIDCtrls::TTimerId           tmidInvalid     = 0;
    constexpr tCIDCtrls::TWndId             widInvalid      = 0xFFFFFFFF;
}

