//
// FILE NAME: CIDCrypto.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/21/1997
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
