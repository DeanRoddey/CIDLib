//
// FILE NAME: Streams6.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/1997
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
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
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

    //
    //  Create a stream format object for the number colum, and one for
    //  the line text.
    //
    TStreamFmt  strmfNum(4, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
    TStreamFmt  strmfText(0, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

    // Get quick refs to the redirectable input/output consoles
    TTextInStream& strmIn = TSysInfo::strmIn();
    TTextOutStream& strmOut = TSysInfo::strmOut();

    //
    //  Just read until the end of input, processing each line. Let any
    //  exceptions just propogate back to the caller.
    //
    TString strInput;
    tCIDLib::TCard4 c4Line(1);
    while (!strmIn.bEndOfStream())
    {
        strmIn.c4GetLine(strInput);
        strmOut << strmfNum << c4Line++
                << strmfText << L". " << strInput << kCIDLib::NewLn;
    }

    strmOut.Flush();
    return tCIDLib::EExitCodes::Normal;
}


