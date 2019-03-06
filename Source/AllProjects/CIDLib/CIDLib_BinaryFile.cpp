//
// FILE NAME: CIDLib_BinFile.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/20/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TBinaryFile class. This class is a derivative
//  of the primeval TFileBase class that allows for random access and
//  arbitrary read/write sizes.
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
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TBinaryFile,TFileBase)


// ---------------------------------------------------------------------------
//   CLASS: TBinaryFile
//  PREFIX: bfl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinaryFile: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinaryFile::TBinaryFile()
{
}

TBinaryFile::TBinaryFile(const TString& strFileName) :

    TFileBase(strFileName)
{
}

TBinaryFile::TBinaryFile(const tCIDLib::TCh* const pszFileName) :

    TFileBase(pszFileName)
{
}

TBinaryFile::~TBinaryFile()
{
}


// ---------------------------------------------------------------------------
//  TBinaryFile: Public, non-virtual methods
// ---------------------------------------------------------------------------
TBinInStream* TBinaryFile::pstrmMakeReadable(const tCIDLib::EAdoptOpts eAdopt)
{
    //
    //  We just create a new file stream implementation object, and pass
    //  it a pointer to ourself, and tell it the adoption scheme that the
    //  caller wanted.
    //
    //  This implementation object is then given to a newly allocated
    //  binary stream object, which is returned to the caller.
    //
    return new TBinInStream(new TFileInStreamImpl(this, eAdopt));
}

TBinOutStream* TBinaryFile::pstrmMakeWriteable(const tCIDLib::EAdoptOpts eAdopt)
{
    //
    //  We just create a new file stream implementation object, and pass
    //  it a pointer to ourself, and tell it the adoption scheme that the
    //  caller wanted.
    //
    //  This implementation object is then given to a newly allocated
    //  binary stream object, which is returned to the caller.
    //
    return new TBinOutStream(new TFileOutStreamImpl(this, eAdopt));
}



// ---------------------------------------------------------------------------
//  TBinaryFile: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Read a buffer's worth of data. They can indicate if not reading all the
//  bytes indicated is an error or not.
//
tCIDLib::TCard4
TBinaryFile::c4ReadBuffer(          TMemBuf&            mbufTarget
                            , const tCIDLib::TCard4     c4BytesToRead
                            , const tCIDLib::EAllData   eAllData)
{
    //
    //  Check that the bytes to read is not larger than the target buffer's
    //  maximum size.
    //
    if (mbufTarget.c4MaxSize() < c4BytesToRead)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BufOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufTarget.c4MaxSize())
        );
    }

    //
    //  Make sure that the buffer is allocated up to the bytes that we
    //  are going to read. Tell it not to preserve current contents.
    //
    if (c4BytesToRead > mbufTarget.c4Size())
        mbufTarget.Reallocate(c4BytesToRead, kCIDLib::False);

    const tCIDLib::TCard4 c4Ret = c4ReadRawBuf(mbufTarget.pc1Data(), c4BytesToRead);
    if ((c4Ret != c4BytesToRead) && (eAllData == tCIDLib::EAllData::FailIfNotAll))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_NotAllData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4Ret)
            , TCardinal(c4BytesToRead)
        );
    }
    return c4Ret;
}

tCIDLib::TCard4
TBinaryFile::c4ReadBuffer(          tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4BufSz
                            , const tCIDLib::EAllData       eAllData)
{
    // Just delegate to our parent class
    const tCIDLib::TCard4 c4Ret = c4ReadRawBuf(pBuf, c4BufSz);
    if ((c4Ret != c4BufSz) && (eAllData == tCIDLib::EAllData::FailIfNotAll))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_NotAllData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4Ret)
            , TCardinal(c4BufSz)
        );
    }
    return c4Ret;
}



tCIDLib::TCard4
TBinaryFile::c4ReadBufferTO(        TMemBuf&            mbufTarget
                            , const tCIDLib::TCard4     c4BytesToRead
                            , const tCIDLib::TCard4     c4WaitFor
                            ,       TEvent&             evWait
                            , const tCIDLib::TBoolean   bThrowIfTO)
{
    //
    //  Check that the bytes to read is not larger than the target buffer's
    //  maximum size.
    //
    if (mbufTarget.c4MaxSize() < c4BytesToRead)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BufOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufTarget.c4MaxSize())
        );
    }

    //
    //  Make sure that the buffer is allocated up to the bytes that we
    //  are going to read. Tell it not to preserve current contents.
    //
    if (c4BytesToRead > mbufTarget.c4Size())
        mbufTarget.Reallocate(c4BytesToRead, kCIDLib::False);

    return c4ReadRawBufTO
    (
        mbufTarget.pc1Data()
        , c4BytesToRead
        , c4WaitFor
        , evWait
        , bThrowIfTO
    );
}

tCIDLib::TCard4
TBinaryFile::c4ReadBufferTO(        tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4BufSz
                            , const tCIDLib::TCard4         c4WaitFor
                            ,       TEvent&                 evWait
                            , const tCIDLib::TBoolean       bThrowIfTO)
{
    // Just delegate to our parent class
    return c4ReadRawBufTO(pBuf, c4BufSz, c4WaitFor, evWait, bThrowIfTO);
}


tCIDLib::TCard4
TBinaryFile::c4WriteBuffer( const   TMemBuf&        mbufSource
                            , const tCIDLib::TCard4 c4BytesToWrite)
{
    //
    //  Check that the bytes to write is not larger than the target. But
    //  check for the special c4MaxCard value which means to write the
    //  current size of the buffer.
    //
    tCIDLib::TCard4 c4ActualWrite = c4BytesToWrite;
    if (c4ActualWrite == kCIDLib::c4MaxCard)
    {
        c4ActualWrite = mbufSource.c4Size();
    }
     else if (mbufSource.c4Size() < c4BytesToWrite)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_NotThatManyBytes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufSource.c4Size())
            , TCardinal(c4BytesToWrite)
        );
    }
    return c4WriteRawBuf(mbufSource.pc1Data(), c4ActualWrite);
}

tCIDLib::TCard4
TBinaryFile::c4WriteBuffer( const   tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4BufSz)
{
    // Just delegate to our parent class
    return c4WriteRawBuf(pBuf, c4BufSz);
}



tCIDLib::TCard4
TBinaryFile::c4WriteBufferTO(const  TMemBuf&        mbufSource
                            , const tCIDLib::TCard4 c4BytesToWrite
                            , const tCIDLib::TCard4 c4WaitFor
                            ,       TEvent&         evWait)
{
    //
    //  Check that the bytes to write is not larger than the target. But
    //  check for the special c4MaxCard value which means to write the
    //  current size of the buffer.
    //
    tCIDLib::TCard4 c4ActualWrite = c4BytesToWrite;
    if (c4ActualWrite == kCIDLib::c4MaxCard)
    {
        c4ActualWrite = mbufSource.c4Size();
    }
     else if (mbufSource.c4Size() < c4BytesToWrite)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_NotThatManyBytes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufSource.c4Size())
            , TCardinal(c4BytesToWrite)
        );
    }
    return c4WriteRawBufTO(mbufSource.pc1Data(), c4ActualWrite, c4WaitFor, evWait);
}

tCIDLib::TCard4
TBinaryFile::c4WriteBufferTO(const  tCIDLib::TVoid* const   pBuf
                            , const tCIDLib::TCard4         c4BufSz
                            , const tCIDLib::TCard4         c4WaitFor
                            ,       TEvent&                 evWait)
{
    // Just delegate to our parent class
    return c4WriteRawBufTO(pBuf, c4BufSz, c4WaitFor, evWait);
}


tCIDLib::TCard8 TBinaryFile::c8OffsetFilePos(const tCIDLib::TInt8& i8OffsetBy)
{
    // Just delegate to our parent class
    return c8OffsetPos(i8OffsetBy);
}


//
//  This is just a seek to a magic value that indicates the end of stream,
//  but it's very easy to use c4MaxCard instead of c8MaxCard and that will
//  cause a 4GB file to be written. It's happened enough times that it's worth
//  just having this much safer helper to do it.
//
tCIDLib::TVoid TBinaryFile::SeekToEnd()
{
    SetPos(kCIDLib::c8MaxCard);
}


tCIDLib::TVoid TBinaryFile::SetFilePos(const tCIDLib::TCard8& c8ToSet)
{
    // Just delegate to our parent class
    SetPos(c8ToSet);
}


