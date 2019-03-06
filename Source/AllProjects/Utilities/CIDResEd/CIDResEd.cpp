//
// FILE NAME: CIDResEd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/10/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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


