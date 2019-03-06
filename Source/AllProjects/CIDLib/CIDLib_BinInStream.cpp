//
// FILE NAME: CIDLib_BinInStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the binary input stream class. It just adds its own
//  constructors and the implementation of the data reading methods.
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
RTTIDecls(TInStreamImpl,TObject)
RTTIDecls(TBinInStream,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TBinInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinInStream: Public static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TBinInStream::CheckRelationship(const   TObject* const  pobjTest
                                , const TClass&         clsTest)
{
    if (!pobjTest->bIsDescendantOf(clsTest))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_NotDerivedClass
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , pobjTest->clsIsA()
            , clsTest
        );
    }
}


// ---------------------------------------------------------------------------
//  TBinInStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinInStream::TBinInStream(TInStreamImpl* const pstrmiToAdopt) :

    m_c4CurAvail(0)
    , m_c4CurIndex(0)
    , m_eEndianMode(tCIDLib::EEndianModes::Little)
    , m_pstrmiIn(pstrmiToAdopt)
{
}

TBinInStream::~TBinInStream()
{
    delete m_pstrmiIn;
}


// ---------------------------------------------------------------------------
//  TBinInStream: Public operators
// ---------------------------------------------------------------------------
TBinInStream& TBinInStream::operator>>(tCIDLib::TBoolean& bToFill)
{
    //
    //  Boolean values are stored as single bytes, regardless of what the
    //  language defines them as. This saves space and avoids byte swapping
    //  issues altogether.
    //
    tCIDLib::TCard1 c1Tmp;
    c4ReadRawBuffer(&c1Tmp, sizeof(c1Tmp));

    if (c1Tmp == 0)
        bToFill = kCIDLib::False;
    else
        bToFill = kCIDLib::True;

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TCard1& c1ToFill)
{
    c4ReadRawBuffer(&c1ToFill, sizeof(c1ToFill));
    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TCard2& c2ToFill)
{
    c4ReadRawBuffer(&c2ToFill, sizeof(c2ToFill));

    // Swap if needed
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        c2ToFill = TRawBits::c2SwapBytes(c2ToFill);

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TCard4& c4ToFill)
{
    c4ReadRawBuffer(&c4ToFill, sizeof(c4ToFill));

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        c4ToFill = TRawBits::c4SwapBytes(c4ToFill);

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TCard8& c8ToFill)
{
    c4ReadRawBuffer(&c8ToFill, sizeof(c8ToFill));

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        c8ToFill = TRawBits::c8SwapBytes(c8ToFill);

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::EStreamMarkers& eMarker)
{
    tCIDLib::TCard1 c1Tmp;
    c4ReadRawBuffer(&c1Tmp, sizeof(c1Tmp));
    eMarker = static_cast<tCIDLib::EStreamMarkers>(c1Tmp);
    if ((eMarker < tCIDLib::EStreamMarkers::Min)
    ||  (eMarker > tCIDLib::EStreamMarkers::Max))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadMarker
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1Tmp)
        );
    }
    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TFloat4& f4ToFill)
{
    c4ReadRawBuffer(&f4ToFill, sizeof(f4ToFill));
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        f4ToFill = TRawBits::f4SwapBytes(f4ToFill, bLittleEndian());

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TFloat8& f8ToFill)
{
    c4ReadRawBuffer(&f8ToFill, sizeof(f8ToFill));
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        f8ToFill = TRawBits::f8SwapBytes(f8ToFill, bLittleEndian());
    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TInt1& i1ToFill)
{
    c4ReadRawBuffer(&i1ToFill, sizeof(i1ToFill));
    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TInt2& i2ToFill)
{
    c4ReadRawBuffer(&i2ToFill, sizeof(i2ToFill));

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        i2ToFill = TRawBits::i2SwapBytes(i2ToFill);

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TInt4& i4ToFill)
{
    c4ReadRawBuffer(&i4ToFill, sizeof(i4ToFill));

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        i4ToFill = TRawBits::i4SwapBytes(i4ToFill);

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TInt8& i8ToFill)
{
    c4ReadRawBuffer(&i8ToFill, sizeof(i8ToFill));

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        i8ToFill = TRawBits::i8SwapBytes(i8ToFill);

    return *this;
}

TBinInStream& TBinInStream::operator>>(tCIDLib::TUInt& uToFill)
{
    c4ReadRawBuffer(&uToFill, sizeof(uToFill));

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        uToFill = TRawBits::c4SwapBytes(uToFill);

    return *this;
}


TBinInStream& TBinInStream::operator>>(tCIDLib::TSInt& iToFill)
{
    c4ReadRawBuffer(&iToFill, sizeof(iToFill));

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        iToFill = TRawBits::i4SwapBytes(iToFill);

    return *this;
}


TBinInStream& TBinInStream::operator>>(tCIDLib::TCh& chToFill)
{
    //
    //  It was stored in a UTF-8 format, so read it in and convert. The
    //  first byte is the count of bytes that were flattened. This avoids
    //  any byte order issues.
    //
    tCIDLib::TCard1 ac1InBuf[kCIDLib::c4MaxUTF8Bytes];

    tCIDLib::TCard1 c1Count;
    c4ReadRawBuffer(&c1Count, sizeof(c1Count));

    // It cannot be bigger than the max UTF8 per/char bytes
    if (c1Count > kCIDLib::c4MaxUTF8Bytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadCharBytes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1Count)
        );
    }

    c4ReadRawBuffer(ac1InBuf, c1Count);

    // And now convert to local wide char
    tCIDLib::TCard4 c4Eaten;
    chToFill = m_tcvtText.chConvertOneFrom(ac1InBuf, c1Count, c4Eaten);

    // It should eat all of the bytes we got
    if (c4Eaten != c1Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_NotAllBytesEaten
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1Count)
        );
    }

    return *this;
}


// ---------------------------------------------------------------------------
//  TBinInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TBinInStream::bEndOfStream() const
{
    //
    //  If we have data in the cache, then we can't be at the end, so check
    //  that first.
    //
    if (c4CacheAvail())
        return kCIDLib::False;

    // We don't have any in the cache, so try to refresh it
    RefreshCache();

    // And check again, this time its definitive
    return !c4CacheAvail();
}


tCIDLib::TBoolean TBinInStream::bIsOpen() const
{
    if (m_pstrmiIn)
        return m_pstrmiIn->bIsOpen();
    return kCIDLib::False;
}


// Set the endian mode. Getting is inlined
tCIDLib::EEndianModes TBinInStream::eEndianMode(const tCIDLib::EEndianModes eToSet)
{
    m_eEndianMode = eToSet;
    return m_eEndianMode;
}


// Returns the bytes currently in the cache
tCIDLib::TCard4 TBinInStream::c4CacheAvail() const
{
    return (m_c4CurAvail - m_c4CurIndex);
}


tCIDLib::TCard4
TBinInStream::c4ReadBuffer(         TMemBuf&            mbufTarget
                            , const tCIDLib::TCard4     c4Count
                            , const tCIDLib::EAllData   eAllData)
{
    //
    //  Make sure that the passed buffer can be expanded to hold the max bytes
    //  the caller is willing to accept.
    //
    if (c4Count > mbufTarget.c4MaxSize())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_CannotExpand
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Overflow
            , TCardinal(c4Count)
            , TCardinal(mbufTarget.c4MaxSize())
        );
    }

    //
    //  Call the other version and pass it the raw buffer pointer. We want to
    //  have a single point of access because of buffering!
    //
    //  If the passed buffer is already allocated large enough to handle all
    //  of the data, then we can just pass it straight in. Otherwise, we will
    //  take one of two approaches. If the max count is not overly large, we
    //  will just go ahead and pre-expand the caller's buffer. Else, we will
    //  use a temp buffer and then copy it into the caller's buffer.
    //
    if (c4Count <= 0x20000)
        mbufTarget.Reallocate(c4Count, kCIDLib::False);

    tCIDLib::TCard4 c4Ret = 0;
    if (mbufTarget.c4Size() >= c4Count)
    {
        c4Ret = c4ReadRawBuffer(mbufTarget.pData(), c4Count, eAllData);
    }
     else
    {
        tCIDLib::TCard1* pc1Tmp = new tCIDLib::TCard1[c4Count];
        TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Tmp);
        c4Ret = c4ReadRawBuffer(pc1Tmp, c4Count, eAllData);
        if (c4Ret)
            mbufTarget.CopyIn(pc1Tmp, c4Ret);
    }
    return c4Ret;
}


tCIDLib::TCard4
TBinInStream::c4ReadRawBuffer(          tCIDLib::TVoid* const   pBufToFill
                                , const tCIDLib::TCard4         c4Count
                                , const tCIDLib::EAllData       eAllData)
{
    //
    //  We do three scenarios here. If the data is fully available via the
    //  cache now, just get it and go on.
    //
    //  If the amount asked for is larger than the cache size, then we don't
    //  bother trying to go via the cache other than to get the current
    //  contents out of it. Otherwise, we just do the normal steps of refilling
    //  the cache if needed and then reading from it.
    //
    tCIDLib::TCard4 c4Ret = 0;
    if (c4Count <= c4CacheAvail())
    {
        TRawMem::CopyMemBuf
        (
            reinterpret_cast<tCIDLib::TCard1*>(pBufToFill)
            , &m_ac1Cache[m_c4CurIndex]
            , c4Count
        );

        // Bump up the cache index and set the bytes read to the full count
        m_c4CurIndex += c4Count;
        c4Ret = c4Count;
    }
     else if (c4Count > c4CacheSz)
    {
        //
        //  The amount asked for is bigger than the whole cache, so we optimize
        //  this one by getting the data from the cache and then reading the
        //  reset straight into the caller's buffer.
        //
        if (c4CacheAvail())
        {
            c4Ret = c4CacheAvail();
            TRawMem::CopyMemBuf
            (
                pBufToFill
                , &m_ac1Cache[m_c4CurIndex]
                , c4Ret
            );
            m_c4CurIndex = m_c4CurAvail;
        }

        // Ask the implementation to provide the rest
        c4Ret += m_pstrmiIn->c4ReadBytes
        (
            reinterpret_cast<tCIDLib::TCard1*>(pBufToFill) + c4Ret
            , c4Count - c4Ret
        );
    }
     else
    {
        //
        //  The amount asked for is less than the cache size but there isn't
        //  enough left in the cache to handle it. So first get what's in the
        //  cache.
        //
        if (c4CacheAvail())
        {
            c4Ret = c4CacheAvail();
            TRawMem::CopyMemBuf
            (
                pBufToFill
                , &m_ac1Cache[m_c4CurIndex]
                , c4Ret
            );
            m_c4CurIndex = m_c4CurAvail;
        }

        // And refresh the cache
        RefreshCache();

        //
        //  Now, we get the rest if we can, or at least as much as we can get
        //  up to the amount left in the requested count
        //
        const tCIDLib::TCard4 c4Extra = tCIDLib::MinVal
        (
            c4Count - c4Ret
            , m_c4CurAvail
        );

        TRawMem::CopyMemBuf
        (
            reinterpret_cast<tCIDLib::TCard1*>(pBufToFill) + c4Ret
            , &m_ac1Cache[m_c4CurIndex]
            , c4Extra
        );

        c4Ret += c4Extra;
        m_c4CurIndex += c4Extra;
    }

    //
    //  If the bytes read is not equal to the bytes requested and the all data
    //  parameter indicates all bytes must be read, then throw. If we get zero
    //  bytes, then throw the end of stream error.
    //
    if ((c4Ret != c4Count) && (eAllData == tCIDLib::EAllData::FailIfNotAll))
    {
        if (c4Ret)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_NotAllData
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::OutResource
                , TCardinal(c4Ret)
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

    return c4Ret;
}


TClass TBinInStream::clsReadClassInfo()
{
    //
    //  First we need to read the type string in. It will start with the
    //  eTypeName marker in a byte, followed by a char count in a Card2, then
    //  that many chars of the name. The null is not stored.
    //
    CheckForMarker(tCIDLib::EStreamMarkers::TypeName, CID_FILE, CID_LINE);

    //
    //  Next should be the number of chars of the name. We know that type
    //  names are limited to 256 chars, so its just stored in a byte.
    //
    tCIDLib::TCard1 c1Count(0);
    *this >> c1Count;
    if (!c1Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadNameCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  Read in the chars, and cap it off, since we don't store the null. This
    //  method will covnert it in from the UTF-8 format used for external
    //  storage, to the internal Unicode format.
    //
    tCIDLib::TCh szUniName[kCIDLib::c1MaxCard + 1];
    TBinInStream_ReadArray(*this, szUniName, c1Count);
    szUniName[c1Count] = kCIDLib::chNull;

    // Hash it so we can compare with the stored hash
    tCIDLib::THashVal hshNewName = TRawStr::hshHashStr
    (
        szUniName
        , kCIDLib::c4ClassModulus
    );

    if (!hshNewName)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  Read in the hash that was stored after the string bytes. We know
    //  its always less than the modulus, so we stored it in a byte.
    //
    tCIDLib::TCard1 c1StoredHash(0);
    *this >> c1StoredHash;

    //
    //  If it does not equal the hash we just generated, then we obviously
    //  are not dealing with a full deck here.
    //
    if (hshNewName != tCIDLib::THashVal(c1StoredHash))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadStoredHash
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1StoredHash)
            , TCardinal(hshNewName)
        );
    }

    //
    //  Return a copy of the class object for this class, if any. If not,
    //  then this throws an exception.
    //
    return TClass::clsForClass(szUniName);
}


// These are forced to Card1 format so no byte order issues
tCIDLib::TVoid
TBinInStream::CheckForMarker(const  tCIDLib::EStreamMarkers eMarker
                            , const tCIDLib::TCh* const     pszFile
                            , const tCIDLib::TCard4         c4Line)
{
    tCIDLib::EStreamMarkers eActual;
    *this >> eActual;

    if (eActual != eMarker)
    {
        tCIDLib::TCh* pszMarker = nullptr;
        if (eMarker == tCIDLib::EStreamMarkers::TypeName)
            pszMarker = L"TypeName";
        else if (eMarker == tCIDLib::EStreamMarkers::EndObject)
            pszMarker = L"EndObject";
        else if (eMarker == tCIDLib::EStreamMarkers::StartObject)
            pszMarker = L"StartObject";
        else if (eMarker == tCIDLib::EStreamMarkers::Frame)
            pszMarker = L"Frame";
        #if CID_DEBUG_ON
        else
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_UnknownMarker
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TInteger(tCIDLib::i4EnumOrd(eActual))
            );
        }
        #endif

        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcStrm_WrongMarker
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(pszMarker)
            , TInteger(tCIDLib::i4EnumOrd(eActual))
        );
    }
}


//
//  This is a convenience. Most apps will never get even close to exceeding
//  the 4GB size on a single file, and will find it much more convenient to
//  deal with positions in terms of TCard4 values. So we check it and if it
//  will fit, give it back to them.
//
tCIDLib::TCard4 TBinInStream::c4CurPos() const
{
    //
    //  We get the current raw position and then subtract back from that
    //  the cache bytes we've not read yet.
    //
    const tCIDLib::TCard8 c8Tmp = m_pstrmiIn->c8CurPos() - c4CacheAvail();

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


tCIDLib::TCard8 TBinInStream::c8CurPos() const
{
    //
    //  We get the current raw position and then subtract back from that
    //  the cache bytes we've not read yet.
    //
    return m_pstrmiIn->c8CurPos() - c4CacheAvail();
}


tCIDLib::TCard4 TBinInStream::c4ReadEnum()
{
    tCIDLib::TCard4 c4Ret;
    *this >> c4Ret;

    // Byte swap if needed
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        c4Ret = TRawBits::c4SwapBytes(c4Ret);

    return c4Ret;
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TBoolean* const    abList
                        , const tCIDLib::TCard4             c4Count)
{
    // Check for silliness
    if (!c4Count)
        return;

    //
    //  We cannot make any assumptions about the local format of booleans,
    //  so they are written out as 1 or 0 value bytes as a canonical format.
    //  To read them back in, we must use a temp buffer, and get the result
    //  back to local boolean format.
    //
    const tCIDLib::TCard4 c4Buf = 128;
    tCIDLib::TCard1 ac1Tmp[c4Buf];
    tCIDLib::TCard1* pc1Buf;
    tCIDLib::TCard1* pc1End;

    // Get a pointer we can move up through the caller's array
    tCIDLib::TBoolean* pbOut = abList;

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
        c4ReadRawBuffer(ac1Tmp, c4ThisTime);

        // Convert the ones we got over to the caller's list
        pc1Buf = ac1Tmp;
        pc1End = pc1Buf + c4ThisTime;
        while (pc1Buf < pc1End)
            *pbOut++ = (*pc1Buf++ != 0);

        // And count down by the number we did this time
        c4ToGo -= c4ThisTime;
    }
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TCard1* const  ac1List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ac1List, c4Count);
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TCard2* const  ac2List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ac2List, sizeof(tCIDLib::TCard2) * c4Count);

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            ac2List[c4Index] = TRawBits::c2SwapBytes(ac2List[c4Index]);
    }
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TCard4* const  ac4List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ac4List, sizeof(tCIDLib::TCard4) * c4Count);

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        TRawBits::SwapCard4Array(ac4List, c4Count);
}

tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TCard8* const  ac8List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ac8List, sizeof(tCIDLib::TCard8) * c4Count);

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            ac8List[c4Index] = TRawBits::c8SwapBytes(ac8List[c4Index]);
    }
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4CharsToRead)
{
    // Check for silliness
    if (!c4CharsToRead)
        return;

    // It was written out in chunks of up to 255 UTF-8 bytes
    const tCIDLib::TCard4 c4BufSz = 255;
    tCIDLib::TCard1 ac1Buf[c4BufSz + 1];

    tCIDLib::TCard1 c1ChunkBytes;
    tCIDLib::TCard2 c2NextChunkInd;
    tCIDLib::TCard4 c4SrcDone;
    tCIDLib::TCard4 c4OutChars;
    tCIDLib::TCard4 c4Total = 0;
    tCIDLib::TCard2 c2ChunkInd = 1;
    TUTF8Converter  tcvtData;

    while (c4Total < c4CharsToRead)
    {
        //
        //  Next things should be the chunk index and the bytes we
        //  get in this chunk. Make sure it matches our running
        //  chunk index counter.
        //
        *this >> c2NextChunkInd >> c1ChunkBytes;
        if (c2NextChunkInd != c2ChunkInd)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_BadChunkInd
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c2ChunkInd)
                , TCardinal(c2NextChunkInd)
            );
        }
        c2ChunkInd++;

        if (!c1ChunkBytes)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_GotZeroChunkSize
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        //
        //  That looked reasonable, so read in the chunk bytes. Default is
        //  to throw if not all bytes read, which is what we want, so we don't
        //  have to check the return.
        //
        c4ReadRawBuffer(ac1Buf, c1ChunkBytes);

        //
        //  Ok, we can try to convert this into our buffer, starting
        //  where we left off.
        //
        c4SrcDone = m_tcvtText.c4ConvertFrom
        (
            ac1Buf
            , c1ChunkBytes
            , &pszToFill[c4Total]
            , c4CharsToRead - c4Total
            , c4OutChars
        );

        if ((c4SrcDone != c1ChunkBytes) || !c4OutChars)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_ConvertChIn
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // Add in the chars done to the total
        c4Total += c4OutChars;
    }

    // And we should have a c2MaxCard final chunk id marker
    *this >> c2NextChunkInd;
    if (c2NextChunkInd != kCIDLib::c2MaxCard)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_NoChunkIndTerm
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TInt1* const   ai1List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ai1List, c4Count);
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TInt2* const   ai2List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ai2List, sizeof(tCIDLib::TInt2) * c4Count);

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            ai2List[c4Index] = TRawBits::i2SwapBytes(ai2List[c4Index]);
    }
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TInt4* const   ai4List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ai4List, sizeof(tCIDLib::TInt4) * c4Count);

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
        TRawBits::SwapInt4Array(ai4List, c4Count);
}

tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TInt8* const   ai8List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(ai8List, sizeof(tCIDLib::TInt8) * c4Count);

    // Do byte swapping if required
    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            ai8List[c4Index] = TRawBits::i8SwapBytes(ai8List[c4Index]);
    }
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TFloat4* const af4List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(af4List, sizeof(tCIDLib::TFloat4)*c4Count);

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            af4List[c4Index] = TRawBits::f4SwapBytes(af4List[c4Index], bLittleEndian());
        }
    }
}


tCIDLib::TVoid
TBinInStream::ReadArray(        tCIDLib::TFloat8* const af8List
                        , const tCIDLib::TCard4         c4Count)
{
    if (!c4Count)
        return;
    c4ReadRawBuffer(af8List, sizeof(tCIDLib::TFloat8)*c4Count);

    if (bLittleEndian() != TSysInfo::bLittleEndian())
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            af8List[c4Index] = TRawBits::f8SwapBytes(af8List[c4Index], bLittleEndian());
    }
}


tCIDLib::TVoid TBinInStream::Reset()
{
    // Clear out the cache and reset the impl object
    m_c4CurIndex = 0;
    m_c4CurAvail = 0;
    m_pstrmiIn->Reset();
}


tCIDLib::TVoid TBinInStream::SkipForwardBy(const tCIDLib::TCard4 c4SkipBy)
{
    //
    //  Run up through the cache first. If there are enough bytes in the cache,
    //  just bump up the index. Otherwise, we do a more complex operation.
    //
    const tCIDLib::TCard4 c4Avail = c4CacheAvail();
    if (c4Avail >= c4SkipBy)
    {
        m_c4CurIndex += c4SkipBy;
    }
     else
    {
        //
        //  First we have to account for the cache data. So we zero out the
        //  current index and cache index, and subtract the amount still
        //  available in the cache from the amount to skip forward by.
        //
        const tCIDLib::TCard4 c4ActualSkip = c4SkipBy - c4Avail;
        m_c4CurIndex = 0;
        m_c4CurAvail = 0;

        // And now ask the impl object to move up the rest of the way
        m_pstrmiIn->SkipForwardBy(c4ActualSkip);

        // And try to reload the cache
        RefreshCache();
    }
}


// ---------------------------------------------------------------------------
//  TBinInStream: Hidden Constructors
// ---------------------------------------------------------------------------
TBinInStream::TBinInStream() :

    m_c4CurAvail(0)
    , m_c4CurIndex(0)
    , m_eEndianMode(tCIDLib::EEndianModes::Little)
    , m_pstrmiIn(nullptr)
{
}


// ---------------------------------------------------------------------------
//  TBinInStream: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TBinInStream::AdoptImplObject(TInStreamImpl* const pstrmiToAdopt)
{
    // Make sure its not already set
    if (m_pstrmiIn)
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
    m_pstrmiIn = pstrmiToAdopt;

    // Assume we are getting reset again, so trash cache content
    TrashCache();
}


tCIDLib::TVoid TBinInStream::DeleteImplObject()
{
    if (m_pstrmiIn)
    {
        delete m_pstrmiIn;
        m_pstrmiIn = nullptr;
    }

    // Assume we are getting reset again, so trash cache content
    TrashCache();
}


tCIDLib::TVoid TBinInStream::TrashCache()
{
    m_c4CurAvail = 0;
    m_c4CurIndex = 0;
}


// ---------------------------------------------------------------------------
//  TBinInStream: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This guy is const because the cache has to sometimes be refreshed in the
//  context of an ostenibly read only method. All of the cache members are
//  mutable for this reason.
//
tCIDLib::TVoid TBinInStream::RefreshCache() const
{
    //
    //  The cache index should always be empty when we get here, so do a debug
    //  mode check that it is.
    //
    #if CID_DEBUG_ON
    if (m_c4CurIndex != m_c4CurAvail)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_CacheIndexErr
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  Make sure that the object has been initialized so that we have an
    //  implementation object.
    //
    if (!m_pstrmiIn)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_NoImplementation
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , clsIsA()
        );
    }

    // Read up to a full cache's worth of data into the cache
    m_c4CurAvail = m_pstrmiIn->c4ReadBytes(m_ac1Cache, c4CacheSz);
    m_c4CurIndex = 0;
}

