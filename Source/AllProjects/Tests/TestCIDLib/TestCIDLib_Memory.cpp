//
// FILE NAME: TestCIDLib_Memory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This module is part of the TestCIDLib.Exe program. This module is called
//  from the program's main function. The functions in this module test the
//  memory classes to make sure that they are functioning correctly.
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
#include    "TestCIDLib.hpp"


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method handles tests that should cause the same results for any
//  memory buffer derivative, so its called for different types of buffers
//
template <class T> static
tCIDLib::TVoid CommonTests(         TTextOutStream&     strmOut
                            ,       T&                  mbufToTest
                            , const tCIDLib::TCard4     c4InitSize
                            , const tCIDLib::TCard4     c4MaxSize
                            , const tCIDLib::TBoolean   bShared)
{
    if (mbufToTest.c4Size() != c4InitSize)
    {
        strmOut << CUR_LN
                << L"Buffer size (" << mbufToTest.c4Size()
                << L" was not != to initial size (" << c4InitSize
                << L")" << kCIDLib::EndLn;
        return;
    }

    // Initialize the initial size so we have a known starting point
    mbufToTest.Set(0, 0, c4InitSize);

    T mbufCopy(mbufToTest);
    if (mbufCopy != mbufToTest)
        strmOut << CUR_LN << L"Copied buffer != original" << kCIDLib::EndLn;

    T mbufAssigned = mbufToTest;

    if (mbufAssigned != mbufToTest)
        strmOut << CUR_LN << L"Assigned buffer != original" << kCIDLib::EndLn;

    // And test through one that just checks the buffer contents
    if (!mbufAssigned.bCompare(mbufToTest))
    {
        strmOut << CUR_LN << L"Assigned buffer != original" << kCIDLib::EndLn;
    }

    //
    //  This will fail for a shared buffer because they will refer to the
    //  same actual buffer. So test it differently.
    //
    mbufToTest[0] = 16;
    if (bShared)
    {
        if (mbufToTest != mbufCopy)
        {
            strmOut << CUR_LN << L"Modified shared buffer was != to original"
            << kCIDLib::EndLn;
        }
    }
     else
    {
        if (mbufToTest == mbufCopy)
            strmOut << CUR_LN << L"Modified buffer was == to original" << kCIDLib::EndLn;

        mbufCopy = mbufToTest;
        if (mbufCopy != mbufToTest)
            strmOut << CUR_LN << L"Assigned buffer was != to original" << kCIDLib::EndLn;
    }

    // Probe the boundaries and check for bad range checks
    mbufToTest[0]               = 1;
    mbufToTest[c4InitSize - 1]  = 255;

    // See if the elements got set
    if (mbufToTest[0] != 1)
        strmOut << CUR_LN << L"Elem set via operator[] not set" << kCIDLib::EndLn;

    if (mbufToTest[c4InitSize - 1] != 255)
        strmOut << CUR_LN << L"Elem set via operator[] not set" << kCIDLib::EndLn;

    //
    //  Make sure that it catches an invalid index. It will get logged
    //  but we will trap it to prevent it showing up as an error in this
    //  test.
    //
    tCIDLib::TBoolean bCaughtIt = kCIDLib::False;
    try
    {
        mbufToTest[c4MaxSize] = 0;
    }

    catch(...)
    {
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
        strmOut << CUR_LN << L"Failed to catch bad index" << kCIDLib::EndLn;

    //
    //  Test the automatic reallocation of the buffer. We try setting the
    //  next byte after the initial size. This should expand it. Only do it
    //  if the max and init are not the same.
    //
    if (c4InitSize != c4MaxSize)
    {
        mbufToTest[c4InitSize] = 0xAC;

        if (mbufToTest.c4Size() == c4InitSize)
        {
            strmOut << CUR_LN << L"Buffer failed to expand" << kCIDLib::EndLn;
            return;
        }

        // Now try to set the last byte
        mbufToTest[c4MaxSize - 1] = 0xCA;

        if (mbufToTest.c4Size() != c4MaxSize)
        {
            strmOut << CUR_LN << L"Buffer failed to fully expand" << kCIDLib::EndLn;
            return;
        }
    }

    //
    //  Now that the buffer is fully allocated, lets just do some basic
    //  access operations.
    //
    mbufToTest[2] = 0xFF;
    if (mbufToTest.c1At(2) != 0xFF)
    {
        strmOut << CUR_LN << L"c1At returned wrong results" << kCIDLib::EndLn;
        return;
    }

    if (mbufToTest.i1At(2) != -1)
    {
        strmOut << CUR_LN << L"i1At returned wrong results" << kCIDLib::EndLn;
        return;
    }

    mbufToTest.PutCard2(0xCAFE, 2);
    if (mbufToTest.c2At(2) != 0xCAFE)
    {
        strmOut << CUR_LN << L"c2At returned wrong results" << kCIDLib::EndLn;
        return;
    }

    mbufToTest.PutInt2(-5122, 20);
    if (mbufToTest.i2At(20) != -5122)
    {
        strmOut << CUR_LN << L"i2At returned wrong results" << kCIDLib::EndLn;
        return;
    }

    mbufToTest.PutFloat4(4.1F, 16);
    if (mbufToTest.f4At(16) != 4.1F)
    {
        strmOut << CUR_LN << L"f4At returned wrong results" << kCIDLib::EndLn;
        return;
    }

    mbufToTest.PutFloat8(8.1, 16);
    if (mbufToTest.f8At(16) != 8.1)
    {
        strmOut << CUR_LN << L"f8At returned wrong results" << kCIDLib::EndLn;
        return;
    }

    mbufToTest.PutInt4(2554, 18);
    if (mbufToTest.i4At(18) != 2554)
    {
        strmOut << CUR_LN << L"i4At returned wrong results" << kCIDLib::EndLn;
        return;
    }

    //
    //  Now do a fill of the buffer with a particular fill byte and
    //  check that it happened.
    //
    mbufToTest.Set(0xFF);

    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4InitSize; c4Ind++)
    {
        if (mbufToTest[c4Ind] != 0xFF)
        {
            strmOut << CUR_LN
                    << L"Element at index " << c4Ind << L" was "
                    << mbufToTest[c4Ind] << kCIDLib::EndLn;
        }
    }

    //
    //  And do some other fills that probe the end to see if any bogus
    //  exceptions are triggered. And do one that should and make sure that
    //  it does.
    //
    mbufToTest.Set(0xAF, c4InitSize - 10);
    mbufToTest.Set(0xAD, c4MaxSize - 15, 15);

    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        mbufToTest.Set(0xAD, c4MaxSize - 15, 16);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcMBuf_BufOverflow))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Incorrect exception caught" << kCIDLib::EndLn;
            return;
        }

        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch overflow buffer" << kCIDLib::EndLn;

    //
    //  Do the same for the checksum mechanism. Make sure it does and does
    //  not cause exceptions as appropriate.
    //
    mbufToTest.c4CheckSum(c4MaxSize - 20);
    mbufToTest.c4CheckSum(c4MaxSize - 20, 20);

    bGotIt = kCIDLib::False;
    try
    {
        mbufToTest.c4CheckSum(c4MaxSize - 20, 21);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcMBuf_BufOverflow))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
            return;
        }

        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch buffer overflow" << kCIDLib::EndLn;


    //
    //  And do the same thing for the hash calculator method.
    //
    mbufToTest.hshCalcHash(17);
    mbufToTest.hshCalcHash(17, c4MaxSize - 20);

    bGotIt = kCIDLib::False;
    try
    {
        mbufToTest.hshCalcHash(17, c4MaxSize);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcGen_IndexError))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
            return;
        }

        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch index error" << kCIDLib::EndLn;

    //
    //  Do some copy in and out operations to probe for errors in the
    //  range and index checking.
    //
    tCIDLib::TCard1 ac1TestBuf[16];

    mbufToTest.CopyOut(ac1TestBuf, 16);
    mbufToTest.CopyOut(ac1TestBuf, 16, c4MaxSize-16);

    bGotIt = kCIDLib::False;
    try
    {
        mbufToTest.CopyOut(ac1TestBuf, 16, c4MaxSize-15);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcMBuf_BufOverflow))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
            return;
        }

        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch buffer overflow" << kCIDLib::EndLn;


    bGotIt = kCIDLib::False;
    try
    {
        mbufToTest.CopyOut(ac1TestBuf, 16, c4MaxSize);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcGen_IndexError))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
            return;
        }

        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch index error" << kCIDLib::EndLn;


    mbufToTest.CopyIn(ac1TestBuf, 16);
    mbufToTest.CopyIn(ac1TestBuf, 16, c4MaxSize-16);

    bGotIt = kCIDLib::False;
    try
    {
        mbufToTest.CopyIn(ac1TestBuf, 16, c4MaxSize-15);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcMBuf_BufOverflow))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
            return;
        }

        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch buffer overflow" << kCIDLib::EndLn;


    bGotIt = kCIDLib::False;
    try
    {
        mbufToTest.CopyIn(ac1TestBuf, 16, c4MaxSize);
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcGen_IndexError))
        {
            TModule::LogEventObj(errToCatch);
            strmOut << CUR_LN << L"Got the wrong exception" << kCIDLib::EndLn;
            return;
        }

        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
        strmOut << CUR_LN << L"Failed to catch index error" << kCIDLib::EndLn;
}


static tCIDLib::TVoid TestCommon(TTextOutStream& strmOut)
{
    // Create a heap buffer and do the common tests on it
    {
        THeapBuf mbufHeap(800, 3100, 16);

        strmOut << L"Common tests of heap buffers" << kCIDLib::EndLn;
        CommonTests(strmOut, mbufHeap, 800, 3100, kCIDLib::False);
    }

    // And a system buffer
    {
        TSysBuf mbufSys(150, 7800, tCIDLib::EMemAccFlags::ReadWrite);

        strmOut << L"Common tests of sys buffers" << kCIDLib::EndLn;
        CommonTests
        (
            strmOut
            , mbufSys
            , 150
            , 7800
            , kCIDLib::False
        );
    }

    // And a shared memory buffer
    {
        tCIDLib::TBoolean bCreated;
        TResourceName     rsnTest(L"CharmedQuark", L"TestCIDLib", L"TestBuffer");
        TSharedMemBuf     mbufShare
        (
            1021
            , 8195
            , rsnTest
            , bCreated
            , tCIDLib::EMemAccFlags::ReadWrite
        );

        strmOut << L"Common tests of share buffers" << kCIDLib::EndLn;
        CommonTests
        (
            strmOut
            , mbufShare
            , 1021
            , 8195
            , kCIDLib::True
        );
    }
}



//
//  This one tests some stuff that is specific to particular types of
//  buffers, so it cannot be tested in the common tests above.
//
static tCIDLib::TVoid TestSpecialCases(TTextOutStream& strmOut)
{
    tCIDLib::TBoolean   bGotIt;

    //
    //  Test the sizes that got set. Note that these guys should always
    //  go in system page size increments.
    //
    const tCIDLib::TCard4 c4HalfPage = kCIDLib::c4MemPageSize / 2;
    TSysBuf mbufSysTest(c4HalfPage);

    // The current and max size should be the indicated size
    if (mbufSysTest.c4Size() != c4HalfPage)
    {
        strmOut << CUR_LN
                << L"Buffer size (" << mbufSysTest.c4Size()
                << L") was not != to init size" << kCIDLib::EndLn;
    }
    if (mbufSysTest.c4MaxSize() != c4HalfPage)
    {
        strmOut << CUR_LN
                << L"Buffer max size (" << mbufSysTest.c4MaxSize()
                << L" did not default to 1 page" << kCIDLib::EndLn;
    }

    // The access flags should have defaults to readwrite
    if (mbufSysTest.eMemAccFlags() != tCIDLib::EMemAccFlags::ReadWrite)
    {
        strmOut << CUR_LN
                << L"Buffer did not default to read/write access" << kCIDLib::EndLn;
    }

    //
    //  Now do a shared memory buffer. The default is to create or open
    //  it, so this should work despite it not already existing.
    //
    tCIDLib::TBoolean bCreated;
    TResourceName     rsnTest(L"CharmedQuark", L"TestCIDLib", L"TestBuffer");
    TSharedMemBuf     mbufShareTest
    (
        kCIDLib::c4MemPageSize
        , kCIDLib::c4MemPageSize
        , rsnTest
        , bCreated
        , tCIDLib::EMemAccFlags::ReadWrite
    );

    if (mbufShareTest.c4Size() != kCIDLib::c4MemPageSize)
    {
        strmOut << CUR_LN
                << L"Buffer size (" << mbufShareTest.c4Size()
                << L" was not != to constructor size" << kCIDLib::EndLn;
    }

    if (mbufShareTest.c4MaxSize() != kCIDLib::c4MemPageSize)
    {
        strmOut << CUR_LN
                << L"Max size (" << mbufShareTest.c4Size()
                << L" was not != to constructor size" << kCIDLib::EndLn;
    }

    if (mbufShareTest.c4AllocatedPages() != 1)
    {
        strmOut << CUR_LN
                << L"Allocated pages (" << mbufShareTest.c4AllocatedPages()
                << L" was not to 1" << kCIDLib::EndLn;
    }

    if (mbufShareTest.rsnName() != rsnTest)
    {
        strmOut << CUR_LN << L"Buffer name was: "
                << mbufShareTest.rsnName() << kCIDLib::EndLn;
    }

    if (mbufShareTest.eAccess() != tCIDLib::EMemAccFlags::ReadWrite)
        strmOut << CUR_LN << L"Access flag was not 'read/write'" << kCIDLib::EndLn;

    //
    //  Test that the create actions work correctly for a shared memory
    //  object. Do one that we know does not exist, and do an 'open if
    //  exists' which should fail.
    //
    bGotIt = kCIDLib::False;
    try
    {
        TResourceName   rsnFail(L"CharmedQuark", L"TestCIDLib", L"DontExist");
        TSharedMemBuf   mbufFailTest
        (
            kCIDLib::c4MemPageSize
            , kCIDLib::c4MemPageSize
            , rsnFail
            , bCreated
            , tCIDLib::EMemAccFlags::ReadWrite
            , tCIDLib::ECreateActs::OpenIfExists
        );
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        strmOut << CUR_LN << L"Named buffer should not exist" << kCIDLib::EndLn;
    }

    //
    //  For the shared buffer, we need to test a couple of alternatives.
    //  First we create a totally different one with the same name and
    //  access. It should be equal because of the way that shared buffers
    //  work.
    //
    {
        TSharedMemBuf   mbufShareCopy
        (
            kCIDLib::c4MemPageSize
            , kCIDLib::c4MemPageSize
            , rsnTest
            , bCreated
            , tCIDLib::EMemAccFlags::ReadWrite
            , tCIDLib::ECreateActs::OpenIfExists
        );

        if (mbufShareCopy != mbufShareTest)
            strmOut << CUR_LN << L"Shared buffer != original" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestBufferExpansion(TTextOutStream& strmOut)
{
    tCIDLib::TBoolean   bGotIt;
    TSysBuf             mbufSysTest(0xFF, 0x10000);

    // Test the initial sizes
    if (mbufSysTest.c4Size() != 0xFF)
    {
        strmOut << CUR_LN
                << L"Initial buffer size (" << mbufSysTest.c4Size()
                << L" was not 0xFF" << kCIDLib::EndLn;
    }

    if (mbufSysTest.c4MaxSize() != 0x10000)
    {
        strmOut << CUR_LN
                << L"Buffer max size (" << mbufSysTest.c4MaxSize()
                << L" was not 64K" << kCIDLib::EndLn;
    }

    //
    //  Commit another page of memory and check the size to make sure that
    //  its now two pages.
    //
    mbufSysTest.Reallocate(8192);

    if (mbufSysTest.c4Size() != 8192)
    {
        strmOut << CUR_LN
                << L"Buffer size (" << mbufSysTest.c4Size()
                << L" was not reallocated to 8192" << kCIDLib::EndLn;
    }

    if (mbufSysTest.c4AllocatedPages() != TRawMem::c4PagesCovered(8192))
    {
        strmOut << CUR_LN
                << L"Buffer size (" << mbufSysTest.c4Size()
                << L" was not reallocated to 2 pages" << kCIDLib::EndLn;
    }

    //
    //  Now make sure it auto-expands via an index access within the
    //  max size but over the current committed size.
    //
    bGotIt = kCIDLib::False;
    try
    {
        mbufSysTest[8195] = 1;
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (bGotIt)
    {
        strmOut << CUR_LN
                << L"Index operator access failed to reallocate" << kCIDLib::EndLn;
    }

    // The size should now be 3 pages
    if (mbufSysTest.c4AllocatedPages() != 3)
    {
        strmOut << CUR_LN
                << L"Buffer size (" << mbufSysTest.c4Size()
                << L" was not reallocated to 3 pages" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestLazyShared(TTextOutStream& strmOut)
{
    tCIDLib::TBoolean bCreated;
    TResourceName     rsnLazy(L"CharmedQuark", L"TestCIDLib", L"Lazy");
    TSharedMemBuf     mbufLazy
    (
        kCIDLib::c4MemPageSize + 1
        , (kCIDLib::c4MemPageSize * 4) + 1
        , rsnLazy
        , bCreated
        , tCIDLib::EMemAccFlags::ReadWrite
        , tCIDLib::ECreateActs::CreateIfNew
    );

    //
    //  Check the allocated pages. It should be two, because we the
    //  initial size was one larger than the page size.
    //
    if (mbufLazy.c4AllocatedPages() != 2)
    {
        strmOut << CUR_LN
                << L"Initial allocated pages was: "
                << mbufLazy.c4AllocatedPages()
                << kCIDLib::EndLn;
        return;
    }

    // Probe both first pages to make sure that they got committed
    try
    {
        mbufLazy[0] = 1;
        mbufLazy[kCIDLib::c4MemPageSize] = 2;
    }

    catch(...)
    {
        strmOut << CUR_LN
                << L"Got an exception on supposedly initially commited pages"
                << kCIDLib::EndLn;
        return;
    }


    // And it should auto-extend for us
    try
    {
        mbufLazy[(kCIDLib::c4MemPageSize * 2)] = 1;
    }

    catch(...)
    {
        strmOut << CUR_LN
                << L"The buffer did not automatically expand"
                << kCIDLib::EndLn;
    }

    if (mbufLazy.c4AllocatedPages() != 3)
    {
        strmOut << CUR_LN
                << L"Auto-expanded to "
                << mbufLazy.c4AllocatedPages()
                << L" pages, but expected 3"
                << kCIDLib::EndLn;
        return;
    }

    // And again
    try
    {
        mbufLazy[(kCIDLib::c4MemPageSize * 3)] = 1;
    }

    catch(...)
    {
        strmOut << CUR_LN
                << L"The buffer did not automatically expand"
                << kCIDLib::EndLn;
    }

    if (mbufLazy.c4AllocatedPages() != 4)
    {
        strmOut << CUR_LN
                << L"Auto-expanded to "
                << mbufLazy.c4AllocatedPages()
                << L" pages, but expected 4"
                << kCIDLib::EndLn;
        return;
    }

    //  And the last time on the last byte
    try
    {
        mbufLazy[(kCIDLib::c4MemPageSize * 4)] = 1;
    }

    catch(...)
    {
        strmOut << CUR_LN
                << L"The buffer did not automatically expand"
                << kCIDLib::EndLn;
    }

    if (mbufLazy.c4AllocatedPages() != 5)
    {
        strmOut << CUR_LN
                << L"Auto-expanded to "
                << mbufLazy.c4AllocatedPages()
                << L" pages, but expected 5"
                << kCIDLib::EndLn;
        return;
    }
}


static tCIDLib::TVoid TestNamedShared(TTextOutStream& strmOut)
{
    tCIDLib::TBoolean bCreated;
    TResourceName     rsnNamed(L"CharmedQuark", L"TestCIDLib", L"NamedShared");

    {
        // Create one, which should allocate two pages
        TSharedMemBuf mbufSrc
        (
            kCIDLib::c4MemPageSize + 1
            , (kCIDLib::c4MemPageSize * 4) + 1
            , rsnNamed
            , bCreated
            , tCIDLib::EMemAccFlags::ReadWrite
            , tCIDLib::ECreateActs::CreateIfNew
        );

        //
        //  Now create another with the same name. It should be able to pick
        //  up the same access and allocation size.
        //
        TSharedMemBuf mbufTest
        (
            kCIDLib::c4MemPageSize
            , (kCIDLib::c4MemPageSize * 4) + 1
            , rsnNamed
            , bCreated
            , tCIDLib::EMemAccFlags::ReadOnly
            , tCIDLib::ECreateActs::OpenIfExists
        );

        if (mbufSrc.c4AllocatedPages() != mbufTest.c4AllocatedPages())
        {
            strmOut << CUR_LN
                    << L"Test buffer did not pick up existing attributes"
                    << kCIDLib::EndLn;
        }
    }
}


// ---------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TFacTestCIDLib::TestMemoryClasses()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"special cases";
        TestSpecialCases(strmOut());

        pszCurTest = L"common";
        TestCommon(strmOut());

        pszCurTest = L"buffer expansion";
        TestBufferExpansion(strmOut());

        pszCurTest = L"lazy shared";
        TestLazyShared(strmOut());

        pszCurTest = L"named shared";
        TestNamedShared(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"An exception occured during the "
                    << pszCurTest
                    << L" test" << kCIDLib::EndLn;
        throw;
    }
}
