//
// FILE NAME: CIDAudStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
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
    static TFacCIDAudStream* pfacCIDAudStream = nullptr;
    if (!pfacCIDAudStream)
    {
        TBaseLock lockInit;
        if (!pfacCIDAudStream)
            pfacCIDAudStream = new TFacCIDAudStream;
    }
    return *pfacCIDAudStream;
}

