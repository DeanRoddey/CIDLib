//
// FILE NAME: LogStressTest.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/19/2019
//
// COPYRIGHT: 2019..2019, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  This file implements a 'test' though not a formal test framework test. This
//  one is more for development testing. It will just log a fairly high rate of
//  events to the CIDLib Log Server from multiple threads so as to test the
//  logging infrastructure on this side, the log server on that side, and the
//  ancillary bits like the log monitor.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $Log$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDOrbUC.hpp"


// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThread
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);
CIDLib_MainModule(TThread(L"LogStressTestMainThread", eMainThread))


// ----------------------------------------------------------------------------
//  Global data
// ----------------------------------------------------------------------------
TOutConsole conOut;

static const tCIDLib::TCard4    c4LoopCnt = 1024;
static const tCIDLib::TCard4    c4PerLoopPause = 100;
static const tCIDLib::TCard4    c4PerThreadOfs = 10;
static const tCIDLib::TCard4    c4ThreadCnt = 12;
static const tCIDLib::TCh*      aszMsgs[c4ThreadCnt] =
{
    L"At least count there were %(1) rabbits running wild in Central Park"
    , L"Finally runner #%(1) crossed the finish line, though it was the wrong finished line."
    , L"An infinite number of jelly donuts is widely considered to be superior to %(1) jelly donuts"
    , L"After he had broken it %(1) times, Bob figured that it was probably not going to work, so he just broke it again"
    , L"This is #%(1)"
    , L"The last %(1) guitars played the last note of the epic last concert of The Dudes"
    , L"%(1) is not the loneliest number, it's the number somewhere above the loneliest number, or so the story goes"
    , L"If an infinite number of monkeys in a room will eventually create Beethoven's sonatas, can %(1) create Party in the USA?"
    , L"%(1) is less than infinity, unless you are looking at it backwards"
    , L"I've had %(1) girlfriends in my life, I just forgot to inform them of their status"
    , L"%(1) diamonds are better than an equal number of lumps of coal, unless you have a very strong vice"
    , L"%(1) years later, Bob finally realized that it was not years later, and he was distraught"
};



// ----------------------------------------------------------------------------
//  A global thread function that we'll start up our test threads on
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eLogThreadFunc(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // Get our index out then let the caller go
    const tCIDLib::TCard4 c4ThrIndex = *reinterpret_cast<tCIDLib::TCard4*>(pData);
    thrThis.Sync();

    TString strFile(L"File_Number");
    strFile.AppendFormatted(c4ThrIndex + 1);
    TString strMsg;
    try
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4LoopCnt; c4Index++)
        {
            strMsg = aszMsgs[c4ThrIndex];
            strMsg.eReplaceToken(c4Index + 1, kCIDLib::chDigit1);
            facCIDLib().LogMsg
            (
                strFile
                , CID_LINE + c4ThrIndex
                , strMsg
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );

            if (!thrThis.bSleep(c4PerLoopPause + (c4ThrIndex * c4PerThreadOfs)))
                break;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
    }

    return tCIDLib::EExitCodes::Normal;
}


// ----------------------------------------------------------------------------
//  Program entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Initialize the client side ORB
        facCIDOrb().InitClient();

        //
        //  Get our results directory to use as the fallback local log file
        //  directory.
        //
        TString strLocalLogDir;
        if (!TProcEnvironment::bFind(L"CID_RESDIR", strLocalLogDir))
        {
            conOut  << L"Could not find CID_RESDIR in the environment"
                    << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::NotFound;
        }

        // And install a log server logger
        TModule::InstallLogger
        (
            new TLogSrvLogger(strLocalLogDir), tCIDLib::EAdoptOpts::Adopt
        );

        //
        //  Start up a number of threads to do the logging.
        //
        TRefVector<TThread> colThreads(tCIDLib::EAdoptOpts::Adopt, c4ThreadCnt);
        TString strName;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
        {
            strName = L"LogStressThread-";
            strName.AppendFormatted(c4Index + 1);

            TThread* pthrNew = new TThread(strName, eLogThreadFunc);
            colThreads.Add(pthrNew);
            pthrNew->Start(&c4Index);
        }

        // Worst case wait time
        const tCIDLib::TCard4 c4WaitTime
        (
            c4LoopCnt * (c4PerLoopPause + (c4ThreadCnt * c4PerThreadOfs))
        );
        TThread::Sleep(c4WaitTime);

        // Now wait for them to die
        colThreads.bForEachNC
        (
            [c4WaitTime](TThread& thrCur)
            {
                tCIDLib::EExitCodes eCode;
                if (!thrCur.bWaitForDeath(eCode, 4096))
                {
                    conOut  << L"Thread " << thrCur.strName() << L" did not die"
                            << kCIDLib::NewEndLn;
                }
                return kCIDLib::True;
            }
        );

        //
        //  Check the log related stats. Those things can't really log if something
        //  goes wrong because it can cause circular freakouts. But we can keep some
        //  stats.
        //
        conOut  << L"\nLogging Stats:\n-------------------\n"
                << L"   Dropped Events: "
                << TStatsCache::c8CheckValue(kCIDLib::pszStat_AppInfo_DroppedLogEvs)
                << L"\n       Exceptions: "
                << TStatsCache::c8CheckValue(kCIDLib::pszStat_AppInfo_LogErrors)
                << kCIDLib::NewEndLn;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
        return tCIDLib::EExitCodes::InitFailed;
    }

    catch(...)
    {
        return tCIDLib::EExitCodes::InitFailed;
    }

    return tCIDLib::EExitCodes::Normal;
}
