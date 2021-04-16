//
// FILE NAME: CIDLib_FileBase.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/20/1993
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
//  This file implements the TFileBase class, which is the lowest level
//  file class.
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
#include    "CIDLib_.hpp"

// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFileBase,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TFileBase
//  PREFIX: fbase
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFileBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TFileBase::TFileBase(const TString& strFileName) :

    m_strName(strFileName)
    , m_kflThis(strFileName.pszBuffer())
{
}

TFileBase::TFileBase(const tCIDLib::TCh* const pszFileName) :

    m_strName(pszFileName)
    , m_kflThis(pszFileName)
{
}

TFileBase::TFileBase() :

    m_strName(kCIDLib::pszEmptyZStr)
{
}

TFileBase::~TFileBase()
{
    if (!m_kflThis.bClose())
    {
        //
        //  If we get an error, log it but don't throw out of the destructor
        //  so we use a warning message.
        //
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
            , m_strName
        );
    }
}


// ---------------------------------------------------------------------------
//  TFileBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFileBase::bEndOfFile()
{
    tCIDLib::TCard8   c8CurPos = 0;
    tCIDLib::TCard8   c8CurSize = 0;

    if (!m_kflThis.bQueryCurSize(c8CurSize))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_QuerySize
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    // If current size is 0, then has to be
    if (!c8CurSize)
        return kCIDLib::True;

    // Query the current file position
    if (!m_kflThis.bQueryFilePtr(c8CurPos))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Seek
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return (c8CurPos >= c8CurSize);
}


tCIDLib::TBoolean
TFileBase::bExists(const tCIDLib::EDirSearchFlags eFlags) const
{
    tCIDLib::TBoolean bFound;
    if (!TKrnlFileSys::bExists(m_strName.pszBuffer(), bFound, eFlags))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_CheckExists
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return bFound;
}

tCIDLib::TBoolean
TFileBase::bExists( const   tCIDLib::EDirSearchFlags    eFlags
                    ,       tCIDLib::TBoolean&          bUnique) const
{
    tCIDLib::TBoolean bFound;
    if (!TKrnlFileSys::bExists(m_strName.pszBuffer(), bFound, bUnique, eFlags))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_CheckExists
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return bFound;
}


tCIDLib::TVoid TFileBase::Flush()
{
    if (!m_kflThis.bFlush())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Flush
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}


tCIDLib::TBoolean TFileBase::bIsOpen() const
{
    return m_kflThis.bIsValid();
}


tCIDLib::TVoid TFileBase::Close()
{
    if (m_kflThis.bIsValid())
    {
        if (!m_kflThis.bClose())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_Close
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , m_strName
            );
        }
    }
}


tCIDLib::TCard8 TFileBase::c8CurPos()
{
    tCIDLib::TCard8 c8Ret;
    if (!m_kflThis.bQueryFilePtr(c8Ret))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Seek
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return c8Ret;
}


tCIDLib::TCard8 TFileBase::c8CurSize()
{
    tCIDLib::TCard8 c8Size;
    if (!m_kflThis.bQueryCurSize(c8Size))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_QuerySize
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return c8Size;
}


const TKrnlFile& TFileBase::kflThis() const
{
    return m_kflThis;
}

TKrnlFile& TFileBase::kflThis()
{
    return m_kflThis;
}


tCIDLib::TVoid
TFileBase::Open(const   tCIDLib::EAccessModes   eAccess
                , const tCIDLib::ECreateActs eAction
                , const tCIDLib::EFilePerms     ePerms
                , const tCIDLib::EFileFlags     eFlags)
{
    //
    //  See if its one of the special file names. If so, then we translate
    //  it into a different call.
    //
    if (m_strName == L"$StdOut$")
    {
        if (!m_kflThis.bOpen(tCIDLib::EStdFiles::StdOut))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_Open
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
            );
        }
    }
     else if (m_strName == L"$StdIn$")
    {
        if (!m_kflThis.bOpen(tCIDLib::EStdFiles::StdIn))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_Open
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
            );
        }
    }
     else if (m_strName == L"$StdErr$")
    {
        if (!m_kflThis.bOpen(tCIDLib::EStdFiles::StdErr))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_Open
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
            );
        }
    }
     else
    {
        //
        //  Its not one of the special files, so lets call the normal method
        //  that opens it by name.
        //
        if (!m_kflThis.bOpen(eAccess, eAction, ePerms, eFlags))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_Open
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
            );
        }
    }
}

tCIDLib::TVoid TFileBase::Open(const tCIDLib::EStdFiles eStdFile)
{
    if (!m_kflThis.bOpen(eStdFile))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Open
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(m_kflThis.pszName())
        );
    }

    //
    //  Get the name out of the kernel file object that it selected for
    //  the standard file that the user wants.
    //
    m_strName = m_kflThis.pszName();
}


tCIDLib::TVoid
TFileBase::QueryFileTimes(TTime& tmLastAccess, TTime& tmLastWrite)
{
    tCIDLib::TEncodedTime enctLastAccess;
    tCIDLib::TEncodedTime enctLastWrite;

    if (!m_kflThis.bQueryFileTimes(enctLastAccess, enctLastWrite))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_QueryTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    // Get the raw times out to the time objects
    tmLastAccess = enctLastAccess;
    tmLastWrite = enctLastWrite;
}


const TString& TFileBase::strName() const
{
    return m_strName;
}


tCIDLib::TVoid
TFileBase::SetFileTimes(const   TTime&  tmLastAccess
                        , const TTime&  tmLastWrite)
{
    if (!m_kflThis.bSetFileTimes(tmLastAccess.enctTime(), tmLastWrite.enctTime()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_SetTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}


tCIDLib::TVoid TFileBase::SetLastAccessTime(const TTime& tmLastAccess)
{
    if (!m_kflThis.bSetLastAccessTime(tmLastAccess.enctTime()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_SetTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}

tCIDLib::TVoid TFileBase::SetLastWriteTime(const TTime& tmLastWrite)
{
    if (!m_kflThis.bSetLastWriteTime(tmLastWrite.enctTime()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_SetTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}


TTime TFileBase::tmLastAccess() const
{
    tCIDLib::TEncodedTime enctLastAccess;
    if (!m_kflThis.bQueryLastAccessTime(enctLastAccess))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_QueryTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return TTime(enctLastAccess);
}

TTime TFileBase::tmLastWrite() const
{
    tCIDLib::TEncodedTime enctLastWrite;
    if (!m_kflThis.bQueryLastWriteTime(enctLastWrite))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_QueryTime
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return TTime(enctLastWrite);
}


tCIDLib::TVoid TFileBase::TruncateAt(const tCIDLib::TCard8& c8Position)
{
    if (!m_kflThis.bTruncateAt(c8Position))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Truncate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}

tCIDLib::TVoid TFileBase::TruncateAt()
{
    if (!m_kflThis.bTruncateAt())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Truncate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}


const TString& TFileBase::strName(const TString& strNew)
{
    if (m_kflThis.bIsValid())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_AlreadyOpen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , m_strName
        );
    }

    if (!m_kflThis.bSetName(strNew.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_SetFileName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strNew
        );
    }

    // Store the new name and return a ref to our member
    m_strName = strNew;
    return m_strName;
}


// ---------------------------------------------------------------------------
//  TFileBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TFileBase::c4ReadRawBuf(        tCIDLib::TVoid* const   pBuf
                        , const tCIDLib::TCard4         c4BufSz)
{
    tCIDLib::TCard4 c4Ret;
    if (!m_kflThis.bReadBuffer(pBuf, c4BufSz, c4Ret))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Read
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4BufSz)
            , m_strName
        );
    }
    return c4Ret;
}


tCIDLib::TCard4
TFileBase::c4ReadRawBufTO(          tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4BufSz
                            , const tCIDLib::TCard4         c4MaxWait
                            ,       TEvent&                 evWait
                            , const tCIDLib::TBoolean       bThrowIfTO)
{
    tCIDLib::TCard4 c4Ret;
    if (!m_kflThis.bReadBufferTO(pBuf
                                , c4BufSz
                                , c4Ret
                                , c4MaxWait
                                , evWait.kevImpl()))
    {
        //
        //  See if it's just a timeout. If so, we only throw if the passed
        //  flag indicates we should.
        //
        TKrnlError kerrToThrow(TKrnlError::kerrLast());
        if (kerrToThrow.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            if (bThrowIfTO)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcFile_IOTimeout
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Timeout
                    , m_strName
                );
            }
        }
         else
        {
            // Not timeout, so throw general read error
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_Read
                , kerrToThrow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(c4BufSz)
                , m_strName
            );
        }
    }
    return c4Ret;
}


tCIDLib::TCard4
TFileBase::c4WriteRawBuf(   const   tCIDLib::TVoid* const pBuf
                            , const tCIDLib::TCard4       c4BufSz)
{
    // Do the write of the requested number of bytes
    tCIDLib::TCard4 c4Actual;
    if (!m_kflThis.bWriteBuffer(pBuf, c4BufSz, c4Actual))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Write
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4BufSz)
            , m_strName
        );
    }
    return c4Actual;
}


tCIDLib::TCard4
TFileBase::c4WriteRawBufTO( const   tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4BufSz
                            , const tCIDLib::TCard4         c4MaxWait
                            ,       TEvent&                 evWait)
{
    tCIDLib::TCard4 c4Ret;
    if (!m_kflThis.bWriteBufferTO(pBuf, c4BufSz, c4Ret, c4MaxWait, evWait.kevImpl()))
    {
        //
        //  See if it's just a timeout. If so, then throw a timeout error,
        //  else throw a failure error.
        //
        TKrnlError kerrToThrow(TKrnlError::kerrLast());
        if (kerrToThrow.errcId() == kKrnlErrs::errcGen_Timeout)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcFile_IOTimeout
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Timeout
                , m_strName
            );
        }

        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Write
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4BufSz)
            , m_strName
        );
    }
    return c4Ret;
}


tCIDLib::TCard8
TFileBase::c8OffsetPos(const tCIDLib::TInt8& i8OffsetBy)
{
    tCIDLib::TCard8 c8Ret;
    if (!m_kflThis.bOffsetFilePointer(i8OffsetBy, c8Ret))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Seek
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
    return c8Ret;
}

tCIDLib::TVoid TFileBase::SetPos(const tCIDLib::TCard8& c8ToSet)
{
    if (!m_kflThis.bSetFilePointer(c8ToSet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Seek
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}
