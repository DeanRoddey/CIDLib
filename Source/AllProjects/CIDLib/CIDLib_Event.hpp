//
// FILE NAME: CIDLib_Event.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_Event.Cpp file, which implements the
//  TEvent class. This class implements a traditional event semaphore, which
//  is a kind of gate which can stop threads or let them through.
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
//  CLASS: TEvent
// PREFIX: ev
// ---------------------------------------------------------------------------
class CIDLIBEXP TEvent : public TObject, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard4 c4WaitMultiple
        (
                    TEvent&                 evOne
            ,       TEvent&                 evTwo
            , const tCIDLib::TCard4         c4WaitFor = kCIDLib::c4MaxWait
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TEvent
        (
            const   tCIDLib::EEventStates   eInitState = tCIDLib::EEventStates::Triggered
            , const tCIDLib::TBoolean       bManual    = kCIDLib::True
        );

        TEvent
        (
            const   TResourceName&          rsnToUse
            , const tCIDLib::EEventStates   eInitState = tCIDLib::EEventStates::Reset
            , const tCIDLib::ECreateActs    eCreateAct = tCIDLib::ECreateActs::OpenOrCreate
            , const tCIDLib::TBoolean       bManual    = kCIDLib::True
        );

        TEvent
        (
            const   TResourceName&          rsnToUse
            ,       tCIDLib::TBoolean&      bCreated
            , const tCIDLib::EEventStates   eInitState = tCIDLib::EEventStates::Reset
            , const tCIDLib::ECreateActs    eCreateAct = tCIDLib::ECreateActs::OpenOrCreate
            , const tCIDLib::TBoolean       bManual    = kCIDLib::True
        );

        TEvent(const TEvent&) = delete;

        ~TEvent();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEvent& operator=(const TEvent&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsNamed() const;

        tCIDLib::TBoolean bWaitFor
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        );

        TKrnlEvent& kevImpl();

        const TResourceName& rsnName() const;

        tCIDLib::TVoid Pulse();

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid Trigger();

        tCIDLib::TVoid WaitFor
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitNamed
        (
                    tCIDLib::TBoolean&      bCreated
            , const tCIDLib::EEventStates   eInitState
            , const tCIDLib::ECreateActs    eCreateAct
            , const tCIDLib::TBoolean       bManual
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNamed
        //      This is set if the event is named or not.
        //
        //  m_kevImpl
        //      This is the kernel event object that provides the real
        //      implementation and platform abstraction.
        //
        //  m_rsnThis
        //      The name of the event, stored in a resource name object.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bNamed;
        TKrnlEvent          m_kevImpl;
        TResourceName       m_rsnThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEvent,TObject)
};

#pragma CIDLIB_POPPACK


