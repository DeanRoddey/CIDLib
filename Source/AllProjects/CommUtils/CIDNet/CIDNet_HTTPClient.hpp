//
// FILE NAME: CIDNet_HTTPClient.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
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
//  This is the header for the CIDNet_HTTPClient.cpp file. This file implements
//  the THTTPClient class. This class provides assistance for HTTP based client
//  apps to do GET, POST type HTTP operations and handle the formatting and
//  parsing of the data that goes into such operations.
//
//  This is based on a common core parser that HTTP, SIP, and some other
//  formats use because they all share a basic common format derived from
//  HTTP.
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
//   CLASS: THTTPClient
//  PREFIX: httpc
// ---------------------------------------------------------------------------
class CIDNETEXP THTTPClient : public TNetCoreParser
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString    strCC_MaxAge;
        static const TString    strCC_MaxStale;
        static const TString    strCC_SMaxAge;
        static const TString    strCC_MustReval;
        static const TString    strCC_NoCache;
        static const TString    strCC_NoStore;

        static const TString    strHdr_Accept;
        static const TString    strHdr_CacheControl;
        static const TString    strHdr_Connection;
        static const TString    strHdr_ContDisposition;
        static const TString    strHdr_ContLen;
        static const TString    strHdr_ContTransferEncoding;
        static const TString    strHdr_ContType;
        static const TString    strHdr_ETag;
        static const TString    strHdr_Expires;
        static const TString    strHdr_Host;
        static const TString    strHdr_IfModifiedSince;
        static const TString    strHdr_LastModified;
        static const TString    strHdr_Location;
        static const TString    strHdr_Origin;
        static const TString    strHdr_SecWSAccept;
        static const TString    strHdr_SecWSKey;
        static const TString    strHdr_TransferEncoding;
        static const TString    strHdr_TE;
        static const TString    strHdr_Upgrade;
        static const TString    strHdr_WWWAuthenticate;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bBuildAuthStr
        (
            const   tCIDLib::TKVPCollect&   colInHdrLines
            , const TString&                strUserName
            , const TString&                strPassword
            , const TURL&                   urlSrc
            ,       tCIDLib::TCard4&        c4ReqCnt
            ,       TString&                strAuthStr
        );

        static tCIDLib::TVoid BuildErrReply
        (
            const   tCIDLib::TCard4         c4ErrCode
            , const TString&                strErrText
            ,       TTextOutStream&         strmTar
        );

        static tCIDLib::TCard4 c4BuildErrReply
        (
            const   tCIDLib::TCard4         c4ErrCode
            , const TString&                strErrText
            ,       TString&                strEncoding
            ,       TMemBuf&                mbufTar
        );

        static tCIDLib::TCard4 c4DoSSLPageOp
        (
            const   tCIDNet::ESSLOps        eOp
            , const TURL&                   urlToGet
            , const TString&                strAgent
            , const TString&                strAccept
            ,       TString&                strContType
            ,       tCIDLib::TCard4&        c4ResCode
            ,       TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4OutLen = 0
        );

        static tCIDLib::TCard4 c4DoSSLPageOp
        (
            const   tCIDNet::ESSLOps        eOp
            , const TURL&                   urlToGet
            , const TString&                strAgent
            , const TString&                strAccept
            ,       TString&                strContType
            ,       tCIDLib::TCard4&        c4ResCode
            ,       TMemBuf&                mbufData
            , const tCIDLib::TKVPCollect&   colInHdrs
            ,       tCIDLib::TKVPCollect&   colOutHdrs
            , const tCIDLib::TCard4         c4OutLen = 0
        );

        static tCIDLib::TVoid EscapeBodyText
        (
            const   TString&                strText
            ,       TTextOutStream&         strmOut
        );

        static tCIDLib::TVoid EscapeBodyText
        (
            const   TString&                strText
            ,       TString&                strOut
            , const tCIDLib::TBoolean       bAppend
        );

        static tCIDLib::TVoid ExpandBodyText
        (
            const   TString&                strText
            ,       TTextOutStream&         strmOut
        );

        static tCIDLib::TVoid ExpandBodyText
        (
            const   TString&                strText
            ,       TString&                strOut
            , const tCIDLib::TBoolean       bAppend
        );

        static tCIDLib::TVoid MakeBasicAuthStr
        (
            const   TString&                strUserName
            , const TString&                strPassword
            ,       TString&                strHashOut
        );

        static tCIDLib::TVoid ParseAuthReq
        (
            const   TString&                strReqLine
            ,       tCIDNet::EHTTPAuthTypes& eType
            ,       TString&                strRealm
            ,       TString&                strNonce
            ,       TString&                strOpaque
            ,       TString&                strDomain
            ,       TString&                strAlgorithm
        );

        static tCIDLib::TVoid ParseCacheControl
        (
            const   TString&                strSrc
            ,       tCIDLib::TKVPCollect&   colToFill
        );

        static tCIDLib::TVoid SendErrReply
        (
                    TCIDDataSrc&            cdsTar
            , const TString&                strServer
            , const tCIDLib::TCard4         c4ErrCode
            , const TString&                strRepText
            , const TString&                strBodyText
            , const tCIDLib::TBoolean       bCloseFlag = kCIDLib::True
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THTTPClient();

        THTTPClient(const THTTPClient&) = delete;

        ~THTTPClient();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THTTPClient& operator=(const THTTPClient&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDNet::ENetPReadRes eBasicMsgVal
        (
            const   tCIDNet::EReadTypes     eReadType
            , const tCIDLib::TKVPCollect&   colHdrLines
            , const TString&                strFirstLn
            ,       tCIDLib::TBoolean&      bClose
            ,       tCIDLib::TBoolean&      bPersist
            ,       TString&                strContType
            ,       tCIDLib::TCard4&        c4ContLen
            ,       TString&                strParm1
            ,       TString&                strParm2
            ,       TString&                strParm3
        )   final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAutoAuth() const;

        tCIDLib::TBoolean bAutoAuth
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4GetSrvReply
        (
                    TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TBoolean       bNoCont
            ,       TString&                strRepType
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
        );

        tCIDLib::TCard4 c4SendGet
        (
                    TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bOutwardBody
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );

        tCIDLib::TCard4 c4SendPut
        (
                    TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bOutwardBody
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );

        tCIDLib::TCard4 c4SendGetRedir
        (
                    TCIDDataSrcJan&         janSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            ,       TString&                strUltimateURL
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );

        tCIDLib::TCard4 c4SendHead
        (
                    TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );

        tCIDLib::TCard4 c4SendPost
        (
                    TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            , const tCIDLib::TKVPCollect&   colBody
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );

        tCIDLib::TCard4 c4SendPost
        (
                    TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bOutwardBody
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );

        tCIDLib::TVoid ClearAuthInfo();

        tCIDLib::TVoid ClearProxy();

        tCIDSock::EAddrTypes eAddrType() const;

        tCIDSock::EAddrTypes eAddrType
        (
            const   tCIDSock::EAddrTypes    eToSet
        );

        tCIDNet::ENetPReadRes eGetClientMsg
        (
                    TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
            ,       TString&                strReqType
            ,       tCIDLib::TKVPCollect&   colHdrLines
            ,       TString&                strContType
            ,       TString&                strURL
            ,       tCIDLib::TCard4&        c4ProtoVer
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bSendErrReply = kCIDLib::True
        );

        tCIDLib::TIPPortNum ippnProxy() const;

        tCIDLib::TVoid Reset();

        const TString& strProxy() const;

        const TString& strRepServer() const;

        TString& strRepServer
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetAuthInfo
        (
            const   TString&                strUserName
            , const TString&                strPassword
        );

        tCIDLib::TVoid SetProxy
        (
            const   TString&                strToSet
            , const tCIDLib::TIPPortNum     ippnProxy
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bValidateReqType
        (
            const   TString&                strToCheck
        );

        virtual tCIDLib::TBoolean bValidateProto
        (
            const   TString&                strToCheck
        );


    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid ThrowBadCharRef
        (
            const   tCIDLib::TCard4         c4Ofs
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHaveAuthInfo
        (
            const   TURL&                   urlSrc
            ,       TString&                strUserName
            ,       TString&                strPassword
        );

        tCIDLib::TCard4 c4DoOp
        (
                    TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strOpType
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bOutwardBody
            , const tCIDLib::TKVPCollect&   colInHdrLines
            , const TKeyValuePair* const    pkvalExLine
        );

        tCIDLib::TCard4 c4ReadResMsg
        (
            const   tCIDNet::ENetPReadRes   eRes
            ,       TString&                strResText
            ,       TString&                strErrMsg
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAutoAuth
        //      Tells us if we can automatically authorize if we have user
        //      info in the URL. Defaults to true, but it can be disabled if
        //      client code wants to do it for some reason.
        //
        //  m_c4ReqNum
        //      Used in digest authorization.
        //
        //  m_c4WaitConnSecs
        //      The number of seconds to wait for the server to accept a
        //      connection. Defaults to 5 seconds. This isn't the time to
        //      wait for a reply, just the connection time.
        //
        //  m_eAddrType
        //      When we resolve host names from URIs, we need to indicate an
        //      address family type. This defaults to IPV4 for backwards
        //      compability, but can be set to IPV6 (or unspecified to allow
        //      name resolution to choose.)
        //
        //  m_ippnProxy
        //      If the m_strProxy string is not empty, then this is the port
        //      number of the proxy to use. Else ignored.
        //
        //  m_strPassword
        //      The password to use if authentication is required and there is
        //      no info in the URL itself.
        //
        //  m_strProxy
        //      If this is not empty, then it's the IP address of a proxy
        //      server to send through.
        //
        //  m_strRepServer
        //      When error replies are generated, this is the name used in the
        //      Server: header line.
        //
        //  m_strUserName
        //      The usename to use if authentication is required and there is
        //      no info in the URL itself.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bAutoAuth;
        tCIDLib::TCard4         m_c4ReqNum;
        tCIDLib::TCard4         m_c4WaitConnSecs;
        tCIDSock::EAddrTypes    m_eAddrType;
        tCIDLib::TIPPortNum     m_ippnProxy;
        TString                 m_strPassword;
        TString                 m_strProxy;
        TString                 m_strRepServer;
        TString                 m_strUserName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(THTTPClient,TNetCoreParser)
};


// ---------------------------------------------------------------------------
//   CLASS: TAsyncHTTPQ
//  PREFIX: ahttpq
// ---------------------------------------------------------------------------
class CIDNETEXP TAsyncHTTPQ : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAsyncHTTPQ();

        TAsyncHTTPQ(const TAsyncHTTPQ&) = delete;

        ~TAsyncHTTPQ();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAsyncHTTPQ& operator=(const TAsyncHTTPQ&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GetOutput
        (
                    tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            ,       TString&                strUltimateURL
        );

        tCIDLib::TVoid CancelOp();

        const TError& errLast() const;

        tCIDLib::EAsyncWaitRes eState() const;

        const TString& strTitle() const;

        tCIDLib::TVoid StartGETRedir
        (
            const   TString&                strTitle
            , const TURL&                   urlToGet
            , const tCIDLib::TCard4         c4WaitFor
            , const TString&                strAgent
            , const TString&                strAccept
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );

        tCIDLib::TVoid StartPOST
        (
            const   TString&                strTitle
            , const TURL&                   urlToGet
            , const tCIDLib::TCard4         c4WaitFor
            , const TString&                strAgent
            , const TString&                strAccept
            , const tCIDLib::TKVPCollect&   colBody
            , const tCIDLib::TKVPCollect&   colInHdrLines
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected data types
        // -------------------------------------------------------------------
        enum class EOpTypes
        {
            None
            , GETRedir
            , POST
        };


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckNotBusy();

        tCIDLib::TVoid CopyInHdrLines
        (
            const   tCIDLib::TKVPCollect&   colSrc
            ,       tCIDLib::TKVPList&      colTar
        );

        tCIDLib::TVoid CopyOutHdrLines
        (
                    tCIDLib::TKVPCollect&   colTar
        );

        tCIDLib::EExitCodes eQueryThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid ResetOutput();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_OpType
        //      This tells the query thread what type of operation was requested,
        //      which tells him which op to do and which members are relevant.
        //
        //  m_eState
        //      Our current state. We use a standard async waiting result type
        //      which suits our needs. This tells us if it's ok to start a request,
        //      whther a call to get results can be completed, and so forth.
        //
        //  m_errLast
        //      If the thread gets an exception, it sets the state to indicate an
        //      error and stores the exception info here.
        //
        //  m_httpcThis
        //      The HTTP client that we use for our async queries.
        //
        //  m_thrQ
        //      The query thread that does the actual work in the background. This
        //      guy is started up for each new request and runs until it completes
        //      or times out. It updates the status member to indicate when it is
        //      done.
        //
        //  m_strTitle
        //      The caller provides us with this title for use in the thread name
        //      and any error messages.
        //
        //
        //  The rest are just the storage for the standard incoming HTTP request
        //  data so that the thread can see it, and for the thread to store the
        //  outgoing stuff to be returned to the caller.
        // -------------------------------------------------------------------
        EOpTypes                m_OpType;
        tCIDLib::EAsyncWaitRes  m_eState;
        TError                  m_errLast;
        THTTPClient             m_httpcThis;
        TThread                 m_thrQ;
        TString                 m_strTitle;

        // Incoming from client
        tCIDLib::TCard4         m_c4InContLen;
        tCIDLib::TCard4         m_c4WaitFor;
        TString                 m_strAgent;
        TString                 m_strAccept;
        tCIDLib::TKVPList       m_colInHdrLines;
        tCIDLib::TKVPList       m_colPostBody;
        THeapBuf                m_mbufInCont;
        TURL                    m_urlToGet;

        // Outgoing back to client
        tCIDLib::TCard4         m_c4OutContLen;
        tCIDLib::TCard4         m_c4ResCode;
        tCIDLib::TKVPList       m_colOutHdrLines;
        tCIDNet::EHTTPCodes     m_eCodeType;
        THeapBuf                m_mbufOutCont;
        TString                 m_strContType;
        TString                 m_strRepText;
        TString                 m_strUltimateURL;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAsyncHTTPQ, TObject)
};

#pragma CIDLIB_POPPACK


