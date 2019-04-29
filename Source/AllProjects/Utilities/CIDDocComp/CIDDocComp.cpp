//
// FILE NAME: CIDDocComp.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "CIDDocComp.hpp"


// -----------------------------------------------------------------------------
//  Glocal data declarations
//
//  facCIDDocComp
//      This is our facility object. This program is complex enough that its
//      advantageous to have a facility class of its own. This allows for
//      easy coordination of any global data.
// -----------------------------------------------------------------------------
TFacCIDDocComp    facCIDDocComp;


// -----------------------------------------------------------------------------
//  Include magic main module code
// -----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacCIDDocComp>(&facCIDDocComp, &TFacCIDDocComp::eMainThread)
    )
)

