//
// FILE NAME: CIDRTP_RTSP.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/08/2015
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
//  This class implements an RTSP server, or the core of one. It will be extended
//  in actual use in many cases.
//
//  The functionality provide is:
//
//  1.  Listens for TCP RTSP connections from clients. These can be to start a new
//      media session, or to continue control over an existing session.
//  2.  Newly established sessions are added to a session list, based on the unique
//      id that this server creates for each session.
//  3.  For each session contains establishes an RTCP TCP connection and a UDP
//      RTP server side socket, and handles the control and media comm.
//  4.  Handles incoming RTSP requests from clients and passes them on to the
//      session objects (identified by the session id.)
//
//  An application could use RTP session objects separately and may often do so,
//  just handling session establishment and control via its own private means, or
//  even fixed session parameters. This class just adds RTSP session control to
//  manage the creation, tear down and control of sessions, taking that burden
//  off of the application, allowing apps to easily support serving up media.
//
//  The application does need to extend this class and handle setup of data
//  sources for a given requested media format (or denying such requests because
//  they are unsupported.) This class doesn't know the ways in which data may
//  be pulled in to be served up.
//
//  We don't support UDP currently, but sessions are not tied to TCP/IP connections.
//  They are kept around for as long as any media session it has started is active.
//  At that point, if there is no connection, then the sesesion is dropped. The
//  session id is used to reconnect the client to the session.
//
//  We store the sessions via a counted pointer so that we can hand them to the
//  servicing threads but keep up with them ourself, and also to know if one is
//  already being serviced, since we don't allow multiple clients to connect to
//  the same session.
//
//  If there isn't a client connected to a session, then it is just idle and doesn't
//  have to do anything, so there doesn't need to be any internal thread for a
//  session. We have a scavenger thread that periodically checks for sessions that
//  are not being serviced and whose media session is completed.
//
//
//  We create our own simple derivative of the HTTP parser because we need to override
//  the standard validation of the request type and protocol. They will not be
//  HTTP standard ones here. Otherwise we can just use the HTTP client as is. But
//  it can be done internally. At our public API level, we only need to forward
//  reference the base HTTP client class.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class THTTPClient;


// ---------------------------------------------------------------------------
//  CLASS: TCIDRTSPSess
// PREFIX: rtsps
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTSPSess : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDRTSPSess
        (
            const   TString&                strId
        );

        TCIDRTSPSess(const TCIDRTSPSess&) = delete;
        TCIDRTSPSess(TCIDRTSPSess&&) = delete;

        ~TCIDRTSPSess();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDRTSPSess& operator=(const TCIDRTSPSess&) = delete;
        TCIDRTSPSess& operator=(TCIDRTSPSess&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Start
        (
            const   TURL&                   urlMedia
            ,       TCIDRTPIOSrc* const     prtpioAdopt
            ,       tCIDLib::TKVPList&      colHdrLines
            ,       TString&                strErr
            , const TString&                strHost
            , const tCIDLib::TIPPortNum     ippnSrvRTP
            , const tCIDLib::TIPPortNum     ippnSrvRTCP
        );

        tCIDLib::TVoid DoCmd
        (
            const   tCIDRTP::ERTSPCmds      eCmd
            ,       TString&                strInfoHdr
        );

        tCIDRTP::ERTSPStates eState() const;

        tCIDRTP::ERTSPStates eState
        (
            const   tCIDRTP::ERTSPStates    eToSet
        );

        const TString& strSessId() const;

        tCIDLib::TVoid Stop();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eState
        //      The current state of the session, we we run up based on interactions
        //      with the client. The ctor sets it to Done so it looks like some
        //      previous session has completed, as it will come back to on
        //      subsequent rounds.
        //
        //  m_rtptsSrc
        //      When the server has created us, it calls Start on us and provides
        //      us with an I/O object. We use that to start up our transmitter.
        //      At that point, it is running on its own and doing the work. This
        //      object just exists to provide a mapping back to the session id and
        //      to track our overall session state.
        //
        //  m_strSessId
        //      The session id assigned to this session. It's set in the ctor, though
        //      it doesn't get sent to the client until the session is actually
        //      ready. But it's the key for this session in the server's list so
        //      we need it set up front.
        //
        //  m_urlMedia
        //      The URL that this session was invoked for, so that we can use it
        //      in any msgs needed, and make it available to the RTP server for
        //      that same purpose.
        // -------------------------------------------------------------------
        tCIDRTP::ERTSPStates    m_eState;
        TCIDRTPTrans            m_rtptTrans;
        TString                 m_strSessId;
        TURL                    m_urlMedia;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDRTSPSess,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTSPSrv
// PREFIX: rtspsv
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTSPSrv : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TSessPtr  = TCntPtr<TCIDRTSPSess>;
        using TSessList = TKeyedHashSet<TSessPtr, TString, TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey
        (
            const   TSessPtr&               cptrSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDRTSPSrv();

        TCIDRTSPSrv(const TCIDRTSPSrv&) = delete;
        TCIDRTSPSrv(TCIDRTSPSrv&&) = delete;

        ~TCIDRTSPSrv();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDRTSPSrv& operator=(const TCIDRTSPSrv&) = delete;
        TCIDRTSPSrv& operator=(TCIDRTSPSrv&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bStart
        (
            const   tCIDLib::TIPPortNum     ippnListen
            , const tCIDLib::TIPPortNum     ippnListenSec = 0
        );

        tCIDLib::TVoid Stop();



    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4QueryMediaInfo
        (
            const   TURL&                   urlToGet
            ,       tCIDLib::TKVPCollect&   colToFill
            ,       TString&                strRepText
        ) = 0;

        virtual [[nodiscard]] TCIDRTPIOSrc* prtpioMakeNew
        (
            const   TURL&                   urlToGet
            , const tCIDLib::TKVPList&      colSessInfo
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckReqType
        (
            const   TString&                strCheck
            , const tCIDLib::TKVPList&      colLines
        );

        tCIDLib::TBoolean bExtractInfo
        (
            const   TString&                strURL
            , const tCIDLib::TKVPList&      colHdrLines
            ,       tCIDLib::TBoolean&      bClose
            ,       tCIDLib::TCard4&        c4SeqNum
            ,       TString&                strErrMsg
        );

        tCIDLib::EExitCodes eSrvThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid SendBasicReply
        (
                    THTTPClient&            httpcIO
            ,       TCIDDataSrc&            cdsClient
            , const tCIDLib::TCard4         c4SeqNum
            , const tCIDLib::TKVPList&      colHdrs
            , const tCIDLib::TBoolean       bClose
        );

        tCIDLib::TVoid SendLinesReply
        (
                    THTTPClient&            httpcIO
            ,       TCIDDataSrc&            cdsClient
            , const tCIDLib::TCard4         c4SeqNum
            , const tCIDLib::TKVPList&      colHdrs
            , const tCIDLib::TKVPList&      colBody
            , const tCIDLib::TBoolean       bClose
        );

        tCIDLib::TVoid SendErrReply
        (
                    THTTPClient&            httpcIO
            ,       TCIDDataSrc&            cdsClient
            , const tCIDLib::TCard4         c4Code
            , const tCIDLib::TCard4         c4SeqNum
            , const TString&                strMsg
            , const tCIDLib::TBoolean       bClose
        );

        tCIDLib::TVoid ServiceClient
        (
                    TThread&                thrThis
            ,       TSockLEngConn&          slecClient
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colSessions
        //      The current list of sessions, keyed by the seession id. It is
        //      thread safe and provides the sync between the incoming threads
        //      needing to to access it.
        //
        //  m_colThreads
        //      These threads watch the listener for incoming connections and
        //      service the clients. These are pointed at eSrvThread.
        //
        //  m_fcolPortMap
        //      Starting at m_ippnFirst, we allocate up to kMaxSessions pairs of
        //      ports for RTP sessions we start. We need some way to keep up with
        //      which ones are in use, so we size this to the max sessions and
        //      set the flag for each session that we put into use. When we drop
        //      a sessions, we clear that flag.
        //
        //  m_ippnFirst
        //      The first port number to use for RTP sessions. It must be an
        //      even number. We use up to the maximum allowed sesion count of
        //      pairs from this. It will default to the standard RTP port, but
        //      often the caller must provide a different one since that one will
        //      be in use.
        //
        //      See m_fcolPortMap also.
        //
        //  m_sleSrv
        //      We use a listener engine to handle listening for clients, on at
        //      least an non-secure connection and optionally secure.
        // -------------------------------------------------------------------
        TSessList               m_colSessions;
        tCIDLib::TThreadList    m_colThreads;
        tCIDLib::TBoolArray     m_fcolPortMap;
        tCIDLib::TIPPortNum     m_ippnFirst;
        TSockListenerEng        m_sleSrv;

        #if CID_DEBUG_ON
        TTextFileOutStream      m_strmLog;
        #endif


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDRTSPSrv,TObject)
};

#pragma CIDLIB_POPPACK


