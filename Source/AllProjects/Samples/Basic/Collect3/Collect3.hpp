//
// FILE NAME: Collect3.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/1997
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
