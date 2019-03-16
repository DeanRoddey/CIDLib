//
// FILE NAME: CIDWebBrowser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2005
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
#include    "CIDWebBrowser_.hpp"


// ---------------------------------------------------------------------------
//  The lazy eval method for our facility object
// ---------------------------------------------------------------------------
TFacCIDWebBrowser& facCIDWebBrowser()
{
    static TFacCIDWebBrowser* pfacCIDWebBrowser = nullptr;
    if (!pfacCIDWebBrowser)
    {
        TBaseLock lockInit;
        if (!pfacCIDWebBrowser)
            pfacCIDWebBrowser = new TFacCIDWebBrowser;
    }
    return *pfacCIDWebBrowser;
}


