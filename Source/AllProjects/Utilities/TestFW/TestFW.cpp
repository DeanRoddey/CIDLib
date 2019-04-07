//
// FILE NAME: TestFW.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2007
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
//  This is the main implementation file of the program. We just define the
//  single global object (the facilty object) and the main module magic
//  macro which starts up the program on a method of the facility object.
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
#include    "TestFW.hpp"


// ----------------------------------------------------------------------------
//  Declare the facilty object
// ----------------------------------------------------------------------------
TFacTestFW facTestFW;



// ----------------------------------------------------------------------------
//  Include magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacTestFW>(&facTestFW, &TFacTestFW::eMainThread)
    )
)

