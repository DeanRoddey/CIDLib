//
// FILE NAME: CIDSChan_DataSrc.hpp
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
                    TClientStreamSocket* const psockSrv
            , const tCIDLib::EAdoptOpts     eAdopt
            , const TString&                strPrincipal
        );

        TCIDSChanClDataSrc
        (
            const   TIPEndPoint&            ipepTar
            , const tCIDSock::ESockProtos   eProtocol
            , const TString&                strPrincipal
            , const tCIDLib::TStrCollect&   colALPNList
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

        tCIDLib::TBoolean bConnected() const override;

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
        //  m_strPrincipal
        //      This is the target we are connecting to. The certificate we get from
        //      the server must match this.
        // -------------------------------------------------------------------
        tCIDLib::TStrList       m_colALPNList;
        TCIDSockStreamDataSrc*  m_pcdsRawData;
        TSChannel               m_schanSec;
        TString                 m_strPrincipal;
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
                    TServerStreamSocket* const psockSrv
            , const tCIDLib::EAdoptOpts     eAdopt
            , const TString&                strCertInfo
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

        tCIDLib::TBoolean bConnected() const override;

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
        // -------------------------------------------------------------------
        TCIDSockStreamDataSrc*  m_pcdsRawData;
        TSChannel               m_schanSec;
        TString                 m_strCertInfo;
};


#pragma CIDLIB_POPPACK




