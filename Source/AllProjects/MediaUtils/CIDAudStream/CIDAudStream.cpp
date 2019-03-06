//
// FILE NAME: CIDAudStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
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
#include    "CIDAudStream_.hpp"


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDAudStream& facCIDAudStream()
{
    static TFacCIDAudStream* pfacCIDAudStream = 0;
    if (!pfacCIDAudStream)
    {
        TBaseLock lockInit;
        if (!pfacCIDAudStream)
            TRawMem::pExchangePtr(pfacCIDAudStream, new TFacCIDAudStream);
    }
    return *pfacCIDAudStream;
}

