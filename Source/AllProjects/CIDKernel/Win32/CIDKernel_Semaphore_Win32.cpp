//
// FILE NAME: CIDKernel_Semaphore_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1998
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
//  TKrnlSemaphore.
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
//   CLASS: TSemaphoreHandle
//  PREFIX: hsem
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSemaphoreHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TSemaphoreHandle::TSemaphoreHandle() :

    m_phsemiThis(nullptr)
{
    m_phsemiThis = new TSemaphoreHandleImpl;
    m_phsemiThis->hSem = 0;
}

TSemaphoreHandle::TSemaphoreHandle(const TSemaphoreHandle& hsemToCopy) :

    m_phsemiThis(nullptr)
{
    m_phsemiThis = new TSemaphoreHandleImpl;
    m_phsemiThis->hSem = hsemToCopy.m_phsemiThis->hSem;
}

TSemaphoreHandle::~TSemaphoreHandle()
{
    delete m_phsemiThis;
    m_phsemiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TSemaphoreHandle&
TSemaphoreHandle::operator=(const TSemaphoreHandle& hsemToAssign)
{
    if (this == &hsemToAssign)
        return *this;

    m_phsemiThis->hSem = hsemToAssign.m_phsemiThis->hSem;
    return *this;
}


tCIDLib::TBoolean
TSemaphoreHandle::operator==(const TSemaphoreHandle& hsemToCompare) const
{
    return (m_phsemiThis->hSem == hsemToCompare.m_phsemiThis->hSem);
}

tCIDLib::TBoolean
TSemaphoreHandle::operator!=(const TSemaphoreHandle& hsemToCompare) const
{
    return !operator==(hsemToCompare);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TSemaphoreHandle::bIsValid() const
{
    return ((m_phsemiThis != nullptr) && (m_phsemiThis->hSem != 0));
}

tCIDLib::TVoid TSemaphoreHandle::Clear()
{
    m_phsemiThis->hSem = 0;
}

tCIDLib::TVoid
TSemaphoreHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phsemiThis->hSem), pszToFill, c4MaxChars, tCIDLib::ERadices::Hex
    );
}

const TSemaphoreHandleImpl& TSemaphoreHandle::hsemiThis() const
{
    return *m_phsemiThis;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlSemaphore
//  PREFIX: ksem
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSemaphore: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSemaphore::TKrnlSemaphore(const tCIDLib::TCard4 c4MaxCount) :

    m_c4MaxCount(c4MaxCount)
    , m_pszName(nullptr)
{
}

TKrnlSemaphore::TKrnlSemaphore( const   tCIDLib::TCh* const pszName
                                , const tCIDLib::TCard4     c4MaxCount) :
    m_c4MaxCount(c4MaxCount)
    , m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
}

TKrnlSemaphore::~TKrnlSemaphore()
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
//  TKrnlSemaphore: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSemaphore::bClose()
{
    if (m_hsemThis.bIsValid())
    {
        if (!::CloseHandle(m_hsemThis.m_phsemiThis->hSem))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }

    if (m_hsemThis.m_phsemiThis)
        m_hsemThis.m_phsemiThis->hSem = 0;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bCreate(const tCIDLib::TCard4 c4InitCount)
{
    if (m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (m_pszName)
    {
        tCIDLib::TBoolean bDummy;
        return bCreateNamed(c4InitCount, kCIDLib::True, bDummy);
    }

    // Make sure we've not been destructed already!
    if (!m_hsemThis.m_phsemiThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    // Not named, so create an unnamed one
    m_hsemThis.m_phsemiThis->hSem = ::CreateSemaphore(0, c4InitCount, m_c4MaxCount, 0);
    if (!m_hsemThis.m_phsemiThis->hSem)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bDuplicate(const TKrnlSemaphore& ksemToDup)
{
    // Make sure we've not been destructed already!
    if (!m_hsemThis.m_phsemiThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    // Close this semaphore if open
    if (!bClose())
        return kCIDLib::False;

    // Clean up the name
    delete [] m_pszName;
    m_pszName = nullptr;

    // Copy over the info
    m_c4MaxCount = ksemToDup.m_c4MaxCount;

    // Recreate the name
    if (ksemToDup.m_pszName)
        m_pszName = TRawStr::pszReplicate(ksemToDup.m_pszName);

    // Duplicate the handle
    BOOL bRes = ::DuplicateHandle
    (
        ksemToDup.m_hsemThis.m_phsemiThis->hSem
        , TKrnlProcess::hprocThis().hprociThis().hProcess
        , TKrnlProcess::hprocThis().hprociThis().hProcess
        , &m_hsemThis.m_phsemiThis->hSem
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


tCIDLib::TBoolean TKrnlSemaphore::bEnter(const tCIDLib::TCard4 c4MilliSecs)
{
    if (!m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle
    (
        m_hsemThis.m_phsemiThis->hSem, c4MilliSecs
    );
    return eRes == tCIDLib::EWaitRes::Success;
}


tCIDLib::TBoolean TKrnlSemaphore::bExit()
{
    if (!m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    if (!::ReleaseSemaphore(m_hsemThis.m_phsemiThis->hSem, 1, 0))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSemaphore::bOpen()
{
    if (m_hsemThis.bIsValid())
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
    if (!m_hsemThis.m_phsemiThis)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotInitialized);
        return kCIDLib::False;
    }

    m_hsemThis.m_phsemiThis->hSem = ::OpenEvent(EVENT_ALL_ACCESS | SYNCHRONIZE, 0, m_pszName);
    if (!m_hsemThis.m_phsemiThis->hSem)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSemaphore::bOpenOrCreate(          tCIDLib::TBoolean&  bCreated
                                , const tCIDLib::TCard4     c4InitCount)
{
    if (m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    return bCreateNamed(c4InitCount, kCIDLib::False, bCreated);
}


tCIDLib::TBoolean TKrnlSemaphore::bSetName(const tCIDLib::TCh* const pszToSet)
{
    if (m_hsemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    delete [] m_pszName;
    m_pszName = nullptr;
    m_pszName = TRawStr::pszReplicate(pszToSet);

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlSemaphore: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSemaphore::bCreateNamed(const  tCIDLib::TCard4     c4InitCount
                            , const tCIDLib::TBoolean   bFailIfExists
                            ,       tCIDLib::TBoolean&  bCreated)
{
    //
    //  The open did not work, so try to create it. Since this one is named,
    //  we have to set a default ACL on it so that it will be shareable from
    //  a service.
    //
    SECURITY_DESCRIPTOR* pSDesc;
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

    m_hsemThis.m_phsemiThis->hSem = ::CreateSemaphore
    (
        &SAttrs, c4InitCount, m_c4MaxCount, m_pszName
    );

    // Either way now, we can clean up the descriptor
    ::LocalDiscard(pSDesc);

    // If it failed, then give up
    if (!m_hsemThis.m_phsemiThis->hSem)
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
        ::CloseHandle(m_hsemThis.m_phsemiThis->hSem);
        m_hsemThis.m_phsemiThis->hSem = 0;
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


