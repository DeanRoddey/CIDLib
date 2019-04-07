//
// FILE NAME: TestFWLib_TestApp.hpp
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
//  This is the header file for the TestFWLib_TestApp.cpp file. This
//  file implements an abstract base class that each test program can
//  derive from. It provides maximum consistency and minimum redundant
//  code in test programs. The test app just points the main thread at
//  our eTestThread method and overrides the required callbacks, and we
//  invoke all the tests.
//
//  We also define an abstract base class for each test that the test
//  program wants to run. In the LoadTests() callback, the test program
//  will gen up derivatives of this class for each test it wants to
//  run and set them on our base app class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TTestFWTest
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TESTFWLIBEXP TTestFWTest : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TTestFWTest() = delete;

        TTestFWTest(const TTestFWTest&) = delete;

        ~TTestFWTest();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TTestFWTest& operator=(const TTestFWTest&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Level() const;

        const TString& strDescription() const;

        const TString& strSubName() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TTestFWTest
        (
            const   TString&                strSubName
            , const TString&                strDescr
            , const tCIDLib::TCard4         c4Level
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Level
        //      Each test is given a level value 1 to 10, that indicates how
        //      fundmental it is (1 end of the range) or how complex and high
        //      level it is (10 end of the range.) This allows for filtering
        //      so that only tests of a given complexity level are run.
        //
        //  m_strDescr
        //  m_strSubName
        //      The derive class will provide these to us in the ctor.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Level;
        TString         m_strDescr;
        TString         m_strSubName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFWTest,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TTestFWApp
// PREFIX: tfwapp
// ---------------------------------------------------------------------------
class TESTFWLIBEXP TTestFWApp : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TTestFWApp(const TTestFWApp&) = delete;

        ~TTestFWApp();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTestFWApp& operator=(const TTestFWApp&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bInitialize
        (
                    TString&                strErr
        ) = 0;

        virtual tCIDLib::TVoid LoadTests() = 0;

        virtual tCIDLib::TVoid PostTest
        (
            const   TTestFWTest&            tfwtFinished
        ) = 0;

        virtual tCIDLib::TVoid PreTest
        (
            const   TTestFWTest&            tfwtStarting
        ) = 0;

        virtual tCIDLib::TVoid Terminate() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eTestThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tTestFWLib::EVerbosity eVerbosity() const;


    protected :
        // -------------------------------------------------------------------
        //  Class types, which the derived test app classes need to see as well
        // -------------------------------------------------------------------
        typedef TRefVector<TTestFWTest>     TTestList;


        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TTestFWApp();


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddTest
        (
                    TTestFWTest* const      ptfwtToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxLevel
        //      The maximum complexity level passed in to us by the test
        //      framework. We will only run tests of this level or lower.
        //
        //  m_eVerbosity
        //      The verbosity level that was passed in to us by the test
        //      framework. This is just made available for the tests to see
        //      via a public method, so that they can adjust their output
        //      verbosity as required.
        //
        //  m_colTests
        //      This is the list of tests that the derived class (created by
        //      the test program) set up for us to run.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxLevel;
        tTestFWLib::EVerbosity  m_eVerbosity;
        TTestList               m_colTests;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTestFWApp,TObject)
};

#pragma CIDLIB_POPPACK


