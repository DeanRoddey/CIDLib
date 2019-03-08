//
// FILE NAME: TestFWLib.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#include    "TestFWLib_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacTestFWLib& facTestFWLib()
{
    static TFacTestFWLib* pfacTestFWLib = nullptr;
    if (!pfacTestFWLib)
    {
        TBaseLock lockInit;
        if (!pfacTestFWLib)
            pfacTestFWLib = new TFacTestFWLib;
    }
    return *pfacTestFWLib;
}

