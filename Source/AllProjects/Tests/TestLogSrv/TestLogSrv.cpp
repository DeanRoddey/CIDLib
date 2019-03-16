//
// FILE NAME: TestLogSrv.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/03/2001
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
//  This is a simple test program for the log server. It uses the Orb services
//  to find the log server's logging object binding, creates a log server
//  client proxy, and logs messages to it.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ----------------------------------------------------------------------------
//  Includes. We don't bother with a facility object or main header for this
//  little test. So just bring in what we need
// ----------------------------------------------------------------------------
#include    "CIDMath.hpp"

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
//  Global data
// ----------------------------------------------------------------------------
static TEvent*          pevStart;
static TMutex*          pmtxOut;
static TTextOutStream*  pstrmOut;


// ---------------------------------------------------------------------------
//  A list of messages that each thread will randomly select from, to keep the
//  data from being completely identical. This will make it more likely to
//  cause compaction and expansion. We just read our own text file and make
//  each line a potential log msg.
// ---------------------------------------------------------------------------
static TVector<TString> colMsgs;



// ---------------------------------------------------------------------------
//  TFacTestOrb: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eTestThreadFunc(TThread& thrThis, tCIDLib::TVoid* pData)
{
    thrThis.Sync();

    // Wait for the signal to go
    pevStart->WaitFor(2000);

    // Create a random number generator. Seed it with our thread id
    TRandomNum randMsg;
    randMsg.Seed(thrThis.tidThis());

    // Get the max message index out for more efficient access
    const tCIDLib::TCard4 c4MaxMsg = colMsgs.c4ElemCount() - 1;

    tCIDLib::TBoolean       bHaveClient = kCIDLib::False;
    tCIDLib::TCard4         c4Counter = 1;
    tCIDLib::ESeverities    eSev = tCIDLib::ESeverities::Min;
    tCIDOrbUC::TLSrvProxy   orbcLS;
    try
    {
        while (kCIDLib::True)
        {
            if (thrThis.bCheckShutdownRequest())
                break;

            //
            //  If we don't have a log server client, then try to get one.
            //  Else, try to log messages.
            //
            if (!bHaveClient)
            {
                try
                {
                    orbcLS = facCIDOrbUC().orbcLogSrvProxy();
                    bHaveClient = kCIDLib::True;

                    TMtxLocker lockOut(pmtxOut);
                    *pstrmOut << thrThis.strName() << L" got a logger proxy"
                              << kCIDLib::EndLn;
                }

                catch(...)
                {
                    *pstrmOut << thrThis.strName()
                              << L" failed to get a logger proxy"
                              << kCIDLib::EndLn;
                    TThread::Sleep(1000);
                }
            }
             else
            {
                try
                {
                    // Find a message to send
                    tCIDLib::TCard4 c4MsgInd = randMsg.c4GetNextNum() % c4MaxMsg;

                    TError errToLog
                    (
                        L"TestFacility"
                        , CID_FILE
                        , CID_LINE
                        , colMsgs[c4MsgInd]
                        , eSev
                        , tCIDLib::EErrClasses::Internal
                    );
                    errToLog.SetLogged();

                    //
                    //  Go back and fiddle with the time a bit, so that we
                    //  get a lot of out of time order data in the log file.
                    //
                    tCIDLib::TEncodedTime enctAdj = errToLog.enctLogged();
                    if (c4MsgInd & 1)
                        enctAdj -= (c4MsgInd % 20) * kCIDLib::enctOneSecond;
                    else
                        enctAdj += (c4MsgInd % 20) * kCIDLib::enctOneSecond;
                    errToLog.enctLogged(enctAdj);

                    // And finally log it
                    orbcLS->LogSingle(errToLog);

                    // Cycle through the severities
                    if (eSev == tCIDLib::ESeverities::Max)
                        eSev = tCIDLib::ESeverities::Min;
                    else
                        eSev++;
                }

                catch(const TError& errToCatch)
                {
                    TMtxLocker lockOut(pmtxOut);
                    *pstrmOut << thrThis.strName() << L" lost his logger proxy:\n"
                              << errToCatch
                              << kCIDLib::DNewLn << kCIDLib::EndLn;

                    bHaveClient = kCIDLib::False;
                    orbcLS.SetPointer(nullptr);
                }

                // Sleep a varying amount, up to a quarter second
                TThread::Sleep(randMsg.c4GetNextNum() % 250);

                // Every 200 times through, purposefully close our client proxy
                c4Counter++;
                if (!(c4Counter % 200))
                {
                    orbcLS.SetPointer(nullptr);
                    bHaveClient = kCIDLib::False;
                }
            }
        }
    }

    catch(const TError& errToCatch)
    {
        TMtxLocker lockOut(pmtxOut);
        *pstrmOut << thrThis.strName() << L" got an unhandled exception:\n"
                  << errToCatch
                  << kCIDLib::DNewLn << kCIDLib::EndLn;
    }
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Set up some global data all the threads need to see
    pstrmOut = &TSysInfo::strmErr();
    pevStart = new TEvent(tCIDLib::EEventStates::Reset);
    pmtxOut  = new TMutex;

    try
    {
        if (TSysInfo::c4CmdLineParmCount() > 3)
        {
            *pstrmOut   << L"Usage:\n   TestLogSrv [seconds threadcount /NSAddr=xxx]\n"
                        << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  If there is a seconds, then get it. Note that /NSAddr is a magic
        //  parm and is removed before we see it.
        //
        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        tCIDLib::TCard4 c4Seconds = 30;
        if (cursParms)
        {
            c4Seconds = cursParms->c4Val();
            ++cursParms;
        }

        // And the thread count
        tCIDLib::TCard4 c4ThreadCount = 1;
        if (cursParms)
        {
            c4ThreadCount = cursParms->c4Val();
            ++cursParms;
        }

        //
        //  To provide a nice sampling of messages of different length, we
        //  read in our own source file and take all of the non-empty lines
        //  longer than 8 chars as messages. The threads will randomly select
        //  from this list.
        //
        {
            TTextFileInStream strmSrc
            (
                L"TestLogSrv.cpp"
                , tCIDLib::ECreateActs::OpenIfExists
                , tCIDLib::EFilePerms::AllAccess
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Excl_Read
                , new TUSASCIIConverter
            );

            TString strCurLn;
            while (!strmSrc.bEndOfStream())
            {
                strmSrc >> strCurLn;
                if (!strCurLn.bIsEmpty())
                {
                    strCurLn.StripWhitespace();
                    if ((strCurLn.c4Length() > 8) && !strCurLn.bStartsWith(L"//"))
                        colMsgs.objAdd(strCurLn);
                }
            }
        }

        if (colMsgs.bIsEmpty())
        {
            *pstrmOut   << L"No source file found for msg source"
                        << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        // Crank up the Orb client support
        *pstrmOut << L"Initializing Orb client support" << kCIDLib::EndLn;
        facCIDOrb().InitClient();

        //
        //  Spin up the indicated number of threads. We pass them an event
        //  that we'll use to pause them all until we are ready for them
        //  to go.
        //
        TThread** apthrTests = new TThread*[c4ThreadCount];
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            TString strName(L"TestLogSrvTThread");
            strName.AppendFormatted(c4Index + 1);
            apthrTests[c4Index] = new TThread(strName, &eTestThreadFunc);
            apthrTests[c4Index]->Start();
        }

        // Ok, they are all going, so release the event
        pevStart->Trigger();

        // Sleep the number of seconds we were told
        TThread::Sleep(c4Seconds * kCIDLib::c4OneSecond);

        // And ask them all to stop
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            if (!apthrTests[c4Index]->bIsRunning())
                continue;

            {
                TMtxLocker lockOut(pmtxOut);
                *pstrmOut << L"Shutting down thread #" << c4Index
                          << kCIDLib::EndLn
                          << kCIDLib::FlushIt;
            }
            apthrTests[c4Index]->ReqShutdownSync(5000);
            apthrTests[c4Index]->eWaitForDeath(5000);
        }

        // Query the last 32 items
        tCIDOrbUC::TLSrvProxy orbcLS = facCIDOrbUC().orbcLogSrvProxy(4000);
        TVector<TLogEvent>  colMatches;
        const tCIDLib::TCard4 c4Count = orbcLS->c4QueryEvents
        (
            colMatches, 32, L"*", L"*", L"*", L"*", 0xFF, 0xFF
        );

        TTime tmLogged;
        tmLogged.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TLogEvent& logevCur = colMatches[c4Index];
            logevCur.AdvFormat(*pstrmOut, tmLogged);
        }
        pstrmOut->Flush();
    }

    catch(const TError& errToCatch)
    {
        *pstrmOut   << L"A CIDLib Exception occured:\n"
                    << errToCatch << kCIDLib::FlushIt;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        *pstrmOut << L"An unknown exception occured:\n" << kCIDLib::FlushIt;
        return tCIDLib::EExitCodes::SystemException;
    }

    try
    {
        //
        //  Close down the orb support. Note that it could not be in the same
        //  scope as the rest of the stuff above. We use a name server proxy
        //  object up there. It won't destruct until the scope ends. If we put
        //  this up there we would terminate the orb before that object got
        //  destroyed, which would cause an error.
        //
        facCIDOrb().Terminate();
    }

    catch(const TError& errToCatch)
    {
        TSysInfo::strmErr() << L"A CIDLib Exception occured during cleanup:\n"
                            << errToCatch << kCIDLib::FlushIt;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        TSysInfo::strmErr() << L"An unknown exception occured furing cleanup:\n"
                            << kCIDLib::FlushIt;
        return tCIDLib::EExitCodes::SystemException;
    }


    return tCIDLib::EExitCodes::Normal;
}

