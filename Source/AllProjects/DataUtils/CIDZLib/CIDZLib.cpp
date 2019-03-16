//
// FILE NAME: CIDZLib.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/2004
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
//  This is the main file of the facility.
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
#include    "CIDZLib_.hpp"


//
//  This is the lazy eval method for the facility object.
//
TFacCIDZLib& facCIDZLib()
{
    static TFacCIDZLib* pfacCIDZLib = nullptr;
    if (!pfacCIDZLib)
    {
        TBaseLock lockInit;
        if (!pfacCIDZLib)
            pfacCIDZLib = new TFacCIDZLib;
    }
    return *pfacCIDZLib;
}

