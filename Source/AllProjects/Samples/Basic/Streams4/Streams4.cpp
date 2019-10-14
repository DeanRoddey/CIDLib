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
//  When using polymorphic streaming, each object streamed out must be
//  streamed along with its class information. This allows the code that
//  streams that objects back in to know what class each stored object is.
//  It uses this information to create a new object of that type and stream
//  the stored information into the new object.
//
//  This extra class information can be a large burden if there are a lot of
//  objects being streamed (particularly if the object's themselves are
//  pretty small.) The TPolyStreamer template class provides smarts for
//  streaming that can drastically reduce the overhead of streaming out
//  class information.
//
//  The polymorphic streamer will maintain a dictionary of types that
//  have been streamed. So it knows whether you've streamed an object out
//  of a particular type. So, instead of writing out full type info, it just
//  writes out an id value. When reading back in, it reads in the dictionary
//  and uses that to know what type the next object is by reading in its type
//  id and mapping it back to a type.
//
// CAVEATS/GOTCHAS:
//
//  1)  This program is very simple so it does not attempt to be language
//      independent and it does not provide its own facility class or object.
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



// ----------------------------------------------------------------------------
//  For this simple demo, just start the program on a global function
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Streams4MainThread", eMainThreadFunc))


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

    //
    //  Create our bag that we will use to hold the widgets and load up some test
    //  widgets into it. Tell it to adopt the widgets we put into it.
    //
    using TBagOWidgets = TRefBag<TBaseWidget>;
    TBagOWidgets colWidgets(tCIDLib::EAdoptOpts::Adopt);
    colWidgets.Add(new TLineWidget(TPoint(10, 10), TPoint(100, 100)));
    colWidgets.Add(new TLineWidget(TPoint(200, 150), TPoint(210, 155)));
    colWidgets.Add(new TCircleWidget(TPoint(50,150), 25));
    colWidgets.Add(new TFilledCircleWidget(TPoint(75,100), 80, TRGBClr(0,0,0)));
    colWidgets.Add(new TLineWidget(TPoint(320, 480), TPoint(20, 15)));

    //
    //  Create a new binary output file stream, overwriting any previous
    //  contents. Then create a linked binary input file stream over that.
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

    // Loop through them streaming and 'drawing' each one
    conOut  << kCIDLib::DNewLn
            << L"Writing and 'drawing' objects" << kCIDLib::EndLn;

    TBagOWidgets::TCursor cursWidgets(&colWidgets);
    for (; cursWidgets; ++cursWidgets)
    {
        pstmrTest.StreamOut(*cursWidgets);
        cursWidgets->Draw(conOut);
        conOut << kCIDLib::NewLn;
    }
    conOut << kCIDLib::EndLn;

    // Flush the collection now so the original objects are destroyed
    colWidgets.RemoveAll();

    //
    //  Now we can stream in the elements that we wrote to the file.
    //  The poly streamer figures out what the objects are and creates
    //  an object of that type for us to stream into. We then just add
    //  them into the bag.
    //
    while (!pstmrTest.bEndOfStream())
    {
        colWidgets.Add(pstmrTest.pobjStreamIn());
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

