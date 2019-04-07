//
// FILE NAME: CIDLib_Memory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/14/1993
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
//  This method implements the basic memory buffer management classes.
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
RTTIDecls(TMemBuf,TObject)




// ---------------------------------------------------------------------------
//   CLASS: TMemBuf
//  PREFIX: mbuf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMemBuf: Public, static methods
// ---------------------------------------------------------------------------
TMemBuf& TMemBuf::Nul_TMemBuf()
{
    static TMemBuf* pmbufNull = 0;
    if (!pmbufNull)
    {
        TBaseLock lockInit;
        if (!pmbufNull)
        {
            TRawMem::pExchangePtr
            (
                &pmbufNull, static_cast<TMemBuf*>(new THeapBuf(1))
            );
        }
    }
    return *pmbufNull;
}


// ---------------------------------------------------------------------------
//  TMemBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TMemBuf::~TMemBuf()
{
}


// ---------------------------------------------------------------------------
//  TMemBuf: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TCard1& TMemBuf::operator[](const tCIDLib::TCard4 c4Ind)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Ind);
    return pc1Buf[c4Ind];
}

tCIDLib::TCard1 TMemBuf::operator[](const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Ind);
    return pc1Buf[c4Ind];
}


tCIDLib::TBoolean TMemBuf::operator==(const TMemBuf& mbufToTest) const
{
    if (this == &mbufToTest)
        return kCIDLib::True;

    const tCIDLib::TCard4 c4OurSize = c4Size();
    const tCIDLib::TCard4 c4TheirSize = mbufToTest.c4Size();

    if ((c4OurSize == c4TheirSize)
    &&  TRawMem::bCompareMemBuf(pc1Data(), mbufToTest.pc1Data(), c4OurSize))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

tCIDLib::TBoolean TMemBuf::operator!=(const TMemBuf& mbufToTest) const
{
    return !operator==(mbufToTest);
}


// ---------------------------------------------------------------------------
//  TMemBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMemBuf::AppendFrom( const   TMemBuf&        mbufSrc
                                    , const tCIDLib::TCard4 c4AppendAt
                                    , const tCIDLib::TCard4 c4SrcBytes)
{
    if (!c4SrcBytes)
        return;

    // Check our buffer to append the indicated bytes at the index given
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE
        , c4AppendAt
        , c4SrcBytes
    );

    //
    //  Verify that the source buffer has the bytes indicated, and then copy
    //  them over.
    //
    const tCIDLib::TCard1* pc1SrcBuf = mbufSrc.pc1CheckRange(CID_LINE, 0, c4SrcBytes);
    TRawMem::CopyMemBuf(&pc1Buf[c4AppendAt], pc1SrcBuf, c4SrcBytes);
}


tCIDLib::TBoolean TMemBuf::bAt(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TBoolean)
    );
    return *reinterpret_cast<const tCIDLib::TBoolean*>(&pc1Buf[c4Ind]);
}


//
//  Convenience methods that just call the more general ones. THese just compare
//  the buffer contents. For the first they can pass max card and we'll take the
//  incoming buffer's current size.
//
tCIDLib::TBoolean
TMemBuf::bCompare(const TMemBuf& mbufToComp, const tCIDLib::TCard4 c4Count) const
{
    const tCIDLib::TCard4 c4RealCnt
    (
        c4Count == kCIDLib::c4MaxCard ? mbufToComp.c4Size() : c4Count
    );

    return eCompare(mbufToComp, c4RealCnt) == tCIDLib::ESortComps::Equal;
}

tCIDLib::TBoolean
TMemBuf::bCompare(  const tCIDLib::TCard1* const    pc1ToComp
                    , const tCIDLib::TCard4         c4Count) const
{
    return eCompare(pc1ToComp, c4Count) == tCIDLib::ESortComps::Equal;
}


tCIDLib::TCard1 TMemBuf::c1At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Ind);
    return pc1Buf[c4Ind];
}


tCIDLib::TCard2 TMemBuf::c2At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TCard2)
    );
    return *reinterpret_cast<const tCIDLib::TCard2*>(&pc1Buf[c4Ind]);
}


tCIDLib::TCard4 TMemBuf::c4At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TCard4)
    );
    return *reinterpret_cast<const tCIDLib::TCard4*>(&pc1Buf[c4Ind]);
}


// Calculate a standard check sum on the whole buffer
tCIDLib::TCard4 TMemBuf::c4CheckSum() const
{
    tCIDLib::TCard4 c4MaxSize;
    tCIDLib::TCard4 c4CurSize;
    const tCIDLib::TCard1* pc1Buf= pc1QueryBufInfo(c4CurSize, c4MaxSize);

    // Calc the checksum
    tCIDLib::TCard4 c4Sum = 0;
    for (tCIDLib::TCard4 c4Tmp = 0; c4Tmp < c4CurSize; c4Tmp++)
    {
        c4Sum += *pc1Buf;
        pc1Buf++;
    }
    return c4Sum;
}

// Calculate a checksum on the buffer from a start index to the end
tCIDLib::TCard4 TMemBuf::c4CheckSum(const tCIDLib::TCard4 c4StartInd) const
{
    // Check the index and get the buffer info
    tCIDLib::TCard4         c4CurSize;
    const tCIDLib::TCard1*  pc1Buf = pc1CheckIndex
    (
        CID_LINE, c4StartInd, c4CurSize
    );

    // Calc the checksum
    tCIDLib::TCard4 c4Sum = 0;
    for (tCIDLib::TCard4 c4Tmp = 0; c4Tmp < c4CurSize; c4Tmp++)
    {
        c4Sum += *pc1Buf;
        pc1Buf++;
    }
    return c4Sum;
}


// Calculate a checksum on a range of the buffer
tCIDLib::TCard4
TMemBuf::c4CheckSum(const   tCIDLib::TCard4 c4StartInd
                    , const tCIDLib::TCard4 c4Count) const
{
    // Check the range and get the buffer info
    tCIDLib::TCard4         c4CurSize;
    const tCIDLib::TCard1*  pc1Buf = pc1CheckRange
    (
        CID_LINE, c4StartInd, c4Count, c4CurSize
    );

    // Calc the checksum
    tCIDLib::TCard4 c4Sum   = 0;
    pc1Buf  = &pc1Buf[c4StartInd];

    const tCIDLib::TCard4 c4End = c4StartInd + c4Count;
    for (tCIDLib::TCard4 c4Tmp = c4StartInd; c4Tmp < c4End; c4Tmp++)
    {
        c4Sum += *pc1Buf;
        pc1Buf++;
    }
    return c4Sum;
}


// Find the index of the indicated byte, starting at a given index
tCIDLib::TCard4
TMemBuf::c4FindByte(const   tCIDLib::TCard4 c4StartInd
                    , const tCIDLib::TCard1 c1ToFind) const
{
    // Check the index and get the buffer info
    tCIDLib::TCard4         c4CurSize;
    const tCIDLib::TCard1*  pc1Buf = pc1CheckIndex
    (
        CID_LINE, c4StartInd, c4CurSize
    );

    for (tCIDLib::TCard4 c4Index = c4StartInd; c4Index < c4CurSize; c4Index++)
    {
        if (pc1Buf[c4Index] == c1ToFind)
            return c4Index;
    }
    return kCIDLib::c4MaxCard;
}


// Return the maximum allocation size of the buffer
tCIDLib::TCard4 TMemBuf::c4MaxSize() const
{
    tCIDLib::TCard4 c4CurSize;
    tCIDLib::TCard4 c4MaxSize;
    pc1QueryBufInfo(c4CurSize, c4MaxSize);
    return c4MaxSize;
}


// Return the current allocation side of the buffer
tCIDLib::TCard4 TMemBuf::c4Size() const
{
    tCIDLib::TCard4 c4CurSize;
    tCIDLib::TCard4 c4MaxSize;
    pc1QueryBufInfo(c4CurSize, c4MaxSize);
    return c4CurSize;
}


// Extract a Card8 value at the indicated index
tCIDLib::TCard8 TMemBuf::c8At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TCard8)
    );
    return *reinterpret_cast<const tCIDLib::TCard8*>(&pc1Buf[c4Ind]);
}


// Chet a unicode char at the indicated index
tCIDLib::TCh TMemBuf::chAt(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, kCIDLib::c4CharBytes
    );
    return *reinterpret_cast<const tCIDLib::TCh*>(&pc1Buf[c4Ind]);
}


// Copy in data from the passed buffer to this buffer
tCIDLib::TVoid
TMemBuf::CopyIn(const   tCIDLib::TVoid* const   pToCopy
                , const tCIDLib::TCard4         c4DataSz
                , const tCIDLib::TCard4         c4StartInd)
{
    if (!c4DataSz)
        return;

    tCIDLib::TCard1* pc1Buf = pc1CheckRange(CID_LINE, c4StartInd, c4DataSz);
    TRawMem::CopyMemBuf(&pc1Buf[c4StartInd], pToCopy, c4DataSz);
}

tCIDLib::TVoid TMemBuf::CopyIn( const   TMemBuf&        mbufToCopy
                                , const tCIDLib::TCard4 c4DataSz
                                , const tCIDLib::TCard4 c4StartInd)
{
    //
    //  Check this buffer and the source one. For the source, just make
    //  sure it has at least the indicated size otherwise we cannot copy
    //  that many bytes out of it.
    //
    const tCIDLib::TCard1* pc1SrcBuf = mbufToCopy.pc1CheckRange(CID_LINE, 0, c4DataSz);
    tCIDLib::TCard1* pc1TarBuf = pc1CheckRange(CID_LINE, c4StartInd, c4DataSz);
    TRawMem::CopyMemBuf(&pc1TarBuf[c4StartInd], pc1SrcBuf, c4DataSz);
}


// Copy out data from this buffer to a passed buffer
tCIDLib::TVoid
TMemBuf::CopyOut(           tCIDLib::TVoid* const   pToFill
                    , const tCIDLib::TCard4         c4DataSz
                    , const tCIDLib::TCard4         c4StartInd) const
{
    if (!c4DataSz)
        return;

    const tCIDLib::TCard1* pc1Buf = pc1CheckRange(CID_LINE, c4StartInd, c4DataSz);
    TRawMem::CopyMemBuf(pToFill, &pc1Buf[c4StartInd], c4DataSz);
}

tCIDLib::TVoid
TMemBuf::CopyOut(           TMemBuf&        mbufToFill
                    , const tCIDLib::TCard4 c4DataSz
                    , const tCIDLib::TCard4 c4StartInd) const
{
    // Check both buffers, the target just needs to be at least large enough
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange(CID_LINE, c4StartInd, c4DataSz);
    tCIDLib::TCard1* pc1TarBuf = mbufToFill.pc1CheckRange(CID_LINE, 0, c4DataSz);

    TRawMem::CopyMemBuf(pc1TarBuf, &pc1Buf[c4StartInd], c4DataSz);
}


//
//  Compare c4Count bytes of this buffer to a passed buffer. They both have
//  to be at least that large.
//
tCIDLib::ESortComps TMemBuf::eCompare(  const   TMemBuf&        mbufToComp
                                        , const tCIDLib::TCard4 c4Count) const
{
    // Check both buffers, the target just needs to be at least large enough
    const tCIDLib::TCard1* pc1This = pc1CheckRange(CID_LINE, 0, c4Count);
    const tCIDLib::TCard1* pc1That = mbufToComp.pc1CheckRange(CID_LINE, 0, c4Count);
    return TRawMem::eCompareMemBuf(pc1This, pc1That, c4Count);
}

tCIDLib::ESortComps
TMemBuf::eCompare(  const   tCIDLib::TCard1* const  pc1ToComp
                    , const tCIDLib::TCard4         c4Count) const
{
    // Check both buffers, the target just needs to be at least large enough
    const tCIDLib::TCard1* pc1This = pc1CheckRange(CID_LINE, 0, c4Count);
    return TRawMem::eCompareMemBuf(pc1This, pc1ToComp, c4Count);
}


// Extra a floating point value from the indicated index
tCIDLib::TFloat4 TMemBuf::f4At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TFloat4)
    );
    return *reinterpret_cast<const tCIDLib::TFloat4*>(&pc1Buf[c4Ind]);
}

tCIDLib::TFloat8 TMemBuf::f8At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TFloat8)
    );
    return *reinterpret_cast<const tCIDLib::TFloat8*>(&pc1Buf[c4Ind]);
}


// Hash the buffer starting at the indicated index
tCIDLib::THashVal
TMemBuf::hshCalcHash(   const   tCIDLib::TCard4 c4Modulus
                        , const tCIDLib::TCard4 c4StartInd) const
{
    tCIDLib::TCard4 c4Size;
    const tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4StartInd, c4Size);
    return TRawMem::hshHashBuffer(pc1Buf, c4Modulus, c4Size - c4StartInd);
}


// Hash count bytes of the buffer from the indicated start index
tCIDLib::THashVal
TMemBuf::hshCalcHash(   const   tCIDLib::TCard4 c4Modulus
                        , const tCIDLib::TCard4 c4StartInd
                        , const tCIDLib::TCard4 c4Count) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange(CID_LINE, c4StartInd, c4Count);
    return TRawMem::hshHashBuffer(&pc1Buf[c4StartInd], c4Modulus, c4Count);
}


//
//  Do a hex encode. It encodes each character to hex text. We can have a separator
//  between each one, or none if the sep char is null. We can append a leading zero
//  character for those less than 0x10, or not. If bytes is zero, we use the size
//  of the buffer.
//
//  It's possible there could be a good number of bytes to convert, so we don't want
//  to use any overly high level stuff here.
//
tCIDLib::TVoid
TMemBuf::HexEncode(const    tCIDLib::TCard4     c4Bytes
                    , const tCIDLib::TCh        chSepChar
                    , const tCIDLib::TBoolean   bAppend
                    , const tCIDLib::TBoolean   bLowerCase
                    ,       TString&            strTarget)
{
    if (!bAppend)
        strTarget.Clear();


    const tCIDLib::TCard4 c4Count = c4Bytes ? c4Bytes : c4Size();
    const tCIDLib::TCard1* pc1Cur = pc1CheckRange(CID_LINE, 0, c4Count);
    tCIDLib::TCh   ch1;
    tCIDLib::TCh   ch2;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // If they want a sep char, then add one if time to
        if (c4Index && (chSepChar != tCIDLib::TCh(0)))
            strTarget.Append(chSepChar);

        const tCIDLib::TCard1 c1Cur = *pc1Cur++;

        // Convert current byte to two hex digits.
        TString::FromHex(c1Cur, ch1, ch2);

        // They will be upper case, so if lower is desired, convert them first
        if (bLowerCase)
        {
            ch1 = TRawStr::chLower(ch1);
            ch2 = TRawStr::chLower(ch2);
        }
        strTarget.Append(ch1);
        strTarget.Append(ch2);
    }
}


// Extract a signed value from the buffer at the indicated index
tCIDLib::TInt1 TMemBuf::i1At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TInt1)
    );
    return *reinterpret_cast<const tCIDLib::TInt1*>(&pc1Buf[c4Ind]);
}

tCIDLib::TInt2 TMemBuf::i2At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TInt2)
    );
    return *reinterpret_cast<const tCIDLib::TInt2*>(&pc1Buf[c4Ind]);
}

tCIDLib::TInt4 TMemBuf::i4At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TInt4)
    );
    return *reinterpret_cast<const tCIDLib::TInt4*>(&pc1Buf[c4Ind]);
}

tCIDLib::TInt8 TMemBuf::i8At(const tCIDLib::TCard4 c4Ind) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Ind, sizeof(tCIDLib::TInt8)
    );
    return *reinterpret_cast<const tCIDLib::TInt8*>(&pc1Buf[c4Ind]);
}


//
//  Moves bytes from c4At to the end of the buffer up by the indicated number
//  of bytes, to make space for something to in there.
//
//  We don't assume the buffer size is where we should start moving from, since it
//  may be way bigger than the data the caller cares about. So they provide an
//  original count of bytes that we are to consider the content of interest.
//
tCIDLib::TVoid
TMemBuf::MakeSpace( const   tCIDLib::TCard4     c4At
                    , const tCIDLib::TCard4     c4SpaceSz
                    , const tCIDLib::TCard4     c4OrgCnt)
{
    //
    //  If the original count or the starting point is larger than the current
    //  allocate size, this makes no sense. It's fine if the org plus the new space
    //  is larger, though that might fail reallocation below if larger than the max.
    //
    const tCIDLib::TCard4 c4CurBufSz = c4Size();
    if ((c4OrgCnt > c4CurBufSz) || (c4At + c4SpaceSz > c4OrgCnt))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_RangeTop
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4At)
            , TCardinal(c4SpaceSz)
            , TCardinal(c4OrgCnt)
        );
    }

    //
    //  Make sure there's enough space in the whole buffer, expanding if needed
    //  to hold all the content the caller says is of interest plus the extra
    //  space
    //
    pc1CheckRange(CID_LINE, 0, c4OrgCnt + c4SpaceSz);

    //
    //  If no space to add, then do nothing. The check above insures that org count
    //  cannot be zero if at and space size isn't zero also, so we don't have to deal
    //  with potential wrap around below when we sub 1 to get the last byte.
    //
    if (!c4SpaceSz)
        return;

    //
    //  Now check the actual index. DO THIS after the above since a reallocation
    //  could invalidate our pointer.
    //
    tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4At);

    // Get the starting point
    tCIDLib::TCard1* pc1Start = pc1Buf + c4At;

    // We need a source at the last byte, and a target c4Bytes beyond that
    tCIDLib::TCard1* pc1Src = pc1Buf + (c4OrgCnt - 1);
    tCIDLib::TCard1* pc1Tar = pc1Src + c4SpaceSz;

    //
    //  Move down till the src pointer passes the original starting point. Since
    //  we are decrementing before testing, we can't check for just equal since
    //  that wouldn't do the final loop. Ans it also means we aren't going beyond
    //  the start point since we accessed then decremented.
    do
    {
        *pc1Tar-- = *pc1Src--;
    }   while (pc1Src >= pc1Start);
}


// Moves a range of bytes down to the start of the buffer
tCIDLib::TVoid
TMemBuf::MoveToStart(const  tCIDLib::TCard4 c4StartInd
                    , const tCIDLib::TCard4 c4Count)
{
    // Very the range
    tCIDLib::TCard1* pc1Buf = pc1CheckRange(CID_LINE, c4StartInd, c4Count);

    // If the start index is zero or the count is zero, nothing to do
    if (!c4StartInd || !c4Count)
        return;

    tCIDLib::TCard1* pc1Src = pc1Buf + c4StartInd;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pc1Buf++ = *pc1Src++;
}


// Returns readable or writeable straems over this buffer
TBinInStream* TMemBuf::pstrmMakeReadable()
{
    return new TBinInStream(new TMemInStreamImpl(this));
}

TBinOutStream* TMemBuf::pstrmMakeWriteable()
{
    return new TBinOutStream(new TMemOutStreamImpl(this));
}


// Provide access to the raw data buffer
const tCIDLib::TVoid* TMemBuf::pData() const
{
    return reinterpret_cast<const tCIDLib::TVoid*>(pc1QueryBuf());
}

tCIDLib::TVoid* TMemBuf::pData()
{
    return reinterpret_cast<tCIDLib::TVoid*>(pc1QueryBuf());
}

const tCIDLib::TCard1* TMemBuf::pc1Data() const
{
    return pc1QueryBuf();
}

tCIDLib::TCard1* TMemBuf::pc1Data()
{
    return pc1QueryBuf();
}


// Same as above but at a particular offset
const tCIDLib::TCard1* TMemBuf::pc1DataAt(const tCIDLib::TCard4 c4Index) const
{
    const tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Index);
    return &pc1Buf[c4Index];
}

tCIDLib::TCard1* TMemBuf::pc1DataAt(const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Index);
    return &pc1Buf[c4Index];
}


// Put various types of data into our buffer. Doesn't deal with endianness
tCIDLib::TVoid TMemBuf::PutBool(const   tCIDLib::TBoolean   bToPut
                                , const tCIDLib::TCard4     c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TBoolean)
    );
    *reinterpret_cast<tCIDLib::TBoolean*>(&pc1Buf[c4Index]) = bToPut;
}

tCIDLib::TVoid TMemBuf::PutCh(  const   tCIDLib::TCh    chToPut
                                , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TCh)
    );
    *reinterpret_cast<tCIDLib::TCh*>(&pc1Buf[c4Index]) = chToPut;
}

tCIDLib::TVoid TMemBuf::PutCard1(   const   tCIDLib::TCard1 c1ToPut
                                    , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Index);
    pc1Buf[c4Index] = c1ToPut;
}

tCIDLib::TVoid TMemBuf::PutCard2(   const   tCIDLib::TCard2 c2ToPut
                                    , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TCard2)
    );
    *reinterpret_cast<tCIDLib::TCard2*>(&pc1Buf[c4Index]) = c2ToPut;
}

tCIDLib::TVoid TMemBuf::PutCard4(   const   tCIDLib::TCard4 c4ToPut
                                    , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TCard4)
    );
    *reinterpret_cast<tCIDLib::TCard4*>(&pc1Buf[c4Index]) = c4ToPut;
}

tCIDLib::TVoid TMemBuf::PutCard8(   const   tCIDLib::TCard8 c8ToPut
                                    , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TCard8)
    );
    *reinterpret_cast<tCIDLib::TCard8*>(&pc1Buf[c4Index]) = c8ToPut;
}

tCIDLib::TVoid TMemBuf::PutFloat4(  const   tCIDLib::TFloat4    f4ToPut
                                    , const tCIDLib::TCard4     c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TFloat4)
    );
    *reinterpret_cast<tCIDLib::TFloat4*>(&pc1Buf[c4Index]) = f4ToPut;
}

tCIDLib::TVoid TMemBuf::PutFloat8(  const   tCIDLib::TFloat8    f8ToPut
                                    , const tCIDLib::TCard4     c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TFloat8)
    );
    *reinterpret_cast<tCIDLib::TFloat8*>(&pc1Buf[c4Index]) = f8ToPut;
}

tCIDLib::TVoid TMemBuf::PutInt1(const   tCIDLib::TInt1  i1ToPut
                                , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Index);
    *reinterpret_cast<tCIDLib::TInt1*>(&pc1Buf[c4Index]) = i1ToPut;
}

tCIDLib::TVoid TMemBuf::PutInt2(const   tCIDLib::TInt2  i2ToPut
                                , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TInt2)
    );
    *reinterpret_cast<tCIDLib::TInt2*>(&pc1Buf[c4Index]) = i2ToPut;
}

tCIDLib::TVoid TMemBuf::PutInt4(const   tCIDLib::TInt4  i4ToPut
                                , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TInt4)
    );
    *reinterpret_cast<tCIDLib::TInt4*>(&pc1Buf[c4Index]) = i4ToPut;
}

tCIDLib::TVoid TMemBuf::PutInt8(const   tCIDLib::TInt8  i8ToPut
                                , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TInt8)
    );
    *reinterpret_cast<tCIDLib::TInt8*>(&pc1Buf[c4Index]) = i8ToPut;
}

tCIDLib::TVoid TMemBuf::PutSCh( const   tCIDLib::TSCh   schToPut
                                , const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckRange
    (
        CID_LINE, c4Index, sizeof(tCIDLib::TSCh)
    );
    *reinterpret_cast<tCIDLib::TSCh*>(&pc1Buf[c4Index]) = schToPut;
}



tCIDLib::TVoid
TMemBuf::Reallocate(const   tCIDLib::TCard4     c4NewSize
                    , const tCIDLib::TBoolean   bPreserve) const
{
    // Get the current buffer info from the derived class
    tCIDLib::TCard4 c4MaxSize;
    tCIDLib::TCard4 c4CurSize;
    pc1QueryBufInfo(c4CurSize, c4MaxSize);

    // If the enw size is larger than the max, we cannot expand
    if (c4NewSize > c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_CannotExpand
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4NewSize)
            , TCardinal(c4MaxSize)
        );
    }

    // If its less than or equal to current size, then nothing to do
    if (c4NewSize <= c4CurSize)
        return;

    // Call the protected virtual method
    Realloc(c4NewSize, bPreserve);
}


//
//  Removes a chunk of bytes from the buffer, moving things beyond that down to fill
//  that hole. It doesn't assume that the buffer size is the end of the data the caller
//  cares about, so they have to provide an original count. So bytes from at+plus count
//  up to orgcount will be moved down to at.
//
tCIDLib::TVoid
TMemBuf::RemoveSpace(const  tCIDLib::TCard4     c4At
                    , const tCIDLib::TCard4     c4RemoveCnt
                    , const tCIDLib::TCard4     c4OrgCnt)
{
    // Very the range is at least within the buffer size
    tCIDLib::TCard1* pc1Buf = pc1CheckRange(CID_LINE, c4At, c4RemoveCnt);

    //
    //  Make sure that the original count is beyond the current size, and the start
    //  plus remove count is within the original count. This also insures that org
    //  count can't be zero unless remove count and at are zero, which would cause
    //  us to do nothing below.
    //
    const tCIDLib::TCard4 c4CurBufSz = c4Size();
    if ((c4OrgCnt > c4CurBufSz) || (c4At + c4RemoveCnt > c4OrgCnt))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_RangeTop
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4At)
            , TCardinal(c4RemoveCnt)
            , TCardinal(c4OrgCnt)
        );
    }

    // If not bytes to remove, then nothing to do
    if (!c4RemoveCnt)
        return;

    //
    //  If the start count equals the original count, nothing to do. The check above
    //  means the count had to be zero.
    //
    if (c4At == c4OrgCnt)
        return;

    tCIDLib::TCard1* pc1Tar = pc1Buf + c4At;
    tCIDLib::TCard1* pc1Src = pc1Buf + c4At + c4RemoveCnt;
    tCIDLib::TCard1* pc1End = pc1Buf + c4OrgCnt;
    while (pc1Src < pc1End)
        *pc1Tar++ = *pc1Src++;
}


tCIDLib::TSCh TMemBuf::schAt(const tCIDLib::TCard4 c4Index)
{
    tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4Index);
    return *reinterpret_cast<tCIDLib::TSCh*>(&pc1Buf[c4Index]);
}


tCIDLib::TVoid TMemBuf::Set(const tCIDLib::TCard1 c1Fill)
{
    tCIDLib::TCard4 c4CurSize;
    tCIDLib::TCard4 c4MaxSize;
    tCIDLib::TCard1* pc1Buf = pc1QueryBufInfo(c4CurSize, c4MaxSize);
    TRawMem::SetMemBuf(pc1Buf, c1Fill, c4CurSize);
}

tCIDLib::TVoid
TMemBuf::Set(const tCIDLib::TCard1 c1Fill, const tCIDLib::TCard4 c4StartInd)
{
    tCIDLib::TCard4 c4CurSize;
    tCIDLib::TCard1* pc1Buf = pc1CheckIndex(CID_LINE, c4StartInd, c4CurSize);
    TRawMem::SetMemBuf(&pc1Buf[c4StartInd], c1Fill, c4CurSize - c4StartInd);
}

tCIDLib::TVoid
TMemBuf::Set(   const   tCIDLib::TCard1 c1Fill
                , const tCIDLib::TCard4 c4StartInd
                , const tCIDLib::TCard4 c4Count)
{
    if (!c4Count)
        return;

    tCIDLib::TCard1* pc1Buf = pc1CheckRange(CID_LINE, c4StartInd, c4Count);
    TRawMem::SetMemBuf(&pc1Buf[c4StartInd], c1Fill, c4Count);
}



// ---------------------------------------------------------------------------
//  TMemBuf: Hidden constructors/operators
// ---------------------------------------------------------------------------
TMemBuf::TMemBuf()
{
    // Don't validate the members because they are invalid now!!
}


//
//  These are here just so that they can be called by derived classes. So, if we
//  ever do add any members here, these will be in placed and called.
//
TMemBuf::TMemBuf(const TMemBuf&)
{
}

TMemBuf::TMemBuf(TMemBuf&&)
{
}

TMemBuf& TMemBuf::operator=(const TMemBuf&)
{
    return *this;
}

TMemBuf& TMemBuf::operator=(TMemBuf&&)
{
    return *this;
}



// ---------------------------------------------------------------------------
//  TMemBuf: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMemBuf::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);
}

tCIDLib::TVoid TMemBuf::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream out the marker
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject;
}


// ---------------------------------------------------------------------------
//  TMemBuf: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  These are for derived classes to call, to validate the common parms that
//  they will all likely accept.
//
tCIDLib::TVoid
TMemBuf::ValidateExpInc(const   tCIDLib::TCard4 c4Size
                        , const tCIDLib::TCard4 c4MaxSize
                        , const tCIDLib::TCard4 c4ExpandIncrement)
{
    // If the size and max are the same, then nothing to check
    if (c4Size == c4MaxSize)
        return;

    // Tthe buffer has room to expand, so there must be an expand increment
    if (!c4ExpandIncrement)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BadState
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , facCIDLib().strMsg(kCIDMsgs::midMem_ExpandIncIs)
        );
    }

    // The expansion increment obviously cannot be larger than max size
    if (c4ExpandIncrement > c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BadState
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , facCIDLib().strMsg
              (
                kCIDMsgs::midMem_ExpandIncSz
                , TCardinal(c4ExpandIncrement)
                , TCardinal(c4MaxSize)
              )
        );
    }
}

tCIDLib::TVoid
TMemBuf::ValidateSizes( const   tCIDLib::TCard4 c4Size
                        , const tCIDLib::TCard4 c4MaxSize)
{
    if (!c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BadState
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , facCIDLib().strMsg(kCIDMsgs::midMem_MaxSizeIs)
        );
    }

    if (!c4Size)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BadState
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , facCIDLib().strMsg(kCIDMsgs::midMem_InitSizeIs)
        );
    }

    if (c4Size > c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_BadState
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , facCIDLib().strMsg
              (
                kCIDMsgs::midMem_SizeVsMaxSize
                , TCardinal(c4Size)
                , TCardinal(c4MaxSize)
              )
        );
    }
}



// ---------------------------------------------------------------------------
//  TMemBuf: Private, non-virtual methods
// ---------------------------------------------------------------------------
const tCIDLib::TCard1*
TMemBuf::pc1CheckIndex( const   tCIDLib::TCard4 c4Line
                        , const tCIDLib::TCard4 c4Index) const
{
    // Call the other version with a dummy size param
    tCIDLib::TCard4 c4CurSize;
    return pc1CheckIndex(c4Line, c4Index, c4CurSize);
}

tCIDLib::TCard1*
TMemBuf::pc1CheckIndex( const   tCIDLib::TCard4 c4Line
                        , const tCIDLib::TCard4 c4Index)
{
    // Call the other version with a dummy size param
    tCIDLib::TCard4 c4CurSize;
    return pc1CheckIndex(c4Line, c4Index, c4CurSize);
}


const tCIDLib::TCard1*
TMemBuf::pc1CheckIndex( const   tCIDLib::TCard4     c4Line
                        , const tCIDLib::TCard4     c4Index
                        ,       tCIDLib::TCard4&    c4Size) const
{
    tCIDLib::TCard4 c4MaxSize;
    const tCIDLib::TCard1* pc1Ret = pc1QueryBufInfo(c4Size, c4MaxSize);

    if (c4Index >= c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4Index)
            , clsThis()
            , TCardinal(c4MaxSize)
        );
    }

    //
    //  See if we need to reallocate. Add one to the index because the realloc
    //  wants a size, not an index.
    //
    if (c4Index >= c4Size)
    {
        Reallocate(c4Index + 1);

        // The buffer might have changed
        pc1Ret = pc1QueryBufInfo(c4Size, c4MaxSize);
    }
    return pc1Ret;
}

tCIDLib::TCard1*
TMemBuf::pc1CheckIndex( const   tCIDLib::TCard4     c4Line
                        , const tCIDLib::TCard4     c4Index
                        ,       tCIDLib::TCard4&    c4Size)
{
    tCIDLib::TCard4 c4MaxSize;
    tCIDLib::TCard1* pc1Ret = pc1QueryBufInfo(c4Size, c4MaxSize);

    if (c4Index >= c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4Index)
            , clsThis()
            , TCardinal(c4MaxSize)
        );
    }

    //
    //  See if we need to reallocate. Add one to the index because the realloc
    //  wants a size, not an index.
    //
    if (c4Index >= c4Size)
    {
        Reallocate(c4Index + 1);

        // The buffer might have changed
        pc1Ret = pc1QueryBufInfo(c4Size, c4MaxSize);
    }
    return pc1Ret;
}


const tCIDLib::TCard1*
TMemBuf::pc1CheckRange( const   tCIDLib::TCard4 c4Line
                        , const tCIDLib::TCard4 c4StartIndex
                        , const tCIDLib::TCard4 c4Count) const
{
    // Call the other version with a dummy size return param
    tCIDLib::TCard4 c4CurSize;
    return pc1CheckRange(c4Line, c4StartIndex, c4Count, c4CurSize);
}

tCIDLib::TCard1*
TMemBuf::pc1CheckRange( const   tCIDLib::TCard4 c4Line
                        , const tCIDLib::TCard4 c4StartIndex
                        , const tCIDLib::TCard4 c4Count)
{
    // Call the other version with a dummy size return param
    tCIDLib::TCard4 c4CurSize;
    return pc1CheckRange(c4Line, c4StartIndex, c4Count, c4CurSize);
}


tCIDLib::TCard1*
TMemBuf::pc1CheckRange( const   tCIDLib::TCard4     c4Line
                        , const tCIDLib::TCard4     c4StartIndex
                        , const tCIDLib::TCard4     c4Count
                        ,       tCIDLib::TCard4&    c4Size)
{
    tCIDLib::TCard4 c4MaxSize;
    tCIDLib::TCard1* pc1Ret = pc1QueryBufInfo(c4Size, c4MaxSize);

    if (c4StartIndex >= c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4StartIndex)
            , clsThis()
            , TCardinal(c4MaxSize)
        );
    }

    if (c4StartIndex + c4Count > c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcMBuf_BufOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4MaxSize - 1)
        );
    }

    //
    //  Now see if we need to reallocate the buffer. If any of the range
    //  would exceed the current allocation size, then we have to.
    //
    if (c4StartIndex + c4Count >= c4Size)
    {
        Reallocate(c4StartIndex + c4Count);

        // The buffer might have changed
        pc1Ret = pc1QueryBuf();
    }
    return pc1Ret;
}

const tCIDLib::TCard1*
TMemBuf::pc1CheckRange( const   tCIDLib::TCard4     c4Line
                        , const tCIDLib::TCard4     c4StartIndex
                        , const tCIDLib::TCard4     c4Count
                        ,       tCIDLib::TCard4&    c4Size) const
{
    tCIDLib::TCard4 c4MaxSize;
    const tCIDLib::TCard1* pc1Ret = pc1QueryBufInfo(c4Size, c4MaxSize);

    if (c4StartIndex >= c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4StartIndex)
            , clsThis()
            , TCardinal(c4MaxSize)
        );
    }

    if (c4StartIndex + c4Count > c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcMBuf_BufOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4MaxSize - 1)
        );
    }

    //
    //  Now see if we need to reallocate the buffer. If any of the range
    //  would exceed the current allocation size, then we have to.
    //
    if (c4StartIndex + c4Count > c4Size)
    {
        Reallocate(c4StartIndex + c4Count);

        // The buffer might have changed
        pc1Ret = pc1QueryBuf();
    }
    return pc1Ret;
}



