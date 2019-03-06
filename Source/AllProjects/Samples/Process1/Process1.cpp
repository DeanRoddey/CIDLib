//
// FILE NAME: Process1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
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
//  This is the main module for the first process control oriented demo
//  program. This program demonstrates some threading stuff.
//
//  Threads can be set up to run two different ways. One is by just providing
//  it a function to run, which is good for ad hoc threads. Most program's
//  main threads are done this way because they are 'one of' anyway. However
//  you can also create threads that derive from TThread and provide their
//  own internal method to run. This is better for providing canned thread
//  functionality, and it lets each thread have its own per-thread data members
//  inside each derived thread object.
//
//  Another obvious advantage to this scheme is that you can create some kind
//  of base thread class that defines a virtual interface. You can then create
//  a number of derivatives of this thread type, and control them all via the
//  common base class. You provide the thread function, which does its work
//  in terms of the new virtual interface. This is a good way to create a
//  'thread farm' subsystem.
//
//  For demonstration purposes this program uses the self contained thread
//  mechanism. A thread class is derived that provides its own internal thread
//  function and works as a self contained unit. This thread class just plays
//  a little tune. The main thread waits for it to die, then it dies too.
//
//  Extra credit for guessing the tune that the fragment comes from! I'm not
//  sure if the tempo is correct, so adjust the EEighth note value below to
//  adjust it to the right value if you are knowledgeable in this area and
//  it offends you.
//
// CAVEATS/GOTCHAS:
//
//  1)  This one does not bother with any language independence, so what
//      little it outputs is just hard coded.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
//
//  We don't have a main header of our own for this simple program, so just
//  include the CIDLib facility header, which is the only one we need.
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ----------------------------------------------------------------------------
//  Local types
//
//  EDottedNote
//      This enum lets us indicate that a note is dotted, so we don't have to
//      define variations in ENoteTypes for dotted types.
//
//  ENotes
//      These represent the frequecies of the notes we need to play. Only the
//      necessary ones are defined. Their values are their hertz frequency.
//
//  ENoteTypes
//      The types of notes our 'music' supports. They are just equated to
//      their time in milliseconds.
//
//  TNote
//      This represents a note. Its a note type (and whether its dotted or
//      not) and its frequency.
// ----------------------------------------------------------------------------
enum EDottedNote
{
    ENormal
    , EDotted
};

enum  ENotes
{
    ENote_C         = 264
    , ENote_D       = 296
    , ENote_Eb      = 312
    , ENote_E       = 328
    , ENote_F       = 352
    , ENote_G       = 392
    , ENote_A       = 440
    , ENote_Bb      = 464
    , ENote_2C      = 528
    , ENote_2D      = 592
    , ENote_2Eb     = 624
    , ENote_2E      = 656
    , ENote_2F      = 704
    , ENote_2G      = 784
};

enum ENoteTypes
{
    EEighth         = 110
    , EQuarter      = EEighth * 2
    , EHalf         = EQuarter * 2
    , EWhole        = EHalf * 2
};

struct TNote
{
    tCIDLib::TCard4     c4Freq;
    ENoteTypes          eType;
    EDottedNote         eDotted;
};



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Local static data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class (derived from
//      TTextOutStream.)
//
//  songData
//  c4NoteCount
//      This the data for the song we play, and the number of notes in the
//      song.
// ----------------------------------------------------------------------------
static TOutConsole      conOut;
static TNote            aSongData[] =
                        {
                                { ENote_D   , EQuarter  , ENormal }
                            ,   { ENote_D   , EQuarter  , EDotted }
                            ,   { ENote_F   , EEighth   , ENormal }
                            ,   { ENote_F   , EQuarter  , ENormal }
                            ,   { ENote_F   , EQuarter  , ENormal }
                            ,   { ENote_F   , EQuarter  , EDotted }
                            ,   { ENote_Bb  , EEighth   , ENormal }
                            ,   { ENote_Bb  , EQuarter  , ENormal }
                            ,   { ENote_Bb  , EEighth   , ENormal }
                            ,   { ENote_2C  , EEighth   , ENormal }
                            ,   { ENote_2D  , EQuarter  , ENormal }
                            ,   { ENote_2C  , EQuarter  , ENormal }
                            ,   { ENote_Bb  , EQuarter  , ENormal }
                            ,   { ENote_A   , EQuarter  , ENormal }
                            ,   { ENote_A   , EQuarter  , EDotted }
                            ,   { ENote_G   , EEighth   , ENormal }
                            ,   { ENote_G   , EQuarter  , ENormal }
                            ,   { ENote_G   , EQuarter  , ENormal }
                            ,   { ENote_G   , EQuarter  , EDotted }
                            ,   { ENote_Bb  , EEighth   , ENormal }
                            ,   { ENote_Bb  , EQuarter  , ENormal }
                            ,   { ENote_Bb  , EQuarter  , ENormal }
                            ,   { ENote_Bb  , EQuarter  , EDotted }
                            ,   { ENote_2Eb , EEighth   , ENormal }
                            ,   { ENote_2Eb , EQuarter  , ENormal }
                            ,   { ENote_2Eb , EEighth   , ENormal }
                            ,   { ENote_2F  , EEighth   , ENormal }
                            ,   { ENote_2G  , EQuarter  , ENormal }
                            ,   { ENote_2F  , EQuarter  , ENormal }
                            ,   { ENote_2Eb , EQuarter  , ENormal }
                            ,   { ENote_2D  , EQuarter  , ENormal }
                            ,   { ENote_2C  , EQuarter  , EDotted }
                            ,   { ENote_Bb  , EEighth   , EDotted }
                            ,   { ENote_Bb  , EQuarter  , EDotted }
                        };
static tCIDLib::TCard4  c4NoteCount = tCIDLib::c4ArrayElems(aSongData);


// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This must be done in one (and only one) module of the client program.
//  It tells CIDLib which is the main thread of the program.) GUI apps use
//  a similar, but differently named, mechanism.
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"Process1MainThread", eMainThreadFunc))




// ----------------------------------------------------------------------------
//   CLASS: TSongThread
//  PREFIX: thr
//
//  A very simple derivaitve of TThread that plays a song. Since its just
//  a local type, we don't bother with unimplemented constructors and
//  operators.
//
//  Since there is only ever one copy of this thread, it names itself in its
//  call to the TThread constructor. If multiples could exist, then either
//  the user code should provide names, or unique names should be gen'ed up
//  for each one (The TUniqueName class is a nice way to do this.)
// ----------------------------------------------------------------------------
class TSongThread : public TThread
{
    public :
        // --------------------------------------------------------------------
        // Constructors and Destructors
        // --------------------------------------------------------------------
        TSongThread();
        ~TSongThread();


    protected :
        // --------------------------------------------------------------------
        //  Protected, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::EExitCodes eProcess();


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TSongThread,TThread)
};

RTTIDecls(TSongThread,TThread)


// ----------------------------------------------------------------------------
//  TSongThread: Public constructors and destructors
// ----------------------------------------------------------------------------
TSongThread::TSongThread() :

    TThread(L"SongThread")
{
}

TSongThread::~TSongThread()
{
}

// ----------------------------------------------------------------------------
//  TSongThread: Protected, inherited methods
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes TSongThread::eProcess()
{
    // We have to let our calling thread go first
    Sync();

    //
    //  Play the song, which is just a static array of note structures
    //  that we whipped up. We use the standard beeper via facCIDLib which
    //  will play through whatever audio device is installed as the CIDLib
    //  audio device.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4NoteCount; c4Index++)
    {
        // Calculate the duration, extended it by half if dotted
        tCIDLib::TCard4 c4Duration = aSongData[c4Index].eType;
        if (aSongData[c4Index].eDotted == EDotted)
            c4Duration = tCIDLib::TCard4(1.5 * c4Duration);

        // Play the frequency for the calculated duration
        TAudio::Beep(aSongData[c4Index].c4Freq, c4Duration);

        // Provide a tiny break between notes
        TThread::Sleep(10);
    }
    return tCIDLib::EExitCodes::Normal;
}


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It just starts the
//  song playing worker thread and waits for it to die.
//
//  RETURN: EExit_Normal if things go ok. If the worker thread returns a
//              non-normal return, we return that.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Start up the worker thread and just wait for it to die. Indicate
    //  what we are doing to the user.
    //
    conOut << kCIDLib::NewLn << L"Starting worker thread..." << kCIDLib::EndLn;
    TSongThread thrdWorker;
    thrdWorker.Start();

    conOut << L"Waiting for thread to die..." << kCIDLib::EndLn;
    tCIDLib::EExitCodes eExit = thrdWorker.eWaitForDeath();

    if (eExit != tCIDLib::EExitCodes::Normal)
    {
        conOut  << L"Worker thread returned non-normal exit code: "
                << eExit << kCIDLib::DNewLn;

        // Make this our return code
        return eExit;
    }

    conOut << L"Thread died, exiting." << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}

