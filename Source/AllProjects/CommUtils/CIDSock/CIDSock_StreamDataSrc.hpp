//
// FILE NAME: CIDSock_StreamDataSrc.hpp
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
//  This is the header for the CIDSock_StreamDataSrc.cpp file. This file implements
//  a derivative of the lower level abstract data source class. In this case it
//  implements one over a stream socket.
//
//  It also implements an intermediate abstract class of a data source over a socket,
//  so that we can (and need to in various places) deal abstractly with data sources
//  that have a stream socket at their base.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDSockStreamBasedDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------
class CIDSOCKEXP TCIDSockStreamBasedDataSrc : public TCIDDataSrc
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDSockStreamBasedDataSrc(const TCIDSockStreamBasedDataSrc&) = delete;

        ~TCIDSockStreamBasedDataSrc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDSockStreamBasedDataSrc& operator=(const TCIDSockStreamBasedDataSrc&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid AssociateReadEvent
        (
                    TEvent&                 evToSet
        ) = 0;

        virtual tCIDSock::ESockEvs eEnumEvents
        (
                    TEvent&                 evReset
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCIDSockStreamBasedDataSrc();
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDSockStreamDataSrc
//  PREFIX: cds
// ---------------------------------------------------------------------------
class CIDSOCKEXP TCIDSockStreamDataSrc : public TCIDSockStreamBasedDataSrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDSockStreamDataSrc() = delete;

        TCIDSockStreamDataSrc
        (
                    TStreamSocket* const    psockSrc
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TCIDSockStreamDataSrc
        (
            const   TIPEndPoint&            ipepTar
        );

        TCIDSockStreamDataSrc(const TCIDSockStreamDataSrc&) = delete;

        ~TCIDSockStreamDataSrc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDSockStreamDataSrc& operator=(const TCIDSockStreamDataSrc&) = delete;


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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Connect
        (
            const   TIPEndPoint&            ipepTar
        );


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
        //  m_eAdopt
        //      Remember whether we own the socket or not.
        //
        //  m_psockSrc
        //      The stream socket we spool from. We can adopt it or not.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts     m_eAdopt;
        TStreamSocket*          m_psockSrc;
};

#pragma CIDLIB_POPPACK



