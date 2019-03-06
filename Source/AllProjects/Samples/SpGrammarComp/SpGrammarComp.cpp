//
// FILE NAME: SpGrammarComp.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/3/2017
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
//  This is the main (only) module of the speech grammar compiler demo program. This
//  guy takes an input file which is the path to a .grxml grammar file and a target
//  file name to compile it to. This creates a binary format version of the grammar
//  that is faster to load and of course cannot be messed with by customers.
//
// CAVEATS/GOTCHAS:
//
//  1)  For whatever reason the grammar compiler needs a full path for the source
//      file. So we have to complete any partial path.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include the recognition engine, which will bring in all we need.
// ----------------------------------------------------------------------------
#include    "CIDSpReco.hpp"



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);



// ----------------------------------------------------------------------------
//  Local data
//
//  bVerbose
//      Driven by an optional command line parm. Defaults to True;
//
//  conOut
//      Cutput console for displaying info to the user.
// ----------------------------------------------------------------------------
static tCIDLib::TBoolean    bVerbose = kCIDLib::True;
static TOutConsole          conOut;



// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"SpGrammarCompMainThread", eMainThreadFunc))



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
        // We have to have 2 or 3 params
        const tCIDLib::TCard4 c4ParmCnt = TSysInfo::c4CmdLineParmCount();
        if ((c4ParmCnt < 2) || (c4ParmCnt > 3))
        {
            conOut  << L"\nUSAGE:\n"
                    << L"   SpGrammarComp srcfile tarfile [/silent]\n"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        TPathStr pathGrammar = TSysInfo::strCmdLineParmAt(0);
        TPathStr pathBin = TSysInfo::strCmdLineParmAt(1);

        // If we have another it is the verbose flag
        if (c4ParmCnt == 3)
        {
            TString strOpt = TSysInfo::strCmdLineParmAt(2);
            if (!strOpt.bCompareI(L"/Silent"))
            {
                conOut  << L"\nUSAGE:\n"
                        << L"   SpGrammarComp srcfile tarfile [/silent]\n"
                        << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::Normal;
            }

            bVerbose = kCIDLib::False;
        }

        // Output a little program blurb if in verbose mode
        if (bVerbose)
        {
            conOut << L"\nSpGrammarComp.exe\n"
                    << L"CIDLib Speech Grammar Compiler Demo\n" << kCIDLib::EndLn;
        }

        // If the source isn't fully qualified, expand it
        if (!pathGrammar.bIsFullyQualified())
        {
            TString strTmp(pathGrammar);
            TFileSys::QueryFullPath(strTmp, pathGrammar);
        }

        // Make sure it exists
        if (!TFileSys::bExists(pathGrammar, tCIDLib::EDirSearchFlags::NormalFiles))
        {
            conOut << L"The input file doesn't exist" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        if (bVerbose)
        {
            conOut  << L"Compiling: " << pathGrammar
                    << L"\nTo: " << pathBin
                    << kCIDLib::NewEndLn;
        }

        facCIDSpReco().CompileGrammarTo(pathGrammar, pathBin);
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

        conOut << L"A CIDLib runtime error occured during processing.\n  Error: "
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
        conOut << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}
