//
// FILE NAME: CIDCypto_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/28/1998
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
//  This header provides the tCIDCrypto namespace which, by CIDLib
//  convention, contains all of the non-class specific typedefs and enums
//  for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace tCIDCrypto
{
    // -----------------------------------------------------------------------
    //  The standard block cypher modes
    // -----------------------------------------------------------------------
    enum class EBlockModes
    {
        ECB
        , CBC
        , CBC_IV
        , OFB

        , Count
        , Min           = ECB
        , Max           = OFB
    };
}
