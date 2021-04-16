//
// FILE NAME: CIDSpReco.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
//  This is the main implementation file of the CIDLib Speeci Recognition support
//  facility.
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
#include    "CIDSpReco_.hpp"


// ---------------------------------------------------------------------------
//  Force some non-standard libraries to be linked in
// ---------------------------------------------------------------------------
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "sapi.lib")


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------

// Fault in our facility object
TFacCIDSpReco& facCIDSpReco()
{
    static TFacCIDSpReco* pfacThis = new TFacCIDSpReco();
    return *pfacThis;
}
