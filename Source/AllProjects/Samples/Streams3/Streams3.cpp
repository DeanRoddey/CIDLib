//
// FILE NAME: Streams3.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
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
//  This is the main module for the third of the streaming oriented demo
//  programs. This one demonstrates the magic of polymorphic stream.
//
//  In order to demonstrate this, we need a set of classes that are all
//  derived from a base class. In order to demonstrate the concept fully,
//  both from the streaming side and from the perspective of writing classes
//  that are streamable and support the required RTTI stuff, we create a
//  small family of classes for testing.
//
//  Streams3_Widgets.hpp implements the family of classes that we use for
//  testing. They are very simple and are just done inline. They are not
//  full fledged classes so they don't deal with all the issues of unimplemented
//  constructors and operators and such.
//
//  This module creates a file stream and streams the objects out to the
//  stream. It then streams them back in, demonstrating the ability to figure
//  out from the stream itself what is in the data stream, create the objects
//  to stream them back in using dynamic typing, and then to stream them in.
//
//  This all happens behind the scenes, so its a very convenient and powerful
//  way to make a heterogenous collection of objects persistent. Managing a
//  heterogenous group of objects via a base class happens all the time from
//  a language perspective. Polymorphic streaming extends that level of
//  abstraction to persistent storage.
//
//  We also demonstrate here a 'by reference' collection which is used to
//  manage our heterogenous list of widgets.
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
#include    "Streams3.hpp"


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


// ----------------------------------------------------------------------------
//  Local types
//
//  TBagOWidgets
//      This is a convenience typedef for our collection template.
// ----------------------------------------------------------------------------
typedef TRefBag<TBaseWidget> TBagOWidgets;


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
CIDLib_MainModule(TThread(L"Streams3MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  Since this program has no user interaction, we need to have some widgets
//  to use, as though they were entered by a user in a real drawing program.
//  So this guy just puts some widgets into the passed collection.
//
static tCIDLib::TVoid FillBag(TBagOWidgets& colWidgets)
{
    // A line from 10,10 to 100,100
    colWidgets.Add(new TLineWidget(TPoint(10, 10), TPoint(100, 100)));

    // An unfilled box from 50,50 to 64,92
    colWidgets.Add(new TBoxWidget(TPoint(50,50), TPoint(64,92)));

    // A line from 200,150 to 210,155
    colWidgets.Add(new TLineWidget(TPoint(200, 150), TPoint(210, 155)));

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
        //
        //  Create a file based binary output stream to stream out the test
        //  objects.
        //
        TBinFileOutStream strmTestOut
        (
            strTestFile
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        // Create our bag that we will use to hold the widgets
        TBagOWidgets colWidgets(tCIDLib::EAdoptOpts::Adopt);

        //
        //  Since we have no user interface, just call a function that will
        //  fill in our bag of widgets with some test objects.
        //
        FillBag(colWidgets);

        //
        //  Now we can create a cursor and iterate the bag. We polymorphically
        //  stream each one out. This uses a template method that understands
        //  how to write out the correct type information with the object.
        //
        TBagOWidgets::TCursor cursWidgets(&colWidgets);
        conOut << kCIDLib::DNewLn << L"Writing and 'drawing' objects" << kCIDLib::EndLn;
        for (; cursWidgets; ++cursWidgets)
        {
            ::PolymorphicWrite(&cursWidgets.objRCur(), strmTestOut);

            // Tell the one we just wrote to 'draw' itself
            cursWidgets->Draw(conOut);
            conOut << kCIDLib::NewLn;
        }

        // Flush the console out
        conOut.Flush();

        // Flush out everything left in the output stream cache
        strmTestOut.Flush();

        // Remember how many we wrote
        const tCIDLib::TCard4 c4CountWritten = colWidgets.c4ElemCount();

        // Flush the collection now so that the original objects are destroyed
        colWidgets.RemoveAll();

        //
        //  Create an input stream and stream the objecst back in. We just
        //  stream until we hit the end of the stream.
        //
        TBinFileInStream strmTestIn
        (
            strTestFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        while (!strmTestIn.bEndOfStream())
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
            ::PolymorphicRead(pwidRead, strmTestIn);

            // Put it into the collection
            colWidgets.Add(pwidRead);
        }

        //
        //  Just to show you how PolymorphicRead can do what it does, here
        //  is small example of dynamic typing. Because we used the
        //  AdvRTTIDecls() macros, above, for our widgets classes, they
        //  support dynamic creation. So we can create a new object by name.
        //  It will be default constructed (though it can be arranged to
        //  have the factory creation not be via the default constructor.)
        //
        //  Trace into this code to see how it works.
        //
        TLineWidget* pwidByName = ::pobjMakeNewOfClass<TLineWidget>(L"TLineWidget");

        // Just to prove its there, do an assignment to it
        *pwidByName = TLineWidget(TPoint(1,1), TPoint(2,2));

        // Now delete it
        delete pwidByName;

        //
        //  Anyway, back to our regularly scheduled program. Make sure that
        //  we streamed back in the number of elements we wrote out before,
        //  just as a basic sanity check.
        //
        if (colWidgets.c4ElemCount() != c4CountWritten)
        {
            conOut  << L"Read in element count != to written out count"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
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
            conOut  << L"Widget collection cursor reset returned false!"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        //
        //  We have the same number of elements so iterate them and 'draw'
        //  each one. Note that, here again, the element is the counted
        //  pointer, so we go the next level and get the object out of it
        //  because that's what we want to call the Draw() method on.
        //
        conOut << kCIDLib::DNewLn << L"'Drawing' read in objects" << kCIDLib::EndLn;
        for (; cursWidgets; ++cursWidgets)
        {
            cursWidgets->Draw(conOut);
            conOut << kCIDLib::EndLn;
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut  << L"A CIDLib runtime error occured during processing.\n"
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
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
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}
