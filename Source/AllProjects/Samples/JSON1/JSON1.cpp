//
// FILE NAME: JSON1.cpp
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
//  This is the main (only) module of the JSON.cpp demo program. This
//  program demonstrates the JSON parser classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own.
// ----------------------------------------------------------------------------
#include    "CIDEncode.hpp"

#include    "CIDNet.hpp"



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);



// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
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
                            << L"    JSON1 [src file]n"
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


        TSysInfo::strmOut() << L"Iteration output\n---------------------\n";
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
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

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

