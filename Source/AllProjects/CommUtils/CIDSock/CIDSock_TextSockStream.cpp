//
// FILE NAME: CIDSock_TextSockStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/26/2000
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
//  This file implements the simple classes that create a text stream over
//  a binary stream over a socket. They just create the binary stream and
//  store it on the base class.
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
#include    "CIDSock_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TTextInSockStream,TTextInStream)
RTTIDecls(TTextOutSockStream,TTextOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TTextInSockStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextInSockStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextInSockStream::TTextInSockStream(const  tCIDLib::TEncodedTime enctMaxWait
                                    ,       TStreamSocket* const  psockToAdopt
                                    , const tCIDLib::EAdoptOpts   eAdopt
                                    ,       TTextConverter* const ptcvtToAdopt) :
    TTextInStream(ptcvtToAdopt)
    , m_pstrmiIn(nullptr)
{
    // Create a binary input sock stream object
    m_pstrmiIn = new TBinInSockStream(enctMaxWait, psockToAdopt, eAdopt);
    AdoptStream(m_pstrmiIn);
}

TTextInSockStream::TTextInSockStream(const  tCIDLib::TEncodedTime enctMaxWait
                                    ,const  TTextOutSockStream&   strmToSyncTo
                                    ,       TTextConverter* const ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmiIn(nullptr)
{
    // Create an input socket synced to the passed output stream
    m_pstrmiIn = new TBinInSockStream(enctMaxWait, *strmToSyncTo.m_pstrmiOut);
    AdoptStream(m_pstrmiIn);
}

TTextInSockStream::~TTextInSockStream()
{
}


// ---------------------------------------------------------------------------
//  TTextInSockStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTextInSockStream::bIsDataAvailable() const
{
    return m_pstrmiIn->bIsDataAvailable();
}


tCIDLib::TBoolean
TTextInSockStream::bWaitForDataReady(const tCIDLib::TEncodedTime enctWait) const
{
    return m_pstrmiIn->bWaitForDataReady(enctWait);
}

tCIDLib::TBoolean
TTextInSockStream::bWaitForDataReadyMS(const tCIDLib::TCard4 c4WaitMillis) const
{
    return m_pstrmiIn->bWaitForDataReadyMS(c4WaitMillis);
}




// ---------------------------------------------------------------------------
//   CLASS: TTextOutSockStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextOutSockStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextOutSockStream::
TTextOutSockStream(         TStreamSocket* const    psockToAdopt
                    , const tCIDLib::EAdoptOpts     eAdopt
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextOutStream(ptcvtToAdopt)
    , m_pstrmiOut(nullptr)
{
    // Create a sock stream object and store it on our member for later use
    m_pstrmiOut = new TBinOutSockStream(psockToAdopt, eAdopt);

    // Store it on our parent class
    AdoptStream(m_pstrmiOut);
}

TTextOutSockStream::
TTextOutSockStream(TBinOutSockStream* const pstrmToAdopt, TTextConverter* const ptcvtToAdopt) :

    TTextOutStream(ptcvtToAdopt)
    , m_pstrmiOut(pstrmToAdopt)
{
    // Store the output stream on our parent class
    AdoptStream(m_pstrmiOut);
}

TTextOutSockStream::~TTextOutSockStream()
{
}

