//
// FILE NAME: TestFW.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main header for the program. It brings in any other headers
//  that are needed. This program implements a test framework that is used
//  for doing unit and regression testing.
//
//  It is built on the CIDKernel and CIDLib facilities, so those two still
//  need their own tests, to verify that they are sound before trusting this
//  test framework.
//
//  This framework is designed to insure that tests themselves cannot
//  corrupt this framework. Therefore, the tests are built as separate
//  executables. They will use a TestFWLib facility which we both share.
//  This allows us to pass in some flags and info to them, and for
//  them to send us back the results of their tests. This is done via a
//  named shared memory block.
//
//  Since the block is encapsulated, neither of us has to be involved in the
//  details of how it works, and it means that even a horribly disfunctional
//  test cannot corrupt the framework. Otherwise, the results of any tests
//  that fail could be suspect due to a memory error in a previous one.
//
//  The parameters to this program are:
//
//      Required:
//          /CfgFile=Path to a test configuration file
//
//      Optional:
//          /Groups=Group 1, Group 2, etc...
//          /Verbosity=[Low | Mid | High]
//
//  The configuration file contains a list of tests that can be run, with
//  some associated information. And there is a list of groups that define
//  sets of tests that can be run. When a test group is run, the tests are
//  run in the order defined in the group. If a test is in multiple groups
//  that are run, then it will be run multiple times. If no groups are
//  indicated, then all tests are just run in the order they are defined
//  in the file.
//
//  The TestLevel value associated with each test is from 1 to 10 and
//  allows for filtering by level, where the most fundamental tests are
//  given lower numbers and the more complex, system-wide ones are given
//  higher numbers.
//
//  An example file is:
//
//      Tests=
//          Test=Strings
//              TestPath=X:\Whatever\Test.exe
//              Description=Basic String Tests
//              TestLevel=2
//          EndTest;
//
//          Test=Advanced Strings
//              TestPath=X:\Whatever\Test2.exe
//              Description=Advanced String Tests
//              TestLevel=4
//          EndTest;
//      EndTests;
//
//      Groups=
//          Group=AllStrings
//              Description=All String Tests
//              Tests=
//                  Strings
//                  Advanced Strings
//              EndTests;
//          EndGroup;
//      EndGroups;
//
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
#include    "CIDLib.hpp"
#include    "TestFWLib.hpp"



// ---------------------------------------------------------------------------
//  Facility constants namespace
// ---------------------------------------------------------------------------
namespace kTestFW
{
}


// ---------------------------------------------------------------------------
//  Facility types namespace
// ---------------------------------------------------------------------------
namespace tTestFW
{
    // -----------------------------------------------------------------------
    //  We need to keep track of lists of things
    // -----------------------------------------------------------------------
    typedef tCIDLib::TStrList   TNameList;


    // -----------------------------------------------------------------------
    //  We collect a set of statistics and this enum is used as an index into
    //  a collection of TCard4 values that we store the stats in.
    //
    //  Exceptions      - The count of test programs for which an exception either
    //                    occured while trying to run it, or which reported that it
    //                    experienced an error whlie trying to get started, so it
    //                    couldn't run.
    //  FailedGrps      - The count of groups for which all of it's contained
    //                    tests didn't succeed (a warning is considered success.)
    //  FailedTests     - The number of individual tests that failed
    //  FailedTestsPrgs - The number of test programs that failed to run to
    //                    completion or couldn't be run. An exception being
    //                    returned would count as failing to run completely.
    //  PassedXXX       - The number of groups and tests taht succeeded
    //  TotalXXX        - The total number of prgs, groups, and tests.
    //  Warnings        - The number of tests that returned warning status. It
    //                    didn't fail but might require some attention.
    // -----------------------------------------------------------------------
    enum class EStats
    {
        Exceptions
        , FailedGrps
        , FailedTests
        , FailedTestPrgs
        , NotRun
        , PassedGrps
        , PassedTests
        , TotalGrps
        , TotalTests
        , TotalTestPrgs
        , Warnings

        , Count
    };
    typedef TFundArray<tCIDLib::TCard4, EStats> TStatList;
}


// ---------------------------------------------------------------------------
//  Include our own headers in the needed order
// ---------------------------------------------------------------------------
#include    "TestFW_ErrorIds.hpp"
#include    "TestFW_MessageIds.hpp"
#include    "TestFW_TestInfo.hpp"
#include    "TestFW_Formatter.hpp"
#include    "TestFW_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object, our only global.
// ---------------------------------------------------------------------------
extern TFacTestFW facTestFW;

