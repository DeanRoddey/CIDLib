//
// FILE NAME: CIDCrypto.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/21/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main file of the facility. It contains any global data or
//  basic initialization required for this facility.
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
#include    "CIDCrypto_.hpp"


// ---------------------------------------------------------------------------
//  Public functions
// ---------------------------------------------------------------------------
TFacCIDCrypto& facCIDCrypto()
{
    static TFacCIDCrypto* pfacCIDCrypto = nullptr;
    if (!pfacCIDCrypto)
    {
        TBaseLock lockInit;
        if (!pfacCIDCrypto)
            pfacCIDCrypto = new TFacCIDCrypto;
    }
    return *pfacCIDCrypto;
}
