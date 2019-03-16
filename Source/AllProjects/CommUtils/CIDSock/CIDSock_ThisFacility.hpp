//
// FILE NAME: CIDSock_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/06/1998
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
//  This is the header file for the CIDSock_ThisFacility.cpp file. This
//  file implements the facility class for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TFacCIDSock;


// ---------------------------------------------------------------------------
//  CLASS: THostAdapter
// PREFIX: hadp
// ---------------------------------------------------------------------------
class CIDSOCKEXP THostAdapter : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THostAdapter();

        THostAdapter
        (
            const   THostAdapter&           hadpSrc
        );

        ~THostAdapter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THostAdapter& operator=
        (
            const   THostAdapter&           hadpSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDHCPEnabled() const;

        tCIDLib::TBoolean bIPV4Enabled() const;

        tCIDLib::TBoolean bIPV6Enabled() const;

        tCIDLib::TBoolean bIsLoopback() const;

        tCIDLib::TBoolean bIsDedicated() const;

        tCIDLib::TBoolean bIsReady() const;

        tCIDLib::TBoolean bIsTunnel() const;

        tCIDLib::TCard1 c1HWAddrByteAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCard4 c4AddrCnt() const;

        tCIDLib::TCard4 c4HWAddrSz() const;

        tCIDLib::TCard4 c4QueryHWAddr
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
        )   const;

        const TIPAddress& ipaAddrAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        const TString& strDescr() const;

        const TString& strName() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TFacCIDSock;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LoadFrom
        (
            const   TKrnlIP::TAdaptorInfo&  kadpSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac1HWAddr
        //      The bytes of the hardware address of the device. m_c4HWAddrSz
        //      indicates how many bytes are valid.
        //
        //  m_aipaAddrs
        //  m_apiaMasks
        //      The IP addresses and masks of all of the end points that this
        //      adapter provides. Usually just one but not always. As usuall
        //      they are in network byte order.
        //
        //  m_bDHCPEnabled
        //      Indicates if the adapter is DHCP enabled.
        //
        //  m_c4AddrCnt
        //      The number of valid items in the addrs/masks list.
        //
        //  m_c4HWAddrSz
        //      The number of valid bytes in the m_ac1HWAddr array.
        //
        //  m_strDescr
        //      The human readable description of this adapter.
        //
        //  m_strName
        //      The unique identifier for this adapter, not for human viewing.
        // -------------------------------------------------------------------
        tCIDLib::TCard1     m_ac1HWAddr[kCIDSock::c4MaxHWAddrSz];
        TIPAddress          m_aipaAddrs[kCIDSock::c4MaxAddrs];
        tCIDLib::TBoolean   m_bDHCPEnabled;
        tCIDLib::TBoolean   m_bIPV4Enabled;
        tCIDLib::TBoolean   m_bIPV6Enabled;
        tCIDLib::TBoolean   m_bIsDedicated;
        tCIDLib::TBoolean   m_bIsLoopback;
        tCIDLib::TBoolean   m_bIsReady;
        tCIDLib::TBoolean   m_bIsTunnel;
        tCIDLib::TCard4     m_c4AddrCnt;
        tCIDLib::TCard4     m_c4HWAddrSz;
        TString             m_strDescr;
        TString             m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(THostAdapter, TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TFacCIDSock
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDSOCKEXP TFacCIDSock : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDSock();

        TFacCIDSock(const TFacCIDSock&) = delete;

        ~TFacCIDSock();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDSock& operator=(const TFacCIDSock&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryAdapterInfo
        (
            const   TString&                strName
            ,       THostAdapter&           hadpToFill
        )   const;

        tCIDLib::TBoolean bResolveAddr
        (
            const   TString&                strAddr
            ,       TIPAddress&             ipaToFill
            , const tCIDSock::EAddrTypes    eType
        )   const;

        tCIDLib::TCard4 c4QueryAdapterList
        (
                    TCollection<THostAdapter>& colToFill
        )   const;

        tCIDSock::EAddrTypes eAvailAddrType() const;

        tCIDSock::EAddrTypes ePrefAddrType() const;

        tCIDSock::EAddrCvtRes eResolveAddr
        (
            const   TString&                strAddr
            ,       TIPAddress&             ipaToFill
            , const tCIDSock::EAddrTypes    eType
        )   const;

        tCIDSock::EAddrCvtRes eResolveAddr
        (
            const   TString&                strAddr
            ,       TKrnlIPAddr&            kipaToFill
            , const tCIDSock::EAddrTypes    eType
        )   const;

        TIPAddress ipaDefLocalAddr
        (
            const   tCIDSock::EAddrTypes    eType = tCIDSock::EAddrTypes::Unspec
        )   const;

        TIPEndPoint ipepDefLocalAddr
        (
            const   tCIDLib::TIPPortNum     ippnToUse
            , const tCIDSock::EAddrTypes    eType = tCIDSock::EAddrTypes::Unspec
        )   const;

        tCIDLib::TVoid QueryAdapterInfo
        (
            const   TString&                strName
            ,       THostAdapter&           hadpToFill
        )   const;

        tCIDLib::TVoid QueryDefAllListenEP
        (
                    TIPEndPoint&            ipepToFill
            , const tCIDLib::TIPPortNum     ippnListenOn
        );

        tCIDLib::TVoid QueryTCPVersion
        (
                    tCIDLib::TCard4&        c4MajVersion
            ,       tCIDLib::TCard4&        c4MinVersion
        );

        const TString& strIPHostName() const;

        TString strIPHostName
        (
            const   TKrnlIPAddr&            kipaToFind
        )   const;

        TString strXlatProto
        (
            const   tCIDSock::ESockProtos   eToXlat
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MajVersion
        //  m_c4MinVersion
        //      The maj/min version of the TCP/IP implementation on the local
        //      host.
        //
        //  m_strDNSName
        //      The local DNS name, which may be different from the host name.
        //      Normally they will be the same, just the NetBIOS level name of this
        //      local host.
        //
        //  m_strHostName
        //      The local host name.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4MajVersion;
        tCIDLib::TCard4     m_c4MinVersion;
        TString             m_strHostName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDSock,TFacility)
};

#pragma CIDLIB_POPPACK
