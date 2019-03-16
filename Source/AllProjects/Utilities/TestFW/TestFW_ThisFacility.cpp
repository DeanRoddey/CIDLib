//
// FILE NAME: TestFW_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2007
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
//  Implements the facility class for this facility, which provides the
//  bulk of the program implementation.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers.
// ---------------------------------------------------------------------------
#include    "TestFW.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacTestFW,TFacility)



// ---------------------------------------------------------------------------
//  CLASS: TFacTestFW
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacTestFW: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacTestFW::TFacTestFW() :

    TFacility
    (
        L"TestFW"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::None
    )
    , m_c4LineNum(0)
    , m_c4MaxLevel(kTestFWLib::c4MaxTestLevel)
    , m_c4NextGroupId(1)
    , m_c4NextTestPrgId(1)
    , m_eVerbosity(tTestFWLib::EVerbosity::Low)
    , m_fcolStats(tTestFW::EStats::Count)
    , m_pstrmErr(nullptr)
    , m_pstrmOut(nullptr)
    , m_strOutFmt(L"HTML")
{
}

TFacTestFW::~TFacTestFW()
{
}


// ---------------------------------------------------------------------------
//  TFacTestFW: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is the main thread, which is set up to be the entry point for the
//  program. It does all the setup then enters the main processing loop.
//
tCIDLib::EExitCodes
TFacTestFW::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our calling thread go
    thrThis.Sync();

    try
    {
        // Set our output streams to whatever the defaults are
        m_pstrmErr = &TSysInfo::strmErr();
        m_pstrmOut = &TSysInfo::strmOut();

        // Show the blurb
        ShowBlurb();

        //
        //  Get any parameters. This will parse the config file and verify
        //  that any groups we are asked to run exist.
        //
        if (!bParseParms())
            return tCIDLib::EExitCodes::BadParameters;

        // Process all the groups we were asked to do
        InvokeGroups();

        // And, finally, we can output the results
        GenerateResults();

        //
        //  Output a little summary at the end. If no groups failed, then
        //  the users doesn't need to really go look at the results.
        //
        if (m_fcolStats[tTestFW::EStats::FailedGrps])
            *m_pstrmOut << L"\n[FAILURES] See test results\n";
        else
            *m_pstrmOut << L"\n[SUCCESS] All groups passed\n";

        // If any were not run, then indicate that
        if (m_fcolStats[tTestFW::EStats::NotRun])
            *m_pstrmOut << L"  * Some tests were not run\n\n";
        else
            *m_pstrmOut << kCIDLib::DNewLn;

        m_pstrmOut->Flush();
    }

    catch(TError& errToCatch)
    {
        // If its not already logged, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        //
        //  Log that we exited due to an unhandled exception. Then display
        //  the information.
        //
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , L"An unhandled CIDLib exception occured. See the logs"
            , tCIDLib::ESeverities::Status
        );

        *m_pstrmOut << L"An unhandled CIDLib exception occured. Error:\n"
                    << errToCatch
                    << kCIDLib::EndLn;

        // And return a runtime error exit
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        *m_pstrmOut << L"An unhandled system exception occured\n"
                    << kCIDLib::EndLn;
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , L"An unhandled system exception occured"
            , tCIDLib::ESeverities::Status
        );

        // And return a runtime error exit
        return tCIDLib::EExitCodes::SystemException;
    }

    LogMsg
    (
        CID_FILE
        , CID_LINE
        , L"Test Framework exited normally"
        , tCIDLib::ESeverities::Status
    );
    return tCIDLib::EExitCodes::Normal;
}




// ---------------------------------------------------------------------------
//  TFacTestFW: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Checks to see if the passed output format type is valid
//
tCIDLib::TBoolean
TFacTestFW::bCheckFmtType(  const   TString&            strToCheck
                            ,       TTFWHTMLFormatter** pptfwfmToFill) const
{
    tCIDLib::TBoolean bOK = kCIDLib::False;
    if (strToCheck == L"HTML")
    {
        bOK = kCIDLib::True;
        if (pptfwfmToFill)
            *pptfwfmToFill = new TTFWHTMLFormatter;
    }
    return bOK;
}


//
//  This is called at the end after all the tests have been run. It cleans
//  out the target directory, creates an output formatter for the requested
//  format, and then passes the test results to it for output.
//
tCIDLib::TVoid TFacTestFW::GenerateResults()
{
    //
    //  First we need to clean out the results directory, if it exists. In
    //  order to be safe, we always create a TestFWRes directory and put
    //  the results in that. So, if they accidentally gave us X:\ we would
    //  not delete the whole driver because there wouldn't be a TestFWRes
    //  subdirectory.
    //
    TPathStr pathRes(m_strResPath);
    pathRes.AddLevel(L"TestFWRes");
    if (TFileSys::bExists(pathRes, tCIDLib::EDirSearchFlags::NormalDirs))
        TFileSys::RemovePath(pathRes);
    TFileSys::MakeDirectory(pathRes);

    // Ok, let's gen up the requested type of formatter
    TTFWFormatter* ptfwfm = nullptr;
    if (m_strOutFmt == L"HTML")
        ptfwfm = new TTFWHTMLFormatter;

    //
    //  If we didn't find the format, then that's a problem. Should have
    //  been checked up front, but be sure.
    //
    if (!ptfwfm)
    {
        facTestFW.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kTestFWErrs::errcGen_FmtrNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_strOutFmt
        );
    }

    // And pass the information to be formatted
    ptfwfm->FormatResult
    (
        pathRes
        , m_strCfgFile
        , m_strTestCfgName
        , m_c4MaxLevel
        , m_fcolStats
        , m_colGroupList
        , m_colGroupsToDo
    );
}


//
//  This method is called to invoke all the tests of all the groups we
//  were asked to do.
//
tCIDLib::TVoid TFacTestFW::InvokeGroups()
{
    //
    //  Create the test framework connection object that we use to connect
    //  to the test programs. Tell it that we are the creator, so it cannot
    //  already exist.
    //
    TTestFWConn tfwcnToUse(kCIDLib::True);

    // Initialize the stats
    m_fcolStats.SetAll(0);
    m_fcolStats[tTestFW::EStats::TotalGrps] = m_colGroupList.c4ElemCount();

    //
    //  For each group in our groups list, invoke all its configured test
    //  programs and store the results.
    //
    const tCIDLib::TCard4 c4GrpCount = m_colGroupsToDo.c4ElemCount();
    for (tCIDLib::TCard4 c4GrpInd = 0; c4GrpInd < c4GrpCount; c4GrpInd++)
    {
        // Find the index of this group and pull out the group object
        const tCIDLib::TCard4 c4CurGrp = c4FindGroup(m_colGroupsToDo[c4GrpInd]);
        TTFWGroupInfo& tfwgiCur = m_colGroupList[c4CurGrp];

        //
        //  Start it at the most optimisitic result, which will be brought
        //  down to the worst case scenario of any of the group's test
        //  program's result.
        //
        tfwgiCur.eResult(tTestFWLib::ETestRes::Success);

        // Iterate the tests that this guy is set up for
        const tCIDLib::TCard4 c4TestCount = tfwgiCur.c4TestCount();
        for (tCIDLib::TCard4 c4TestInd = 0; c4TestInd < c4TestCount; c4TestInd++)
        {
            TTFWTestPrgInfo& tfwtpiCur = tfwgiCur.tfwtpiAt(c4TestInd);

            //
            //  Set up the connection object with the info that we pass in
            //  to the tests.
            //
            tfwcnToUse.SetTestData(m_eVerbosity, m_c4MaxLevel);

            // Ok, invoke this test and get the results back
            tCIDLib::TBoolean bException = kCIDLib::False;
            try
            {
                // Bump the total count of test programs
                m_fcolStats[tTestFW::EStats::TotalTestPrgs]++;

                *m_pstrmOut << L"Invoking Test Program: " << tfwtpiCur.strName()
                            << kCIDLib::EndLn;

                //
                //  Set up the working directory. They indicate one if they
                //  want one. If they don't, it's empty and we'll just end
                //  up in the current directory. If it's not fully qualified,
                //  then we make it relative to the directory where the test
                //  config file was found.
                //
                TPathStr pathWD = tfwtpiCur.strWorkDir();
                if (!pathWD.bIsFullyQualified())
                {
                    pathWD.AddToBasePath(m_pathTestBaseDir);
                    pathWD.Normalize();
                }

                // Start it and wait for it to end
                TExternalProcess extpTest;
                extpTest.Start
                (
                    tfwtpiCur.strPath(), pathWD, tCIDLib::EExtProcFlags::None
                );
                tCIDLib::EExitCodes eCode = extpTest.eWaitForDeath();
                *m_pstrmOut << L"   [Completed]\n" << kCIDLib::EndLn;

                // Get the test results back out
                TTFWTestRes& tfwtrOut = tfwtpiCur.tfwtrOutput();
                TString& strErrOutput = tfwtpiCur.strErrInfo();
                tTestFWLib::EPhases ePhase = tfwcnToUse.eQueryTestRes
                (
                    tfwtrOut, strErrOutput
                );
                tfwtpiCur.ePhase(ePhase);

                // Add the count of results to the total tests count
                m_fcolStats[tTestFW::EStats::TotalTests] += tfwtrOut.c4ResCount();

                //
                //  The phase will indicate whether the test program ran to
                //  the end successfully or not. If not, then bump stats and
                //  fail things, else check all the test results.
                //
                if (ePhase == tTestFWLib::EPhases::Completed)
                {
                    //
                    //  Go through the individual tests of this test program
                    //  and bump all the appropriate stats.
                    //
                    const TTFWTestRes& tfwtrOut = tfwtpiCur.tfwtrOutput();
                    const tCIDLib::TCard4 c4Count = tfwtrOut.c4ResCount();
                    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                    {
                        const TTFWOneTestRes& tfwotrCur = tfwtrOut.tfwotrAt(c4Index);

                        switch(tfwotrCur.eResult())
                        {
                            case tTestFWLib::ETestRes::NotRun :
                                m_fcolStats[tTestFW::EStats::NotRun]++;
                                break;

                            case tTestFWLib::ETestRes::Success :
                                m_fcolStats[tTestFW::EStats::PassedTests]++;
                                break;

                            case tTestFWLib::ETestRes::Failed :
                                m_fcolStats[tTestFW::EStats::FailedTests]++;
                                break;

                            case tTestFWLib::ETestRes::Exception :
                            case tTestFWLib::ETestRes::KrnlException :
                            case tTestFWLib::ETestRes::SysException :
                                m_fcolStats[tTestFW::EStats::Exceptions]++;
                                m_fcolStats[tTestFW::EStats::FailedTests]++;
                                break;

                            default :
                                *m_pstrmErr << L"Unknown test result\n"
                                            << kCIDLib::EndLn;
                                break;
                        };

                        //
                        //  If the test result is worse than the current result
                        //  for this group, then store that as the new group
                        //  result. But, we don't do 'not run', because that's
                        //  not a failure, it just means the test wasn't run.
                        //
                        if ((tfwotrCur.eResult() < tfwgiCur.eResult())
                        &&  (tfwotrCur.eResult() != tTestFWLib::ETestRes::NotRun))
                        {
                            tfwgiCur.eResult(tfwotrCur.eResult());
                        }

                        // If the warning flag is set, then bump the warnings
                        if (tfwotrCur.bWarning())
                            m_fcolStats[tTestFW::EStats::Warnings]++;
                    }
                }
                 else
                {
                    // The test program failed
                    m_fcolStats[tTestFW::EStats::FailedTestPrgs]++;

                    // If it was for reasons of an exception, then bump that
                    if (ePhase <= tTestFWLib::EPhases::Exception)
                        m_fcolStats[tTestFW::EStats::Exceptions]++;

                    // Fail the group (if not already worse)
                    if (tfwgiCur.eResult() > tTestFWLib::ETestRes::Failed)
                        tfwgiCur.eResult(tTestFWLib::ETestRes::Failed);
                }
            }

            catch(const TError& errToCatch)
            {
                // Fail the group
                tfwgiCur.eResult(tTestFWLib::ETestRes::Failed);

                // The test program failed and it was due to an exception
                m_fcolStats[tTestFW::EStats::FailedTestPrgs]++;
                m_fcolStats[tTestFW::EStats::Exceptions]++;

                *m_pstrmErr << L"  (Exception occurred)\n"
                            << errToCatch
                            << kCIDLib::EndLn;
            }

            catch(const TKrnlError& kerrToCatch)
            {
                // Fail the group
                tfwgiCur.eResult(tTestFWLib::ETestRes::Failed);

                // The test program failed and it was due to an exception
                m_fcolStats[tTestFW::EStats::FailedTestPrgs]++;
                m_fcolStats[tTestFW::EStats::Exceptions]++;

                *m_pstrmErr << L"  (Kernel exception occurred: KrnlErr="
                            << kerrToCatch.errcId() << L", SysErr="
                            << kerrToCatch.errcHostId() << L")"
                            << kCIDLib::EndLn;
            }

            catch(...)
            {
                // Fail the group
                tfwgiCur.eResult(tTestFWLib::ETestRes::Failed);

                // The test program failed and it was due to an exception
                m_fcolStats[tTestFW::EStats::FailedTestPrgs]++;
                m_fcolStats[tTestFW::EStats::Exceptions]++;

                *m_pstrmErr << L"  (System exception occurred)"
                            << kCIDLib::EndLn;
            }
        }

        // Fail the group if any failures
        if (tfwgiCur.eResult() >= tTestFWLib::ETestRes::NotRun)
            m_fcolStats[tTestFW::EStats::PassedGrps]++;
        else
            m_fcolStats[tTestFW::EStats::FailedGrps]++;
    }
}


