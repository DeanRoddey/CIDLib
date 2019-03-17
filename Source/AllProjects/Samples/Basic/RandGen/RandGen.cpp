//
// FILE NAME: RandGen.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/21/2005
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
//  This is the main module for a simple program that generates a set of random
//  numbers in a given range and generates a set of unique ids.
//
//
//  As with most of these basic samples, this guy does not create a facility
//  object, it just starts up a thread on a local function.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDMath.hpp"
#include    "CIDCrypto.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"RandGenMainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
TOutConsole conOut;



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TCard4 c4GenNum(        TRandomNum&     randOut
                                , const tCIDLib::TCard4 c4MinVal
                                , const tCIDLib::TCard4 c4MaxVal)
{
    tCIDLib::TCard4 c4Ret = 0;
    while (kCIDLib::True)
    {
        // Generate a number
        c4Ret = randOut.c4GetNextNum();

        // Shift it down a few bits
        c4Ret >>= 3;

        // And take the low 16 bits
        c4Ret &= 0xFFFF;

        // If it's within the min/max range, take it
        if ((c4Ret >= c4MinVal) && (c4Ret <= c4MaxVal))
            break;
    }
    return c4Ret;
}


//
//  Shows the parameter usage for the program.
//
static tCIDLib::TVoid ShowUsage()
{
    conOut << kCIDLib::NewLn
             << L"Usage: RandGen minval maxval" << kCIDLib::DNewLn
             << L"          minval - Minimum value to generate\n"
             << L"          maxval - Maximum value to generate\n"
             << L"       Both are unsigned values, so no negative numbers\n"
             << kCIDLib::EndLn;
}


//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Display the program blurb
    conOut << kCIDLib::NewLn
             << L"RandGen.Exe, Version: "
             << facCIDLib().strVersion() << kCIDLib::NewLn
             << L"CIDLib Random Number Generator"
             << kCIDLib::NewLn << kCIDLib::EndLn;

    //
    //  Since this is a demo and testing program, we'd like to catch
    //  all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        const tCIDLib::TCard4 c4ArgCount = TSysInfo::c4CmdLineParmCount();
        if (c4ArgCount != 2)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  Set up a random number generator. Generate a simple seed from the
        //  current millis.
        //
        TRandomNum randOut;
        randOut.Seed(TTime::c4Millis());

        // Get the two parms out
        const tCIDLib::TCard4 c4MinVal
                        = TSysInfo::strCmdLineParmAt(0).c4Val(tCIDLib::ERadices::Dec);
        const tCIDLib::TCard4 c4MaxVal
                        = TSysInfo::strCmdLineParmAt(1).c4Val(tCIDLib::ERadices::Dec);

        // Generate the random numbers
        conOut << L"Generating random numbers:\n-----------------------" << kCIDLib::EndLn;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 64; c4Index++)
        {
            tCIDLib::TCard4 c4Cur = c4GenNum(randOut, c4MinVal, c4MaxVal);
            conOut << c4Cur << kCIDLib::chSpace;
        }
        conOut << kCIDLib::DNewLn;

        // Generate the unique ids
        TString strUID;
        conOut << L"Generating unique ids:\n-----------------------" << kCIDLib::EndLn;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 24; c4Index++)
        {
            TUniqueId::MakeId(strUID);
            conOut << strUID << kCIDLib::NewLn;
        }
        conOut << kCIDLib::NewEndLn;
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut <<  L"A CIDLib runtime error occured during processing. "
                 <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::NewLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"A kernel error occured during processing.\n  Error="
                 << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing"
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}

