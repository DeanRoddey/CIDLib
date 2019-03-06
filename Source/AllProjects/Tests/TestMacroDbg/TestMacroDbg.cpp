//
// FILE NAME: TestMacroDbg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#include    "TestMacroDbg.hpp"


// ----------------------------------------------------------------------------
//  Main module magic macro
// ----------------------------------------------------------------------------
CIDLib_CtrlsModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacTestMacroDbg>(&facTestMacroDbg, &TFacTestMacroDbg::eMainThread)
    )
)


// ----------------------------------------------------------------------------
//  Our one global object, which is the facility object for this facility
// ----------------------------------------------------------------------------
TFacTestMacroDbg facTestMacroDbg;


