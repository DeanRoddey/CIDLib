//
// FILE NAME: CIDKernel_Thread.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
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
//  This is the header for the CIDKernel_Thread.Cpp file, which implements
//  the TKrnlThread class. This is the kernel wrapper to provide access to
//  host threading services.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  A thread cannot call its own eWaitForDeath() method. That's only
//      for others to call. Doing so will cause an exception.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references.
// ---------------------------------------------------------------------------
class   TThread;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlThread
//  PREFIX: kthr
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlThread
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types.
        //
        //  TCallBack is the form of a callback method we pass into the system level
        //  thread startup. When we come out on the other side in the context of
        //  the new thread, which is on an internal, platform specific handler, this
        //  let's us call back to the portable handler that the CIDLib layer wants
        //  to use for this thread.
        // -------------------------------------------------------------------
        using TCallBack = tCIDLib::EExitCodes (*)
        (
                TThread* const          pthrStarting
            ,   tCIDLib::TVoid* const   pUserData
        );


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bIsCaller
        (
           const    TKrnlThread&            kthrToTest
        );

        static tCIDLib::TBoolean bIsCallerGUIThread();

        static tCIDLib::TBoolean bPriorityOf
        (
            const   TKrnlThread&            kthrToQuery
            ,       tCIDLib::EPrioLevels&   eToFill
        );

        static tCIDLib::TVoid Exit
        (
            const   tCIDLib::EExitCodes     eExitCode
        );

        static tCIDLib::TVoid KrnlThreadInit();

        static TKrnlThread* pkthrCaller();

        static tCIDLib::TVoid Sleep
        (
            const   tCIDLib::TCard4         c4WaitMillis
        );

        static tCIDLib::TThreadId tidCaller();

        static tCIDLib::TThreadId tidPrimary();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlThread();

        TKrnlThread(const TKrnlThread&) = delete;

        TKrnlThread
        (
                    TKrnlThread&&           kthrSrc
        );

        ~TKrnlThread();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlThread& operator=(const TKrnlThread&) = delete;

        TKrnlThread& operator=
        (
                    TKrnlThread&&           kthrSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAdoptCaller();

        tCIDLib::TBoolean bBeginThread
        (
            const   TKrnlThread::TCallBack  pfnFunc
            , const tCIDLib::TCard4         c4StackSize
            ,       tCIDLib::TVoid* const   pData
            , const tCIDLib::EThreadFlags   eFlags
            ,       TThread* const          pthrStart
            , const tCIDLib::TCh* const     pszThreadName
        );

        tCIDLib::TBoolean bBlockMyself();

        tCIDLib::TBoolean bIsGUIThread() const;

        tCIDLib::TBoolean bIsRunning
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bPriority
        (
                    tCIDLib::EPrioLevels&   eToFill
        )   const;

        tCIDLib::TBoolean bQueryExitCode
        (
                    tCIDLib::EExitCodes&    eToFill
        )   const;

        tCIDLib::TBoolean bUnblock();

        tCIDLib::TBoolean bWaitEvOrDeath
        (
                    TKrnlEvent&             kevWait
            ,       tCIDLib::TBoolean&      bDied
            , const tCIDLib::TCard4         c4MilliSeconds = kCIDLib::c4MaxWait
        );

        tCIDLib::TBoolean bWaitForDeath
        (
                    tCIDLib::TBoolean&      bState
            ,       tCIDLib::EExitCodes&    eToFill
            , const tCIDLib::TCard4         c4MilliSeconds = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TBoolean bSetPriority
        (
            const   tCIDLib::EPrioLevels    eLevel
        );

        const TThreadHandle& hthrThis() const
        {
            return m_hthrThis;
        }

        TThreadHandle& hthrThis()
        {
            return m_hthrThis;
        }

        tCIDLib::TVoid MarkAsGUIThread();

        tCIDLib::TVoid Orphan();

        tCIDLib::TVoid SetNotRunning();

        tCIDLib::TThreadId tidThis() const;


    protected  :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TThread;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hthrThis
        //      This is the handle of the thread, which internally means different
        //      things for each platform driver.
        // -------------------------------------------------------------------
        TThreadHandle       m_hthrThis;
};

#pragma CIDLIB_POPPACK

