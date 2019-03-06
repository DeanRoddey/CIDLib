//
// FILE NAME: CIDKernel_Mutex_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation of the class
//  TKrnlMutex class.
//
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
//   CLASS: TMutexHandle
//  PREFIX: hmtx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMutexHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TMutexHandle::TMutexHandle() :

    m_phmtxiThis(nullptr)
{
    m_phmtxiThis = (TMutexHandleImpl*)::_aligned_malloc(sizeof(TMutexHandleImpl), 32);
    m_phmtxiThis->hMutex = 0;
}

TMutexHandle::TMutexHandle(const TMutexHandle& hmtxToCopy) :

    m_phmtxiThis(nullptr)
{
    m_phmtxiThis = (TMutexHandleImpl*)::_aligned_malloc(sizeof(TMutexHandleImpl), 32);
    m_phmtxiThis->hMutex = hmtxToCopy.m_phmtxiThis->hMutex;
}

TMutexHandle::~TMutexHandle()
{
    ::_aligned_free(m_phmtxiThis);
    m_phmtxiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TMutexHandle& TMutexHandle::operator=(const TMutexHandle& hmtxToAssign)
{
    if (this == &hmtxToAssign)
        return *this;

    m_phmtxiThis->hMutex = hmtxToAssign.m_phmtxiThis->hMutex;
    return *this;
}


tCIDLib::TBoolean
TMutexHandle::operator==(const TMutexHandle& hmtxToCompare) const
{
    return (m_phmtxiThis->hMutex == hmtxToCompare.m_phmtxiThis->hMutex);
}

tCIDLib::TBoolean
TMutexHandle::operator!=(const TMutexHandle& hmtxToCompare) const
{
    return !operator==(hmtxToCompare);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TMutexHandle::bIsValid() const
{
    return ((m_phmtxiThis != nullptr) && (m_phmtxiThis->hMutex != 0));
}

tCIDLib::TVoid TMutexHandle::Clear()
{
    m_phmtxiThis->hMutex = 0;
}

tCIDLib::TVoid
TMutexHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phmtxiThis->hMutex)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}

const TMutexHandleImpl& TMutexHandle::hmtxiThis() const
{
    return *m_phmtxiThis;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlMutex
//  PREFIX: kmtx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlMutex: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlMutex::TKrnlMutex() :

    m_pszName(nullptr)
{
}

TKrnlMutex::TKrnlMutex(const tCIDLib::TCh* const pszName) :

    m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
}

TKrnlMutex::~TKrnlMutex()
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
//  TKrnlMutex: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TKrnlMutex::bClose()
{
    if (bIsValid())
    {
        if (!::CloseHandle(m_hmtxThis.m_phmtxiThis->hMutex))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }

    if (m_hmtxThis.m_phmtxiThis)
        TKrnlWin32::AtomicHandleSet(m_hmtxThis.m_phmtxiThis->hMutex, 0);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlMutex::bCreate(const tCIDLib::ELockStates eInitState)
{
    if (bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    // Make sure we've not been destructed already!
    if (!m_hmtxThis.m_phmtxiThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    if (m_pszName)
    {
        tCIDLib::TBoolean bDummy;
        return bCreateNamed(eInitState, kCIDLib::True, bDummy);
    }

    HANDLE hTmp = ::CreateMutex(0, eInitState == tCIDLib::ELockStates::Locked, 0);
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // We survived, so store the handle
    TKrnlWin32::AtomicHandleSet(m_hmtxThis.m_phmtxiThis->hMutex, hTmp);
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bDuplicate(const TKrnlMutex& kmtxToDup)
{
    // Make sure we've not been destructed already!
    if (!m_hmtxThis.m_phmtxiThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    // Close it if open
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] m_pszName;
    m_pszName = nullptr;

    // Recreate the name
    if (kmtxToDup.m_pszName)
        m_pszName = TRawStr::pszReplicate(kmtxToDup.m_pszName);

    // Duplicate the handle
    HANDLE hTmp;
    if (!::DuplicateHandle
    (
        kmtxToDup.m_hmtxThis.m_phmtxiThis->hMutex
        , TKrnlProcess::hprocThis().hprociThis().hProcess
        , TKrnlProcess::hprocThis().hprociThis().hProcess
        , &hTmp
        , 0
        , 0
        , DUPLICATE_SAME_ACCESS))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // It went ok, so store the new handle
    TKrnlWin32::AtomicHandleSet(m_hmtxThis.m_phmtxiThis->hMutex, hTmp);
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlMutex::bLock(const tCIDLib::TCard4 c4MilliSecs) const
{
    if (!bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle
    (
        m_hmtxThis.m_phmtxiThis->hMutex, c4MilliSecs
    );
    return (eRes == tCIDLib::EWaitRes::Success);
}


tCIDLib::TBoolean TKrnlMutex::bOpen()
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
    if (!m_hmtxThis.m_phmtxiThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    HANDLE hTmp = ::OpenMutex(MUTEX_ALL_ACCESS | SYNCHRONIZE, 0, m_pszName);
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // We survived, so store the handle
    TKrnlWin32::AtomicHandleSet(m_hmtxThis.m_phmtxiThis->hMutex, hTmp);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlMutex::bOpenOrCreate(  const   tCIDLib::ELockStates    eInitState
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

    return bCreateNamed(eInitState, kCIDLib::False, bCreated);
}


tCIDLib::TBoolean TKrnlMutex::bSetName(const tCIDLib::TCh* const pszNewName)
{
    if (bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    // Delete the old name and replicate the new one
    delete [] m_pszName;
    m_pszName = nullptr;
    m_pszName = TRawStr::pszReplicate(pszNewName);

    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlMutex::bUnlock() const
{
    if (!bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (!::ReleaseMutex(m_hmtxThis.m_phmtxiThis->hMutex))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlMutex: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlMutex::bCreateNamed(const  tCIDLib::ELockStates    eState
                        , const tCIDLib::TBoolean       bFailIfExists
                        ,       tCIDLib::TBoolean&      bCreated)
{
    // This is a named one, so set up the default security descriptor
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

    //
    //  And do the call, setting up the initial owner flag according to
    //  the incoming initial lock state.
    //
    BOOL bInitOwner(eState == tCIDLib::ELockStates::Locked ? TRUE : FALSE);
    HANDLE hTmp = ::CreateMutex(&SAttrs, bInitOwner, m_pszName);

    // Either way now, we can clean up the descriptor
    ::LocalDiscard(pSDesc);

    // If it failed, then we give up now
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

    // It worked, so store the platform specific handle info
    TKrnlWin32::AtomicHandleSet(m_hmtxThis.m_phmtxiThis->hMutex, hTmp);
    return kCIDLib::True;
}


