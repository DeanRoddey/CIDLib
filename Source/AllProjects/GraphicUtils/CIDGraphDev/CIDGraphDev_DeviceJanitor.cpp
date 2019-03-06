//
// FILE NAME: CIDGraphDev_DeviceJanitor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TGraphicDevJanitor class, which is a janitor
//  class for saving and restoring the state of a drawable device.
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
#include    "CIDGraphDev_.hpp"


// ---------------------------------------------------------------------------
//  RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGraphicDevJanitor,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TGraphicDevJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphicDevJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TGraphicDevJanitor::TGraphicDevJanitor(TGraphDrawDev* const pgdevToSanitize) :

    m_pgdevSanitize(pgdevToSanitize)
{
    m_pgdevSanitize->PushContext();
}

TGraphicDevJanitor::~TGraphicDevJanitor()
{
    // Ask the the device to restore the last state that he saved
    m_pgdevSanitize->PopContext();
}

