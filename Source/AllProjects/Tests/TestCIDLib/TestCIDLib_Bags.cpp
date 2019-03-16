//
// FILE NAME: TestCIDLib_Bags.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  bag collection classes.
//
//  This module brings in a common set of collection tests that are implemented
//  as templates themselves so that they can be invoked for all types of
//  collections without redundancy. But some of the testing is always specific
//  to the particular collection type.
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
//  Typedef our collections used here
// -----------------------------------------------------------------------------
typedef TBag<TCardinal>         TBagOfTCardinal;
typedef TCntPtr<TString>        TCntStr;
typedef TBag<TCntStr>           TBagOfTString;
typedef TRefBag<TString>        TRefBagOfTString;
typedef TSortedBag<TFloat>      TSortedBagOfTFloat;
typedef TRefSortedBag<TString>  TRefSortedBagOfTString;


// -----------------------------------------------------------------------------
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class TBag<TCardinal>;
template class TSortedBag<TFloat>;
template class TRefSortedBag<TFloat>;


// -----------------------------------------------------------------------------
//  Local static functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestBagBasics(TTextOutStream& strmOut)
{
    //
    //  Create a bag of cardinal objects and load it up with consecutive
    //  numeric values.
    //
    TBagOfTCardinal colTest;
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 16; c4Ind++)
        colTest.objAdd(TCardinal(c4Ind+1));

    //
    //  We want to make sure that all of the values are present. We cannot
    //  assume any order for bags, so we use a check off array.
    //
    tCIDLib::TCard1 ac1Check[16];
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 16; c4Ind++)
        ac1Check[c4Ind] = 0;

    // Create a cursor
    TBagOfTCardinal::TCursor cursTest(&colTest);

    // Reset the cursor
    cursTest.bReset();
    do
    {
        tCIDLib::TCard4 c4Val = cursTest.objRCur().c4Val();

        if (c4Val > 16)
        {
            strmOut << CUR_LN << L"Bag element value > 15" << kCIDLib::EndLn;
            break;
        }

        if (c4Val < 1)
        {
            strmOut << CUR_LN << L"Bag element value < 1" << kCIDLib::EndLn;
            break;
        }

        if (ac1Check[c4Val-1])
        {
            strmOut << CUR_LN << L"Bag element value "
                    << c4Val << L" occured more than once" << kCIDLib::EndLn;
        }

        // Mark this one
        ac1Check[c4Val-1]++;

    }   while (cursTest.bNext());

    // Make sure we hit them all
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 16; c4Ind++)
    {
        if (!ac1Check[c4Ind])
        {
            strmOut << CUR_LN << L"Bag element value "
                    << c4Ind+1 << L" was not found" << kCIDLib::EndLn;
        }
    }
}


static tCIDLib::TVoid TestBagStreaming(TTextOutStream& strmOut)
{
    //
    //  Create a bag of cardinals load it up with consecutive numeric values.
    //
    TBagOfTCardinal colTest;
    tCIDLib::TCard4 c4Ind;
    for (c4Ind = 0; c4Ind < 16; c4Ind++)
        colTest.objAdd(TCardinal(c4Ind+1));

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

    // And test the values
    TBagOfTCardinal::TCursor cursTest(&colTest);
    if (!cursTest.bReset())
    {
        strmOut << CUR_LN << L"Bag cursor reset failed" << kCIDLib::EndLn;
        return;
    }

    c4Ind = 1;
    do
    {
        if (cursTest.objRCur() != c4Ind)
        {
            strmOut << CUR_LN << L"Expected value "
                    << c4Ind << L" but got " << cursTest.objRCur() << kCIDLib::EndLn;
            break;
        }
        c4Ind++;
    }   while (cursTest.bNext());
}


static tCIDLib::TVoid TestByRefBags(TTextOutStream& strmOut)
{
    //
    //  Create a by ref bag to test.
    //
    TRefBagOfTString colToTest(tCIDLib::EAdoptOpts::Adopt);

    //
    //  Fill in the array with some string objects, in an order we can easily
    //  check below.
    //
    colToTest.Add(new TString(L"This is string #1"));
    colToTest.Add(new TString(L"This is string #2"));
    colToTest.Add(new TString(L"This is string #3"));
    colToTest.Add(new TString(L"This is string #4"));
    colToTest.Add(new TString(L"This is string #5"));
    colToTest.Add(new TString(L"This is string #6"));
    colToTest.Add(new TString(L"This is string #7"));

    // We should have 7 elements now
    if (colToTest.c4ElemCount() != 7)
    {
        strmOut << CUR_LN
                << L"Element count should have been 7 but is "
                << colToTest.c4ElemCount() << kCIDLib::EndLn;
        return;
    }

    // Create a read and write cursor
    TRefBagOfTString::TCursor cursR(&colToTest);
    TRefBagOfTString::TNCCursor cursW(&colToTest);

    // Check that they agree as to the first element
    if (cursR.objRCur() != cursW.objWCur())
        strmOut << CUR_LN << L"Cursors don't agree" << kCIDLib::EndLn;

    // Iterate the strings and make sure they iterate in insert order
    tCIDLib::TCard4 c4Index = 1;
    cursR.bReset();
    cursW.bReset();
    while (cursR && cursW)
    {
        TString strTmp(L"This is string #");
        strTmp.AppendFormatted(c4Index);

        if (*cursR != strTmp)
        {
            strmOut << CUR_LN
                    << L"Read cursor didn't iterate in insertion order" << kCIDLib::EndLn;
            break;
        }

        if (cursW.objWCur() != strTmp)
        {
            strmOut << CUR_LN
                    << L"Write cursor didn't iterate in insertion order" << kCIDLib::EndLn;
            break;
        }

        c4Index++;

        ++cursR;
        ++cursW;
    }

    // Make sure that they are both now invalid
    if (cursR || cursW)
        strmOut << CUR_LN << L"Both cursors were not at the end" << kCIDLib::EndLn;

    //
    //  Reset them and make sure that they are both valid now. And they should
    //  both return true since there are elements in the collection.
    //
    if (!cursR.bReset())
        strmOut << CUR_LN << L"Read cursor reset returned false" << kCIDLib::EndLn;

    if (!cursW.bReset())
        strmOut << CUR_LN << L"Write cursor reset returned false" << kCIDLib::EndLn;

    // Flush the collection and check the element count
    colToTest.RemoveAll();
    if (!colToTest.bIsEmpty())
    {
        strmOut << CUR_LN
                << L"Flushed ref bag didn't have zero elements" << kCIDLib::EndLn;
    }

    // This should invalidate the cursors again
    if (cursR.bIsValid() || cursW.bIsValid())
        strmOut << CUR_LN << L"Flush didn not invalidate cursors" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestByRefSortedBags(TTextOutStream& strmOut)
{
    TRefSortedBagOfTString colToTest(tCIDLib::EAdoptOpts::Adopt, &TString::eComp);

    //
    //  Fill in the array with some string objects in unsorted order, so we
    //  make sure that they get sorted.
    //
    colToTest.Add(new TString(L"This is string #7"));
    colToTest.Add(new TString(L"This is string #3"));
    colToTest.Add(new TString(L"This is string #5"));
    colToTest.Add(new TString(L"This is string #1"));
    colToTest.Add(new TString(L"This is string #4"));
    colToTest.Add(new TString(L"This is string #2"));
    colToTest.Add(new TString(L"This is string #6"));

    // Create a read and write cursor
    TRefBagOfTString::TCursor cursR(&colToTest);
    TRefBagOfTString::TNCCursor cursW(&colToTest);

    // Check that they agree as to the first element
    if (cursR.objRCur() != cursW.objWCur())
        strmOut << CUR_LN << L"Cursors don't agree" << kCIDLib::EndLn;

    // Iterate the strings and make sure they iterator in insert order
    tCIDLib::TCard4 c4Index = 1;
    cursR.bReset();
    cursW.bReset();
    while (cursR && cursW)
    {
        TString strTmp(L"This is string #");
        strTmp.AppendFormatted(c4Index);

        if (*cursR != strTmp)
        {
            strmOut << CUR_LN
                    << L"Read cursor didn't iterate in insertion order" << kCIDLib::EndLn;
            break;
        }

        if (cursW.objWCur() != strTmp)
        {
            strmOut << CUR_LN
                    << L"Write cursor didn't iterate in insertion order" << kCIDLib::EndLn;
            break;
        }

        c4Index++;
        ++cursR;
        ++cursW;
    }

    // Make sure that they are both now invalid
    if (cursR.bIsValid() || cursW.bIsValid())
        strmOut << CUR_LN << L"Both cursors were not at the end" << kCIDLib::EndLn;

    //
    //  Reset them and make sure that they are both valid now. And they should
    //  both return true since there are elements in the collection.
    //
    if (!cursR.bReset())
        strmOut << CUR_LN << L"Read cursor reset returned false" << kCIDLib::EndLn;

    if (!cursW.bReset())
        strmOut << CUR_LN << L"Write cursor reset returned false" << kCIDLib::EndLn;

    // Flush the collection and check the element count
    colToTest.RemoveAll();
    if (!colToTest.bIsEmpty())
    {
        strmOut << CUR_LN
                << L"Flushed ref bag didn't have zero elements" << kCIDLib::EndLn;
    }

    // This should invalidate the cursors again
    if (cursR.bIsValid() || cursW.bIsValid())
        strmOut << CUR_LN << L"Flush didn not invalidate cursors" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestSortedBags(TTextOutStream& strmOut)
{
    // Create a bag of cardinal objects. It can hold up to 16 objects
    TSortedBagOfTFloat colTest(tCIDLib::eComp<TFloat>);

    //
    //  Throw in some cardinal objects, in reverse order. It should sort
    //  them as they are inserted.
    //
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 16; c4Ind++)
        colTest.objAdd(TFloat(16UL - c4Ind));

    // Get a cursor and check the elements
    TSortedBagOfTFloat::TCursor cursTest(&colTest);

    // Iterate the bag and make sure that they are in sorted order.
    if (!cursTest.bReset())
    {
        strmOut << CUR_LN << L"Cursor reset but there are elements available"
                << kCIDLib::EndLn;
        return;
    }

    tCIDLib::TFloat8 f8Test = 1.0;
    do
    {
        if (cursTest.objRCur() != f8Test)
        {
            strmOut << CUR_LN << L"Elements were not found in sorted order"
                    << kCIDLib::EndLn;
            break;
        }
        f8Test += 1.0;
    }   while (cursTest.bNext());
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the bag collections.
//
tCIDLib::TVoid TFacTestCIDLib::TestBags()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        TestBagBasics(strmOut());

        pszCurTest = L"streaming";
        TestBagStreaming(strmOut());

        pszCurTest = L"by ref";
        TestByRefBags(strmOut());

        pszCurTest = L"sorted";
        TestSortedBags(strmOut());

        pszCurTest = L"by ref sorted";
        TestByRefSortedBags(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" bag test" << kCIDLib::EndLn;
        throw;
    }
}
