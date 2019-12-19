//
// FILE NAME: TestRegX.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
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
//  This is the main implementation file of the test program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "TestServers.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TestServersApp,TTestFWApp)
RTTIDecls(TTestServersBase,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTestServersBase
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTestServersBase: Constructor and Destructor
// ---------------------------------------------------------------------------
TTestServersBase::~TTestServersBase()
{
}


// ---------------------------------------------------------------------------
//  TTestServersBase: Hidden constructors
// ---------------------------------------------------------------------------

// For now we default the sizes of the results lists
TTestServersBase::TTestServersBase( const   TString&    strName
                                    , const TString&    strDescr) :

    TTestFWTest(strName, strDescr, 5)
    , m_colMsgs(1)
    , m_colThreads(tCIDLib::EAdoptOpts::Adopt)
    , m_fcolResults(1)
{
}


// ---------------------------------------------------------------------------
//  TTestServersBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  After InvokeThreads returns, the derived class calls this to let us process the
//  results.
//
tTestFWLib::ETestRes TTestServersBase::eProcessThreadRes(TTextStringOutStream& strmOut)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Run through the thread results
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_fcolResults.c4ElemCount(); c4Index++)
    {
        if (!m_fcolResults[c4Index])
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << m_colMsgs[c4Index] << kCIDLib::DNewLn;
        }
    }
    return eRes;
}


// The derived class calls this to run the threads which call him back to do the work
tCIDLib::TVoid TTestServersBase::InvokeThreads(const tCIDLib::TCard4 c4Count)
{
    CIDAssert(((c4Count >= 0) && (c4Count < 64)), L"Thread count must be 1 to 64");

    // Make sure the results lists are sized correctly
    if (m_fcolResults.c4ElemCount() != c4Count)
        m_fcolResults.Reallocate(c4Count, kCIDLib::False);
    if (m_colMsgs.tElemCount() != c4Count)
        m_colMsgs.Realloc(c4Count);

    //
    //  Initialize all of the messages to indicate no status was ever stored. We set
    //  the result flags to false. So any thread that doesn't store a result will show
    //  up in error with this message.
    //
    m_colMsgs.SetAll(L"No thread result was ever stored");

    // Create our threads first, then we'll spin them up
    TString strThrName;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strThrName = strSubName();
        strThrName.AppendFormatted(c4Index + 1);
        m_colThreads.Add
        (
            new TThread
            (
                strThrName
                , TMemberFunc<TTestServersBase>(this, &TTestServersBase::eTestThreadStart)
            )
        );
    }

    // That worked, so let's start them up
    m_colThreads.bForEachNCI
    (
        [](TThread& thrCur, const tCIDLib::TCard4 c4Index)
        {
            // Avoid the need for a cast by copying the index to a non-const
            tCIDLib::TCard4 c4ThreadInd = c4Index;
            thrCur.Start(&c4ThreadInd);
            return kCIDLib::True;
        }
    );

    //
    //  Now we just wait for them all to complete or we are asked to stop. We
    //  just loop through the list and check them. When we find one that is
    //  done, we delete it and null that slot out, so that we know which ones
    //  are done.
    //
    TThread* pthrUs = TThread::pthrCaller();
    while (!m_colThreads.bIsEmpty() && pthrUs->bSleep(2000))
    {
        tCIDColAlgo::bRemoveIf
        (
            m_colThreads, [](const TThread& thrTest) { return !thrTest.bIsRunning(); }
        );
    }
}


tCIDLib::TVoid
TTestServersBase::SetThreadFailure(const tCIDLib::TCard4 c4Index, const TString& strErrMsg)
{
    m_fcolResults[c4Index] = kCIDLib::False;
    m_colMsgs[c4Index] = strErrMsg;
}

tCIDLib::TVoid
TTestServersBase::SetThreadFailure(const tCIDLib::TCard4 c4Index, const TError& errFailure)
{
    m_fcolResults[c4Index] = kCIDLib::False;
    m_colMsgs[c4Index].AppendFormatted(errFailure);
}

tCIDLib::TVoid TTestServersBase::SetThreadSuccess(const tCIDLib::TCard4 c4Index)
{
    m_fcolResults[c4Index] = kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TTestServersBase: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes
TTestServersBase::eTestThreadStart(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We get a pointer to our index, so store that then let our calling thread go
    tCIDLib::TCard4 c4OurIndex = *static_cast<tCIDLib::TCard4*>(pData);
    thrThis.Sync();

    // Call the virtual method
    try
    {
        TestThreadCallback(c4OurIndex);
    }

    catch(TError& errToCatch)
    {
        SetThreadFailure(c4OurIndex, errToCatch);
    }

    catch(...)
    {
        SetThreadFailure(c4OurIndex, L"A system exception occurred");
    }

    return tCIDLib::EExitCodes::Normal;
}




// ---------------------------------------------------------------------------
//  CLASS: TestServersApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TestServersApp: Constructor and Destructor
// ----------------------------------------------------------------------------
TestServersApp::TestServersApp()
{
}

TestServersApp::~TestServersApp()
{
}


// ----------------------------------------------------------------------------
//  TestServersApp: Public, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TestServersApp::bInitialize(TString& strErr)
{
    // We need to initialize the client side ORB
    facCIDOrb().InitClient();

    return kCIDLib::True;
}

tCIDLib::TVoid TestServersApp::LoadTests()
{
    // Load up our tests on our parent class
    AddTest(new TTest_NameSrv);
}

tCIDLib::TVoid TestServersApp::PostTest(const TTestFWTest&)
{
    // Nothing to do
}

tCIDLib::TVoid TestServersApp::PreTest(const TTestFWTest&)
{
    // NOthing to do currently
}

tCIDLib::TVoid TestServersApp::Terminate()
{
    // Terminate the ORB
    facCIDOrb().Terminate();
}



// ----------------------------------------------------------------------------
//  Declare the test app object
// ----------------------------------------------------------------------------
TestServersApp   tfwappServers;



// ----------------------------------------------------------------------------
//  Include magic main module code. We just point it at the test thread
//  entry point of the test framework app class.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"TestThread"
        , TMemberFunc<TestServersApp>(&tfwappServers, &TestServersApp::eTestThread)
    )
)

