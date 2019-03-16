//
// FILE NAME: CIDGraphDev_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This is the types header for the CIDGraphDev facility. It contains all
//  of the non-class enums, structures, function prototypes, etc... that
//  are not totally class specific (class specific ones are nested within
//  that class.)
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that this file will be used directly by the CIDResComp resource
//      compiler, which has to understand some of these types in order to build the
//      stored resource formats.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace tCIDGraphDev
{
    // -----------------------------------------------------------------------
    //  The ways you can weave a clip region or path into the existing clip
    //  region.
    // -----------------------------------------------------------------------
    enum class EClipModes
    {
        And
        , Copy
        , Diff
        , Or
        , Xor
    };


    // -----------------------------------------------------------------------
    //  These are used to filter fonts during font enumeration and selection
    // -----------------------------------------------------------------------
    enum class EFontBases
    {
        Device
        , Software
        , Either
    };

    enum class EFontFamilies
    {
        DontCare
        , Decorative
        , Modern
        , Roman
        , Script
        , Swiss
    };

    enum class EFontPitches
    {
        Fixed
        , Variable
        , Either
    };

    enum class EFontTypes
    {
        Scalable
        , Raster
        , Either
    };


    // -----------------------------------------------------------------------
    //  The ways that font selection uses the requested height value
    // -----------------------------------------------------------------------
    enum class EFontHeights
    {
        Default
        , EM
        , Cell
    };


    // -----------------------------------------------------------------------
    //  Icons come in a few standard sizes. This enum allows you to indicate
    //  which one you want.
    // -----------------------------------------------------------------------
    enum class EIconSizes
    {
        Small
        , Normal
        , Large

        , Count
        , Min           = Small
        , Max           = Large
    };


    // -----------------------------------------------------------------------
    //  The flags available for text display
    // -----------------------------------------------------------------------
    enum class ETextFmts : tCIDLib::TCard4
    {
        None            = 0x00000000
        , NoClip        = 0x00000001
        , Mnemonic      = 0x00000002
        , StrikeOut     = 0x00000004
        , Disabled      = 0x00000008

        , Reserve       = 0x10000000
        , All           = 0x0000000F
    };
}

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Provide streaming and standard enum support for some of our enums
// ---------------------------------------------------------------------------
#if !defined(CIDGRAPHDEV_NOCLASSES)
EnumBinStreamMacros(tCIDGraphDev::EFontHeights)
EnumBinStreamMacros(tCIDGraphDev::ETextFmts)

#endif

