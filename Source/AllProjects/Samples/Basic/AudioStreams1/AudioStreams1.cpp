//
// FILE NAME: AudioStreams1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/05/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the main module for the first of the first of some audio streaming
//  related samples. This one just iterates the input and output WAVE style
//  streaming devices available and outputs their names and ids.
//
// CAVEATS/GOTCHAS:
//
//  1)  This simple program does not attempt to be language independent.
//
//  2)  This program also does not create a facility object of its own because
//      its so simple that it has no need for one.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDAudStream.hpp"


// ----------------------------------------------------------------------------
//  Local data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
// ----------------------------------------------------------------------------
static TOutConsole  conOut;


// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"AudioStreams1MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This is the the thread function for the main thread.
//
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
        TCIDAudioStreamList colStreams;

        // Do the input devices. Tell it not to throw if none found
        if (TCIDWaveInStream::bEnumWaveInDevs(colStreams, kCIDLib::False))
        {
            conOut  << L"\nAvailable WAVE Input Streams\n"
                    << L"-------------------------------------\n";

            for (auto cursSteams = colStreams.cursThis(); cursSteams; ++cursSteams)
            {
                conOut  << L"  " << cursSteams->strName()
                        << L"  ID=" << cursSteams->strId()
                        << L"\n";
            }
        }

        // And do teh same for the output streams
        conOut.Flush();
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        conOut  << L"A CIDLib runtime error occured during processing.\n"
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\n  Error="
                << kerrToCatch.errcId() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}

