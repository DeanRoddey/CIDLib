//
// FILE NAME: TestFW_TestInfo.cpp
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
//  Implements test info classes, that hold the information that we parse
//  out of the configuration file.
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
RTTIDecls(TTFWTestPrgInfo,TObject)
RTTIDecls(TTFWGroupInfo,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TTFWTestPrgInfo
// PREFIX: tfwtpi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTFWTestPrgInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TTFWTestPrgInfo::TTFWTestPrgInfo() :

    m_c4Id(0)
    , m_ePhase(tTestFWLib::EPhases::Connect)
    , m_strDescr()
    , m_strPath()
{
}

TTFWTestPrgInfo::TTFWTestPrgInfo(const TTFWTestPrgInfo& tfwtpiSrc) :

    m_c4Id(tfwtpiSrc.m_c4Id)
    , m_ePhase(tfwtpiSrc.m_ePhase)
    , m_strDescr(tfwtpiSrc.m_strDescr)
    , m_strErrInfo(tfwtpiSrc.m_strErrInfo)
    , m_strPath(tfwtpiSrc.m_strPath)
    , m_strName(tfwtpiSrc.m_strName)
    , m_strWorkDir(tfwtpiSrc.m_strWorkDir)
{
}

TTFWTestPrgInfo::~TTFWTestPrgInfo()
{
}


// ---------------------------------------------------------------------------
//  TTFWTestPrgInfo: Public operators
// ---------------------------------------------------------------------------
TTFWTestPrgInfo&
TTFWTestPrgInfo::operator=(const TTFWTestPrgInfo& tfwtpiSrc)
{
    if (this != &tfwtpiSrc)
    {
        m_c4Id       = tfwtpiSrc.m_c4Id;
        m_ePhase     = tfwtpiSrc.m_ePhase;
        m_strDescr   = tfwtpiSrc.m_strDescr;
        m_strErrInfo = tfwtpiSrc.m_strErrInfo;
        m_strPath    = tfwtpiSrc.m_strPath;
        m_strName    = tfwtpiSrc.m_strName;
        m_strWorkDir = tfwtpiSrc.m_strWorkDir;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TTFWTestPrgInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the test program id
tCIDLib::TCard4 TTFWTestPrgInfo::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TTFWTestPrgInfo::c4Id(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Id = c4ToSet;
    return m_c4Id;
}


// Get the description
const TString& TTFWTestPrgInfo::strDescription() const
{
    return m_strDescr;
}


// Get/set the overall phase of the test program
tTestFWLib::EPhases TTFWTestPrgInfo::ePhase() const
{
    return m_ePhase;
}

tTestFWLib::EPhases TTFWTestPrgInfo::ePhase(const tTestFWLib::EPhases eToSet)
{
    m_ePhase = eToSet;
    return m_ePhase;
}


//
//  Get/set the formatted error information. We provide non-const access to
//  that the framework can stream dirctly into it.
//
const TString& TTFWTestPrgInfo::strErrInfo() const
{
    return m_strErrInfo;
}

TString& TTFWTestPrgInfo::strErrInfo()
{
    return m_strErrInfo;
}

const TString& TTFWTestPrgInfo::strErrInfo(const TString& strToSet)
{
    m_strErrInfo = strToSet;
    return m_strErrInfo;
}


// Get the test path
const TString& TTFWTestPrgInfo::strPath() const
{
    return m_strPath;
}


// Get the test name
const TString& TTFWTestPrgInfo::strName() const
{
    return m_strName;
}


// Get the working directory
const TString& TTFWTestPrgInfo::strWorkDir() const
{
    return m_strWorkDir;
}


// Set up this test object with the descriptive info
tCIDLib::TVoid TTFWTestPrgInfo::Set(const   TString&        strName
                                    , const TString&        strDescr
                                    , const TString&        strPath
                                    , const TString&        strWorkDir)
{
    m_strDescr = strDescr;
    m_strName = strName;
    m_strPath = strPath;
    m_strWorkDir = strWorkDir;
}


// Provide access to our test results object
const TTFWTestRes& TTFWTestPrgInfo::tfwtrOutput() const
{
    return m_tfwtrOutput;
}

TTFWTestRes& TTFWTestPrgInfo::tfwtrOutput()
{
    return m_tfwtrOutput;
}





// ---------------------------------------------------------------------------
//  CLASS: TTFWGroupInfo
// PREFIX: tfwgi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTFWGroupInfo: Constructors and Destructors.
// ---------------------------------------------------------------------------
TTFWGroupInfo::TTFWGroupInfo() :

    m_c4Id(0)
    , m_eResult(tTestFWLib::ETestRes::Success)
{
}

TTFWGroupInfo::TTFWGroupInfo(const TTFWGroupInfo& tfwgiSrc) :

    m_c4Id(tfwgiSrc.m_c4Id)
    , m_colTestPrgList(tfwgiSrc.m_colTestPrgList)
    , m_eResult(tfwgiSrc.m_eResult)
    , m_strDescr(tfwgiSrc.m_strDescr)
    , m_strName(tfwgiSrc.m_strName)
{
}

TTFWGroupInfo::~TTFWGroupInfo()
{
}


// ---------------------------------------------------------------------------
//  TTFWGroupInfo: Public operators
// ---------------------------------------------------------------------------
TTFWGroupInfo& TTFWGroupInfo::operator=(const TTFWGroupInfo& tfwgiSrc)
{
    if (this != &tfwgiSrc)
    {
        m_c4Id          = tfwgiSrc.m_c4Id;
        m_eResult       = tfwgiSrc.m_eResult;
        m_strDescr      = tfwgiSrc.m_strDescr;
        m_strName       = tfwgiSrc.m_strName;

        m_colTestPrgList = tfwgiSrc.m_colTestPrgList;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TTFWGroupInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Create a new test info item using the passed info and add it to our list
tCIDLib::TVoid TTFWGroupInfo::AddTestPrg(const TTFWTestPrgInfo& tfwtpiToAdd)
{
    m_colTestPrgList.objAdd(tfwtpiToAdd);
}


//
//  Returns true if any of our contains tests were not run. We just delegate
//  to each of our test programs to ask them if any of their tests were not
//  run. If any says yes, we say yes.
//
tCIDLib::TBoolean TTFWGroupInfo::bAnyTestsNotRun() const
{
    const tCIDLib::TCard4 c4Count = m_colTestPrgList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colTestPrgList[c4Index].tfwtrOutput().bAnyNotRun())
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  Find the index of a test with the indicated name. Return maxcard if it is
//  not found.
//
tCIDLib::TCard4 TTFWGroupInfo::c4FindTest(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colTestPrgList.c4ElemCount();
    tCIDLib::TCard4 c4Ret = 0;
    for (; c4Ret < c4Count; c4Ret++)
    {
        if (m_colTestPrgList[c4Ret].strName() == strToFind)
            break;
    }
    if (c4Ret == c4Count)
        c4Ret = kCIDLib::c4MaxCard;
    return c4Ret;
}


// Get/set the group id
tCIDLib::TCard4 TTFWGroupInfo::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TTFWGroupInfo::c4Id(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Id = c4ToSet;
    return m_c4Id;
}


// Get the count of tests in this group
tCIDLib::TCard4 TTFWGroupInfo::c4TestCount() const
{
    return m_colTestPrgList.c4ElemCount();
}


// Get/set the overally group result
tTestFWLib::ETestRes TTFWGroupInfo::eResult() const
{
    return m_eResult;
}

tTestFWLib::ETestRes TTFWGroupInfo::eResult(const tTestFWLib::ETestRes eToSet)
{
    m_eResult = eToSet;
    return m_eResult;
}


// Get the description
const TString& TTFWGroupInfo::strDescription() const
{
    return m_strDescr;
}


// Get the group name
const TString& TTFWGroupInfo::strName() const
{
    return m_strName;
}


// Get the name of the group at the given index
const TString& TTFWGroupInfo::strTestAt(const tCIDLib::TCard4 c4At) const
{
    return m_colTestPrgList[c4At].strName();
}


// Set up the overall info for this group
tCIDLib::TVoid TTFWGroupInfo::Set(  const   TString&    strName
                                    , const TString&    strDescr)
{
    m_strDescr = strDescr;
    m_strName = strName;
}


// Get access to the test at the indicated index
const TTFWTestPrgInfo&
TTFWGroupInfo::tfwtpiAt(const tCIDLib::TCard4 c4At) const
{
    return m_colTestPrgList[c4At];
}

TTFWTestPrgInfo& TTFWGroupInfo::tfwtpiAt(const tCIDLib::TCard4 c4At)
{
    return m_colTestPrgList[c4At];
}

