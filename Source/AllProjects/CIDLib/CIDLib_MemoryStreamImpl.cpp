//
// FILE NAME: CIDLib_MemoryStreamImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/29/1995
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
//  This file implements a stream data source/sink that wraps around a
//  memory buffer object. One of these is given to a stream which will then
//  use it as the backing storing for the data read and written on the
//  stream. There is one for binary streams and one for text streams.
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
RTTIDecls(TMemInStreamImpl,TInStreamImpl)
RTTIDecls(TMemOutStreamImpl,TOutStreamImpl)



// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TMemStreamImplInfo::TMemStreamImplInfo( const   TMemBuf* const      pmbufToUse
                                        , const tCIDLib::EAdoptOpts eAdoptBuf
                                        , const tCIDLib::TCard4     c4InitLogicalEnd) :
    c4EndIndex(c4InitLogicalEnd)
    , eAdopted(eAdoptBuf)
    , pmbufData(pmbufToUse)
{
}

TMemStreamImplInfo::~TMemStreamImplInfo()
{
    // If we adopted the memory buffer, then clean it up
    if (eAdopted == tCIDLib::EAdoptOpts::Adopt)
    {
        delete pmbufData;
        pmbufData = 0;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TMemInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMemInStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TMemInStreamImpl::TMemInStreamImpl( const   TMemBuf* const      pmbufToStream
                                    , const tCIDLib::TCard4     c4InitLogicalEnd
                                    , const tCIDLib::EAdoptOpts eAdopt) :
    m_c4Index(0)
    , m_pmbufSrc(pmbufToStream)
{
    //
    //  Make sure the initial logical end is not beyond the allocation size
    //  of the memory buffer.
    //
    if (c4InitLogicalEnd > pmbufToStream->c4Size())
    {
        // If we were to adopt, then clean up the buffer
        if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
            delete pmbufToStream;

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadEnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4InitLogicalEnd)
            , TCardinal(pmbufToStream->c4Size())
        );
    }

    // Looks ok, so create the info object and store it
    m_cptrInfo.SetPointer
    (
        new TMemStreamImplInfo(pmbufToStream, eAdopt, c4InitLogicalEnd)
    );
}

TMemInStreamImpl::TMemInStreamImpl(const TMemOutStreamImpl& strmiToSyncWith) :

    m_c4Index(0)
    , m_cptrInfo(strmiToSyncWith.m_cptrInfo)
    , m_pmbufSrc(nullptr)
{
    // Store our copy of the buffer
    m_pmbufSrc = m_cptrInfo->pmbufData;
}

TMemInStreamImpl::~TMemInStreamImpl()
{
    // Dtor of the counted pointer will clean up info object if no more refs
}


// ---------------------------------------------------------------------------
//  TMemInStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TMemInStreamImpl::c4ReadBytes(          tCIDLib::TVoid* const   pBufToFill
                                , const tCIDLib::TCard4         c4Count)
{
    //
    //  If the current index is beyond the end of data index, then set
    //  the actual bytes to read to zero. Else see if the current index
    //  plus the bytes read would go beyond the end of data, and clip the
    //  actual bytes to read back if so.
    //
    const tCIDLib::TCard4 c4EndIndex = m_cptrInfo->c4EndIndex;
    tCIDLib::TCard4 c4Actual = c4Count;
    if (m_c4Index >= c4EndIndex)
        c4Actual = 0;
    else if (m_c4Index + c4Count > c4EndIndex)
        c4Actual = c4EndIndex - m_c4Index;

    // If we have any actual bytes to read, then do it
    if (c4Actual)
    {
        //
        //  This will extend the allocation if required. However, this
        //  should never happen since we only allow reads up to the end of
        //  the available data.
        //
        m_pmbufSrc->CopyOut(pBufToFill, c4Actual, m_c4Index);

        // Add the actual bytes to the current position
        m_c4Index += c4Actual;
    }

    return c4Actual;
}


tCIDLib::TVoid TMemInStreamImpl::SkipForwardBy(const tCIDLib::TCard4 c4SkipBy)
{
    // We can only skip forward by the data available
    if (m_c4Index + c4SkipBy > m_cptrInfo->c4EndIndex)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_InvalidRelSeek
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4SkipBy)
        );
    }

    // Add the skip to our current index since it's legal
    m_c4Index += c4SkipBy;
}


// ---------------------------------------------------------------------------
//  TMemInStreamImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMemInStreamImpl::SetEndIndex(const tCIDLib::TCard4 c4NewIndex)
{
    // Make sure the new end isn't beyond the actual size of the backing buffer.
    if (c4NewIndex > m_cptrInfo->pmbufData->c4Size())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadNewEnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4NewIndex)
            , TCardinal(m_cptrInfo->pmbufData->c4Size())
        );
    }

    // Looks ok, so set the end index
    m_cptrInfo->c4EndIndex = c4NewIndex;
}



// ---------------------------------------------------------------------------
//   CLASS: TMemOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMemOutStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TMemOutStreamImpl::TMemOutStreamImpl(       TMemBuf* const      pmbufToStream
                                    , const tCIDLib::TCard4     c4InitLogicalEnd
                                    , const tCIDLib::EAdoptOpts eAdopt) :
    m_c4Index(0)
    , m_pmbufSink(pmbufToStream)
{
    //
    //  Make sure the initial logical end is not beyond the allocation size
    //  of the memory buffer.
    //
    if (c4InitLogicalEnd > pmbufToStream->c4Size())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadEnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4InitLogicalEnd)
            , TCardinal(pmbufToStream->c4Size())
        );
    }

    // Looks ok, so create the info object and store it
    m_cptrInfo.SetPointer
    (
        new TMemStreamImplInfo(pmbufToStream, eAdopt, c4InitLogicalEnd)
    );
}

TMemOutStreamImpl::~TMemOutStreamImpl()
{
    // Dtor of the counter pointer will clean up info object if no more refs
}


// ---------------------------------------------------------------------------
//  TMemOutStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TMemOutStreamImpl::c4WriteBytes(const   tCIDLib::TVoid* const   pBufToUse
                                , const tCIDLib::TCard4         c4Count)
{
    tCIDLib::TCard4 c4Actual = c4Count;
    tCIDLib::TCard4 c4CurPos = m_c4Index;

    if (c4CurPos + c4Count > m_pmbufSink->c4MaxSize())
        c4Actual = m_pmbufSink->c4MaxSize() - c4CurPos;

    // This will extend the current allocation if needed
    m_pmbufSink->CopyIn(pBufToUse, c4Actual, c4CurPos);

    // Add the actual bytes to the current position
    m_c4Index += c4Actual;

    //
    //  A write extends the readable data, so a successful write means
    //  we need to move our end position index up to the current position.
    //
    m_cptrInfo->c4EndIndex = m_c4Index;

    return c4Actual;
}


// ---------------------------------------------------------------------------
//  TMemOutStreamImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMemOutStreamImpl::TruncateAt(const tCIDLib::TCard4 c4At)
{
    // We don't allow it to be past our current end of stream
    if (c4At > m_cptrInfo->c4EndIndex)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadEnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , TCardinal(m_cptrInfo->c4EndIndex)
        );
    }

    // We reset the current position to zero
    m_c4Index = 0;
    m_cptrInfo->c4EndIndex = tCIDLib::TCard4(c4At);
}

