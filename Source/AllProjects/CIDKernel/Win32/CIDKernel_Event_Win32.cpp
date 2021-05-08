//
// FILE NAME: CIDKernel_Event_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//  This file provides the Win32 specific implementation for the class
//  TKrnlEvent.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"



// ---------------------------------------------------------------------------
//  Define our implementation of the event data
// ---------------------------------------------------------------------------
struct TKrnlEvent::TEventData
{
    HANDLE      m_hEvent;
};


// ---------------------------------------------------------------------------
//   CLASS: TKrnlEvent
//  PREFIX: kev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlEvent: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlEvent::bWaitMultiple(          TKrnlEvent&         kevOne
                            ,       TKrnlEvent&         kevTwo
                            ,       tCIDLib::TCard4&    c4Which
                            , const tCIDLib::TCard4     c4Wait)
{
    // Decide what the actual time is
    const DWORD dwRealWait = (c4Wait == kCIDLib::c4MaxCard) ? INFINITE : c4Wait;

    // Set up the array of handles
    HANDLE aHandles[2];
    aHandles[0] = kevOne.m_pData->m_hEvent;
    aHandles[1] = kevTwo.m_pData->m_hEvent;

    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandles
    (
        aHandles, 2, c4Which, c4Wait
    );
    return (eRes == tCIDLib::EWaitRes::Success);
}


// ---------------------------------------------------------------------------
//  TKrnlEvent: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlEvent::TKrnlEvent() :

    m_pData(new TEventData{0})
    , m_pszName(nullptr)
{
}

TKrnlEvent::TKrnlEvent(const tCIDLib::TCh* const pszName) :

    m_pData(new TEventData{0})
    , m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
}

TKrnlEvent::TKrnlEvent(TKrnlEvent&& kevSrc) :

    TKrnlEvent()
{
    *this = tCIDLib::ForceMove(kevSrc);
}

TKrnlEvent::~TKrnlEvent()
{
    if (m_pszName)
    {
        delete [] m_pszName;
        m_pszName = nullptr;
    }

    if (!bClose())
    {
        //
        //  If it fails and we are debugging, then do a popup. Otherwise
        //  there is not much we can do.
        //
        #if CID_DEBUG_ON
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
            , kmodCIDKernel.pszLoadCIDFacMsg(kKrnlErrs::errcGen_CloseHandle)
        );
        #endif
    }

    delete m_pData;
    m_pData->m_hEvent = nullptr;
}


// ---------------------------------------------------------------------------
//  TKrnlEvent: Public operators
// ---------------------------------------------------------------------------

TKrnlEvent& TKrnlEvent::operator=(TKrnlEvent&& kevSrc)
{
    if (this != &kevSrc)
    {
        tCIDLib::Swap(m_pszName, kevSrc.m_pszName);
        tCIDLib::Swap(m_pData, kevSrc.m_pData);
    }
    return *this;
}

// ---------------------------------------------------------------------------
//  TKrnlEvent: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlEvent::bClose()
{
    if (bIsValid())
    {
        if (!::CloseHandle(m_pData->m_hEvent))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }
    m_pData->m_hEvent = 0;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEvent::bCreate(const   tCIDLib::EEventStates   eInitState
                    , const tCIDLib::TBoolean       bManual)
{
    if (bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (m_pszName)
    {
        tCIDLib::TBoolean bDummy;
        return bCreateNamed(eInitState, kCIDLib::True, bManual, bDummy);
    }

    DWORD ManFlag = (bManual == kCIDLib::True);
    DWORD InitFlag = (eInitState == tCIDLib::EEventStates::Triggered);
    m_pData->m_hEvent = ::CreateEvent(0, ManFlag, InitFlag, 0);
    if (!m_pData->m_hEvent)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bDuplicate(const TKrnlEvent& kevToDup)
{
    // Close this event if open
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] m_pszName;
    m_pszName = nullptr;

    // Duplicate the source object's name
    if (kevToDup.m_pszName)
        m_pszName = TRawStr::pszReplicate(kevToDup.m_pszName);

    // Duplicate the handle
    const BOOL bRes = ::DuplicateHandle
    (
        kevToDup.m_pData->m_hEvent
        , GetCurrentProcess()
        , GetCurrentProcess()
        , &m_pData->m_hEvent
        , 0
        , 0
        , DUPLICATE_SAME_ACCESS
    );

    if (bRes == FALSE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bIsValid() const noexcept
{
    return m_pData->m_hEvent != 0;
}


tCIDLib::TBoolean TKrnlEvent::bOpen()
{
    if (bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    m_pData->m_hEvent = ::OpenEvent(EVENT_ALL_ACCESS | SYNCHRONIZE, 0, m_pszName);
    if (!m_pData->m_hEvent)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEvent::bOpenOrCreate(  const   tCIDLib::EEventStates   eInitState
                            , const tCIDLib::TBoolean       bManual
                            ,       tCIDLib::TBoolean&      bCreated)
{
    if (bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    return bCreateNamed(eInitState, kCIDLib::False, bManual, bCreated);
}


tCIDLib::TBoolean TKrnlEvent::bPulse()
{
    if (!bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (!::PulseEvent(m_pData->m_hEvent))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bReset()
{
    if (!bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (!::ResetEvent(m_pData->m_hEvent))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bSetName(const tCIDLib::TCh* const pszToSet)
{
    if (bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    delete [] m_pszName;
    m_pszName = nullptr;
    m_pszName = TRawStr::pszReplicate(pszToSet);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bTrigger()
{
    if (!bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (!::SetEvent(m_pData->m_hEvent))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bWaitFor(const tCIDLib::TCard4 c4Wait)
{
    if (!bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle(m_pData->m_hEvent, c4Wait);
    return (eRes == tCIDLib::EWaitRes::Success);
}


//
//  Provide access to our handle, which must be done generically. It should only be
//  accessed by other platform code that knows what it is.
//
tCIDLib::TVoid* TKrnlEvent::pHandle() const
{
    return &m_pData->m_hEvent;
}


// ---------------------------------------------------------------------------
//  TKrnlEvent: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlEvent::bCreateNamed(const  tCIDLib::EEventStates   eState
                        , const tCIDLib::TBoolean       bFailIfExists
                        , const tCIDLib::TBoolean       bManual
                        ,       tCIDLib::TBoolean&      bCreated)
{
    //
    //  The open did not work, so try to create it. Since this one is named,
    //  we have to set a default ACL on it so that it will be shareable from
    //  a service.
    //
    SECURITY_DESCRIPTOR*    pSDesc;
    pSDesc = reinterpret_cast<SECURITY_DESCRIPTOR*>
    (
        ::LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, SECURITY_DESCRIPTOR_MIN_LENGTH)
    );

    if (!pSDesc)
    {
        ::LocalDiscard(pSDesc);
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Initialize the allocated descriptor
    if (!::InitializeSecurityDescriptor(pSDesc, SECURITY_DESCRIPTOR_REVISION))
    {
        ::LocalDiscard(pSDesc);
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // And now set up the default ACL
    if (!::SetSecurityDescriptorDacl(pSDesc, TRUE, (PACL)0, FALSE))
    {
        ::LocalDiscard(pSDesc);
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Set up a security attributes that we can pass in
    SECURITY_ATTRIBUTES     SAttrs;
    SAttrs.nLength = sizeof(SAttrs);
    SAttrs.lpSecurityDescriptor = pSDesc;
    SAttrs.bInheritHandle = TRUE;

    DWORD ManFlag = (bManual == kCIDLib::True);
    DWORD InitFlag = (eState == tCIDLib::EEventStates::Triggered);
    m_pData->m_hEvent = ::CreateEvent(&SAttrs, ManFlag, InitFlag, m_pszName);

    // Either way now, we can clean up the descriptor
    ::LocalDiscard(pSDesc);

    // If it failed, then give up
    if (!m_pData->m_hEvent)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  Get the last error to see if it already existed. If it comes back
    //  something other than already exists, then we created it.
    //
    bCreated = (::GetLastError() != ERROR_ALREADY_EXISTS);

    //
    //  If we didn't create it, but they asked to fail if exists, then close
    //  it again and return a failure.
    //
    if (!bCreated && bFailIfExists)
    {
        ::CloseHandle(m_pData->m_hEvent);
        m_pData->m_hEvent = 0;
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

