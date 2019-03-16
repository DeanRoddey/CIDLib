//
// FILE NAME: CIDSock_BinarySockStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2000
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
//  This file implements the simple classes that create a binary stream over
//  a socket. All they have to do is to create the impl object and store it
//  with the base class.
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
RTTIDecls(TBinInSockStream,TBinInStream)
RTTIDecls(TBinOutSockStream,TBinOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TBinStreamInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinInSockStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinInSockStream::TBinInSockStream( const   tCIDLib::TEncodedTime enctWait
                                    ,       TStreamSocket* const  psockToAdopt
                                    , const tCIDLib::EAdoptOpts   eAdopt) :
    m_pstrmiIn(nullptr)
{
    //
    //  Create the impl object and store a convenience pointer for ourselves,
    //  but our parent adopts it.
    //
    m_pstrmiIn = new TSockInStreamImpl(enctWait, psockToAdopt, eAdopt);
    AdoptImplObject(m_pstrmiIn);
}

TBinInSockStream::TBinInSockStream( const   tCIDLib::TEncodedTime enctWait
                                    , const TBinOutSockStream&    strmToSyncTo) :
    m_pstrmiIn(nullptr)
{
    //
    //  Create an input impl object synced to the passed output stream's
    //  impl object. Keep a convenience pointer for ourselves, but our parent
    //  adopts it.
    //
    m_pstrmiIn = new TSockInStreamImpl(enctWait, *strmToSyncTo.m_pstrmiOut);
    AdoptImplObject(m_pstrmiIn);
}

TBinInSockStream::~TBinInSockStream()
{
}


// ---------------------------------------------------------------------------
//  TBinInSockStream: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TBinInSockStream::bIsDataAvailable() const
{
    return m_pstrmiIn->bIsDataAvailable();
}


tCIDLib::TBoolean
TBinInSockStream::bWaitForDataReady(const tCIDLib::TEncodedTime enctWait) const
{
    return m_pstrmiIn->bWaitForDataReady(enctWait);
}

tCIDLib::TBoolean
TBinInSockStream::bWaitForDataReadyMS(const tCIDLib::TCard4 c4WaitMillis) const
{
    return m_pstrmiIn->bWaitForDataReadyMS(c4WaitMillis);
}


tCIDLib::TEncodedTime TBinInSockStream::enctMaxWait() const
{
    return m_pstrmiIn->enctMaxWait();
}

tCIDLib::TEncodedTime
TBinInSockStream::enctMaxWait(const tCIDLib::TEncodedTime enctToSet)
{
    return m_pstrmiIn->enctMaxWait(enctToSet);
}




// ---------------------------------------------------------------------------
//   CLASS: TBinStreamOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinOutSockStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinOutSockStream::TBinOutSockStream(       TStreamSocket* const    psockToAdopt
                                    , const tCIDLib::EAdoptOpts     eAdopt) :
    m_pstrmiOut(nullptr)
{
    // Create the impl object, and keep a convenience pointer to it for ourselves
    m_pstrmiOut = new TSockOutStreamImpl(psockToAdopt, eAdopt);

    // Store it on our parent class
    AdoptImplObject(m_pstrmiOut);
}

TBinOutSockStream::~TBinOutSockStream()
{
}

