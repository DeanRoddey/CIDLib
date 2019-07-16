//
// FILE NAME: TestCIDLib_KeyedHashSet.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/22/1997
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  hash map collection class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"
#include    "TestCIDLib_CommonCollect.hpp"



// -----------------------------------------------------------------------------
//  Typedef our test map. We use a TFindBuf object since its a very obvious
//  type of object for this kind of collection. The file name will be the
//  key field.
// -----------------------------------------------------------------------------
typedef TKeyedHashSet<TFindBuf,TPathStr,TStringKeyOps>   TKeyedHashSetOfTFindBuf;


// -----------------------------------------------------------------------------
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class TKeyedHashSet<TFindBuf, TPathStr, TStringKeyOps>;



// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------
static const TPathStr& pathGetKey(const TFindBuf& fndbData)
{
    return fndbData.pathFileName();
}

static tCIDLib::TVoid TestMapCommon(TTextOutStream& strmOut)
{
    // Create a hash of THashData objects
    TKeyedHashSetOfTFindBuf colTest(17, TStringKeyOps(), pathGetKey);

    // Check any params that common testing does not do
    if (colTest.c4HashModulus() != 17)
        strmOut << CUR_LN << L"Modulus set in constructor not found" << kCIDLib::EndLn;

    //
    //  Load up a listing of the current directory. We can do this with just
    //  a call to the file system class.
    //
    tCIDLib::TCard4 c4Elems = TFileSys::c4SearchDir(kCIDLib::pszAllFilesSpec, colTest);
    if (!c4Elems)
    {
        strmOut << L"Did not find any files for test" << kCIDLib::EndLn;
        return;
    }

    //
    //  Do the copy and duplication tests. We provide a standard object
    //  equality function for the element comparison.
    //
    TestColCopy<TKeyedHashSetOfTFindBuf, TFindBuf>(strmOut, colTest);

    //
    //  Invoke the basic collection testing template function. It will do
    //  some generic things that all collections should do the same.
    //
    TestColBasics<TKeyedHashSetOfTFindBuf, TFindBuf>(strmOut, colTest, c4Elems);
}


static tCIDLib::TVoid TestMapBasics(TTextOutStream& strmOut)
{
    // Create a map of our test type.
    TKeyedHashSetOfTFindBuf colTest(17, TStringKeyOps(), pathGetKey);

    // Check any params that common testing does not do
    if (colTest.c4HashModulus() != 17)
        strmOut << L"Modulus set in constructor not found" << kCIDLib::EndLn;

    //
    //  Load up a listing of the current directory. We can do this with just
    //  a call to the file system class.
    //
    tCIDLib::TCard4 c4Elems = TFileSys::c4SearchDir(kCIDLib::pszAllFilesSpec, colTest);
    if (!c4Elems)
    {
        strmOut << CUR_LN << L"Did not find any files for test" << kCIDLib::EndLn;
        return;
    }

    //
    //  Check the value of each element by key lookup. We do a separate
    //  iteration of the directory via a directory iterator to get the
    //  elements.
    //
    TDirIter diterTest;
    TFindBuf fndbCur;
    if (!diterTest.bFindFirst(kCIDLib::pszAllFilesSpec, fndbCur))
    {
        strmOut << CUR_LN << L"Did not find any files for test" << kCIDLib::EndLn;
        return;
    }

    // Save this first one for later
    TFindBuf fndbFirst(fndbCur);

    do
    {
        if (colTest.objFindByKey(fndbCur.pathFileName()) != fndbCur)
            strmOut << CUR_LN << L"The value looked up by key was wrong" << kCIDLib::EndLn;
    }   while (diterTest.bFindNext(fndbCur));

    // Remove one of the elements that we know was found
    colTest.bRemoveKey(fndbFirst.pathFileName());

    // Make sure the element count was adjusted correctly
    if (colTest.c4ElemCount() != c4Elems-1)
        strmOut << CUR_LN << L"Removing key did not adjust element count" << kCIDLib::EndLn;

    // Make sure that we cannot add a key that is already in the map.
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        colTest.objAdd(fndbCur);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_DuplicateKey))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Expected duplicate key error" << kCIDLib::EndLn;
        }
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch duplicate key insert" << kCIDLib::EndLn;

    // But we should be able to add back in the one we removed
    colTest.objAdd(fndbFirst);

    // Remove one that doesn't exist and tell it not to throw
    if (colTest.bRemoveKey(L"BlahBlahBlah", kCIDLib::False))
        strmOut << CUR_LN << L"Got true when removing non-existant element" << kCIDLib::EndLn;

    // Flush the map and check the element count
    colTest.RemoveAll();
    if (colTest.c4ElemCount() != 0)
        strmOut << CUR_LN << L"Flush did not set element count to 0" << kCIDLib::EndLn;


    //
    //  Hash sets are not ordered. So they are considered equal if they have the same
    //  keys with the same values. So create two of them, add the same values in
    //  different order, and then compare them.
    //
    {
        tCIDLib::TKVHashSet col1(7, TStringKeyOps(kCIDLib::False), &TKeyValuePair::strExtractKey);
        tCIDLib::TKVHashSet col2(7, TStringKeyOps(kCIDLib::False), &TKeyValuePair::strExtractKey);

        // Should be equal if they are both empty
        if (!tCIDLib::bCompareElems(col1, col2, TKeyValuePair::bComp))
            strmOut << CUR_LN << L"Empty hash sets should be equal" << kCIDLib::EndLn;

        col1.objAdd(TKeyValuePair(L"Test1", L"Value 1"));

        // Should not be equal since not the same number of values
        if (tCIDLib::bCompareElems(col1, col2, TKeyValuePair::bComp))
            strmOut << CUR_LN << L"Hash sets with different element counts should not be equal" << kCIDLib::EndLn;

        // now fill them both up with the same stuff (in opposite order) and compare
        col1.objAdd(TKeyValuePair(L"Test2", L"Value 2"));
        col1.objAdd(TKeyValuePair(L"Test3", L"Value 3"));

        col2.objAdd(TKeyValuePair(L"Test3", L"Value 3"));
        col2.objAdd(TKeyValuePair(L"Test2", L"Value 2"));
        col2.objAdd(TKeyValuePair(L"Test1", L"Value 1"));

        if (!tCIDLib::bCompareElems(col1, col2, TKeyValuePair::bComp))
            strmOut << CUR_LN << L"Hash sets should be equal" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestMapStreaming(TTextOutStream& strmOut)
{
    typedef tCIDLib::TKVHashSet TTestCol;

    // Create a hash set of key/value pair objects.
    TTestCol colTest(17, TStringKeyOps(), &TKeyValuePair::strExtractKey);

    // Put some values into it
    colTest.objAdd(TKeyValuePair(L"Key1", L"Value1"));
    colTest.objAdd(TKeyValuePair(L"Key2", L"Value2"));

    //
    //  Create a memory based output stream, and an input stream linked
    //  to it.
    //
    TBinMBufOutStream strmTestOut(4096UL);
    TBinMBufInStream strmTestIn(strmTestOut);

    // Stream the bag out, flush it, then stream back into it
    strmTestOut << colTest << kCIDLib::FlushIt;
    colTest.RemoveAll();
    strmTestIn >> colTest;

    // Make sure we got the right stuff back
    if (colTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN << L"Streamed in set had bad element count" << kCIDLib::EndLn;
        return;
    }

    if (!colTest.bKeyExists(L"Key1")
    ||  !colTest.bKeyExists(L"Key2"))
    {
        strmOut << CUR_LN << L"Streamed in set had missing elements" << kCIDLib::EndLn;
        return;
    }
}



// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the hash map class.
//
tCIDLib::TVoid TFacTestCIDLib::TestKeyedHashSet()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"common";
        TestMapCommon(strmOut());

        pszCurTest = L"basic";
        TestMapBasics(strmOut());

        pszCurTest = L"streaming";
        TestMapStreaming(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" hash map test" << kCIDLib::EndLn;
        throw;
    }
}
