//
// FILE NAME: TestCIDLib_CommonCollection.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/1993
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
//  This header provides some templatized tests that can be instantiated for
//  any kind of collection. This allows us to apply these tests to all
//  collection types while minimizing redundancy of code. All of the collection
//  testing modules also have some collection specific code, but this common
//  stuff helps minimize the work a lot.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ----------------------------------------------------------------------------
//  This template does some very basic collection tests that pretty much any
//  collection must support.
// ----------------------------------------------------------------------------
template <typename TCol, class TElem> tCIDLib::TVoid
TestColBasics(          TTextOutStream& strmOut
                ,       TCol&           colToTest
                , const tCIDLib::TCard4 c4InitialCount)
{
    // Make sure we didn't get called with 0 initial count
    if (!c4InitialCount)
    {
        strmOut << CUR_LN << L"Cannot have initial count of 0 for test!"
                << kCIDLib::NewLn;
        return;
    }

    // Get the reported element count
    tCIDLib::TCard4 c4ElemCount = colToTest.c4ElemCount();

    if (c4ElemCount != c4InitialCount)
    {
        strmOut << CUR_LN << L"Collection reports " << c4ElemCount
                << L" elements, but initial count said " << c4InitialCount
                << kCIDLib::NewLn;
        return;
    }

    // Get a cursor and check the elements
    typename TCol::TNCCursor cursTest(&colToTest);

    if (cursTest.c4ElemCount() != c4InitialCount)
        strmOut << CUR_LN << L"Element count not correct via cursor" << kCIDLib::NewLn;

    // So a reset on the cursor must work
    if (!cursTest.bReset())
        strmOut << CUR_LN << L"Cursor reset failed" << kCIDLib::NewLn;

    // Create aanother cursor to test invalidation
    typename TCol::TCursor cursTest2(&colToTest);

    //
    //  Delete the first element. We have to reset the cursor, which leaves it
    //  at the first element, then tell the collection to delete that value.
    //
    cursTest.bReset();
    colToTest.RemoveAt(cursTest);

    //
    //  The passed cursor should have stayed valid because it was adjusted
    //  But the other one should not be now.
    //
    if (!cursTest.bIsValid() && !colToTest.bIsEmpty())
        strmOut << CUR_LN << L"Cursor was incorrectly invalidated" << kCIDLib::NewLn;

    if (cursTest2.bIsValid())
        strmOut << CUR_LN << L"Cursor should have been invalidated" << kCIDLib::NewLn;

    // There should be one less items in the bag now
    if (colToTest.c4ElemCount() != c4InitialCount - 1)
        strmOut << CUR_LN << L"Element count not correct after flush of element" << kCIDLib::NewLn;

    // There should be one less object via the cursor also
    if (cursTest.c4ElemCount() != c4InitialCount - 1)
        strmOut << CUR_LN << L"Element count not correct via cursor after flush of element" << kCIDLib::NewLn;

    // Flush the objects all out of the collection
    colToTest.RemoveAll();

    // Make sure the element count is still right
    if (colToTest.c4ElemCount() != 0)
        strmOut << CUR_LN << L"RemoveAll did not zero element count" << kCIDLib::NewLn;

    // Check it the other way
    if (!colToTest.bIsEmpty())
        strmOut << CUR_LN << L"bIsEmpty is false but there are no elements" << kCIDLib::NewLn;

    // And both cursors should now be invalid
    if (cursTest.bIsValid() || cursTest2.bIsValid())
        strmOut << CUR_LN << L"RemoveAll failed to invalidate cursors" << kCIDLib::NewLn;


    // Test the ++ operator
    const tCIDLib::TCard4 c4Count = colToTest.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    cursTest.bReset();
    while (cursTest.bIsValid())
    {
        c4Index++;
        ++cursTest;
    }

    if (c4Index != c4Count)
        strmOut << CUR_LN << L"++ cursor loop didn't count right" << kCIDLib::NewLn;

}


template <typename TCol, class TElem> tCIDLib::TVoid
TestColCopy(TTextOutStream& strmOut, const TCol& colToTest)
{
    // Create a copy of the collection
    TCol colNew(colToTest);

    // Make sure that they are equal in both available ways
    if (!tCIDLib::bCompareElems(colToTest, colNew))
    {
        strmOut << CUR_LN
                << L"bCompareCols failed for copied collection"
                << kCIDLib::NewLn;
    }

    // Flush the collection and then copy the elements to back to it
    colNew.RemoveAll();
    colNew = colToTest;

    // Make sure that they are equal again using equality op this time
    if (colNew != colToTest)
    {
        strmOut << CUR_LN
                << L"The assigned collection is not equal to original"
                << kCIDLib::NewLn;
    }
}


template <typename TCol>
tCIDLib::TVoid DoCommonColTests(TTextOutStream&         strmOut
                                , TCol&                 colTest
                                , const tCIDLib::TCard4 c4MaxCount)
{
    // Throw in some cardinal objects
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4MaxCount; c4Ind++)
        colTest.objAdd(TCardinal(c4Ind + 1));

    //
    //  Do the copy and duplication tests.
    //
    TestColCopy<TCol, TCardinal>(strmOut, colTest);

    //
    //  Invoke the basic collection testing template function. It will do
    //  some generic things that all collections should do the same.
    //
    TestColBasics<TCol, TCardinal>(strmOut, colTest, c4MaxCount);

    //
    //  Do the basic test again but this time with only 1 element, to check
    //  for any boundary conditions.
    //
    colTest.RemoveAll();
    colTest.objAdd(TCardinal(1));
    TestColBasics<TCol, TCardinal>(strmOut, colTest, 1);

    // Throw back in some cardinal objects
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4MaxCount; c4Ind++)
        colTest.objAdd(TCardinal(c4Ind + 1));
}
