//
// FILE NAME: CIDRTP_RTSPSrv.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/08/2015
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
//  This file implements the core of an RTSP server.
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
#include    "CIDRTP_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDRTSPSess,TObject)
RTTIDecls(TCIDRTSPSrv,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TRTPHTTPClient
// PREFIX: httpc
//
//  We need a simple derivative of the HTTP client class, so that we can override
//  some standard HTTP validation stuff, and allow our RTSP specific stuff.
// ---------------------------------------------------------------------------
class TRTPHTTPClient : public THTTPClient
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRTPHTTPClient() {}

        ~TRTPHTTPClient() {}


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValidateReqType
        (
            const   TString&                strToCheck
        );

        tCIDLib::TBoolean bValidateProto
        (
            const   TString&                strToCheck
        );
};


tCIDLib::TBoolean TRTPHTTPClient::bValidateReqType(const TString& strToCheck)
{
    return
    (
        strToCheck.bCompareI(L"ANNOUNCE")
        || strToCheck.bCompareI(L"DESCRIBE")
        || strToCheck.bCompareI(L"OPTIONS")
        || strToCheck.bCompareI(L"PAUSE")
        || strToCheck.bCompareI(L"PLAY")
        || strToCheck.bCompareI(L"SETUP")
        || strToCheck.bCompareI(L"TEARDOWN")
    );
}


tCIDLib::TBoolean TRTPHTTPClient::bValidateProto(const TString& strToCheck)
{
    return strToCheck.bCompareI(L"RTSP/1.0");
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDRTSPSess
// PREFIX: rtsps
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTSPSess: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDRTSPSess::TCIDRTSPSess(const TString& strId) :

    m_eState(tCIDRTP::ERTSPStates::Done)
    , m_strSessId(strId)
{
}

TCIDRTSPSess::~TCIDRTSPSess()
{
}


// ---------------------------------------------------------------------------
//  TCIDRTSPSess: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Start this session. This is done in response to the setup msg from the
//  client. The server creates a new I/O object for the requested URL and then
//  calls us here, with the setup header lines if we want to get more info from
//  them.
//
//  We use the I/O object to set up our transmitter. We first though make sure
//  that we have all the info we need, so that we can return an error directly
//  from here if possible. Once we start up the transmitter, any errors that
//  happen in there cannot be sent back via RTSP, it becomes RTP stuff at that
//  point.
//
tCIDLib::TCard4
TCIDRTSPSess::c4Start(  const   TURL&               urlMedia
                        ,       TCIDRTPIOSrc* const prtpioAdopt
                        ,       tCIDLib::TKVPList&  colHdrLines
                        ,       TString&            strErr
                        , const TString&            strTarHost
                        , const tCIDLib::TIPPortNum ippnOurRTP
                        , const tCIDLib::TIPPortNum ippnOurRTCP)
{
    // Put the I/O object into a janitor until we get it stored away
    TJanitor<TCIDRTPIOSrc> janIO(prtpioAdopt);

    try
    {
        TString strTransLn;

        // We should be in the Done state
        if (m_eState != tCIDRTP::ERTSPStates::Done)
        {
            strErr = L"Session already started";
            return kCIDNet::c4HTTPStatus_SrvError;
        }

        //
        //  Go through the headers and find any SDP info we need to have in order
        //  to start the session.
        //
        TString strTrans;
        if (!TNetCoreParser::bFindHdrLine(colHdrLines, L"Transport", strTrans))
        {
            strErr = L"Missing Transport info";
            return kCIDNet::c4HTTPStatus_BadHdrForResource;
        }

        //
        //  We can get more than one transport scheme. So first let's break it
        //  into comma separated values and look for the unicast one, which is
        //  all we can handle for now.
        //
        tCIDLib::TCard4 c4At;
        tCIDLib::TStrList colTransTypes;
        if (!TStringTokenizer::bParseCSVLine(strTrans, colTransTypes, c4At))
        {
            strErr = L"Parameter not understood";
            return kCIDNet::c4HTTPStatus_ParamNotKnown;
        }

        const tCIDLib::TCard4 c4TTCount = colTransTypes.c4ElemCount();
        tCIDLib::TCard4 c4TTIndex = 0;
        tCIDLib::TStrList colOpts;
        TString strToken;
        while (c4TTIndex < c4TTCount)
        {
            // Get the current trans type value to break it into semicolon tokens
            strTrans = colTransTypes[c4TTIndex];
            TStringTokenizer stokParse(&strTrans, L";");

            // Check the two required values
            TString strFmt;
            TString strCast;
            if (!stokParse.bGetNextToken(strFmt) || !stokParse.bGetNextToken(strCast))
            {
                // Can't be good, force an error and break out
                c4TTIndex = c4TTCount;
                break;;
            }

            if ((strFmt.bCompareI(L"RTP/AVP") || strFmt.bCompareI(L"RTP/AVP/UDP"))
            &&  strCast.bCompareI(L"unicast"))
            {
                // Break out the options
                colOpts.RemoveAll();
                while (stokParse.bGetNextToken(strToken))
                {
                    strToken.StripWhitespace();
                    colOpts.objAdd(strToken);
                }

                // And now break out
                break;
            }

            c4TTIndex++;
        }

        // If we went through them all, we never found a good one
        if (c4TTIndex == c4TTCount)
        {
            strErr = L"Unsupported transport";
            return kCIDNet::c4HTTPStatus_UnsupTransport;
        }

        // We have to at least get the client's port indicator
        tCIDLib::TIPPortNum ippnRTP = 0;
        tCIDLib::TIPPortNum ippnRTCP = 0;
        tCIDLib::TCard4 c4OptCnt = colOpts.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4OptCnt; c4Index++)
        {
            TString strCur = colOpts[c4Index];
            if (strCur.bStartsWithI(L"client_port"))
            {
                // Cut up to the equal sign then strip any other whitespace
                strCur.CutUpTo(kCIDLib::chEquals);
                strCur.StripWhitespace();

                // Now we should have two numbers, separated by dashes
                TString strRight;
                if (!strCur.bSplit(strRight, kCIDLib::chHyphenMinus)
                ||  !strCur.bToCard4(ippnRTP)
                ||  !strRight.bToCard4(ippnRTCP))
                {
                    strErr = L"Parameter not understood";
                    return kCIDNet::c4HTTPStatus_ParamNotKnown;
                }
            }
        }

        // If either client port is zero, we didn't get needed info
        if (!ippnRTP || !ippnRTCP)
        {
            strErr = L"Parameter not understood";
            return kCIDNet::c4HTTPStatus_ParamNotKnown;
        }

        // Create the IP end points for the two RTP session ports
        TIPEndPoint ipepClRTP(strTarHost, ippnRTP, tCIDSock::EAddrTypes::IPV4);
        TIPEndPoint ipepClRTCP(strTarHost, ippnRTCP, tCIDSock::EAddrTypes::IPV4);

        // Store the URL for later use
        m_urlMedia = urlMedia;

        if (facCIDRTP().bLogInfo())
        {
            facCIDRTP().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kRTPMsgs::msgInfo_Starting
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , ipepClRTP
                , ipepClRTCP
            );
        }

        //
        //  OK, let's start up our transmitter. We use our session id as the
        //  CNAME of the RTP session.
        //
        //  At this point we've done nothing that needs to be undone. If the
        //  transmitter fails to start, then it throws and we give up with an
        //  exception error below.
        //
        m_rtptTrans.StartSession
        (
            ipepClRTP
            , ipepClRTCP
            , ippnOurRTP
            , ippnOurRTCP
            , m_strSessId
            , janIO.pobjOrphan()
        );

        // Build up the return transport line
        strTrans = L"RTP/AVP/UDP;unicast;client_port=";
        strTrans.AppendFormatted(ippnRTP);
        strTrans.Append(kCIDLib::chHyphenMinus);
        strTrans.AppendFormatted(ippnRTCP);
        strTrans.Append(L";server_port=");
        strTrans.AppendFormatted(ippnOurRTP);
        strTrans.Append(kCIDLib::chHyphenMinus);
        strTrans.AppendFormatted(ippnOurRTCP);

        //
        //  Now clear out the header lines and start adding back in any that we
        //  want the server to send back.
        //
        colHdrLines.RemoveAll();
        colHdrLines.objAdd(TKeyValuePair(L"Session", m_strSessId));
        colHdrLines.objAdd(TKeyValuePair(L"Transport", strTrans));
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        strErr = L"Exception in session startup";
        return 500;
    }

    //
    //  Update our state to indicate we are ready. So now we can accept a run
    //  command to start.
    //
    m_eState = tCIDRTP::ERTSPStates::Ready;

    return kCIDNet::c4HTTPStatus_OK;
}


tCIDLib::TVoid
TCIDRTSPSess::DoCmd(const tCIDRTP::ERTSPCmds eCmd, TString& strInfoHdr)
{
    // If not in ready state, then do nothing
    if (m_eState != tCIDRTP::ERTSPStates::Ready)
        return;

    tCIDLib::TCard4 c4RTPTime;
    tCIDLib::TCard4 c4StartSeq;
    m_rtptTrans.DoCmd(eCmd, c4StartSeq, c4RTPTime);

    m_urlMedia.QueryText(strInfoHdr, kCIDLib::True, kCIDLib::False);
    strInfoHdr.Insert(L"url=", 0);
    strInfoHdr.Append(L";seq=");
    strInfoHdr.AppendFormatted(c4StartSeq);
    strInfoHdr.Append(L";rtptime=");
    strInfoHdr.AppendFormatted(c4RTPTime);
}


//
//  Get or set the sesssion state. Mostly we update state internally based on
//  incoming requests, but the server may want to change it.
//
tCIDRTP::ERTSPStates TCIDRTSPSess::eState() const
{
    return m_eState;
}

tCIDRTP::ERTSPStates TCIDRTSPSess::eState(const tCIDRTP::ERTSPStates eToSet)
{
    m_eState = eToSet;
    return m_eState;
}


// Access the session id
const TString& TCIDRTSPSess::strSessId() const
{
    return m_strSessId;
}



tCIDLib::TVoid TCIDRTSPSess::Stop()
{
    try
    {
        m_rtptTrans.EndSession();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}






// ---------------------------------------------------------------------------
//  CLASS: TCIDRTSPSrv
// PREFIX: rtptr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTSPSrv: Public, static methods
// ---------------------------------------------------------------------------
const TString& TCIDRTSPSrv::strKey(const TSessPtr& cptrSrc)
{
    return cptrSrc->strSessId();
}


// ---------------------------------------------------------------------------
//  TCIDRTSPSrv: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  If debugging, the max sessions value is 1, to make it more practical to debug
//  without multiple threads hitting the break points.
//
//  We set a sort of random first port value that is likely to be something
//  available, but it can be changed.
//
TCIDRTSPSrv::TCIDRTSPSrv() :

    m_colSessions
    (
      109
      , new TStringKeyOps
      , &TCIDRTSPSrv::strKey
      , tCIDLib::EMTStates::Safe
    )
    , m_colThreads(tCIDLib::EAdoptOpts::Adopt, kCIDRTP::c4MaxSessions)
    , m_fcolPortMap(kCIDRTP::c4MaxSessions, kCIDLib::False)
    , m_ippnFirst(kCIDRTP::ippnFirstRTP)
{
}

TCIDRTSPSrv::~TCIDRTSPSrv()
{
    // Make sure that we are all shut down
    Stop();
}


// ---------------------------------------------------------------------------
//  TCIDRTSPSrv: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TCIDRTSPSrv::bStart(const   tCIDLib::TIPPortNum    ippnListen
                    , const tCIDLib::TIPPortNum    ippnListenSec)
{
    // Make sure we aren't already running
    if (m_colThreads.c4ElemCount())
        return kCIDLib::False;

    //
    //  Make sure the listener engine can initialize before we bother setting up
    //  any processing threads.
    //
    try
    {
        m_sleSrv.Initialize(tCIDSock::ESockProtos::TCP, ippnListen, ippnListenSec);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
        return kCIDLib::False;
    }

    // Let's spin up all of our worker threads
    try
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDRTP::c4MaxSessions; c4Index++)
        {
            m_colThreads.Add
            (
                new TThread
                (
                    facCIDLib().strNextThreadName(TString(L"RTSPServer"))
                    , TMemberFunc<TCIDRTSPSrv>(this, &TCIDRTSPSrv::eSrvThread)
                )
            );
        }

        for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDRTP::c4MaxSessions; c4Index++)
            m_colThreads[c4Index]->Start();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TVoid TCIDRTSPSrv::Stop()
{
    //
    //  Stop the threads. We make one pass to ask them to start shutting down,
    //  non-blocking, and then go back and wait for them to stop.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDRTP::c4MaxSessions; c4Index++)
        m_colThreads[c4Index]->ReqShutdownNoSync();

    for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDRTP::c4MaxSessions; c4Index++)
    {
        try
        {
            m_colThreads[c4Index]->eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }
    }

    // Now we can clean up the listener engine
    try
    {
        m_sleSrv.Cleanup();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    // Clean up any outstanding sessions
    try
    {

    }

    catch(TError& errToCatch)
    {
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TCIDRTSPSrv: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We check the passed heaader lines for an acceptable content type line and
//  see if it includes the passed value, case insensitive just in case.
//
tCIDLib::TBoolean
TCIDRTSPSrv::bCheckReqType(const TString& strCheck, const tCIDLib::TKVPList& colLines)
{
    // If no accept line, then clearly
    TString strLine;
    if (!TNetCoreParser::bFindHdrLine(colLines, THTTPClient::strHdr_Accept, strLine))
        return kCIDLib::False;

    //
    //  Tokenize it on commas. Add spaces as well, so we don't have to bother doing
    //  another whitespace stripping. There can't be any spaces in the values.
    //
    TStringTokenizer stokCur(&strLine, L", ");

    TString strVal;
    while (stokCur.bGetNextToken(strVal))
    {
        if (strVal.bCompareI(strCheck))
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  Given some info from the incoming client message, we extract the commonly
//  required informaiton and return it, if found.
//
//  We also get the current state of the connection. If we don't get some piece of
//  info that is required for the current state, we return false and a reply msg
//  to send.
//
tCIDLib::TBoolean
TCIDRTSPSrv::bExtractInfo(  const   TString&            strURL
                            , const tCIDLib::TKVPList&  colHdrLines
                            ,       tCIDLib::TBoolean&  bClose
                            ,       tCIDLib::TCard4&    c4SeqNum
                            ,       TString&            strErrMsg)
{
    TString strVal;

    // Assume not closing unless the client indicates he is
    bClose = kCIDLib::False;
    if (TNetCoreParser::bFindHdrLine(colHdrLines, THTTPClient::strHdr_Connection, strVal))
        bClose = strVal.eCompareI(L"Close") == tCIDLib::ESortComps::Equal;

    // We have to have a sequence number
    if (!TNetCoreParser::bFindHdrLine(colHdrLines, L"CSeq", strVal)
    ||  !strVal.bToCard4(c4SeqNum, tCIDLib::ERadices::Dec))
    {
        c4SeqNum = 0;
        strErrMsg = L"CSeq number was not provided or invalid";
        return kCIDLib::False;
    }

    // Everything went OK
    return kCIDLib::True;
}



//
//  All of the worker threads are pointed at this guy. They all just block on the
//  listener engine waiting for a connection. When we get one, we service that
//  client.
//
tCIDLib::EExitCodes TCIDRTSPSrv::eSrvThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the calling thread go
    thrThis.Sync();

    while (kCIDLib::True)
    {
        try
        {
            // Check for shutdown requests
            if (thrThis.bCheckShutdownRequest())
                break;

            // Just block on the listener engine waiting for a client
            TSockLEngConn* pslecNew = m_sleSrv.pslecWait(500);
            if (pslecNew)
            {
                //
                //  Put a janitor on the connection and let's service the
                //  client.
                //
                TJanitor<TSockLEngConn> janConn(pslecNew);
                ServiceClient(thrThis, *pslecNew);
            }

        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {

        }
    }

    return tCIDLib::EExitCodes::Normal;
}


//
//  This is called when we get a connection from a client. There are these
//  scenarios based on the msg we get.
//
//  1. We get an OPTIONS msg, in which case we just respond with our supported
//      options.
//  2. We get a DESCRIBE msg, in which case we respond with SDP describing the
//      available data.
//  3. We get a SETUP msg, in which case we create a new session with a newly
//      generated session id, and put it in the list.
//  4. We get some other msg, in which case it is referring to an existing session,
//      so we look up the session and let it process the request.
//
//
//  Other options we don't worry about, because we don't report them as supported
//  options.
//
tCIDLib::TVoid
TCIDRTSPSrv::ServiceClient(TThread& thrThis, TSockLEngConn& slecClient)
{
    tCIDLib::TBoolean       bCloseReq = kCIDLib::False;
    tCIDLib::TCard4         c4ContLen;
    tCIDLib::TCard4         c4CSeq;
    tCIDLib::TCard4         c4ProtoVer;
    tCIDLib::TKVPList       colHdrLines;
    tCIDLib::TKVPList       colOutLines;
    tCIDLib::TKVPList       colBodyLines;
    TString                 strReqType;
    TString                 strContType;
    TString                 strURL;
    TString                 strErrMsg;

    try
    {
        #if CID_DEBUG_ON
        if (!m_strmLog.bIsOpen())
        {
            m_strmLog.Open
            (
                L"RTSPLog.Txt"
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Write
            );
        }
        #endif

        // Create some other stuff best done inside the try
        THeapBuf                mbufCont(8192, 128 * 1024);
        TRTPHTTPClient          httpcIO;

        //
        //  Create a data source to talk to the client and put a janitor on it.
        //  That will initalize it and insure it gets cleaned up correctly if we
        //  exist normally or exception.
        //
        TCIDSockStreamDataSrc*  pcdsClient = new TCIDSockStreamDataSrc
        (
            slecClient.psockConn(), tCIDLib::EAdoptOpts::NoAdopt
        );
        TCIDDataSrcJan janDataSrc
        (
            pcdsClient
            , tCIDLib::EAdoptOpts::Adopt
            , kCIDLib::True
            , TTime::enctNowPlusSecs(3)
            , 3000
        );

        while(kCIDLib::True)
        {
            // Watch for a shutdown request
            if (thrThis.bCheckShutdownRequest())
                break;

            // Wait a while for a msg from the client
            const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs(2500);
            colHdrLines.RemoveAll();
            const tCIDNet::ENetPReadRes eRes = httpcIO.eGetClientMsg
            (
                *pcdsClient
                , enctEnd
                , strReqType
                , colHdrLines
                , strContType
                , strURL
                , c4ProtoVer
                , mbufCont
                , c4ContLen
                , kCIDLib::False
            );

            // If nothing, then go back to the top
            if (eRes != tCIDNet::ENetPReadRes::Success)
                continue;

            // Log the msg info if debugging
            #if CID_DEBUG_ON
            {
                m_strmLog   << L"\n---- InMsg: ----------------\n"
                            << L"URL=" << strURL << kCIDLib::NewLn
                            << L"Proto=" << TCardinal(c4ProtoVer, tCIDLib::ERadices::Hex) << kCIDLib::NewLn
                            << L"ReqType=" << strReqType << kCIDLib::NewLn
                            << L"ContLen=" << c4ContLen << kCIDLib::DNewLn;

                const tCIDLib::TCard4 c4Count = colHdrLines.c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    const TKeyValuePair& kvalCur = colHdrLines[c4Index];
                    m_strmLog   << L"      " << kvalCur.strKey() << L"="
                                << kvalCur.strValue() << kCIDLib::NewLn;
                }
                m_strmLog << kCIDLib::DNewLn;
            }
            #endif

            //
            //  Call a helper that will pull out various important bits of info
            //  for us, saving a lot of busy work. Some of it may not be present
            //  or necessary given our current state.
            //
            const tCIDLib::TBoolean bGotInfo = bExtractInfo
            (
                strURL, colHdrLines, bCloseReq, c4CSeq, strErrMsg
            );

            // If we didn't get required info for our state, then error out
            if (!bGotInfo)
            {
                SendErrReply(httpcIO, *pcdsClient, 400, c4CSeq, strErrMsg, kCIDLib::True);
                break;
            }

            tCIDLib::TCard4 c4Res;
            if (strReqType.bCompareI(L"ANNOUNCE"))
            {
                //
                //  Not supported, so send an error back. Don't need to disconnect
                //  unless they asked to. They can try something else.
                //
                SendErrReply(httpcIO, *pcdsClient, 501, c4CSeq, L"Not implemented", kCIDLib::True);
            }
             else if (strReqType.bCompareI(L"OPTIONS"))
            {
                // Just send back our options
                colOutLines.RemoveAll();
                colOutLines.objAdd
                (
                    TKeyValuePair(L"Public", L"DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE")
                );
                SendBasicReply(httpcIO, *pcdsClient, c4CSeq, colOutLines, bCloseReq);
            }
             else if (strReqType.bCompareI(L"DESCRIBE"))
            {
                // Verify they can accept SDP
                if (bCheckReqType(L"application/sdp", colHdrLines))
                {
                    TURL urlSrc(strURL, tCIDSock::EQualified::Full);

                    //
                    //  Let the derived class look at the URL and provide us with
                    //  the description info. We get back a list of key/value pairs,
                    //  which we assume to be SDP content.
                    //
                    colBodyLines.RemoveAll();
                    c4Res = c4QueryMediaInfo(urlSrc, colBodyLines, strErrMsg);

                    if (c4Res == kCIDNet::c4HTTPStatus_OK)
                    {
                        colOutLines.RemoveAll();
                        colOutLines.objAdd
                        (
                            TKeyValuePair(THTTPClient::strHdr_ContType, L"application/sdp")
                        );

                        SendLinesReply
                        (
                            httpcIO, *pcdsClient, c4CSeq, colOutLines, colBodyLines, bCloseReq
                        );
                    }
                     else
                    {
                        SendErrReply(httpcIO, *pcdsClient, c4Res, c4CSeq, strErrMsg, kCIDLib::True);
                    }
                }
                 else
                {
                    // Can't do it, so send error and close
                    SendErrReply(httpcIO, *pcdsClient, 406, c4CSeq, L"Not acceptable", kCIDLib::True);
                }
            }
             else if (strReqType.bCompareI(L"SETUP"))
            {
                //
                //  If there is a session id, then it has to be referring to a
                //  previously established session, which we will reject because
                //  we don't support changing session format.
                //
                TString strId;
                if (TNetCoreParser::bFindHdrLine(colHdrLines, L"Session", strId))
                {
                    SendErrReply(httpcIO, *pcdsClient, 455, c4CSeq, L"Not in this state", kCIDLib::True);
                }
                 else
                {
                    // Parse the URL that we are to do
                    TURL urlSrc(strURL, tCIDSock::EQualified::Full);

                    //
                    //  Let's create a new session. We need to generate a new
                    //  unique id for it.
                    //
                    TSessPtr cptrNew(new TCIDRTSPSess(TUniqueId::strMakeId()));

                    //
                    //  Try to initialize it. If it works, then start it up and
                    //  add it to the list, else reject and the the cnt ptr object
                    //  will clean it up on the way out.
                    //
                    //  We have to ask our derived class to create the appropriate
                    //  I/O object for the URL and header info.
                    //
                    TCIDRTPIOSrc* prtpioSess = prtpioMakeNew(urlSrc, colHdrLines);
                    if (prtpioSess)
                    {
                        //
                        //  Figure out the ports we'll use for our side. We look
                        //  through the port pair map to find one marked as available
                        //
                        tCIDLib::TIPPortNum ippnRTP = m_ippnFirst;

                        TString strTransLn;
                        c4Res = cptrNew->c4Start
                        (
                            urlSrc
                            , prtpioSess
                            , colHdrLines
                            , strErrMsg
                            , slecClient.ipepClient().strAsText()
                            , ippnRTP
                            , ippnRTP + 1
                        );

                        if (c4Res == kCIDNet::c4HTTPStatus_OK)
                        {
                            //
                            //  Add it to the list and send an OK response. The session
                            //  object will give us back any lines we should send back
                            //  out.
                            //
                            m_colSessions.objAdd(cptrNew);

                            colOutLines.RemoveAll();
                            SendBasicReply(httpcIO, *pcdsClient, c4CSeq, colHdrLines, bCloseReq);
                        }
                         else
                        {
                            SendErrReply(httpcIO, *pcdsClient, c4Res, c4CSeq, strErrMsg, kCIDLib::False);
                        }
                    }
                     else
                    {
                        SendErrReply(httpcIO, *pcdsClient, 500, c4CSeq, L"I/O initialization failed", kCIDLib::True);
                    }
                }
            }
             else if (strReqType.bCompareI(L"PLAY")
                  ||  strReqType.bCompareI(L"PAUSE"))
            {
                TString strId;
                TSessPtr cptrTar;
                if (TNetCoreParser::bFindHdrLine(colHdrLines, L"Session", strId))
                    cptrTar = m_colSessions.objFindByKey(strId);

                if (cptrTar.pobjData())
                {
                    tCIDRTP::ERTSPCmds eCmd;
                    TString strInfo;
                    if (strReqType.bCompareI(L"PLAY"))
                        eCmd = tCIDRTP::ERTSPCmds::Play;
                    else if (strReqType.bCompareI(L"PAUSE"))
                        eCmd = tCIDRTP::ERTSPCmds::Pause;
                    cptrTar->DoCmd(eCmd, strInfo);

                    colOutLines.RemoveAll();
                    colOutLines.objAdd(TKeyValuePair(L"Session", cptrTar->strSessId()));

                    if (strReqType.bCompareI(L"PLAY"))
                        colOutLines.objAdd(TKeyValuePair(L"RTP-Info", strInfo));
                    SendBasicReply(httpcIO, *pcdsClient, c4CSeq, colOutLines, bCloseReq);
                }
                 else
                {
                    SendErrReply
                    (
                        httpcIO
                        , *pcdsClient
                        , kCIDNet::c4HTTPStatus_SessNotFound
                        , c4CSeq
                        , L"Session not found"
                        , kCIDLib::False
                    );
                }
            }
             else if (strReqType.bCompareI(L"TEARDOWN"))
            {
                TString strId;
                TSessPtr cptrTar;
                if (TNetCoreParser::bFindHdrLine(colHdrLines, L"Session", strId))
                    cptrTar = m_colSessions.objFindByKey(strId);

                if (cptrTar.pobjData())
                {
                    // Stop the session
                    try
                    {
                        cptrTar->Stop();
                    }

                    catch(TError& errToCatch)
                    {
                    }

                    catch(...)
                    {
                    }

                    m_colSessions.bRemoveKeyIfExists(strId);

                    // Send an OK
                    colOutLines.RemoveAll();
                    SendBasicReply(httpcIO, *pcdsClient, c4CSeq, colOutLines, bCloseReq);
                }
                 else
                {
                    SendErrReply(httpcIO, *pcdsClient, 454, c4CSeq, L"Session not found", kCIDLib::False);
                }
            }
             else
            {
            }

            // If we got a close request, then let's bail out
            if (bCloseReq)
                break;


            #if CID_DEBUG_ON
            m_strmLog.Flush();
            #endif
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }

    #if CID_DEBUG_ON
    try { m_strmLog.Flush(); } catch(...) {}
    #endif
}


//
//  In this cases where we don't need to send any msg body, this guy is called
//  to send a simple reply. We know that the response is 200/OK since we are sending
//  back a non-error message.
//
tCIDLib::TVoid
TCIDRTSPSrv::SendBasicReply(        THTTPClient&        httpcIO
                            ,       TCIDDataSrc&        cdsClient
                            , const tCIDLib::TCard4     c4SeqNum
                            , const tCIDLib::TKVPList&  colHdrs
                            , const tCIDLib::TBoolean   bClose)
{
    // Set up memory buffer based text stream. Use one of our I/O streams
    TTextMBufOutStream strmTar(4192, 16 * 1024, new TUTF8Converter);
    strmTar.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

    // Output the header stuff
    strmTar << L"RTSP/1.0 200 OK\n"
            << L"CSeq: " << c4SeqNum << kCIDLib::NewLn
            << L"Content-Length: 0\n";

    // Put out a close indicator if asked
    if (bClose)
        strmTar << L"Connect: Close\n";

    // Do any of the headers in the header list
    const tCIDLib::TCard4 c4HdrCnt = colHdrs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4HdrCnt; c4Index++)
    {
        const TKeyValuePair& kvalCur = colHdrs[c4Index];
        strmTar << kvalCur.strKey() << L": " << kvalCur.strValue() << kCIDLib::NewLn;
    }

    // And put out the separator empty line
    strmTar << kCIDLib::EndLn;

    #if CID_DEBUG_ON
    {
        m_strmLog   << L"\n---- OutMsg: ---------------\n";
        TUTF8Converter tcvtTmp;
        TString strMsg;
        tcvtTmp.c4ConvertFrom
        (
            strmTar.mbufData().pc1Data()
            , strmTar.c4CurSize()
            , strMsg
        );
        tCIDLib::TCard4 c4At = 0;
        strMsg.bReplaceSubStr(L"\r\n", L"\n", c4At, kCIDLib::True);
        m_strmLog   << strMsg
                    << L"\n----------------------" << kCIDLib::NewEndLn;
    }
    #endif

    // And now we can send the accumulated data
    cdsClient.WriteBytes(strmTar.mbufData(), strmTar.c4CurSize());
    cdsClient.FlushOut(TTime::enctNowPlusSecs(5));
}



//
//  In this cases where the body content is just a list of key=value pairs, such
//  as an SDP reply. We know the first line is 200/OK. We also can get headerlines
//  to send.
//
tCIDLib::TVoid
TCIDRTSPSrv::SendLinesReply(        THTTPClient&        httpcIO
                            ,       TCIDDataSrc&        cdsClient
                            , const tCIDLib::TCard4     c4SeqNum
                            , const tCIDLib::TKVPList&  colHdrs
                            , const tCIDLib::TKVPList&  colBody
                            , const tCIDLib::TBoolean   bClose)
{
    //
    //  Set up an output stream for the body content and get that streamed out
    //  so that we know the content length. UTF8 as with all RTSP stuff.
    //
    TTextMBufOutStream strmBody(8192, 16 * 1024, new TUTF8Converter);
    {
        strmBody.eNewLineType(tCIDLib::ENewLineTypes::CRLF);
        const tCIDLib::TCard4 c4BodyCnt = colBody.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BodyCnt; c4Index++)
        {
            const TKeyValuePair& kvalCur = colBody[c4Index];
            strmBody    << kvalCur.strKey() << L"=" << kvalCur.strValue()
                        << kCIDLib::NewLn;
        }
        strmBody.Flush();
    }

    // And another one for the msg header stuff
    TTextMBufOutStream strmHdr(1024, 16 * 1024, new TUTF8Converter);
    {
        strmHdr.eNewLineType(tCIDLib::ENewLineTypes::CRLF);
        strmHdr << L"RTSP/1.0 200 OK\n"
                << L"CSeq: " << c4SeqNum << kCIDLib::NewLn
                << THTTPClient::strHdr_ContLen << L": " << strmBody.c4CurSize()
                << kCIDLib::NewLn;

        // Put out a close indicator if asked
        if (bClose)
            strmHdr << L"Connect: Close\n";

        // Do any of the headers in the header list
        const tCIDLib::TCard4 c4HdrCnt = colHdrs.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4HdrCnt; c4Index++)
        {
            const TKeyValuePair& kvalCur = colHdrs[c4Index];
            strmHdr << kvalCur.strKey() << L": " << kvalCur.strValue() << kCIDLib::NewLn;
        }

        // Now put out the blank line, and this will flush it also
        strmHdr << kCIDLib::EndLn;
    }

    #if CID_DEBUG_ON
    {
        m_strmLog   << L"\n---- OutMsg: ---------------\n";
        TUTF8Converter tcvtTmp;
        TString strMsg;
        tcvtTmp.c4ConvertFrom
        (
            strmHdr.mbufData().pc1Data()
            , strmHdr.c4CurSize()
            , strMsg
        );
        tcvtTmp.c4ConvertFrom
        (
            strmBody.mbufData().pc1Data()
            , strmBody.c4CurSize()
            , strMsg
            , tCIDLib::EAppendOver::Append
        );

        tCIDLib::TCard4 c4At = 0;
        strMsg.bReplaceSubStr(L"\r\n", L"\n", c4At, kCIDLib::True);
        m_strmLog   << strMsg
                    << L"\n----------------------" << kCIDLib::NewEndLn;
    }
    #endif

    // And now we can send the accumulated data
    cdsClient.WriteBytes(strmHdr.mbufData(), strmHdr.c4CurSize());
    cdsClient.WriteBytes(strmBody.mbufData(), strmBody.c4CurSize());

    // And flush the data source to send the data
    cdsClient.FlushOut(TTime::enctNowPlusSecs(5));
}


//
//  A general purpose helper to send an error response.
//
tCIDLib::TVoid
TCIDRTSPSrv::SendErrReply(          THTTPClient&        httpcIO
                            ,       TCIDDataSrc&        cdsClient
                            , const tCIDLib::TCard4     c4Code
                            , const tCIDLib::TCard4     c4SeqNum
                            , const TString&            strMsg
                            , const tCIDLib::TBoolean   bClose)
{
    // Set up memory buffer based text stream. Use one of our I/O streams
    TTextMBufOutStream strmTar(4192, 16 * 1024, new TUTF8Converter);
    strmTar.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

    // Output the header stuff
    strmTar << L"RTSP/1.0 " << c4Code << L' ' << strMsg << kCIDLib::NewLn
            << L"CSeq: " << c4SeqNum << kCIDLib::NewLn
            << L"Content-length: 0\n";

    // Put out a close indicator if asked
    if (bClose)
        strmTar << L"Connect: Close\n";

    // And put out the separator empty line
    strmTar << kCIDLib::EndLn;

    #if CID_DEBUG_ON
    {
        m_strmLog   << L"\n---- OutMsg: ---------------\n";
        TUTF8Converter tcvtTmp;
        TString strMsg;
        tcvtTmp.c4ConvertFrom
        (
            strmTar.mbufData().pc1Data()
            , strmTar.c4CurSize()
            , strMsg
        );
        tCIDLib::TCard4 c4At = 0;
        strMsg.bReplaceSubStr(L"\r\n", L"\n", c4At, kCIDLib::True);
        m_strmLog   << strMsg
                    << L"\n----------------------" << kCIDLib::NewEndLn;
    }
    #endif

    // And now we can send the accumulated data
    cdsClient.WriteBytes(strmTar.mbufData(), strmTar.c4CurSize());
    cdsClient.FlushOut(TTime::enctNowPlusSecs(5));
}


