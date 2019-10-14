//
// FILE NAME: Streams4.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/1997
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
AdvRTTIDecls(TFilledCircleWidget,TCircleWidget)


// ----------------------------------------------------------------------------
//  Local types
//
//  TBagOWidgets
//      This is a convenience typedef for our collection template.
// ----------------------------------------------------------------------------
typedef TRefBag<TBaseWidget>   TBagOWidgets;


// ----------------------------------------------------------------------------
//  For this simple demo, just start the program on a global function
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Streams4MainThread", eMainThreadFunc))


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
    colWidgets.Add(new TLineWidget(TPoint(10, 10), TPoint(100, 100)));
    colWidgets.Add(new TLineWidget(TPoint(200, 150), TPoint(210, 155)));
    colWidgets.Add(new TCircleWidget(TPoint(50,150), 25));
    colWidgets.Add(new TFilledCircleWidget(TPoint(75,100), 80, TRGBClr(0,0,0)));
    colWidgets.Add(new TLineWidget(TPoint(320, 480), TPoint(20, 15)));
}


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

// The entry point for this program
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // We need a console to 'draw' our widgets to
     TOutConsole  conOut;

    // Create our bag that we will use to hold the widgets
    TBagOWidgets colWidgets(tCIDLib::EAdoptOpts::Adopt);

    //
    //  Create a new binary output file stream, overwriting any previous
    //  contents. Then create a linked binary input file stream over that.
    //  So we can read and write this file. The poly streamer want's to have
    //  both.
    //
    const TString strTestFile(L"TestStreamFile.Dat");
    TBinFileOutStream strmTestOut
    (
        strTestFile
        , tCIDLib::ECreateActs::CreateAlways
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

    //
    //  Since we have no user interface, just call a function that will
    //  fill in our bag of widgets with some test objects.
    //
    FillBag(colWidgets);

    //
    //  Create a cursor for our widget list and stream them out. We pass each
    //  one to the poly streamer object.
    conOut  << kCIDLib::DNewLn << L"Writing and 'drawing' objects"
            << kCIDLib::EndLn;

    TBagOWidgets::TCursor cursWidgets(&colWidgets);
    for (; cursWidgets; ++cursWidgets)
    {
        pstmrTest.StreamOut(*cursWidgets);

        // Tell the one we just wrote to 'draw' itself
        cursWidgets->Draw(conOut);
        conOut << kCIDLib::NewLn;
    }
    conOut << kCIDLib::EndLn;

    // Flush the collection now so the original objects are destroyed
    colWidgets.RemoveAll();

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
        //  into the collection.
        //
        pwidRead = pstmrTest.pobjStreamIn();
        colWidgets.Add(pwidRead);
    }

    // Now we can 'draw' them again and should get the same output
    conOut << kCIDLib::DNewLn << L"'Drawing' read in objects" << kCIDLib::NewLn;
    for (cursWidgets.bReset(); cursWidgets; ++cursWidgets)
    {
        cursWidgets->Draw(conOut);
        conOut << kCIDLib::EndLn;
    }
    conOut << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}

