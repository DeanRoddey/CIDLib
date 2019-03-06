//
// FILE NAME: CIDLogSrv.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main implementation file. It just provides the magic program
//  entry point macro, which starts the primary thread on a member function
//  of the facility class.
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
#include    "CIDLogSrv.hpp"


// ----------------------------------------------------------------------------
//  Main module magic macro
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacCIDLogSrv>(&facCIDLogSrv, &TFacCIDLogSrv::eMainThread)
    )
)


// ----------------------------------------------------------------------------
//  Declare the one global object, our facility object
// ----------------------------------------------------------------------------
TFacCIDLogSrv   facCIDLogSrv;

