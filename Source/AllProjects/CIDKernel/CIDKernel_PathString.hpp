//
// FILE NAME: CIDKernel_PathString.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/08/1999
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
//  This is the header for the CIDKernel_Path.Cpp file. This file implements
//  the TKrnlPathStr namespace, which provides path parsing services for the
//  system. Path's are a tricky issue, so this stuff tries to abstract them
//  to the extent possible.
//
//  The assumptions of paths are that they follow the basic Unix/Win32/OS2
//  model. The components of a path are:
//
//  1)  The path. This is the part between the volume and any actual file
//      name at the end of the path.
//  2)  The name. This is the base part of any file name at the end of the
//      path, not including any extension.
//  3)  The extension. The file extension part of the file name. A file name
//      does not have to have an extension, but it usually does.
//  4)  Optionally a 'node' name can be prepended, which handles the scenarios
//      like PC LANs, where a remote path is \\node/volume/path/etc... The
//      node is the name of the remote workstation.
//
// CAVEATS/GOTCHAS:
//
//  1)  The methods here all assume that leading and trailing whitespace has
//      been stripped from the string. The TPathStr class, which is the main
//      user of this class, always takes care of this.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlPathStr
// ---------------------------------------------------------------------------
namespace TKrnlPathStr
{
    KRNLEXPORT tCIDLib::TBoolean bFindPart
    (
        const   tCIDLib::TCh* const pszSrc
        ,       tCIDLib::TCard4&    c4Start
        ,       tCIDLib::TCard4&    c4End
        , const tCIDLib::EPathParts ePart
    );

    KRNLEXPORT tCIDLib::TBoolean bHasExt
    (
        const   tCIDLib::TCh* const     pszToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bHasName
    (
        const   tCIDLib::TCh* const     pszToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bHasPath
    (
        const   tCIDLib::TCh* const     pszToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bHasVolume
    (
        const   tCIDLib::TCh* const     pszToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsFullyQualified
    (
        const   tCIDLib::TCh* const     pszToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryExt
    (
        const   tCIDLib::TCh* const pszSrc
        ,       tCIDLib::TCh* const pszToFill
        , const tCIDLib::TCard4     c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryName
    (
        const   tCIDLib::TCh* const pszSrc
        ,       tCIDLib::TCh* const pszToFill
        , const tCIDLib::TCard4     c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryNameExt
    (
        const   tCIDLib::TCh* const pszSrc
        ,       tCIDLib::TCh* const pszToFill
        , const tCIDLib::TCard4     c4MaxChars
    );

    tCIDLib::TBoolean bQueryPart
    (
        const   tCIDLib::TCh* const pszSrc
        ,       tCIDLib::TCh* const pszToFill
        , const tCIDLib::TCard4     c4MaxChars
        , const tCIDLib::EPathParts ePart
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryPath
    (
        const   tCIDLib::TCh* const pszSrc
        ,       tCIDLib::TCh* const pszToFill
        , const tCIDLib::TCard4     c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryVolume
    (
        const   tCIDLib::TCh* const pszSrc
        ,       tCIDLib::TCh* const pszToFill
        , const tCIDLib::TCard4     c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bRemoveLevel
    (
                tCIDLib::TCh* const pszToEdit
    );
}

#pragma CIDLIB_POPPACK
