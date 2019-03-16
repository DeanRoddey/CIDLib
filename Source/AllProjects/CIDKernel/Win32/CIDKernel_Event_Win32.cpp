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
//   CLASS: TEventHandle
//  PREFIX: hev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEventHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TEventHandle::TEventHandle() :

    m_pheviThis(nullptr)
{
    m_pheviThis = (TEventHandleImpl*)::_aligned_malloc(sizeof(TEventHandleImpl), 32);
    m_pheviThis->hEvent = 0;
}

TEventHandle::TEventHandle(const TEventHandle& hevToCopy) :

    m_pheviThis(nullptr)
{
    m_pheviThis = (TEventHandleImpl*)::_aligned_malloc(sizeof(TEventHandleImpl), 32);
    m_pheviThis->hEvent = hevToCopy.m_pheviThis->hEvent;
}

TEventHandle::~TEventHandle()
{
    ::_aligned_free(m_pheviThis);
    m_pheviThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TEventHandle& TEventHandle::operator=(const TEventHandle& hevToAssign)
{
    if (this == &hevToAssign)
        return *this;

    m_pheviThis->hEvent = hevToAssign.m_pheviThis->hEvent;

    return *this;
}


tCIDLib::TBoolean
TEventHandle::operator==(const TEventHandle& hevToCompare) const
{
    return (m_pheviThis->hEvent == hevToCompare.m_pheviThis->hEvent);
}

tCIDLib::TBoolean
TEventHandle::operator!=(const TEventHandle& hevToCompare) const
{
    return !operator==(hevToCompare);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TEventHandle::bIsValid() const
{
    return ((m_pheviThis != nullptr) && (m_pheviThis->hEvent != 0));
}


tCIDLib::TVoid TEventHandle::Clear()
{
    m_pheviThis->hEvent = 0;
}


tCIDLib::TVoid
TEventHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_pheviThis->hEvent)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}


const TEventHandleImpl& TEventHandle::heviThis() const
{
    return *m_pheviThis;
}



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
    aHandles[0] = kevOne.m_hevThis.m_pheviThis->hEvent;
    aHandles[1] = kevTwo.m_hevThis.m_pheviThis->hEvent;

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

    m_pszName(nullptr)
{
}

TKrnlEvent::TKrnlEvent(const tCIDLib::TCh* const pszName) :

    m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
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
}


// ---------------------------------------------------------------------------
//  TKrnlEvent: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlEvent::bClose()
{
    if (bIsValid())
    {
        if (!::CloseHandle(m_hevThis.m_pheviThis->hEvent))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }
    TKrnlWin32::AtomicHandleSet(m_hevThis.m_pheviThis->hEvent, 0);
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

    // Make sure we've not been destructed already!
    if (!m_hevThis.m_pheviThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    DWORD ManFlag = (bManual == kCIDLib::True);
    DWORD InitFlag = (eInitState == tCIDLib::EEventStates::Triggered);
    HANDLE hTmp = ::CreateEvent(0, ManFlag, InitFlag, 0);
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // We survived, so store the handle
    TKrnlWin32::AtomicHandleSet(m_hevThis.m_pheviThis->hEvent, hTmp);
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlEvent::bDuplicate(const TKrnlEvent& kevToDup)
{
    // Make sure we've not been destructed already!
    if (!m_hevThis.m_pheviThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

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
    HANDLE hTmp;
    if (!::DuplicateHandle
    (
        kevToDup.m_hevThis.m_pheviThis->hEvent
        , GetCurrentProcess()
        , GetCurrentProcess()
        , &hTmp
        , 0
        , 0
        , DUPLICATE_SAME_ACCESS))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    TKrnlWin32::AtomicHandleSet(m_hevThis.m_pheviThis->hEvent, hTmp);
    return kCIDLib::True;
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

    // Make sure we've not been destructed already!
    if (!m_hevThis.m_pheviThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    HANDLE hTmp = ::OpenEvent(EVENT_ALL_ACCESS | SYNCHRONIZE, 0, m_pszName);
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // We survived, so store the handle
    TKrnlWin32::AtomicHandleSet(m_hevThis.m_pheviThis->hEvent, hTmp);
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

    if (!::PulseEvent(m_hevThis.m_pheviThis->hEvent))
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

    if (!::ResetEvent(m_hevThis.m_pheviThis->hEvent))
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

    if (!::SetEvent(m_hevThis.m_pheviThis->hEvent))
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

    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle
    (
        m_hevThis.m_pheviThis->hEvent, c4Wait
    );
    return (eRes == tCIDLib::EWaitRes::Success);
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
    HANDLE hTmp;

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
    hTmp = ::CreateEvent(&SAttrs, ManFlag, InitFlag, m_pszName);

    // Either way now, we can clean up the descriptor
    ::LocalDiscard(pSDesc);

    // If it failed, then give up
    if (!hTmp)
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
        ::CloseHandle(hTmp);
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    // It worked, so store the handle
    TKrnlWin32::AtomicHandleSet(m_hevThis.m_pheviThis->hEvent, hTmp);
    return kCIDLib::True;
}


