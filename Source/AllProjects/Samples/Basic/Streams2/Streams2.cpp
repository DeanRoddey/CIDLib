//
// FILE NAME: Streams2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/1997
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
//  This is the main module for the second of the streaming oriented demo
//  programs. This one just demonstrates the basics of binary streaming.
//
//  This guy has a local function that will stream a number of objects to
//  a passed binary stream. It prints out the number of byte used after
//  each object is streamed. Then it resets the stream and streams the
//  object back into different objects and compares them to the originals
//  to make sure that they came back in correctly.
//
//  The main program creates a memory based binary stream, and passes
//  it to the function for testing. Then it creates a file based binary
//  stream and passes it in also. These shoold have the exact same results
//  except one goes to a file and another goes to a memory buffer,
//  demonstrating the flexibility of streams.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  This program is very simple so it does not attempt to be language
//      independent and it does not provide its own facility object since
//      it does not need one.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDMath.hpp"


// ----------------------------------------------------------------------------
//  Local static data
//
//  strTestFile
//      This is the name of the file that we use as the backing for the file
//      stream that we test.
// ----------------------------------------------------------------------------
static const TString    strTestFile(L"TestStreamFile.Dat");


// ----------------------------------------------------------------------------
//  Local data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
// ----------------------------------------------------------------------------
static TOutConsole  conOut;


// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Streams2MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This is a testing function. It takes a stream (any derivative of the
//  binary stream class) and does some basic binary streaming operations
//  on it. It stream out some objects, then streams them back into another
//  set of objects. It then compares the two sets to make sure that they
//  came back in correctly.
//
static tCIDLib::TVoid StreamTo( TBinInStream&       strmTestIn
                                , TBinOutStream&    strmTestOut)
{
    //
    //  Ok, So lets create a set of objects that we can stream to this
    //  new stream. All of these classes implement the MStreamable mixin
    //  interface. The << and >> operators are written in terms of this
    //  mixin, so there is just one set of these operators that work for
    //  all streamable classes.
    //
    TPoint      pntTest1(1,1);
    TPoint      pntTest2(100, 100);
    TArea       areaTest1(10, 10, 64, 92);
    T3DVector   vecTest1(1.0, 2.0, 3.0);
    TTime       tmTest1(kCIDLib::enctOneHour);

    //
    //  Ok, stream them puppies into the stream buffer. After each one,
    //  just for fun, print out the size of the memory stream data, which
    //  is just the current stream position.
    //
    strmTestOut << pntTest1 << kCIDLib::FlushIt;
    conOut    << L"Streamed pntTest1. Stream data bytes="
                << strmTestOut.c8CurPos() << kCIDLib::EndLn;

    strmTestOut << pntTest2 << kCIDLib::FlushIt;
    conOut    << L"Streamed pntTest2. Stream data bytes="
                << strmTestOut.c8CurPos() << kCIDLib::EndLn;

    strmTestOut << areaTest1 << kCIDLib::FlushIt;
    conOut    << L"Streamed areaTest1. Stream data bytes="
                << strmTestOut.c8CurPos() << kCIDLib::EndLn;

    strmTestOut << vecTest1 << kCIDLib::FlushIt;
    conOut    << L"Streamed vecTest1. Stream data bytes="
                << strmTestOut.c8CurPos() << kCIDLib::EndLn;

    strmTestOut << tmTest1 << kCIDLib::FlushIt;
    conOut    << L"Streamed tmTest1. Stream data bytes="
                << strmTestOut.c8CurPos() << kCIDLib::EndLn;

    //
    //  Now, lets stream them back in and compare them.
    //
    //  Create a new set of objects with different names so that we
    //  compare them to the originals.
    //
    TPoint      pntRead1;
    TPoint      pntRead2;
    TArea       areaRead1;
    T3DVector   vecRead1;
    TTime       tmRead1;

    // And stream them puppies back in
    strmTestIn  >> pntRead1
                >> pntRead2
                >> areaRead1
                >> vecRead1
                >> tmRead1;

    //
    //  Compare each object with its original. Any differences is an
    //  error in the streaming system (or more likely in the StreamTo()
    //  and StreamFrom() methods of the class that fails.)
    //
    if (pntRead1 != pntTest1)
        conOut << L"First TPoint object failed comparison\n";

    if (pntRead2 != pntTest2)
        conOut << L"Second TPoint object failed comparison\n";

    if (areaRead1 != areaTest1)
        conOut << L"First TArea object failed comparison\n";

    if (vecRead1 != vecTest1)
        conOut << L"First T3DVector object failed comparison\n";

    if (tmRead1 != tmTest1)
        conOut << L"First TTime object failed comparison\n";

    conOut << kCIDLib::FlushIt;
}


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. This guy just
//  creates two different types of binary streams and passes them both to
//  the _StreamTo() testing method above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        //
        //  Create a set of memory buffer based binary streams, one for input
        //  and one for output. We sync them up so that they keep each other
        //  informed of the logical end of stream, which is more than just
        //  using the same memory buffer. We do this by passing the output
        //  stream to the constructor of the input stream.
        //
        TBinMBufOutStream strmBufTestOut(kCIDLib::c4Sz_8K);
        TBinMBufInStream strmBufTestIn(strmBufTestOut);

        //
        //  Call the test function with this stream and let it do the streaming
        //  tests on it.
        //
        conOut << L"Streaming to the memory stream..." << kCIDLib::EndLn;
        StreamTo(strmBufTestIn, strmBufTestOut);
        conOut << kCIDLib::NewEndLn;

        //
        //  Now lets create file based binary streams. Unlike with the memory
        //  buffer stream above, the underlying file system will handle keeping
        //  these streams in sync about where the end of stream is. So there
        //  is no need to sync them.
        //
        TBinFileOutStream strmFileTestOut
        (
            strTestFile
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        TBinFileInStream strmFileTestIn
        (
            strTestFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        //
        //  Now pass this one to the same test function. The test function
        //  does not know or care what kind of stream it is. So this one
        //  should have the same results.
        //
        conOut << L"Streaming to the file stream..." << kCIDLib::EndLn;
        StreamTo(strmFileTestIn, strmFileTestOut);
        conOut << kCIDLib::NewEndLn;
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut  << L"A CIDLib runtime error occured during processing.\n  "
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\nError=  "
                << kerrToCatch.errcId() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}
