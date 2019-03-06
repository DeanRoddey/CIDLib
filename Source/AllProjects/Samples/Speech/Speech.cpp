//
// FILE NAME: Speech.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/11/2010
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
//  This is the main module for a simple demonstration of the text to speech
//  capabilities of CIDLib. It just creates a speech object, and loops,
//  speaking the same text over and over with a slight pause between.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDLib, which is all we need.
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Local constant data
//
//  strToSay
//      This is the text that we speak.
// ----------------------------------------------------------------------------
static const TString    strToSay
(
    L"This is the Speech sample program. I may not be very smart, but I can "
    L"talk. That's not bad for a software program. My parents would be so "
    L"proud. They just did calculations."
);


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
CIDLib_MainModule(TThread(L"SpeechMainThread", eMainThreadFunc))



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
        TSpeech spchTest;

        conOut << L"Speech.exe - CIDLib TTS Sample Program\n"
                  L"   Press Ctrl-C to exit" << kCIDLib::EndLn;

        while (kCIDLib::True)
        {
            //
            //  Default if not indicated otherwise with the flag parms is
            //  do synchronous speaking, so this won't come back till it
            //  completes.
            //
            spchTest.Speak(strToSay,  tCIDLib::ESpeechFlags::None);

            // Sleep a bit and watch for shutdown request at the same time
            if (!thrThis.bSleep(2000))
                break;
        }
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

        conOut  << L"A CIDLib runtime error occured during processing.\n"
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
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
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}

