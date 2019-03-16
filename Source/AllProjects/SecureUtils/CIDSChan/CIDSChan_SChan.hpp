//
// FILE NAME: CIDCrypto_SChan.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2014
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
//  This is the header for the CIDCrypto_SChan.cpp file. This file implements
//  support for a secure channel connection (generally TLS 1.0, since that's the
//  best combination for most secure and widest implementation.
//
//  The caller manages his own communcations, and just uses this class to set up,
//  manage, and shut down the session, and read/write data.
//
//  We make it easier on the caller by decrypting internally and caching the data
//  and spooling it out to the caller in chunks he asks for. Otherwise, the caller
//  would have to deal with the fact that incoming data bears no relationship (size-
//  wise) to the data he is expecting. This way, he can treat it reasonably like
//  a regular socket communication.
//
//
//  This method has a per-platform implementation file since this is something that
//  so far has been too complex to implement ourselves. So create the correct
//  platform directory and implement the per-platform specific methods. See the Win32
//  one as an example.
//
// LOG:
//
// CAVEATS:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

struct TSChanPlatData;


// ---------------------------------------------------------------------------
//   CLASS: TSChannel
//  PREFIX: schan
// ---------------------------------------------------------------------------
class CIDSCHANEXP TSChannel : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSChannel();

        TSChannel(const TSChannel&) = delete;

        ~TSChannel();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSChannel& operator=(const TSChannel&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadData
        (
                    TCIDDataSrc&            cdsSrc
            ,       TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4ReqBytes
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReadRawData
        (
                    TCIDDataSrc&            cdsSrc
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4ReqBytes
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReadDataMS
        (
                    TCIDDataSrc&            cdsSrc
            ,       TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4ReqBytes
            , const tCIDLib::TCard4         c4ToWait
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TCard4 c4ReadRawDataMS
        (
                    TCIDDataSrc&            cdsSrc
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4ReqBytes
            , const tCIDLib::TCard4         c4ToWait
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TVoid ClConnect
        (
            const   TString&                strName
            ,       TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strCertInfo
            , const tCIDLib::TStrCollect&   colALPNList
            , const tCIDSChan::EConnOpts    eOpts = tCIDSChan::EConnOpts::None
            , const TString&                strSecPrincipal = TString::strEmpty()
        );

        tCIDLib::TVoid SrvConnect
        (
            const   TString&                strName
            ,       TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strCertInfo
            , const tCIDSChan::EConnOpts    eOpts = tCIDSChan::EConnOpts::None
        );

        tCIDLib::TVoid WriteData
        (
                    TCIDDataSrc&            cdsTar
            , const TMemBuf&                mbufToSend
            , const tCIDLib::TCard4         c4SendBytes
        );

        tCIDLib::TVoid WriteData
        (
                    TCIDDataSrc&            cdsTar
            , const tCIDLib::TCard1* const  pc1ToSend
            , const tCIDLib::TCard4         c4SendBytes
        );

        tCIDLib::TVoid Terminate
        (
                    TCIDDataSrc&            cdsTar
            , const tCIDLib::TEncodedTime   enctEnd
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReceiveData
        (
                    TCIDDataSrc&            cdsSrc
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4ReqBytes
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::FailIfNotAll
        );

        tCIDLib::TVoid Cleanup();

        tCIDLib::TVoid ClNegotiate
        (
                    TCIDDataSrc&            cdsTar
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid DoConnect
        (
                    TCIDDataSrc&            cdsSrc
            , const TString&                strCertInfo
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid DoDisconnect
        (
                    TCIDDataSrc&            cdsTar
        );

        tCIDLib::TVoid SrvNegotiate
        (
                    TCIDDataSrc&            cdsTar
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid TransmitData
        (
                    TCIDDataSrc&            cdsTar
            , const tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4SendBytes
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bClient
        //      Indicates whether we were set up for client or server side. This may
        //      affect how some stuff is done. The per-platform code should set this
        //      appropriately upon successful connection, and will often use it
        //      internally to do things correctly depending on which side of the
        //      connection it is handling.
        //
        //  m_c4DecBufSz
        //      Some platforms may not be able to control the amount of data exactly
        //      due to block size issues with encryption.) So we provide a decrypt
        //      buffer that they can use to decrypt into, and then spool out of until
        //      they run out and need to decrypt more. If a given platform doesn't
        //      need this, then they can ignore it and leave it zero.
        //
        //  m_colALPNList
        //      The client code can provide a list of preferred protocols that will
        //      be used in the 'application layer protocol negotiation' phase of the
        //      secure connection process. We have to keep it around in case we need
        //      to re-negotiate.
        //
        //  m_eOpts
        //      A set of option flags to enable/disable this or that, mostly for future
        //      flexibility.
        //
        //  m_mbufDecBuf
        //      A temp buffer to decrypt into and to hold into decyrpted data that
        //      is too much for our caller's needs on the current round. See
        //      m_c4DecBufSz above.
        //
        //  m_pInfo
        //      This is an opaque per-platform structure. Each platform implementation
        //      defines this internally for his own needs. The presence of this structure
        //      (non-null) should be used as an indicator that there is an active session.
        //      Cleanup() should release any OS and memory resources it holds, then
        //      delete the structure and null this pointer to indicate no active session.
        //
        //  m_strName
        //      For debugging/logging purposes, the caller gives a name to the
        //      channel. It doesn't have to be unique, though that could be useful.
        //
        //  m_strPrincipal
        //      For client side connections, the per-platform stuff should store the
        //      cert info here after a successful session establishment and clear it
        //      when Cleanup() is called.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bClient;
        tCIDLib::TCard4         m_c4DecBufSz;
        tCIDLib::TStrList       m_colALPNList;
        tCIDSChan::EConnOpts    m_eOpts;
        THeapBuf                m_mbufDecBuf;
        TSChanPlatData*         m_pInfo;
        TString                 m_strName;
        TString                 m_strPrincipal;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSChannel,TObject)
};

#pragma CIDLIB_POPPACK



