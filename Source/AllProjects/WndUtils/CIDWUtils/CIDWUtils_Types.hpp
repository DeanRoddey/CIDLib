//
// FILE NAME: CIDWUtils_Types.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
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
//  The non-class types header for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace tCIDWUtils
{
    // Line states supported by the source editor
    enum class ECEdLStates : tCIDLib::TCard2
    {
        None                = 0
        , CurLine           = 0x0001
        , BreakPoint        = 0x0002
        , DisabledBreak     = 0x0004

        // For internal use
        , FirstOfLine       = 0x0008

        , All               = 0x000F
        , BPBits            = 6
        , BitCount          = 3
    };


    // The value returns from the error options dialog
    enum class EErrOpts
    {
        Ignore
        , Cancel
    };


    //
    //  Flags supported by our basic find criteria dialog, used mostly for simple
    //  searching in a multi-edit type control but possibly other things. These control
    //  which of the options the calling app wants to allow the user to set.
    //
    enum class EFindFlags : tCIDLib::TCard4
    {
        None                = 0x00000000
        , CanSearchSel      = 0x00000001
        , CanFullOnly       = 0x00000002

        , AllOpts           = 0x00000003
    };


    // The find/replace flags supported by our source editor class
    enum class EFindOpts
    {
        Case
        , PosixRE
        , RegEx
        , SelOnly
        , WholeWords
    };


    // Styles for the image preview window
    enum class EImgPStyles : tCIDLib::TCard4
    {
        None                = 0
        , Transparent       = 0x00000001
    };


    //
    //  Usec by some of the simple value input getting dialogs, to let the caller
    //  indicate some constraints on the value. StringNE means can't be empty. NZCardinal
    //  means it can't be zero.
    //
    enum class EListValFmts
    {
        Cardinal
        , NZCardinal
        , Integer
        , String
        , StringNE
    };


    // The value returns from the overwrite options dialog
    enum class EOverOpts
    {
        Cancel
        , Discard
        , Overwrite
    };


    // The value returns from the save options dialog
    enum class ESaveOpts
    {
        Cancel
        , Discard
        , Save
    };


    // Events reported by the source editor control
    enum class ESEdEvents
    {
        AddBP
        , DisableBP
        , EnableBP
        , Modified
        , RemoveBP
        , LineNum
    };


    // Styles for the text out window
    enum class ETOutStyles : tCIDLib::TCard4
    {
        None    = 0
    };
};

