//
// FILE NAME: HTTPClient1.cpp
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
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDEncode.hpp"
#include    "CIDNet.hpp"


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

//
//  Once the HTTP operation is done below, this is called to dump out the info
//  for examimation. We get the body content, the output headers and the content
//  type info. We parse the content type to see if the body is a text format and
//  to set up a text converter to convert it for display.
//
static tCIDLib::TVoid
DumpInfo(const  tCIDLib::TKVPList&  colOutHdrs
        , const TMemBuf&            mbufCont
        , const tCIDLib::TCard4     c4ContLen
        , const TString&            strContType)
{
    //
    //  If we got header lines, dump those. They are in the form of a
    //  vector of key/value pair strings.
    //
    if (!colOutHdrs.bIsEmpty())
    {
        strmOut << L"Header Lines:\n---------------------\n";
        colOutHdrs.bForEach
        (
            [&](const TKeyValuePair& kvalCur)
            {
                strmOut << L"    " << kvalCur.strKey() << L'=' << kvalCur.strValue()
                        << kCIDLib::NewLn;
                return kCIDLib::True;
            }
        );
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
    if (c4ContLen)
    {
        //
        //  Try to figure out the encoding. If there's not one indicated, then try
        //  the default.
        //
        TString strEncoding;
        if (!TNetCoreParser::bFindTextEncoding(colOutHdrs, mbufCont, c4ContLen, strEncoding))
            strEncoding = kCIDNet::pszDefHTTPEncoding;

        //
        //  It is text, so see if we have a transcoder for this encoding in our
        //  CIDEncode facility. If so, we will display the text.
        //
        if (facCIDEncode().bSupportsEncoding(strEncoding))
        {
            // Show the content type
            strmOut << L"\n\nBody Text\n-------------------------" << kCIDLib::NewEndLn;

            //
            //  Create a text converter for the encoding, then create a memory based
            //  input stream over the body content and give it the text converter to
            //  use.
            //
            TTextConverter* ptcvtBody = facCIDEncode().ptcvtMakeNew(strEncoding);
            ptcvtBody->eErrorAction(tCIDLib::ETCvtActions::StopThenThrow);
            TTextMBufInStream strmSrc
            (
                &mbufCont, c4ContLen, tCIDLib::EAdoptOpts::NoAdopt, ptcvtBody
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


static tCIDLib::TVoid ShowUsage(TTextOutStream& strmOut)
{
    strmOut << L"Usage:\n"
                L"   HTTPClient1 /URL=url [/FMT=[MIMETYPE] /Close]\n\n"
                L"   /Close means send a close header line\n"
                L"   /FMT= indicates an 'accept' MIME type, else it accepts text\n\n"
                L"   The URL must be fully qualified!\n"
            << kCIDLib::FlushIt;
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

        //
        //  Some of these we have to set via parms, and some we may override
        //  via parms.
        //
        tCIDLib::TBoolean bClose = kCIDLib::False;
        TString strRetFmt = L"text/html, text/plain, *.*";
        TString strURLText;
        TString strReqType = L"GET";

        //
        //  Get the paramters and set up the URL. It should be a fully
        //  qualified URL. These all fit the standard form for CIDLib parameters,
        //  so we can call a little helper that will pre-parse them for us. It
        //  uses the key/value/flag type, where the flag is true if was an
        //  'option' parameter i.e. just /XXX, and false if a value parameter, i.e.
        //  /XXX=YYY
        //
        tCIDLib::TKVPFList colParms;
        const tCIDLib::TCard4 c4ParmCnt = TSysInfo::c4StdCmdLineParse(colParms);
        if ((c4ParmCnt < 1) || (c4ParmCnt > 3))
        {
            ShowUsage(strmOut);
            return tCIDLib::EExitCodes::BadParameters;
        }

        // We have the right number, so iterate them and store the info oawa
        const tCIDLib::TBoolean bBadParm = !colParms.bForEach
        (
            [&](const TKeyValFPair& kvalfCur)
            {
                tCIDLib::TBoolean bGood = kCIDLib::True;
                if (kvalfCur.strKey().bCompareI(L"Close"))
                {
                    bClose = kCIDLib::True;
                }
                 else if (kvalfCur.strKey().bCompareI(L"FMT"))
                {
                    if (kvalfCur.bFlag())
                        strRetFmt = kvalfCur.strValue();
                    else
                        bGood = kCIDLib::False;
                }
                 else if (kvalfCur.strKey().bCompareI(L"URL"))
                {
                    if (kvalfCur.bFlag())
                        strURLText = kvalfCur.strValue();
                    else
                        bGood = kCIDLib::False;
                }
                 else
                {
                    bGood = kCIDLib::False;
                }
                return bGood;
            }
        );

        if (bBadParm || strURLText.bIsEmpty())
        {
            ShowUsage(strmOut);
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Parse the URL, which must be fully qualified
        TURL urlGet(strURLText, tCIDSock::EQualified::Full);

        // We only support HTTP and HTTPS
        if ((urlGet.eProto() != tCIDSock::EProtos::HTTP)
        &&  (urlGet.eProto() != tCIDSock::EProtos::HTTPS))
        {
            strmOut << L"This program only supports HTTP/HTTPS access"
                    << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Some stuff that will be given back to us from the query
        tCIDLib::TCard4     c4ContLen = 0;
        tCIDLib::TKVPList   colInHdrs;
        tCIDLib::TKVPList   colOutHdrs;
        tCIDNet::EHTTPCodes eCodeType;
        TString             strContType;
        TString             strRepLine;
        THeapBuf            mbufContent(kCIDLib::c4Sz_32K);
        TString             strUltimateURL;
        THTTPClient         httpcGet;

        // If asked to send a close, add a header line for that
        if (bClose)
            colInHdrs.objAdd(TKeyValuePair(L"Connection", L"close"));

        // We'll wait up to 8 seconds for the operation to complete
        const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusSecs(8);

        //
        //  Do the GET operation. We pass a null data source, which means this
        //  will create a one shot data source for us (for secure or non-secure
        //  comms depending on the protocol.)
        //
        tCIDLib::TCard4 c4Res = httpcGet.c4SendGet
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
            , mbufContent
            , c4ContLen
            , kCIDLib::False
            , colInHdrs
        );

        //
        //  Dump the results out, starting with the ultimate URL, in case
        //  we were redirected, the overall status code and the status
        //  reply line.
        ///
        strmOut << L"  Reply Code=" << c4Res << kCIDLib::NewLn
                << L"  Reply Text=" << strRepLine << kCIDLib::DNewLn;

        if (c4ContLen)
            DumpInfo(colOutHdrs, mbufContent, c4ContLen, strContType);
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        strmOut << L"A CIDLib runtime error occured during processing.\n  Error: "
                << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }
    return tCIDLib::EExitCodes::Normal;
}

