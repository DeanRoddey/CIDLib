//
// FILE NAME: HTTPSrv1.cpp
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
//  This is the main (only) module of the HTTPSrv1 demo program. This program demos
//  a simple web server type functionality. This guy doesn't support listening on both
//  secure and non-secure ports at the same time, so you have to choose one mode or
//  the other via startup parameters.
//
//  We do the following:
//
//  1. Create a listener for the port you indicate. We don't use the more elaborate
//      listener engine (which will listen on all interfaces) we just create a simple
//      single listener object.
//  2. Spin up a pool of threads to handle requests. You can request 'single' mode
//      for a single thread. That's easier for debugging purposes since some browsers
//      will make multiple simultaneous requests on different socket connections.
//  3. Listen for connections and queue them up when received
//  4. A thread from the pool picks up each connection and services that client until
//      it drops the connection or we get some error or it's a 1.0 connection or the
//      close header flag is set.
//
//  A command line parameter indicates the port we listen on, and whether we support
//  secure or non-secure connections. For secure connections we also require a
//  certificate info string that tells us which certificate we pass to clients for
//  secure connections.
//
//  You also provide a root file path that represents the '/' URL path. All URLs are
//  then translated into paths relative to that root path. So if the path is
//  C:\Test, a URL of /Bogus/Data.html will be C:\Test\Bogus\Data.html.
//
//  Of course in a real web server you'd do this a lot nicer and with more
//  infrastructure, but this demonstrates the basic functionality. The service threads
//  would likely be derived from TThread so that the thread method is internal and
//  not just a global method like we do here for simplicity. And you'd have to deal
//  with the possiblity of spinning up more worker threads if the load gets too
//  high. There's also nothing done here to deal with caching of data, which a real
//  web server would need to do.
//
//  We also aren't dealing with anything related to query parameters on URLs. We don't
//  support any sort of plugin handlers for customized handling of queries in this
//  simple test, and just serving up files never uses any query parameters. In a real
//  server you'd likely provide a means to invoke a CML macro or some sort of outboard
//  customization schemes, to which you'd pass any query parms.
//
//  And we only only handle GETs here, no POSTs or PUTS or HEADs. Anything other than a
//  GET will cause an error reply to be sent back.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDCrypto.hpp"
#include    "CIDEncode.hpp"
#include    "CIDNet.hpp"
#include    "CIDSChan.hpp"


// ----------------------------------------------------------------------------
//  Local types
// ----------------------------------------------------------------------------
using TClientQ = TRefQueue<TServerStreamSocket>;



// ----------------------------------------------------------------------------
//  Local data
//
//  bSecure
//      Indicates whether we are doing secure connections or non-secure. Driven by
//      command line parms.
//
//  bSingleThread
//      Defaults to false, but the /Single parameter sets it to true. It makes us
//      only accept a single connection at a time, so we don't have multiple threads
//      confusing the debugging process.
//
//  c4DefThreadCnt
//      The default number of worker threads we start up to process requests. But
//      if /Single is provided, we only do 1, which is very useful for debugging.
//
//  c4WaitReq
//      how many seconds we'll wait for the client to send us a request before we drop
//      the connection. Make it longer when debugging so that we can trace down into
//      calls without it timing out.
//
//  colSockQ
//      New sockets from client connections are queued up here for worker threads
//      to pick up. It is created in thread safe mode of course.
//
//  mtxSyncOut
//      For our simple program, we want to dump output to the console to help with
//      seeing what is going on. So we need to synchronize this output because multiple
//      threads are involved unless you use /Single.
//
//  strCertInfo
//      If doing secure connections this is the certificate info that we use to tell
//      the secure channel what certificate to present to clients.
//
//  strRootPath
//      The path that represents the / URL. So everything is relative to this path.
//
//  strmOut
//      We don't need any interactivity for this program so we just set up an output
//      text stream, which writes to the standard output. We use this to dump out
//      debugging type info to indicate what is going on.
// ----------------------------------------------------------------------------
static tCIDLib::TBoolean        bSecure = kCIDLib::False;
static tCIDLib::TBoolean        bSingleThread = kCIDLib::False;
static const tCIDLib::TCard4    c4DefThreadCnt = 8;
static TClientQ                 colSockQ(tCIDLib::EAdoptOpts::Adopt, tCIDLib::EMTStates::Safe);
static TMutex                   mtxSyncOut;
static TString                  strCertInfo;
static TString                  strRootPath;
static TTextFileOutStream       strmOut(tCIDLib::EStdFiles::StdOut);

#if CID_DEBUG_ON
static const tCIDLib::TCard4    c4WaitReq = 30;
#else
static const tCIDLib::TCard4    c4WaitReq = 10;
#endif



// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"HTTPSrv1MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  Show the info for an incoming request. We just dump the info to the console,
//  synchronized using the mutex (so multple incoming request handlers don't step
//  on each other.)
//
static tCIDLib::TVoid ShowReq(  const   tCIDLib::TCard4     c4ThreadInd
                                , const TString&            strURL
                                , const tCIDLib::TKVPList&  colHdrLines)
{
    TLocker lockrSync(&mtxSyncOut);

    strmOut << L'[' << c4ThreadInd << L"] - Request For : " << strURL << kCIDLib::EndLn;

    TString strClip;
    tCIDLib::TKVPList::TCursor cursHdr(&colHdrLines);
    for (; cursHdr; ++cursHdr)
    {
        const TKeyValuePair& kvalCur = *cursHdr;

        strmOut << L"      " << kvalCur.strKey() << L" = ";
        if (kvalCur.strValue().c4Length() > 40)
        {
            strClip = kvalCur.strValue();
            strClip.CapAt(40);
            strClip.Append(L"...");
            strmOut << strClip;
        }
         else
        {
            strmOut << kvalCur.strValue();
        }
        strmOut << kCIDLib::NewLn;
    }
    strmOut.Flush();
};



//
//  Some helpers to output msgs to our console window in a thread safe way since
//  we can have multiple service threads working at once.
//
static tCIDLib::TVoid ShowMsg(  const   tCIDLib::TCard4 c4ThreadInd
                                , const TString&        strToDisplay)
{
    TLocker lockrSync(&mtxSyncOut);
    strmOut << L'[' << c4ThreadInd << L"] - " << strToDisplay << kCIDLib::EndLn;
}

static tCIDLib::TVoid ShowMsg(  const   tCIDLib::TCard4     c4ThreadInd
                                , const tCIDLib::TCh* const pszToDisplay)
{
    TLocker lockrSync(&mtxSyncOut);
    strmOut << L'[' << c4ThreadInd << L"] - " << pszToDisplay << kCIDLib::EndLn;
}


//
//  This will only be called before any threads are started, so no need to sync
//  output to the output stream.
//
static tCIDLib::TVoid ShowUsage()
{
    strmOut << L"Usage:\n"
               L"   HTTPSrv1 rootpath port [/Single] [/Secure=certinfo]\n"

               L"      example: C:\\HTMLData 587 /Secure=\"UStore:My,Bubba\"\n"
               L"      example: C:\\HTMLData 80\n\n"

               L"   For testing use /Single to only accept one connection at a time\n"
            << kCIDLib::NewEndLn;
}


//
//  Called to process a request that appears to be reasonable, i.e. it's a valid path
//  to a file that exists.
//
//  The caller provides an output stream to use. It is set up for UTF-8 encoding,
//  which is the default, so we don't have to indicate that if returning text.
//
static tCIDLib::TVoid
ProcessReq( const   tCIDLib::TCard4     c4ThreadInd
            , const TURL&               urlGot
            , const TPathStr            pathRes
            , const TFindBuf&           fndbRes
            , const tCIDLib::TKVPList   colHdrLines
            ,       TTextMBufOutStream& strmTar
            ,       TCIDDataSrc&        cdsClient
            , const tCIDLib::TBoolean   bClose)
{
    // Reset the output stream so we can build up a new msg
    strmTar.Reset();

    //
    //  Let's try to read in the contents of the message, and guess based on the
    //  extension what MIME type it is. We don't try to support all possible types
    //  here, just the obvious stuff for testing. We assume text files are in
    //  standard Latin1 format, so we don't send a charset.
    //
    try
    {
        const tCIDLib::TCard4 c4ContLen = tCIDLib::TCard4(fndbRes.c8Size());
        THeapBuf mbufFile(c4ContLen, c4ContLen);
        TBinaryFile flSrc(pathRes);
        flSrc.Open
        (
            tCIDLib::EAccessModes::Multi_Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        flSrc.c4ReadBuffer(mbufFile, c4ContLen);

        //
        //  Get the extension out and use that to set up the MIME type. If no
        //  extension, just assume text of some sort.
        //
        TString strContType;
        if (!pathRes.bQueryExt(strContType))
            strContType = L"text";

        strContType.ToUpper();
        if (strContType == L"CSS")
            strContType = L"text/css";
        else if ((strContType == L"HTML") || (strContType == L"HTM"))
            strContType = L"text/html";
        else if ((strContType == L"JPEG") || (strContType == L"JPG"))
            strContType = L"image/jpeg";
        else if (strContType == L"JS")
            strContType = L"text/javascript";
        else if (strContType == L"JSON")
            strContType = L"application/json";
        else if (strContType == L"PNG")
            strContType = L"image/png";
        else if (strContType == L"XML")
            strContType = L"text/xml";
        else
            strContType = L"text";

        //
        //  Format out the header content. Include a close header line if the
        //  handler says it needs to close after this reply.
        //
        strmTar << L"HTTP/1.1 200 OK" << kCIDLib::NewLn
                << THTTPClient::strHdr_ContType << L": " << strContType << kCIDLib::NewLn
                << THTTPClient::strHdr_ContLen << L": " << c4ContLen << kCIDLib::NewLn
                << THTTPClient::strHdr_Connection
                << (bClose ? L": Close\n" : L": Keep-Alive\n")
                << kCIDLib::NewLn
                << kCIDLib::FlushIt;

        //
        //  Now write out both the header and body content to the data source, and
        //  flush it to the client, which causes it to be sent. We give it an end
        //  time it has to send it by or fail.
        //
        cdsClient.WriteBytes(strmTar.mbufData(), strmTar.c4CurSize());
        cdsClient.WriteBytes(mbufFile, c4ContLen);
        cdsClient.FlushOut(TTime::enctNowPlusSecs(5));

        ShowMsg(c4ThreadInd, L"Sent requested resource");
    }

    catch(const TError& errToCatch)
    {
        TString strErr(L"Error sending reply: ", errToCatch.strErrText().c4Length());
        strErr += errToCatch.strErrText();
        ShowMsg(c4ThreadInd, strErr);
    }
}


//
//  A simple helper to send an error reply with no body
//
static tCIDLib::TVoid
SendErrReply(const  tCIDLib::TCard4     c4ThreadInd
            , const tCIDLib::TCard4     c4ResCode
            , const TString&            strResMsg
            ,       TTextMBufOutStream& strmTar
            ,       TCIDDataSrc&        cdsClient)
{
    strmTar.Reset();

    strmTar << L"HTTP/1.1 "
            << c4ResCode << L" "
            << strResMsg << kCIDLib::NewLn
            << THTTPClient::strHdr_Connection<< L": Close"
            << kCIDLib::NewLn
            << THTTPClient::strHdr_ContLen << L": 0"
            << kCIDLib::DNewLn;
    strmTar.Flush();

    // Write the data to the data source and flush it
    cdsClient.WriteBytes(strmTar.mbufData(), strmTar.c4CurSize());
    cdsClient.FlushOut(TTime::enctNowPlusMSs(2500));

    // Show what we just did
    TString strMsg(L"Sent ");
    strMsg.AppendFormatted(c4ResCode);
    strMsg.Append(L" error reply\n");
    ShowMsg(c4ThreadInd, strMsg);
}


//
//  We start up multiple threads on this method, which acts as a thread pool to
//  service clients.
//
tCIDLib::EExitCodes eServiceThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // Get out data out, which in our case is just a thread index for use in log msgs
    const tCIDLib::TCard4 c4ThreadInd = *reinterpret_cast<tCIDLib::TCard4*>(pData) + 1;

    // And now let our caller go
    thrThis.Sync();

    tCIDLib::TCard4     c4ContLen;
    tCIDLib::TCard4     c4ProtoVer;
    tCIDLib::TKVPList   colHdrLines;
    TFindBuf            fndbRes;
    THTTPClient         httpcSrv;
    THeapBuf            mbufData;
    TPathStr            pathRes;
    TString             strContType;
    TString             strType;
    TString             strURL;
    TString             strHdrLnVal;
    TTextStringOutStream strmMsg(2048UL);

    //
    //  Create memory based buffer that will format content out to UTF-8 format, since
    //  that's the format we'll report back to the client. Force the new line style
    //  to match that expected by HTTP.
    //
    TTextMBufOutStream  strmTar(4192, 4192, new TUTF8Converter);
    strmTar.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

    // Loop until asked to shut down
    while (kCIDLib::True)
    {
        // Check for shutdown requests
        if (thrThis.bCheckShutdownRequest())
            break;

        try
        {
            //
            //  Wait a bit for a client to serve, if nothing, try again. This provides
            //  our throttling so we don't run wild.
            //
            TServerStreamSocket* psockClient =  colSockQ.pobjGetNext(250, kCIDLib::False);
            if (!psockClient)
                continue;

            //
            //  Depending on whether we are secure not, create the correct type of
            //  data source.
            //
            TCIDDataSrc* pcdsClient = nullptr;
            if (bSecure)
            {
                pcdsClient = new TCIDSChanSrvDataSrc
                (
                    L"HTTPSrv 1"
                    , psockClient
                    , tCIDLib::EAdoptOpts::Adopt
                    , strCertInfo
                    , tCIDSChan::EConnOpts::None
                );
            }
             else
            {
                pcdsClient = new TCIDSockStreamDataSrc
                (
                    psockClient, tCIDLib::EAdoptOpts::Adopt
                );
            }

            //
            //  Put a janitor on the data source to get it cleaned up at the end. It
            //  will also call Init/Term methods on the data source. So, if secure, it
            //  will set up/tear down the secure connection.
            //
            TCIDDataSrcJan janSrc(pcdsClient, tCIDLib::EAdoptOpts::Adopt, kCIDLib::True);

            // Loop until the client closes (or after the first round if its 1.0 or an error)
            tCIDLib::TBoolean bClose = kCIDLib::False;
            while (!bClose)
            {
                //
                //  Wait for a msg from the client, for up to so many seconds. If we
                //  don't get it in that time, we will give up.
                //
                const tCIDNet::ENetPReadRes eRes = httpcSrv.eGetClientMsg
                (
                    *pcdsClient
                    , TTime::enctNowPlusSecs(c4WaitReq)
                    , strType
                    , colHdrLines
                    , strContType
                    , strURL
                    , c4ProtoVer
                    , mbufData
                    , c4ContLen
                );


                //
                //  And now decide how to respond. We have various error scenarios
                //  and one successful one where we give back the requested data.
                //
                if (eRes == tCIDNet::ENetPReadRes::Timeout)
                {
                    // Never got a good request from the client
                    ShowMsg(c4ThreadInd, L"Timed out waiting for a valid request from client\n");
                    bClose = kCIDLib::True;
                }
                 else if (eRes == tCIDNet::ENetPReadRes::Success)
                {
                    // Show the request info to the console
                    ShowReq(c4ThreadInd, strURL, colHdrLines);

                    //
                    //  If HTTP 1.0, or there's a connection close header from the client,
                    //  then we are going to close after this round.
                    //
                    if (c4ProtoVer == 0x0100)
                    {
                        bClose = kCIDLib::True;
                    }
                    else
                    {
                        if (TNetCoreParser::bFindHdrLine(colHdrLines, L"Connection", strHdrLnVal)
                        &&  strHdrLnVal.bCompareI(L"close"))
                        {
                            bClose = kCIDLib::True;
                        }
                    }

                    //
                    //  Parse the URL and get out the resource requested, expanded. We don't
                    //  have to get fully qualified URLs, but they must be relative to the
                    //  root if relative.
                    //
                    TString strResource;
                    TURL urlGot(strURL, tCIDSock::EQualified::DontCare);
                    TURL::ExpandTo
                    (
                        urlGot.strPath()
                        , strResource
                        , TURL::EExpTypes::Path
                        , tCIDLib::EAppendOver::Overwrite
                    );

                    // Replace the URL type slashes with local slashes
                    strResource.bReplaceChar(kCIDLib::chForwardSlash, kCIDLib::chPathSep);

                    //
                    //  Convert that to a file path relative to our root, and normalize it so
                    //  that we can catch any attempt to access up above the root directory,
                    //  though we don't both with that check here.
                    //
                    pathRes = strRootPath;
                    pathRes.AddLevel(strResource);
                    pathRes.Normalize();

                    // And now try to process it
                    if (!strType.bCompareI(kCIDNet::pszHTTP_GET))
                    {
                        // We only support GETs in this simple example
                        SendErrReply(c4ThreadInd, 505, L"Not implemented", strmTar, *pcdsClient);
                        bClose = kCIDLib::True;
                    }
                     else if (!pathRes.bStartsWithI(strRootPath))
                    {
                        // It normalized to something above the root
                        SendErrReply(c4ThreadInd, 404, L"Not Found", strmTar, *pcdsClient);
                        bClose = kCIDLib::True;
                    }
                     else if (!TFileSys::bExists(pathRes, fndbRes))
                    {
                        //
                        //  We couldn't find the requested resource. We don't close
                        //  the connection here, just send back an error reply. But
                        //  the client may close it.
                        //
                        SendErrReply(c4ThreadInd, 404, L"Not Found", strmTar, *pcdsClient);
                    }
                     else
                    {
                        // We got a seemingly reasonable request, so process it
                        ProcessReq
                        (
                            c4ThreadInd
                            , urlGot
                            , pathRes
                            , fndbRes
                            , colHdrLines
                            , strmTar
                            , *pcdsClient
                            , kCIDLib::False
                        );
                    }
                }
                 else
                {
                    // Some failure occurred while getting the request
                    strmMsg << L"Failed to get valid request from client. Reason="
                            << eRes << kCIDLib::NewLn << kCIDLib::FlushIt;
                    ShowMsg(c4ThreadInd, strmMsg.strData());
                    bClose = kCIDLib::True;
                }
            }

            ShowMsg(c4ThreadInd, L"Closing connection\n\n");
        }

        catch(TError& errToCatch)
        {
            TString strErr(L"Service exception", errToCatch.strErrText().c4Length());
            strErr += errToCatch.strErrText();
            ShowMsg(c4ThreadInd, strErr);
        }

        catch(...)
        {
            ShowMsg(c4ThreadInd, L"System exception in service thread");
        }
    }

    ShowMsg(c4ThreadInd, L"Shutting down...");
    return tCIDLib::EExitCodes::Normal;
}



// ----------------------------------------------------------------------------
//  Public methods
// ----------------------------------------------------------------------------

tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    using TThreadList = TRefVector<TThread>;
    TThreadList colThreads(tCIDLib::EAdoptOpts::Adopt);
    try
    {
        // Output a little program blurb
        strmOut << L"\nHTTPSrv1.Exe\n"
                << L"CIDLib Net Server Demo #1\n" << kCIDLib::EndLn;

        // We get from 2 to 4 parameters
        const tCIDLib::TCard4 c4PCount = TSysInfo::c4CmdLineParmCount();
        if ((c4PCount < 2) || (c4PCount > 4))
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Get the parms
        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        tCIDLib::TIPPortNum ippnListen = 0;
        {
            // First should be the root path
            TString strParm = *cursParms++;

            if (!TFileSys::bIsDirectory(strParm))
            {
                strmOut << L"Root path does not exist" << kCIDLib::NewEndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }

            // If it's not fully qualified, expand it
            TFileSys::QueryFullPath(strParm, strRootPath);

            // Make sure it doesn't have a trailing slash
            if (strRootPath.chLast() == kCIDLib::chPathSep)
                strRootPath.DeleteLast();

            // Next should be the port
            strParm = *cursParms++;
            if (!strParm.bToCard4(ippnListen, tCIDLib::ERadices::Dec))
            {
                    strmOut << L"Invalid port number'" << strParm << L"'"
                            << kCIDLib::NewEndLn;

                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }

            // The process the rest based on their prefix
            for (; cursParms; ++cursParms)
            {
                strParm = *cursParms;

                if (strParm.bStartsWithI(L"/CertInfo="))
                {
                    strParm.Cut(0, 10);
                    strCertInfo = strParm;
                    bSecure = kCIDLib::True;
                }
                 else if (strParm.bCompareI(L"/Single"))
                {
                    bSingleThread = kCIDLib::True;
                }
                 else
                {
                    strmOut << L"Unknown parameter '" << strParm << L"'"
                            << kCIDLib::NewEndLn;

                    ShowUsage();
                    return tCIDLib::EExitCodes::BadParameters;
                }
            }
        }

        //
        //  Start up our thread poll threads. We only start one if run in single
        //  thread mode, else a few more. We pass each one its respective index.
        //  The CIDLib facility has a helper to create unique thread names but we
        //  just do our own in this case.
        //
        {
            const tCIDLib::TCard4 c4Count = bSingleThread ? 1 : c4DefThreadCnt;
            TString strThreadName;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                strThreadName = L"HTTPSrv1_Thread";
                strThreadName.AppendFormatted(c4Index + 1);
                TThread* pthrNew = new TThread(strThreadName, eServiceThread);
                colThreads.Add(pthrNew);
                pthrNew->Start(&c4Index);
            }
        }

        //
        //  Create a listener, for our simple purposes, setting the max connections
        //  we will accept up to four for our simple test server.
        //
        TSocketListener socklSrv(ippnListen, tCIDSock::ESockProtos::TCP, 4);
        const tCIDLib::TEncodedTime enctWait(kCIDLib::enctOneSecond * 2);
        TIPEndPoint ipepClient;
        while (!thrThis.bCheckShutdownRequest())
        {
            // Wait a while for a connection
            TServerStreamSocket* psockSess = socklSrv.psockListenFor(enctWait, ipepClient);
            if (psockSess)
                colSockQ.Add(psockSess);
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        strmOut << L"A CIDLib runtime error occured during processing.\n  Error: "
                << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        strmOut << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    // In a real server we'd also error out any pending connections in the queue

    //
    //  Stop all of the worker threads. First just go through and do a non-syncing
    //  shutdown request, basically just sets the shutdown flag on the thread without
    //  waiting for it to respond to it.
    //
    //  Then go back and wait for them to die. This way we can stop them all in
    //  parallel.
    //
    const tCIDLib::TCard4 c4Count = bSingleThread ? 1 : c4DefThreadCnt;
    TThreadList::TNCCursor cursThreads(&colThreads);
    for (; cursThreads; ++cursThreads)
        cursThreads->ReqShutdownNoSync();

    cursThreads.bReset();
    for (; cursThreads; ++cursThreads)
    {
        try
        {
            cursThreads->eWaitForDeath(8000);
        }

        catch(const TError& errToCatch)
        {
            strmOut << L"Failed to stop thread " << cursThreads->strName()
                    << kCIDLib::EndLn
                    << errToCatch
                    << kCIDLib::EndLn;
        }
    }
    return tCIDLib::EExitCodes::Normal;
}

