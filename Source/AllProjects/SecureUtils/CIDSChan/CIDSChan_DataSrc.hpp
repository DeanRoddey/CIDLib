//
// FILE NAME: CIDSChan_DataSrc.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/22/2014
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
//  This is the header for the CIDSChan_DataSrc.cpp file. This file implements
//  a derivative of the lower level abstract data source class. In this case it
//  implements a couple derivatives that work in terms of a secure channel. We have
//  one for server side and one for client side, since they are pretty different
//  in how they set up.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that it's a bit wierd because the secure channel works in terms of a
//      data source as well, but it can't be us because we expect to read/write
//      decrypted data, whereas the secure channel needs to get to the raw encrypted
//      data.
//
//      So we create a staight socket based data channel as a member and give it the
//      socket, and this is what we pass to the secure channel to read/write.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDSChanClDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------
class CIDSCHANEXP TCIDSChanClDataSrc : public TCIDSockStreamBasedDataSrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDSChanClDataSrc
        (
            const   TString&                strName
            ,       TClientStreamSocket* const psockSrv
            , const tCIDLib::EAdoptOpts     eAdopt
            , const TString&                strCertInfo
            , const tCIDLib::TStrCollect&   colALPNList
            , const tCIDSChan::EConnOpts    eOpts = tCIDSChan::EConnOpts::None
            , const TString&                strPrincipal = TString::strEmpty()
        );

        TCIDSChanClDataSrc
        (
            const   TString&                strName
            , const TIPEndPoint&            ipepTar
            , const tCIDSock::ESockProtos   eProtocol
            , const TString&                strCertInfo
            , const tCIDLib::TStrCollect&   colALPNList
            , const tCIDSChan::EConnOpts    eOpts = tCIDSChan::EConnOpts::None
            , const TString&                strPrincipal = TString::strEmpty()
        );

        TCIDSChanClDataSrc(const TCIDSChanClDataSrc&) = delete;

        ~TCIDSChanClDataSrc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDSChanClDataSrc& operator=(const TCIDSChanClDataSrc&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AssociateReadEvent
        (
                    TEvent&                 evToSet
        )   override;

        tCIDLib::TBoolean bIsConnected() const override;

        tCIDSock::ESockEvs eEnumEvents
        (
                    TEvent&                 evReset
        )   override;

        tCIDLib::TVoid FormatSrcInfo
        (
                    TString&                strToFill
        )   override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GetData
        (
                    TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctEnd
        )   override;

        tCIDLib::TVoid PutData
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4ToSend
            , const tCIDLib::TEncodedTime   enctEnd
        )   override;

        tCIDLib::TVoid SetupSrc
        (
            const   tCIDLib::TEncodedTime   enctEnd
        )   override;

        tCIDLib::TVoid TerminateSrc
        (
            const   tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bClose
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colALPNList
        //      The client code can provide a list of preferred protocols that will
        //      be used in the 'application layer protocol negotiation' phase of the
        //      secure connection process.
        //
        //  m_eOpts
        //      We have to save this away to pass to the channel object when we
        //      initialize it.
        //
        //  m_pcdsRawData
        //      A straight data source over socket that we point at the socket we
        //      are given. This is what we pass to the secure channel for I/O. We
        //      pass along to it the adoption flag we are given as well.
        //
        //      It's a pointer so that we don't have to impose the socket headers
        //      on downstream code just for this.
        //
        //  m_schanSec
        //      The secure channel we will use to process the communications.
        //
        //  m_strCertInfo
        //      If they want a client side certificate, then this will be set to the
        //      certificate info, else empty.
        //
        //  m_strPrincipal
        //      This is the target we are connecting to. The certificate we get from
        //      the server must match this.
        //
        //  m_strName
        //      A name that can be given to us for use in logging and such. We just
        //      set it on the underlying secure channel.
        // -------------------------------------------------------------------
        tCIDLib::TStrList       m_colALPNList;
        tCIDSChan::EConnOpts    m_eOpts;
        TCIDSockStreamDataSrc*  m_pcdsRawData;
        TSChannel               m_schanSec;
        TString                 m_strCertInfo;
        TString                 m_strPrincipal;
        TString                 m_strName;
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDSChanSrvDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------
class CIDSCHANEXP TCIDSChanSrvDataSrc : public TCIDSockStreamBasedDataSrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDSChanSrvDataSrc
        (
            const   TString&                strName
            ,       TServerStreamSocket* const psockSrv
            , const tCIDLib::EAdoptOpts     eAdopt
            , const TString&                strCertInfo
            , const tCIDSChan::EConnOpts    eOpts = tCIDSChan::EConnOpts::None
        );

        TCIDSChanSrvDataSrc(const TCIDSChanSrvDataSrc&) = delete;

        ~TCIDSChanSrvDataSrc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDSChanSrvDataSrc& operator=(const TCIDSChanSrvDataSrc&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AssociateReadEvent
        (
                    TEvent&                 evToSet
        )   override;

        tCIDLib::TBoolean bIsConnected() const override;

        tCIDSock::ESockEvs eEnumEvents
        (
                    TEvent&                 evReset
        )   override;

        tCIDLib::TVoid FormatSrcInfo
        (
                    TString&                strToFill
        )   override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GetData
        (
                    TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctEnd
        )   override;

        tCIDLib::TVoid PutData
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4ToSend
            , const tCIDLib::TEncodedTime   enctEnd
        )   override;

        tCIDLib::TVoid SetupSrc
        (
            const   tCIDLib::TEncodedTime   enctEnd
        )   override;

        tCIDLib::TVoid TerminateSrc
        (
            const   tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bClose
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eOpts
        //      We have to save this away to pass to the channel object when we
        //      initialize it.
        //
        //  m_pcdsRawData
        //      A straight data source over socket that we point at the socket we
        //      are given. This is what we pass to the secure channel for I/O. We
        //      pass along to it the adoption flag we are given as well.
        //
        //  m_schanSec
        //      The secure channel we will use to process the communications.
        //
        //  m_strCertInfo
        //      The caller provides us with a certificate info string that we pass
        //      on to the secure channel, to tell him where to find the certificate
        //      that we want to use as our server side cert.
        //
        //  m_strName
        //      Client code can give us a name to be used in logged errors and such.
        // -------------------------------------------------------------------
        tCIDSChan::EConnOpts    m_eOpts;
        TCIDSockStreamDataSrc*  m_pcdsRawData;
        TSChannel               m_schanSec;
        TString                 m_strCertInfo;
        TString                 m_strName;
};


#pragma CIDLIB_POPPACK




