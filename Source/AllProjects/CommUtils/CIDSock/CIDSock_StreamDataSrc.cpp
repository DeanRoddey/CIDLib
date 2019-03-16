//
// FILE NAME: CIDSock_StreamDataSrc.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/2014
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
//  This file implements a data source derivative that works over a stream socket.
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
//   CLASS: TCIDSockStreamBasedDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDSockStreamBasedDataSrc: Destructor
// ---------------------------------------------------------------------------
TCIDSockStreamBasedDataSrc::~TCIDSockStreamBasedDataSrc()
{
}


// ---------------------------------------------------------------------------
//  TCIDSockStreamBasedDataSrc: Hidden constructors
// ---------------------------------------------------------------------------
TCIDSockStreamBasedDataSrc::TCIDSockStreamBasedDataSrc()
{
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDSockStreamDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDSockStreamDataSrc: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDSockStreamDataSrc::
TCIDSockStreamDataSrc(          TStreamSocket* const    psockSrc
                        , const tCIDLib::EAdoptOpts     eAdopt) :

    m_eAdopt(eAdopt)
    , m_psockSrc(psockSrc)
{
}

TCIDSockStreamDataSrc:: TCIDSockStreamDataSrc(const TIPEndPoint& ipepTar) :

    m_eAdopt(tCIDLib::EAdoptOpts::Adopt)
    , m_psockSrc(nullptr)
{
    m_psockSrc = new TClientStreamSocket(tCIDSock::ESockProtos::TCP, ipepTar);
}

TCIDSockStreamDataSrc::~TCIDSockStreamDataSrc()
{
    // If we own the socket, clean it up
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
    {
        try
        {
            delete m_psockSrc;
        }

        catch(...)
        {
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDSockStreamDataSrc: Public, inherited methods
// ---------------------------------------------------------------------------

// Associate the passed event with our socket for reads
tCIDLib::TVoid TCIDSockStreamDataSrc::AssociateReadEvent(TEvent& evToSet)
{
    m_psockSrc->AssociateReadEvent(evToSet);
}


// Return whether our socket is connected
tCIDLib::TBoolean TCIDSockStreamDataSrc::bConnected() const
{
    return m_psockSrc->bIsConnected();
}


//
//  Get any events associated with the passed event, and it will correctly reset the
//  event.
//
tCIDSock::ESockEvs TCIDSockStreamDataSrc::eEnumEvents(TEvent& evReset)
{
    return m_psockSrc->eEnumEvents(evReset);
}


//
//  Since we can support various types of sources, this allows info about the source
//  to be queried generically. In our case we format out the remote end pointer.
//
tCIDLib::TVoid TCIDSockStreamDataSrc::FormatSrcInfo(TString& strToFill)
{
    m_psockSrc->ipepRemoteEndPoint().FormatPlain(strToFill);
}




// ---------------------------------------------------------------------------
//  TCIDSockStreamDataSrc: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If the socket is already connected, this will throw. Use bConnected above to
//  see if it needs to be reconnected. If so, call Terminate on this source, with
//  the close option being true. Then you can call this again to reconnect, then
//  re-initialize this source.
//
tCIDLib::TVoid TCIDSockStreamDataSrc::Connect(const TIPEndPoint& ipepTar)
{
    m_psockSrc->Connect(ipepTar);
}



// ---------------------------------------------------------------------------
//  TCIDSockStreamDataSrc: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4
TCIDSockStreamDataSrc::c4GetData(       TMemBuf&                mbufTar
                                , const tCIDLib::TCard4         c4MaxBytes
                                , const tCIDLib::TEncodedTime   enctEnd)
{
    //
    //  If the end time is zero, then just do a check for anything already present.
    //  Else, wait for up to the end time for something to show up.
    //
    tCIDLib::TEncodedTime enctWait = 0;
    if (enctEnd)
    {
        tCIDLib::TEncodedTime enctCur = TTime::enctNow();
        if (enctCur >= enctEnd)
            return 0;
        enctWait = enctEnd - enctCur;
    }

    //
    //  Wait up to the max time for any data to be available. If some does, just return
    //  what is available now.
    //
    tCIDLib::TCard4 c4Ret = 0;
    if (m_psockSrc->bWaitForDataReady(enctWait))
        c4Ret = m_psockSrc->c4ReceiveMBuf(mbufTar, c4MaxBytes);

    return c4Ret;
}


tCIDLib::TVoid
TCIDSockStreamDataSrc::PutData( const   TMemBuf&                mbufSrc
                                , const tCIDLib::TCard4         c4ToSend
                                , const tCIDLib::TEncodedTime   enctEnd)
{
    tCIDLib::TEncodedTime enctNow = TTime::enctNow();
    if (enctNow >= enctEnd)
        return;

    //
    //  Just pass it along, it will fail if it can't write all bytes. We do a throttled
    //  send.
    //
    m_psockSrc->SendThrottled(mbufSrc, enctEnd - enctNow, c4ToSend);
}


//
//  We don't have any particular setup concerns. That is more for for secure data
//  sources to do their dance.
//
tCIDLib::TVoid TCIDSockStreamDataSrc::SetupSrc(const tCIDLib::TEncodedTime)
{
}


//
//  We only need to close the socket if the close option is set on the terminate
//  operation.
//
tCIDLib::TVoid
TCIDSockStreamDataSrc::TerminateSrc(const   tCIDLib::TEncodedTime
                                    , const tCIDLib::TBoolean       bClose)
{
    if (bClose)
        m_psockSrc->Close();
}

