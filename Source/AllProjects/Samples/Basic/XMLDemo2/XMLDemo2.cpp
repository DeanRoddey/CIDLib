//
// FILE NAME: XMLDemo2.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/21/2000
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
//  This is the main module for the second of the XML demo programs. This one
//  shows how to use the simple tree parser. This parse deals with the details
//  of interacting with the XML parser, and builds up a simple, read only
//  tree structure which represents the document parsed.
//
//  The simple tree parser is intended for that large number of applications
//  which need to get a fast and light and read-only representation of the
//  document into memory, in order to poke around in it and use that data to
//  drive the program in some way. This is very commonly required for XML
//  based configuration files and such.
//
// CAVEATS/GOTCHAS:
//
//  1)  This program is very simple so it does not attempt to be language
//      independent and it does not provide its own facility object since
//      it does not need one.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDEncode.hpp"
#include    "CIDXML.hpp"


// ----------------------------------------------------------------------------
//  For this simple example, just start the main thread on a global function.
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLDemo2Thread(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"XMLDemo2Thread", eXMLDemo2Thread))


// ----------------------------------------------------------------------------
//  Local data
//
//  conOut
//      The parsed XML output can go to this, or to an output file. Otherwise this
//      is used for program output, usage msg, error msg.
// ----------------------------------------------------------------------------
TOutConsole conOut;


// ----------------------------------------------------------------------------
//  Local helper functions
// ----------------------------------------------------------------------------

// Just shows a usage banner if we get bad parms
static tCIDLib::TVoid ShowUsage()
{
    conOut  << L"Usage: XMLDemo2 [options]\n"
               L"       Options:\n"
               L"          /InFile=xxx\n"
               L"          /OutFile=xxx\n"
               L"          /IgnoreBadChars\n"
               L"          /Encoding=xxx\n"
               L"          /Validate\n"
               L"          /IgnoreDTD\n"
               L"          /IgnoreLTSpace\n\n"
               L"   If no output file, output is to the console\n"
               L"   Encoding is only used if outputing to a file\n\n"
               L"   IgnoreLTSpace ignores leading/trailing white space\n"
               L"   in text nodes for output, which is often useful.\n\n"
            << kCIDLib::NewEndLn;
}


//
//  This is called to do the actual parse. If it works, then the passed
//  tree parser is filled in. We get the input file name for error msg
//  display purposes. We get an output stream to dump the resulting
//  XML content back out to.
//
static tCIDLib::TVoid DoParse(          TXMLTreeParser&     xtprsToUse
                                , const TString&            strFile
                                , const tCIDXML::EParseOpts eParseOpts
                                , const tCIDXML::EPrintOpts ePrintOpts
                                ,       TTextOutStream&     strmOut)
{
    try
    {
        //
        //  Do the XML parse using the tree parser. Tell the tree to store all
        //  XML content. Often you will just want to store elements and character
        //  data if the purpose is to process the contents for your own purposes.
        //  here we are writing it back out so we want it all.
        //
        if (xtprsToUse.bParseRootEntity(strFile
                                        , eParseOpts
                                        , tCIDXML::EParseFlags::All))
        {
            //  It worked, so format it back out to the output passed output stream,
            xtprsToUse.xtdocThis().PrintTo(strmOut, 0, tCIDXML::EPrintFmts::Pretty, ePrintOpts);
            strmOut << kCIDLib::NewEndLn;
        }
         else
        {
            // XML errors occurred, display the first one
            const TXMLTreeParser::TErrInfo& errFirst = xtprsToUse.erriFirst();
            conOut.FormatF
            (
                L"\nThe parse failed\n    %(1)\n    (%(2).%(3)) %(4)\n"
                , errFirst.strText()
                , errFirst.c4Line()
                , errFirst.c4Column()
                , errFirst.strSystemId()
            );
        }
    }

    catch(TError& errToCatch)
    {
        conOut.FormatF
        (
            L"A CIDLib runtime error occured during parsing.\n    Error: %(1)\n\n"
            , errToCatch.strErrText()
        );
    }
}


// ----------------------------------------------------------------------------
//  The main thread entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLDemo2Thread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Parse our parameters, some are defaulted
    tCIDLib::TBoolean   bParmsOK = kCIDLib::True;
    tCIDXML::EPrintOpts ePrintOpts = tCIDXML::EPrintOpts::Escape;
    tCIDXML::EParseOpts eParseOpts = tCIDXML::EParseOpts::None;
    TString             strInFileParm;
    TString             strOutFileParm;
    TString             strEncoding;
    {
        TCmdLine cmdlLoad;

        // Remove consumed parms so we can check for unknown ones at the end
        cmdlLoad.bRemoveConsumed(kCIDLib::True);

        // The input file is required
        if (!cmdlLoad.bFindOptionVal(L"InFile", strInFileParm))
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Values that are optional or defaulted if not set
        cmdlLoad.bFindOptionVal(L"OutFile", strOutFileParm);
        if (!cmdlLoad.bFindOptionVal(L"Encoding", strEncoding))
            strEncoding = L"UTF-8";

        //
        //  These are just option flags. Note we are OR'ing class type enums
        //  since our enums are IDL generated and support that.
        //
        if (cmdlLoad.bFindOption(L"IgnoreBadChars"))
            eParseOpts |= tCIDXML::EParseOpts::IgnoreBadChars;
        if (cmdlLoad.bFindOption(L"IgnoreDTD"))
            eParseOpts |= tCIDXML::EParseOpts::IgnoreDTD;
        if (cmdlLoad.bFindOption(L"Validate"))
            eParseOpts |= tCIDXML::EParseOpts::Validate;
        if (cmdlLoad.bFindOption(L"IgnoreLTSpace"))
            ePrintOpts |= tCIDXML::EPrintOpts::StripLTSpace;

        // If any left, they are unknown ones
        if (!cmdlLoad.bIsEmpty())
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    // See if we can create the requested encoding
    TTextConverter* ptcvtOut = facCIDEncode().ptcvtMake(strEncoding);
    if (!ptcvtOut)
    {
        conOut.FormatF
        (
            L"Could not create a converter for encoding: %(1)\n\n", strEncoding
        );
        return tCIDLib::EExitCodes::BadParameters;
    }

    //
    //  Decide where to send the output. If they provided an output file, then
    //  let's create a file stream for that. Else we use the console. The file
    //  output stream adopts our text converter even if we don't end up using it.
    //
    TTextOutStream*     pstrmOut = &conOut;
    TTextFileOutStream  strmOutFile(ptcvtOut);
    if (!strOutFileParm.bIsEmpty())
    {
        strmOutFile.Open
        (
            strOutFileParm
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SafeStream
            , tCIDLib::EAccessModes::Excl_Write
        );
        pstrmOut = &strmOutFile;
    }

    // And do the parse and output, passing along options and target stream
    TXMLTreeParser xtprsToUse;
    DoParse(xtprsToUse, strInFileParm, eParseOpts, ePrintOpts, *pstrmOut);

    return tCIDLib::EExitCodes::Normal;
}
