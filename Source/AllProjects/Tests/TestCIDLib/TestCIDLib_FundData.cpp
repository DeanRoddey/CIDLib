//
// FILE NAME: TestCIDLib_FundData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/1998
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
//  simple data structures that manage fundamental data types. These are
//  simple classes that are mainly for internal use, but we make them
//  available and have to test them out.
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



// ----------------------------------------------------------------------------
//  Force full instantiation of all of the templates. This will catch any
//  basic syntax errors.
// ----------------------------------------------------------------------------
template class TFundArray<tCIDLib::TCard4>;
template class TFundDeque<tCIDLib::TCard2>;
template class TFundQueue<tCIDLib::TFloat4>;
template class TFundStack<tCIDLib::TInt4>;


// ----------------------------------------------------------------------------
//  Functions for local use
// ----------------------------------------------------------------------------
static tCIDLib::TVoid TestArray(TTextOutStream& strmOut)
{
    {
        TFundArray<tCIDLib::TCard1> fcolTest(256);

        // Set some elements and read them back
        fcolTest[255] = 0xAF;
        fcolTest[0]   = 0xAF;

        if (fcolTest[255] != fcolTest[0])
            strmOut << CUR_LN << L"TCard1 element set/get test failed" << kCIDLib::EndLn;

        //
        //  Set the all entries to an arbitrary value and then check them all
        //  to make sure they were set.
        //
        fcolTest.SetAll(156);

        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < kCIDLib::c1MaxCard; c4Ind++)
        {
            if (fcolTest[c4Ind] != 156)
                strmOut << CUR_LN << L"TCard1Array value was not set to 156" << kCIDLib::EndLn;
        }

        //
        //  Zero back out all of the elements and then check them all to make
        //  sure they were cleared.
        //
        fcolTest.ZeroAll();

        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < kCIDLib::c1MaxCard; c4Ind++)
        {
            if (fcolTest[c4Ind] != 0)
                strmOut << CUR_LN << L"TCard1Array value was not set to 0" << kCIDLib::EndLn;
        }

        //
        //  Fill the array with some values and sort them. To get some semi
        //  random numbers (we don't have a random number generator at this
        //  level, its in CIDMath), we use the low bytes of the standard 16
        //  bit CRC table.
        //
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < kCIDLib::c1MaxCard; c4Ind++)
        {
            fcolTest[c4Ind] = tCIDLib::TCard1
            (
                kCIDLib::ac2CRCTable[c4Ind] & 0xFF
            );
        }

        fcolTest.Sort(tCIDLib::eComp<tCIDLib::TCard1>);

        // Now confirm the sort
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < kCIDLib::c1MaxCard; c4Ind++)
        {
            if (fcolTest[c4Ind] > fcolTest[c4Ind+1])
            {
                strmOut << CUR_LN << L"Array was not in sorted order at element: "
                        << c4Ind << kCIDLib::EndLn;
            }
        }
    }

    {
        TFundArray<tCIDLib::TCard1> fcolTest(256);

        //
        //  Make sure that all of the elements got zeroed out to start with.
        //
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 256; c4Ind++)
        {
            if (fcolTest[c4Ind] != 0)
            {
                strmOut << CUR_LN
                        << L"Initial TCard1Array value is not 0 at index "
                        << c4Ind << kCIDLib::EndLn;
                break;
            }
        }

        TFundArray<tCIDLib::TCard4> ac4Test(64, 16);

        //
        //  Make sure that all of the elements got zeroed out to start with.
        //
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 64; c4Ind++)
        {
            if (ac4Test[c4Ind] != 16)
            {
                strmOut << CUR_LN
                        << L"Initial TCard4Array value is not 16 at index "
                        << c4Ind << kCIDLib::EndLn;
                break;
            }
        }
    }

    {
        // Create an array to rotate, and number it
        TFundArray<tCIDLib::TInt4> fcolTest(8);
        for (tCIDLib::TInt4 i4Index = 0; i4Index < 8; i4Index++)
            fcolTest[i4Index] = i4Index;

        // Test the rotation methods
        fcolTest.RotateUp(2);
        if ((fcolTest[0] != 6) || (fcolTest[7] != 5))
            strmOut << CUR_LN << L"RotateUp() method did not work" << kCIDLib::EndLn;

        fcolTest.RotateDown(4);
        if ((fcolTest[0] != 2) || (fcolTest[7] != 1))
            strmOut << CUR_LN << L"RotateDown() method did not work" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestDeque(TTextOutStream& strmOut)
{
    TFundDeque<tCIDLib::TCard2> fcolTest(4);

    // There should be zero elements
    if (fcolTest.c4ElemCount())
        strmOut << CUR_LN << L"Current elements not zero after ctor" << kCIDLib::EndLn;

    if (!fcolTest.bIsEmpty())
        strmOut << CUR_LN << L"bEmpty() false after ctor" << kCIDLib::EndLn;

    // Check max elements
    if (fcolTest.c4MaxElemCount() != 4)
        strmOut << CUR_LN << L"Max elements not 4 after ctor" << kCIDLib::EndLn;

    // Try to get from an empty deque and make sure we get the exception
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        fcolTest.tGetFromFront();
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_IsEmpty))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch get from empty deque" << kCIDLib::EndLn;

    bGotIt = kCIDLib::False;
    try
    {
        fcolTest.tGetFromBack();
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_IsEmpty))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch get from empty deque" << kCIDLib::EndLn;

    // Add some elements
    fcolTest.AddAtFront(2);
    fcolTest.AddAtFront(3);

    // Should be two elements
    if (fcolTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN << L"Two adds set element count to "
                << fcolTest.c4ElemCount() << kCIDLib::EndLn;
    }

    // Push a couple more
    fcolTest.AddAtBack(4);
    fcolTest.AddAtBack(5);

    // Should be four elements
    if (fcolTest.c4ElemCount() != 4)
    {
        strmOut << CUR_LN << L"Two more adds set element count to "
                << fcolTest.c4ElemCount() << kCIDLib::EndLn;
    }

    // One more should cause a full error
    bGotIt = kCIDLib::False;
    try
    {
        fcolTest.AddAtFront(6);
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_TooManyNodes))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch add to full deque" << kCIDLib::EndLn;

    bGotIt = kCIDLib::False;
    try
    {
        fcolTest.AddAtBack(6);
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_TooManyNodes))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch add to full deque" << kCIDLib::EndLn;

    // Should also report full
    if (!fcolTest.bIsFull())
        strmOut << CUR_LN << L"Did not report full, though its full" << kCIDLib::EndLn;

    //
    //  And now lets get out the values and make sure that they are in the
    //  right order.
    //
    if ((fcolTest.tGetFromFront() != 3)
    ||  (fcolTest.tGetFromFront() != 2))
    {
        strmOut << CUR_LN << L"Gotten values not correct" << kCIDLib::EndLn;
    }

    if ((fcolTest.tGetFromBack() != 5)
    ||  (fcolTest.tGetFromBack() != 4))
    {
        strmOut << CUR_LN << L"Gotten values not correct" << kCIDLib::EndLn;
    }

    // Should have zero elements now
    if (fcolTest.c4ElemCount())
        strmOut << CUR_LN << L"Queue has elements after gets" << kCIDLib::EndLn;

    if (!fcolTest.bIsEmpty())
        strmOut << CUR_LN << L"Queue not empty after gets" << kCIDLib::EndLn;

    //
    //  Make sure that adding at the tail on an empty deque and then
    //  getting back out at the head, returns the item just added.
    //
    fcolTest.AddAtBack(2);
    if (fcolTest.tGetFromFront() != 2)
        strmOut << CUR_LN << L"Get at top returned wrong value" << kCIDLib::EndLn;

    // Test arbitrary removal. Should fail if empty
    if (fcolTest.bRemoveIfPresent(1))
        strmOut << CUR_LN << L"Should not have removed an element" << kCIDLib::EndLn;

    // Add some elements and remove on
    fcolTest.AddAtBack(1);
    fcolTest.AddAtBack(2);
    fcolTest.AddAtBack(3);

    if (!fcolTest.bRemoveIfPresent(2))
        strmOut << CUR_LN << L"Should have removed an element" << kCIDLib::EndLn;

    // Should be two elements
    if (fcolTest.c4ElemCount() != 2)
        strmOut << CUR_LN << L"Should have two elements left" << kCIDLib::EndLn;

    // The two left, from the front, should be 1 and three
    if ((fcolTest.tGetFromFront() != 1)
    ||  (fcolTest.tGetFromFront() != 3))
    {
        strmOut << CUR_LN << L"Remaining elements in wrong order" << kCIDLib::EndLn;
    }

    // Should have zero elements now
    if (fcolTest.c4ElemCount())
        strmOut << CUR_LN << L"Queue has elements after removes" << kCIDLib::EndLn;

    // Add some elements back in again
    fcolTest.AddAtFront(1);
    fcolTest.AddAtFront(2);
    fcolTest.AddAtFront(3);
    fcolTest.AddAtFront(4);

    if (!fcolTest.bRemoveIfPresent(1)
    ||  !fcolTest.bRemoveIfPresent(3)
    ||  !fcolTest.bRemoveIfPresent(4))
    {
        strmOut << CUR_LN << L"Should have removed elements" << kCIDLib::EndLn;
    }

    // Should be three elements
    if (fcolTest.c4ElemCount() != 1)
        strmOut << CUR_LN << L"Should have one element left" << kCIDLib::EndLn;

    // The the back and front element should be 2
    if (fcolTest.tGetFromFront() != 2)
        strmOut << CUR_LN << L"Remaining elements in wrong order" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestQueue(TTextOutStream& strmOut)
{
    TFundQueue<tCIDLib::TCard4> fcolTest(4);

    // There should be zero elements
    if (fcolTest.c4ElemCount())
        strmOut << CUR_LN << L"Current elements not zero after ctor" << kCIDLib::EndLn;

    if (!fcolTest.bIsEmpty())
        strmOut << CUR_LN << L"bIsEmpty() is false after ctor" << kCIDLib::EndLn;

    // Check max elements
    if (fcolTest.c4MaxElemCount() != 4)
        strmOut << CUR_LN << L"Max elements not 4 after ctor" << kCIDLib::EndLn;

    // Try to get from an empty queue and make sure we get the exception
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        fcolTest.tGet();
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_IsEmpty))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch pop of empty stack" << kCIDLib::EndLn;

    // Add some elements
    fcolTest.Add(2);
    fcolTest.Add(3);

    // Should be two elements
    if (fcolTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN << L"Two adds set element count to "
                << fcolTest.c4ElemCount() << kCIDLib::EndLn;
    }

    // Push a couple more
    fcolTest.Add(4);
    fcolTest.Add(5);

    // Should be four elements
    if (fcolTest.c4ElemCount() != 4)
    {
        strmOut << CUR_LN << L"Two more adds set element count to "
                << fcolTest.c4ElemCount() << kCIDLib::EndLn;
    }

    // One more should cause a full error
    bGotIt = kCIDLib::False;
    try
    {
        fcolTest.Add(6);
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_TooManyNodes))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch add to full queue" << kCIDLib::EndLn;

    // Should also report full
    if (!fcolTest.bIsFull())
        strmOut << CUR_LN << L"Did not report full, though its full" << kCIDLib::EndLn;

    // Peek top element
    if (fcolTest.tPeek() != 2)
        strmOut << CUR_LN << L"Peek returned wrong value" << kCIDLib::EndLn;

    //
    //  And now lets get out the values and make sure that they are in the
    //  right order.
    //
    if ((fcolTest.tGet() != 2)
    ||  (fcolTest.tGet() != 3)
    ||  (fcolTest.tGet() != 4)
    ||  (fcolTest.tGet() != 5))
    {
        strmOut << CUR_LN << L"Gotten values not correct" << kCIDLib::EndLn;
    }

    // Should have zero elements now
    if (fcolTest.c4ElemCount())
        strmOut << CUR_LN << L"Queue has elements after gets" << kCIDLib::EndLn;

    if (!fcolTest.bIsEmpty())
        strmOut << CUR_LN << L"Queue not empty after gets" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestStack(TTextOutStream& strmOut)
{
    TFundStack<tCIDLib::TCard4> fcolTest(4);

    // There should be zero elements
    if (fcolTest.c4ElemCount())
        strmOut << CUR_LN << L"Current elements not zero after ctor" << kCIDLib::EndLn;

    if (!fcolTest.bIsEmpty())
        strmOut << CUR_LN << L"bIsEmpty() false after ctor" << kCIDLib::EndLn;

    // Check max elements
    if (fcolTest.c4MaxElemCount() != 4)
        strmOut << CUR_LN << L"Max elements not 4 after ctor" << kCIDLib::EndLn;

    // A pop should fail
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        fcolTest.tPop();
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_IsEmpty))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch pop of empty stack" << kCIDLib::EndLn;

    // Push a couple elements on
    fcolTest.Push(10);
    fcolTest.Push(11);

    // Should be two elements
    if (fcolTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN << L"Two pushes set element count to "
                << fcolTest.c4ElemCount() << kCIDLib::EndLn;
    }

    // Push a couple more
    fcolTest.Push(12);
    fcolTest.Push(13);

    // Should be four elements
    if (fcolTest.c4ElemCount() != 4)
    {
        strmOut << CUR_LN << L"Two more pushes set element count to "
                << fcolTest.c4ElemCount() << kCIDLib::EndLn;
    }

    // One more should cause a full error
    bGotIt = kCIDLib::False;
    try
    {
        fcolTest.Push(14);
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_TooManyNodes))
        {
            bGotIt = kCIDLib::True;
        }
         else
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
        }
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch push of full stack" << kCIDLib::EndLn;

    // Should also report full
    if (!fcolTest.bIsFull())
        strmOut << CUR_LN << L"Did not report full, though its full" << kCIDLib::EndLn;

    // Peek top element
    if (fcolTest.tPeek() != 13)
        strmOut << CUR_LN << L"Peek returned wrong value" << kCIDLib::EndLn;

    //
    //  And now lets pop off the values and make sure that they are in the
    //  right order.
    //
    if ((fcolTest.tPop() != 13)
    ||  (fcolTest.tPop() != 12)
    ||  (fcolTest.tPop() != 11)
    ||  (fcolTest.tPop() != 10))
    {
        strmOut << CUR_LN << L"Popped values not correct" << kCIDLib::EndLn;
    }

    // Should have zero elements now
    if (fcolTest.c4ElemCount())
        strmOut << CUR_LN << L"Stack has elements after gets" << kCIDLib::EndLn;

    if (!fcolTest.bIsEmpty())
        strmOut << CUR_LN << L"Stack not empty after gets" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestFDataStreaming(TTextOutStream& strmOut)
{
    // Create one each and fill them in with known values
    TFundArray<tCIDLib::TCard4> fcolArray(4);
    fcolArray[0] = 1;
    fcolArray[1] = 2;
    fcolArray[2] = 3;
    fcolArray[3] = 4;

    TFundDeque<tCIDLib::TCard4> fcolDeque(4);
    fcolDeque.AddAtBack(1);
    fcolDeque.AddAtBack(2);
    fcolDeque.AddAtBack(3);
    fcolDeque.AddAtBack(4);

    TFundQueue<tCIDLib::TCard4> fcolQueue(4);
    fcolQueue.Add(1);
    fcolQueue.Add(2);
    fcolQueue.Add(3);
    fcolQueue.Add(4);

    TFundStack<tCIDLib::TCard4> fcolStack(4);
    fcolStack.Push(1);
    fcolStack.Push(2);
    fcolStack.Push(3);
    fcolStack.Push(4);

    // Create a binary output memory stream and stream them out
    TBinMBufOutStream strmTestOut(8192, 8192);
    strmTestOut << fcolArray << fcolDeque << fcolQueue << fcolStack
                << kCIDLib::FlushIt;

    //
    //  Create a linked input stream and stream them back in to new objects.
    //  Given them different initial sizes to make sure this doesn't
    //  cause them to appear different.
    //
    TFundArray<tCIDLib::TCard4> fcolArray2(6);
    TFundDeque<tCIDLib::TCard4> fcolDeque2(6);
    TFundQueue<tCIDLib::TCard4> fcolQueue2(6);
    TFundStack<tCIDLib::TCard4> fcolStack2(6);

    TBinMBufInStream strmTestIn(strmTestOut);
    strmTestIn >> fcolArray2 >> fcolDeque2 >> fcolQueue2 >> fcolStack2;

    // Now compare them to the originals
    if (fcolArray != fcolArray2)
        strmOut << CUR_LN << L"Streamed array != to original" << kCIDLib::EndLn;

    if (fcolDeque != fcolDeque2)
        strmOut << CUR_LN << L"Streamed deque != to original" << kCIDLib::EndLn;

    if (fcolQueue != fcolQueue2)
        strmOut << CUR_LN << L"Streamed queue != to original" << kCIDLib::EndLn;

    if (fcolStack != fcolStack2)
        strmOut << CUR_LN << L"Streamed stack != to original" << kCIDLib::EndLn;
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the fundamental data managements classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestFundData()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        // Invoke the by value array tests
        pszCurTest = L"array";
        TestArray(strmOut());

        // Invoke the by value stack tests
        pszCurTest = L"stack";
        TestStack(strmOut());

        // Invoke the by value queue tests
        pszCurTest = L"queue";
        TestQueue(strmOut());

        // Invoke the by value deque tests
        pszCurTest = L"deque";
        TestDeque(strmOut());

        // Test the by value ones for binary streaming
        pszCurTest = L"bin streaming";
        TestFDataStreaming(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" FData test" << kCIDLib::EndLn;
        throw;
    }
}
