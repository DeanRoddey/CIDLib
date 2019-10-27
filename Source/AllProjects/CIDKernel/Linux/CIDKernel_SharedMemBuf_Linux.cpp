//
// FILE NAME: CIDKernel_SharedMemBuf_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/16/1998
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
//  This file provides the Linux specific implementation of the class
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
#include    "CIDKernel_InternalHelpers_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TMemoryHandle
//  PREFIX: hev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMemoryHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TMemoryHandle::TMemoryHandle() :

    m_phmemiThis(nullptr)
{
}

TMemoryHandle::TMemoryHandle(const TMemoryHandle& hmemToCopy) :

    m_phmemiThis(hmemToCopy.m_phmemiThis)
{
}

TMemoryHandle::~TMemoryHandle()
{
    m_phmemiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TMemoryHandle& TMemoryHandle::operator=(const TMemoryHandle& hmemToAssign)
{
    if (this == &hmemToAssign)
        return *this;

    m_phmemiThis = hmemToAssign.m_phmemiThis;
    return *this;
}


tCIDLib::TBoolean
TMemoryHandle::operator==(const TMemoryHandle& hmemToCompare) const
{
    return (m_phmemiThis == hmemToCompare.m_phmemiThis);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TMemoryHandle::bIsValid() const
{
    return (m_phmemiThis && m_phmemiThis->c4RefCount);
}


tCIDLib::TVoid TMemoryHandle::Clear()
{
    m_phmemiThis = nullptr;
}


tCIDLib::TVoid
TMemoryHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TInt4(m_phmemiThis)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}




// ---------------------------------------------------------------------------
//  TKrnlSharedMemBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSharedMemBuf::TKrnlSharedMemBuf() :

    m_c4AllocatedPages(0)
    , m_c4MaxSize(0)
    , m_eAccess(tCIDLib::EMemAccFlags::ReadOnly)
    , m_pBuffer(nullptr)
    , m_pszName(nullptr)
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
TKrnlSharedMemBuf::operator==(const TKrnlSharedMemBuf& ksmbToCompare) const
{
    //
    //  We only compare the name and access at this level. The public CIDLib
    //  class that uses us as its implementation, TSharedMemBuf, will handle
    //  the actual memory comparison.
    //

    if (!TRawStr::bCompareStr(m_pszName, ksmbToCompare.m_pszName))
        return kCIDLib::False;

    if (m_eAccess != ksmbToCompare.m_eAccess)
        return kCIDLib::False;

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
                            , const tCIDLib::ECreateActs    eCreateFlags)
{
    if (m_hmemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyAllocated);
        return kCIDLib::False;
    }

    if (!pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    if (eCreateFlags == tCIDLib::ECreateActs::TruncateExisting)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    // Replicate the name of the buffer
    m_pszName = TRawStr::pszReplicate(pszName);

    // Store the size and allocate type stuff
    m_c4MaxSize = c4MaxSize;
    m_eAccess = eAccess;
    m_eAllocType = eAllocType;

    // Calculate the maximum pages
    m_c4MaxPages = (c4MaxSize / kCIDLib::c4MemPageSize);
    if (c4MaxSize % kCIDLib::c4MemPageSize)
        m_c4MaxPages++;

    // <TBD> For now, just say all allocated. We need to come back to this
    m_c4AllocatedPages = m_c4MaxPages;

    // Translate the creation flags
    tCIDLib::TSInt iFlags;
    tCIDLib::TBoolean bOwner;
    if (eCreateFlags == tCIDLib::ECreateActs::CreateAlways
    ||  eCreateFlags == tCIDLib::ECreateActs::OpenOrCreate
    ||  eCreateFlags == tCIDLib::ECreateActs::CreateIfNew)
    {
        iFlags = IPC_CREAT | IPC_EXCL;
        bOwner = kCIDLib::True;
    }
    else
    {
        iFlags = 0;
        bOwner = kCIDLib::False;
    }

    // Translate the access flags
    if (m_eAccess == tCIDLib::EMemAccFlags::ReadOnly)
        iFlags |= S_IRUSR | S_IRGRP | S_IROTH;
    else if (m_eAccess == tCIDLib::EMemAccFlags::ReadWrite)
        iFlags |= S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAcc_InvalidAccess);
        return kCIDLib::False;
    }

    key_t key = TRawStr::hshHashStr(pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmpId = ::shmget(key, c4MaxSize, iFlags);
    if (iTmpId == -1)
    {
        if (errno == EEXIST
        &&  eCreateFlags == tCIDLib::ECreateActs::OpenOrCreate)
        {
            bOwner = kCIDLib::False;

            iFlags &= ~(IPC_CREAT | IPC_EXCL);

            iTmpId = ::shmget(key, c4MaxSize, iFlags);

            if (iTmpId == -1)
            {
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }
        }
        else
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

    }

    iFlags = (m_eAccess == tCIDLib::EMemAccFlags::ReadOnly) ? SHM_RDONLY : 0;

    tCIDLib::TVoid* pTmp = ::shmat(iTmpId, 0, iFlags);
    if (pTmp == reinterpret_cast<tCIDLib::TVoid*>(-1))
    {
        TKrnlError::SetLastHostError(errno);
        if (bOwner)
            ::shmctl(iTmpId, IPC_RMID, 0);
        return kCIDLib::False;
    }

    m_pBuffer = pTmp;

    m_hmemThis.m_phmemiThis = new TMemoryHandleImpl;
    m_hmemThis.m_phmemiThis->c4RefCount = 1;
    m_hmemThis.m_phmemiThis->iShmId = iTmpId;
    m_hmemThis.m_phmemiThis->bOwner = bCreated = bOwner;

    m_hmemThis.m_phmemiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    m_hmemThis.m_phmemiThis->prmtxThis->iInitialize();

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSharedMemBuf::bDuplicate(const TKrnlSharedMemBuf& ksmbSrc)
{
    // Can't do this if there is already a buffer in this object
    if (m_hmemThis.bIsValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyAllocated);
        return kCIDLib::False;
    }

    m_pBuffer = ksmbSrc.m_pBuffer;
    m_eAccess = ksmbSrc.m_eAccess;
    m_c4AllocatedPages = ksmbSrc.m_c4AllocatedPages;
    m_c4MaxPages = ksmbSrc.m_c4MaxPages;
    m_c4MaxSize = ksmbSrc.m_c4MaxSize;
    if (ksmbSrc.m_pszName)
    {
        delete [] m_pszName;
        m_pszName = TRawStr::pszReplicate(ksmbSrc.m_pszName);
    }

    m_hmemThis = ksmbSrc.m_hmemThis;
    if (m_hmemThis.bIsValid())
    {
        m_hmemThis.m_phmemiThis->prmtxThis->iLock();
        m_hmemThis.m_phmemiThis->c4RefCount++;
        m_hmemThis.m_phmemiThis->prmtxThis->iUnlock();
    }

    return kCIDLib::True;
}


// Fow now a no-op, since we are always fully committed
tCIDLib::TBoolean
TKrnlSharedMemBuf::bCommitToSize(const tCIDLib::TCard4 c4NewSize)
{
    // If this is beyond the max size, its an error
    if (c4NewSize > m_c4MaxSize)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
        return kCIDLib::False;
    }


    return kCIDLib::True;
}



tCIDLib::TBoolean TKrnlSharedMemBuf::bFree()
{
    // If the name got allocated, then free it
    delete [] m_pszName;
    m_pszName = nullptr;

    if (m_hmemThis.bIsValid())
    {
        m_hmemThis.m_phmemiThis->prmtxThis->iLock();

        // Copy our buffer pointer and then zero it, so we ar esure it's cleraed
        tCIDLib::TVoid* pBuf = m_pBuffer;
        m_pBuffer = nullptr;

        if (!--m_hmemThis.m_phmemiThis->c4RefCount)
        {

            if (::shmdt(reinterpret_cast<tCIDLib::TSCh*>(pBuf)))
            {
                m_hmemThis.m_phmemiThis->c4RefCount++;
                m_hmemThis.m_phmemiThis->prmtxThis->iUnlock();
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }

            if (m_hmemThis.m_phmemiThis->bOwner)
            {
                if (::shmctl(m_hmemThis.m_phmemiThis->iShmId, IPC_RMID, 0))
                {
                    m_hmemThis.m_phmemiThis->c4RefCount++;
                    m_hmemThis.m_phmemiThis->prmtxThis->iUnlock();
                    TKrnlError::SetLastHostError(errno);
                    return kCIDLib::False;
                }
            }

            m_hmemThis.m_phmemiThis->prmtxThis->iUnlock();
            tCIDLib::TOSErrCode HostErr;
            HostErr = m_hmemThis.m_phmemiThis->prmtxThis->iDestroy();
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete m_hmemThis.m_phmemiThis->prmtxThis;
            delete m_hmemThis.m_phmemiThis;

        }
        else
        {
            m_hmemThis.m_phmemiThis->prmtxThis->iUnlock();
        }
    }

    return kCIDLib::True;
}


// <TBD> For now a no-op for us
tCIDLib::TBoolean TKrnlSharedMemBuf::bSyncView()
{
    return kCIDLib::True;
}



tCIDLib::TBoolean TKrnlSharedMemBuf::bZero()
{
    if (!m_pBuffer)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Zero out the allocated pages
    TRawMem::SetMemBuf(m_pBuffer, tCIDLib::TCard1(0), m_c4AllocatedPages * kCIDLib::c4MemPageSize);
    return kCIDLib::True;
}
