//
// FILE NAME: CIDKernel_ConCharSet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/04/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains an enum that enumerates the 'virtual console character
//  set' that the TKrnlConIn class must support in its 'single character'
//  mode. It must translate all user input into either an actual character, or
//  one of the special control characters. This shelters the rest of the
//  system from how the characters are encoded.
//
//  If its an actual character, then the ConKey_Char value is used, and the
//  char returned is the Unicode code point for that char (in the local wide
//  char type.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// Like all of the non-class types, its part of the tCIDLib namespace
namespace tCIDLib
{
    enum class EConKeys
    {
        // Miscellaneous keys
        Null
        , Backspace
        , BackTab
        , Break
        , Char
        , Delete
        , Enter
        , Escape
        , Insert
        , Tab

        // Movement keys
        , Up
        , Down
        , Left
        , Right
        , Previous
        , Next
        , Home
        , End
    };
}
