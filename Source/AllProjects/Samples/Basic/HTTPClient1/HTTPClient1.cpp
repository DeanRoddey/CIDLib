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
//  GET request on a provided URL and dumping out the returned information
//  if it is text format.
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
static tCIDLib::TVoid ShowUsage(TTextOutStream& strmOut)
{
    strmOut << L"Usage:\n"
                L"   HTTPClient1 url [/Fmt=[MIMETYPE] /Close]\n\n"
                L"   /Fmt= indicates an 'accept' MIME type, else it accepts text\n"
                L"   /URL= indicates a fully qualified URL to GET\n"
            << kCIDLib::FlushIt;
}


// ----------------------------------------------------------------------------
//  Program entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Output a little program blurb
    strmOut << L"\nHTTPClient1.Exe\n"
            << L"CIDLib HTTP Client Demo #1\n" << kCIDLib::EndLn;

    // Input values, which will be set/overridden by parms
    TString strRetFmt;
    TString strURLText;

    //
    //  Get the parameters and set up the URL. Tell it remove consumed params
    //  so that we can check for unknown ones.
    //
    {
        TCmdLine cmdlLoad;
        cmdlLoad.bRemoveConsumed(kCIDLib::True);
        if ((cmdlLoad.c4ParmCount() < 1) || (cmdlLoad.c4ParmCount() > 2))
        {
            ShowUsage(strmOut);
            return tCIDLib::EExitCodes::BadParameters;
        }

        // The 0th one is a value parameter that contains the URL
        if (!cmdlLoad.bValueAt(0, strURLText))
        {
            ShowUsage(strmOut);
            return tCIDLib::EExitCodes::BadParameters;
        }

        // If no format set, default it
        if (!cmdlLoad.bFindOptionVal(L"Fmt", strRetFmt))
             strRetFmt = L"text/*";

        // If any left, they are unknown ones
        if (!cmdlLoad.bIsEmpty())
        {
            ShowUsage(strmOut);
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    //
    //  Parse the URL, which must be fully qualified. To avoid an exception
    //  we don't force the issue during parse but check it after.
    //
    TURL urlGet(strURLText, tCIDSock::EQualified::DontCare);
    if (!urlGet.bFullyQualified())
    {
        strmOut << L"The URL must be fully qualified!"
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }

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
    THeapBuf            mbufCont(kCIDLib::c4Sz_32K);
    TString             strUltimateURL;
    THTTPClient         httpcGet;

    //
    //  We have to create a data source in order to use the redirect aware
    //  get, because it requires creating multiple connections potentially.
    //  We create a regular socket based one or a secure one, depending on
    //  the URL protocol. For simple source creation there's a helper that
    //  will do this for us
    //
    TCIDDataSrcJan janSrc
    (
        facCIDNet().pcdsMakeSocketSrc(L"HTTP1 Client", urlGet)
        , tCIDLib::EAdoptOpts::Adopt
        , kCIDLib::True
    );

    // Do the GET operation, handling redirection
    TString strFinalURL;
    tCIDLib::TCard4 c4Res = httpcGet.c4SendGetRedir
    (
        janSrc
        , urlGet
        , TTime::enctNowPlusSecs(5)
        , L"CIDLib HTTP Client 1/1.0"
        , strRetFmt
        , eCodeType
        , strRepLine
        , colOutHdrs
        , strContType
        , mbufCont
        , c4ContLen
        , strFinalURL
        , colInHdrs
    );

    //
    //  Dump the results out, starting with the ultimate URL, in case
    //  we were redirected, the overall status code and the status
    //  reply line.
    ///
    strmOut << L"  Reply Code: " << c4Res << kCIDLib::NewLn
            << L"  Reply Text: " << strRepLine << kCIDLib::NewLn
            << L" Content Len: " << c4ContLen << kCIDLib::NewLn;

    if (strFinalURL != strURLText)
        strmOut << L"   Final URL: " << strFinalURL << kCIDLib::NewLn;

    strmOut << kCIDLib::NewLn;


    // If we got header lines, dump those
    if (!colOutHdrs.bIsEmpty())
    {
        strmOut << L"Header Lines:\n---------------------\n";
        colOutHdrs.bForEach
        (
            [](const TKeyValuePair& kvalCur) -> tCIDLib::TBoolean
            {
                strmOut << L"    " << kvalCur.strKey() << L": "
                        << kvalCur.strValue() << kCIDLib::NewLn;
                return kCIDLib::True;
            }
        );
        strmOut << kCIDLib::NewLn;
    }

    //
    //  If we have content, see if there is an explicit or discoverable text
    //  encoding. If so, see if we support that encoding. If so, then let's
    //  dump it out as text.
    //
    TString strEncoding;
    if (c4ContLen
    &&  TNetCoreParser::bFindTextEncoding(colOutHdrs, mbufCont, c4ContLen, strEncoding)
    &&  facCIDEncode().bSupportsEncoding(strEncoding))
    {
        strmOut << L"\n\nBody Text\n-------------------------" << kCIDLib::NewEndLn;

        //
        //  Create a text input stream over the buffer, giving it a text encoder
        //  for the encoding we found. He adopts it.
        //
        TTextMBufInStream strmSrc
        (
            tCIDLib::ForceMove(mbufCont)
            , c4ContLen
            , facCIDEncode().ptcvtMake(strEncoding, tCIDLib::ETCvtActs::StopThenThrow)
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
        strmOut << L"Body content is a binary or unknown text encoding"
                << kCIDLib::NewEndLn;

        TBinFileOutStream strmTar
        (
            L"Test.jpg"
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );

        strmTar.c4WriteBuffer(mbufCont, c4ContLen);
        strmTar.Flush();

    }

    return tCIDLib::EExitCodes::Normal;
}

