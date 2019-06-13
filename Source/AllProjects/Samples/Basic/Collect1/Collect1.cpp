//
// FILE NAME: Collect1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/24/1997
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
//  This is the main module for the first of the collection oriented demo
//  programs. This one just demonstrates the most basic collection, the
//  bag. It just adds some items to bag, then iterates and displays them.
//
//  As with most of these basic samples, it doesn't have a facility object,
//  it just starts the main thread on a local function.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Collect1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // A console for test output
    TOutConsole  conOut;

    //
    //  Add some strings to a bag o' strings. There's a standard alias for this
    //  type, so we use that.
    //
    tCIDLib::TStrBag colOfStrs;
    TString strVal;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        colOfStrs.objAdd(strVal);
    }

    // Get a cursor for the bag and iterate, displaying each string
    TBag<TString>::TCursor cursFiles(&colOfStrs);
    for (; cursFiles; ++cursFiles)
        conOut << *cursFiles << kCIDLib::NewLn;
    conOut << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}
