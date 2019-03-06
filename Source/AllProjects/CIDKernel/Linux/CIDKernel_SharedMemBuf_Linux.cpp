//
// FILE NAME: CIDKernel_SharedMemBuf_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/16/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
    __phmemiThis(0)
{
}

TMemoryHandle::TMemoryHandle(const TMemoryHandle& hmemToCopy) :
    __phmemiThis(hmemToCopy.__phmemiThis)
{
}

TMemoryHandle::~TMemoryHandle()
{
    __phmemiThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TMemoryHandle& TMemoryHandle::operator=(const TMemoryHandle& hmemToAssign)
{
    if (this == &hmemToAssign)
        return *this;

    __phmemiThis = hmemToAssign.__phmemiThis;

    return *this;
}


tCIDLib::TBoolean
TMemoryHandle::operator==(const TMemoryHandle& hmemToCompare) const
{
    return (__phmemiThis == hmemToCompare.__phmemiThis);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TMemoryHandle::bValid() const
{
    return (__phmemiThis && __phmemiThis->c4RefCount);
}

tCIDLib::TVoid TMemoryHandle::Clear()
{
    __phmemiThis = 0;
}

tCIDLib::TVoid
TMemoryHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TInt4(__phmemiThis)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}




// ---------------------------------------------------------------------------
//  TKrnlSharedMemBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSharedMemBuf::TKrnlSharedMemBuf() :
    __c4Size(0)
    , __eAccess(tCIDLib::EMemAccFlags::ReadOnly)
    , __pBuffer(0)
    , __pszName(0)
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

    if (!TRawStr::bCompareStr(__pszName, ksmbToCompare.__pszName))
        return kCIDLib::False;

    if (__eAccess != ksmbToCompare.__eAccess)
        return kCIDLib::False;

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlSharedMemBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSharedMemBuf::bAlloc(  const  tCIDLib::TCh* const      pszName
                            , const tCIDLib::TCard4         c4Size
                            , const tCIDLib::EMemAccFlags   eAccess
                            , const tCIDLib::ECreateActs eCreateFlags)
{
    tCIDLib::TBoolean bDummy;
    return bAlloc(pszName, c4Size, eAccess, bDummy, eCreateFlags);
}


tCIDLib::TBoolean
TKrnlSharedMemBuf::bAlloc(  const  tCIDLib::TCh* const      pszName
                            , const tCIDLib::TCard4         c4Size
                            , const tCIDLib::EMemAccFlags   eAccess
                            ,       tCIDLib::TBoolean&      bCreated
                            , const tCIDLib::ECreateActs eCreateFlags)
{
    if (__hmemThis.bValid())
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
    __pszName = TRawStr::pszReplicate(pszName);

    // Store the size and access flags
    __c4Size = c4Size;
    __eAccess = eAccess;

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

    if (__eAccess == tCIDLib::EMemAccFlags::ReadOnly)
        iFlags |= S_IRUSR | S_IRGRP | S_IROTH;
    else if (__eAccess == tCIDLib::EMemAccFlags::ReadWrite)
        iFlags |= S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAcc_InvalidAccess);
        return kCIDLib::False;
    }

    key_t key = TRawStr::hshHashStr(pszName, kCIDLib::i4MaxInt);

    tCIDLib::TSInt iTmpId = ::shmget(key, c4Size, iFlags);
    if (iTmpId == -1)
    {
        if (errno == EEXIST
        &&  eCreateFlags == tCIDLib::ECreateActs::OpenOrCreate)
        {
            bOwner = kCIDLib::False;

            iFlags &= ~(IPC_CREAT | IPC_EXCL);

            iTmpId = ::shmget(key, c4Size, iFlags);

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

    iFlags = (__eAccess == tCIDLib::EMemAccFlags::ReadOnly) ? SHM_RDONLY : 0;

    tCIDLib::TVoid* pTmp = ::shmat(iTmpId, 0, iFlags);
    if (pTmp == reinterpret_cast<tCIDLib::TVoid*>(-1))
    {
        TKrnlError::SetLastHostError(errno);
        if (bOwner)
            ::shmctl(iTmpId, IPC_RMID, 0);
        return kCIDLib::False;
    }

    __pBuffer = pTmp;

    __hmemThis.__phmemiThis = new TMemoryHandleImpl;
    __hmemThis.__phmemiThis->c4RefCount = 1;
    __hmemThis.__phmemiThis->iShmId = iTmpId;
    __hmemThis.__phmemiThis->bOwner = bCreated = bOwner;

    __hmemThis.__phmemiThis->prmtxThis = new TKrnlLinux::TRecursiveMutex();
    __hmemThis.__phmemiThis->prmtxThis->iInitialize();

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSharedMemBuf::bDuplicate(const TKrnlSharedMemBuf& ksmbToCopy)
{
    // Can't do this if there is already a buffer in this object
    if (__hmemThis.bValid())
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyAllocated);
        return kCIDLib::False;
    }

    __pBuffer = ksmbToCopy.__pBuffer;
    __eAccess = ksmbToCopy.__eAccess;
    __c4Size = ksmbToCopy.__c4Size;
    if (ksmbToCopy.__pszName)
    {
        delete [] __pszName;
        __pszName = TRawStr::pszReplicate(ksmbToCopy.__pszName);
    }

    __hmemThis = ksmbToCopy.__hmemThis;
    if (__hmemThis.bValid())
    {
        __hmemThis.__phmemiThis->prmtxThis->iLock();
        __hmemThis.__phmemiThis->c4RefCount++;
        __hmemThis.__phmemiThis->prmtxThis->iUnlock();
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSharedMemBuf::bFree()
{
    // If the name got allocated, then free it
    delete [] __pszName;
    __pszName = 0;

    if (__hmemThis.bValid())
    {
        __hmemThis.__phmemiThis->prmtxThis->iLock();

        if (!--__hmemThis.__phmemiThis->c4RefCount)
        {

            if (::shmdt(reinterpret_cast<tCIDLib::TSCh*>(__pBuffer)))
            {
                __hmemThis.__phmemiThis->c4RefCount++;
                __hmemThis.__phmemiThis->prmtxThis->iUnlock();
                TKrnlError::SetLastHostError(errno);
                return kCIDLib::False;
            }

            if (__hmemThis.__phmemiThis->bOwner)
            {
                if (::shmctl(__hmemThis.__phmemiThis->iShmId, IPC_RMID, 0))
                {
                    __hmemThis.__phmemiThis->c4RefCount++;
                    __hmemThis.__phmemiThis->prmtxThis->iUnlock();
                    TKrnlError::SetLastHostError(errno);
                    return kCIDLib::False;
                }
            }

            __hmemThis.__phmemiThis->prmtxThis->iUnlock();
            tCIDLib::TOSErrCode HostErr;
            HostErr = __hmemThis.__phmemiThis->prmtxThis->iDestroy();
            if (HostErr)
            {
                TKrnlError::SetLastHostError(HostErr);
                return kCIDLib::False;
            }

            delete __hmemThis.__phmemiThis->prmtxThis;
            delete __hmemThis.__phmemiThis;
        }
        else
        {
            __hmemThis.__phmemiThis->prmtxThis->iUnlock();
        }
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlSharedMemBuf::bZero()
{
    if (!__pBuffer)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    TRawMem::SetMemBuf(__pBuffer, tCIDLib::TCard1(0), __c4Size);

    return kCIDLib::True;
}
