//
// FILE NAME: CIDLib_Streamable.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/1996
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
//  This file provides the little bit of out of line implementation
//  needed for the MStreamable mixin class. Note that, we need to be able
//  to provide a null object for this class, which raises some issues since
//  this class is not directly constructable. So we just construct a TPoint
//  object which implements MStreamable. That will serve for our needs.
//  The outside world doesn't know what it is.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  CLASS: MStreamable
// PREFIX: strmbl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MStreamable: Public, static methods
// ---------------------------------------------------------------------------
MStreamable& MStreamable::Nul_MStreamable()
{
    // This isn't a concrete class so we just use a point object which implements it
    static TPoint mstrmblNull;
    return mstrmblNull;
}

