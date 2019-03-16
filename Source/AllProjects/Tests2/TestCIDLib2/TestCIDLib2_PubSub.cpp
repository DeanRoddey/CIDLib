//
// FILE NAME: TestCIDLib2_PubSub.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2018
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file contains tests related to the tCIDPubSub namespace and related classes,
//  which provides an intra-application publish/subscribe mechanism.
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
#include    "TestCIDLib2.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_PubSub1,TTestFWTest)
RTTIDecls(TTest_PubSubVector,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_PubSub1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_PubSub1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_PubSub1::TTest_PubSub1() :

    TTestFWTest
    (
        L"Pub/Sub 1", L"Basic tests of the Publish/Subscribe framework", 5
    )
{
}

TTest_PubSub1::~TTest_PubSub1()
{
}


// ---------------------------------------------------------------------------
//  TTest_PubSub1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_PubSub1::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Publish a test couple topics, where the msgs are strings
    const TString strTopic1(L"/Test/Topic1");
    const TString strTopic2(L"/Test/Topic2");

    // Put the topic in a scope so that we can force it out of scope below and test that
    {
        TPubSubTopic pstopTest1 = TPubSubTopic::pstopCreateTopic(strTopic1, TString::clsThis());
        TPubSubTopic pstopTest2 = TPubSubTopic::pstopCreateTopic(strTopic2, TString::clsThis());

        // The topics should show up as existing now
        if (!TPubSubTopic::bTopicExists(strTopic1))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Topic1 not reported as existing after being published\n\n";
        }

        if (!TPubSubTopic::bTopicExists(strTopic2))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Topic2 not reported as existing after being published\n\n";
        }

        // Subscribe us to these topics
        SubscribeToTopic(strTopic1, kCIDLib::True);
        SubscribeToTopic(strTopic2, kCIDLib::True);

        // And there should be 1 reported subscribers
        if (pstopTest1.c4SubCount() != 1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Should have 1 subcribers for Topic1 but got "
                    << TCardinal(pstopTest1.c4SubCount())
                    << L"\n\n";
        }

        if (pstopTest2.c4SubCount() != 1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Should have 1 subcribers for Topic2 but got "
                    << TCardinal(pstopTest2.c4SubCount())
                    << L"\n\n";
        }

        // And we should show up subscribed
        if (!bIsSubscribedToTopic(strTopic1))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Not subscribed to Topic1 after subscription call\n\n";
        }

        if (!bIsSubscribedToTopic(strTopic2))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Not subscribed to Topic2 after subscription call\n\n";
        }

        // Create a msg object and attempt to access the object before it's set
        TPubSubMsg psmsgTest;
        try
        {
            psmsgTest.objMsg();
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Accessing empty msg didn't throw\n\n";
        }

        catch(...)
        {
        }

        // Publish a test message on each topic and test it
        m_strLastMsg.Clear();
        m_strLastTopic.Clear();
        pstopTest1.Publish(new TString(L"This is a test"));
        try
        {
            if (m_strLastMsg != L"This is a test")
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Received msg != to original published one\n\n";
            }

            if (m_strLastTopic != strTopic1)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Received msg has wrong topic: "
                        << m_strLastTopic << kCIDLib::DNewLn;
            }
        }

        catch(TError& errToCatch)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Error accessing msg data:\n"
                    << errToCatch << L"\n\n";
        }

        m_strLastMsg.Clear();
        m_strLastTopic.Clear();
        pstopTest2.Publish(new TString(L"This is a test 2"));
        try
        {
            if (m_strLastMsg != L"This is a test 2")
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Received msg != to original published one\n\n";
            }

            if (m_strLastTopic != strTopic2)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Received msg has wrong topic: "
                        << m_strLastTopic << kCIDLib::DNewLn;
            }
        }

        catch(TError& errToCatch)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Error accessing msg data:\n"
                    << errToCatch << L"\n\n";
        }
    }

    // The topics should no longer exist now that they are out of scope
    if (TPubSubTopic::bTopicExists(strTopic1))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Topic1 reported as existing after going out of scope\n\n";
    }

    if (TPubSubTopic::bTopicExists(strTopic2))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Topic2 reported as existing after going out of scope\n\n";
    }

    // Now let's unsubscribe explicitly
    UnsubscribeFromTopic(strTopic1);
    if (!bIsSubscribedToTopic(strTopic2))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Unsubscribe from Topic1 unsubscribed Topic2\n\n";
    }
    UnsubscribeFromTopic(strTopic2);
    if (bIsSubscribedToTopic(strTopic1))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Still subscribed to Topic1 after unsubscribe call\n\n";
    }
    if (bIsSubscribedToTopic(strTopic2))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Still subscribed to Topic2 after unsubscribe call\n\n";
    }

    //
    //  If we now do another subscription and say the topic must exist, we should
    //  get an error.
    //
    try
    {
        SubscribeToTopic(strTopic1, kCIDLib::True);

        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Required topic didn't throw after topic dtor:\n\n";
    }

    catch(...)
    {
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_PubSub1: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTest_PubSub1::ProcessPubMsg(const TPubSubMsg& psmsgIn)
{
    m_strLastMsg = psmsgIn.objMsgAs<TString>();
    m_strLastTopic = psmsgIn.strSrcTopicPath();
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_PubSubVector
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_PubSubVector: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_PubSubVector::TTest_PubSubVector() :

    m_c4MsgCount(0)
    , TTestFWTest(L"Pub/Sub Vector", L"Test vector support of pub/sub", 6)
{
}

TTest_PubSubVector::~TTest_PubSubVector()
{
}


// ---------------------------------------------------------------------------
//  TTest_PubSubVector: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_PubSubVector::eRunTest(TTextStringOutStream&  strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    const TString strTopicId(L"/Tests/PubSubVector/Strings");

    // Create a ref vector and enable publishing on it
    TRefVector<TString> colTest(tCIDLib::EAdoptOpts::Adopt, 64);
    colTest.EnablePubSub(strTopicId);

    // Subscribe us to this topic. Say it must exist
    SubscribeToTopic(strTopicId, kCIDLib::True);

    // A counter we use to verify we only got as many msgs as we expect
    tCIDLib::TCard4 c4TestCnt = 0;

    // Do an add element
    colTest.Add(new TString(L"Test String"));
    if (!bTestPubMsg(strmOut, TColPubSubInfo::EEvents::ElemAdded, 0, 0, c4TestCnt))
        eRes = tTestFWLib::ETestRes::Failed;

    // And test removing it
    colTest.RemoveAt(0);
    if (!bTestPubMsg(strmOut, TColPubSubInfo::EEvents::ElemRemoved, 0, 0, c4TestCnt))
        eRes = tTestFWLib::ETestRes::Failed;

    // Add a string, then do a remove all. We should get an add and a clear
    colTest.Add(new TString(L"Test String"));
    if (!bTestPubMsg(strmOut, TColPubSubInfo::EEvents::ElemAdded, 0, 0, c4TestCnt))
        eRes = tTestFWLib::ETestRes::Failed;

    colTest.RemoveAll();
    if (!bTestPubMsg(strmOut, TColPubSubInfo::EEvents::Cleared, 0, 0, c4TestCnt))
        eRes = tTestFWLib::ETestRes::Failed;

    // Test the block load stuff
    {
        TColBlockModeJan janBlock(&colTest, kCIDLib::True, 0);

        TString strVal;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 32; c4Index++)
        {
            colTest.Add(new TString(TCardinal(c4Index)));
            janBlock.IncCount();
        }
    }

    // We should now have a block added msgs
    if (!bTestPubMsg(strmOut, TColPubSubInfo::EEvents::BlockAdded, 0, 32, c4TestCnt))
        eRes = tTestFWLib::ETestRes::Failed;

    // Now do the same for remove mode
    {
        TColBlockModeJan janBlock(&colTest, kCIDLib::False, 5);
        for (tCIDLib::TCard4 c4Index = 5; c4Index < 10; c4Index++)
        {
            colTest.RemoveAt(c4Index);
            janBlock.IncCount();
        }
    }

    if (!bTestPubMsg(strmOut, TColPubSubInfo::EEvents::BlockRemoved, 5, 5, c4TestCnt))
        eRes = tTestFWLib::ETestRes::Failed;

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_PubSub1: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTest_PubSubVector::ProcessPubMsg(const TPubSubMsg& psmsgIn)
{
    m_c4MsgCount++;
    m_colpsiLast = psmsgIn.objMsgAs<TColPubSubInfo>();
    m_strLastTopic = psmsgIn.strSrcTopicPath();
}



// ---------------------------------------------------------------------------
//  TTest_PubSubVector: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TTest_PubSubVector::bTestPubMsg(        TTextStringOutStream&   strmOut
                                , const TColPubSubInfo::EEvents eEvent
                                , const tCIDLib::TCard4         c4Index1
                                , const tCIDLib::TCard4         c4Index2
                                ,       tCIDLib::TCard4&        c4Count)
{
    // Make sure the counts match. We should now have one more than the incoming
    if (c4Count + 1 != m_c4MsgCount)
    {
        strmOut << TFWCurLn << L"Expected msg count of " << (c4Count + 1)
                << L" but got " << m_c4MsgCount << L"\n\n";
        return kCIDLib::False;
    }

    // Update the caller's count now
    c4Count++;

    // And test the event info
    if (m_colpsiLast.eEvent() != eEvent)
    {
        strmOut << TFWCurLn << L"Expected add item event\n\n";
        return kCIDLib::False;
    }

    if (m_colpsiLast.c4Index1() != c4Index1)
    {
        strmOut << TFWCurLn << L"Expected index one of " << c4Index1
                << L"but got " << c4Index1 << L"\n\n";
        return kCIDLib::False;
    }

    if (m_colpsiLast.c4Index2() != c4Index2)
    {
        strmOut << TFWCurLn << L"Expected index two of " << c4Index2
                << L"but got " << c4Index2 << L"\n\n";
        return kCIDLib::False;
    }

    return kCIDLib::True;
}
