//
// FILE NAME: TestFWLib_TestApp.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
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
//  This file implements TTestFWTest and TTestFWApp classes. These provide
//  the framework that the individual test programs use.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "TestFWLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TTestFWTest,TObject)
RTTIDecls(TTestFWApp,TObject)




// ---------------------------------------------------------------------------
//  CLASS: TTestFWTest
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTestFWTest: Destructor
// ---------------------------------------------------------------------------
TTestFWTest::~TTestFWTest()
{
}


// ---------------------------------------------------------------------------
//  TTestFWTest: Hidden Constructors
// ---------------------------------------------------------------------------
TTestFWTest::TTestFWTest(const  TString&            strSubName
                        , const TString&            strDescr
                        , const tCIDLib::TCard4     c4Level) :

    m_bLong(kCIDLib::False)
    , m_c4Level(c4Level)
    , m_strDescr(strDescr)
    , m_strSubName(strSubName)
{
}




// ---------------------------------------------------------------------------
//  CLASS: TTestFWApp
// PREFIX: tfwcn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Destructor
// ---------------------------------------------------------------------------
TTestFWApp::~TTestFWApp()
{
}


// ---------------------------------------------------------------------------
//  TTestFWApp: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TTestFWApp::eTestThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the calling thread go
    thrThis.Sync();

    //
    //  Get the standard output stream for us to output some status info.
    //  We inherit the console of the test framework who runs us.
    //
    TOutConsole strmOut;

	//
	//  In order to be able to debug the test programs, we support a /TFWDebug flag
	//	that will make us set up the connection info ourself and set some dummy
	//	info.
	//
	tCIDLib::TBoolean bDebugMode = TSysInfo::bFindCmdLineParm(L"/TFWDebug");

    //
    //  The first thing we'll do is create the connection object that
    //  connects us to the test framework, and get any test info out.
    //  That will also set it up for us to start outputing test results.
    //
    //  If we fail here, there's nothing we can since we cannot talk to the
    //  framework. So we just have to create an error out stream and
    //  report it and give up.
    //
    TTestFWConn* ptfwcnInfo = nullptr;
    try
    {
        // Pass the debug flag to let it set up itself up if in debug mode
        ptfwcnInfo = new TTestFWConn(bDebugMode);
        m_bNoLong = ptfwcnInfo->bNoLong();
        m_eVerbosity = ptfwcnInfo->eQueryTestData(m_c4MaxLevel);
    }

    catch(const TError& errToCatch)
    {
        TSysInfo::strmErr() << L"TestApp: " << TProcess::strProcessName()
                            << L" - The test connection could not be created."
                            << L"Exception=\n" << kCIDLib::NewLn
                            << errToCatch << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(const TKrnlError& kerrToCatch)
    {
        TSysInfo::strmErr() << L"TestApp: " << TProcess::strProcessName()
                            << L" - The test connection could not be created."
                            << L"KrnlErr=" << kerrToCatch.errcId()
                            << L", HostErr=" << kerrToCatch.errcHostId()
                            << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        TSysInfo::strmErr() << L"TestApp: " << TProcess::strProcessName()
                            << L" - The test connection could not be created "
                            << L"due to a system exception" << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
	//  When invoked in debug mode we want to look for some parameter that otherwise
    //  would have come via the connection info above.
    //
    if (bDebugMode)
        m_bNoLong = TSysInfo::bFindCmdLineParm(L"/NoLong");

    //
    //  Ok, we know we can now get the information back to the caller.
    //  So let's start driving the test process.
    //
    //  Create a test results object that we'll feed the results into
    //  for streaming back.
    //
    TTFWTestRes tfwtrOutput;
    try
    {
        // First we let the derived class initialize
        ptfwcnInfo->SetPhase(tTestFWLib::EPhases::Initialize);

        TString strErr;
        if (!bInitialize(strErr))
        {
            TSysInfo::strmErr() << L"TestApp: Test could not be initialized. Reason: "
                                << strErr << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        // Now have him load any tests he wants to be executed
        ptfwcnInfo->SetPhase(tTestFWLib::EPhases::LoadTests);
        LoadTests();

        // Iterate all the tests and invoke each one
        ptfwcnInfo->SetPhase(tTestFWLib::EPhases::RunTests);
        const tCIDLib::TCard4 c4Count = m_colTests.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            // Get the current test to run
            TTestFWTest& tfwtCur = *m_colTests[c4Index];

            // Out put the test name followed by 36-namelen periods
            strmOut << L"   " << tfwtCur.strSubName() << L' ';
            const tCIDLib::TCard4 c4SNLen = tfwtCur.strSubName().c4Length();
            if (c4SNLen < 40)
                strmOut << TTextOutStream::RepChars(L'.', 36 - c4SNLen);
            strmOut.Flush();

            // Create a single test results object for this guy
            TTFWOneTestRes tfwotrCur
            (
                tfwtCur.strSubName(), tfwtCur.strDescription()
            );

            //
            //  Create an output stream over the results object's output
            //  string.
            //
            TTextStringOutStream strmOutput(&tfwotrCur.strOutput());

            try
            {
                if (tfwtCur.c4Level() > m_c4MaxLevel)
                {
                    //
                    //  This one is being filtered out by the maximum level
                    //  setting. So indicate that, both in the console out
                    //  output and by writing out to the test results stream.
                    //  The test result we remain an its default 'not run'
                    //  setting.
                    //
                    strmOutput  << L"Filtered by MaxLevel setting";
                    strmOut << L" Skipped by MaxLevel" << kCIDLib::EndLn;
                }
                 else if (m_bNoLong && tfwtCur.bLong())
                {
                    strmOutput  << L"Filtered by NoLong setting";
                    strmOut << L" Skipped by NoLong" << kCIDLib::EndLn;
                }
                 else
                {
                    // It's runnable, so run it. Call the pre-test callback
                    PreTest(tfwtCur);

                    tCIDLib::TBoolean    bWarning = kCIDLib::False;
                    tTestFWLib::ETestRes eRes;
                    {
                        eRes = tfwtCur.eRunTest(strmOutput, bWarning);
                        strmOutput.Flush();
                    }

                    // Store the result code and warning flag
                    tfwotrCur.eResult(eRes);
                    tfwotrCur.bWarning(bWarning);

                    // Call the post-test callback
                    PostTest(tfwtCur);

                    if (eRes == tTestFWLib::ETestRes::Success)
                        strmOut << L" Done" << kCIDLib::EndLn;
                    else
                        strmOut << L" Failed" << kCIDLib::EndLn;
                }
            }

            catch(const TError& errToCatch)
            {
                tfwotrCur.eResult(tTestFWLib::ETestRes::Exception);
                strmOutput << errToCatch << kCIDLib::EndLn;
                strmOut << L" Exception occurred" << kCIDLib::EndLn;
            }

            catch(const TKrnlError& kerrToCatch)
            {
                tfwotrCur.eResult(tTestFWLib::ETestRes::KrnlException);
                strmOutput  << L"KrnlErr=" << kerrToCatch.errcId()
                            << L", HostErr=" << kerrToCatch.errcHostId()
                            << kCIDLib::EndLn;
                strmOut << L" Krnl exception occurred" << kCIDLib::EndLn;
            }

            catch(...)
            {
                tfwotrCur.eResult(tTestFWLib::ETestRes::SysException);
                strmOut << L" System exception occurred" << kCIDLib::EndLn;
            }

            strmOutput.Flush();

            // And add this to our results object
            tfwtrOutput.AddTestResult(tfwotrCur);
        }

        // And now terminate
        ptfwcnInfo->SetPhase(tTestFWLib::EPhases::Terminate);
        Terminate();

        // And give back the results, will set the phase to completed
        ptfwcnInfo->SetTestRes(tfwtrOutput);
    }

    catch(const TError& errToCatch)
    {
        ptfwcnInfo->SetException(errToCatch);
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(const TKrnlError& kerrToCatch)
    {
        ptfwcnInfo->SetKrnlException(kerrToCatch);
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        ptfwcnInfo->SetSysException(0);
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}


// ---------------------------------------------------------------------------
//  TTestFWApp:Hidden Constructors
// ---------------------------------------------------------------------------
TTestFWApp::TTestFWApp() :

    m_bNoLong(kCIDLib::False)
    , m_c4MaxLevel(kTestFWLib::c4MaxTestLevel)
    , m_eVerbosity(tTestFWLib::EVerbosity::Low)
    , m_colTests(tCIDLib::EAdoptOpts::Adopt, 32)
{
}


// ---------------------------------------------------------------------------
//  TTestFWApp:Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTestFWApp::AddTest(TTestFWTest* const ptfwtToAdopt)
{
    m_colTests.Add(ptfwtToAdopt);
}

