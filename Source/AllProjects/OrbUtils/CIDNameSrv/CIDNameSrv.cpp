//
// FILE NAME: CIDNameSrv.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/09/2001
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
#include    "CIDNameSrv.hpp"


// ----------------------------------------------------------------------------
//  Main module magic macro
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacCIDNameSrv>(&facCIDNameSrv, &TFacCIDNameSrv::eMainThread)
    )
)


// ----------------------------------------------------------------------------
//  Declare the one global object, our facility object
// ----------------------------------------------------------------------------
TFacCIDNameSrv  facCIDNameSrv;

