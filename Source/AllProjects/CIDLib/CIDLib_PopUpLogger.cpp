//
// FILE NAME: CIDLib_PopUpLogger.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1996
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
//  This file implements a popup logger, which sends logged errors to a
//  the standard popup mechanism.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TPopUpLogger,TObject)



// ---------------------------------------------------------------------------
//  TPopUpLogger: Constructors and operators
// ---------------------------------------------------------------------------
TPopUpLogger::TPopUpLogger()
{
}

TPopUpLogger::~TPopUpLogger()
{
}


// ---------------------------------------------------------------------------
//  TPopUpLogger: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPopUpLogger::LogEvent(const TLogEvent& logevToLog)
{
    //
    //  We just pass this on to the facility object which has the services
    //  for doing popups in a convenient way, though it just turns around
    //  and passes it on to lower level services in the kernel.
    //
    facCIDLib().ErrorPopUp(logevToLog);
}

