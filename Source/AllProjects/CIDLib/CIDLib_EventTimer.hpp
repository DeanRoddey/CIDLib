//
// FILE NAME: CIDLib_EventTimer.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/15/2002
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
//  This header provides a very simple, all inline class that implements a
//  timer class. Just create one of them, and you can then call start/stop
//  methods on it. On the stop, it will calc the time since the start call
//  and add it to an accumulator, and bump the number of samples it's taken.
//
//  You can get an average, which just divides the accumulator by the number
//  of samples. And you can reset it, to start the accumulator and counter
//  over. And you can indicate a 'skip' count which will make it ignore that
//  many samples before it starts accumulating.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TEventTimer
//  PREFIX: et
// ---------------------------------------------------------------------------
class TEventTimer
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEventTimer(const tCIDLib::TCard4 c4Skip = 0) :

            m_c4Count(0)
            , m_c4Skip(c4Skip)
            , m_enctAverage(0)
            , m_enctLongest(0)
            , m_enctShortest(kCIDLib::c4MaxCard)
            , m_enctStart(0)
        {
        }

        TEventTimer(const TEventTimer&) = delete;
        TEventTimer(TEventTimer&&) = delete;

        ~TEventTimer() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEventTimer& operator=(const TEventTimer&) = delete;
        TEventTimer& operator=(TEventTimer&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime enctAverage() const
        {
            if (!m_c4Count)
                return 0;
            return m_enctAverage / m_c4Count;
        }

        tCIDLib::TVoid Reset(const tCIDLib::TCard4 c4Skip = 0)
        {
            m_enctAverage = 0;
            m_c4Count = 0;
            m_c4Skip = c4Skip;
        }

        tCIDLib::TVoid Start()
        {
            m_enctStart = TTime::enctNow();
        }

        tCIDLib::TVoid Stop()
        {
            if (m_c4Skip)
            {
                m_c4Skip--;
                return;
            }

            const tCIDLib::TEncodedTime enctElapsed = TTime::enctNow() - m_enctStart;
            if (enctElapsed && (enctElapsed < m_enctShortest))
                m_enctShortest = enctElapsed;
            if (enctElapsed > m_enctLongest)
                m_enctLongest = enctElapsed;

            m_c4Count++;
            m_enctAverage += enctElapsed;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Count;
        tCIDLib::TCard4         m_c4Skip;
        tCIDLib::TEncodedTime   m_enctAverage;
        tCIDLib::TEncodedTime   m_enctLongest;
        tCIDLib::TEncodedTime   m_enctShortest;
        tCIDLib::TEncodedTime   m_enctStart;
};

#pragma CIDLIB_POPPACK


