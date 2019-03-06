//
// FILE NAME: TestMData_ChunkedFile.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  These are tests of the chunked file classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include our public header and our own specific header
// ---------------------------------------------------------------------------
#include    "TestCIDMData.hpp"
#include    "TestCIDMData_ChunkedFile.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_ChunkedBasic, TTestFWTest)
RTTIDecls(TTest_ChunkedExt, TTestFWTest)
RTTIDecls(TTest_ChunkedMeta, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedBasic: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedBasic::TTest_ChunkedBasic() :

    TTestFWTest
    (
        L"Basic Chunked File", L"Basic tests of the chunked file framework", 4
    )
{
}

TTest_ChunkedBasic::~TTest_ChunkedBasic()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedBasic: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedBasic::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Create a pair of linked in/out memory streams to test round tripping
    TBinMBufOutStream   strmTestOut(4092UL);
    TBinMBufInStream    strmTestIn(strmTestOut);

    //
    //  First read/write a default constructed object to make sure that works. And make
    //  sure the data reported for a default object is correct.
    //
    {
        //
        //  Default ctor one and do the basic sttate tests on it. If this most basic
        //  testing fails, consider it fatal and just return now. There should be no
        //  outstanding changes.
        //
        TChunkedFile chflTest;
        if (!bTestState(strmOut, chflTest, L"def ctor", CID_LINE, 1, kCIDLib::False, 0, 0, 0))
            return tTestFWLib::ETestRes::Failed;

        //
        //  The last change time stamp should not have remained zero. It should have been
        //  set to the creation time just now.
        //
        if (chflTest.enctLastChange() == 0)
        {
            strmOut << TFWCurLn << L"Default ctor file should have set last change time"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // Check that we can find the two default chunks by id
        if (!chflTest.bChunkExists(kCIDMData::strChunkId_Data)
        ||  !chflTest.bChunkExists(kCIDMData::strChunkId_Meta))
        {
            // Treat this as fatal and just give up since something is way wrong
            strmOut << TFWCurLn << L"Could not find data and/or meta chunks by id"
                    << kCIDLib::NewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // Let's round trip it
        strmTestOut.Reset();
        strmTestOut << chflTest << kCIDLib::FlushIt;

        TChunkedFile chflRound;
        strmTestIn.Reset();
        strmTestIn >> chflRound;

        if (chflRound != chflTest)
        {
            // Treat this as fatal and just give up since something is way wrong
            strmOut << TFWCurLn << L"Round tripped default ctor file was not equal"
                    << kCIDLib::NewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // The original should have no unsaved changes now
        if (chflTest.bFileChanged())
        {
            strmOut << TFWCurLn << L"Saved file still has unshaved changes"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }


    //
    //  Set up some test data we'll use to create a file, and later to test against
    //  what we read back in.
    //

    // These are stored sorted, so put them in the list unsorted to test that
    tCIDLib::TKVPFList colMeta(16);
    TKeyValFPair kvalfNew;

    // Set one of them as a file change type
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Author, L"Dean Roddey");
    colMeta.objAdd(kvalfNew);
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Version, L"1.2.3", kCIDLib::True);
    colMeta.objAdd(kvalfNew);
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Application, L"TestCIDMData");
    colMeta.objAdd(kvalfNew);
    const tCIDLib::TCard4 c4TestMetaCnt = colMeta.c4ElemCount();

    const tCIDLib::TCard4 c4InitDataSz = 1024;
    THeapBuf mbufData(c4InitDataSz, c4InitDataSz);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4InitDataSz; c4Index++)
        mbufData[c4Index] = tCIDLib::TCard1(c4Index);


    // Now try one with some data in it and manipulate the data
    {
        TChunkedFile chflTest(colMeta, c4InitDataSz, mbufData);
        tCIDLib::TBoolean bBasicRes = bTestState
        (
            strmOut
            , chflTest
            , L"init data"
            , CID_LINE
            , 1
            , kCIDLib::False
            , c4TestMetaCnt
            , c4InitDataSz
            , 0
        );

        // Consider this a fundamental failure and give up now
        if (!bBasicRes)
            return tTestFWLib::ETestRes::Failed;

        // Let's round trip it
        strmTestOut.Reset();
        strmTestOut << chflTest << kCIDLib::FlushIt;

        TChunkedFile chflRound;
        strmTestIn.Reset();
        strmTestIn >> chflRound;

        if (chflRound != chflTest)
        {
            // Treat this as fatal and just give up since something is way wrong
            strmOut << TFWCurLn << L"Round tripped init data ctor file was not equal"
                    << kCIDLib::NewLn;
            return tTestFWLib::ETestRes::Failed;
        }

        // The original should have no unsaved changes now
        if (chflTest.bFileChanged())
        {
            strmOut << TFWCurLn << L"Saved file still has unshaved changes"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // Check that the data is correct
        const TMemBuf& mbufTest = chflTest.mbufData();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4InitDataSz; c4Index++)
        {
            if (mbufData[c4Index] != tCIDLib::TCard1(c4Index))
            {
                strmOut << TFWCurLn << L"Initial data is wrong at byte "
                        << TCardinal(c4Index) << kCIDLib::NewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }

        // Do some 'by id' checks
        if (chflTest.c4DataBytes() != chflTest.c4DataBytesById(kCIDMData::strChunkId_Data))
        {
            strmOut << TFWCurLn << L"Data bytes call returned "
                    << TCardinal(chflTest.c4DataBytes())
                    << L" but the same by id returned "
                    << TCardinal(chflTest.c4DataBytesById(kCIDMData::strChunkId_Data))
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }


        // Stream the file to clear file change indicators
        strmTestOut.Reset();
        strmTestOut << chflTest << kCIDLib::FlushIt;

        //
        //  Set new data on it. We'll just change the first byte. Remember the serial
        //  number and last change stamp first.
        //
        tCIDLib::TCard4 c4OrgSerialNum = chflTest.c4SerialNum();
        tCIDLib::TEncodedTime enctOrgLast = chflTest.enctLastChange();

        // Pause a little to make sure the time stamp is different if stored
        TThread::Sleep(10);
        THeapBuf mbufNew(mbufData);
        mbufNew[0] = 0xFF;
        if (!chflTest.bSetDataChunk(c4InitDataSz, mbufNew))
        {
            strmOut << TFWCurLn << L"Set of new data returned false" << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        bBasicRes = bTestState
        (
            strmOut
            , chflTest
            , L"new data"
            , CID_LINE
            , 2
            , kCIDLib::True
            , c4TestMetaCnt
            , c4InitDataSz
            , enctOrgLast
        );
        if (!bBasicRes)
            eRes = tTestFWLib::ETestRes::Failed;

        // Stream the file to clear file change indicators
        strmTestOut.Reset();
        strmTestOut << chflTest << kCIDLib::FlushIt;

        // Set the same data again, which should not cause changes
        c4OrgSerialNum = chflTest.c4SerialNum();
        enctOrgLast = chflTest.enctLastChange();

        // Pause a little to make sure the time stamp is different if stored
        TThread::Sleep(10);
        if (chflTest.bSetDataChunk(c4InitDataSz, mbufNew))
        {
            strmOut << TFWCurLn << L"Set of equal data returned true" << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        if ((chflTest.c4SerialNum() != c4OrgSerialNum)
        ||  (chflTest.enctLastChange() != enctOrgLast))
        {
            strmOut << TFWCurLn << L"Set of equal data changed serial number or stamp"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }


        // Check the meta data
        TString strValue;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestMetaCnt; c4Index++)
        {
            if (!chflTest.bFindMetaValue(colMeta[c4Index].strKey(), strValue))
            {
                strmOut << TFWCurLn << L"Meta key " << colMeta[c4Index].strKey()
                        << L" was not found" << kCIDLib::NewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
             else if (strValue != colMeta[c4Index].strValue())
            {
                strmOut << TFWCurLn << L"Meta key " << colMeta[c4Index].strKey()
                        << L" has wrong value" << kCIDLib::NewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }

        // Stream the file to clear file change indicators
        strmTestOut.Reset();
        strmTestOut << chflTest << kCIDLib::FlushIt;

        //
        //  Setting the same metadata values again shouldn't change anything. The
        //  return should be false since no values changed. The file changed param
        //  should be false since no 'file change' marked values changed, and the
        //  overall file change method (which compares chunk hashes) should not report
        //  any change.
        //
        //  The flags will be ignored in this case.
        //
        tCIDLib::TBoolean bFileChanged;
        if (chflTest.bSetMetaValues(colMeta, bFileChanged))
        {
            strmOut << TFWCurLn << L"Setting identical meta values list returned true"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        if (chflTest.bFileChanged() || bFileChanged)
        {
            strmOut << TFWCurLn << L"Storing identical meta value list caused a change"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }


        // Reset the file and make sure it looks right
        chflTest.Reset();
        if (!bTestState(strmOut, chflTest, L"reset", CID_LINE, 1, kCIDLib::False, 0, 0, 0))
            eRes = tTestFWLib::ETestRes::Failed;
    }


    // Do a test related to changing the size of the data
    {
        TChunkedFile chflTest(colMeta, c4InitDataSz, mbufData);
        const tCIDLib::TBoolean bBasicRes = bTestState
        (
            strmOut
            , chflTest
            , L"init data"
            , CID_LINE
            , 1
            , kCIDLib::False
            , c4TestMetaCnt
            , c4InitDataSz
            , 0
        );

        // Consider this a fundamental failure and give up now
        if (!bBasicRes)
            return tTestFWLib::ETestRes::Failed;

        // Set the same data but only half of it
        if (!chflTest.bSetDataChunk(c4InitDataSz / 2, mbufData))
        {
            strmOut << TFWCurLn << L"Setting smaller data returned false"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        if (!chflTest.bFileChanged())
        {
            strmOut << TFWCurLn << L"Setting smaller data did not show a change"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // Stream the file to clear file change indicators
        strmTestOut.Reset();
        strmTestOut << chflTest << kCIDLib::FlushIt;
    }

    // Do copy and assignment tests
    {
        TChunkedFile chflFirst(colMeta, c4InitDataSz, mbufData);
        TChunkedFile chflCopy(chflFirst);

        if (chflFirst != chflCopy)
        {
            strmOut << TFWCurLn << L"Copy of file did not equal original"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        TChunkedFile chflAssign;
        chflAssign = chflFirst;
        if (chflAssign != chflCopy)
        {
            strmOut << TFWCurLn << L"Assignment of file did not equal original"
                    << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedBasic: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Avoid some redundancy by providing a test for basic attributes of a file
tCIDLib::TBoolean
TTest_ChunkedBasic::bTestState(         TTextOutStream&         strmOut
                                , const TChunkedFile&           chflTest
                                , const TString&                strName
                                , const tCIDLib::TCard4         c4Line
                                , const tCIDLib::TCard4         c4SerialNum
                                , const tCIDLib::TBoolean       bChanges
                                , const tCIDLib::TCard4         c4MetaKeys
                                , const tCIDLib::TCard4         c4Bytes
                                , const tCIDLib::TEncodedTime   enctPrevious)
        {
    // There should be unsaved changes
    if (chflTest.bFileChanged())
    {
        if (!bChanges)
        {
            strmOut << TTFWCurLn(CID_FILE, c4Line)
                    << L"Changes reported on " << strName << L" when there should be none"
                    << kCIDLib::NewLn;
            return kCIDLib::False;
        }
    }
     else
    {
        if (bChanges)
        {
            strmOut << TTFWCurLn(CID_FILE, c4Line)
                    << L"No changes reported on " << strName << L" when there should be some"
                    << kCIDLib::NewLn;
            return kCIDLib::False;
        }
    }

    if (chflTest.c4DataBytes() != c4Bytes)
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line) << strName << L" file data bytes is not "
                << TCardinal(c4Bytes)
                << kCIDLib::NewLn;
        return kCIDLib::False;
    }

    if (chflTest.c4MetaKeyCount() != c4MetaKeys)
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strName << L" file should have " << c4MetaKeys
                << L" meta keys but has " << chflTest.c4MetaKeyCount() << kCIDLib::NewLn;
        return kCIDLib::False;
    }

    // If the serial number isn't 0 test it
    if (c4SerialNum && (chflTest.c4SerialNum() != c4SerialNum))
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strName << L" file should have serial number of "
                << c4SerialNum << L"but got " << chflTest.c4SerialNum() << kCIDLib::NewLn;
        return kCIDLib::False;
    }

    // If a previous time stamp, then we must have a new one
    if ((enctPrevious && chflTest.enctLastChange() == enctPrevious))
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line) << strName
                << L" should have bumped the last change time stamp"<< kCIDLib::NewLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedExt
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedExt: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedExt::TTest_ChunkedExt() :

    TTestFWTest
    (
        L"Chunked File Ext", L"Tests chunked file extension chunks", 4
    )
{
}

TTest_ChunkedExt::~TTest_ChunkedExt()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedExt: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedExt::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Create a pair of linked in/out memory streams to test round tripping
    TBinMBufOutStream   strmTestOut(4092UL);
    TBinMBufInStream    strmTestIn(strmTestOut);

    // Set up some test data
    tCIDLib::TKVPFList colMeta(16);
    TKeyValFPair kvalfNew;

    kvalfNew.Set(kCIDMData::strChFlMetaKey_Author, L"Dean Roddey");
    colMeta.objAdd(kvalfNew);
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Version, L"1.2.3");
    colMeta.objAdd(kvalfNew);
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Application, L"TestCIDMData");
    colMeta.objAdd(kvalfNew);
    const tCIDLib::TCard4 c4TestMetaCnt = colMeta.c4ElemCount();

    const tCIDLib::TCard4 c4InitDataSz = 1024;
    THeapBuf mbufData(c4InitDataSz, c4InitDataSz);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4InitDataSz; c4Index++)
        mbufData[c4Index] = tCIDLib::TCard1(c4Index);

    // Set up the file with this initial data
    TChunkedFile chflTest(colMeta, c4InitDataSz, mbufData);

    // Should not be any changes yet
    if (chflTest.bFileChanged())
    {
        // Somethis is fundamentally wrong, so give up
        strmOut << TTFWCurLn(CID_FILE, CID_LINE)
                << L"Changes reported after initial dtor"
                << kCIDLib::NewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Set up some new data for an extension chunk and add it. Indicate this one is
    //  a data change chunk.
    //
    const tCIDLib::TCard4 c4InitExtSz = 64;
    THeapBuf mbufDCExt(c4InitExtSz, c4InitExtSz);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4InitExtSz; c4Index++)
        mbufDCExt[c4Index] = tCIDLib::TCard1(c4Index);
    chflTest.AddExtChunk(L"/TestFW/DCExt", c4InitExtSz, mbufDCExt, kCIDLib::True);

    // And set up another one that is not a data change chunk
    THeapBuf mbufExt(c4InitExtSz, c4InitExtSz);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4InitExtSz; c4Index++)
        mbufExt[c4Index] = tCIDLib::TCard1(c4Index + 1);
    chflTest.AddExtChunk(L"/TestFW/Ext", c4InitExtSz, mbufExt, kCIDLib::False);

    // Try to add one of them again with the same id
    try
    {
        chflTest.AddExtChunk(L"/TestFW/Ext", c4InitExtSz, mbufExt, kCIDLib::True);
        strmOut << TTFWCurLn(CID_FILE, CID_LINE)
                << L"Failed to catch adding two extension chunks with the same id"
                << kCIDLib::NewLn;

        // This will create a mess so give up
        return tTestFWLib::ETestRes::Failed;
    }

    catch(...)
    {
    }

    // Let's try a round trip with extension chunks now
    strmTestOut.Reset();
    strmTestOut << chflTest << kCIDLib::FlushIt;

    TChunkedFile chflRound;
    strmTestIn.Reset();
    strmTestIn >> chflRound;

    if (chflRound != chflTest)
    {
        // Treat this as fatal and just give up since something is way wrong
        strmOut << TFWCurLn << L"Round tripped extension chunk file was not equal"
                << kCIDLib::NewLn;
        return tTestFWLib::ETestRes::Failed;
    }

    // The original should have no unsaved changes now
    if (chflTest.bFileChanged())
    {
        strmOut << TFWCurLn << L"Saved file still has unshaved changes"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do some by id tests
    tCIDLib::TCard4 c4BufSz;
    if ((chflTest.mbufChunkById(L"/TestFW/Ext", c4BufSz)[0] != 1)
    ||  (c4BufSz != c4InitExtSz))
    {
        strmOut << TFWCurLn << L"Non DC extension data is incorrect"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((chflTest.mbufChunkById(L"/TestFW/DCExt", c4BufSz)[0] != 0)
    ||  (c4BufSz != c4InitExtSz))
    {
        strmOut << TFWCurLn << L"DC extension data is incorrect"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Stream the file to clear file change indicators
    strmTestOut.Reset();
    strmTestOut << chflTest << kCIDLib::FlushIt;

    //
    //  Change the DC extension and that should cause a file change and should
    //  bump the data change values.
    //
    tCIDLib::TCard4 c4OrgSerialNum = chflTest.c4SerialNum();
    tCIDLib::TEncodedTime enctOrgStamp = chflTest.enctLastChange();
    mbufDCExt[0] = 0xFF;
    if (!chflTest.bSetChunkById(L"/TestFW/DCExt", c4InitExtSz, mbufDCExt))
    {
        strmOut << TFWCurLn << L"Change to DC extension data returned false"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (!chflTest.bFileChanged())
    {
        strmOut << TFWCurLn << L"Change to DC extension data did not cause file change"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((chflTest.c4SerialNum() == c4OrgSerialNum)
    ||  (chflTest.enctLastChange() == enctOrgStamp))
    {
        strmOut << TFWCurLn << L"Change to DC extension data did not bump serialnum/stamp"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Stream the file to clear file change indicators
    strmTestOut.Reset();
    strmTestOut << chflTest << kCIDLib::FlushIt;

    //
    //  Doing the same to the non-DC extension should cause a file change, but should
    //  not bump the data change values.
    //
    c4OrgSerialNum = chflTest.c4SerialNum();
    enctOrgStamp = chflTest.enctLastChange();
    mbufExt[0] = 0xF0;
    if (!chflTest.bSetChunkById(L"/TestFW/Ext", c4InitExtSz, mbufExt))
    {
        strmOut << TFWCurLn << L"Change to extension data returned false"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (!chflTest.bFileChanged())
    {
        strmOut << TFWCurLn << L"Change to non-DC extension data did not cause file change"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((chflTest.c4SerialNum() != c4OrgSerialNum)
    ||  (chflTest.enctLastChange() != enctOrgStamp))
    {
        strmOut << TFWCurLn << L"Change to non-DC extension data bumped serialnum/stamp"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedMeta
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ChunkedMeta: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ChunkedMeta::TTest_ChunkedMeta() :

    TTestFWTest
    (
        L"Chunked File Meta", L"Tests chunked file metadata", 4
    )
{
}

TTest_ChunkedMeta::~TTest_ChunkedMeta()
{
}


// ---------------------------------------------------------------------------
//  TTest_ChunkedMeta: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ChunkedMeta::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Set up some test data
    TBinMBufOutStream   strmTestOut(4092UL);
    tCIDLib::TKVPFList colMeta(16);
    TKeyValFPair kvalfNew;

    // Mark the first two as file change keys
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Author, L"Dean Roddey", kCIDLib::True);
    colMeta.objAdd(kvalfNew);
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Version, L"1.2.3");
    colMeta.objAdd(kvalfNew);
    kvalfNew.Set(kCIDMData::strChFlMetaKey_Application, L"TestCIDMData");
    colMeta.objAdd(kvalfNew);
    const tCIDLib::TCard4 c4TestMetaCnt = colMeta.c4ElemCount();

    const tCIDLib::TCard4 c4InitDataSz = 1024;
    THeapBuf mbufData(c4InitDataSz, c4InitDataSz);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4InitDataSz; c4Index++)
        mbufData[c4Index] = tCIDLib::TCard1(c4Index);

    // Set up the file with this initial data
    TChunkedFile chflTest(colMeta, c4InitDataSz, mbufData);

    // Should not be any changes yet
    if (chflTest.bFileChanged())
    {
        // Somethis is fundamentally wrong, so give up
        strmOut << TTFWCurLn(CID_FILE, CID_LINE)
                << L"Changes reported after initial dtor"
                << kCIDLib::NewLn;
        return tTestFWLib::ETestRes::Failed;
    }


    //
    //  Change a meta value that is marked as a file change type. This should bump
    //  the serial number/stamp.
    //
    tCIDLib::TBoolean bFileChange;
    tCIDLib::TCard4 c4OrgSerialNum = chflTest.c4SerialNum();
    tCIDLib::TEncodedTime enctOrgLast = chflTest.enctLastChange();
    TThread::Sleep(10);
    if (!chflTest.bSetMetaValue(kCIDMData::strChFlMetaKey_Author, L"Bubba Gump", bFileChange))
    {
        strmOut << TFWCurLn << L"Changing existing meta value returned false"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (!chflTest.bFileChanged() || !bFileChange)
    {
        strmOut << TFWCurLn << L"Changing file change marked meta value did not cause changed"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((chflTest.c4SerialNum() == c4OrgSerialNum)
    ||  (chflTest.enctLastChange() == enctOrgLast))
    {
        strmOut << TFWCurLn << L"Changing file change value did not bump serialnum/stamp"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Stream the file to clear file change indicators
    strmTestOut.Reset();
    strmTestOut << chflTest << kCIDLib::FlushIt;

    // Do it again but for a non-file change key
    c4OrgSerialNum = chflTest.c4SerialNum();
    enctOrgLast = chflTest.enctLastChange();
    TThread::Sleep(10);
    if (!chflTest.bSetMetaValue(kCIDMData::strChFlMetaKey_Version, L"5.5.2", bFileChange))
    {
        strmOut << TFWCurLn << L"Changing meta value returned false"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // We should not get a file change indicator (no bump of serial number/stamp)
    if (bFileChange)
    {
        strmOut << TFWCurLn << L"Changing non-file change value returned file change status"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // The overall file contents should have changed though
    if (!chflTest.bFileChanged())
    {
        strmOut << TFWCurLn << L"Changing meta value did not cause file overall changed"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((chflTest.c4SerialNum() != c4OrgSerialNum)
    ||  (chflTest.enctLastChange() != enctOrgLast))
    {
        strmOut << TFWCurLn << L"Changing non-file change value bumped serialnum/stamp"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Stream the file to clear file change indicators
    strmTestOut.Reset();
    strmTestOut << chflTest << kCIDLib::FlushIt;

    // Set the key to the same value which should do nothing
    c4OrgSerialNum = chflTest.c4SerialNum();
    enctOrgLast = chflTest.enctLastChange();
    TThread::Sleep(10);
    if (chflTest.bSetMetaValue(kCIDMData::strChFlMetaKey_Version, L"5.5.2", bFileChange))
    {
        strmOut << TFWCurLn << L"Set existing meta value to same value returned true"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (chflTest.bFileChanged() || bFileChange)
    {
        strmOut << TFWCurLn << L"Setting non-file change value to same value caused changed"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((chflTest.c4SerialNum() != c4OrgSerialNum)
    ||  (chflTest.enctLastChange() != enctOrgLast))
    {
        strmOut << TFWCurLn << L"Setting non-file change value to same value bumped serialnum/stamp"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Add a new key
    const TString strTestKey(L"/CQSL/TestCIDMData/TestKey");
    chflTest.AddMetaKey(strTestKey, L"Initial Value", kCIDLib::True);

    // There should be four keys now
    if (chflTest.c4MetaKeyCount() != 4)
    {
        strmOut << TFWCurLn << L"New key did not show up in key count"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Test in the default non-case sensitive way
    if (!chflTest.bMetaValueEquals(strTestKey, L"INITIAL VALUE"))
    {
        strmOut << TFWCurLn << L"Non-case sensitive value test failed"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // ANd do case sensitive
    if (chflTest.bMetaValueEquals(strTestKey, L"INITIAL VALUE", kCIDLib::True))
    {
        strmOut << TFWCurLn << L"Case sensitive value test failed"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Try to access a non-existent key
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        chflTest.bMetaValueEquals(L"/SomeValue", L"Test");
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Did not catch query of non-existent meta key"
                << kCIDLib::NewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}

