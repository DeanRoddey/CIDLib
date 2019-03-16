//
// FILE NAME: CIDZLib_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/2004
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
//  This is the type header for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace tCIDZLib
{
    // -----------------------------------------------------------------------
    //  The aailable compression levels
    // -----------------------------------------------------------------------
    enum class ECompLevels
    {
        L0
        , L1
        , L2
        , L3
        , L4
        , L5
        , L6
        , L7
        , L8
        , L9

        , Count

        // Some named levels
        , None      = 0
        , BestSpeed = 1
        , BestCompr = 9
        , Default   = 6
    };
}

