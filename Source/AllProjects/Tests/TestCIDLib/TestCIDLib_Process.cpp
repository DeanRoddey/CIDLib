//
// FILE NAME: TestCIDLib_Process.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/12/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the process
//  control classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"



// -----------------------------------------------------------------------------
//  Local data types
//
//  TThreadData
//      This is the data buffer that is passed to each instance of the TThread
//      class. It is used to test control between threads.
//
//  TThreadList
//      This is a structure that we use to create an array of threads.
// -----------------------------------------------------------------------------
struct  TThreadData
{
    tCIDLib::EExitCodes eRetVal;
    tCIDLib::TCard4     c4ThreadNum;
    tCIDLib::TCard4     c4Counter;
};

struct  TThreadList
{
    TThread*            pThread;
    TThreadData*        pBuf;
};



// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

//
//  This is the thread function for the test threads. These threads just wake
//  up every 1/2 second and bump up their counters by 1.
//
tCIDLib::EExitCodes eThreadFunc(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // Look at the passed data as a thread data
    TThreadData* const pBuf = static_cast<TThreadData*>(pData);
    thrThis.Sync();

    try
    {
        while (1)
        {
            // If we get an exit signal, then exit
            if (thrThis.bCheckShutdownRequest())
                break;

            // Sleep a little
            thrThis.Sleep(100);

            // Bump up the counter
            pBuf->c4Counter++;
        }
    }

    catch(...)
    {
        TAudio::AudioCue(tCIDLib::EAudioCues::Chime);
    }

    // Return the return value that we were told to
    return pBuf->eRetVal;
}


tCIDLib::EExitCodes eThreadFunc2(TThread& thrThis, tCIDLib::TVoid* pData)
{
    tCIDLib::TBoolean* pbResult = reinterpret_cast<tCIDLib::TBoolean*>(pData);
    thrThis.Sync();

    //  Do a sleep, which the caller will interrupt via a shutdown request
    //  so we should get a false return from this.
    //
    *pbResult = thrThis.bSleep(10000);
    return tCIDLib::EExitCodes::Normal;
}


//
//  This process does a basic threading test in which one thread creates
//  and controls a set of worker threads.
//
static tCIDLib::TVoid TestBasicThreading(TTextOutStream& strmOut)
{
    const tCIDLib::TCard4   c4ThreadCount = 8;
    TThreadList             arThreads[c4ThreadCount];
    tCIDLib::TCard4         c4Ind;

    // Create instances of the local test thread
    TTextStringOutStream   strmTmp(64);
    for (c4Ind = 0; c4Ind < c4ThreadCount; c4Ind++)
    {
        arThreads[c4Ind].pBuf               = new TThreadData;
        arThreads[c4Ind].pBuf->eRetVal      = tCIDLib::EExitCodes(c4Ind);
        arThreads[c4Ind].pBuf->c4ThreadNum  = c4Ind+1;
        arThreads[c4Ind].pBuf->c4Counter    = 1;

        strmTmp.Reset();
        strmTmp << L"Thread" << c4Ind+1 << kCIDLib::FlushIt;
        arThreads[c4Ind].pThread = new TThread(strmTmp.strData(), &eThreadFunc);

        if (arThreads[c4Ind].pThread->bIsRunning())
        {
            strmOut << CUR_LN
                    << L"Thread " << c4Ind+1
                    << L" should not be running yet" << kCIDLib::EndLn;
        }

        // Start the thread now and pass it its data
        arThreads[c4Ind].pThread->Start(arThreads[c4Ind].pBuf);

        if (!arThreads[c4Ind].pThread->bIsRunning())
        {
            strmOut << CUR_LN
                    << L"Thread " << c4Ind+1 << L" should be running" << kCIDLib::EndLn;
        }
    }

    // Sleep for a little to let the threads count up some
    TThread::Sleep(3000);

    // Sync up with each thread and check its counter
    for (c4Ind = 0; c4Ind < c4ThreadCount; c4Ind++)
    {
        arThreads[c4Ind].pThread->WaitSync(5000);

        if (arThreads[c4Ind].pBuf->c4Counter == 1)
        {
            strmOut << CUR_LN
                    << L"Thread " << c4Ind+1 << L" is not counting up"
                    << kCIDLib::EndLn;
        }

        // Let the thread go again
        arThreads[c4Ind].pThread->Release();
    }

    // Sync up with each thread and tell it to exit
    for (c4Ind = 0; c4Ind < c4ThreadCount; c4Ind++)
        arThreads[c4Ind].pThread->ReqShutdownSync(5000);

    // Wait for them to die
    for (c4Ind = 0; c4Ind < c4ThreadCount; c4Ind++)
        arThreads[c4Ind].pThread->eWaitForDeath(5000);

    //
    //  Test the exit codes. Each thread will set its exit code to the
    //  value we gave it for that purpose.
    //
    for (c4Ind = 0; c4Ind < c4ThreadCount; c4Ind++)
    {
        if (arThreads[c4Ind].pThread->eTermCode()
                                        != arThreads[c4Ind].pBuf->eRetVal)
        {
            strmOut << CUR_LN << L"Thread "
                    << c4Ind+1 << L" had a bad exit code. " << L"Expected:"
                    << arThreads[c4Ind].pBuf->eRetVal << L" but got: "
                    << arThreads[c4Ind].pThread->eTermCode() << kCIDLib::EndLn;
        }
    }

    // Destroy the threads and thread data objects
    for (c4Ind = 0; c4Ind < c4ThreadCount; c4Ind++)
    {
        delete arThreads[c4Ind].pThread;
        delete arThreads[c4Ind].pBuf;
    }
}


static tCIDLib::TVoid TestBasicShutdown(TTextOutStream& strmOut)
{
    tCIDLib::TBoolean bTest = kCIDLib::True;
    TThread thrTest(L"BasicShutdownTest1Thread", &eThreadFunc2);

    // Start the thread and ask it to shutdown immediately
    try
    {
        thrTest.Start(&bTest);
        thrTest.ReqShutdownSync(5000);
        thrTest.eWaitForDeath(5000);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut << L"Exception waiting for basic shutdown test thread to stop\n"
                << kCIDLib::EndLn;
        throw;
    }

    // The flag should have been set to false
    if (bTest)
    {
        strmOut << CUR_LN << L"The thread didn't see the shutdown request"
                << kCIDLib::EndLn;
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method tests out the process related classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestProcess()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic threading";
        TestBasicThreading(strmOut());

        pszCurTest = L"basic shutdown";
        TestBasicShutdown(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" test" << kCIDLib::EndLn << kCIDLib::FlushIt;
        throw;
    }
}

