//
// FILE NAME: CIDDBase.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main file of the facility. It provides an init function that
//  is called by the magic main module macro that users must include in their
//  main program file.
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
#include    "CIDDBase_.hpp"


// ---------------------------------------------------------------------------
//  This facility talks directly to some system APIs, so we need to force it
//  to bring in the ODBC library, so as not to force this on everyone.
// ---------------------------------------------------------------------------
#pragma comment(lib, "odbc32.lib")


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------

//
//  The lazy evaluation method for the facility object. This is the only
//  access to the facility object, insuring that it gets allocated when
//  its required.
//
TFacCIDDBase& facCIDDBase()
{
    static TFacCIDDBase* pfacCIDDBase = nullptr;
    if (!pfacCIDDBase)
    {
        TBaseLock lockInit;
        if (!pfacCIDDBase)
            pfacCIDDBase = new TFacCIDDBase;
    }
    return *pfacCIDDBase;
}



