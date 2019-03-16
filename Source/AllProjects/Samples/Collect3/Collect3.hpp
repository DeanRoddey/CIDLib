//
// FILE NAME: Collect3.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the main header for the program. It brings anything needed by
//  the program so that all.cpp files just have to include this one file.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Bring in required underlying headers
// ---------------------------------------------------------------------------
#include    "CIDMath.hpp"


// ---------------------------------------------------------------------------
//  Typedef our collection
// ---------------------------------------------------------------------------
typedef TSortedBag<TCardinal>   TTestCol;


// ---------------------------------------------------------------------------
//  Bring in any program headers
// ---------------------------------------------------------------------------
#include    "Collect3_Thread.hpp"
