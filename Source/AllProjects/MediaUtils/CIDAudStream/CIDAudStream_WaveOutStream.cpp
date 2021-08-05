//
// FILE NAME: CIDAudStream_WaveOutStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements a standard output stream that works over Windows audio
//  WAVE outputs.
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
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDWaveOutStream,TCIDAudioTarStream)



// ---------------------------------------------------------------------------
//   CLASS: TCIDWaveOutStream
//  PREFIX: sprecev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDWaveOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDWaveOutStream::TCIDWaveOutStream() :

    m_pInfo(nullptr)
{
}

TCIDWaveOutStream::~TCIDWaveOutStream()
{
    try
    {
        // Let the per-platform stuff clean up
        Terminate();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TCIDWaveOutStream: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDWaveOutStream::ThrowNotReady() const
{
    facCIDAudStream().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kAudStrErrs::errcStrm_NotReady
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotReady
    );
}

