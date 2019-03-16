//
// FILE NAME: CIDResEd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/10/2000
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
//  This is the main module of the program. It just defines the facility
//  object and provides a little other stuff that has no other specific
//  home.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Force in any libraries we need
// ---------------------------------------------------------------------------
#pragma comment(lib, "User32.lib")



// ----------------------------------------------------------------------------
//  Global data declarations
// ----------------------------------------------------------------------------
TFacCIDResEd    facCIDResEd;


// ----------------------------------------------------------------------------
//  Magic program entry point macro
// ----------------------------------------------------------------------------
CIDLib_CtrlsModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacCIDResEd>(&facCIDResEd, &TFacCIDResEd::eMainThread)
    )
)


