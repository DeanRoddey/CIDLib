//
// FILE NAME: CIDKernel_Event.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This is the header for the CIDKernel_Event.cpp file. This file implements
//  the TKrnlEvent class, which is the classic open/closed type of sync
//  mechanism.
//
// CAVEATS/GOTCHAS:
//
//  1)  Duplicate() will duplicate an event. This is the only way to open
//      multiple copies of an unnamed event. For named events, you can either
//      create another event with the same name, or use the duplicate method.
//
//  2)  On Win32 systems only manual mode semaphores are supported in order
//      to maintain portability, since the auto mode is not really supported
//      on other platforms.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlEvent
//  PREFIX: kev
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlEvent
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bWaitMultiple
        (
                    TKrnlEvent&             kevOne
            ,       TKrnlEvent&             kevTwo
            ,       tCIDLib::TCard4&        c4Which
            , const tCIDLib::TCard4         c4Wait
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlEvent();

        TKrnlEvent
        (
            const   tCIDLib::TCh* const     pszName
        );

        TKrnlEvent(const TKrnlEvent&) = delete;

        TKrnlEvent
        (
                    TKrnlEvent&&            kevSrc
        );

        ~TKrnlEvent();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlEvent& operator=(const TKrnlEvent&) = delete;

        TKrnlEvent& operator=
        (
                    TKrnlEvent&&            kevSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bCreate
        (
            const   tCIDLib::EEventStates   eInitState
            , const tCIDLib::TBoolean       bManual
        );

        tCIDLib::TBoolean bDuplicate
        (
            const   TKrnlEvent&             kevToDup
        );

        [[nodiscard]] tCIDLib::TBoolean bIsValid() const
        {
            return m_hevThis.bIsValid();
        }

        tCIDLib::TBoolean bNamed() const
        {
            return (m_pszName != nullptr);
        }

        tCIDLib::TBoolean bOpen();

        tCIDLib::TBoolean bOpenOrCreate
        (
            const   tCIDLib::EEventStates   eInitState
            , const tCIDLib::TBoolean       bManual
            ,       tCIDLib::TBoolean&      bCreated
        );

        tCIDLib::TBoolean bPulse();

        tCIDLib::TBoolean bReset();

        tCIDLib::TBoolean bSetName
        (
            const   tCIDLib::TCh* const     pszNewName
        );

        tCIDLib::TBoolean bTrigger();

        tCIDLib::TBoolean bWaitFor
        (
            const   tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        );

        const TEventHandle& hevThis() const
        {
            return m_hevThis;
        }

        const tCIDLib::TCh* pszName() const
        {
            return m_pszName;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreateNamed
        (
            const   tCIDLib::EEventStates   eInitState
            , const tCIDLib::TBoolean       bFailIfExists
            , const tCIDLib::TBoolean       bManual
            ,       tCIDLib::TBoolean&      bCreated
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hevThis
        //      This is the handle to the event.
        //
        //  m_pszName
        //      The name of the semaphore.
        // -------------------------------------------------------------------
        TEventHandle        m_hevThis;
        tCIDLib::TCh*       m_pszName;
};

#pragma CIDLIB_POPPACK

