//
// FILE NAME: CIDEncode_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/26/1999
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
//  This is the private header for the CIDEncode Facility. Source files
//  of this Facility only need to include this one header to get access to
//  all public and private stuff. Its also the precompiled headers file.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Import our own public header for internal consumption
// ---------------------------------------------------------------------------
#include    "CIDEncode.hpp"


// ---------------------------------------------------------------------------
//  Include any intrafacility headers
// ---------------------------------------------------------------------------
#include    "CIDEncode_MessageIds.hpp"

// Miscellaneous encodings
#include    "CIDEncode_ISO8859_1_.hpp"
#include    "CIDEncode_UCS4_.hpp"

// All the mode 1 encodings
#include    "CIDEncode_EBCDIC_037_.hpp"
#include    "CIDEncode_EBCDIC_1140_.hpp"
#include    "CIDEncode_ISO8859_2_.hpp"
#include    "CIDEncode_ISO8859_3_.hpp"
#include    "CIDEncode_ISO8859_4_.hpp"
#include    "CIDEncode_ISO8859_5_.hpp"
#include    "CIDEncode_ISO8859_6_.hpp"
#include    "CIDEncode_PC_437_.hpp"
#include    "CIDEncode_PC_850_.hpp"
#include    "CIDEncode_Win_1251_.hpp"
#include    "CIDEncode_Win_1252_.hpp"
#include    "CIDEncode_ZWaveOEM_.hpp"



// ---------------------------------------------------------------------------
//  We need a comparator function in a number of places where arrays of
//  TCharMap structures are binary searched.
// ---------------------------------------------------------------------------
inline tCIDLib::ESortComps
eCharMapComp(const  tCIDLib::TCh&           chTarget
            , const tCIDEncode::TCharMap&   cmComp)
{
    const tCIDLib::TInt4 i4Diff(tCIDLib::TInt4(chTarget) - tCIDLib::TInt4(cmComp.chWide));
    if (i4Diff < 0)
        return tCIDLib::ESortComps::FirstLess;
    else if (i4Diff > 0)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}

