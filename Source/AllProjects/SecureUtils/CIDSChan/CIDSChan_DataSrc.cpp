//
// FILE NAME: CIDSChan_DataSrc.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/22/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements some secure channel based data sources that work over
//  a stream socket. We have a client one and a server one.
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
#include    "CIDSChan_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TCIDSChanClDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDSChanClDataSrc: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDSChanClDataSrc::
TCIDSChanClDataSrc(         TClientStreamSocket* const  psockSrc
                    , const tCIDLib::EAdoptOpts         eAdopt
                    , const TString&                    strPrincipal) :

    m_pcdsRawData(new TCIDSockStreamDataSrc(psockSrc, eAdopt))
    , m_strPrincipal(strPrincipal)
{
}

TCIDSChanClDataSrc::
TCIDSChanClDataSrc( const   TIPEndPoint&            ipepTar
                    , const tCIDSock::ESockProtos   eProtocol
                    , const TString&                strPrincipal
                    , const tCIDLib::TStrCollect&   colALPNList) :

    m_pcdsRawData(nullptr)
    , m_strPrincipal(strPrincipal)
{
    TClientStreamSocket* psockSrc = nullptr;
    try
    {
        psockSrc = new TClientStreamSocket(eProtocol, ipepTar);
        m_pcdsRawData = new TCIDSockStreamDataSrc(psockSrc, tCIDLib::EAdoptOpts::Adopt);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (psockSrc)
        {
            try
            {
                delete psockSrc;
            }

            catch(...)
            {
            }
        }
        throw;
    }

    // Copy over the protocol list if any
    if (!colALPNList.bIsEmpty())
    {
        TColCursor<TString>* pcursALPN = colALPNList.pcursNew();
        TJanitor<TColCursor<TString>> janCurs(pcursALPN);
        for (; pcursALPN->bIsValid(); pcursALPN->bNext())
            m_colALPNList.objAdd(pcursALPN->objRCur());
    }
}

TCIDSChanClDataSrc::~TCIDSChanClDataSrc()
{
    // Our data source will clean up the socket if we adopted it
    try
    {
        delete m_pcdsRawData;
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TCIDSChanClDataSrc: Public, inherited methods
// ---------------------------------------------------------------------------

// Pass this on to our internal data source
tCIDLib::TVoid TCIDSChanClDataSrc::AssociateReadEvent(TEvent& evToSet)
{
    m_pcdsRawData->AssociateReadEvent(evToSet);
}


// Just pass it on to our internal data source
tCIDLib::TBoolean TCIDSChanClDataSrc::bConnected() const
{
    return m_pcdsRawData->bConnected();
}


// Just pass on to our internal data source
tCIDSock::ESockEvs TCIDSChanClDataSrc::eEnumEvents(TEvent& evReset)
{
    return m_pcdsRawData->eEnumEvents(evReset);
}


// Just pass it on to the internal data source
tCIDLib::TVoid TCIDSChanClDataSrc::FormatSrcInfo(TString& strToFill)
{
    m_pcdsRawData->FormatSrcInfo(strToFill);
}




// ---------------------------------------------------------------------------
//  TCIDSChanClDataSrc: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We call the secure channel to read data and pass it our internal data source to
//  get data from. We tell it it doesn't have to real all data. The base data
//  source class call us is handling accumulating data. We jut return what we can
//  get each time.
//
tCIDLib::TCard4
TCIDSChanClDataSrc::c4GetData(          TMemBuf&                mbufTar
                                , const tCIDLib::TCard4         c4MaxBytes
                                , const tCIDLib::TEncodedTime   enctEnd)
{
    return m_schanSec.c4ReadData
    (
        *m_pcdsRawData, mbufTar, c4MaxBytes, enctEnd, tCIDLib::EAllData::OkIfNotAll
    );
}


//
//  This is called when the user flushes data. We just pass it on to the scecure
//  channel, passing our internal data source as the target.
//
tCIDLib::TVoid
TCIDSChanClDataSrc::PutData(const   TMemBuf&                mbufSrc
                            , const tCIDLib::TCard4         c4ToSend
                            , const tCIDLib::TEncodedTime   enctEnd)
{
    m_schanSec.WriteData(*m_pcdsRawData, mbufSrc, c4ToSend);
}


//
//  We override this so that we can set up the secure channel. This must be called
//  on any newly created data source before it is used, so it works for our needs
//  to do the key exchange before any comm happens.
//
tCIDLib::TVoid TCIDSChanClDataSrc::SetupSrc(const tCIDLib::TEncodedTime enctEnd)
{
    //
    //  Recursively initalize our own data source first. If it's already initialized
    //  this will throw.
    //
    m_pcdsRawData->Initialize(enctEnd);

    //
    //  We have to use the secure channel to set up the encrypted connection. We
    //  indicate client side and pass the principal as the certificate info.
    //
    m_schanSec.Connect
    (
        *m_pcdsRawData
        , kCIDLib::True
        , enctEnd
        , m_strPrincipal
        , L"Secure Client Data Source"
        , m_colALPNList
    );
}


//
//  We override this so that we can clean up the secure channel session. We pass
//  the close flag to the underlying raw source to close if it the caller wants to.
//  This would allow it to be re-connected (using a class specific method) and then
//  we could be reinitialized. Otherwise, the underlying connection stays active and
//  we could be reinitialized again on the same connection.
//
tCIDLib::TVoid
TCIDSChanClDataSrc::TerminateSrc(const  tCIDLib::TEncodedTime   enctEnd
                                , const tCIDLib::TBoolean       bClose)
{
    // Terminate the secure channel
    try
    {
        m_schanSec.Terminate(*m_pcdsRawData, enctEnd);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    // And our nested data source, optionally closing it
    try
    {
        m_pcdsRawData->Terminate(enctEnd, bClose);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDSChanSrvDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDSChanSrvDataSrc: Constructors and Destructor
// ---------------------------------------------------------------------------

TCIDSChanSrvDataSrc::
TCIDSChanSrvDataSrc(        TServerStreamSocket* const  psockSrc
                    , const tCIDLib::EAdoptOpts         eAdopt
                    , const TString&                    strCertInfo) :

    m_pcdsRawData(new TCIDSockStreamDataSrc(psockSrc, eAdopt))
    , m_strCertInfo(strCertInfo)
{
}

TCIDSChanSrvDataSrc::~TCIDSChanSrvDataSrc()
{
    // Our data source will clean up the socket if we adopted it
    try
    {
        delete m_pcdsRawData;
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TCIDSChanSrvDataSrc: Public, inherited methods
// ---------------------------------------------------------------------------

// Pass this on to our internal data source
tCIDLib::TVoid TCIDSChanSrvDataSrc::AssociateReadEvent(TEvent& evToSet)
{
    m_pcdsRawData->AssociateReadEvent(evToSet);
}


// Just pass it on to our internal data source
tCIDLib::TBoolean TCIDSChanSrvDataSrc::bConnected() const
{
    return m_pcdsRawData->bConnected();
}


// Just pass on to our internal data source
tCIDSock::ESockEvs TCIDSChanSrvDataSrc::eEnumEvents(TEvent& evReset)
{
    return m_pcdsRawData->eEnumEvents(evReset);
}


// Just pass it on to the internal data source
tCIDLib::TVoid TCIDSChanSrvDataSrc::FormatSrcInfo(TString& strToFill)
{
    m_pcdsRawData->FormatSrcInfo(strToFill);
}



// ---------------------------------------------------------------------------
//  TCIDSChanSrvDataSrc: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We call the secure channel to read data and pass it our internal data source to
//  get data from. We tell it it doesn't have to real all data. The base data
//  source class call us is handling accumulating data. We jut return what we can
//  get each time.
//
tCIDLib::TCard4
TCIDSChanSrvDataSrc::c4GetData(         TMemBuf&                mbufTar
                                , const tCIDLib::TCard4         c4MaxBytes
                                , const tCIDLib::TEncodedTime   enctEnd)
{
    return m_schanSec.c4ReadData
    (
        *m_pcdsRawData, mbufTar, c4MaxBytes, enctEnd, tCIDLib::EAllData::OkIfNotAll
    );
}


//
//  This is called when the user flushes data. We just pass it on to the scecure
//  channel, passing our internal data source as the target.
//
tCIDLib::TVoid
TCIDSChanSrvDataSrc::PutData(const  TMemBuf&                mbufSrc
                            , const tCIDLib::TCard4         c4ToSend
                            , const tCIDLib::TEncodedTime   enctEnd)
{
    m_schanSec.WriteData(*m_pcdsRawData, mbufSrc, c4ToSend);
}


//
//  We override this so that we can set up the secure channel. This must be called
//  on any newly created data source before it is used, so it works for our needs
//  to do the key exchange before any comm happens.
//
tCIDLib::TVoid TCIDSChanSrvDataSrc::SetupSrc(const tCIDLib::TEncodedTime enctEnd)
{
    // Recursively initalize our own data source first
    m_pcdsRawData->Initialize(enctEnd);

    // The ALPN list is not used on the server side, so pass an empty list
    tCIDLib::TStrList colALPN;
    m_schanSec.Connect
    (
        *m_pcdsRawData
        , kCIDLib::False
        , enctEnd
        , m_strCertInfo
        , L"Secure Server Data Source"
        , colALPN
    );
}


//
//  We override this so that we can clean up the secure channel session. We pass
//  the close flag to the underlying raw source to close if it the caller wants to.
//  This would allow it to be re-connected (using a class specific method) and then
//  we could be reinitialized. Otherwise, the underlying connection stays active and
//  we could be reinitialized again on the same connection.
//
tCIDLib::TVoid
TCIDSChanSrvDataSrc::TerminateSrc(  const   tCIDLib::TEncodedTime   enctEnd
                                    , const tCIDLib::TBoolean       bClose)
{
    // Terminate the secure channel
    try
    {
        m_schanSec.Terminate(*m_pcdsRawData, enctEnd);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    // And our nested data source
    try
    {
        m_pcdsRawData->Terminate(enctEnd, bClose);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}

