//
// FILE NAME: CIDLib_StatusControllerTTY.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  A simple derivative of TStatusController is provided here, called
//  TStatusControllerTTY, which will serve many TTY style programs that want to
//  start a thread and just wait until the thread is done, letting it output to
//  the program's output stream while it works.
//
//  The implementation is actually in our parent class' Cpp file.
//
// CAVEATS/GOTCHAS:
//
//  1)  The controlling thread just calls the WaitForComplete() method to
//      wait on the controlled thread.
//
//  2)  The constructor takes an output text file to write to. This lets the
//      controlling thread redirect the working thread's output easily.  It is
//      not adopted, just referenced.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TStatusControllerTTY
//  PREFIX: statc
// ---------------------------------------------------------------------------
class CIDLIBEXP TStatusControllerTTY : public TStatusController
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TStatusControllerTTY() = delete;

        TStatusControllerTTY
        (
                    TTextOutStream* const   pstrmToUse
        );

        TStatusControllerTTY(const TStatusControllerTTY&) = delete;

        ~TStatusControllerTTY();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStatusControllerTTY& operator=(const TStatusControllerTTY&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Starting
        (
            const   TString&                strStartText
        );

        tCIDLib::TVoid WaitForComplete
        (
            const   tCIDLib::TCard4         c4MilliSecs
        );

        tCIDLib::TVoid Working
        (
            const    TString&               strStatusText
            , const     tCIDLib::TCard4     c4PercentComplete
        );

        tCIDLib::TVoid WorkComplete
        (
            const   TString&                strClosingComment
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDone();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_evWaitForIt
        //      This is an event semaphore that is created initially closed.
        //      When The thread calls WorkComplete() it will be posted to
        //      let anyone blocked on WaitForComplete() go again.
        //
        //  m_pstrmTarget
        //      This is a pointer to the output text stream we are to send
        //      our text to. It is just a pointer to a stream that we do not
        //      own.
        // -------------------------------------------------------------------
        TEvent          m_evWaitForIt;
        TTextOutStream* m_pstrmTarget;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStatusControllerTTY,TStatusController)
};

#pragma CIDLIB_POPPACK

