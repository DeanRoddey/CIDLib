//
// FILE NAME: Collect3_Thread.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/1997
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
//  This module implements the simple thread class derivative that the program
//  uses for its test. Each thread is given a pointer to a sorted bag of
//  TCardinal values, which is stores away for use. There are two sorts of
//  threads, adders and removers, which is controlled by a parameter in their
//  constructor.
//
//  Each thread started sleeps for some small random time. Then it wakes up
//  and creates another randum number. It checks the the bag to see if that
//  random value is in the bag. If not, then it adds the value if its an adder.
//  If it is, and its a remover, then it removes that element.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes.
// ---------------------------------------------------------------------------
#include    "Collect3.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TColThread,TThread)


// ---------------------------------------------------------------------------
//   CLASS: TColThread
//  PREFIX: thr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TColThread: Constructors and destructors
// ---------------------------------------------------------------------------

//
//
//  The only available constructor takes the info that the thread needs. It
//  will be started later by the caller. We use a helper in the CIDLib facility
//  to generate unique names for each thread
//
TColThread::TColThread(         TTestCol* const             pcolToUse
                        , const TColThread::EThreadTypes    eType
                        ,       TTextOutStream* const       pstrmOut) :

    TThread(facCIDLib().strNextThreadName(L"Col3Thread"))
    , m_eType(eType)
    , m_pcolToUse(pcolToUse)
    , m_pstrmOut(pstrmOut)
{
}

TColThread::~TColThread()
{
}


// ---------------------------------------------------------------------------
//  TColThread: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  This is the thread function for the test thread. It works on the collection
//  stored during construction.
//
tCIDLib::EExitCodes TColThread::eProcess()
{
    // Let the starting thread go
    Sync();

    // Set up a random number generator for this thread
    TRandomNum randGen;
    randGen.Seed(TTime::c4Millis() + tidThis());

    //
    //  Loop until we are asked to shutdown. The main thread will let us
    //  play for a while then ask us to shutdown.
    //
    while (1)
    {
        // If we get an exit signal, then exit
        if (bCheckShutdownRequest())
            break;

        // Generate a semi random number between 500 and 2000
        tCIDLib::TCard4 c4Value = 0;
        while (c4Value < 500)
            c4Value = randGen.c4GetNextNum() % 2000;

        // Lets sleep for this long
        TThread::Sleep(c4Value);

        //
        //  Now we need to create a new random number and see if this value is
        //  in the bag. If not, we need to add it. For this, we need to lock
        //  the collection because this requires multiple operations.
        //
        {
            TMtxLocker lockCol(m_pcolToUse->pmtxLock());

            // Create a new value to add, between 0 and 500
            c4Value = randGen.c4GetNextNum() % 500;

            //
            //  If no elements, then no need to even search. Just put the
            //  new element in if an adder. Do nothing if a remover.
            //
            //  Otherwise we have to see the element is already in the
            //  collection.
            //
            if (!m_pcolToUse->c4ElemCount())
            {
                if (m_eType == EThreadTypes::Adder)
                {
                    m_pcolToUse->objAdd(TCardinal(c4Value));

                    // Say what we did
                    *m_pstrmOut << L"Thread: " << strName() << kCIDLib::NewLn
                                << L"        Added value: "
                                << c4Value << kCIDLib::EndLn;
                }
            }
             else
            {
                TTestCol::TNCCursor cursNums(m_pcolToUse);
                for (; cursNums; ++cursNums)
                {
                    if (*cursNums == c4Value)
                    {
                        // If we are a remover, then remove it
                        if (m_eType == EThreadTypes::Remover)
                        {
                            //
                            //  Remove this one. All cursors other than ours will
                            //  be invalidated, but we don't use this one again
                            //  once this happens anyway.
                            //
                            m_pcolToUse->RemoveAt(cursNums);

                            // Indicate what we did
                            *m_pstrmOut << L"Thread: " << strName() << kCIDLib::EndLn
                                        << L"        Removed value: "
                                        << c4Value << kCIDLib::EndLn;
                        }
                        break;
                    }
                }

                // If the cursor is invalid, then we didn't find it
                if (!cursNums && (m_eType == EThreadTypes::Adder))
                {
                    m_pcolToUse->objAdd(TCardinal(c4Value));

                    // Say what we did
                    *m_pstrmOut << L"Thread: " << strName() << kCIDLib::NewLn
                                << L"        Added value: " << c4Value
                                << kCIDLib::EndLn;
                }
            }
        }
    }
    return tCIDLib::EExitCodes::Normal;
}
