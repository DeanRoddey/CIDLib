//
// FILE NAME: HTTPClient1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the main (only) module of the HTTPClient1 demo program. This
//  program demonstrates the THTTPClient class, by just using it to send a
//  GET request and just dumping out the returned information. It does a get
//  of a URL provided on the command line.
//
//  As with most of these basic samples, this guy does not create a facility
//  object, it just starts a thread on a local function.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes.
// ----------------------------------------------------------------------------
#include    "CIDEncode.hpp"
#include    "CIDNet.hpp"
#include    "CIDSChan.hpp"



// ----------------------------------------------------------------------------
//  Local data
//
//  strmOut
//      We don't need any interactivity for this program so we just set up
//      an output text stream, which writes to the standard output. There are
//      other ways to get this, which some of the other samples do. This is
//      is another.
// ----------------------------------------------------------------------------
static TTextFileOutStream strmOut(tCIDLib::EStdFiles::StdOut);



// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"HTTPClient1MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

// Dumps out the info we get back from the HTTP operation
static tCIDLib::TVoid
DumpInfo(const  tCIDLib::TKVPList&  colOutHdrs
        , const TMemBuf&            mbufContent
        , const tCIDLib::TCard4     c4ContLen
        , const TString&            strContType
        , const TString             strReqType)
{
    //
    //  If we got header lines, dump those. They are in the form of a
    //  vector of key/value pair strings.
    //
    if (!colOutHdrs.bIsEmpty())
    {
        strmOut << L"Header Lines:\n---------------------\n";
        const tCIDLib::TCard4 c4Count = colOutHdrs.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TKeyValuePair& kvalCur = colOutHdrs[c4Index];

            //
            //  Handle the cache control header specially and break out
            //  the values. Otherwise, just dump the key=value. The HTTP
            //  client class has a helper to prase the cache control info
            //  line, also giving us back a set of key/value strings.
            //
            if (kvalCur.strKey().bCompareI(THTTPClient::strHdr_CacheControl))
            {
                strmOut << L"    Cache-Control:\n";
                tCIDLib::TKVPList colCacheVals;
                THTTPClient::ParseCacheControl(kvalCur.strValue(), colCacheVals);

                const tCIDLib::TCard4 c4Count = colCacheVals.c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    const TKeyValuePair& kvalCur = colCacheVals[c4Index];
                    strmOut << L"       " << kvalCur.strKey();
                    if (!kvalCur.strValue().bIsEmpty())
                        strmOut << L"=" << kvalCur.strValue();
                    strmOut << kCIDLib::NewLn;
                }
            }
             else
            {
                strmOut << L"    " << kvalCur.strKey() << L'=' << kvalCur.strValue()
                        << kCIDLib::NewLn;
            }
        }
        strmOut << kCIDLib::NewLn;
    }

    //
    //  If there was body content, then let's display some info about
    //  that. There may be none if there was an error. If it's text
    //  content we'll dump the actual text out.
    //
    strmOut << L"Derived Info:\n\n---------------------\n"
            << L"    Content Len=" << c4ContLen << kCIDLib::NewLn;

    // If we have content, try to dump it
    if (c4ContLen && (strReqType != L"HEAD"))
    {
        //
        //  Try to figure out the encoding. If there's not one indicated, then try the
        //  default.
        //
        TString strEncoding;
        if (!TNetCoreParser::bFindTextEncoding(colOutHdrs, mbufContent, c4ContLen, strEncoding))
            strEncoding = kCIDNet::pszDefHTTPEncoding;

        //
        //  It is text, so see if we have a transcoder for this
        //  encoding in our CIDEncode facility.
        //
        if (facCIDEncode().bSupportsEncoding(strEncoding))
        {
            // Show the content type
            strmOut << L"\n\nBody Text\n-------------------------" << kCIDLib::NewEndLn;

            //
            //  We do so create a text encoder and set that on a text input stream
            //  we create over the body content, so that we can stream it in. We
            //  got the size of the body content in the GET, and provide that as the
            //  end of stream point.
            //
            //  We tell it not to adopt the memory buffer of course,  though it always
            //  adopts the text converter, so we don't need to clean that up.
            //
            //  To help finding text encoding errors, we tell the text encoder to stop
            //  parsing if it hits an error, and return what it got. That way, we'll
            //  dump out text up to the error point, instead of stopping way before that
            //  (since the stream pre-streams in chunks at a time.)
            //
            TTextConverter* ptcvtBody = facCIDEncode().ptcvtMakeNew(strEncoding);
            ptcvtBody->eErrorAction(tCIDLib::ETCvtActions::StopThenThrow);
            TTextMBufInStream strmSrc
            (
                &mbufContent, c4ContLen, tCIDLib::EAdoptOpts::NoAdopt, ptcvtBody
            );

            // OK, just read and dump lines until we hit the end of the body content.
            TString strLine;
            while (!strmSrc.bEndOfStream())
            {
                strmSrc >> strLine;
                strmOut << strLine << kCIDLib::NewLn;
            }
            strmOut << kCIDLib::EndLn;
        }
         else
        {
            strmOut << L"The content type text encoding is not supported"
                    << kCIDLib::NewEndLn;
        }
    }
     else
    {
        strmOut << L"Body is a binary or uknown text type" << kCIDLib::NewEndLn;
    }

    strmOut << kCIDLib::NewEndLn;
}



tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Output a little program blurb
        strmOut << L"\nHTTPClient1.Exe\n"
                << L"CIDLib Net Demo #1\n" << kCIDLib::EndLn;

        // Get the URL parameter
        if ((TSysInfo::c4CmdLineParmCount() < 1)
        ||  (TSysInfo::c4CmdLineParmCount() > 3))
        {
            strmOut << L"Usage:\n"
                       L"   HTTPClient1 url [/HEAD /FMT=[MIMETYPE] /Close /POST]\n\n"
                       L" The URL must be fully qualified!\n"
                       L" Use /HEAD option to only return headers\n"
                    << kCIDLib::FlushIt;
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  Get the paramter and set up the URL. It should be a fully
        //  qualified URL.
        //
        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        tCIDLib::TBoolean bClose = kCIDLib::False;
        TString strRetFmt = L"text/html, text/plain, *.*";
        TString strURLText;
        TString strReqType = L"GET";

        // The URL must be the first parm always. So get it and parse it
        strURLText = *cursParms++;
        TURL urlGet(strURLText, tCIDSock::EQualified::Full);

        // We only support HTTP and HTTPS
        if ((urlGet.eProto() != tCIDSock::EProtos::HTTP)
        &&  (urlGet.eProto() != tCIDSock::EProtos::HTTPS))
        {
            strmOut << L"This program only supports HTTP/HTTPS access"
                    << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        // If any optional parms get those
        for (; cursParms; ++cursParms)
        {
            TString strOption = *cursParms;

            if (strOption.bCompareI(L"/Close"))
            {
                bClose = kCIDLib::True;
            }
             else if (strOption.bStartsWithI(L"/FMT="))
            {
                strOption.Cut(0, 5);
                strRetFmt = strOption;
            }
             else if (strOption.bCompareI(L"/HEAD"))
            {
                strReqType = L"HEAD";
            }
             else if (strOption.bCompareI(L"/POST"))
            {
                strReqType = L"POST";
            }
             else
            {
                strmOut << L"Usage:\n"
                           L"   HTTPClient1 url [/HEAD /Close /Fmt]\n\n"
                           L" The URL must be fully qualified!\n"
                           L" Use /HEAD option to only return headers\n"
                        << kCIDLib::FlushIt;
                return tCIDLib::EExitCodes::BadParameters;
            }
        }

        // Some stuff that will be given back to us
        tCIDLib::TCard4     c4ContLen;
        tCIDLib::TKVPList   colInHdrs;
        tCIDLib::TKVPList   colOutHdrs;
        tCIDNet::EHTTPCodes eCodeType;
        TString             strContType;
        TString             strRepLine;
        THeapBuf            mbufContent(8, 0x800000, 32 * 1024);
        TString             strUltimateURL;
        THTTPClient         httpcGet;

        //
        //  Do the get. Use the version that supoprts redirection. We'll get back
        //  the HTTP status code as the return. We set an end time and keep passing
        //  that back in. If we run out of time at any of the rounds, we give up.
        //
        tCIDLib::TCard4 c4Res;
        const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusSecs(8);
        if (strReqType == L"HEAD")
        {
            // Let the client object do a one-shot connection
            c4Res = httpcGet.c4SendHead
            (
                nullptr
                , urlGet
                , enctEnd
                , L"CIDLib Net1 Sample/1.0"
                , strRetFmt
                , eCodeType
                , strRepLine
                , colOutHdrs
                , strContType
                , c4ContLen
                , colInHdrs
            );
        }
         else if (strReqType == L"GET")
        {
            //
            //  In our case we could have just passed in a null data source and the
            //  HTTP client would have created one in a one-shot sort of way for us.
            //  But we want to demonstrate how you create one if you want to do multiple
            //  calls on a single connection.
            //
            //  Based on the protocol, we create a regular socket based data source or
            //  a secure channel based one.
            //
            TCIDDataSrc* pcdsSrv = nullptr;
            if (urlGet.eProto() == tCIDSock::EProtos::HTTP)
            {
                pcdsSrv = new TCIDSockStreamDataSrc(urlGet.ipepHost());
            }
             else
            {
                //
                //  Create a socket and and then a secure channel data source that adopts it.
                //  Give it the host to use as the security principal.
                //
                TClientStreamSocket* psockSrc = new TClientStreamSocket
                (
                    tCIDSock::ESockProtos::TCP, urlGet.ipepHost()
                );

                tCIDLib::TStrList colALPN;
                pcdsSrv = new TCIDSChanClDataSrc
                (
                    psockSrc, tCIDLib::EAdoptOpts::Adopt, urlGet.strHost(), colALPN
                );
            }

            //
            //  Put the data source into a janitor, and let it adopt the source. Tell it
            //  to do the shutdown sequence when it cleans up.
            //
            TCIDDataSrcJan janSrc(pcdsSrv, tCIDLib::EAdoptOpts::Adopt, kCIDLib::True);

            // If asked to send a close, add that
            if (bClose)
                colInHdrs.objAdd(TKeyValuePair(L"Connection", L"close"));

            c4Res = httpcGet.c4SendGetRedir
            (
                janSrc
                , urlGet
                , enctEnd
                , L"CIDLib Net1 Sample/1.0"
                , strRetFmt
                , eCodeType
                , strRepLine
                , colOutHdrs
                , strContType
                , mbufContent
                , c4ContLen
                , strUltimateURL
                , colInHdrs
            );
        }

        //
        //  Dump the results out, starting with the ultimate URL, in case
        //  we were redirected, the overall status code and the status
        //  reply line.
        ///
        strmOut << L"Ultimate URL=" << strUltimateURL << kCIDLib::NewLn
                << L"  Reply Code=" << c4Res << kCIDLib::NewLn
                << L"  Reply Text=" << strRepLine << kCIDLib::DNewLn;

        if (c4ContLen)
            DumpInfo(colOutHdrs, mbufContent, c4ContLen, strContType, strReqType);
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

    return tCIDLib::EExitCodes::Normal;
}

