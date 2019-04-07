//
// FILE NAME: MakeDocs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "MakeDocs.hpp"



// -----------------------------------------------------------------------------
//  Local function prototypes
//
//  eMainTheadFunc
//      This is the thread function for the main thread that provides the
//      execution for this program.
// -----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// -----------------------------------------------------------------------------
//  Glocal data declarations
//
//  facMakeDocs
//      This is our facility object. This program is complex enough that its
//      advantageous to have a facility class of its own. This allows for
//      easy coordination of any global data.
// -----------------------------------------------------------------------------
TFacMakeDocs    facMakeDocs;


// -----------------------------------------------------------------------------
//  Include magic main module code
// -----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"MakeDocsMainThread", eMainThreadFunc))


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

//
//  This is the main program thread, where the program starts. The use of the
//  CIDLib_MainModule macro above makes this thread start as the first thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid* const)
{
    // Let our caller go
    thrThis.Sync();
    try
    {

    }

    catch(TError& errToCatch)
    {
        return tCIDLib::EExitCodes::FatalError;
    }

    catch(...)
    {
        return tCIDLib::EExitCodes::SystemException;

    }

    return tCIDLib::EExitCodes::Normal;
}

