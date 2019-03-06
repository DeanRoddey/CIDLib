//
// FILE NAME: CIDKernel_PlatformConstants.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/25/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
    const tCIDLib::TCard4           c4MaxPathLen    = 255;
    const tCIDLib::TCh              chPathSeparator = L'/';
    const tCIDLib::TCh* const       szPathSeparator = L"/";
    const tCIDLib::TCh* const       szBadPathChars  = L"/;|<>";
    const tCIDLib::TCh* const       pszAllFilesSpec = L"*";
    const tCIDLib::TCh* const       pszAllDirsSpec  = L"*";
    const tCIDLib::TCh              chExtSeparator  = L'.';
    const tCIDLib::TCh* const       szExtSeparator  = L".";
    const tCIDLib::TCh* const       szWhitespace    = L"\r\n\t\a\f\v ";
    const tCIDLib::TCh* const       szExeExtension  = L"";
    const tCIDLib::TCh* const       szLibraryPrefix = L"lib";
    const tCIDLib::TCh* const       szLibrarySuffix = L".so";


    // -----------------------------------------------------------------------
    //  Memory related constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4           c4MemPageSize   = 4096;


    // -----------------------------------------------------------------------
    //  Process and thread related constants
    // -----------------------------------------------------------------------
    const tCIDLib::TProcessId       pidInvalid      = 0;
    const tCIDLib::TThreadId        tidInvalid      = 0;


    // -----------------------------------------------------------------------
    //  Socket oriented constants
    // -----------------------------------------------------------------------
    const tCIDLib::TIPAddr          ipaLoopBack     = 0x0100007F;
    const tCIDLib::TIPAddr          ipaAny          = 0;
    const tCIDLib::TIPAddr          ipaBroadcast    = 0xFFFFFFFF;


    // -----------------------------------------------------------------------
    //  Character oriented constants. Here, we are responsible for the short
    //  and long char types. The Unicode char type is defined in a platform
    //  independent way.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4           c4SCharBytes    = 1;
    const tCIDLib::TSCh             schMaxSChar     = char(0x7F);
    const tCIDLib::TCard4           c4CharBytes     = sizeof(wchar_t);
    const tCIDLib::TCh              chMaxChar       = wchar_t(0xFFFFFFFF);
}
