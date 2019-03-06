//
// FILE NAME: TestFWLib_TestConn.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TestFWConn class, which provides the connection
//  between the test framework and tests, and the classes that define the
//  data passed back and forth.
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
RTTIDecls(TTFWOneTestRes,TObject)
RTTIDecls(TTFWTestRes,TObject)
RTTIDecls(TTestFWConn,TObject)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace TestFWLib
{
    // -----------------------------------------------------------------------
    //  Our binary format version for the results class. We don't persist any
    //  of these objects, but we want to be able to catch version mismatches
    //  between the framework and test programs. The contained classes don't
    //  need versioning, just the to level one.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2FmtTestRes    = 1;
}




// ---------------------------------------------------------------------------
//  CLASS: TTFWOneTestRes
// PREFIX: tfwotr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTFWOneTestRes: Constructors and Destructor
// ---------------------------------------------------------------------------
TTFWOneTestRes::TTFWOneTestRes() :

    m_bWarning(kCIDLib::False)
    , m_eRes(tTestFWLib::ETestRes::NotRun)
{
}

TTFWOneTestRes::TTFWOneTestRes(const TTFWOneTestRes& tfwotrSrc) :

    m_bWarning(tfwotrSrc.m_bWarning)
    , m_eRes(tfwotrSrc.m_eRes)
    , m_strDescr(tfwotrSrc.m_strDescr)
    , m_strOutput(tfwotrSrc.m_strOutput)
    , m_strSubName(tfwotrSrc.m_strSubName)
{
}

TTFWOneTestRes::TTFWOneTestRes( const   TString&    strSubName
                                , const TString&    strDescr) :

    m_bWarning(kCIDLib::False)
    , m_eRes(tTestFWLib::ETestRes::NotRun)
    , m_strDescr(strDescr)
    , m_strSubName(strSubName)
{
}

TTFWOneTestRes::~TTFWOneTestRes()
{
}


// ---------------------------------------------------------------------------
//  TTFWOneTestRes: Public operator
// ---------------------------------------------------------------------------
TTFWOneTestRes& TTFWOneTestRes::operator=(const TTFWOneTestRes& tfwotrSrc)
{
    if (this != &tfwotrSrc)
    {
        m_bWarning   = tfwotrSrc.m_bWarning;
        m_eRes       = tfwotrSrc.m_eRes;
        m_strDescr   = tfwotrSrc.m_strDescr;
        m_strOutput  = tfwotrSrc.m_strOutput;
        m_strSubName = tfwotrSrc.m_strSubName;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TTFWOneTestRes: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the warning flag
tCIDLib::TBoolean TTFWOneTestRes::bWarning() const
{
    return m_bWarning;
}

tCIDLib::TBoolean TTFWOneTestRes::bWarning(const tCIDLib::TBoolean bToSet)
{
    m_bWarning = bToSet;
    return m_bWarning;
}


// Get/set the test result
tTestFWLib::ETestRes TTFWOneTestRes::eResult() const
{
    return m_eRes;
}

tTestFWLib::ETestRes
TTFWOneTestRes::eResult(const tTestFWLib::ETestRes eToSet)
{
    m_eRes = eToSet;
    return m_eRes;
}


// Get the description string for this test
const TString& TTFWOneTestRes::strDescription() const
{
    return m_strDescr;
}


//
//  Get the test output text. We provide a non-const so that a text output
//  stream can be put over it and passed to the test to stream general
//  descriptive output to.
//
const TString& TTFWOneTestRes::strOutput() const
{
    return m_strOutput;
}

TString& TTFWOneTestRes::strOutput()
{
    return m_strOutput;
}


// The subname (i.e. the name of this test within our overall test program)
const TString& TTFWOneTestRes::strSubName() const
{
    return m_strSubName;
}



// ---------------------------------------------------------------------------
//  TTFWOneTestRes: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTFWOneTestRes::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);
    strmToReadFrom  >> m_strSubName
                    >> m_bWarning
                    >> m_eRes
                    >> m_strDescr
                    >> m_strOutput;
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TTFWOneTestRes::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << m_strSubName
                    << m_bWarning
                    << m_eRes
                    << m_strDescr
                    << m_strOutput
                    << tCIDLib::EStreamMarkers::EndObject;
}




// ---------------------------------------------------------------------------
//  CLASS: TTFWTestRes
// PREFIX: tfwtr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTFWTestRes: Constructors and Destructor
// ---------------------------------------------------------------------------
TTFWTestRes::TTFWTestRes()
{
}

TTFWTestRes::TTFWTestRes(const TTFWTestRes& tfwtrToCopy) :

    m_colResults(tfwtrToCopy.m_colResults)
{
}

TTFWTestRes::~TTFWTestRes()
{
}


// ---------------------------------------------------------------------------
//  TTFWTestRes: Public operators
// ---------------------------------------------------------------------------
TTFWTestRes& TTFWTestRes::operator=(const TTFWTestRes& tfwtrToAssign)
{
    if (this != &tfwtrToAssign)
    {
        m_colResults = tfwtrToAssign.m_colResults;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TTFWTestRes: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Add a single test result to our list of results
tCIDLib::TVoid TTFWTestRes::AddTestResult(const TTFWOneTestRes& tfwotrToAdd)
{
    m_colResults.objAdd(tfwotrToAdd);
}


// Tells the caller if any of the stored results are failures
tCIDLib::TBoolean TTFWTestRes::bAnyFailures() const
{
    const tCIDLib::TCard4 c4Count = m_colResults.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colResults[c4Index].eResult() < tTestFWLib::ETestRes::NotRun)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Tells the caller if any tests are marked not run
tCIDLib::TBoolean TTFWTestRes::bAnyNotRun() const
{
    const tCIDLib::TCard4 c4Count = m_colResults.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colResults[c4Index].eResult() == tTestFWLib::ETestRes::NotRun)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Get the count of single test results we have
tCIDLib::TCard4 TTFWTestRes::c4ResCount() const
{
    return m_colResults.c4ElemCount();
}


// Get a single test result at the given index
const TTFWOneTestRes& TTFWTestRes::tfwotrAt(const tCIDLib::TCard4 c4At) const
{
    return m_colResults[c4At];
}



// ---------------------------------------------------------------------------
//  TTFWTestRes: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTFWTestRes::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the version to make sure we understand it
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom >> c2FmtVersion;
    if (c2FmtVersion > TestFWLib::c2FmtTestRes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Looks ok, so stream it in
    strmToReadFrom  >> m_colResults;

    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TTFWTestRes::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << TestFWLib::c2FmtTestRes
                    << m_colResults
                    << tCIDLib::EStreamMarkers::EndObject;
}




// ---------------------------------------------------------------------------
//  CLASS: TTestFWConn
// PREFIX: tfwcn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTestFWConn: Constructors and Destructor
// ---------------------------------------------------------------------------
TTestFWConn::TTestFWConn(const tCIDLib::TBoolean bCreate) :

    m_pmbufInfo(nullptr)
    , m_pmbufRes(nullptr)
    , m_pConnInfo(nullptr)
{
    // Set up the resource names for the named buffers
    const TResourceName rsnRes(L"CQSL", L"TestFW", L"TestFWRes");
    const TResourceName rsnInfo(L"CQSL", L"TestFW", L"TestFWInfo");

    // Create the info buffer
    tCIDLib::TBoolean bCreated;
    m_pmbufInfo = new TSharedMemBuf
    (
        kCIDLib::c4MemPageSize
        , kCIDLib::c4MemPageSize
        , rsnInfo
        , bCreated
        , tCIDLib::EMemAccFlags::ReadWrite
        , bCreate ? tCIDLib::ECreateActs::CreateAlways
                  : tCIDLib::ECreateActs::OpenIfExists
    );

    // Create the test results buffer
    m_pmbufRes = new TSharedMemBuf
    (
        kCIDLib::c4Sz_64K
        , kCIDLib::c4Sz_4M
        , rsnRes
        , bCreated
        , tCIDLib::EMemAccFlags::ReadWrite
        , bCreate ? tCIDLib::ECreateActs::CreateAlways
                  : tCIDLib::ECreateActs::OpenIfExists
    );

    //
    //  Set up the info structure point to point at the info buffer and
    //  zero it out if we are creating it.
    //
    m_pConnInfo = reinterpret_cast<TConnInfo*>(m_pmbufInfo->pc1Data());
    if (bCreate)
    {
        TRawMem::SetMemBuf(m_pConnInfo, tCIDLib::TCard1(0), sizeof(TConnInfo));
        m_pConnInfo->c4MaxLevel = kTestFWLib::c4MaxTestLevel;
    }
}

TTestFWConn::~TTestFWConn()
{
    //
    //  Clean up the the buffers. The buffers are named, so the invoked
    //  test program's closing them won't close the TestFW's copies.
    //
    try
    {
        delete m_pmbufInfo;
        delete m_pmbufRes;
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TTestFWConn: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Called by the test framework to query the results back out
tTestFWLib::EPhases
TTestFWConn::eQueryTestRes( TTFWTestRes&    tfwtrToFill
                            , TString&      strErrInfo)
{
    //
    //  If the phase indicates that the test program made it to the end
    //  and stored results, then stream them back in. Else, stream back in
    //  any error info.
    //
    //  Tell the input stream where the end of the buffer is, so that it
    //  will see the available data.
    //
    m_pmbufRes->SyncView();
    TBinMBufInStream strmSrc(m_pmbufRes, m_pConnInfo->c4OutBytes);
    if (m_pConnInfo->ePhase == tTestFWLib::EPhases::Completed)
        strmSrc >> tfwtrToFill;
    else if (m_pConnInfo->c4OutBytes)
        strmSrc >> strErrInfo;
    return m_pConnInfo->ePhase;
}


//
//  Called by the test program to query the passed in test data. There are
//  just a few flags so they are stored in the info structure and returned
//  as separate values.
//
tTestFWLib::EVerbosity TTestFWConn::eQueryTestData(tCIDLib::TCard4& c4MaxLevel)
{
    c4MaxLevel = m_pConnInfo->c4MaxLevel;
    return m_pConnInfo->eVerbosity;
}


//
//  These methods are called by the test app framework if it encounters an
//  error. We stream out the error info and set the phase appropriately.
//
tCIDLib::TVoid TTestFWConn::SetException(const TError& errToSet)
{
    // First format the error info to a string
    TString strErrInfo;
    {
        TTextStringOutStream strmFmt(&strErrInfo);
        strmFmt << errToSet << kCIDLib::FlushIt;
    }

    // Then stream out the formatted error info to the result buffer
    TBinMBufOutStream strmOut(m_pmbufRes);
    strmOut << strErrInfo << kCIDLib::FlushIt;
    m_pConnInfo->c4OutBytes = strmOut.c4CurSize();

    m_pConnInfo->ePhase = tTestFWLib::EPhases::Exception;
}


tCIDLib::TVoid TTestFWConn::SetKrnlException(const TKrnlError& kerrToSet)
{
    // First format the error info to a string
    TString strErrInfo;
    {
        TTextStringOutStream strmFmt(&strErrInfo);
        strmFmt << L"KrnlErr=" << kerrToSet.errcId()
                << L"HostErr=" << kerrToSet.errcHostId()
                << kCIDLib::FlushIt;
    }

    // Then stream out the formatted error info to the result buffer
    TBinMBufOutStream strmOut(m_pmbufRes);
    strmOut << strErrInfo << kCIDLib::FlushIt;
    m_pConnInfo->c4OutBytes = strmOut.c4CurSize();

    m_pConnInfo->ePhase = tTestFWLib::EPhases::KrnlException;
}


tCIDLib::TVoid TTestFWConn::SetSysException(const tCIDLib::TCard4 c4ToSet)
{
    // First format the error info to a string
    TString strErrInfo;
    {
        TTextStringOutStream strmFmt(&strErrInfo);
        strmFmt << L"SystemErr=" << c4ToSet << kCIDLib::FlushIt;
    }

    // Then stream out the formatted error info to the result buffer
    TBinMBufOutStream strmOut(m_pmbufRes);
    strmOut << strErrInfo << kCIDLib::FlushIt;
    m_pConnInfo->c4OutBytes = strmOut.c4CurSize();

    m_pConnInfo->ePhase = tTestFWLib::EPhases::SysException;
}


//
//  The test app framework calls this to store the current phase that it's
//  in. We just store it in the info buffer's structure.
//
tCIDLib::TVoid TTestFWConn::SetPhase(const tTestFWLib::EPhases ePhase)
{
    m_pConnInfo->ePhase = ePhase;
}


//
//  Called by the test framework to set the incoming test data. There are
//  just a few flags, so we don't have a separate class for this. We just
//  store them in the info buffer's structure.
//
tCIDLib::TVoid
TTestFWConn::SetTestData(const  tTestFWLib::EVerbosity  eToSet
                        , const tCIDLib::TCard4         c4MaxLevel)
{
    // Store the incoming info
    m_pConnInfo->c4MaxLevel = c4MaxLevel;
    m_pConnInfo->eVerbosity = eToSet;

    // And clear the output fields until they are explicitly set again
    m_pConnInfo->ePhase = tTestFWLib::EPhases::Connect;
    m_pConnInfo->c4OutBytes = 0;
}


//
//  Called by the test program to set the result data. This sets the phase
//  to completed, since setting the test results implies that we ran to the
//  end successfully.
//
tCIDLib::TVoid TTestFWConn::SetTestRes(const TTFWTestRes& tfwtrToSet)
{
    //
    //  Set up a binary output stream over the results buffer and stream
    //  out the results.
    //
    TBinMBufOutStream strmOut(m_pmbufRes);
    strmOut << tfwtrToSet << kCIDLib::FlushIt;

    // Store the size of the streamed data in the info buffer
    m_pConnInfo->c4OutBytes = strmOut.c4CurSize();
    m_pConnInfo->ePhase = tTestFWLib::EPhases::Completed;
}


