//
// FILE NAME: CIDIDL.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/08/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
//
// DESCRIPTION:
//
//  This is the main header of the CID IDL compiler product. It does the usual
//  things that a CIDLib based facility main file does. Mainly it maps the
//  entry point, in this case to a facility method, and it declares the
//  actual facility object. Since this is not a DLL/SharedLib, we don't have
//  to use a lazy eval method.
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
#include    "CIDIDL.hpp"


// ----------------------------------------------------------------------------
//  Main module magic macro
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacCIDIDL>(&facCIDIDL, &TFacCIDIDL::eMainThread)
    )
)


// ----------------------------------------------------------------------------
//  Declare the one global object, our facility object
// ----------------------------------------------------------------------------
TFacCIDIDL  facCIDIDL;

