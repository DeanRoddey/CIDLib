//
// FILE NAME: CIDImage_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/25/1998
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
//  This header contains the public types namespace for this facility.
//
// CAVEATS/GOTCHAS:
//
//  1)  Some of our types need to be known by the CIDKernel.Dll facility
//      because some of the operations are host specific. It still puts
//      them into the tCIDImage namespace so they looke like we define them.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace tCIDImage
{
    // -----------------------------------------------------------------------
    //  The types of bitmap compression
    // -----------------------------------------------------------------------
    enum class EBmpComp
    {
        None
        , RLE8
        , RLE4
        , RLE24
        , Huffman1D
        , BitFields

        , Count
        , Min           = None
        , Max           = BitFields
    };


    // -----------------------------------------------------------------------
    //  The types of bitmaps files
    // -----------------------------------------------------------------------
    enum class EBmpTypes
    {
        Win_1x
        , Win_2x
        , Win_3x
        , Win_4x
        , OS2_2x

        , Count
        , Min           = Win_1x
        , Max           = OS2_2x
    };


    // -----------------------------------------------------------------------
    //  The default palettes that we provide. TClrPalette can construct
    //  from one of these values to get them.
    // -----------------------------------------------------------------------
    enum class EDefPalettes
    {
        BlackAndWhite
        , Default16
        , GrayScale16
        , Default256
        , GrayScale256
        , LinearGray

        , Count
        , Min           = BlackAndWhite
        , Max           = LinearGray
    };



    // -----------------------------------------------------------------------
    //  The possible formats of a textually formatted color palette. Used
    //  by TClrPalette to inport/export palettes.
    // -----------------------------------------------------------------------
    enum class EPalTxtFmts
    {
        SpaceDelimited_OnePer
        , SpaceDelimited
        , CommaDelimited
        , CommaDelimited_OnePer
    };


    // -----------------------------------------------------------------------
    //  Some types of images are stored top to bottom and some bottom to top,
    //  so each derived image class sets a value of this type on the base
    //  image class to indicate which type it is.
    // -----------------------------------------------------------------------
    enum class ERowOrders
    {
        TopDown
        , BottomUp
    };


    // ---------------------------------------------------------------------------
    //  Provide streaming support and standard enum tricks for some of our enums
    // ---------------------------------------------------------------------------
    #if !defined(CIDIMAGE_NOCLASSES)
    EnumStreamMacros(tCIDImage::EBmpTypes)
    EnumStreamMacros(tCIDImage::EBmpComp)
    EnumBinStreamMacros(tCIDImage::EPalTxtFmts)
    EnumBinStreamMacros(tCIDImage::ERowOrders)
    #endif

    StdEnumTricks(tCIDImage::EBmpTypes)
    StdEnumTricks(tCIDImage::EBmpComp)
    StdEnumTricks(tCIDImage::EDefPalettes)
}
