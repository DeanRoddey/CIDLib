//
// FILE NAME: CIDKernel_File_Win32.Cpp
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
//  This file provides the Win32 specific implementation of the TKrnlFile
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
//  Local static functions
// ---------------------------------------------------------------------------

//
//  Converts a Win32 'info by handle' buffer from the system format to our
//  standard file info format.
//
static tCIDLib::TBoolean
bConvertByHandleBuf(const   BY_HANDLE_FILE_INFORMATION& FileData
                    , const tCIDLib::TCh* const         pszFileName
                    ,       TKrnlFileSys::TRawFileFind& fndbToFill)
{
    // We have to convert these types to local times so this is a temp for that
    FILETIME LocalTime;

    // Convert and store the last access time
    if (!::FileTimeToLocalFileTime(&FileData.ftLastAccessTime, &LocalTime))
        return kCIDLib::False;

    fndbToFill.enctLastAccessTime =
    (
        TRawBits::c8From32(LocalTime.dwLowDateTime, LocalTime.dwHighDateTime)
        - TKrnlTimeStamp::enctBaseOfs
    );

    // Convert and store the last write time
    if (!::FileTimeToLocalFileTime(&FileData.ftLastWriteTime, &LocalTime))
        return kCIDLib::False;
    fndbToFill.enctLastWriteTime =
    (
        TRawBits::c8From32(LocalTime.dwLowDateTime, LocalTime.dwHighDateTime)
        - TKrnlTimeStamp::enctBaseOfs
    );

    // The file size coverts as is
    fndbToFill.c8FileSize = FileData.nFileSizeLow;

    // Convert the file attributes over to our flags
    fndbToFill.eInfoFlags = TKrnlWin32::eConvertAttrs
    (
        FileData.dwFileAttributes
        , pszFileName
    );

    return kCIDLib::True;
}




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
    m_phfliThis->hFile = 0;
}

TFileHandle::TFileHandle(const TFileHandle& hflToCopy) :

    m_phfliThis(nullptr)
{
    m_phfliThis = new TFileHandleImpl;
    m_phfliThis->hFile = hflToCopy.m_phfliThis->hFile;
}

TFileHandle::~TFileHandle()
{
    delete m_phfliThis;
    m_phfliThis = nullptr;
}


// -------------------------------------------------------------------
//  TFileHandle: Public operators
// -------------------------------------------------------------------
TFileHandle& TFileHandle::operator=(const TFileHandle& hflToAssign)
{
    if (this == &hflToAssign)
        return *this;

    m_phfliThis->hFile = hflToAssign.m_phfliThis->hFile;

    return *this;
}


tCIDLib::TBoolean
TFileHandle::operator==(const TFileHandle& hflToCompare) const
{
    return (m_phfliThis->hFile == hflToCompare.m_phfliThis->hFile);
}

tCIDLib::TBoolean TFileHandle::operator!=(const TFileHandle& hflToCompare) const
{
    return !operator==(hflToCompare);
}


// -------------------------------------------------------------------
//  TFileHandle: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TFileHandle::bIsValid() const
{
    return (m_phfliThis->hFile != 0);
}

tCIDLib::TVoid TFileHandle::Clear()
{
    m_phfliThis->hFile = 0;
}

tCIDLib::TVoid
TFileHandle::FormatToStr(       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phfliThis->hFile)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}

const TFileHandleImpl& TFileHandle::hfliThis() const
{
    return *m_phfliThis;
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
    HANDLE hTmp = INVALID_HANDLE_VALUE;

    if (eStdFile == tCIDLib::EStdFiles::StdIn)
        hTmp = ::GetStdHandle(STD_INPUT_HANDLE);
    else if (eStdFile == tCIDLib::EStdFiles::StdOut)
        hTmp = ::GetStdHandle(STD_OUTPUT_HANDLE);
    else if (eStdFile == tCIDLib::EStdFiles::StdErr)
        hTmp = ::GetStdHandle(STD_ERROR_HANDLE);

    if (hTmp == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // It worked so fill in the passed handle
    hflToFill.m_phfliThis->hFile = hTmp;
    return kCIDLib::True;;
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
    // If they provided a name, then replicate it to our member
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
    m_pszName = nullptr;
}


// ---------------------------------------------------------------------------
//  TKrnlFile: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlFile::bClose()
{
    // If the handle shows its supposed to be open, try to close it
    if (m_hflThis.m_phfliThis->hFile)
    {
        if (!::CloseHandle(m_hflThis.m_phfliThis->hFile))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
        m_hflThis.m_phfliThis->hFile = 0;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bFlush()
{
    if (!::FlushFileBuffers(m_hflThis.m_phfliThis->hFile))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bIsValid() const
{
    return  (m_hflThis.m_phfliThis->hFile != 0);
}


tCIDLib::TBoolean
TKrnlFile::bOffsetFilePointer(  const   tCIDLib::TInt8&     i8OffsetBy
                                ,       tCIDLib::TCard8&    c8ToFill)
{
    LARGE_INTEGER liOfs;
    liOfs.QuadPart = i8OffsetBy;
    LARGE_INTEGER liNew;

    if (!::SetFilePointerEx(m_hflThis.m_phfliThis->hFile, liOfs, &liNew, SEEK_CUR))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    c8ToFill = liNew.QuadPart;
    return kCIDLib::True;;
}


tCIDLib::TBoolean TKrnlFile::bOpen( const  tCIDLib::EAccessModes    eAccess
                                    , const tCIDLib::ECreateActs eAction
                                    , const tCIDLib::EFilePerms     ePerms
                                    , const tCIDLib::EFileFlags     eFlags)
{
    // Sanity check, just in case we are already open
    if (m_hflThis.m_phfliThis->hFile)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (!m_pszName)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    //
    //  Map our access flags to the system specific flags. The others are
    //  already mapped correctly.
    //
    tCIDLib::TCard4 c4Access = 0;
    if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::Read))
        c4Access |= GENERIC_READ;

    if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::Write))
        c4Access |= GENERIC_WRITE;

    tCIDLib::TCard4 c4Share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::DenyRead))
        c4Share &= ~FILE_SHARE_READ;

    if (tCIDLib::bAllBitsOn(eAccess, tCIDLib::EAccessModes::DenyWrite))
        c4Share &= ~FILE_SHARE_WRITE;

    // Convert the flags to system specific flags
    tCIDLib::TCard4 c4Flags = 0;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EFileFlags::RandomAccess))
        c4Flags |= FILE_FLAG_RANDOM_ACCESS;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EFileFlags::SequentialScan))
        c4Flags |= FILE_FLAG_SEQUENTIAL_SCAN;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EFileFlags::WriteThrough))
        c4Flags |= FILE_FLAG_WRITE_THROUGH;

    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EFileFlags::Overlapped))
        c4Flags |= FILE_FLAG_OVERLAPPED;

    //
    //  <TBD> We really don't use the permissions flag at this time. Later,
    //  under NT, it could be used to invoke the security system. Right now
    //  all we do is to at least set the read only file attribute if the
    //  permissions indicate that the file should be readable by all.
    //
    tCIDLib::TCard4 c4Attrs = FILE_ATTRIBUTE_NORMAL;
    if (ePerms == tCIDLib::EFilePerms::AllRead)
        c4Attrs = FILE_ATTRIBUTE_READONLY;

    //
    //  Ok, lets try to do this thing using all the stuff that we just
    //  set up.
    //
    HANDLE hTmp = ::CreateFile
    (
        m_pszName
        , c4Access
        , c4Share
        , 0
        , TKrnlWin32::c4XlatCreateAction(eAction)
        , c4Flags | c4Attrs
        , 0
    );

    if (hTmp == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // We survived, so store the handle
    m_hflThis.m_phfliThis->hFile = hTmp;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bOpen(const tCIDLib::EStdFiles eStdFile)
{
    // Sanity check, just in case we are already open
    if (m_hflThis.m_phfliThis->hFile)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    if (eStdFile == tCIDLib::EStdFiles::StdIn)
    {
        m_pszName = TRawStr::pszReplicate(L"STDIN");
        m_hflThis.m_phfliThis->hFile = ::GetStdHandle(STD_INPUT_HANDLE);
    }
    else if (eStdFile == tCIDLib::EStdFiles::StdOut)
    {
        m_pszName = TRawStr::pszReplicate(L"STDOUT");
        m_hflThis.m_phfliThis->hFile = ::GetStdHandle(STD_OUTPUT_HANDLE);
    }
    else if (eStdFile == tCIDLib::EStdFiles::StdErr)
    {
        m_pszName = TRawStr::pszReplicate(L"STDERR");
        m_hflThis.m_phfliThis->hFile = ::GetStdHandle(STD_ERROR_HANDLE);
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bQueryCurSize(tCIDLib::TCard8& c8ToFill) const
{
    LARGE_INTEGER liSize;
    if (!::GetFileSizeEx(m_hflThis.m_phfliThis->hFile, &liSize))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Fill in the caller's parm and return success
    c8ToFill = liSize.QuadPart;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bQueryFileInfo(TKrnlFileSys::TRawFileFind& fndbToFill)
{
    BY_HANDLE_FILE_INFORMATION  FileData;
    if (!::GetFileInformationByHandle(m_hflThis.m_phfliThis->hFile, &FileData))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert the system structure to our structure
    if (!bConvertByHandleBuf(FileData, m_pszName, fndbToFill))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bQueryFilePtr(tCIDLib::TCard8& c8ToFill) const
{
    LARGE_INTEGER liOfs;
    liOfs.QuadPart = 0;
    LARGE_INTEGER liNew;

    if (!::SetFilePointerEx(m_hflThis.m_phfliThis->hFile, liOfs, &liNew, SEEK_CUR))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Fill in the caller's buffer
    c8ToFill = liNew.QuadPart;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bQueryFileTimes( tCIDLib::TEncodedTime&      enctLastAccess
                            , tCIDLib::TEncodedTime&    enctLastWrite)
{
    FILETIME LastAccess;
    FILETIME LastWrite;
    FILETIME LocalTime;

    // Query the file times in UTC format
    if (!::GetFileTime(m_hflThis.m_phfliThis->hFile, 0, &LastAccess, &LastWrite))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  And now convert them to local time and encode them into the caller's
    //  parameters.
    //
    if (!::FileTimeToLocalFileTime(&LastAccess, &LocalTime))
        return kCIDLib::False;
    enctLastAccess = TRawBits::c8From32(LocalTime.dwLowDateTime, LocalTime.dwHighDateTime)
                     - TKrnlTimeStamp::enctBaseOfs;

    if (!::FileTimeToLocalFileTime(&LastWrite, &LocalTime))
        return kCIDLib::False;

    enctLastWrite = TRawBits::c8From32(LocalTime.dwLowDateTime, LocalTime.dwHighDateTime)
                    - TKrnlTimeStamp::enctBaseOfs;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bQueryLastAccessTime(tCIDLib::TEncodedTime& enctToFill) const
{
    FILETIME LastAccess;

    // Query the last access UTC time
    if (!::GetFileTime(m_hflThis.m_phfliThis->hFile, 0, &LastAccess, 0))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert to a local file time before encoding
    FILETIME LocalTime;
    if (!::FileTimeToLocalFileTime(&LastAccess, &LocalTime))
        return kCIDLib::False;

    enctToFill = TRawBits::c8From32(LocalTime.dwLowDateTime, LocalTime.dwHighDateTime)
                 - TKrnlTimeStamp::enctBaseOfs;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bQueryLastWriteTime(tCIDLib::TEncodedTime& enctToFill) const
{
    FILETIME LastWrite;

    // Query the last write UTC time
    if (!::GetFileTime(m_hflThis.m_phfliThis->hFile, 0, 0, &LastWrite))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Convert to a local file time before encoding
    FILETIME LocalTime;
    if (!::FileTimeToLocalFileTime(&LastWrite, &LocalTime))
        return kCIDLib::False;

    enctToFill = TRawBits::c8From32(LocalTime.dwLowDateTime, LocalTime.dwHighDateTime)
                - TKrnlTimeStamp::enctBaseOfs;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bReadBuffer(         tCIDLib::TVoid* const   pBuffer
                        , const tCIDLib::TCard4         c4ToRead
                        ,       tCIDLib::TCard4&        c4BytesRead)
{
    if (!::ReadFile
    (
        m_hflThis.m_phfliThis->hFile
        , pBuffer
        , c4ToRead
        , &c4BytesRead
        , 0))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bReadBufferTO(       tCIDLib::TVoid* const   pBuffer
                        , const tCIDLib::TCard4         c4ToRead
                        ,       tCIDLib::TCard4&        c4BytesRead
                        , const tCIDLib::TCard4         c4MaxWait
                        ,       TKrnlEvent&             kevToPost)
{
    // Set up an overlapped structure
    OVERLAPPED OverData = {0};
    OverData.hEvent = *tCIDLib::pReCastPtr<HANDLE>(kevToPost.pHandle());

    //
    //  Do the read. If there's data already there, it'll come back just
    //  like a normal read. If not, and the error indicates it's pending,
    //  then we block for up to the indicated max wait on the event. Else,
    //  it's just a plain error.
    //
    if (!::ReadFile
    (
        m_hflThis.m_phfliThis->hFile
        , pBuffer
        , c4ToRead
        , &c4BytesRead
        , &OverData))
    {
        // If not pending, then it's just a plain jane error
        const DWORD dwErr = ::GetLastError();

        if (dwErr != ERROR_IO_PENDING)
        {
            TKrnlError::SetLastHostError(dwErr);
            return kCIDLib::False;
        }

        //
        //  Let's wait for it to complete, up to the indicated time. If it
        //  doesn't happen within that time, then cancel it and return a
        //  timeout error.
        //
        if (!kevToPost.bWaitFor(c4MaxWait))
        {
            ::CancelIo(m_hflThis.m_phfliThis->hFile);
            TKrnlError::SetLastError(kKrnlErrs::errcGen_Timeout);
            return kCIDLib::False;
        }
    }

    //
    //  Ok, at this point, one way or the other we got a read,
    //
    if (!::GetOverlappedResult(m_hflThis.m_phfliThis->hFile, &OverData, &c4BytesRead, 0))
    {
        TKrnlError::SetLastError(kKrnlErrs::errcGen_Timeout);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bSetFilePointer(const tCIDLib::TCard8& c8ToSet)
{
    //
    //  If the position to set is the maximum, that means do a seek to the end
    //  of the file, so flip around the seek position and seek type to cause
    //  that.
    //
    LARGE_INTEGER liActual;
    liActual.QuadPart = c8ToSet;
    tCIDLib::TCard4 c4SeekType = SEEK_SET;
    if (c8ToSet == kCIDLib::c8MaxCard)
    {
        liActual.QuadPart = 0;
        c4SeekType = SEEK_END;
    }

    if (!::SetFilePointerEx(m_hflThis.m_phfliThis->hFile, liActual, 0, c4SeekType))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;;
}


tCIDLib::TBoolean
TKrnlFile::bSetFileTimes(const  tCIDLib::TEncodedTime&  enctLastAccess
                        , const tCIDLib::TEncodedTime&  enctLastWrite)
{
    // Assume that any of them can be "dont cares"
    FILETIME*       pLastAccess = 0;
    FILETIME*       pLastWrite = 0;
    tCIDLib::TCard8 c8Tmp;

    //
    //  For each time that was provided, decode it to a file time, then
    //  convert that to a UTC file time which is needed for the system
    //  call.
    //
    FILETIME    TmpTime;
    if (enctLastAccess >= 0)
    {
        pLastAccess = new FILETIME;
        c8Tmp = enctLastAccess + TKrnlTimeStamp::enctBaseOfs;
        TmpTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
        TmpTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);
        LocalFileTimeToFileTime(&TmpTime, pLastAccess);
    }

    if (enctLastWrite >= 0)
    {
        pLastWrite = new FILETIME;
        c8Tmp = enctLastWrite + TKrnlTimeStamp::enctBaseOfs;
        TmpTime.dwLowDateTime = TRawBits::c4Low32From64(c8Tmp);
        TmpTime.dwHighDateTime = TRawBits::c4High32From64(c8Tmp);
        LocalFileTimeToFileTime(&TmpTime, pLastWrite);
    }

    // And now do the system call
    if (!::SetFileTime(m_hflThis.m_phfliThis->hFile, 0, pLastAccess, pLastWrite))
    {
        delete pLastAccess;
        delete pLastWrite;

        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Clean up those that got allocated
    delete pLastAccess;
    delete pLastWrite;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFile::bSetName(const tCIDLib::TCh* const pszNewName)
{
    if (m_hflThis.m_phfliThis->hFile)
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
    // Move tot he indicated position
    LARGE_INTEGER liAt;
    liAt.QuadPart = c8Position;
    if (!::SetFilePointerEx(m_hflThis.m_phfliThis->hFile, liAt, 0, SEEK_SET))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // And make the current pos the end of the file
    if (!::SetEndOfFile(m_hflThis.m_phfliThis->hFile))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TKrnlFile::bTruncateAt()
{
    if (!::SetEndOfFile(m_hflThis.m_phfliThis->hFile))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bWriteBuffer(const   tCIDLib::TVoid* const   pBuffer
                        , const tCIDLib::TCard4         c4ToWrite
                        ,       tCIDLib::TCard4&        c4BytesWritten)
{
    if (!::WriteFile
    (
        m_hflThis.m_phfliThis->hFile
        , pBuffer
        , c4ToWrite
        , &c4BytesWritten
        , 0))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFile::bWriteBufferTO(  const   tCIDLib::TVoid* const   pBuffer
                            , const tCIDLib::TCard4         c4ToWrite
                            ,       tCIDLib::TCard4&        c4BytesWritten
                            , const tCIDLib::TCard4         c4MaxWait
                            ,       TKrnlEvent&             kevToPost)
{
    // Set up an overlapped structure
    OVERLAPPED OverData = {0};
    OverData.hEvent = *tCIDLib::pReCastPtr<HANDLE>(kevToPost.pHandle());

    //
    //  Do the write. If there's data already there, it'll come back just
    //  like a normal read. If not, and the error indicates it's pending,
    //  then we block for up to the indicated max wait on the event. Else,
    //  it's just a plain error.
    //
    if (!::WriteFile
    (
        m_hflThis.m_phfliThis->hFile
        , pBuffer
        , c4ToWrite
        , 0
        , &OverData))
    {
        // If not pending, then it's just a plain jane error
        const DWORD dwErr = ::GetLastError();

        if (dwErr != ERROR_IO_PENDING)
        {
            TKrnlError::SetLastHostError(dwErr);
            return kCIDLib::False;
        }

        //
        //  Let's wait for it to complete, up to the indicated time. If it
        //  doesn't happen within that time, then cancel it and return a
        //  timeout error.
        //
        if (!kevToPost.bWaitFor(c4MaxWait))
        {
            ::CancelIo(m_hflThis.m_phfliThis->hFile);
            TKrnlError::SetLastError(kKrnlErrs::errcGen_Timeout);
            return kCIDLib::False;
        }
    }

    // Get the amount of data that was transferred
    ::GetOverlappedResult
    (
        m_hflThis.m_phfliThis->hFile
        , &OverData
        , &c4BytesWritten
        , 0
    );

    //
    //  Ok, at this point, one way or the other we got a write, and it's
    //  in the caller's buffers, so return true.
    //
    return kCIDLib::True;
}


