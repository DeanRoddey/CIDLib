//
// FILE NAME: TestLogSrv2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/12/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This is the second test of the log server system. The first one tests the
//  direct link to the log server. This one tests the more commonly used
//  'log server logger'. Its an implementation of the MLogger interface, which
//  in this case of course logs to the log server, and uses local fallback
//  files if the log server isn't available.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ----------------------------------------------------------------------------
//  Includes. We don't bother with a facility object or main header for this
//  little test. Its just this one file. So we directly include the CIDLib
//  headers we need.
// ----------------------------------------------------------------------------
#include    "CIDOrbUC.hpp"


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestLogSrvMainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Though we'd not normally need one for so small an app, we create a facility
//  object so that we can use our exe's path as the path for local fallback
//  files. We have no specialized needs, so we just use TFacility.
// ----------------------------------------------------------------------------
TFacility   facTestLogSrv2
(
    L"TestLogSrv2"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::None
);



// ---------------------------------------------------------------------------
//  TFacTestOrb: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        tCIDLib::TCard4     c4LoopCount = 2048;
        tCIDLib::TCard4     c4WaitTime = 10;
        TString             strParmVal;

        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        for (; cursParms; ++cursParms)
        {
            strParmVal = *cursParms;

            if (strParmVal.bStartsWith(L"/Count="))
            {
                strParmVal.Cut(0, 7);
                c4LoopCount = strParmVal.c4Val();
            }
             else if (strParmVal.bStartsWith(L"/Time="))
            {
                strParmVal.Cut(0, 6);
                c4WaitTime = strParmVal.c4Val();
            }
             else
            {
                TSysInfo::strmErr()
                        << L"Usage:\n"
                           L"    TestLogSrv [/Count=ccc /Time=ttt /NSAddr=xxx]\n\n"
                           L"      ccc = loop count\n"
                           L"      ttt = inter-loop pause time\n"
                        << kCIDLib::FlushIt;
                return tCIDLib::EExitCodes::BadParameters;
            }
        }

        // Crank up the Orb client support
        TSysInfo::strmOut() << L"Initializing Orb client support" << kCIDLib::EndLn;
        facCIDOrb().InitClient();

        // Install a log server logger. He adopts it
        TSysInfo::strmOut() << L"Installing new logger" << kCIDLib::EndLn;
        TLogSrvLogger* plgrTest = new TLogSrvLogger(facTestLogSrv2.strPath());
        TModule::InstallLogger(plgrTest);

        //
        //  And now log a bunch o' messages. Just log them as status messages,
        //  so that they will always be logged and won't throw.
        //
        TString strMsgToLog(L"This is a test message to log, dude #");
        const tCIDLib::TCard4 c4CapAt(strMsgToLog.c4Length());

        TSysInfo::strmOut() << L"Logging test messages" << kCIDLib::EndLn;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4LoopCount; c4Index++)
        {
            strMsgToLog.CapAt(c4CapAt);
            strMsgToLog.AppendFormatted(c4Index);

            facTestLogSrv2.LogMsg
            (
                CID_FILE
                , CID_LINE
                , strMsgToLog
                , L"This is some long aux text in order to test out the log monitor's resizing capabilities, which is important to test, because it could be important to test"
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );
            TThread::Sleep(c4WaitTime);
        }

        if (plgrTest->c4DroppedEvents())
        {
            TSysInfo::strmOut() << plgrTest->c4DroppedEvents()
                                << L" events were dropped" << kCIDLib::FlushIt;
        }

        // Close down the orb support
        TSysInfo::strmOut() << L"Terminating Orb support" << kCIDLib::EndLn;
        facCIDOrb().Terminate();
    }

    catch(const TError& errToCatch)
    {
        TSysInfo::strmErr() << L"A CIDLib Exception occured:\n"
                            << errToCatch << kCIDLib::FlushIt;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        TSysInfo::strmErr() << L"An unknown exception occured:\n" << kCIDLib::FlushIt;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

