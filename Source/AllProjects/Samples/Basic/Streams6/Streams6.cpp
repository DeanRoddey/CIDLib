//
// FILE NAME: Streams6.cpp
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
//  This is the main module for the first of the streaming oriented demo
//  programs. This one just demonstrates the basics of text streaming.
//
//  This reads the lines in a file from the input stream, and dumps them
//  to the output stream, but numbered. So basically it is a filter that
//  creates a numbered version of a text file.
//
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
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid* pData);
CIDLib_MainModule(TThread(L"Streams6MainThread", eMainThreadFunc))



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

    // Get quick refs to the redirectable input/output consoles
    TTextInStream& strmIn = TSysInfo::strmIn();
    TTextOutStream& strmOut = TSysInfo::strmOut();

    //
    //  Just read until the end of input, processing each line. We output
    //  the line number, right justified in a 4 character field, then a period,
    //  then the line text.
    //
    //  We use the simple Format() method for this, since it makes it easy. It
    //  takes a format string and variable parameters. The token numbers are not
    //  actually used in this case, they are positional relative to the provided
    //  tokens, but they have to have a number, so 1, 2, 3 is the obvious thing.
    //
    TString strInput;
    tCIDLib::TCard4 c4Line(1);
    while (!strmIn.bEndOfStream())
    {
        strmIn.c4GetLine(strInput);
        strmOut.Format(L"%(1,4). %(2)\n", c4Line++, strInput);
    }

    strmOut.Flush();
    return tCIDLib::EExitCodes::Normal;
}
