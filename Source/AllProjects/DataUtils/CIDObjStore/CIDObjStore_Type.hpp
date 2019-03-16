//
// FILE NAME: CIDObjStore_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2002
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
//  This is the header for the non-class types of this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace tCIDObjStore
{
    // -----------------------------------------------------------------------
    //  The flags for initializing the engine.
    //
    //  Normally the key sare not case sensitive, but this allows for dups if you are not
    //  careful. You can make them sensitive, but have to be sure you keep the keys in
    //  a consistent case or you will not find things that are really in the store.
    // -----------------------------------------------------------------------
    enum class EFlags : tCIDLib::TCard2
    {
        None              = 0x0000
        , CaseSensitive   = 0x0001

        , Count           = 2
    };
}
