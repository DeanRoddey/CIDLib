//
// FILE NAME: CIDKernel_RemMedia_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/27/2019
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
//  This file provides the Linux specific implementation for the class
//  TKrnlRemMedia. 
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
#include    "CIDKernel_.hpp"
#include    "CIDKernel_RemMedia.hpp"


// ---------------------------------------------------------------------------
//  TFacCIDKernel: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Not used yet, but here for future use
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermRemMedia(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
    }
     else if (eState == tCIDLib::EInitTerm::Terminate)
    {
    }
    return kCIDLib::True;
}

