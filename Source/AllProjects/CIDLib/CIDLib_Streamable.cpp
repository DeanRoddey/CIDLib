//
// FILE NAME: CIDLib_Streamable.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
    static TPoint* pstrmblNull = nullptr;
    if (!pstrmblNull)
    {
        TBaseLock lockInit;
        if (!pstrmblNull)
            TRawMem::pExchangePtr(pstrmblNull, new TPoint);
    }
    return *pstrmblNull;
}

