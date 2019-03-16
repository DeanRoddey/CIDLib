//
// FILE NAME: Streams4.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//  This is the main module for the fourth of the streaming oriented demo
//  programs. This one is basically just like the previous version, except
//  that this one uses smart polymorphic streaming.
//
//  When using polymorphic streaming, each object streamed out must also be
//  streamed along with its class information. This allows the code that
//  streams that objects back in to know what class each stored object is.
//  It uses this information to create a new object of that type and stream
//  the stored information inot the new object.
//
//  This extra class information can be a large burden if there are a lot of
//  objects being streamed (particularly if the object's themselves are
//  pretty small.) The TPolyStreamer template class provides smarts for
//  streaming that can drastically reduce the overhead of streaming out
//  class information.
//
//  The smart polymorphic streamer will maintain a dictionary of types that
//  have been streamed. So it knows whether you've streamed an object out
//  of a particular type. So, instead of writing out full type info, it just
//  writes out an id value. When reading back in, it reads in the dictionary
//  and uses that to know what type the next object is by reading in its type
//  id and mapping it back to a type.
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
//  Include our main header, which includes anything else we need
// ----------------------------------------------------------------------------
#include    "Streams4.hpp"


// ----------------------------------------------------------------------------
//  Since all of our test widget classes are inline, they don't have their
//  own.cpp file. So we do their RTTI macros for them here. Note that we
//  use AdvRTTIDecls, which is the version that enables dynamic type creation.
//  TBaseWidget does not enable this because it is an abstract base class and
//  cannot be directly created.
// ----------------------------------------------------------------------------
RTTIDecls(TBaseWidget,TObject)
AdvRTTIDecls(TLineWidget,TBaseWidget)
AdvRTTIDecls(TCircleWidget,TBaseWidget)
AdvRTTIDecls(TBoxWidget,TBaseWidget)
AdvRTTIDecls(TFilledBoxWidget,TBoxWidget)
AdvRTTIDecls(TTextWidget,TBaseWidget)


// ----------------------------------------------------------------------------
//  Local types
//
//  TBagOWidgets
//      This is a convenience typedef for our collection template.
// ----------------------------------------------------------------------------
typedef TRefBag<TBaseWidget>   TBagOWidgets;


// ----------------------------------------------------------------------------
//  Local static data
//
//  rgbBlack
//  rgbWhite
//      Some convenience colors to use in our test.
//
//  strTestFile
//      This is the name of the file that we use as the backing for the file
//      stream that we test.
// ----------------------------------------------------------------------------
static const TRGBClr    rgbBlack(0,0,0);
static const TRGBClr    rgbWhite(0xFF,0xFF,0xFF);
static const TString    strTestFile(L"TestStreamFile.Dat");


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Local data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
// ----------------------------------------------------------------------------
static TOutConsole  conOut;


// ----------------------------------------------------------------------------
//  Do the magic main module code
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"Streams4MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  Since this program has no user interaction, we need to have some widgets
//  to use, as though they were entered by a user in a real drawing program.
//  So this guy just puts some widgets into the passed collection.
//  RETURN: None
//
static tCIDLib::TVoid FillBag(TBagOWidgets& colWidgets)
{
    // A line from 10,10 to 100,100
    colWidgets.Add(new TLineWidget(TPoint(10, 10), TPoint(100, 100)));

    // An unfilled box from 50,50 to 64,92
    colWidgets.Add(new TBoxWidget(TPoint(50,50), TPoint(64,92)));

    // A line from 200,150 to 210,155
    colWidgets.Add(new TLineWidget(TPoint(200, 150), TPoint(210, 155)));

    // A text widet at 20, 40
    colWidgets.Add(new TTextWidget(TPoint(20, 40), TString(L"The text")));

    // A filled box from 500,50 to 525,100, white filled
    colWidgets.Add(new TFilledBoxWidget(TPoint(500,500), TPoint(525,100), rgbWhite));

    // A filled box from 50,100 to 55,110, black filled
    colWidgets.Add(new TFilledBoxWidget(TPoint(50,100), TPoint(55,110), rgbBlack));

    // A line from 320,480 to 20, 15
    colWidgets.Add(new TLineWidget(TPoint(320, 480), TPoint(20, 15)));
}


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It creates the file
//  stream, streams out the objects 'drawing' them as it goes, then streams
//  them back in and 'draws' them again. This should cause the exact same
//  output as the originals.
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
        // Create our bag that we will use to hold the widgets
        TBagOWidgets colWidgets(tCIDLib::EAdoptOpts::Adopt);

        //
        //  Open or create a file based binary stream to use as the test
        //  stream. Check its size afterwards. If its zero sized, then
        //  we just created it and need to fill it in.
        //
        TBinFileOutStream strmTestOut
        (
            strTestFile
            , tCIDLib::ECreateActs::OpenOrCreate
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        TBinFileInStream strmTestIn
        (
            strTestFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        // Create a poly streamer to stream out with
        TPolyStreamer<TBaseWidget> pstmrTest(&strmTestIn, &strmTestOut);

        // Create a cursor for our widget list
        TBagOWidgets::TCursor cursWidgets(&colWidgets);
        if (strmTestIn.c8CurSize() == 0)
        {
            //
            //  Since we have no user interface, just call a function that will
            //  fill in our bag of widgets with some test objects.
            //
            FillBag(colWidgets);

            // Reset the cursor to get it back into sync
            cursWidgets.bReset();

            //
            //  Now we can iterate the bag. We polymorphically stream each
            //  widget element out. This uses a template method that understands
            //  now to write out the correct type information with the object.
            //
            conOut  << kCIDLib::DNewLn << L"Writing and 'drawing' objects"
                    << kCIDLib::EndLn;
            for (; cursWidgets; ++cursWidgets)
            {
                pstmrTest.StreamOut(*cursWidgets);

                // Tell the one we just wrote to 'draw' itself
                cursWidgets->Draw(conOut);
                conOut << kCIDLib::NewLn;
            }

            // Flush the console output
            conOut << kCIDLib::FlushIt;

            // Flush the collection now so the original objects are destroyed
            colWidgets.RemoveAll();
        }

        //
        //  Now we can stream in the elements that we written to the file.
        //  Note that we don't need to know what order they were streamed
        //  out because our polymorphic streaming system figures this out
        //  for us.
        //
        while (!pstmrTest.bEndOfStream())
        {
            // A base widget pointer to read objects back into
            TBaseWidget* pwidRead;

            //
            //  Now we use the polymorphic reading template function that
            //  understands now to find out what the next object is, create
            //  an object of that type using dynamic type creation, then
            //  to stream into that object.
            //
            //  We get back a dynamically allocated object that can be put
            //  into a counted pointer object and into the collection.
            //
            pwidRead = pstmrTest.pobjStreamIn();

            // Put it into the collection
            colWidgets.Add(pwidRead);
        }

        //
        //  Now reset the cursor so we can iterate the collection again
        //  and 'draw' the objects once more. The output should be exactly
        //  the same as before.
        //
        //  Here again, sanity check and make sure that the reset call returns
        //  true, which indicates that there are elements to be cursored.
        //
        if (!cursWidgets.bReset())
        {
            conOut  << L"Widget collection cursor reset returned false!\n"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        //
        //  We have the same number of elements so iterate them and 'draw'
        //  each one. Note that, here again, the element is the counted
        //  pointer, so we go the next level and get the object out of it
        //  because that's what we want to call the Draw() method on.
        //
        conOut << kCIDLib::DNewLn << L"'Drawing' read in objects" << kCIDLib::NewLn;
        for (; cursWidgets; ++cursWidgets)
        {
            cursWidgets->Draw(conOut);
            conOut << kCIDLib::EndLn;
        }
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut  << L"A CIDLib runtime error occured during processing.\n"
                << L"Error: " << errToCatch.strErrText();
        if (!errToCatch.strAuxText().bIsEmpty())
            conOut << L", " << errToCatch.strAuxText();
        conOut << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing\n"
                << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

