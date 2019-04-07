//
// FILE NAME: SpReco1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2000
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
//  This is the main (only) module of the SpReco1demo program. This guy creates an
//  instance of the speech recognition engine and starts it. Then it loads a grammar
//  and starts pumping recognition events from the engine, displaying the resulting
//  events for demo purposes.
//
//  So it takes a single parameter, which is the path to the grammar file to load.
//
//  Use the SpGrammarComp program to compile a grammar file to use with this program.
//  There is a simple example Solitaire grammar (from Microsoft) in this same directory
//  that you can compile and play with. Read the grammar file for a description of
//  the utterances involved.
//
//
//  Like most of the basic samples, this one doesn't create a facility object, it
//  just starts a main thread on a local
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
#include    "CIDSpReco.hpp"



// ----------------------------------------------------------------------------
//  Local data
//
//  conIn
//  conOut
//      Input and output consoles. We only need the input to get an enter key
//      from the user to end the program.
//
//  strGrammarPath
//      We get a command line parameter with the path to the grammar file to load.
// ----------------------------------------------------------------------------
static TInConsole   conIn;
static TOutConsole  conOut;
static TString      strGrammarPath;


// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread.
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"SpReco1MainThread", eMainThreadFunc))



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
        conOut << L"\nSpReco1.exe\n"
                << L"CIDLib Speech Recognition Demo #1\n" << kCIDLib::EndLn;

        // Get the path to the grammar to load
        if (TSysInfo::c4CmdLineParmCount() != 1)
        {
            conOut  << L"\n  You must provide a path to a grammar to load"
                    << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // It wants a fully qualified path, so expand what we got
        TFileSys::QueryFullPath(TSysInfo::strCmdLineParmAt(0), strGrammarPath);

        //
        //  Create the speech recognition engine and initialize it. Let it create a
        //  default audio source by passing null.
        //
        TCIDSpReco sprecoTest(L"SpRecoTest1");
        sprecoTest.Initialize(nullptr);

        // Load the grammar we were told to load. It's not dynamic so false 2nd parm
        sprecoTest.LoadGrammarFromFile(strGrammarPath, kCIDLib::False);

        //
        //  And complete the grammar loading process to let the engine know it can
        //  do any final manipulation and start up.
        //
        sprecoTest.GrammarLoadingDone();

        // Loop and process events until the user stops us by hitting a key
        TCIDSpeechRecoEv sprecevCur;
        tCIDAudStream::EStrmStates eState;
        while (kCIDLib::True)
        {
            if (sprecoTest.bGetNextRecEvent(sprecevCur, 250, eState))
            {
                conOut << sprecevCur << kCIDLib::NewEndLn;
            }

            tCIDLib::EConKeys eKeyType;
            if (conIn.chReadChar(eKeyType, 10))
                break;
        }

        // And terminate the reco engine
        sprecoTest.Terminate();
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
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
