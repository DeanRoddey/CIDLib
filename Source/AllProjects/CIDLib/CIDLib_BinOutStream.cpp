//
// FILE NAME: CIDLib_BinOutStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1999
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
//  This file implements the binary output stream class. It just adds its own
//  constructors and the implementation of the data writing methods.
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
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TOutStreamImpl,TObject)
RTTIDecls(TBinOutStream,TObject)


// ---------------------------------------------------------------------------
//  TBinOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinOutStream::TBinOutStream(TOutStreamImpl* const pstrmiToAdopt) :

    m_c4CurIndex(0)
    , m_eEndianMode(tCIDLib::EEndianModes::Little)
    , m_pstrmiOut(pstrmiToAdopt)
{
}

TBinOutStream::~TBinOutStream()
{
    delete m_pstrmiOut;
}


// ---------------------------------------------------------------------------
//  TBinOutStream: Public operators
// ---------------------------------------------------------------------------
TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TBoolean bVal)
{
    //
    //  Boolean values are stored as single bytes, regardless of what the
    //  language defines them as. This saves space and avoids byte swapping
    //  issues altogether.
    //
    const tCIDLib::TCard1 c1Tmp = bVal ? 1 : 0;
    c4WriteRawBuffer(&c1Tmp, sizeof(c1Tmp));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TCard1 c1Val)
{
    c4WriteRawBuffer(&c1Val, sizeof(c1Val));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TCard2 c2Val)
{
    // Do byte swapping if required
    tCIDLib::TCard2 c2Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        c2Actual = TRawBits::c2SwapBytes(c2Val);
    else
        c2Actual = c2Val;

    c4WriteRawBuffer(&c2Actual, sizeof(c2Actual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TCard4 c4Val)
{
    // Do byte swapping if required
    tCIDLib::TCard4 c4Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        c4Actual = TRawBits::c4SwapBytes(c4Val);
    else
        c4Actual = c4Val;

    c4WriteRawBuffer(&c4Actual, sizeof(c4Actual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TCard8 c8Val)
{
    // Do byte swapping if required
    tCIDLib::TCard8 c8Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        c8Actual = TRawBits::c8SwapBytes(c8Val);
    else
        c8Actual = c8Val;

    c4WriteRawBuffer(&c8Actual, sizeof(c8Actual));
    return *this;
}


// These are forced to Card1 format, so no byte swapping issues
TBinOutStream& TBinOutStream::operator<<(const tCIDLib::EStreamMarkers eMarker)
{
    tCIDLib::TCard1 c1Tmp = static_cast<tCIDLib::TCard1>(eMarker);
    c4WriteRawBuffer(&c1Tmp, sizeof(c1Tmp));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TFloat4 f4Val)
{
    tCIDLib::TFloat4 f4Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        f4Actual = TRawBits::f4SwapBytes(f4Val, TSysInfo::bLittleEndian());
    else
        f4Actual = f4Val;

    c4WriteRawBuffer(&f4Actual, sizeof(f4Actual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TFloat8& f8Val)
{
    tCIDLib::TFloat8 f8Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        f8Actual = TRawBits::f8SwapBytes(f8Val, TSysInfo::bLittleEndian());
    else
        f8Actual = f8Val;

    c4WriteRawBuffer(&f8Actual, sizeof(f8Actual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TInt1 i1Val)
{
    c4WriteRawBuffer(&i1Val, sizeof(i1Val));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TInt2 i2Val)
{
    // Do byte swapping if required
    tCIDLib::TInt2 i2Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        i2Actual = TRawBits::i2SwapBytes(i2Val);
    else
        i2Actual = i2Val;

    c4WriteRawBuffer(&i2Actual, sizeof(i2Actual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TInt4 i4Val)
{
    // Do byte swapping if required
    tCIDLib::TInt4 i4Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        i4Actual = TRawBits::i4SwapBytes(i4Val);
    else
        i4Actual = i4Val;

    c4WriteRawBuffer(&i4Actual, sizeof(i4Actual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TInt8 i8Val)
{
    // Do byte swapping if required
    tCIDLib::TInt8 i8Actual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        i8Actual = TRawBits::i8SwapBytes(i8Val);
    else
        i8Actual = i8Val;

    c4WriteRawBuffer(&i8Actual, sizeof(i8Actual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TUInt uVal)
{
    // Do byte swapping if required
    tCIDLib::TUInt uActual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        uActual = TRawBits::c4SwapBytes(uVal);
    else
        uActual = uVal;

    c4WriteRawBuffer(&uActual, sizeof(uActual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TSInt iVal)
{
    // Do byte swapping if required
    tCIDLib::TSInt iActual;
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        iActual = TRawBits::i4SwapBytes(iVal);
    else
        iActual = iVal;

    c4WriteRawBuffer(&iActual, sizeof(iActual));
    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::EStrmSpecial eSpecial)
{
    // If its a flush, just do that one up front
    if (eSpecial == tCIDLib::EStrmSpecial::FlushIt)
    {
        Flush();
    }
    #if CID_DEBUG_ON
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_BadSpecial
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eSpecial))
        );
    }
    #endif

    return *this;
}

TBinOutStream& TBinOutStream::operator<<(const tCIDLib::TCh chToWrite)
{
    // Convert it to UTF-8
    tCIDLib::TCard1 ac1OutBuf[kCIDLib::c4MaxUTF8Bytes];
    const tCIDLib::TCard4 c4Bytes = m_tcvtText.c4ConvertOneTo
    (
        chToWrite, ac1OutBuf, kCIDLib::c4MaxUTF8Bytes
    );

    if (!c4Bytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_ConvertChOut
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Write out the number of bytes we flattened first, then the data
    const tCIDLib::TCard1 c1Count = tCIDLib::TCard1(c4Bytes);
    c4WriteRawBuffer(&c1Count, 1);
    c4WriteRawBuffer(ac1OutBuf, c4Bytes);
    return *this;
}



// ---------------------------------------------------------------------------
//  TBinOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TBinOutStream::bIsOpen() const
{
    if (m_pstrmiOut)
        return m_pstrmiOut->bIsOpen();
    return kCIDLib::False;
}


//
//  This is a convenience. Most apps will never get even close to exceeding
//  the 4GB size on a single file, and will find it much more convenient to
//  deal with positions in terms of TCard4 values. So we check it and if it
//  will fit, give it back to them.
//
tCIDLib::TCard4 TBinOutStream::c4CurPos() const
{
    //
    //  We get the current raw position and add our current index into the
    //  cache to get the real offset.
    //
    const tCIDLib::TCard8 c8Tmp = m_pstrmiOut->c8CurPos() + m_c4CurIndex;

    // If it won't fit, we can't do it
    if (c8Tmp > kCIDLib::c4MaxCard)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_PosOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Overflow
        );
    }
    return tCIDLib::TCard4(c8Tmp);
}

tCIDLib::TCard8 TBinOutStream::c8CurPos() const
{
    // Get the current external position and add the current cache index
    return m_pstrmiOut->c8CurPos() + m_c4CurIndex;
}


tCIDLib::TVoid TBinOutStream::Flush()
{
    // If nothing in the cache, then do nothing
    if (!m_c4CurIndex)
        return;

    // Try to write the current cache contents
    const tCIDLib::TCard4 c4Flushed = m_pstrmiOut->c4WriteBytes
    (
        m_ac1Cache, m_c4CurIndex
    );

    // If we wrote it all, then return now and zero the current index
    if (c4Flushed == m_c4CurIndex)
    {
        m_c4CurIndex = 0;
        return;
    }

    // If not, clear the rest and throw
    m_c4CurIndex = 0;
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcStrm_PartialCacheWrite
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
        , TCardinal(c4Flushed)
    );
}


tCIDLib::TCard4
TBinOutStream::c4WriteBuffer(const  TMemBuf&            mbufSrc
                            , const tCIDLib::TCard4     c4Count
                            , const tCIDLib::EAllData   eAllData)
{
    // Just call the raw version
    return c4WriteRawBuffer(mbufSrc.pData(), c4Count, eAllData);
}


tCIDLib::TCard4
TBinOutStream::c4WriteRawBuffer(const   tCIDLib::TVoid* const   pBufToUse
                                , const tCIDLib::TCard4         c4Count
                                , const tCIDLib::EAllData       eAllData)
{
    //
    //  Would the new data to write more than fill up the buffer? If so, then
    //  write what's in the buffer first. Don't count this as data we've written
    //  on this round.
    //
    tCIDLib::TCard4 c4Written = 0;
    if (c4CacheSpaceAvail() < c4Count)
    {
        // Flush the current cache data
        Flush();

        //
        //  It went ok. If the data to write is bigger than half the cache
        //  size, then just write it directly. Else, copy it to the catch.
        //
        if (c4Count > (c4CacheSz / 2))
        {
            c4Written = m_pstrmiOut->c4WriteBytes
            (
                reinterpret_cast<const tCIDLib::TCard1*>(pBufToUse)
                , c4Count
            );
        }
         else
        {
            TRawMem::CopyMemBuf(&m_ac1Cache[m_c4CurIndex], pBufToUse, c4Count);

            // Bump the cache index and set the written to count
            m_c4CurIndex += c4Count;
            c4Written = c4Count;
        }
    }
     else
    {
        // It will fit into the cache so just stick it in there
        TRawMem::CopyMemBuf(&m_ac1Cache[m_c4CurIndex], pBufToUse, c4Count);

        // Bump the cache index and set the written to count
        m_c4CurIndex += c4Count;
        c4Written = c4Count;
    }

    //
    //  If the bytes written is not equal to the bytes requested and the all
    //  data parameter indicates all bytes must be written, then throw. If we
    //  get zero bytes, then throw the end of stream error.
    //
    if ((c4Written != c4Count) && (eAllData == tCIDLib::EAllData::FailIfNotAll))
    {
        if (c4Written)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_NotAllData
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::OutResource
                , TCardinal(c4Written)
                , TCardinal(c4Count)
            );
        }
         else
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_EndOfStream
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::OutResource
            );
        }
    }
    return c4Written;
}


//
//  Write the content of one stream to this one. We just reset it, then loop through,
//  reading chunks at a time and calling the raw buffer write method above to write
//  each one out.
//
//  We assume it must all be writeable.
//
tCIDLib::TCard4 TBinOutStream::c4WriteStream(TBinInStream& strmSrc)
{
    const tCIDLib::TCard4 c4ChunkSz(32 * 1024);
    THeapBuf mbufTmp(c4ChunkSz, c4ChunkSz);

    tCIDLib::TCard4 c4SoFar = 0;
    strmSrc.Reset();
    while (!strmSrc.bEndOfStream())
    {
        const tCIDLib::TCard4 c4ThisTime = strmSrc.c4ReadBuffer
        (
            mbufTmp, c4ChunkSz, tCIDLib::EAllData::OkIfNotAll
        );

        c4WriteRawBuffer(mbufTmp.pc1Data(), c4ThisTime, tCIDLib::EAllData::FailIfNotAll);
        c4SoFar += c4ThisTime;
    }

    // We had to have written all of the src bytes
    return c4SoFar;
}


// Set the endian mode. Getting is inlined
tCIDLib::EEndianModes TBinOutStream::eEndianMode(const tCIDLib::EEndianModes eToSet)
{
    m_eEndianMode = eToSet;
    return m_eEndianMode;
}


tCIDLib::TVoid TBinOutStream::Reset()
{
    // Empty the cache first, then reset the underlying impl
    m_c4CurIndex = 0;
    m_pstrmiOut->Reset();
}


tCIDLib::TVoid TBinOutStream::SeekToEnd()
{
    // Ask the impl object to do this, but flush the cache first
    Flush();
    m_pstrmiOut->SeekToEnd();
}


//
//  These all handle writing arrays of values to the stream. They will byte swap
//  all entries if required. If swapping, they have to use a local buffer to swap
//  the values, so they do it in chunks to avoid having to possibly create a really
//  bit temp buffer.
//
//  WE ASSUME the passed data is in system endianness format! So we swap if the
//  stream's format is not the same as the system.
//
//  They don't write out any element count or anything. These are assumed to be
//  raw arrays. The caller must do any of that sort of thing required to know how
//  to read it back in. In some cases it may be a fixed value so there's no need
//  for any extra info.
//
tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TBoolean* const    abList
                            , const tCIDLib::TCard4             c4Count)
{
    // Check for silliness
    if (!c4Count)
        return;

    //
    //  We cannot make any assumptions about the local format of booleans,
    //  so they are written out as 1 or 0 value bytes as a canonical format.
    //  This means we cannot just write them directly from the caller's
    //  buffer.
    //
    const tCIDLib::TCard4 c4Buf = 128;
    tCIDLib::TCard1 ac1Tmp[c4Buf];
    tCIDLib::TCard1* pc1Buf;
    tCIDLib::TCard1* pc1End;

    // Get a pointer we can move up through the caller's array
    const tCIDLib::TBoolean* pbSrc = abList;

    // Set up a countdown counter, to keep up with now many are left
    tCIDLib::TCard4 c4ToGo = c4Count;
    while (c4ToGo)
    {
        //
        //  See how many to try to read this time, and try to read that many.
        //  We either do a full local buffer's worth, or a final partial
        //  buffer's worth.
        //
        tCIDLib::TCard4 c4ThisTime = (c4ToGo > c4Buf) ? c4Buf : c4ToGo;

        // Convert that many values to one or zero values in the temp array
        pc1Buf = ac1Tmp;
        pc1End = pc1Buf + c4ThisTime;
        while (pc1Buf < pc1End)
        {
            *pc1Buf++ = *pbSrc ? 1 : 0;
            pbSrc++;
        }

        // And try to write those out
        c4WriteRawBuffer(ac1Tmp, c4ThisTime);

        // And count down by the number we did this time
        c4ToGo -= c4ThisTime;
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TCh* const pszToWrite
                            , const tCIDLib::TCard4     c4Len)
{
    // Check for silliness
    if (!c4Len)
        return;

    //
    //  We convert it to UTF-8 in order to minimize the space required for storage
    //  and to avoid byte order issues.
    //
    const tCIDLib::TCard4 c4BufSz = 255;
    tCIDLib::TCard1 ac1Buf[c4BufSz + 1];

    tCIDLib::TCard4 c4SrcDone;
    tCIDLib::TCard4 c4OutBytes;
    tCIDLib::TCard4 c4Total = 0;
    tCIDLib::TCard2 c2ChunkInd = 1;

    // Create the converter if not created yet

    // Loop until we do all of our source chars
    while (c4Total < c4Len)
    {
        //
        //  Convert the next chunk worth, starting where we left off last
        //  time. We ask to do up to all the chars left to do, but it'll
        //  only do as much as can fit in the target buffer.
        //
        c4SrcDone = m_tcvtText.c4ConvertTo
        (
            &pszToWrite[c4Total], c4Len - c4Total, ac1Buf, c4BufSz, c4OutBytes
        );

        if (!c4SrcDone || !c4OutBytes)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_ConvertChOut
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // Add the source bytes we did this time to the total
        c4Total += c4SrcDone;

        //
        //  Write these bytes to the stream. We put out a chunk id, then
        //  the count of bytes in the chunk (reduced to a TCard1 since it
        //  cannot be more than 255 bytes), and then the raw bytes. The
        //  default is to throw if not all written, so we don't have to
        //  check the return.
        //
        *this << c2ChunkInd << tCIDLib::TCard1(c4OutBytes);
        c4WriteRawBuffer(ac1Buf, c4OutBytes);

        // Bump the chunk index for the next time around
        c2ChunkInd++;
    }

    // Write out a chunk id of c2MaxCard to mark the end
    *this << kCIDLib::c2MaxCard;
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TCard1* const  ac1List
                            , const tCIDLib::TCard4         c4Count)
{
    // Check for silliness
    if (!c4Count)
        return;

    c4WriteRawBuffer(ac1List, c4Count);
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TCard2* const  ac2List
                            , const tCIDLib::TCard4         c4Count)
{
    // Check for silliness
    if (!c4Count)
        return;

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TCard2 ac2Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            ac2Chunk[c4ChunkInd] = TRawBits::c2SwapBytes(ac2List[c4Index]);
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                c4WriteRawBuffer(ac2Chunk, sizeof(tCIDLib::TCard2) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
            c4WriteRawBuffer(ac2Chunk, sizeof(tCIDLib::TCard2) * c4ChunkInd);
    }
     else
    {
        c4WriteRawBuffer(ac2List, sizeof(tCIDLib::TCard2) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TCard4* const  ac4List
                            , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TCard4 ac4Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            ac4Chunk[c4ChunkInd] = ac4List[c4Index];
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                // We can bulk swap these
                TRawBits::SwapCard4Array(ac4Chunk, c4ChunkSz);
                c4WriteRawBuffer(ac4Chunk, sizeof(tCIDLib::TCard4) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
        {
            TRawBits::SwapCard4Array(ac4Chunk, c4ChunkInd);
            c4WriteRawBuffer(ac4Chunk, sizeof(tCIDLib::TCard4) * c4ChunkInd);
        }
    }
     else
    {
        c4WriteRawBuffer(ac4List, sizeof(tCIDLib::TCard4) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TCard8* const  ac8List
                            , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TCard8 ac8Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            ac8Chunk[c4ChunkInd] = TRawBits::c8SwapBytes(ac8List[c4Index]);
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                c4WriteRawBuffer(ac8Chunk, sizeof(tCIDLib::TCard8) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
            c4WriteRawBuffer(ac8Chunk, sizeof(tCIDLib::TCard8) * c4ChunkInd);
    }
    else
    {
        c4WriteRawBuffer(ac8List, sizeof(tCIDLib::TCard8) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TInt1* const   ai1List
                            , const tCIDLib::TCard4         c4Count)
{
    // Check for silliness
    if (!c4Count)
        return;

    c4WriteRawBuffer(ai1List, c4Count);
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TInt2*  const  ai2List
                            , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TInt2 ai2Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            ai2Chunk[c4ChunkInd] = TRawBits::i2SwapBytes(ai2List[c4Index]);
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                c4WriteRawBuffer(ai2Chunk, sizeof(tCIDLib::TInt2) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
            c4WriteRawBuffer(ai2Chunk, sizeof(tCIDLib::TInt2) * c4ChunkInd);
    }
     else
    {
        c4WriteRawBuffer(ai2List, sizeof(tCIDLib::TInt2) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TInt4* const   ai4List
                            , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TInt4 ai4Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            ai4Chunk[c4ChunkInd] = ai4List[c4Index];
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                // We can bulk swap these
                TRawBits::SwapInt4Array(ai4Chunk, c4ChunkSz);
                c4WriteRawBuffer(ai4Chunk, sizeof(tCIDLib::TInt4) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
        {
            TRawBits::SwapInt4Array(ai4Chunk, c4ChunkInd);
            c4WriteRawBuffer(ai4Chunk, sizeof(tCIDLib::TInt4) * c4ChunkInd);
        }
    }
     else
    {
        c4WriteRawBuffer(ai4List, sizeof(tCIDLib::TInt4) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TInt8* const   ai8List
                            , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TInt8 ai8Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            ai8Chunk[c4ChunkInd] = TRawBits::i8SwapBytes(ai8List[c4Index]);
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                c4WriteRawBuffer(ai8Chunk, sizeof(tCIDLib::TInt8) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
            c4WriteRawBuffer(ai8Chunk, sizeof(tCIDLib::TInt8) * c4ChunkInd);
    }
     else
    {
        c4WriteRawBuffer(ai8List, sizeof(tCIDLib::TInt8) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TFloat4* const af4List
                            , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;

    const tCIDLib::TBoolean bLittle = TSysInfo::bLittleEndian();
    if (bLittleEndian() != bLittle)
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TFloat4 af4Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            af4Chunk[c4ChunkInd] = TRawBits::f4SwapBytes(af4List[c4Index], bLittle);
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                c4WriteRawBuffer(af4Chunk, sizeof(tCIDLib::TFloat4) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
            c4WriteRawBuffer(af4Chunk, sizeof(tCIDLib::TFloat4) * c4ChunkInd);
    }
     else
    {
        c4WriteRawBuffer(af4List, sizeof(tCIDLib::TFloat4) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteArray(  const   tCIDLib::TFloat8* const af8List
                            , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;

    const tCIDLib::TBoolean bLittle = TSysInfo::bLittleEndian();
    if (bLittleEndian() != bLittle)
    {
        const tCIDLib::TCard4 c4ChunkSz = 128;
        tCIDLib::TFloat8 af8Chunk[c4ChunkSz];

        tCIDLib::TCard4 c4ChunkInd = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            af8Chunk[c4ChunkInd] = TRawBits::f8SwapBytes(af8List[c4Index], bLittle);
            c4ChunkInd++;

            // Its time to write this full chunk
            if (c4ChunkInd == c4ChunkSz)
            {
                c4WriteRawBuffer(af8Chunk, sizeof(tCIDLib::TFloat8) * c4ChunkSz);
                c4ChunkInd = 0;
            }
        }

        // If there is a partial chunk left, write it out
        if (c4ChunkInd)
            c4WriteRawBuffer(af8Chunk, sizeof(tCIDLib::TFloat8) * c4ChunkInd);
    }
     else
    {
        c4WriteRawBuffer(af8List, sizeof(tCIDLib::TFloat8) * c4Count);
    }
}


tCIDLib::TVoid
TBinOutStream::WriteClassInfo(const TClass& clsToWrite)
{
    // Insure that the class name is valid
    if (!clsToWrite.bIsNameValid())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_InvalidName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Write out the typename stream marker. No byte swapping issues for these
    *this << tCIDLib::EStreamMarkers::TypeName;

    // Now get the length of the type name text
    tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(clsToWrite.pszClassName());

    // If debugging, check the length
    CIDAssert(c4Len <= kCIDLib::c1MaxCard, L"The class info name is too long");

    //
    //  Write out the length of the name, also in a byte. We know this
    //  is legal cause we checked the value above.
    //
    *this << tCIDLib::TCard1(c4Len);

    //
    //  And write out the characters of the class name. This will convert it
    //  to UTF-8, which is canonical and efficient for our needs.
    //
    TBinOutStream_WriteArray(*this, clsToWrite.pszClassName(), c4Len);

    //
    //  Now write out the 1st byte of the hash value. We know that its
    //  never greater than the modulus, which will fit easily in one byte.
    //
    tCIDLib::TCard1 c1Hash = tCIDLib::TCard1(clsToWrite.hshInternal());
    *this << c1Hash;
}


tCIDLib::TVoid TBinOutStream::WriteEnum(const tCIDLib::TCard4 c4Enum)
{
    *this << c4Enum;
}



// ---------------------------------------------------------------------------
//  TBinOutStream: Hidden Constructors
// ---------------------------------------------------------------------------
TBinOutStream::TBinOutStream() :

    m_c4CurIndex(0)
    , m_eEndianMode(tCIDLib::EEndianModes::Little)
    , m_pstrmiOut(nullptr)
{
}



// ---------------------------------------------------------------------------
//  TBinOutStream: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TBinOutStream::AdoptImplObject(TOutStreamImpl* const pstrmiToAdopt)
{
    // Make sure its not already set
    if (m_pstrmiOut)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_ImplAlreadySet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , clsIsA()
        );
    }
    m_pstrmiOut = pstrmiToAdopt;
}


tCIDLib::TCard4 TBinOutStream::c4CacheSpaceAvail() const
{
    return (c4CacheSz - m_c4CurIndex);
}


tCIDLib::TVoid TBinOutStream::DeleteImplObject()
{
    if (m_pstrmiOut)
    {
        delete m_pstrmiOut;
        m_pstrmiOut = 0;
    }
}


