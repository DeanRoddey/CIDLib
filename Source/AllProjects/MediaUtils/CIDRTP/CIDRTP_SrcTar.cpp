//
// FILE NAME: CIDRTP_DataSrc.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
//  This file implements the abstracted data source interface for this facility.
//  Various derivatives will be created for data coming from WAV files, CID media
//  codec,
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
#include    "CIDRTP_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDRTPIO,TObject)
RTTIDecls(TCIDRTPIOSrc,TCIDRTPIO)
RTTIDecls(TCIDRTPIOTar,TCIDRTPIO)



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIO
// PREFIX: rtpio
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTPIO: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDRTPIO::TCIDRTPIO()
{
}

TCIDRTPIO::~TCIDRTPIO()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIOSrc
// PREFIX: rtpio
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTPIOSrc: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDRTPIOSrc::TCIDRTPIOSrc()
{
}

TCIDRTPIOSrc::~TCIDRTPIOSrc()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIOTar
// PREFIX: rtpio
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTPIOTar: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDRTPIOTar::TCIDRTPIOTar()
{
}

TCIDRTPIOTar::~TCIDRTPIOTar()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPIOJan
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTPIOJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDRTPIOJan::TCIDRTPIOJan() :

    m_prtpioToSanitize(nullptr)
{
}

TCIDRTPIOJan::TCIDRTPIOJan(TCIDRTPIO* const prtpioToSanitize) :

    m_prtpioToSanitize(prtpioToSanitize)
{
}

TCIDRTPIOJan::~TCIDRTPIOJan()
{
    // Clean up the source
    if (m_prtpioToSanitize)
        CleanupSource();
}


// ---------------------------------------------------------------------------
//  TCIDRTPIOJan: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Provide access to the data in the janitor
const TCIDRTPIO& TCIDRTPIOJan::rtpioData() const
{
    CIDAssert(m_prtpioToSanitize != 0, L"The RTP I/O janitor object is not set");
    return *m_prtpioToSanitize;
}

TCIDRTPIO& TCIDRTPIOJan::rtpioData()
{
    CIDAssert(m_prtpioToSanitize != 0, L"The RTP I/O janitor object is not set");
    return *m_prtpioToSanitize;
}


// Orphan the object out of the janitor. The caller becomes responsible for it
TCIDRTPIO* TCIDRTPIOJan::prtpioOrphan()
{
    TCIDRTPIO* prtpioRet = m_prtpioToSanitize;
    m_prtpioToSanitize = nullptr;
    return prtpioRet;

}

tCIDLib::TVoid TCIDRTPIOJan::Orphan()
{
    m_prtpioToSanitize = nullptr;
}


tCIDLib::TVoid TCIDRTPIOJan::SetNew(TCIDRTPIO* const prtpioToSanitize)
{
    // If there's a current source, clean it up
    if (m_prtpioToSanitize)
        CleanupSource();

    // And store the new one
    m_prtpioToSanitize = prtpioToSanitize;
}


// ---------------------------------------------------------------------------
//  TCIDRTPIOJan: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDRTPIOJan::CleanupSource()
{
    try
    {
        m_prtpioToSanitize->Terminate();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    delete m_prtpioToSanitize;
    m_prtpioToSanitize = nullptr;
}


