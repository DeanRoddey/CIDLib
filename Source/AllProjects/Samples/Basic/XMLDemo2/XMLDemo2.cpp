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
//  Include magic main module code to start the main thread
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
               L"          /MemTest\n"
               L"          /Validate\n"
               L"          /IgnoreDTD\n\n"
               L" If no output file, output is to the console"
            << kCIDLib::NewEndLn;
}


//
//  This is called to do the actual parse. If it works, then the passed
//  tree parser is filled in. We get the input file name for error msg
//  dipslay purposes. We get an output stream to dump the resulting
//  XML content back out to.
//
static tCIDLib::TVoid DoParse(          TXMLTreeParser&     xtprsToUse
                                , const TString&            strFile
                                , const tCIDXML::EParseOpts eOpts
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
                                        , eOpts
                                        , tCIDXML::EParseFlags::All))
        {
            strmOut << xtprsToUse.xtdocThis() << kCIDLib::DNewLn;
        }
         else
        {
            // XML errors occurred, display the first one
            const TXMLTreeParser::TErrInfo& errCur = xtprsToUse.erriFirst();
            conOut  << L"The parse failed\n"
                    << errCur.strText()
                    << kCIDLib::NewLn << L"(" << errCur.c4Line()
                    << kCIDLib::chPeriod << errCur.c4Column()
                    << L") " << errCur.strSystemId() << kCIDLib::NewEndLn;
        }
    }

    catch(TError& errToCatch)
    {
        conOut  << L"A CIDLib runtime error occured during parsing.\n"
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewEndLn;
    }
}


// ----------------------------------------------------------------------------
//  The main thread entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLDemo2Thread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    tCIDLib::TBoolean   bParmsOK = kCIDLib::True;
    tCIDXML::EParseOpts eOptsToUse = tCIDXML::EParseOpts::None;
    TString             strInFileParm;
    TString             strOutFileParm;
    TString             strEncoding(L"UTF-8");
    {
        //
        //  Do a standard CIDLib style command line parse. These should all be option
        //  type parameters (start with /, which means the flag should be set on all
        //  of them.)
        //
        tCIDLib::TKVPFList colParms;
        TSysInfo::c4StdCmdLineParse(colParms);
        tCIDLib::TKVPFList::TCursor cursParms = tCIDLib::TKVPFList::TCursor(&colParms);
        for (; cursParms && bParmsOK; ++cursParms)
        {
            const TKeyValFPair& kvalfCur = *cursParms;
            if (!kvalfCur.bFlag())
            {
                // Wasn't option style, so can't be good
                bParmsOK = kCIDLib::False;
                break;
            }

            if (kvalfCur.strKey().bCompareI(L"InFile"))
                strInFileParm = kvalfCur.strValue();
            else if (kvalfCur.strKey().bCompareI(L"OutFile"))
                strOutFileParm = kvalfCur.strValue();
            else if (kvalfCur.strKey().bCompareI(L"IgnoreBadChars"))
                eOptsToUse |= tCIDXML::EParseOpts::IgnoreBadChars;
            else if (kvalfCur.strKey().bCompareI(L"IgnoreDTD"))
                eOptsToUse |= tCIDXML::EParseOpts::IgnoreDTD;
            else if (kvalfCur.strKey().bCompareI(L"Validate"))
                eOptsToUse |= tCIDXML::EParseOpts::Validate;
            else
            {
                conOut << L"\nUnknown parameter '" << kvalfCur.strKey()
                         << L"'" << kCIDLib::NewEndLn;
                bParmsOK = kCIDLib::False;
            }
        }

        // If no file name or bad parms in general, then missing something
        if (strInFileParm.bIsEmpty() || !bParmsOK)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    // See if we can create the requested encoding
    TTextConverter* ptcvtOut = facCIDEncode().ptcvtMake(strEncoding);
    if (!ptcvtOut)
    {
        conOut  << L"Could not create a converter for encoding: " << strEncoding
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }

    //
    //  Decide where to send the output. If they provided an output file, then
    //  let's create a stream for that. Else we use the console. The file output
    //  stream adopts our converter even if we don't end up using it.
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

    // Run the test, dumping to the selected target
    TXMLTreeParser xtprsToUse;
    DoParse(xtprsToUse, strInFileParm, eOptsToUse, *pstrmOut);

    return tCIDLib::EExitCodes::Normal;
}
