//
// FILE NAME: CIDLib_Event.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/20/1993
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
//  This file implements the TEvent class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TEvent,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TEvent
//  PREFIX: ev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEvent: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TEvent::c4WaitMultiple(         TEvent&             evOne
                        ,       TEvent&             evTwo
                        , const tCIDLib::TCard4     c4WaitFor
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDLib::TCard4 c4Ret = kCIDLib::c4MaxCard;
    if (!TKrnlEvent::bWaitMultiple( evOne.m_kevImpl
                                    , evTwo.m_kevImpl
                                    , c4Ret
                                    , c4WaitFor))
    {
        TKrnlError kerrLast = TKrnlError::kerrLast();
        if (kerrLast.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            if (bThrowIfNot)
            {
                facCIDLib().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcEv_MultiTimeout
                    , kerrLast
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
            return kCIDLib::c4MaxCard;
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_MultiWait
            , kerrLast
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return c4Ret;
}


// ---------------------------------------------------------------------------
//  TEvent: Constructors and Destructor
// ---------------------------------------------------------------------------
TEvent::TEvent( const   tCIDLib::EEventStates   eInitState
                , const tCIDLib::TBoolean       bManual) :

    m_bNamed(kCIDLib::False)
{
    if (!m_kevImpl.bCreate(eInitState, bManual))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Create
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
        );
    }
}

TEvent::TEvent( const   TResourceName&          rsnToUse
                , const tCIDLib::EEventStates   eInitState
                , const tCIDLib::ECreateActs eAction
                , const tCIDLib::TBoolean       bManual) :

    m_bNamed(kCIDLib::True)
    , m_kevImpl(rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Event).pszBuffer())
    , m_rsnThis(rsnToUse)
{
    tCIDLib::TBoolean bDummy;
    InitNamed(bDummy, eInitState, eAction, bManual);
}

TEvent::TEvent( const   TResourceName&          rsnToUse
                ,       tCIDLib::TBoolean&      bCreated
                , const tCIDLib::EEventStates   eInitState
                , const tCIDLib::ECreateActs eAction
                , const tCIDLib::TBoolean       bManual) :

    m_bNamed(kCIDLib::True)
    , m_kevImpl(rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Event).pszBuffer())
    , m_rsnThis(rsnToUse)
{
    InitNamed(bCreated, eInitState, eAction, bManual);
}

TEvent::~TEvent()
{
    if (!m_kevImpl.bClose())
    {
        //
        //  If we get an error, log it but don't throw out of the destructor
        //  so we use a warning message.
        //
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
        );
    }
}


// ---------------------------------------------------------------------------
//  TEvent: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TEvent::bIsNamed() const
{
    return m_bNamed;
}


tCIDLib::TBoolean TEvent::bWaitFor(const tCIDLib::TCard4 c4Timeout)
{
    if (!m_kevImpl.bWaitFor(c4Timeout))
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        if (kerrRes.errcId() != kKrnlErrs::errcGen_Timeout)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Wait
                , kerrRes
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


TKrnlEvent& TEvent::kevImpl()
{
    return m_kevImpl;
}


tCIDLib::TVoid TEvent::Pulse()
{
    if (!m_kevImpl.bPulse())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Pulse
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
        );
    }
}


tCIDLib::TVoid TEvent::Reset()
{
    if (!m_kevImpl.bReset())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Reset
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
        );
    }
}


const TResourceName& TEvent::rsnName() const
{
    return m_rsnThis;
}


tCIDLib::TVoid TEvent::Trigger()
{
    if (!m_kevImpl.bTrigger())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcEv_Trigger
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
        );
    }
}


tCIDLib::TVoid TEvent::WaitFor(const tCIDLib::TCard4 c4Timeout)
{
    if (!m_kevImpl.bWaitFor(c4Timeout))
    {
        const TKrnlError& kerrRes = TKrnlError::kerrLast();
        if (kerrRes.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Timeout
                , kerrRes
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
            );
        }
         else
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Wait
                , kerrRes
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TEvent: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TEvent::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event);
}


// ---------------------------------------------------------------------------
//  TEvent: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TEvent::InitNamed(          tCIDLib::TBoolean&      bCreated
                    , const tCIDLib::EEventStates   eInitState
                    , const tCIDLib::ECreateActs eCreateAct
                    , const tCIDLib::TBoolean       bManual)
{
    if (eCreateAct == tCIDLib::ECreateActs::CreateIfNew)
    {
        if (!m_kevImpl.bCreate(eInitState, bManual))
        {
            const TKrnlError& kerrLast = TKrnlError::kerrLast();
            tCIDLib::TErrCode errcToThrow = 0;
            if (kerrLast.errcId() == kKrnlErrs::errcGen_AlreadyExists)
                errcToThrow = kCIDErrs::errcEv_AlreadyExists;
            else
                errcToThrow = kCIDErrs::errcEv_Create;

            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Create
                , kerrLast
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
            );
        }

        // It's always created (or fails) in this case
        bCreated = kCIDLib::True;
    }
     else if (eCreateAct == tCIDLib::ECreateActs::OpenOrCreate)
    {
        if (!m_kevImpl.bOpenOrCreate(eInitState, bManual, bCreated))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_OpenOrCreate
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
            );
        }
    }
     else if (eCreateAct == tCIDLib::ECreateActs::OpenIfExists)
    {
        if (!m_kevImpl.bOpen())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcEv_Open
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
            );
        }

        // It's never created in this case
        bCreated = kCIDLib::False;
    }
     else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadCreateAct
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eCreateAct))
            , TString(L"event")
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Event)
        );
    }
}


