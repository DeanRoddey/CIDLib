//
// FILE NAME: CIDImage.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
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
//  This is the main file of the facility.
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
#include    "CIDImage_.hpp"


// ---------------------------------------------------------------------------
//  Global methods
// ---------------------------------------------------------------------------
TFacCIDImage& facCIDImage()
{
    static TFacCIDImage facCIDImage;
    return facCIDImage;
}

