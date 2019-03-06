//
// FILE NAME: CIDLib_StatusController.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TStatusControllerTTY class, which is a simple
//  derivative of the abstract TStatusController class (also in our header.)
//  This canned derivative is provided because lots of TTY style output
//  programs can use this type of scheme.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TStatusController,TObject)
RTTIDecls(TStatusControllerTTY,TStatusController)



// ---------------------------------------------------------------------------
//   CLASS: TStatusControllerTTY
//  PREFIX: statc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStatusControllerTTY: Constuctors and Destructor
// ---------------------------------------------------------------------------
TStatusControllerTTY::TStatusControllerTTY(TTextOutStream* const pstrmToUse) :

    m_evWaitForIt(tCIDLib::EEventStates::Reset)
    , m_pstrmTarget(pstrmToUse)
{
}

TStatusControllerTTY::~TStatusControllerTTY() {}



// ---------------------------------------------------------------------------
//  TStatusControllerTTY: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TStatusControllerTTY::Starting(const TString& strStartText)
{
    *m_pstrmTarget << strStartText << kCIDLib::EndLn;
}


tCIDLib::TVoid
TStatusControllerTTY::WaitForComplete(const tCIDLib::TCard4 c4MilliSecs)
{
    m_evWaitForIt.WaitFor(c4MilliSecs);
}


tCIDLib::TVoid
TStatusControllerTTY::Working(  const    TString&           strStatusText
                                , const tCIDLib::TCard4     c4PercentComplete)
{
    *m_pstrmTarget << strStatusText << kCIDLib::EndLn;
}


tCIDLib::TVoid TStatusControllerTTY::WorkComplete(const TString& strClosingComment)
{
    *m_pstrmTarget << strClosingComment << kCIDLib::EndLn;
    m_evWaitForIt.Trigger();
}


// ---------------------------------------------------------------------------
//  TStatusControllerTTY: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TStatusControllerTTY::bIsDone()
{
    try
    {
        m_evWaitForIt.WaitFor(0);
    }

    catch(...)
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}