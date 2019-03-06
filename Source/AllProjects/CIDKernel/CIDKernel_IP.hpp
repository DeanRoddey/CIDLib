//
// FILE NAME: CIDKernel_IP.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/30/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_IP.Cpp file, which implements
//  the TKrnlIP namespace. This namespace provides access to core IP related
//  functionality such as name resolution, querying the available adaptors,
//  getting local host name info, etc...
//
//  We also define a simple class for storing IP addresses. This makes it
//  vastly easier to deal with IPV4 and IPV6 in the same program. It can
//  store either type, and the higher level CIDSock equivalents includes
//  one of these as its means of storing the address as well, so it can
//  just be passed down into kernel socket calls without any conversion.
//
//  The address data is always stored in network order, and we just treat it
//  as a magic cookie and never really bother to interpret it, other than
//  for display purposes, where we have to be careful to format the values
//  in the right order, or when parsing out a formatted one. But neither of
//  those things are done at this level.
//
// CAVEATS/GOTCHAS:
//
//  1)  Don't directly use the address resolution method here. Use the
//      ones at the CIDSock level, even within the CIDSock facility, because
//      caching is done at that level and if you do it through here you
//      won't get the caching benefits.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TKrnlIPAddr
//  PREFIX: kipa
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlIPAddr
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlIPAddr();

        TKrnlIPAddr
        (
            const   tCIDSock::ESpecAddrs    eAddr
            , const tCIDSock::EAddrTypes    eType
        );

        TKrnlIPAddr
        (
            const   TKrnlIPAddr&            kipaToCopy
        );

        ~TKrnlIPAddr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TKrnlIPAddr&            kipaSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKrnlIPAddr&            kipaSrc
        )   const;

        TKrnlIPAddr& operator=
        (
            const   TKrnlIPAddr&            kipaSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsLoopback() const;

        tCIDLib::TBoolean bSet
        (
            const   tCIDLib::TVoid* const   pAddrInfo
            , const tCIDLib::TCard4         c4Count
            , const tCIDSock::EAddrTypes    eType
            , const tCIDLib::TCard4         c4ScopeId
            , const tCIDLib::TCard4         c4FlowInfo
        );

        tCIDLib::TCard4 c4Count() const;

        tCIDLib::TCard4 c4FlowInfo() const;

        tCIDLib::TCard4 c4ScopeId() const;

        tCIDSock::EAddrTypes eType() const;

        const tCIDLib::TCard1* pc1Data() const;

        tCIDLib::TVoid Reset();




        // These are platform dependent
        tCIDLib::TBoolean bFromSockAddr
        (
            const   tCIDLib::TVoid* const   pAddr
            , const tCIDLib::TCard4         c4Len
            ,       tCIDLib::TIPPortNum&    ippnToFill
        );

        tCIDLib::TBoolean bToInAddr
        (
                    tCIDLib::TVoid* const   pAddr
            ,       tCIDLib::TCard4&        c4SzInOut
        )   const;

        tCIDLib::TBoolean bToSockAddr
        (
                    tCIDLib::TVoid* const   pAddr
            ,       tCIDLib::TCard4&        c4SzInOut
            , const tCIDLib::TIPPortNum     ippnToSet = 0
        )   const;

        tCIDLib::TVoid SetToSpecial
        (
            const   tCIDSock::ESpecAddrs    eAddr
            , const tCIDSock::EAddrTypes    eType
        );


    private :
        // -------------------------------------------------------------------
        //  Private class members
        //
        //  m_ac1Data
        //      We store the data here in network order. We never interpret
        //      it at all, other than for formatting which is not done at
        //      this level.
        //
        //  m_c4Count
        //      The number of bytes in the address data. The m_eType value
        //      really implies this, but it's a lot more efficient to just
        //      store it.
        //
        //  m_c4FlowInfo
        //      If an IPV6 address, this is the flow info value.
        //
        //  m_c4ScopeId
        //      If an IPV6 address, this is the scope id, which is used to
        //      differentiate the same link local address assigned to multiple
        //      adaptors. If not used it's zero.
        //
        //  m_eType
        //      The address type we contain. If we default construct, it's
        //      set to IPV4, ANY address.
        // -------------------------------------------------------------------
        tCIDLib::TCard1         m_ac1Data[kCIDSock::c4MaxIPAddrBytes];
        tCIDLib::TCard4         m_c4Count;
        tCIDLib::TCard4         m_c4FlowInfo;
        tCIDLib::TCard4         m_c4ScopeId;
        tCIDSock::EAddrTypes    m_eType;
};


namespace TKrnlIP
{
    //
    //  This is used to return adaptor info lists. They should call
    //  c4GetAdaptorCount(), allocate a buffer of that many of these
    //  structures, then call QueryAdaptorInfo().
    //
    struct TAdaptorInfo
    {
        tCIDLib::TCh        szName[kCIDSock::c4MaxAdapterNameLn + 1];
        tCIDLib::TCh        szDescr[kCIDSock::c4MaxAdapterDescLn + 1];
        tCIDLib::TCard1     ac1HWAddr[kCIDSock::c4MaxHWAddrSz];
        tCIDLib::TCard4     c4HWAddrSz;
        tCIDLib::TBoolean   bDHCPEnabled;
        tCIDLib::TBoolean   bDedicated;
        tCIDLib::TBoolean   bLoopback;
        tCIDLib::TBoolean   bIPV4Enabled;
        tCIDLib::TBoolean   bIPV6Enabled;
        tCIDLib::TBoolean   bReady;
        tCIDLib::TBoolean   bTunnel;
        TKrnlIPAddr         akipaAddrs[kCIDSock::c4MaxAddrs];
        tCIDLib::TCard4     c4AddrCnt;
        TKrnlIPAddr         kipaDNS1;
    };

    KRNLEXPORT tCIDLib::TBoolean bAddToFirewall
    (
        const   tCIDLib::TCh* const     pszAppPath
        , const tCIDLib::TCh* const     pszAppName
        ,       tCIDLib::TCh* const     pszErrText
        , const tCIDLib::TCard4         c4MaxErrChars
    );

    KRNLEXPORT tCIDLib::TBoolean bIPV4Avail();

    KRNLEXPORT tCIDLib::TBoolean bIPV6Avail();

    KRNLEXPORT tCIDLib::TBoolean bLoadTCPIPMsg
    (
        const   tCIDLib::TOSErrCode     errcId
        ,       tCIDLib::TCh* const     pszBuffer
        , const tCIDLib::TCard4         c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryAdaptorInfo
    (
        const   tCIDLib::TCh* const     pszName
        ,       TAdaptorInfo&           kadpToFill
        ,       tCIDLib::TBoolean&      bFound
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryAdaptorList
    (
                TKrnlLList<TAdaptorInfo>& kllstToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryDefLocalAddr
    (
                TKrnlIPAddr&            kipaToFill
        , const tCIDSock::EAddrTypes    eType
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryHostAddrs
    (
        const   tCIDLib::TCh* const     pszHostName
        ,       TKrnlLList<TKrnlIPAddr>& kllstAddrs
        , const tCIDSock::EAddrInfoFlags eFlags
        , const tCIDLib::TBoolean       bIncludeLoopback
        , const tCIDLib::TBoolean       bAppend = kCIDLib::False
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryLocalName
    (
                TKrnlString&            kstrToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryNameByAddr
    (
        const   TKrnlIPAddr&            kipaToQuery
        ,       TKrnlString&            kstrToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryTCPVersion
    (
                tCIDLib::TCard4&        c4MajVersion
        ,       tCIDLib::TCard4&        c4MinVersion
    );

    KRNLEXPORT tCIDLib::TBoolean bTextFromIPAddr
    (
        const   TKrnlIPAddr&            kipaToConvert
        ,       TKrnlString&            kstrToFill
    );

    KRNLEXPORT tCIDLib::TCard4 c4XlatError
    (
        const   tCIDLib::TSInt          iToXlat
    );

    KRNLEXPORT tCIDSock::EAddrTypes eDefAddrType();

    KRNLEXPORT tCIDSock::EAddrCvtRes eIPAFromText
    (
        const   tCIDLib::TCh* const     pszIPAddrString
        ,       TKrnlIPAddr&            kipaToFill
        , const tCIDLib::TBoolean       bDoLookup
        , const tCIDSock::EAddrTypes    eReqAddr = tCIDSock::EAddrTypes::Unspec
    );

    KRNLEXPORT tCIDLib::TVoid QueryDefAllListen
    (
                TKrnlIPAddr&            kipaToFill
    );
}

