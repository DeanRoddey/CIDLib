//
// FILE NAME: CIDLib_ChunkedMemStreamImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/12/2014
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
//  This file implements a stream data source/sink that wraps around a growable
//  list of memory chunks. This allows it to grow efficiently to whatever size is
//  required.
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
RTTIDecls(TChunkedBinInStream,TBinInStream)
RTTIDecls(TChunkedBinOutStream,TBinOutStream)
RTTIDecls(TChunkedTextInStream,TTextInStream)
RTTIDecls(TChunkedTextOutStream,TTextOutStream)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDLib_ChunkedMemStream
    {
        //
        //  The size of our memory chunks. We grow by 1MB chunks.
        //
        const tCIDLib::TCard4 c4ChunkSz = 0x100000;
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TChunkedMemStreamImplInfo
//  PREFIX: sii
// ---------------------------------------------------------------------------
class TChunkedMemStreamImplInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TChunkedMemStreamImplInfo() = delete;

        TChunkedMemStreamImplInfo
        (
            const   tCIDLib::TCard4         c4MaxSize
        );

        TChunkedMemStreamImplInfo(const TChunkedMemStreamImplInfo&) = delete;

        ~TChunkedMemStreamImplInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedMemStreamImplInfo& operator=(const TChunkedMemStreamImplInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurEnd() const;

        tCIDLib::TCard4 c4MaxSize() const;

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToRead
            , const tCIDLib::TCard4         c4At
        );

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
            , const tCIDLib::TCard4         c4At
        );

        tCIDLib::TCard4 c4CopyOutTo
        (
                    TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4TarStartInd
        )   const;

        tCIDLib::TCard4 c4CopyOutToStream
        (
                    TBinOutStream&          strmTar
            , const tCIDLib::TCard4         c4Count = kCIDLib::c4MaxCard
        )   const;

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard4         c4At
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4AbsorbData
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcCnt
            , const tCIDLib::TCard4         c4SrcIndex
            , const tCIDLib::TCard4         c4TarIndex
        );

        tCIDLib::TCard4 c4PullData
        (
                    tCIDLib::TCard1*        pc1Tar
            , const tCIDLib::TCard4         c4EndSrcInd
            , const tCIDLib::TCard4         c4SrcIndex
            , const tCIDLib::TCard4         c4TarIndex
        );

        tCIDLib::TVoid ExpandUpTo
        (
            const   tCIDLib::TCard4         c4TarIndex
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4EndBuf
        //  m_c4EndByte
        //      This defines the current end of data, which can be less than the
        //      actual allocated data, since we can be reset. We want to resuse
        //      already allocated buffers, so we have to work back up through
        //      them, then we allocate more.
        //
        //  m_c4MaxSize
        //      The maximum size that the client code wants to let it grow to.
        //
        //  m_colBufs
        //      The collection of heap memory buffers we use to store the data.
        //      We allocate an initial one upon first write, and then add more
        //      as required.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4EndBuf;
        tCIDLib::TCard4         m_c4EndByte;
        const tCIDLib::TCard4   m_c4MaxSize;
        TRefVector<THeapBuf>    m_colBufs;
};


// ---------------------------------------------------------------------------
//  TChunkedMemStreamImplInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TChunkedMemStreamImplInfo::TChunkedMemStreamImplInfo(const tCIDLib::TCard4 c4MaxSize) :

    m_c4EndBuf(0)
    , m_c4EndByte(0)
    , m_c4MaxSize(c4MaxSize)
    , m_colBufs(tCIDLib::EAdoptOpts::Adopt)
{
    // Max size cannot be zero
    if (!m_c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_ZeroBufSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
}

TChunkedMemStreamImplInfo::~TChunkedMemStreamImplInfo()
{
    // Force a release of our buffers so that we can catch any exxeptions
    try
    {
        m_colBufs.RemoveAll();
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::BinStreams))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }
}


// ---------------------------------------------------------------------------
//  TChunkedMemStreamImplInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TChunkedMemStreamImplInfo::c4CurEnd() const
{
    // If no buffers yet, then clearly we are empty
    if (m_colBufs.bIsEmpty())
        return 0;

    // Else calculate it
    return (m_c4EndBuf * CIDLib_ChunkedMemStream::c4ChunkSz) + m_c4EndByte;

}


tCIDLib::TCard4 TChunkedMemStreamImplInfo::c4MaxSize() const
{
    return m_c4MaxSize;
}


tCIDLib::TCard4
TChunkedMemStreamImplInfo::c4ReadBytes(         tCIDLib::TVoid* const   pBuffer
                                        , const tCIDLib::TCard4         c4Count
                                        , const tCIDLib::TCard4         c4At)
{
    //
    //  If we have no buffers, then clearly can't return anything, and this avoids
    //  special cases below. We want to avoid allocating anything unless it's
    //  needed, so we don't preallocate an initial buffer.
    //
    if (m_colBufs.bIsEmpty())
        return 0;

    // If the at is at or beyond the end of data, then nothing
    const tCIDLib::TCard4 c4End = c4CurEnd();
    if (c4At >= c4End)
        return 0;

    // See how many we can actually read
    tCIDLib::TCard4 c4RealCnt = c4Count;
    if (c4At + c4Count > c4End)
    {
        // We can take up to the max amount
        c4RealCnt = c4End - c4At;
    }

    // If we can't get anything, return zero now to avoid corner cases below
    if (!c4RealCnt)
        return 0;

    // Look at it as a byte array
    tCIDLib::TCard1* pc1Tar = static_cast<tCIDLib::TCard1*>(pBuffer);


    //
    //  Read data from our buffers until we hit the end or get the count that
    //  we calculated.
    //
    tCIDLib::TCard4 c4SrcIndex = c4At;
    const tCIDLib::TCard4 c4EndIndex = c4At + c4RealCnt;
    tCIDLib::TCard4 c4TarIndex = 0;
    while (c4SrcIndex < c4EndIndex)
    {
        const tCIDLib::TCard4 c4ThsTime = c4PullData
        (
            pc1Tar, c4EndIndex, c4SrcIndex, c4TarIndex
        );

        c4SrcIndex += c4ThsTime;
        c4TarIndex += c4ThsTime;
    }

    CIDAssert(c4TarIndex == c4RealCnt, L"Did not pull expected byte count");

    return c4RealCnt;
}


//
//  Note that the at position could be beyond the current end of data. That is
//  allowed, and we are supposed to just start writing there. That means we will
//  have to expand the data from where it is, up to the point where the new write
//  would occur.
//
tCIDLib::TCard4
TChunkedMemStreamImplInfo::c4WriteBytes(const   tCIDLib::TVoid* const   pBuffer
                                        , const tCIDLib::TCard4         c4Count
                                        , const tCIDLib::TCard4         c4At)
{
    //
    //  If the at is at or beyond the max, we can't do anyhing, and this avoids
    //  possible underflow in the calculations below.
    //
    if (c4At >= m_c4MaxSize)
        return 0;

    // Calculate how many bytes we can actually accomodate
    tCIDLib::TCard4 c4RealCnt = c4Count;
    if (c4At + c4Count > m_c4MaxSize)
    {
        // We can take up to the max amount
        c4RealCnt = m_c4MaxSize - c4At;
    }

    //
    //  If we can't accomodate anything, return zero now to avoid corner cases below.
    //  This shouldn't happen, given the above, but don't temp fate.
    //
    if (!c4RealCnt)
        return 0;

    // Look at it as a byte array
    const tCIDLib::TCard1* pc1Src = static_cast<const tCIDLib::TCard1*>(pBuffer);

    //
    //  If we have no buffers yet, then allocate one, chunk sized, fully allocated.
    //  We do this so that this guy won't allocate anything unless actually written
    //  to, so we have to fault the first one in.
    //
    if (m_colBufs.bIsEmpty())
    {
        m_colBufs.Add
        (
            new THeapBuf
            (
                CIDLib_ChunkedMemStream::c4ChunkSz
                , CIDLib_ChunkedMemStream::c4ChunkSz
            )
        );

        // Make sure our two buffers are reset
        m_c4EndBuf = 0;
        m_c4EndByte = 0;
    }

    //
    //  If the at is beyond our current data, then handle the special case of expanding
    //  our data out until the new at position falls within a valid chunk. We know
    //  that this write won't go beyond our max, so this is safe.
    //
    if (c4At > c4CurEnd())
        ExpandUpTo(c4At);

    // Now absorb data until we handle what we calculated we can handle
    tCIDLib::TCard4 c4SrcIndex = 0;
    tCIDLib::TCard4 c4TarIndex = c4At;
    while (c4SrcIndex < c4RealCnt)
    {
        const tCIDLib::TCard4 c4ThsTime = c4AbsorbData
        (
            pc1Src, c4RealCnt, c4SrcIndex, c4TarIndex
        );

        c4SrcIndex += c4ThsTime;
        c4TarIndex += c4ThsTime;
    }

    // We should have absorbed the real count
    CIDAssert(c4SrcIndex == c4RealCnt, L"Did not absorb expected byte count");
    return c4RealCnt;

}


//
//  This allows the outside world to get to our data as a contiguous memory buffer
//  if they have to. We copy our data out to the target, starting at the indicated
//  target index.
//
tCIDLib::TCard4
TChunkedMemStreamImplInfo::c4CopyOutTo(         TMemBuf&        mbufTar
                                        , const tCIDLib::TCard4 c4TarStartInd) const
{
    //
    //  Calculate how many bytes we have and return now if none. That avoids
    //  special cases below.
    //
    const tCIDLib::TCard4 c4Count = c4CurEnd();
    if (!c4Count)
        return 0;

    // If larger than the max of the target, he can't take it
    if (c4Count + c4TarStartInd > mbufTar.c4MaxSize())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BufOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Overflow
            , TCardinal(mbufTar.c4MaxSize())
        );
    }

    // Allocate him up to the right size if not already that or more
    if (mbufTar.c4Size() < c4Count + c4TarStartInd)
        mbufTar.Reallocate(c4Count + c4TarStartInd, kCIDLib::False);

    // This tracks where we copy each new chunk into the target
    tCIDLib::TCard4 c4TarIndex = c4TarStartInd;

    // First copy out any full chunks
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4EndBuf; c4Index++)
    {
        mbufTar.CopyIn
        (
            *m_colBufs[c4Index]
            , CIDLib_ChunkedMemStream::c4ChunkSz
            , c4TarIndex
        );
        c4TarIndex += CIDLib_ChunkedMemStream::c4ChunkSz;
    }

    // And now any remainder in the current buffer
    if (m_c4EndByte)
    {
        mbufTar.CopyIn(*m_colBufs[m_c4EndBuf], m_c4EndByte, c4TarIndex);
        c4TarIndex += m_c4EndByte;
    }

    // We should have done all of the available bytes
    CIDAssert(c4TarIndex == c4Count + c4TarStartInd, L"Did not get to expected index");
    return c4Count;
}


//
//  Similar to CopyOutTo but copies our data to another output stream. This often
//  avoids the need to copy the data out to a buffer just so it can be sent to another
//  stream.
//
tCIDLib::TCard4
TChunkedMemStreamImplInfo::c4CopyOutToStream(       TBinOutStream&  strmTar
                                            , const tCIDLib::TCard4 c4Count) const
{
    //
    //  Calculate how many bytes we have and return now if none. That avoids
    //  special cases below.
    //
    tCIDLib::TCard4 c4RealCount = c4Count;
    if (c4RealCount == kCIDLib::c4MaxCard)
        c4RealCount = c4CurEnd();

    if (!c4RealCount)
        return 0;

    // Calculate how many full buffers we can do and do those
    tCIDLib::TCard4 c4BytesSoFar = 0;
    tCIDLib::TCard4 c4BufInd = 0;
    const tCIDLib::TCard4 c4FullCnt = c4RealCount / CIDLib_ChunkedMemStream::c4ChunkSz;
    while (c4BufInd < c4FullCnt)
    {
        strmTar.c4WriteBuffer(*m_colBufs[c4BufInd], CIDLib_ChunkedMemStream::c4ChunkSz);
        c4BytesSoFar += CIDLib_ChunkedMemStream::c4ChunkSz;
        c4BufInd++;
    }

    // If we aren't done yet, then do any trailing partial one
    if (c4BytesSoFar < c4RealCount)
    {
        const tCIDLib::TCard4 c4Remainder(c4RealCount - c4BytesSoFar);
        strmTar.c4WriteBuffer(*m_colBufs[c4BufInd], c4Remainder);
        c4BytesSoFar += c4Remainder;
    }

    // We should have done all of the available bytes
    CIDAssert(c4BytesSoFar == c4RealCount, L"Did not get to expected index");

    // Make sure the target is flushed
    strmTar.Flush();
    return c4Count;
}


//
//  If the passed position is within our current data, then we will set our end
//  position to there.
//
tCIDLib::TVoid TChunkedMemStreamImplInfo::TruncateAt(const tCIDLib::TCard4 c4At)
{
    // Can't be past the current size
    const tCIDLib::TCard4 c4End = c4CurEnd();
    if (c4At > c4End)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadEnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4At)
            , TCardinal(c4End)
        );
    }

    // Set the new end indices to the indicated position
    m_c4EndBuf = (c4At / CIDLib_ChunkedMemStream::c4ChunkSz);
    m_c4EndByte = (c4At % CIDLib_ChunkedMemStream::c4ChunkSz);
}



// ---------------------------------------------------------------------------
//  TChunkedMemStreamImplInfo:: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is the primitive for absorbing user data. We figure out what buffer and
//  index the target index reflects and will copy as much data as we can into that
//  buffer.
//
//  If this is the last buffer and we use it all the way up, we will add another one.
//
tCIDLib::TCard4
TChunkedMemStreamImplInfo::c4AbsorbData(const   tCIDLib::TCard1* const  pc1Src
                                        , const tCIDLib::TCard4         c4SrcCount
                                        , const tCIDLib::TCard4         c4SrcIndex
                                        , const tCIDLib::TCard4         c4TarIndex)
{
    // Figure out the chunk and chunk index for the target index
    const tCIDLib::TCard4 c4TarChunkInd = (c4TarIndex / CIDLib_ChunkedMemStream::c4ChunkSz);
    const tCIDLib::TCard4 c4TarByteInd = (c4TarIndex % CIDLib_ChunkedMemStream::c4ChunkSz);

    // How much space do we have in the target chunk
    const tCIDLib::TCard4 c4SpaceAvail = CIDLib_ChunkedMemStream::c4ChunkSz - c4TarByteInd;

    // We can absorb the smaller of the data remaining to aborb or the space available
    const tCIDLib::TCard4 c4CanAbsorb = tCIDLib::MinVal
    (
        c4SrcCount - c4SrcIndex, c4SpaceAvail
    );

    //
    //  Copy in at the target index, from the source index within the source
    //  buffer.
    //
    m_colBufs[c4TarChunkInd]->CopyIn(&pc1Src[c4SrcIndex], c4CanAbsorb, c4TarByteInd);

    // If this is the last buffer, then we have to do some special checks
    if (c4TarChunkInd == m_c4EndBuf)
    {
        //
        //  If we went beyond what is the current end point, then extend the end
        //  upwards to that point.
        //
        if (c4TarByteInd + c4CanAbsorb > m_c4EndByte)
            m_c4EndByte = c4TarByteInd + c4CanAbsorb;

        CIDAssert
        (
            m_c4EndByte <= CIDLib_ChunkedMemStream::c4ChunkSz
            , L"Invalid chunk index after absorption"
        );

        // If we maxed out this buffer, move up, allocate a new one if needed
        if (m_c4EndByte == CIDLib_ChunkedMemStream::c4ChunkSz)
        {
            // Move up to the next buffer
            m_c4EndBuf++;

            // If it's not been allocated yet, then do it
            if (m_c4EndBuf >= m_colBufs.c4ElemCount())
            {
                m_colBufs.Add
                (
                    new THeapBuf
                    (
                        CIDLib_ChunkedMemStream::c4ChunkSz
                        , CIDLib_ChunkedMemStream::c4ChunkSz
                    )
                );
            }

            // And the end byte index is now zero within this new end buffer
            m_c4EndByte = 0;
        }
    }

    // Return how much we absorbed
    return c4CanAbsorb;
}


//
//  The opposite of absorb data above, this is the primitive for pulling data out
//  of our buffers. We figure out what chunk and index within that chunk that the
//  current source index would be at. Then we see how much we can serve up out of
//  that buffer and give back that much.
//
tCIDLib::TCard4
TChunkedMemStreamImplInfo::c4PullData(          tCIDLib::TCard1*    pc1Tar
                                        , const tCIDLib::TCard4     c4EndSrcInd
                                        , const tCIDLib::TCard4     c4SrcIndex
                                        , const tCIDLib::TCard4     c4TarIndex)
{
    // Figure out the chunk and chunk index for the source index
    const tCIDLib::TCard4 c4SrcChunkInd = (c4SrcIndex / CIDLib_ChunkedMemStream::c4ChunkSz);
    const tCIDLib::TCard4 c4SrcByteInd = (c4SrcIndex % CIDLib_ChunkedMemStream::c4ChunkSz);

    //
    //  How much data do we have in the target chunk. If it's the last buffer, we have
    //  to go by the current end byte, else the full size. Make sure that it's not
    //  beyond the available, or we'll underflow
    //
    tCIDLib::TCard4 c4DataAvail = 0;
    if (c4SrcChunkInd == m_c4EndBuf)
    {
        CIDAssert(c4SrcByteInd <= m_c4EndByte, L"Underflow in chunked stream pull");
        c4DataAvail = m_c4EndByte - c4SrcByteInd;
    }
     else
    {
        CIDAssert
        (
            c4SrcByteInd <= CIDLib_ChunkedMemStream::c4ChunkSz
            , L"Underflow in chunked stream pull"
        );
        c4DataAvail = CIDLib_ChunkedMemStream::c4ChunkSz - c4SrcByteInd;
    }

    // We can pull the smaller of the remaining data or the bytes left to pull
    const tCIDLib::TCard4 c4CanPull = tCIDLib::MinVal
    (
        c4EndSrcInd - c4SrcIndex, c4DataAvail
    );

    // Copy out from the source index, to the target index
    m_colBufs[c4SrcChunkInd]->CopyOut(&pc1Tar[c4TarIndex], c4CanPull, c4SrcByteInd);

    return c4CanPull;
}



//
//  If the target index is beyond our current expansion, then we need to expand up
//  to the point where the target falls within an allocated buffer.
//
//  The caller will only call this if it actually needs to be done.
//
//  The data in between is unitialized, since it's never been written to.
//
tCIDLib::TVoid TChunkedMemStreamImplInfo::ExpandUpTo(const tCIDLib::TCard4 c4TarIndex)
{
    // Figure out the chunk index that this index would fall into
    const tCIDLib::TCard4 c4TarChunk = (c4TarIndex / CIDLib_ChunkedMemStream::c4ChunkSz);

    // Allocate new buffers until we get to that one
    tCIDLib::TCard4 c4Count = m_colBufs.c4ElemCount();
    while (c4Count < c4TarChunk)
    {
        m_colBufs.Add
        (
            new THeapBuf
            (
                CIDLib_ChunkedMemStream::c4ChunkSz
                , CIDLib_ChunkedMemStream::c4ChunkSz
            )
        );
    }
}





// ---------------------------------------------------------------------------
//   CLASS: TChunkedInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class TChunkedInStreamImpl : public TInStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TChunkedInStreamImpl() = delete;

        TChunkedInStreamImpl
        (
            const   TChunkedOutStreamImpl&  strmiToSyncTo
        );

        TChunkedInStreamImpl(const TChunkedInStreamImpl&) = delete;

        ~TChunkedInStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedInStreamImpl& operator=(const TChunkedInStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToRead
        )   override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        tCIDLib::TVoid SetEndIndex
        (
            const   tCIDLib::TCard4         c4NewIndex
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Index
        //      The current index into the memory buffer, i.e. our read
        //      position for this stream impl.
        //
        //  m_pcptrInfo
        //      This impl object's stream impl info object. It may be shared with
        //      an output stream, so that's why its counted.
        // -------------------------------------------------------------------
        tCIDLib::TCard4                     m_c4Index;
        TCntPtr<TChunkedMemStreamImplInfo>  m_cptrInfo;
};



// ---------------------------------------------------------------------------
//   CLASS: TChunkedOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class TChunkedOutStreamImpl : public TOutStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TChunkedOutStreamImpl() = delete;

        TChunkedOutStreamImpl(const TChunkedOutStreamImpl&) = delete;

        TChunkedOutStreamImpl
        (
            const   tCIDLib::TCard4         c4MaxSize
        );

        ~TChunkedOutStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedOutStreamImpl& operator=(const TChunkedOutStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
        )   override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SeekToEnd() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CurSize() const;

        tCIDLib::TCard4 c4MaxSize() const;

        tCIDLib::TCard4 c4CopyOutTo
        (
                    TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4StartAt
        )   const;

        tCIDLib::TCard4 c4CopyOutToStream
        (
                    TBinOutStream&          strmTar
            , const tCIDLib::TCard4         c4Count = kCIDLib::c4MaxCard
        )   const;

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard4         c4At
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TChunkedInStreamImpl;
        friend class TChunkedInTextStream;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurPos
        //      Our current write position, which may not be at the actual logical
        //      end of stream.
        //
        //  m_cptrInfo
        //      A counter point to the stream info object that provides our buffers.
        //      It may be shared with an input stream impl.
        // -------------------------------------------------------------------
        tCIDLib::TCard4                     m_c4CurPos;
        TCntPtr<TChunkedMemStreamImplInfo>  m_cptrInfo;
};




// ---------------------------------------------------------------------------
//   CLASS: TChunkedInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedInStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------

// We only support liked input streams
TChunkedInStreamImpl::TChunkedInStreamImpl(const TChunkedOutStreamImpl& strmiToSyncTo) :

    m_c4Index(0)
    , m_cptrInfo(strmiToSyncTo.m_cptrInfo)
{
}

TChunkedInStreamImpl::~TChunkedInStreamImpl()
{
    // Force a drop of our info object ref, so we can catch any exceptions
    try
    {
        m_cptrInfo.DropRef();
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::BinStreams))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }
}


// ---------------------------------------------------------------------------
//  Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TChunkedInStreamImpl::bIsOpen() const
{
    // We are always open
    return kCIDLib::True;
}


tCIDLib::TCard4
TChunkedInStreamImpl::c4ReadBytes(          tCIDLib::TVoid* const   pBuffer
                                    , const tCIDLib::TCard4         c4Count)
{
    //
    //  If the current index is beyond the end of data index, then set
    //  the actual bytes to read to zero. Else see if the current index
    //  plus the bytes read would go beyond the end of data, and clip the
    //  actual bytes to read back if so.
    //
    const tCIDLib::TCard4 c4EndIndex = m_cptrInfo->c4CurEnd();
    tCIDLib::TCard4 c4Actual = c4Count;
    if (m_c4Index >= c4EndIndex)
        c4Actual = 0;
    else if (m_c4Index + c4Count > c4EndIndex)
        c4Actual = c4EndIndex - m_c4Index;

    // If we have any actual bytes to read, then do it
    if (c4Actual)
    {
        if (m_cptrInfo->c4ReadBytes(pBuffer, c4Actual, m_c4Index) != c4Actual)
        {
            CIDAssert2(L"Bytes read was not what was calculated");
        }

        // Add the actual bytes to the current position
        m_c4Index += c4Actual;
    }

    return c4Actual;

}


tCIDLib::TCard8 TChunkedInStreamImpl::c8CurPos() const
{
    // Just return our current position index
    return m_c4Index;
}


tCIDLib::TVoid TChunkedInStreamImpl::Reset()
{
    // Just reset our read index
    m_c4Index = 0;
}


tCIDLib::TVoid TChunkedInStreamImpl::SkipForwardBy(const tCIDLib::TCard4 c4SkipBy)
{
    // We can only skip forward by the data available
    if (m_c4Index + c4SkipBy > m_cptrInfo->c4CurEnd())
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

    // Add the skip to our current index, since it's legal
    m_c4Index += c4SkipBy;
}



// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TChunkedInStreamImpl::c4CurSize() const
{
    // Return the current end of data
    return m_cptrInfo->c4CurEnd();
}


tCIDLib::TVoid TChunkedInStreamImpl::SetEndIndex(const tCIDLib::TCard4 c4NewIndex)
{
    //
    //  If beyond the end of current data, this will throw. If we have a linked
    //  output stream it may leave his write position beyond end of data.
    //
    m_cptrInfo->TruncateAt(c4NewIndex);

    // And reset our position back to zero
    m_c4Index = 0;
}





// ---------------------------------------------------------------------------
//   CLASS: TChunkedOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedOutStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TChunkedOutStreamImpl::TChunkedOutStreamImpl(const tCIDLib::TCard4 c4MaxSize) :

    m_c4CurPos(0)
    , m_cptrInfo(new TChunkedMemStreamImplInfo(c4MaxSize))
{

}

TChunkedOutStreamImpl::~TChunkedOutStreamImpl()
{
    // Force a drop of our info object ref, so we can catch any exceptions
    try
    {
        m_cptrInfo.DropRef();
    }

    catch(TError& errToCatch)
    {
        if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::BinStreams))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }
}


// ---------------------------------------------------------------------------
//  TChunkedOutStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------

// This one just always returns true
tCIDLib::TBoolean TChunkedOutStreamImpl::bIsOpen() const
{
    return kCIDLib::True;
}


//
//  We write as much as we can into our current buffer. If it cannot hold it all
//  we loop until we can store it all, adding new buffers to the list.
//
//  The only reason we wouldn't write it all is if we max out on the allowable
//  size.
//
tCIDLib::TCard4
TChunkedOutStreamImpl::c4WriteBytes(const   tCIDLib::TVoid* const   pBufToUse
                                    , const tCIDLib::TCard4         c4Count)
{
    // Pass on to the info object
    const tCIDLib::TCard4 c4Written = m_cptrInfo->c4WriteBytes
    (
        pBufToUse, c4Count, m_c4CurPos
    );

    //
    //  Move our current position up by the number of bytes actually written, and
    //  return that count to the caller.
    //
    m_c4CurPos += c4Written;

    return c4Written;
}


tCIDLib::TCard8 TChunkedOutStreamImpl::c8CurPos() const
{
    return m_c4CurPos;
}



tCIDLib::TVoid TChunkedOutStreamImpl::SeekToEnd()
{
    // Just set our position to the info object's reported current end
    m_c4CurPos = m_cptrInfo->c4CurEnd();
}


//
//  Reset our current position back to zero and truncate the current end of stream
//  in the shared info object back to zero.
//
tCIDLib::TVoid TChunkedOutStreamImpl::Reset()
{
    m_c4CurPos = 0;
    m_cptrInfo->TruncateAt(0);
}



// ---------------------------------------------------------------------------
//  TChunkedOutStreamImpl:: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TChunkedOutStreamImpl::c4CurSize() const
{
    // Just return our info object's current end position
    return m_cptrInfo->c4CurEnd();
}


tCIDLib::TCard4 TChunkedOutStreamImpl::c4MaxSize() const
{
    // Just pass on to the info object
    return m_cptrInfo->c4MaxSize();
}


tCIDLib::TCard4
TChunkedOutStreamImpl::c4CopyOutTo(         TMemBuf&        mbufTar
                                    , const tCIDLib::TCard4 c4StartAt) const
{
    // Pass it on to the info object
    return m_cptrInfo->c4CopyOutTo(mbufTar, c4StartAt);
}

tCIDLib::TCard4
TChunkedOutStreamImpl::c4CopyOutToStream(       TBinOutStream&  strmTar
                                        , const tCIDLib::TCard4 c4Count) const
{
    // Pass it on to the info object
    return m_cptrInfo->c4CopyOutToStream(strmTar, c4Count);
}


tCIDLib::TVoid TChunkedOutStreamImpl::TruncateAt(const tCIDLib::TCard4 c4At)
{
    // If this is beyond the current end of stream, this will throw
    m_cptrInfo->TruncateAt(c4At);

    // Reset our current position to zero
    m_c4CurPos = 0;
}





// ---------------------------------------------------------------------------
//   CLASS: TChunkedBinInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedBinInStream: Constructors and Destructor
// ---------------------------------------------------------------------------

// We only allow linked input streams
TChunkedBinInStream::TChunkedBinInStream(const TChunkedBinOutStream& strmSyncTo) :

    m_pstrmiIn(new TChunkedInStreamImpl(strmSyncTo.strmiThis()))
{
    AdoptImplObject(m_pstrmiIn);
}

TChunkedBinInStream::~TChunkedBinInStream()
{
}


// ---------------------------------------------------------------------------
//  TChunkedBinInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TChunkedBinInStream::c4CurSize() const
{
    return m_pstrmiIn->c4CurSize();
}


const TChunkedInStreamImpl& TChunkedBinInStream::strmiThis() const
{
    return *m_pstrmiIn;
}


tCIDLib::TVoid TChunkedBinInStream::SetEndIndex(const tCIDLib::TCard4 c4NewIndex)
{
    m_pstrmiIn->SetEndIndex(c4NewIndex);
}





// ---------------------------------------------------------------------------
//   CLASS: TChunkedBinOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedBinOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TChunkedBinOutStream::TChunkedBinOutStream(const tCIDLib::TCard4 c4MaxSize) :

    m_pstrmiOut(nullptr)
{
    // Make sure max size is not zero. If so, bump it up to at least one byte
    tCIDLib::TCard4 c4RealSz = c4MaxSize;
    if (!c4MaxSize)
        c4RealSz = 1;

    //
    //  Allocate our implementation object and give it to our parent. Keep a
    //  typed one for ourself.
    //
    m_pstrmiOut = new TChunkedOutStreamImpl(c4RealSz);
    AdoptImplObject(m_pstrmiOut);
}

TChunkedBinOutStream::~TChunkedBinOutStream()
{
}


// ---------------------------------------------------------------------------
//  TChunkedBinOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TChunkedBinOutStream::c4CurSize() const
{
    // Just pass these on to our implementation
    return m_pstrmiOut->c4CurSize();
}

tCIDLib::TCard4 TChunkedBinOutStream::c4MaxSize() const
{
    return m_pstrmiOut->c4MaxSize();
}

tCIDLib::TCard4
TChunkedBinOutStream::c4CopyOutTo(          TMemBuf&        mbufTar
                                    , const tCIDLib::TCard4 c4StartAt) const
{
    return m_pstrmiOut->c4CopyOutTo(mbufTar, c4StartAt);
}

tCIDLib::TCard4
TChunkedBinOutStream::c4CopyOutToStream(        TBinOutStream&  strmTar
                                        , const tCIDLib::TCard4 c4Count) const
{
    return m_pstrmiOut->c4CopyOutToStream(strmTar, c4Count);
}

const TChunkedOutStreamImpl& TChunkedBinOutStream::strmiThis() const
{
    return *m_pstrmiOut;
}


tCIDLib::TVoid TChunkedBinOutStream::TruncateAt(const tCIDLib::TCard4 c4At)
{
    //
    //  Flush any data first and then do the truncation. It could thrown if the stream
    //  is full and some is in the cache still, so catch that.
    //
    try
    {
        Flush();
    }

    catch(...)
    {
    }

    m_pstrmiOut->TruncateAt(c4At);
}





// ---------------------------------------------------------------------------
//   CLASS: TChunkedTextInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedTextInStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TChunkedTextInStream::
TChunkedTextInStream(const  TChunkedTextOutStream&  strmToSyncWith
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmIn(new TChunkedBinInStream(*strmToSyncWith.m_pstrmOut))
{
    AdoptStream(m_pstrmIn);
}


TChunkedTextInStream::~TChunkedTextInStream()
{
}

// ---------------------------------------------------------------------------
//  TChunkedTextInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TChunkedTextInStream::c4CurSize() const
{
    return m_pstrmIn->c4CurSize();
}


const TChunkedBinInStream& TChunkedTextInStream::strmIn() const
{
    return *m_pstrmIn;
}



// ---------------------------------------------------------------------------
//   CLASS: TChunkedTextOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TChunkedTextOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TChunkedTextOutStream::
TChunkedTextOutStream(  const   tCIDLib::TCard4         c4MaxSize
                        ,       TTextConverter* const   ptcvtToAdopt) :

    TTextOutStream(ptcvtToAdopt)
    , m_pstrmOut(nullptr)
{
    // Create a chunked binary stream and give it to our parent
    m_pstrmOut = new TChunkedBinOutStream(c4MaxSize);
    AdoptStream(m_pstrmOut);
}

TChunkedTextOutStream::~TChunkedTextOutStream()
{
}


// ---------------------------------------------------------------------------
//  TChunkedTextOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TChunkedTextOutStream::c4CurSize() const
{
    return m_pstrmOut->c4CurSize();
}


tCIDLib::TCard4 TChunkedTextOutStream::c4MaxSize() const
{
    return m_pstrmOut->c4MaxSize();
}


tCIDLib::TCard4
TChunkedTextOutStream::c4CopyOutTo(         TMemBuf&        mbufTar
                                    , const tCIDLib::TCard4 c4StartAt) const
{
    return m_pstrmOut->c4CopyOutTo(mbufTar, c4StartAt);
}


tCIDLib::TCard4
TChunkedTextOutStream::c4CopyOutToStream(       TBinOutStream&  strmTar
                                        , const tCIDLib::TCard4 c4Count) const
{
    return m_pstrmOut->c4CopyOutToStream(strmTar, c4Count);
}


const TChunkedBinOutStream& TChunkedTextOutStream::strmOut() const
{
    return *m_pstrmOut;
}

