//
// FILE NAME: CIDLib_Signals.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/25/1999
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
//  This file implements the TSignals namespace.
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
//   CLASS: TSignalJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSignalJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TSignalJanitor::TSignalJanitor(MSignalHandler* const pmshToAdd) :

    m_pmshToAdd(pmshToAdd)
{
    m_c4HandlerId = TSignals::c4AddHandler(pmshToAdd);
}

TSignalJanitor::~TSignalJanitor()
{
    TSignals::RemoveHandler(m_pmshToAdd, m_c4HandlerId);
}


// ---------------------------------------------------------------------------
//  TSignals namespace methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TSignals::c4AddHandler(MSignalHandler* const pmshToAdd)
{
    return TKrnlSignals::c4AddHandler(pmshToAdd);
}


tCIDLib::TVoid TSignals::GenerateCtrlCSignal()
{
    TKrnlSignals::GenerateCtrlCSignal();
}


tCIDLib::TVoid
TSignals::RemoveHandler(        MSignalHandler* const   pmshToRemove
                        , const tCIDLib::TCard4         c4HandlerId)
{
    if (!TKrnlSignals::bRemoveHandler(pmshToRemove, c4HandlerId))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSig_CantRemoveHandler
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

