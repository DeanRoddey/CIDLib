//
// FILE NAME: CIDLib_TexTStdErrLogger.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/18/2001
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
//  This file implements a std err logger, which sends logged errors to
//  the standard error stream.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TStdErrLogger,TObject)


// ---------------------------------------------------------------------------
//  TStdErrLogger: Constructors and operators
// ---------------------------------------------------------------------------
TStdErrLogger::TStdErrLogger()
{
}

TStdErrLogger::~TStdErrLogger()
{
}


// ---------------------------------------------------------------------------
//  TStdErrLogger: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TStdErrLogger::LogEvent(const TLogEvent& logevToLog)
{
    //
    //  Create some static stuff for us in logging. We know we can do this
    //  because calls to this method are thread serialized by the CIDLib
    //  code that calls it. This lets us avoid the creation on every call.
    //
    static tCIDLib::TBoolean    bFirstTime = kCIDLib::True;
    static TTime                tmCurrent;

    //
    //  If this is the first time through, then we need to set the default
    //  format for the time object so that it will naturally format in the
    //  style we want. It will be in the form:
    //
    //  MM:DD HH:MM:SS
    //
    //  In a 24 hour format.
    //
    if (bFirstTime)
    {
        tmCurrent.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");
        bFirstTime = kCIDLib::False;
    }

    // Tell the time object to the time in the event
    tmCurrent = logevToLog.enctLogged();

    TTextOutStream& strmOut = TSysInfo::strmErr();

    // Stream out the basic info about who/what/where
    tmCurrent = logevToLog.enctLogged();
    strmOut << tmCurrent << L"-"
            << logevToLog.strProcess() << L": "
            << logevToLog.strFacName() << L", "
            << logevToLog.strThread() << L", "
            << logevToLog.strFileName() << L"."
            << logevToLog.c4LineNum() << L", "
            << logevToLog.eSeverity() << L"/"
            << logevToLog.eClass();

    // Only do the error stuff if there are any non-zero codes
    if (logevToLog.bAnyError())
    {
        strmOut << L", " << facCIDLib().strMsg(kCIDMsgs::midGen_Error)
                << L":" << logevToLog.errcId()
                << L"/" << logevToLog.errcKrnlId()
                << L"/" << logevToLog.errcHostId();
    }
    strmOut << L"\n{\n";

    // Now append the error text itself, indenting it over
    strmOut << L"   " << logevToLog.strErrText();

    // If there is aux text, log it next and indent it also.
    if (logevToLog.bHasAuxText())
        strmOut << L"\n   " << logevToLog.strAuxText();

    // Close the brace, add new line, and flush the stream
    strmOut << L"\n}\n" << kCIDLib::FlushIt;
}


