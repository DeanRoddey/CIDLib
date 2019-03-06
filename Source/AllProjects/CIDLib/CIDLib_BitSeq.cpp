//
// FILE NAME: CIDLib_BitSet.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/26/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TBitSeq class.
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
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TBitSeq,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TBitSeq
//  PREFIX: bsq
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBitSeq: Constructors and Destructor
// ---------------------------------------------------------------------------
TBitSeq::TBitSeq(const tCIDLib::TCard4 c4InitAlloc) :

    m_c4CurAlloc(c4InitAlloc)
    , m_c4CurRBit(7)
    , m_c4CurRByte(0)
    , m_c4CurWBit(7)
    , m_c4CurWByte(0)
    , m_pc1Buffer(0)
{
    if (!m_c4CurAlloc)
        m_c4CurAlloc = 64;
    m_pc1Buffer = new tCIDLib::TCard1[m_c4CurAlloc];

    // And zero out the first byte, others will be zeroed as we hit them
    *m_pc1Buffer = 0;
}

TBitSeq::TBitSeq(const  tCIDLib::TCard1* const  pc1InitData
                , const tCIDLib::TCard4         c4Bytes) :

    m_c4CurAlloc(c4Bytes)
    , m_c4CurRBit(7)
    , m_c4CurRByte(0)
    , m_c4CurWBit(7)
    , m_c4CurWByte(c4Bytes)
    , m_pc1Buffer(0)
{
    m_pc1Buffer = new tCIDLib::TCard1[m_c4CurAlloc];
    TRawMem::CopyMemBuf(m_pc1Buffer, pc1InitData, m_c4CurAlloc);
}

TBitSeq::TBitSeq(const TBitSeq& bsqToCopy) :

    m_c4CurAlloc(bsqToCopy.m_c4CurAlloc)
    , m_c4CurRBit(bsqToCopy.m_c4CurRBit)
    , m_c4CurRByte(bsqToCopy.m_c4CurRByte)
    , m_c4CurWBit(bsqToCopy.m_c4CurWBit)
    , m_c4CurWByte(bsqToCopy.m_c4CurWByte)
    , m_pc1Buffer(0)
{
    // Allocate a buffer of the same size
    m_pc1Buffer = new tCIDLib::TCard1[m_c4CurAlloc];

    // Copy over the live bits
    TRawMem::CopyMemBuf(m_pc1Buffer, bsqToCopy.m_pc1Buffer, m_c4CurWByte + 1);
}

TBitSeq::~TBitSeq()
{
    // We own the buffer and have to clean it up
    delete [] m_pc1Buffer;
}


// ---------------------------------------------------------------------------
//  TBitSeq: Public operators
// ---------------------------------------------------------------------------
TBitSeq& TBitSeq::operator=(const TBitSeq& bsqToAssign)
{
    if (this == &bsqToAssign)
        return *this;

    // If is not the same size, then reallocate ours
    if (m_c4CurAlloc != bsqToAssign.m_c4CurAlloc)
    {
        delete [] m_pc1Buffer;
        m_c4CurAlloc = bsqToAssign.m_c4CurAlloc;
        m_pc1Buffer = new tCIDLib::TCard1[m_c4CurAlloc];
    }

    // Copy over current offset/bit info
    m_c4CurRBit     = bsqToAssign.m_c4CurRBit;
    m_c4CurRByte    = bsqToAssign.m_c4CurRByte;
    m_c4CurWBit     = bsqToAssign.m_c4CurWBit;
    m_c4CurWByte    = bsqToAssign.m_c4CurWByte;

    //
    //  Copy over the source buffer. We only need to copy over the bytes that
    //  have live data in them.
    //
    TRawMem::CopyMemBuf(m_pc1Buffer, bsqToAssign.m_pc1Buffer, m_c4CurWByte + 1);

    return *this;
}


tCIDLib::TBoolean TBitSeq::operator[](const tCIDLib::TCard4 c4BitIndex) const
{
    // Just delegate to another method
    return bAt(c4BitIndex);
}


//
//  Note that the buffer allocation size doesn't matter, since its an internal
//  implementation detail and expands as required. We just care about the actual
//  data written, the read offsets, and the data itself.
//
tCIDLib::TBoolean TBitSeq::operator==(const TBitSeq& bsqToCompare) const
{
    // Check for self comparison
    if (this == &bsqToCompare)
        return kCIDLib::True;

    if ((m_c4CurRBit != bsqToCompare.m_c4CurRBit)
    ||  (m_c4CurWBit != bsqToCompare.m_c4CurWBit)
    ||  (m_c4CurRByte != bsqToCompare.m_c4CurRByte)
    ||  (m_c4CurWByte != bsqToCompare.m_c4CurWByte))
    {
        return kCIDLib::False;
    }

    //
    //  Oh well, gotta compare the data buffers. We only care about the 'live'
    //  data in the buffers, i.e. the amount written so far.
    //
    const tCIDLib::ESortComps eRes = TRawMem::eCompareMemBuf
    (
        m_pc1Buffer
        , bsqToCompare.m_pc1Buffer
        , m_c4CurWByte + 1
    );
    return (eRes == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TBitSeq::operator!=(const TBitSeq& bsqToCompare) const
{
    return !operator==(bsqToCompare);
}



// ---------------------------------------------------------------------------
//  TBitSeq: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TBitSeq::AppendBits(const   tCIDLib::TCard4     c4Val
                    , const tCIDLib::TCard4     c4BitCount
                    , const tCIDLib::EBitSeqs   eMode)
{
    // If no bits, do nothing
    if (!c4BitCount)
        return;

    // Make sure its not more than 32 bits
    if (c4BitCount > 32)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitSeq_BadBitCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4BitCount)
            , TInteger(32)
        );
    }

    // Make sure we have storage for the new bits
    CheckWAvail(c4BitCount, eMode);

    if ((eMode == tCIDLib::EBitSeqs::Normal)
    ||  (eMode == tCIDLib::EBitSeqs::DupCompl))
    {
        DoAppend(c4Val, c4BitCount, tCIDLib::EBitSeqs::Normal);

        // If duplicated complement, then complement and write again
        if (eMode == tCIDLib::EBitSeqs::DupCompl)
        {
            const tCIDLib::TCard4 c4Tmp = c4Val ^ kCIDLib::c4MaxCard;
            DoAppend(c4Tmp, c4BitCount, tCIDLib::EBitSeqs::Normal);
        }
    }
     else if (eMode == tCIDLib::EBitSeqs::InterleavedCompl)
    {
        DoAppend(c4Val, c4BitCount, eMode);
    }
     else
    {
        // This is bad, don't know what the mode is
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal(tCIDLib::c4EnumOrd(eMode))
            , TString(L"tCIDLib::EBitSeqs")
        );
    }
}

tCIDLib::TVoid TBitSeq::AppendBits( const   TBitSeq&        bsqToAppend
                                    , const tCIDLib::TCard4 c4FromBit)
{
    //
    //  If the from bit is max card, then we take the current read off of
    //  the buffer.
    //
    const tCIDLib::TCard4 c4ActualFrom  = (c4FromBit == kCIDLib::c4MaxCard)
                                        ? bsqToAppend.c4ReadOfs() : c4FromBit;

    // Make sure the from bit is legal for the source sequence
    if (c4ActualFrom > bsqToAppend.c4WriteOfs())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4FromBit)
            , clsThis()
            , TCardinal(bsqToAppend.c4WriteOfs())
        );
    }

    // See how many bits we will actually append
    const tCIDLib::TCard4 c4BitsToAppend = bsqToAppend.c4WriteOfs() - c4ActualFrom;

    // Make the logic below simpler by checking for no source bits now
    if (!c4BitsToAppend)
        return;

    //
    //  Make sure we have storage for the new bits. We do this one manually
    //  since the private helper method normally used is designed for the up
    //  to 32 bits at a time scheme.
    //
    //  If our current bits written, plus the bits bits to append, is greater
    //  or equal to the max, then we have to expand.
    //
    if (c4WriteOfs() + c4BitsToAppend >= (m_c4CurAlloc << 3))
    {
        //
        //  Allocate a new buffer, bit enough plus some. Rounding down the bits
        //  will lose any trailing partial byte, but since we extra anyway,
        //  this doesn't matter.
        //
        const tCIDLib::TCard4 c4New = m_c4CurAlloc
                                      + (c4BitsToAppend >> 3)
                                      + 16;
        TArrayJanitor<tCIDLib::TCard1> janBuf(new tCIDLib::TCard1[c4New]);

        // Copy over our old data to our new buffer and delete the old one
        TRawMem::CopyMemBuf(janBuf.paThis(), m_pc1Buffer, m_c4CurAlloc);
        delete [] m_pc1Buffer;

        // And update our members for the new data
        m_pc1Buffer = janBuf.paOrphan();
        m_c4CurAlloc = c4New;
    }

    //
    //  <TBD> We could optimize the code below if our write bit position is
    //  byte aligned, and the source start bit is byte aligned.
    //

    //
    //  Calc the source byte and bit of the actual start bit. We'll start here
    //  and copy over bits to our sequence until we hit the c4BitsToAppend
    //  count.
    //
    tCIDLib::TCard4 c4SrcByte = c4ActualFrom / 8;
    tCIDLib::TCard4 c4SrcBit = 7 - (c4ActualFrom % 8);

    tCIDLib::TCard4 c4CopyCount = 0;
    while (c4CopyCount < c4BitsToAppend)
    {
        //
        //  If the current source bit is on, then OR a 1 bit into the correct
        //  bit of our sequence. If its not on, then we don't need to do
        //  anything, since we zero out each new target byte as we come to it,
        //  so all bits are already defaulted to zero.
        //
        if (bsqToAppend.m_pc1Buffer[c4SrcByte] & (0x1 << c4SrcBit))
            m_pc1Buffer[m_c4CurWByte] |= (0x1 << m_c4CurWBit);

        // Move both bit/byte counters to the next bit, wrapping as required
        if (!m_c4CurWBit)
        {
            m_c4CurWBit = 7;
            m_c4CurWByte++;

            // Zero the new target byte
            m_pc1Buffer[m_c4CurWByte] = 0;
        }
         else
        {
            m_c4CurWBit--;
        }

        if (!c4SrcBit)
        {
            c4SrcBit = 7;
            c4SrcByte++;
        }
         else
        {
            c4SrcBit--;
        }

        c4CopyCount++;
    }
}


tCIDLib::TBoolean TBitSeq::bAt(const tCIDLib::TCard4 c4BitIndex) const
{
    // Check it for legal index
    if (c4BitIndex >= c4WriteOfs())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4BitIndex)
            , clsThis()
            , TCardinal(c4WriteOfs())
        );
    }

    // Looks ok, so turn it into byte and bit offset
    const tCIDLib::TCard4 c4Byte = c4BitIndex / 8;
    const tCIDLib::TCard4 c4Bit  = 7 - (c4BitIndex % 8);

    return ((m_pc1Buffer[c4Byte] & (0x1 << c4Bit)) != 0);
}


tCIDLib::TBoolean TBitSeq::bSeqIsEqual( const   TBitSeq&        bsqToCompare
                                        , const tCIDLib::TCard4 c4ByteCount)
{
    // Get our actual byte count
    tCIDLib::TCard4 c4OurCount = m_c4CurWByte;
    if (m_c4CurWBit > 7)
        c4OurCount++;

    // And get the same for the source
    tCIDLib::TCard4 c4SrcCount = bsqToCompare.m_c4CurWByte;
    if (bsqToCompare.m_c4CurWBit > 7)
        c4SrcCount++;

    //
    //  Get the actual length. If the passed count is max card, then we
    //  do all the bytes of the source.
    //
    tCIDLib::TCard4 c4Actual = c4ByteCount;
    if (c4ByteCount == kCIDLib::c4MaxCard)
        c4Actual = c4SrcCount;

    // If either our or the source's length is less than this, we can't do it
    if ((c4OurCount < c4Actual) || (c4SrcCount < c4Actual))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitSeq_BadCompLen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Actual)
            , TCardinal(c4OurCount)
            , TCardinal(c4SrcCount)
        );
    }

    // Ok, compare bytes for that count
    return TRawMem::eCompareMemBuf
    (
        m_pc1Buffer
        , bsqToCompare.m_pc1Buffer
        , c4Actual
    ) == tCIDLib::ESortComps::Equal;
}


tCIDLib::TCard1 TBitSeq::c1At(const tCIDLib::TCard4 c4ByteIndex) const
{
    //
    //  Check it for legal index. It has to be within the range of the bytes
    //  that we've written so far. We let them get the last byte, even though
    //  it may not be fully filled.
    //
    tCIDLib::TCard4 c4Last = m_c4CurWByte;
    if (m_c4CurWBit < 7)
        c4Last++;

    if (c4ByteIndex >= c4Last)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4ByteIndex)
            , clsThis()
            , TCardinal(c4Last)
        );
    }

    // Looks ok, so return the indicated byte
    return m_pc1Buffer[c4ByteIndex];
}


tCIDLib::TCard4 TBitSeq::c4BytesWritten() const
{
    // If we are partly into the current byte, then include it
    if (m_c4CurWBit < 7)
        return m_c4CurWByte + 1;

    // Else don't
    return m_c4CurWByte;
}


tCIDLib::TCard4
TBitSeq::c4ExtractBits( const   tCIDLib::TCard4     c4BitCount
                        , const tCIDLib::EBitSeqs   eMode)
{
    // If no bits, do nothing
    if (!c4BitCount)
        return 0;

    // Make sure its not more than 32 bits
    if (c4BitCount > 32)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitSeq_BadBitCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4BitCount)
            , TInteger(32)
        );
    }

    // Make sure we have this many bits available
    CheckRAvail(c4BitCount, eMode);

    tCIDLib::TCard4 c4Ret = 0;

    if ((eMode == tCIDLib::EBitSeqs::Normal)
    ||  (eMode == tCIDLib::EBitSeqs::DupCompl))
    {
        // Extract the bits and throw away the complementary duplicate bits
        c4Ret = c4DoExtract(c4BitCount, tCIDLib::EBitSeqs::Normal);
        if (eMode == tCIDLib::EBitSeqs::DupCompl)
            c4DoExtract(c4BitCount, tCIDLib::EBitSeqs::Normal);
    }
     else if (eMode == tCIDLib::EBitSeqs::InterleavedCompl)
    {
        c4Ret = c4DoExtract(c4BitCount, eMode);
    }
     else
    {
        // This is bad, don't know what the mode is
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal(tCIDLib::c4EnumOrd(eMode))
            , TString(L"tCIDLib::EBitSeqs")
        );
    }

    return c4Ret;
}


tCIDLib::TCard4 TBitSeq::c4ReadOfs() const
{
    return (m_c4CurRByte << 3) + (7 - m_c4CurRBit);
}


tCIDLib::TCard4 TBitSeq::c4WriteOfs() const
{
    return (m_c4CurWByte << 3) + (7 - m_c4CurWBit);
}


tCIDLib::TVoid TBitSeq::CapAt(const tCIDLib::TCard4 c4BitCount)
{
    // Get the current write offset
    const tCIDLib::TCard4 c4CurWOfs = c4WriteOfs();

    //
    //  We allow it to be up to equal. If its equal, then it will basically
    //  have no effect since it would just cap it where it already is.
    //
    if (c4BitCount > c4WriteOfs())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitSeq_BadCapAt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4BitCount)
            , TCardinal(c4CurWOfs)
        );
    }

    m_c4CurWBit = 7 - (c4BitCount % 8);
    m_c4CurWByte = c4BitCount / 8;
}


const tCIDLib::TCard1* TBitSeq::pc1Buffer() const
{
    return m_pc1Buffer;
}


tCIDLib::TVoid TBitSeq::ReadReset(const tCIDLib::TCard4 c4AtBit)
{
    // If resetting to bit 0, then that's easy, else do more work
    if (!c4AtBit)
    {
        m_c4CurRBit  = 7;
        m_c4CurRByte = 0;
    }
     else
    {
        // Make sure its a legal bit
        if (c4AtBit >= c4WriteOfs())
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcBitSeq_BadReadReset
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Index
                , TCardinal(c4AtBit)
                , TCardinal(c4WriteOfs())
            );
        }

        // Looks ok, so convert to byte/bit and save
        m_c4CurRByte = c4AtBit / 8;
        m_c4CurRBit = 7 - (c4AtBit % 8);
    }
}


tCIDLib::TVoid TBitSeq::Reset()
{
    ReadReset(0);
    WriteReset();
}


tCIDLib::TVoid TBitSeq::SetAt(  const   tCIDLib::TCard4 c4ByteIndex
                                , const tCIDLib::TCard1 c1Value)
{
    //
    //  Check it for legal index. It has to be within the range of the bytes
    //  that we've written so far. We let them get the last byte, even though
    //  it may not be fully filled.
    //
    tCIDLib::TCard4 c4Last = m_c4CurWByte;
    if (m_c4CurWBit < 7)
        c4Last++;

    if (c4ByteIndex >= c4Last)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4ByteIndex)
            , clsThis()
            , TCardinal(c4Last)
        );
    }

    // Looks ok, so return the indicated byte
    m_pc1Buffer[c4ByteIndex] = c1Value;
}


tCIDLib::TVoid TBitSeq::WriteReset()
{
    // We have to reset the read indices in this case as well
    m_c4CurRBit  = 7;
    m_c4CurRByte = 0;
    m_c4CurWBit  = 7;
    m_c4CurWByte = 0;

    // Zero the first byte. We'll zero the others as we hit them
    *m_pc1Buffer = 0;
}


// ---------------------------------------------------------------------------
//  TBitSeq: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We assume that the public API that called us has already checked the bit
//  count for validity and that we have enough space to store them, though
//  we still check for overflow if in debug mode.
//
tCIDLib::TVoid
TBitSeq::DoAppend(  const   tCIDLib::TCard4     c4Val
                    , const tCIDLib::TCard4     c4BitCount
                    , const tCIDLib::EBitSeqs   eMode)
{
    tCIDLib::TCard4 c4Mask = 0x1U << (c4BitCount - 1);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BitCount; c4Index++)
    {
        // If debugging, make sure we don't overflow
        #if CID_DEBUG_ON
        if (m_c4CurWByte >= m_c4CurAlloc)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcBitSeq_WriteOverflow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Overflow
            );
        }
        #endif

        // Get a 1 or zero value for the current bit
        tCIDLib::TCard1 c1BitVal = (c4Mask & c4Val) ? 1 : 0;

        // Shift it to the current bit
        c1BitVal <<= m_c4CurWBit;

        // And mask it into the current byte
        m_pc1Buffer[m_c4CurWByte] |= c1BitVal;

        // Move up to the next bit, wrap if needed
        if (!m_c4CurWBit)
        {
            m_c4CurWBit = 7;
            m_c4CurWByte++;

            // Zero the new byte
            m_pc1Buffer[m_c4CurWByte] = 0;
        }
         else
        {
            m_c4CurWBit--;
        }

        // And now output the complemented version if needed
        if (eMode == tCIDLib::EBitSeqs::InterleavedCompl)
        {
            if (c1BitVal)
                c1BitVal = 0;
            else
                c1BitVal = 1;

            // Shift it to the current bit
            c1BitVal <<= m_c4CurWBit;

            // And mask it into the current byte
            m_pc1Buffer[m_c4CurWByte] |= c1BitVal;

            // Move up to the next bit, wrap if needed
            if (!m_c4CurWBit)
            {
                m_c4CurWBit = 7;
                m_c4CurWByte++;

                // Zero the new byte
                m_pc1Buffer[m_c4CurWByte] = 0;
            }
             else
            {
                m_c4CurWBit--;
            }
        }

        // And shift the mask for the next src bit
        c4Mask >>= 1;
    }

}


tCIDLib::TCard4
TBitSeq::c4DoExtract(const  tCIDLib::TCard4     c4BitCount
                    , const tCIDLib::EBitSeqs   eMode)
{
    tCIDLib::TCard4 c4Ret = 0;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BitCount; c4Index++)
    {
        //
        //  If debugging, make sure we don't overflow. In this case we have
        //  to check the read offset against the write offset.
        //
        #if CID_DEBUG_ON
        if (m_c4CurRByte > m_c4CurWByte)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcBitSeq_ReadOverflow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        #endif

        // Set up the mask for ths round
        const tCIDLib::TCard1 c1Mask = tCIDLib::TCard1(0x1U << m_c4CurRBit);

        //
        //  Check this bit in the buffer and use the result to correctly shift
        //  a bit into the return value.
        //
        c4Ret <<= 1;
        if (c1Mask & m_pc1Buffer[m_c4CurRByte])
            c4Ret |= 0x1U;

        // Move up to the next bit, wrap if needed
        if (!m_c4CurRBit)
        {
            m_c4CurRBit = 7;
            m_c4CurRByte++;
        }
         else
        {
            m_c4CurRBit--;
        }

        // If in any complementary mode, do it again to skip the compl bit
        if (eMode == tCIDLib::EBitSeqs::InterleavedCompl)
        {
            if (!m_c4CurRBit)
            {
                m_c4CurRBit = 7;
                m_c4CurRByte++;
            }
             else
            {
                m_c4CurRBit--;
            }
        }
    }
    return c4Ret;
}


tCIDLib::TVoid TBitSeq::CheckRAvail(const   tCIDLib::TCard4     c4BitCount
                                    , const tCIDLib::EBitSeqs   eMode)
{
    // Set up the full number of bits we have to work with
    const tCIDLib::TCard4 c4AvailBits = (m_c4CurWByte << 3)
                                      + (7 - m_c4CurWBit);

    // And get the number of bits we've read so far
    const tCIDLib::TCard4 c4SoFar = c4ReadOfs();

    // Calc the actual bit count, which is affected by the mode
    tCIDLib::TCard4 c4Actual = c4BitCount;
    if (eMode > tCIDLib::EBitSeqs::Normal)
        c4Actual <<= 1;

    // If not enough, then throw
    if (c4SoFar + c4Actual > c4AvailBits)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcBitSeq_NoMoreBits
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4Actual)
            , TCardinal(c4AvailBits - c4SoFar)
        );
    }
}


tCIDLib::TVoid TBitSeq::CheckWAvail(const   tCIDLib::TCard4     c4BitCount
                                    , const tCIDLib::EBitSeqs   eMode)
{
    // Set up the full number of bits we have to work with
    const tCIDLib::TCard4 c4MaxBits = m_c4CurAlloc << 3;

    // Calc the actual bit count, which is affected by the mode
    tCIDLib::TCard4 c4Actual = c4BitCount;
    if (eMode > tCIDLib::EBitSeqs::Normal)
        c4Actual <<= 1;

    // If not enough, then expand
    if (c4WriteOfs() + c4Actual >= c4MaxBits)
        Expand();
}


tCIDLib::TVoid TBitSeq::Expand()
{
    // Allocate a new buffer, expanded by 125 percent
    const tCIDLib::TCard4 c4New = tCIDLib::TCard4(m_c4CurAlloc * 1.25);
    TArrayJanitor<tCIDLib::TCard1> janBuf(new tCIDLib::TCard1[c4New]);

    // Copy over the old data to our new buffer and delete the old one
    TRawMem::CopyMemBuf(janBuf.paThis(), m_pc1Buffer, m_c4CurAlloc);
    delete [] m_pc1Buffer;

    // And update our members for the new data
    m_pc1Buffer = janBuf.paOrphan();
    m_c4CurAlloc = c4New;
}

