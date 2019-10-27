//
// FILE NAME: CIDKernel_File_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/09/1998
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
//  This file provides the Linux specific implementation of the TKrnlFile
//  class.
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
//   CLASS: TFileHandle
//  PREFIX: hfl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFileHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TFileHandle::TFileHandle() :

    m_phfliThis(nullptr)
{
    m_phfliThis = new TFileHandleImpl;
    m_phfliThis->iFd = -1;
}

TFileHandle::TFileHandle(const TFileHandle& hflToCopy) :

    m_phfliThis(nullptr)
{
    m_phfliThis = new TFileHandleImpl;
    m_phfliThis->iFd = hflToCopy.m_phfliThis->iFd;
}

TFileHandle::~TFileHandle()
{
    delete m_phfliThis;
    m_phfliThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TFileHandle& TFileHandle::operator=(const TFileHandle& hflToAssign)
{
    if (this != &hflToAssign)
        m_phfliThis->iFd = hflToAssign.m_phfliThis->iFd;
    return *this;
}


tCIDLib::TBoolean
TFileHandle::operator==(const TFileHandle& hflToCompare) const
{
    return (m_phfliThis->iFd == hflToCompare.m_phfliThis->iFd);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TFileHandle::bIsValid() const
{
    return (m_phfliThis->iFd != -1);
}

tCIDLib::TVoid TFileHandle::Clear()
{
    m_phfliThis->iFd = -1;
}

tCIDLib::TVoid
TFileHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TInt4(m_phfliThis->iFd)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlFile
//  PREFIX: kfl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlFile: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlFile::bOpenStdFile(const   tCIDLib::EStdFiles  eStdFile
                        ,       TFileHandle&        hflToFill)
{
    switch (eStdFile)
    {
    case tCIDLib::EStdFiles::StdOut:
        hflToFill.m_phfliThis->iFd = 1;
        break;
    case tCIDLib::EStdFiles::StdIn:
        hflToFill.m_phfliThis->iFd = 0;
        break;
    case tCIDLib::EStdFiles::StdErr:
        hflToFill.m_phfliThis->iFd = 2;
        break;
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlFile: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlFile::TKrnlFile() :

    m_pszName(nullptr)
{
}

TKrnlFile::TKrnlFile(const tCIDLib::TCh* const pszName) :

    m_pszName(nullptr)
{
    if (pszName)
        m_pszName = TRawStr::pszReplicate(pszName);
}

TKrnlFile::~TKrnlFile()
{
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

    // And now delete our name member
    delete [] m_pszName;
}


// ---------------------------------------------------------------------------
//  TKrnlFile: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlFile::bClose()
{
    // If the handle shows its supposed to be open, try to close it
    if (m_hflThis.m_phfliThis->iFd > 2)
    {
        if (::close(m_hflThis.m_phfliThis->iFd))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
        m_hflThis.m_phfliThis->iFd = -1;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bQueryCurSize(tCIDLib::TCard8& c8ToFill) const
{
    struct stat StatBuf;
    if (::fstat(m_hflThis.m_phfliThis->iFd, &StatBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    c8ToFill = StatBuf.st_size;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bQueryFilePtr(tCIDLib::TCard8& c8ToFill) const
{
    off_t offset = ::lseek(m_hflThis.m_phfliThis->iFd, 0, SEEK_CUR);
    if (offset == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    c8ToFill = offset;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bQueryLastAccessTime(tCIDLib::TEncodedTime& enctToFill) const
{
    struct stat StatBuf;
    if (::fstat(m_hflThis.m_phfliThis->iFd, &StatBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_atime, enctToFill);
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bQueryLastWriteTime(tCIDLib::TEncodedTime& enctToFill) const
{
    struct stat StatBuf;
    if (::fstat(m_hflThis.m_phfliThis->iFd, &StatBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_mtime, enctToFill);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bOffsetFilePointer(  const   tCIDLib::TInt8&     i8OffsetBy
                                ,       tCIDLib::TCard8&    c8ToFill)
{
    off_t offThis = ::lseek(m_hflThis.m_phfliThis->iFd, i8OffsetBy, SEEK_CUR);

    if (offThis == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c8ToFill = offThis;

    return kCIDLib::True;;
}


tCIDLib::TBoolean
TKrnlFile::bOpen(const  tCIDLib::EAccessModes    eAccess
                 , const tCIDLib::ECreateActs eAction
                 , const tCIDLib::EFilePerms     ePerms
                 , const tCIDLib::EFileFlags)
{
    // Sanity check, just in case we are already open
    if (m_hflThis.m_phfliThis->iFd != -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    tCIDLib::TSInt iFlags = 0;

    if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::ReadWrite)
    ||  tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::DenyRead))
    {
        iFlags = O_RDWR;
    }
    else if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::Read))
    {
        iFlags = O_RDONLY;
    }
    else if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::Write))
    {
        iFlags = O_WRONLY;
    }
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAcc_InvalidAccess);
        return kCIDLib::False;
    }

    switch (eAction)
    {
        case tCIDLib::ECreateActs::CreateIfNew:
            iFlags |= O_CREAT | O_EXCL;
            break;

        case tCIDLib::ECreateActs::CreateAlways:
            iFlags |= O_CREAT | O_TRUNC;
            break;

        case tCIDLib::ECreateActs::TruncateExisting:
            iFlags |= O_TRUNC;
            break;

        case tCIDLib::ECreateActs::OpenOrCreate:
            iFlags |= O_CREAT;
            break;

        default :
            break;
    }

    mode_t mode = 0;
    if (iFlags & O_CREAT)
    {
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::OwnerRead))
            mode |= S_IRUSR;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::OwnerWrite))
            mode |= S_IWUSR;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::OwnerExecute))
            mode |= S_IXUSR;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::GroupRead))
            mode |= S_IRGRP;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::GroupWrite))
            mode |= S_IWGRP;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::GroupExecute))
            mode |= S_IXGRP;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::WorldRead))
            mode |= S_IROTH;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::WorldWrite))
            mode |= S_IWOTH;
        if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::WorldExecute))
            mode |= S_IXOTH;
    }

    tCIDLib::TSCh* pszLocName = TRawStr::pszConvert(m_pszName);
    TArrayJanitor<tCIDLib::TSCh> janName(pszLocName);

    tCIDLib::TSInt iFdTmp = ::open(pszLocName, iFlags, mode);

    if (iFdTmp == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    //
    //  Now query the lock status. Locks are advisory, which means
    //  we have to do all the work. To make it act like Windows we
    //  only have to check lock status at open() time.
    //
    struct flock FlockBuf;
    ::memset(&FlockBuf, 0, sizeof(FlockBuf));
    if (::fcntl(iFdTmp, F_GETLK, &FlockBuf) == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if ((FlockBuf.l_type == F_WRLCK)
    ||  (FlockBuf.l_type == F_RDLCK && tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::Write)))
    {
        ::close(iFdTmp);
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAcc_LockViolation);
        return kCIDLib::False;
    }

    // Now see if we need to set a lock. For our purposes DenyRead implies
    // DenyWrite.
    if (tCIDLib::bAllBitsOn(eAccess, (tCIDLib::EAccessModes::DenyRead | tCIDLib::EAccessModes::DenyWrite)))
    {
        ::memset(&FlockBuf, 0, sizeof(FlockBuf));
        if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::DenyRead))
            FlockBuf.l_type = F_WRLCK;
        else
            FlockBuf.l_type = F_RDLCK;

        if (::fcntl(iFdTmp, F_SETLK, &FlockBuf) == -1)
        {
            ::close(iFdTmp);
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }

    m_hflThis.m_phfliThis->iFd = iFdTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bOpen(const tCIDLib::EStdFiles eStdFile)
{
    // Sanity check, just in case we are already open
    if (m_hflThis.m_phfliThis->iFd != -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    switch (eStdFile)
    {
        case tCIDLib::EStdFiles::StdOut:
            m_hflThis.m_phfliThis->iFd = 1;
            break;

        case tCIDLib::EStdFiles::StdIn:
            m_hflThis.m_phfliThis->iFd = 0;
            break;

        case tCIDLib::EStdFiles::StdErr:
            m_hflThis.m_phfliThis->iFd = 2;
            break;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bQueryFileInfo(TKrnlFileSys::TRawFileFind& fndbToFill)
{
    struct stat StatBuf;
    if (::fstat(m_hflThis.m_phfliThis->iFd, &StatBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_atime, fndbToFill.enctLastAccessTime);
    TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_mtime, fndbToFill.enctLastWriteTime);

    fndbToFill.c8FileSize = StatBuf.st_size;

    TKrnlLinux::StatBufToInfoFlags(StatBuf, fndbToFill.eInfoFlags);

    TRawStr::CopyStr(fndbToFill.szName, m_pszName, kCIDLib::c4MaxPathLen);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bQueryFileTimes( tCIDLib::TEncodedTime&    enctLastAccess
                            , tCIDLib::TEncodedTime&    enctLastWrite)
{
    struct stat StatBuf;
    if (::fstat(m_hflThis.m_phfliThis->iFd, &StatBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_atime, enctLastAccess);
    TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_mtime, enctLastWrite);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bReadBuffer(         tCIDLib::TVoid* const   pBuffer
                        , const tCIDLib::TCard4         c4ToRead
                        ,       tCIDLib::TCard4&        c4BytesRead)
{
    ssize_t count = ::read(m_hflThis.m_phfliThis->iFd, pBuffer, c4ToRead);

    if (count == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4BytesRead = count;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bSetFilePointer(const tCIDLib::TCard8& c8ToSet)
{
    //
    //  If the position to set is the maximum, that means do a seek to the end
    //  of the file, so flip around the seek position and seek type to cause
    //  that.
    //
    off_t offActualPos = c8ToSet;
    tCIDLib::TSInt iSeekType = SEEK_SET;

    if (c8ToSet == kCIDLib::c4MaxCard)
    {
        offActualPos = 0;
        iSeekType = SEEK_END;
    }

    off_t offReturn = ::lseek(m_hflThis.m_phfliThis->iFd, offActualPos, iSeekType);
    if (offReturn == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bSetFileTimes(const tCIDLib::TEncodedTime& enctLastAccess
                         , const tCIDLib::TEncodedTime& enctLastWrite)
{
    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    struct utimbuf UTimeBuf;

    TKrnlLinux::CIDFileTimeToLinuxFileTime(enctLastAccess, UTimeBuf.actime);
    TKrnlLinux::CIDFileTimeToLinuxFileTime(enctLastWrite, UTimeBuf.modtime);

    tCIDLib::TSCh* pszLocName = TRawStr::pszConvert(m_pszName);
    TArrayJanitor<tCIDLib::TSCh> janName(pszLocName);

    if (::utime(pszLocName, &UTimeBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bSetName(const tCIDLib::TCh* const pszNewName)
{
    if (m_hflThis.m_phfliThis->iFd != -1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    // Delete the current name and replicate the new one
    delete [] m_pszName;
    m_pszName = 0;
    m_pszName = TRawStr::pszReplicate(pszNewName);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bTruncateAt(const tCIDLib::TCard8& c8Position)
{
    tCIDLib::TCard8 c8Cur = ::lseek(m_hflThis.m_phfliThis->iFd, 0, SEEK_CUR);
    if (c8Cur == tCIDLib::TCard8(-1))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (::ftruncate(m_hflThis.m_phfliThis->iFd, c8Position))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (::lseek(m_hflThis.m_phfliThis->iFd, tCIDLib::MinVal(c8Cur, c8Position), SEEK_SET) == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlFile::bTruncateAt()
{
    tCIDLib::TCard8 c8This;
    if (bQueryFilePtr(c8This) && bTruncateAt(c8This))
        return kCIDLib::True;

    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlFile::bIsValid() const
{
    return  (m_hflThis.m_phfliThis->iFd != -1);
}


tCIDLib::TBoolean
TKrnlFile::bWriteBuffer(const   tCIDLib::TVoid* const   pBuffer
                        , const tCIDLib::TCard4         c4ToWrite
                        ,       tCIDLib::TCard4&        c4BytesWritten)
{
    ssize_t count = ::write(m_hflThis.m_phfliThis->iFd, pBuffer, c4ToWrite);

    if (count == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    c4BytesWritten = count;

    return kCIDLib::True;
}
