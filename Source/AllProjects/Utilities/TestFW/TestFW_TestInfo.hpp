//
// FILE NAME: TestFW_TestInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2007
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
//  This is the header for the TestFW_TestInfo.cpp file. We define test
//  program info and group info classes. These are used to store the info
//  that we read out of the test configuration file.
//
//  The test program info class also stores the results returned from the
//  test program's execution. The group info class stores a list of testinfo
//  objects for the tests that were configured for it. Since the same test
//  could be run in multiple tests, we have to have this per-group storage,
//  instead of just a flat list of tests that the groups reference by name
//  or index.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TTFWTestPrgInfo
// PREFIX: tfwtpi
// ---------------------------------------------------------------------------
class TTFWTestPrgInfo : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors.
        // -------------------------------------------------------------------
        TTFWTestPrgInfo();

        TTFWTestPrgInfo
        (
            const   TTFWTestPrgInfo&        tfwtpiToCopy
        );

        ~TTFWTestPrgInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTFWTestPrgInfo& operator=
        (
            const   TTFWTestPrgInfo&        tfwtpiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tTestFWLib::EPhases ePhase() const;

        tTestFWLib::EPhases ePhase
        (
            const   tTestFWLib::EPhases     eToSet
        );

        const TString& strDescription() const;

        const TString& strErrInfo() const;

        TString& strErrInfo();

        const TString& strErrInfo
        (
            const   TString&                strToSet
        );

        const TString& strName() const;

        const TString& strPath() const;

        const TString& strWorkDir() const;

        tCIDLib::TVoid Set
        (
            const   TString&                strName
            , const TString&                strDescr
            , const TString&                strPath
            , const TString&                strWorkDir
        );

        const TTFWTestRes& tfwtrOutput() const;

        TTFWTestRes& tfwtrOutput();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      Each test program is assigned a sequential id by the facility
        //      object, as they are added to the list. These are used for
        //      generating per-test program links.
        //
        //  m_ePhase
        //      The overall phase of the test program, which indicates where
        //      it got to (with 'completed' indicating success.)
        //
        //  m_strDescr
        //      The descriptive text for this test prg.
        //
        //  m_strErrInfo
        //      If the test program cannot even run the tests, or we cannot
        //      run the test program, then any error information is formatted
        //      out and stored here.
        //
        //  m_strName
        //      The name of this test prg, which must be unique in a given
        //      configuration file.
        //
        //  m_strPath
        //      The path to the program that implements these tests. If it
        //      is relative, then it's assumed to be relative to the top
        //      level of the test directory (where the .TestCfg file is.)
        //
        //  m_strWorkDir
        //      The path that the test prg wants to be set as it's current
        //      directory. We set it before invoking the program. This one
        //      is optional, so it usuall is empty.
        //
        //  m_tfwtrOutput
        //      This is the test results object that will be filled in with
        //      the results of this test.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Id;
        tTestFWLib::EPhases m_ePhase;
        TString             m_strDescr;
        TString             m_strErrInfo;
        TString             m_strName;
        TString             m_strPath;
        TString             m_strWorkDir;
        TTFWTestRes         m_tfwtrOutput;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTFWTestPrgInfo,TObject)
};


//
//  Create a list of test objects in our facilty name space. This needs to be
//  seen by the group class below.
//
namespace tTestFW
{
    typedef TVector<TTFWTestPrgInfo>        TTestPrgList;
}



// ---------------------------------------------------------------------------
//  CLASS: TTFWGroupInfo
// PREFIX: tfwgi
// ---------------------------------------------------------------------------
class TTFWGroupInfo : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors.
        // -------------------------------------------------------------------
        TTFWGroupInfo();

        TTFWGroupInfo
        (
            const   TTFWGroupInfo&          tfwgiToCopy
        );

        ~TTFWGroupInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTFWGroupInfo& operator=
        (
            const   TTFWGroupInfo&          tfwgiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddTestPrg
        (
            const   TTFWTestPrgInfo&        tfwtpiToAdd
        );

        tCIDLib::TBoolean bAnyTestsNotRun() const;

        tCIDLib::TCard4 c4FindTest
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4TestCount() const;

        tTestFWLib::ETestRes eResult() const;

        tTestFWLib::ETestRes eResult
        (
            const   tTestFWLib::ETestRes    eToSet
        );

        const TString& strDescription() const;

        const TString& strName() const;

        const TString& strTestAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid Set
        (
            const   TString&                strName
            , const TString&                strDescr
        );

        const TTFWTestPrgInfo& tfwtpiAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        TTFWTestPrgInfo& tfwtpiAt
        (
            const   tCIDLib::TCard4         c4At
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      Each group is assigned a sequential id by the facility object,
        //      as they are added to the list. These are used for generating
        //      per-group links.
        //
        //  m_colTestPrgList
        //      A list of TFWTestPrgInfo objects for the tests that this group
        //      is set up for. It's a full test info object because the test
        //      might be referenced via multiple groups. So we need a place
        //      to store results info for the instance of the run for this
        //      group.
        //
        //  m_eResult
        //      During the running of the tests of this group, we set this to
        //      the lowest (worst case) result of any of the tests.
        //
        //  m_strDescr
        //      The descriptive text for this test.
        //
        //  m_strName
        //      The name of this test, which must be unique in a given
        //      configuration file.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Id;
        tTestFW::TTestPrgList   m_colTestPrgList;
        tTestFWLib::ETestRes    m_eResult;
        TString                 m_strDescr;
        TString                 m_strName;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTFWGroupInfo,TObject)
};


// Create a list of group objects in our facility name space.
namespace tTestFW
{
    typedef TVector<TTFWGroupInfo>  TGroupList;
}

