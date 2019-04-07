//
// FILE NAME: JSON1.cpp
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
//  This is the main (only) module of the JSON.cpp demo program. This
//  program demonstrates the JSON parser classes.
//
//  It takes a JSON file to parse, parses it, and then outputs in two ways. One
//  is via regular hierarchy iteration, which outputs valid JSON back out. The
//  other is via the callback mechanism, which passes back each node along with
//  a pseudo path you can use to look for nodes you are interested in.
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



// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"JSON1MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
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
    try
    {
        // Output a little program blurb
        TSysInfo::strmOut() << L"\nJSON1.Exe\n"
                            << L"CIDLib JSON Demo #1\n"
                            << L"    JSON1 [src file]"
                            << kCIDLib::NewEndLn;


        TString strSrcFile = TSysInfo::strCmdLineParmAt(0);
        TTextFileInStream strmFile
        (
            strSrcFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_Read
            , new TUSASCIIConverter
        );

        // Do a parse and format the output
        TSysInfo::strmOut() << L"Formatted output\n---------------------\n";
        {
            TJSONParser jprsTest;
            TJSONValue* pjprsnRoot = jprsTest.pjprsnParse(strmFile);
            TJanitor<TJSONValue> janRoot(pjprsnRoot);

            // Format it out to the output
            pjprsnRoot->FormatToText(TSysInfo::strmOut(), 0, 3);
        }
        TSysInfo::strmOut() << kCIDLib::NewEndLn;

        TSysInfo::strmOut() << L"Callback output\n---------------------\n";
        {
            strmFile.Reset();
            TJSONParser jprsTest;
            TTextOutStream& strmTar = TSysInfo::strmOut();
            jprsTest.bParse
            (
                strmFile
                , [&strmTar](const TString& strPath
                            , const TString& strValue
                            , const tCIDNet::EJSONVTypes eType) -> tCIDLib::TBoolean
                {
                    strmTar << kCIDLib::chOpenBracket
                            << tCIDNet::strXlatEJSONVTypes(eType)
                            << L", "
                            << strPath
                            << L"] - "
                            << strValue
                            << kCIDLib::NewLn;
                    return kCIDLib::True;
                }
            );
            TSysInfo::strmOut() << kCIDLib::NewEndLn;
        }


        //
        //  Enable memory check in debug mode, then take a snap shot and
        //  run the test again and clean up. Then dump anything still
        //  on the heap.
        //
        #if CID_DEBUG_ON
        TKrnlMemCheck kmchkTest;
        kmchkTest.ReportToFile(L".\\dump.txt");
        kmchkTest.TakeSnapshot();

        {
            TJSONParser jprsTest;
            TJSONValue* pjprsnRoot = jprsTest.pjprsnParse(strmFile);
            TJanitor<TJSONValue> janRoot(pjprsnRoot);
        }

        kmchkTest.DumpLeaks();
        #endif
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        TSysInfo::strmOut()
                << L"A CIDLib runtime error occured during processing.\n  Error: "
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
        TSysInfo::strmOut()
                << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        TSysInfo::strmOut()
                << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

