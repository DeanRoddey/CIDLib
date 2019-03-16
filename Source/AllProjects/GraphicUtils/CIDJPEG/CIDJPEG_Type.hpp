//
// FILE NAME: CIDJPEG_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2005
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
//  This is the type header for this facility. Some of these are really for
//  internal use, but are referenced in public APIs.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace tCIDJPEG
{
    // Indicates the sampling used for compression
    enum class EOutSamples
    {
        F4_2_0
        , F4_2_1
        , F4_2_2
        , F4_4_4
    };
}



