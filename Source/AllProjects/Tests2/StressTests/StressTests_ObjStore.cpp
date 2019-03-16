//
// FILE NAME: TestObjStore_ObjStoreTests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This file implements a set of object store stress tests.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "StressTests.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_ObjStore1,TTestFWTest)
RTTIDecls(TTest_ObjStore2,TTestFWTest)
RTTIDecls(TTest_ObjStore3,TTestFWTest)
RTTIDecls(TTest_ObjStore4,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ObjStore1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ObjStore1::TTest_ObjStore1() :

    TTestFWTest
    (
        L"Object Store 1"
        , L"Test of store expansion. Writes a large number of items and validates"
        , 5
    )
{
}

TTest_ObjStore1::~TTest_ObjStore1()
{
}



// ---------------------------------------------------------------------------
//  TTest_ObjStore1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ObjStore1::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    TCIDObjStore oseTest;

    TArea area1;
    TArea area2;
    tCIDLib::TCard4 c4Ver1 = 0;
    tCIDLib::TCard4 c4Ver2 = 0;

    // Initialize the engine we are going to use
    if (!oseTest.bInitialize(L".\\", L"StressTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have created the store, not opened it\n\n";
    }

    // There should be no objects
    if (oseTest.c4ObjectsInStore())
    {
        strmOut << TFWCurLn
                << L"Expected to find 0 objects in the store, but found "
                << oseTest.c4ObjectsInStore() << kCIDLib::DNewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Just loop and write 128 large strings. We need to know what the
    //  values of each one is, so that we can validate its contents
    //  later.
    //
    const tCIDLib::TCard4   c4Count = 512;
    TString                 astrKeys[c4Count];
    TString                 strValTmpl
    (
        L"This is the text for the values. We need to have a relatively "
        L"long string. We format the index value at the end so we can "
        L"tell if we have the right one. By having long enough text, we "
        L"can insure that the store will have to expand by the time we've "
        L"added all of the items to the store, and will therefore test out "
        L"the store expansion capabilities without any other complexities "
        L"of moving items around in the store and whatnot, since we just "
        L"continue to add, and never modify or remove any items. So we "
        L"should end up with a completely filled store, with no gaps. "
        L"#"
    );
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        astrKeys[c4Index] = L"/Stress1Strings/String";
        astrKeys[c4Index].AppendFormatted(c4Index + 1);

        TString strValue(strValTmpl);
        strValue.AppendFormatted(c4Index + 1);
        oseTest.AddObject(astrKeys[c4Index], strValue, 16);
    }

    //
    //  Go back through them and make sure that we can get them all back,
    //  and validate their values.
    //
    tCIDLib::TCard4 c4Ver;
    TString strTest;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TString strValue(strValTmpl);
        strValue.AppendFormatted(c4Index + 1);

        c4Ver = 0;
        if (!oseTest.bReadObject(astrKeys[c4Index], c4Ver, strTest))
        {
            strmOut << TFWCurLn
                    << L"Did not get new data for index " << c4Index
                    << kCIDLib::DNewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        if (strTest != strValue)
        {
            strmOut << TFWCurLn
                    << L"String at index " << c4Index
                    << L" was not equal to original\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    //
    //  Expand the string beyond the reserve size and then go back and
    //  update them all.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        astrKeys[c4Index] = L"/Stress1Strings/String";
        astrKeys[c4Index].AppendFormatted(c4Index + 1);

        TString strValue(strValTmpl);
        strValue.AppendFormatted(c4Index + 1);
        strValue.Append(L"\nThis should take it over the limit and force movement");
        oseTest.c4UpdateObject(astrKeys[c4Index], strValue);
    }

    // Validate the store and then close it
    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    // Reopen it again, revalidate, and close again
    if (oseTest.bInitialize(L".\\", L"StressTestStore"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Should have opened the store, not created it\n\n";
    }

    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ObjStore2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ObjStore2::TTest_ObjStore2() :

    TTestFWTest
    (
        L"Object Store 2"
        , L"A single threaded simulation of 1 minute of normal usage patterns"
        , 7
    )
{
}

TTest_ObjStore2::~TTest_ObjStore2()
{
}



// ---------------------------------------------------------------------------
//  TTest_ObjStore2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ObjStore2::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Create a set of keys for our own use, based on our thread name,
    //  so they will be unique in the store.
    //
    const tCIDLib::TCard4 c4KeyCount = 256;
    TString astrKeys[c4KeyCount];

    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4KeyCount; c4Index++)
    {
        astrKeys[c4Index] = L"/StressTest2/Key";
        astrKeys[c4Index].AppendFormatted(c4Index);
    }

    // And these are our data values and the item ids and versions
    tCIDLib::TCard4 ac4Versions[c4KeyCount];
    TString         astrValues[c4KeyCount];
    TString         strTmp;
    TCIDObjStore    oseTest;
    TRandomNum      randSelect;

    // Create the store
    oseTest.bInitialize(L".\\", L"StressTest2Store");

    //
    //  Loop for a minute. We generate a random number (modulo our key
    //  count), and we check whether that item exist and create it if
    //  not. Else, we read it in, update the value, and write it back
    //  out.
    //
    tCIDLib::TCard4 c4ErrCount = 0;
    const tCIDLib::TCard4 c4Start = TTime::c4Millis();
    const tCIDLib::TCard4 c4End   = c4Start + 60000;
    randSelect.Seed(c4Start);
    while (TTime::c4Millis() < c4End)
    {
        // Get the index for this round
        tCIDLib::TCard4 c4CurIndex = randSelect.c4GetNextNum();
        c4CurIndex %= c4KeyCount;

        //
        //  See if this key exists yet, and if not, create it. Since we
        //  know that no threads share keys, we don't have to worry about
        //  this not being atomic.
        //
        if (!oseTest.bKeyExists(astrKeys[c4CurIndex]))
        {
            //
            //  Give it a reserve of 16 bytes. This will get us a few
            //  expansions within the reserve and a few that go over the
            //  reserve.
            //
            astrValues[c4CurIndex] = L"1";
            oseTest.AddObject(astrKeys[c4CurIndex], astrValues[c4CurIndex], 16);

            // Initialize the version for this one
            ac4Versions[c4CurIndex] = 1;
        }
         else
        {
            //
            //  It exists, so get the current value, and compare it
            //  against what we last wrote out. We have to force the
            //  read by using a fake version, since it shouldn't have
            //  changed since we last modified it. But check it with
            //  our current id first, to insure that it hasn't been
            //  changed.
            //
            if (oseTest.bReadObject(astrKeys[c4CurIndex], ac4Versions[c4CurIndex], strTmp))
            {
                c4ErrCount++;
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Should not have gotten a new value\n\n";
            }

            // Now force a read
            tCIDLib::TCard4 c4TmpId = 0;
            if (!oseTest.bReadObject(astrKeys[c4CurIndex], c4TmpId, strTmp))
            {
                c4ErrCount++;
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn
                        << L"Did not get value with forced version\n\n";
            }

            // Compare the value to what we last wrote
            if (strTmp != astrValues[c4CurIndex])
            {
                c4ErrCount++;
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Didn't equal last written value\n\n";
            }

            //
            //  Update the value, and write it out again, storing the
            //  new version.
            //
            astrValues[c4CurIndex].Append(L"1");
            ac4Versions[c4CurIndex] = oseTest.c4UpdateObject
            (
                astrKeys[c4CurIndex], astrValues[c4CurIndex]
            );
        }

        // If we hit 32 errors, then break out. It's not worth going on
        if (c4ErrCount > 32)
            break;

        // Sleep a bit
        TThread::Sleep(20);
    }

    // Validate the store and close it
    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    // Reopen it again, revalidate, and close again
    if (oseTest.bInitialize(L".\\", L"StressTest2Store"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }

    oseTest.ValidateStore(strmOut);
    oseTest.Close();

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ObjStore3: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ObjStore3::TTest_ObjStore3() :

    TTestFWTest
    (
        L"Object Store 3"
        , L"A multi-threaded simulation of 1 minute of normal usage patterns"
        , 7
    )
    , m_c4ThreadErrs(0)
    , m_evStart(tCIDLib::EEventStates::Reset)
    , m_pstrmOut(nullptr)
{
}

TTest_ObjStore3::~TTest_ObjStore3()
{
}



// ---------------------------------------------------------------------------
//  TTest_ObjStore3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ObjStore3::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Make the output stream available to the thread
    m_pstrmOut = &strmOut;

    // Create the object store that all the threads share
    m_oseTest.bInitialize(L".\\", L"StressTest3Store");

    //
    //  We keep an array of thread objects, so we can track them and
    //  shut them down when the time is up.
    //
    const tCIDLib::TCard4 c4ThreadCount = 8;
    TThread* apthrStress3[c4ThreadCount];

    // Allocate the thread objects
    tCIDLib::TCard4 c4Index;
    TString strName;
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
    {
        strName = L"ObjStoreStress3Thread";
        strName.AppendFormatted(c4Index);
        apthrStress3[c4Index] = new TThread
        (
            strName
            , TMemberFunc<TTest_ObjStore3>(this, &TTest_ObjStore3::eTestThread)
        );
    }

    try
    {
        // And now start them up
        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
            apthrStress3[c4Index]->Start();

        // They are all blocked on our event, so let them go now
        m_evStart.Trigger();

        // Wait for a minute for them to do their thing
        TThread::Sleep(60000);

        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            if (apthrStress3[c4Index]->bIsRunning())
                apthrStress3[c4Index]->ReqShutdownSync();
            apthrStress3[c4Index]->eWaitForDeath();
        }
    }

    catch(const TError&)
    {
        // Shutdown any that got started
        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            if (apthrStress3[c4Index]->bIsRunning())
                apthrStress3[c4Index]->ReqShutdownSync();
            apthrStress3[c4Index]->eWaitForDeath();
        }
    }

    // Validate the store and close it
    m_oseTest.ValidateStore();
    m_oseTest.Close();

    // Reopen it again, revalidate, and close again
    if (m_oseTest.bInitialize(L".\\", L"StressTest3Store"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }
    m_oseTest.ValidateStore();
    m_oseTest.Close();

    // If any thread errors, we failed
    if (m_c4ThreadErrs)
        eRes = tTestFWLib::ETestRes::Failed;

    return eRes;
}


//
//  This is used by each of the test threads. Note that we share a common
//  output stream. So we have to sync before writing any message to the test
//  output.
//
tCIDLib::EExitCodes TTest_ObjStore3::eTestThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We've got the data, so let the caller go
    thrThis.Sync();

    //
    //  Create a set of keys for our own use, based on our thread name,
    //  so they will be unique in the store.
    //
    const tCIDLib::TCard4 c4KeyCount = 32;
    TString astrKeys[c4KeyCount];

    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4KeyCount; c4Index++)
    {
        astrKeys[c4Index] = L"/";
        astrKeys[c4Index].Append(thrThis.strName());
        astrKeys[c4Index].Append(L"/Key");
        astrKeys[c4Index].AppendFormatted(c4Index);
    }

    // And these are our data values and the item ids and versions
    tCIDLib::TCard4 ac4Versions[c4KeyCount];
    TString         astrValues[c4KeyCount];
    TString         strTmp;
    TRandomNum      randSelect;
    try
    {
        //
        //  Block on the event. Once all the isntances are ready, the main
        //  thread will let us all go.
        //
        m_evStart.WaitFor(5000);

        //
        //  Loop until we are told to shutdown. We generate a random number
        //  (modulo our key count), and we check whether that item exist and
        //  create it if not. Else, we read it in, update the value, and write
        //  it back out.
        //
        randSelect.Seed(TTime::c4Millis());
        while (kCIDLib::True)
        {
            // Check for a shutdown request
            if (thrThis.bCheckShutdownRequest())
                break;

            // Get the index for this round
            tCIDLib::TCard4 c4CurIndex = randSelect.c4GetNextNum();
            c4CurIndex %= c4KeyCount;

            //
            //  See if this key exists yet, and if not, create it. Since we
            //  know that no threads share keys, we don't have to worry about
            //  this not being atomic.
            //
            if (!m_oseTest.bKeyExists(astrKeys[c4CurIndex]))
            {
                //
                //  Give it a reserve of 16 bytes. This will get us a few
                //  expansions within the reserve and a few that go over the
                //  reserve.
                //
                astrValues[c4CurIndex] = L"1";
                m_oseTest.AddObject(astrKeys[c4CurIndex], astrValues[c4CurIndex], 16);

                // Initialize the version for this one
                ac4Versions[c4CurIndex] = 1;
            }
             else
            {
                //
                //  It exists, so get the current value, and compare it
                //  against what we last wrote out. We have to force the read
                //  by using a fake version, since it shouldn't have changed
                //  since we last modified it. But check it with our current
                //  id first, to insure that it hasn't been changed.
                //
                if (m_oseTest.bReadObject(  astrKeys[c4CurIndex]
                                            , ac4Versions[c4CurIndex]
                                            , strTmp))
                {
                    TMtxLocker lockOut(&m_mtxOut);
                    m_c4ThreadErrs++;
                    *m_pstrmOut << TFWCurLn
                                << L"Should not have gotten a new value\n\n";
                }

                // Now force a read
                tCIDLib::TCard4 c4TmpId = 0;
                if (!m_oseTest.bReadObject(astrKeys[c4CurIndex], c4TmpId, strTmp))
                {
                    TMtxLocker lockOut(&m_mtxOut);
                    m_c4ThreadErrs++;
                    *m_pstrmOut << TFWCurLn
                                << L"Did not get value with forced version\n\n";
                }

                // Compare the value to what we last wrote
                if (strTmp != astrValues[c4CurIndex])
                {
                    TMtxLocker lockOut(&m_mtxOut);
                    m_c4ThreadErrs++;
                    *m_pstrmOut << TFWCurLn
                                << L"Didn't equal last written value\n\n";
                }

                //
                //  Update the value, and write it out again, storing the
                //  new version.
                //
                astrValues[c4CurIndex].Append(L"1");
                ac4Versions[c4CurIndex] = m_oseTest.c4UpdateObject
                (
                    astrKeys[c4CurIndex], astrValues[c4CurIndex]
                );
            }

            // If we've hit 32 errors, then just give up
            if (m_c4ThreadErrs > 32)
                break;

            // Sleep a bit
            TThread::Sleep(50);
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"\nA CIDLib runtime error occured. Thread="
                    << thrThis.strName() << L"\n   Error: "
                    << errToCatch << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"A kernel error occured. Thread="
                    << thrThis.strName() << L"\n   Error: "
                    << kerrToCatch.errcId() << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"\nA general exception occured. Thread="
                    << thrThis.strName() << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}





// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjStore4
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ObjStore4: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ObjStore4::TTest_ObjStore4() :

    TTestFWTest
    (
        L"Object Store 4"
        , L"A multi-threaded test of some threads adding and some removing objects"
        , 7
    )
    , m_c4NextKeyId(1)
    , m_c4ThreadErrs(0)
    , m_colKeys(tCIDLib::EMTStates::Safe)
    , m_evStart(tCIDLib::EEventStates::Reset)
    , m_pstrmOut(nullptr)
{
}

TTest_ObjStore4::~TTest_ObjStore4()
{
}



// ---------------------------------------------------------------------------
//  TTest_ObjStore4: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ObjStore4::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Make the output stream available to the thread
    m_pstrmOut = &strmOut;

    // Create an object store that all the threads share
    m_oseTest.bInitialize(L".\\", L"StressTest4Store");

    //
    //  We keep an array of thread objects, so we can track them and
    //  shut them down when the time is up. We make half of them add
    //  threads and half delete threads.
    //
    const tCIDLib::TCard4 c4ThreadCount = 4;
    TThread* apthrStress4[c4ThreadCount];

    // Allocate the thread objects
    tCIDLib::TCard4 c4Index;

    TString strName;
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
    {
        strName = L"ObjStoreStress4Thread";
        strName.AppendFormatted(c4Index);

        if (c4Index & 1)
        {
            apthrStress4[c4Index] = new TThread
            (
                strName
                , TMemberFunc<TTest_ObjStore4>(this, &TTest_ObjStore4::eDelThread)
            );
        }
         else
        {
            apthrStress4[c4Index] = new TThread
            (
                strName
                , TMemberFunc<TTest_ObjStore4>(this, &TTest_ObjStore4::eAddThread)
            );
        }
    }

    try
    {
        // And now start them up
        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
            apthrStress4[c4Index]->Start();

        // They are all blocked on our event, so let them go now
        m_evStart.Trigger();

        // Wait for a bit for them to do their thing
        TThread::Sleep(30000);

        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            if (apthrStress4[c4Index]->bIsRunning())
                apthrStress4[c4Index]->ReqShutdownSync();
            apthrStress4[c4Index]->eWaitForDeath();
        }
    }

    catch(const TError&)
    {
        // Shutdown any that got started
        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            if (apthrStress4[c4Index]->bIsRunning())
                apthrStress4[c4Index]->ReqShutdownSync();
            apthrStress4[c4Index]->eWaitForDeath();
        }
    }

    // Validate the store and close it
    m_oseTest.ValidateStore();
    m_oseTest.Close();

    // Reopen it again, revalidate, and close again
    if (m_oseTest.bInitialize(L".\\", L"StressTest4Store"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn
                << L"Should have opened the store, not created it\n\n";
    }
    m_oseTest.ValidateStore();
    m_oseTest.Close();

    // If any thread errors, we failed
    if (m_c4ThreadErrs)
        eRes = tTestFWLib::ETestRes::Failed;

    return eRes;
}



//
//  These are used by the add and delete test threads. Note that we share a
//  common output stream. So we have to sync before writing any message to
//  the test output.
//
tCIDLib::EExitCodes TTest_ObjStore4::eAddThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We've got the data, so let the caller go
    thrThis.Sync();

    TString     strKey;
    TString     strValue;
    TRandomNum  randSelect;
    try
    {
        //
        //  Block on the event. Once all the isntances are ready, the main
        //  thread will let us all go.
        //
        m_evStart.WaitFor(5000);

        // We'll generate random numbers to create data of different sizes
        randSelect.Seed(TTime::c4Millis());

        // Loop until we are told to shutdown
        tCIDLib::TCard4 c4Index = 1;
        while (kCIDLib::True)
        {
            // Check for a shutdown request
            if (thrThis.bCheckShutdownRequest())
                break;

            //
            //  If the list  is full, we've caught up and shouldn't
            //  add any until the delete threads have caught up a bit.
            //
            if (m_colKeys.bIsFull(1024))
            {
                TThread::Sleep(150);
                continue;
            }

            // Create a key based on the next available id
            strKey = L"/Test/Key";
            {
                TMtxLocker lockOut(&m_mtxOut);
                strKey.AppendFormatted(m_c4NextKeyId++);
            }

            // Set up the data of a random size
            tCIDLib::TCard4 c4DataSz = randSelect.c4GetNextNum() % 1024;
            strValue.Clear();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4DataSz; c4Index++)
                strValue.Append(kCIDLib::chSpace);

            // Add it, with 16 bytes of expansion
            m_oseTest.AddObject(strKey, strValue, 16);

            //
            //  It in now, so add it to the list so the delete thread can
            //  come get it. Sync while we do this. The queue is thread
            //  safe.
            //
            //  Generate a random priority value for the queue item, so
            //  that the delete thread can pull them out in a different
            //  order from what we add them, by just doing a simple read
            //  of the next highest priority item.
            //
            const tCIDLib::EQPrios ePrio = tCIDLib::EQPrios
            (
                randSelect.c4GetNextNum() % tCIDLib::c4EnumOrd(tCIDLib::EQPrios::Count)
            );
            m_colKeys.objPut(strKey, ePrio);

            // Sleep a bit
            TThread::Sleep(50);
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"\nA CIDLib runtime error occured. Thread="
                    << thrThis.strName() << L"\n   Error: "
                    << errToCatch << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"A kernel error occured. Thread="
                    << thrThis.strName() << L"\n   Error: "
                    << kerrToCatch.errcId() << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"\nA general exception occured. Thread="
                    << thrThis.strName() << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::EExitCodes TTest_ObjStore4::eDelThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // We've got the data, so let the caller go
    thrThis.Sync();

    TString strKey;
    try
    {
        //
        //  Block on the event. Once all the instances are ready, the main
        //  thread will let us all go.
        //
        m_evStart.WaitFor(5000);

        // Loop until we are told to shutdown
        while (kCIDLib::True)
        {
            // Check for a shutdown request
            if (thrThis.bCheckShutdownRequest())
                break;

            //
            //  Get the next highest priority item, if any are available,
            //  and remove it if so. The queue is thread safe.
            //
            if (m_colKeys.bGetHighestPrio(strKey, 25))
                m_oseTest.DeleteObject(strKey);

            // Sleep a bit
            TThread::Sleep(75);
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"\nA CIDLib runtime error occured. Thread="
                    << thrThis.strName() << L"\n   Error: "
                    << errToCatch << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"A kernel error occured. Thread="
                    << thrThis.strName() << L"\n   Error: "
                    << kerrToCatch.errcId() << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        TMtxLocker lockOut(&m_mtxOut);
        m_c4ThreadErrs++;
        *m_pstrmOut << L"\nA general exception occured. Thread="
                    << thrThis.strName() << kCIDLib::DNewLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


