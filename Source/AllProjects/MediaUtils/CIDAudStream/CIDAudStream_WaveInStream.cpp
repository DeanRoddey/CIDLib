//
// FILE NAME: CIDAudStream_WaveInSrcStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/03/2017
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
//  This file implements a standard input stream that works over Windows audio inputs.
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
RTTIDecls(TWaveInSrcStream,TCIDAudioSrcStream)



// ---------------------------------------------------------------------------
//   CLASS: TWaveInSrcStream
//  PREFIX: sprecev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TWaveInSrcStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TWaveInSrcStream::TWaveInSrcStream() :

    m_pInfo(nullptr)
{
}

TWaveInSrcStream::~TWaveInSrcStream()
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
//  TWaveInSrcStream: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TWaveInSrcStream::ThrowNotReady() const
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

