//
// FILE NAME: CIDLib_SearchNSort.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/27/2011
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  The bulk of the search and sort stuff is templatized. But, anything
//  we can out of line we do, to limit bloat.
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


//
//  If the array is less than the minimum merge size we just return the
//  array size and it'll be processed as a whole. If it's a power of two,
//  then we return the minimum merge size / 2. Else it returns:
//
//      minmerge / 2 <= return vale <= minmerge
//
//  Such that the array size divided by the returned value is close to but
//  less than an exact power of 2.
//
tCIDLib::TCard4 TArrayOps::c4CalcMinRunLen(const tCIDLib::TCard4 c4ArraySz)
{
    tCIDLib::TCard4 c4Bumper = 0;
    tCIDLib::TCard4 c4MinRun;

    c4MinRun = c4ArraySz;

    while (c4MinRun >= TArrayOps::c4MinMergeSz)
    {
        c4Bumper |= (c4MinRun & 1);
        c4MinRun >>= 1;
    }
    return c4MinRun + c4Bumper;
}


