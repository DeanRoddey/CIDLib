//
// FILE NAME: CIDKernel_SharedMemBuf_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/134/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation of the class
//  TKrnlSharedMemBuf
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



// ---------------------------------------------------------------------------
//   CLASS: TMemoryHandle
//  PREFIX: hev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMemoryHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TMemoryHandle::TMemoryHandle() :

    m_phmemiThis(0)
{
    m_phmemiThis = new TMemoryHandleImpl;
    m_phmemiThis->hBacking = 0;
}

TMemoryHandle::TMemoryHandle(const TMemoryHandle& hevToCopy) :

    m_phmemiThis(0)
{
    m_phmemiThis = new TMemoryHandleImpl;
    m_phmemiThis->hBacking = hevToCopy.m_phmemiThis->hBacking;
}

TMemoryHandle::~TMemoryHandle()
{
    delete m_phmemiThis;
    m_phmemiThis = 0;
}


// ---------------------------------------------------------------------------
//  TMemoryHandle: Public operators
// ---------------------------------------------------------------------------
TMemoryHandle& TMemoryHandle::operator=(const TMemoryHandle& hevToAssign)
{
    if (this == &hevToAssign)
        return *this;

    m_phmemiThis->hBacking = hevToAssign.m_phmemiThis->hBacking;

    return *this;
}


tCIDLib::TBoolean
TMemoryHandle::operator==(const TMemoryHandle& hevToCompare) const
{
    return (m_phmemiThis->hBacking == hevToCompare.m_phmemiThis->hBacking);
}

tCIDLib::TBoolean
TMemoryHandle::operator!=(const TMemoryHandle& hmemToCompare) const
{
    return !operator==(hmemToCompare);
}


// ---------------------------------------------------------------------------
//  TMemoryHandle: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMemoryHandle::bIsValid() const
{
    return ((m_phmemiThis != 0) && (m_phmemiThis->hBacking != 0));
}

tCIDLib::TVoid TMemoryHandle::Clear()
{
    m_phmemiThis->hBacking = 0;
}

tCIDLib::TVoid
TMemoryHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phmemiThis->hBacking)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}

const TMemoryHandleImpl& TMemoryHandle::hmemiThis() const
{
    return *m_phmemiThis;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlSharedMemBuf
//  PREFIX: ksmb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSharedMemBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSharedMemBuf::TKrnlSharedMemBuf() :

    m_c4AllocatedPages(0)
    , m_c4MaxPages(0)
    , m_eAccess(tCIDLib::EMemAccFlags::ReadOnly)
    , m_eAllocType(tCIDLib::EAllocTypes::Commit)
    , m_pBuffer(0)
    , m_pszName(0)
{
}

TKrnlSharedMemBuf::~TKrnlSharedMemBuf()
{
    if (!bFree())
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
//  TKrnlSharedMemBuf: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSharedMemBuf::operator==(const TKrnlSharedMemBuf& ksmbSrc) const
{
    if (&ksmbSrc != this)
    {
        //
        //  We only compare the name and access at this level. The public CIDLib
        //  class that uses us as its implementation, TSharedMemBuf, will handle
        //  the actual memory comparison.
        //
        if (!TRawStr::bCompareStr(m_pszName, ksmbSrc.m_pszName))
            return kCIDLib::False;

        if (m_eAccess != ksmbSrc.m_eAccess)
            return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlSharedMemBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSharedMemBuf::bAlloc(  const   tCIDLib::TCh* const     pszName
                            , const tCIDLib::TCard4         c4MaxSize
                            , const tCIDLib::EAllocTypes    eAllocType
                            , const tCIDLib::EMemAccFlags   eAccess
                            ,       tCIDLib::TBoolean&      bCreated
                            , const tCIDLib::ECreateActs eCreateFlags)
{
    if (m_pBuffer)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyAllocated);
        return kCIDLib::False;
    }

    if (!pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    // Replicate the name of the buffer
    m_pszName = TRawStr::pszReplicate(pszName);

    //
    //  Store the page sizes and access flags. If its a committed buffer,
    //  then go ahead and set the allocated to the max. Else, set it to
    //  zero.
    //
    m_c4MaxSize  = c4MaxSize;
    m_eAccess = eAccess;
    m_eAllocType = eAllocType;

    m_c4MaxPages = (c4MaxSize / kCIDLib::c4MemPageSize);
    if (c4MaxSize % kCIDLib::c4MemPageSize)
        m_c4MaxPages++;

    if (eAllocType == tCIDLib::EAllocTypes::Commit)
        m_c4AllocatedPages = m_c4MaxPages;
    else
        m_c4AllocatedPages = 0;

    //
    //  Check for invalid create modes for a shared memory buffer and
    //  return an error.
    //
    if ((eCreateFlags == tCIDLib::ECreateActs::TruncateExisting)
    ||  (eCreateFlags == tCIDLib::ECreateActs::None))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    //
    //  Ok, so now create a file access set of flags that match the
    //  memory access flags we just did above. We need one set for
    //  some calls and one set for others.
    //
    //  We already checked for invalid access, so we don't have to
    //  check again.
    //
    tCIDLib::TCard4 c4FileAccess = 0;
    if (m_eAccess == tCIDLib::EMemAccFlags::ReadOnly)
        c4FileAccess = FILE_MAP_READ;
    else if (m_eAccess == tCIDLib::EMemAccFlags::ReadWrite)
        c4FileAccess = FILE_MAP_WRITE;

    //
    //  Try to create the handle and buffer pointer. Init them first
    //  to zero so we can clean up on exception.
    //
    bCreated = kCIDLib::False;
    HANDLE              hFl = 0;
    tCIDLib::TVoid*     pBuf = 0;

    hFl = ::OpenFileMapping(c4FileAccess, 0, pszName);
    if (!hFl)
    {
        // If the create action is 'open if exists' we are done now
        if (eCreateFlags == tCIDLib::ECreateActs::OpenIfExists)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }

        // Convert our access flags to the appropriate ones.
        tCIDLib::TCard4 c4Access = 0;

        if (eAllocType == tCIDLib::EAllocTypes::Commit)
            c4Access |= SEC_COMMIT;
        else
            c4Access |= SEC_RESERVE;

        if (m_eAccess == tCIDLib::EMemAccFlags::ReadOnly)
            c4Access |= PAGE_READONLY;
        else if (m_eAccess == tCIDLib::EMemAccFlags::ReadWrite)
            c4Access |= PAGE_READWRITE;
        else
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcAcc_InvalidAccess);
            return kCIDLib::False;
        }

        // Ok, so lets try to create it
        hFl = ::CreateFileMapping
        (
            INVALID_HANDLE_VALUE
            , 0
            , c4Access
            , 0
            , m_c4MaxPages * kCIDLib::c4MemPageSize
            , pszName
        );

        // If this one fails, then we just give up
        if (!hFl)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }

        bCreated = kCIDLib::True;
    }
     else
    {
        //
        //  If the flags indicate to fail if it exists, then we need
        //  to close it down and return an already exists error.
        //
        if (eCreateFlags == tCIDLib::ECreateActs::CreateIfNew)
        {
            ::CloseHandle(hFl);
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
            return kCIDLib::False;
        }
    }

    //
    //  Now create the mapping, mapping to the whole thing. It might not
    //  be allocated yet, but we have to be able to see it all.
    //
    pBuf = ::MapViewOfFile
    (
        hFl, c4FileAccess, 0, 0, m_c4MaxPages * kCIDLib::c4MemPageSize
    );

    if (!pBuf)
    {
        ::CloseHandle(hFl);
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  If we didn't created it, then get the current info about it, so
    //  we can be in sync in terms of size and allocation.
    //
    if (!bCreated)
    {
        TRawMem::TSysMemInfo Info;
        TRawMem::pQueryMemFlags(pBuf, Info);

        m_eAccess = Info.eAllocAccess;

        // If the base page is commited, then take the region size
        if (tCIDLib::bAllBitsOn(Info.eRegionState, tCIDLib::EMemStates::Commit))
            m_c4AllocatedPages = Info.c4RegionSize / kCIDLib::c4MemPageSize;
    }


    // Store the handle and buffer pointer
    m_hmemThis.m_phmemiThis->hBacking = hFl;
    m_pBuffer = pBuf;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSharedMemBuf::bCommitToSize(const tCIDLib::TCard4 c4NewSize)
{
    // If this is beyond the max size, its an error
    if (c4NewSize > m_c4MaxSize)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
        return kCIDLib::False;
    }

    //
    //  Calculate the page address of the next unallocated page, and the
    //  bytes we've already allocated.
    //
    const tCIDLib::TCard4 c4CurAlloc = (m_c4AllocatedPages * kCIDLib::c4MemPageSize);
    tCIDLib::TVoid* pPageAdr = pNextToCommit();

    //
    //  Now calculate how many pages we have to commit to get up to the
    //  new size. This is passed along to the commit function.
    //
    const tCIDLib::TCard4 c4Diff = c4NewSize - c4CurAlloc;
    tCIDLib::TCard4 c4Pages = c4Diff / kCIDLib::c4MemPageSize;
    if (c4Diff % kCIDLib::c4MemPageSize)
        c4Pages++;

    const tCIDLib::TBoolean bRet = TRawMem::bCommitPages
    (
        pPageAdr, c4Pages, m_eAccess
    );

    // If it succeeded, bump the allocated pages count
    if (bRet)
        m_c4AllocatedPages += c4Pages;

    return bRet;
}


tCIDLib::TBoolean
TKrnlSharedMemBuf::bDuplicate(const TKrnlSharedMemBuf& ksmbToCopy)
{
    // Can't do this if there is already a buffer in this object
    if (m_pBuffer)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyAllocated);
        return kCIDLib::False;
    }

    //
    //  Try to duplicate the handle first. If this doesn't work, then there
    //  is nothing left to do.
    //
    HANDLE hTmp;
    if (!::DuplicateHandle
    (
        TKrnlProcess::hprocThis().hprociThis().hProcess
        , ksmbToCopy.m_hmemThis.m_phmemiThis->hBacking
        , TKrnlProcess::hprocThis().hprociThis().hProcess
        , &hTmp
        , 0
        , 0
        , DUPLICATE_SAME_ACCESS))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  Convert its access rights to the system flags
    //  platform access flags for the mapping step.
    //
    tCIDLib::TCard4 c4FileAccess = 0;
    if (ksmbToCopy.m_eAccess == tCIDLib::EMemAccFlags::ReadOnly)
        c4FileAccess = FILE_MAP_READ;
    else if (ksmbToCopy.m_eAccess == tCIDLib::EMemAccFlags::ReadWrite)
        c4FileAccess = FILE_MAP_WRITE;

    // Create a mapping for this new handle
    tCIDLib::TVoid* pBuf = ::MapViewOfFile(hTmp, c4FileAccess, 0, 0, 0);

    if (!pBuf)
    {
        // It failed so destroy the temp handle and throw an error
        ::CloseHandle(hTmp);
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // It worked so store away the info
    m_pBuffer           = pBuf;
    m_c4AllocatedPages  = ksmbToCopy.m_c4AllocatedPages;
    m_c4MaxPages        = ksmbToCopy.m_c4MaxPages;
    m_c4MaxSize         = ksmbToCopy.m_c4MaxSize;
    m_eAccess           = ksmbToCopy.m_eAccess;
    m_eAllocType        = ksmbToCopy.m_eAllocType;

    delete [] m_pszName;
    m_pszName = TRawStr::pszReplicate(ksmbToCopy.m_pszName);

    // Store our handle now that we know its all happy
    m_hmemThis.m_phmemiThis->hBacking = hTmp;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSharedMemBuf::bFree()
{
    // If the name got allocated, then free it
    delete [] m_pszName;
    m_pszName = 0;

    // If we got the memory allocated, then free it
    tCIDLib::TBoolean bRet = kCIDLib::True;
    if (m_pBuffer)
    {
        if (!::UnmapViewOfFile(m_pBuffer))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            bRet = kCIDLib::False;
        }

        if (!::CloseHandle(m_hmemThis.m_phmemiThis->hBacking))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            bRet = kCIDLib::False;
        }

        m_pBuffer = 0;
        m_hmemThis.m_phmemiThis->hBacking = 0;
    }
    return bRet;
}


tCIDLib::TBoolean TKrnlSharedMemBuf::bSyncView()
{
    //
    //  Probe the memory to see how far it's been extended by current
    //  users of it.
    //
    TRawMem::TSysMemInfo Info;
    TRawMem::pQueryMemFlags(m_pBuffer, Info);

    // If the base page is commited, then update our info
    if (tCIDLib::bAllBitsOn(Info.eRegionState,  tCIDLib::EMemStates::Commit))
        m_c4AllocatedPages = Info.c4RegionSize / kCIDLib::c4MemPageSize;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSharedMemBuf::bZero()
{
    if (!m_pBuffer)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Only do the committed part
    TRawMem::SetMemBuf
    (
        m_pBuffer
        , tCIDLib::TCard1(0)
        , m_c4AllocatedPages * kCIDLib::c4MemPageSize
    );
    return kCIDLib::True;
}


