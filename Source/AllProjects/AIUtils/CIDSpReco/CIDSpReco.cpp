//
// FILE NAME: CIDSpReco.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main implementation file of the CIDLib Speeci Recognition support
//  facility.
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
#include    "CIDSpReco_.hpp"


// ---------------------------------------------------------------------------
//  Force some non-standard libraries to be linked in
// ---------------------------------------------------------------------------
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "sapi.lib")


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDSpReco& facCIDSpReco()
{
    static TFacCIDSpReco* pfacCIDSpReco = 0;
    if (!pfacCIDSpReco)
    {
        TBaseLock lockInit;
        if (!pfacCIDSpReco)
            TRawMem::pExchangePtr(pfacCIDSpReco, new TFacCIDSpReco);
    }
    return *pfacCIDSpReco;
}
