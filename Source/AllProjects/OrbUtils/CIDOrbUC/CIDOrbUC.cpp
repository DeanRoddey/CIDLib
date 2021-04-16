//
// FILE NAME: CIDOrbUC.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDOrbUC_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDOrbUC& facCIDOrbUC()
{
    static TFacCIDOrbUC* pfacThis = new TFacCIDOrbUC();
    return *pfacThis;
}

