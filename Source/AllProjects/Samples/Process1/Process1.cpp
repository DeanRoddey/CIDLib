//
// FILE NAME: Process1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
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
//  For demonstration purposes this program uses the self contained thread
//  mechanism. This thread class just plays a little tune. The main thread waits
//  for it to die, then it dies too.
//
//  Extra credit for guessing the tune that the fragment comes from! I'm not
//  sure if the tempo is correct, so adjust the EEighth note value below to
//  adjust it to the right value if you are knowledgeable in this area and
//  it offends you.
//
//
//  As with most of these basic samples, this guy does not create a facility
//  object, it just starts up a thread on a local function.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
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
enum class EDotted
{
    Normal
    , Dotted
};

enum class ENotes
{
    C           = 264
    , D         = 296
    , Eb        = 312
    , E         = 328
    , F         = 352
    , G         = 392
    , A         = 440
    , Bb        = 464
    , C2        = 528
    , D2        = 592
    , Eb2       = 624
    , E2        = 656
    , F2        = 704
    , G2        = 784
};

enum class ETypes
{
    Eighth      = 125
    , Quarter   = Eighth * 2
    , Half      = Quarter * 2
    , Whole     = Half * 2
};

struct TNote
{
    ENotes      eFreq;
    ETypes      eType;
    EDotted     eDotted;
};



// ----------------------------------------------------------------------------
//  Local static data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output.
//
//  aSongData
//  c4NoteCount
//      This the data for the song we play, and the number of notes in the
//      song.
// ----------------------------------------------------------------------------
static TOutConsole  conOut;
static TNote        aSongData[] =
{
        { ENotes::D   , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::D   , ETypes::Quarter  , EDotted::Dotted }
    ,   { ENotes::F   , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::F   , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::F   , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::F   , ETypes::Quarter  , EDotted::Dotted }
    ,   { ENotes::Bb  , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::Bb  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::Bb  , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::C2  , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::D2  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::C2  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::Bb  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::A   , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::A   , ETypes::Quarter  , EDotted::Dotted }
    ,   { ENotes::G   , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::G   , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::G   , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::G   , ETypes::Quarter  , EDotted::Dotted }
    ,   { ENotes::Bb  , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::Bb  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::Bb  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::Bb  , ETypes::Quarter  , EDotted::Dotted }
    ,   { ENotes::Eb2 , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::Eb2 , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::Eb2 , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::F2  , ETypes::Eighth   , EDotted::Normal }
    ,   { ENotes::G2  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::F2  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::Eb2 , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::D2  , ETypes::Quarter  , EDotted::Normal }
    ,   { ENotes::C2  , ETypes::Quarter  , EDotted::Dotted }
    ,   { ENotes::Bb  , ETypes::Eighth   , EDotted::Dotted }
    ,   { ENotes::Bb  , ETypes::Quarter  , EDotted::Dotted }
};
static tCIDLib::TCard4  c4NoteCount = tCIDLib::c4ArrayElems(aSongData);


// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
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
        TSongThread() :

            TThread(L"SongThread")
        {
        }

        TSongThread(const TSongThread&) = delete;

        ~TSongThread()
        {
        }

    protected :
        // --------------------------------------------------------------------
        //  Protected, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::EExitCodes eProcess() override
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
                tCIDLib::TCard4 c4Duration = tCIDLib::c4EnumOrd(aSongData[c4Index].eType);
                if (aSongData[c4Index].eDotted == EDotted::Dotted)
                    c4Duration = tCIDLib::TCard4(1.5 * c4Duration);

                // Play the frequency for the calculated duration
                TAudio::Beep
                (
                    tCIDLib::c4EnumOrd(aSongData[c4Index].eFreq), c4Duration
                );
            }
            return tCIDLib::EExitCodes::Normal;
        }


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TSongThread,TThread)
};
RTTIDecls(TSongThread,TThread)



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

