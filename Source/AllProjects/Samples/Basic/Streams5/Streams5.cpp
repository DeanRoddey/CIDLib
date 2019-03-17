//
// FILE NAME: Streams5.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/1997
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
//  This is the main module for the fifth of the streaming oriented demo
//  programs. This one just demonstrates some basic text stream input
//  mechanisms. It just outputs some prompts to ask the user to enter some
//  values and then gets the input and displays whether the input was
//  correct or not.
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
//  Local data
//
//  conIn
//      This is an input console object used to get user input.
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
// ----------------------------------------------------------------------------
static TInConsole   conIn;
static TOutConsole  conOut;



// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"Streams5MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  A template function that is used to get all the various types of values.
//  It does the standard work of prompting and checking and displaying results
//
//  RETURN: A dummy return value to get around an error in VC++. Get rid
//              of this and every invocation will use the type of the last
//              invocation!!
//
template <class T> T GetAValue(const TString& strLegal, const TString& strType)
{
    T Val;

    // Ask the user to enter a value value
    try
    {
        conOut  << L"Enter a " << strType << L" value ("
                << strLegal << L"): " << kCIDLib::FlushIt;
        conIn   >> Val;
        conOut  << L"You entered: " << Val << kCIDLib::NewLn << kCIDLib::EndLn;
    }

    catch(const TError& errToCatch)
    {
        conOut  << L"Value was invalid. Reason: "
                << errToCatch.strErrText()
                << kCIDLib::NewLn << kCIDLib::EndLn;
    }
    return Val;
}


//
//  This function is called from the main thread to do the work of the
//  program.
//
static tCIDLib::TVoid DoWork()
{
    // Ask the user to enter a boolean value
    GetAValue<tCIDLib::TBoolean>(L"True/False", L"TBoolean");

    // Ask the user to enter a TCard1 value
    GetAValue<tCIDLib::TCard1>(L"0..255", L"TCard1");

    // Ask the user to enter a TCard2 value
    GetAValue<tCIDLib::TCard2>(L"0..65535", L"TCard2");

    // Ask the user to enter a TCard4 value
    GetAValue<tCIDLib::TCard4>(L"0..4294967295", L"TCard4");

    // Ask the user to enter a TInt1 value
    GetAValue<tCIDLib::TInt1>(L"-128..127", L"TInt1");

    // Ask the user to enter a TInt2 value
    GetAValue<tCIDLib::TInt2>(L"-32768..32767", L"TInt2");

    // Ask the user to enter a TInt4 value
    GetAValue<tCIDLib::TInt4>(L"-2147483648..2147483647", L"TInt4");

    // Ask the user to enter a TFloat4 value
    GetAValue<tCIDLib::TFloat8>
    (
        L"1.175494351e-38F..3.402823466e+38F"
        , L"TFloat4"
    );

    // Ask the user to enter a TFloat8 value
    GetAValue<tCIDLib::TFloat8>
    (
        L"2.22507385e-308 .. 1.7976931e+308"
        , L"TFloat8"
    );
}


//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all unexpected exceptions cleanly and report them. So put the
    //  whole thing in a try.
    //
    try
    {
        DoWork();
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
    //  Kernel errors should never propogate out of CIDLib.Dll, but I test
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
        conOut  << L"A general exception occured during processing\n"
                << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}
