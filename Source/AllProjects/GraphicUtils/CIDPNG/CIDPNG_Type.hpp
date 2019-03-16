//
// FILE NAME: CIDPNG_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/18/2004
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
//  This is the type header for this facility. Some of these are really only
//  for internal use, but they are referenced in public class headers.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace tCIDPNG
{
    // -----------------------------------------------------------------------
    //  Our representation of the chunk types. We make it bitmapped so that
    //  we can also use it to keep up with whether we've seen specific
    //  chunks.
    // -----------------------------------------------------------------------
    enum class EChunks : tCIDLib::TCard4
    {
        Unknown         = 0x00000000
        , Header        = 0x00000001
        , Palette       = 0x00000002
        , Trans         = 0x00000004
        , Data          = 0x00000018
        , End           = 0x00000010

        // We don't keep up with these since they are very optional
        , Gamma         = 0x00100000
        , BgnClr        = 0x00200000

        // And our private chunks
        , TransClr      = 0x10000000
    };


    // -----------------------------------------------------------------------
    //  Compression types
    // -----------------------------------------------------------------------
    enum class ECompTypes
    {
        Deflate   = 0

        , Count
    };


    // -----------------------------------------------------------------------
    //  Interlace schemes. The literal values are 0 and 1, so the enum can
    //  be cast to/from the actual value.
    // -----------------------------------------------------------------------
    enum class EInterlaces
    {
        None
        , Adam7

        , Count
    };
}


// ---------------------------------------------------------------------------
//  Do streaming macros for some enums
// ---------------------------------------------------------------------------
EnumBinStreamMacros(tCIDPNG::ECompTypes)
EnumBinStreamMacros(tCIDPNG::EInterlaces)

