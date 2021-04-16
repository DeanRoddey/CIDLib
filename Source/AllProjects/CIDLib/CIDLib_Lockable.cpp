//
// FILE NAME: CIDLib_Lockable.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/26/2019
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
//  This file just implements the wee bit of out of line stuff from the
//  MLockable mixin.
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
//  Magic macros
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
//   CLASS: MLockable
//  PREFIX: mlock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MDuplicable: Public, static methods
// ---------------------------------------------------------------------------
MLockable& MLockable::Nul_MLockable()
{
    //
    //  We need some object to represent this and it's not a concrete class
    //  so we just use a mutex which implements this interface.
    //
    static TMutex mlockNull;
    return mlockNull;
}
