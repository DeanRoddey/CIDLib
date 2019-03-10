//
// FILE NAME: XMLDemo2.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/21/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#include    "CIDXML.hpp"



// ----------------------------------------------------------------------------
//  Include magic main module code to start the main thread
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLDemo2Thread(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"XMLDemo2Thread", eXMLDemo2Thread))


// ----------------------------------------------------------------------------
//  Local data
//
//  uptrOut
//      The output stream. If its redirected, we create a file stream with a
//      UTF-8 converter. Otherwise, its just a console.
// ----------------------------------------------------------------------------
using   TStreamPtr = TUniquePtr<TTextOutStream>;
static  TStreamPtr  uptrOut;



// ----------------------------------------------------------------------------
//  Local helper methods
// ----------------------------------------------------------------------------

// Just shows a usage banner if we get bad parms
static tCIDLib::TVoid ShowUsage()
{
    *uptrOut << L"Usage: XMLDemo2 [options]\n"
                L"       Options:\n"
                L"          /File=xxx\n"
                L"          /IgnoreBadChars\n"
                L"          /MemTest\n"
                L"          /Validate\n"
            << kCIDLib::EndLn;
}


// ----------------------------------------------------------------------------
//  Local helper functions
// ----------------------------------------------------------------------------

//
//  This is called to do the actual parse. If it works, then the passed
//  tree parser is filled in.
//
static tCIDLib::TVoid DoParse(          TXMLTreeParser&     xtprsToUse
                                , const TString&            strFile
                                , const tCIDXML::EParseOpts eOpts
                                , const tCIDLib::TBoolean   bDump)
{
    try
    {
        // And now parse the file
        if (xtprsToUse.bParseRootEntity(strFile
                                        , eOpts
                                        , tCIDXML::EParseFlags::All))
        {
            // Print the document out if asked to
            if (bDump)
                *uptrOut << xtprsToUse.xtdocThis() << kCIDLib::DNewLn;
        }
         else
        {
            const TXMLTreeParser::TErrInfo& errCur = xtprsToUse.colErrors()[0];
            *uptrOut   << L"The parse failed\n"
                       << errCur.strText()
                       << kCIDLib::NewLn << L"(" << errCur.c4Line()
                       << kCIDLib::chPeriod << errCur.c4Column()
                       << L") " << errCur.strSystemId() << kCIDLib::NewEndLn;
        }

        // Flush the output stream to force all output out
        uptrOut->Flush();
    }

    catch(TError& errToCatch)
    {
        *uptrOut    << L"A CIDLib runtime error occured during parsing.\n"
                    << L"Error: " << errToCatch.strErrText() << kCIDLib::NewEndLn;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        *uptrOut << L"A kernel error occured during parsing.\nError="
                 << kerrToCatch.errcId() << kCIDLib::NewEndLn;
    }

    // Catch a general exception
    catch(...)
    {
        *uptrOut << L"A general exception occured during parsing"
                 << kCIDLib::NewEndLn;
    }
}


// ----------------------------------------------------------------------------
//  The main thread entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLDemo2Thread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Create our standard output stream
    if (TFileSys::bIsRedirected(tCIDLib::EStdFiles::StdOut))
    {
        uptrOut = TStreamPtr
        (
            new TTextFileOutStream
            (
                tCIDLib::EStdFiles::StdOut, new TUTFConverter(TUTFConverter::EEncodings::UTF8)
            )
        );
    }
     else
    {
        uptrOut = TStreamPtr(new TOutConsole);
    }

    tCIDLib::TBoolean   bMemTest = kCIDLib::False;
    tCIDXML::EParseOpts eOptsToUse = tCIDXML::EParseOpts::None;
    TString             strFileParm;
    try
    {
        // Get the command line parms
        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        TString strParm;
        for (; cursParms; ++cursParms)
        {
            strParm = *cursParms;

            if (strParm.bStartsWithI(L"/File="))
            {
                strParm.Cut(0, 6);
                strFileParm = strParm;
            }
             else if (strParm.bCompareI(L"/IgnoreBadChars"))
            {
                eOptsToUse = tCIDLib::eOREnumBits
                (
                    eOptsToUse, tCIDXML::EParseOpts::IgnoreBadChars
                );
            }
             else if (strParm.bCompareI(L"/IgnoreDTD"))
            {
                eOptsToUse = tCIDLib::eOREnumBits
                (
                    eOptsToUse, tCIDXML::EParseOpts::IgnoreDTD
                );
            }
             else if (strParm.bCompareI(L"/Validate"))
            {
                eOptsToUse = tCIDLib::eOREnumBits
                (
                    eOptsToUse, tCIDXML::EParseOpts::Validate
                );
            }
             else if (strParm.bCompareI(L"/MemTest"))
            {
                bMemTest = kCIDLib::True;
            }
             else
            {
                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }
        }

        if (strFileParm.bIsEmpty())
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        *uptrOut << L"A CIDLib runtime error occured during init.\n"
                 << L"Error: " << errToCatch.strErrText()
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        *uptrOut   << L"A kernel error occured during init.\nError="
                   << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        *uptrOut   << L"A general exception occured during init"
                   << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    // Run the test. Ask it to dump out the results
    TXMLTreeParser xtprsToUse;
    DoParse(xtprsToUse, strFileParm, eOptsToUse, kCIDLib::True);

    //
    //  Debug memory leaks by running it again with a memory check around it.
    //  The first one will fault in any statics, and this one should only involve
    //  stuff allocated and freed during the parse. It'll create a memory dump
    //  in the current directory.
    //
    #if CID_DEBUG_ON
    #define DUMP_MEM 0
    #if DUMP_MEM
    if (bMemTest)
    {
        xtprsToUse.Reset();
        TKrnlMemCheck kmchkTest;
        kmchkTest.ReportToFile(L"MemDump_XMLDemo2.Txt");
        kmchkTest.TakeSnapshot();
        {
            DoParse(xtprsToUse, strFileParm, eOptsToUse, kCIDLib::False);
            DoParse(xtprsToUse, strFileParm, eOptsToUse, kCIDLib::False);
        }
        xtprsToUse.Reset();
        kmchkTest.DumpSnapDiffs();
    }
    #endif
    #endif

    return tCIDLib::EExitCodes::Normal;
}


