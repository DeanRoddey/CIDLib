//
// FILE NAME: CIDLib_HeapMemBuf.cpp
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
//  This method implements the heap based derivative of the basic memory
//  buffer class.
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
RTTIDecls(THeapBuf,TMemBuf)


// ---------------------------------------------------------------------------
//  Local data and types
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDLib_HeapMemBuf
    {
        // -----------------------------------------------------------------------
        //  Some internal constants we use
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4DefMinAlloc = 64;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: THeapBuf
//  PREFIX: mbuf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THeapBuf: Constructors and Destructor
// ---------------------------------------------------------------------------

// We leave the buffer empty and will fault it in if needed
THeapBuf::THeapBuf() :

    m_c4ExpandIncrement(4096)
    , m_c4Size(8)
    , m_c4MaxSize(kCIDLib::c4DefMaxBufferSz)
    , m_pc1Data(nullptr)
{
}


//
//  Create a buffer with initial content based on the contents of the passed
//  string. The literal native wide character contents are copied into the
//  buffer, so no flattening is done. Use a stream for that!
//
//  If max size is zero, then it's set to the actual size allocated, so the
//  buffer becomes fully allocated. Else it must be bigger than the size
//  required.
//
THeapBuf::THeapBuf( const   TString&        strInitData
                    , const tCIDLib::TCard4 c4MaxSize
                    , const tCIDLib::TCard4 c4ExpandIncrement) :

    m_c4ExpandIncrement(c4ExpandIncrement)
    , m_c4Size(0)
    , m_c4MaxSize(c4MaxSize)
    , m_pc1Data(nullptr)
{
    // Calc the source bytes we need to copy
    const tCIDLib::TCard4 c4SrcSize = strInitData.c4Length() * kCIDLib::c4CharBytes;

    // Use this to set the init alloc size. If zero, default it
    m_c4Size = c4SrcSize;
    if (!m_c4Size)
    {
        m_c4Size = tCIDLib::MinVal(CIDLib_HeapMemBuf::c4DefMinAlloc, m_c4MaxSize);
        if (!m_c4Size)
            m_c4Size = CIDLib_HeapMemBuf::c4DefMinAlloc;
    }

    // If it's zero, then set it to the alloc size and zero increment
    if (!m_c4MaxSize)
    {
        m_c4MaxSize = m_c4Size;
        m_c4ExpandIncrement = 0;
    }
     else
    {
        // Validate the sizes, since we aren't defaulting them
        ValidateSizes(m_c4Size, m_c4MaxSize);

        // If we have expansion room, validate or default the expand increment
        if (m_c4Size != m_c4MaxSize)
        {
            if (!m_c4ExpandIncrement)
            {
                m_c4ExpandIncrement = m_c4MaxSize - m_c4Size;
                if (m_c4ExpandIncrement > kCIDLib_::c4MaxDefMemExpandInc)
                    m_c4ExpandIncrement = kCIDLib_::c4MaxDefMemExpandInc;
            }
             else
            {
                // We didn't default it, so vaalidate it
                ValidateExpInc(m_c4Size, m_c4MaxSize, m_c4ExpandIncrement);
            }
        }
    }

    //
    //  Allocate the buffer and copy over the initial contents. Note that
    //  we use the source size for the copy, not our alloc size, since
    //  they aren't necessarily the same.
    //
    m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    try
    {
        TRawMem::CopyMemBuf(m_pc1Data, strInitData.pszBuffer(), c4SrcSize);
    }

    catch(...)
    {
        delete m_pc1Data;
        throw;
    }
}


//
//  Create a buffer with initial content based on the contents of the passed
//  raw string. The literal native wide character contents are copied into
//  the buffer, so no flattening is done. Use a stream for that!
//
//  If max size is zero, then it's set to the actual size allocated, so the
//  buffer becomes fully allocated. Else it must be bigger than the size
//  required.
//
THeapBuf::THeapBuf( const   tCIDLib::TCh* const pszInitData
                    , const tCIDLib::TCard4     c4MaxSize
                    , const tCIDLib::TCard4     c4ExpandIncrement) :

    m_c4ExpandIncrement(c4ExpandIncrement)
    , m_c4Size(0)
    , m_c4MaxSize(c4MaxSize)
    , m_pc1Data(nullptr)
{
    // Calc the source bytes we need to copy
    const tCIDLib::TCard4 c4SrcSize
    (
        TRawStr::c4StrLen(pszInitData) * kCIDLib::c4CharBytes
    );

    // Use this to set the init alloc size. If zero, default it
    m_c4Size = c4SrcSize;
    if (!m_c4Size)
    {
        m_c4Size = tCIDLib::MinVal(CIDLib_HeapMemBuf::c4DefMinAlloc, m_c4MaxSize);
        if (!m_c4Size)
            m_c4Size = CIDLib_HeapMemBuf::c4DefMinAlloc;
    }

    // If it's zero, then set it to the alloc size and zero increment
    if (!m_c4MaxSize)
    {
        m_c4MaxSize = m_c4Size;
        m_c4ExpandIncrement = 0;
    }
     else
    {
        // Validate the sizes, since we aren't defaulting them
        ValidateSizes(m_c4Size, m_c4MaxSize);

        // If we have expansion room, validate or default the expand increment
        if (m_c4Size != m_c4MaxSize)
        {
            if (!m_c4ExpandIncrement)
            {
                m_c4ExpandIncrement = m_c4MaxSize - m_c4Size;
                if (m_c4ExpandIncrement > kCIDLib_::c4MaxDefMemExpandInc)
                    m_c4ExpandIncrement = kCIDLib_::c4MaxDefMemExpandInc;
            }
             else
            {
                // We didn't default it, so vaalidate it
                ValidateExpInc(m_c4Size, m_c4MaxSize, m_c4ExpandIncrement);
            }
        }
    }

    //
    //  Allocate the buffer and copy over the initial contents. Note that
    //  we use the source size for the copy, not our alloc size, since
    //  they aren't necessarily the same.
    //
    m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    try
    {
        TRawMem::CopyMemBuf(m_pc1Data, pszInitData, c4SrcSize);
    }

    catch(...)
    {
        delete m_pc1Data;
        throw;
    }
}


//
//  The size must be non-zero on this one. If max size is zero, then it's set
//  to the actual size, so the buffer becomes fully allocated. Else it must be
//  bigger than the size required.
//
//  We don't actually create the buffer here, we let it fault in later if actually
//  ever used.
//
THeapBuf::THeapBuf( const   tCIDLib::TCard4 c4Size
                    , const tCIDLib::TCard4 c4MaxSize
                    , const tCIDLib::TCard4 c4ExpandIncrement) :

    m_c4ExpandIncrement(c4ExpandIncrement)
    , m_c4Size(c4Size)
    , m_c4MaxSize(c4MaxSize)
    , m_pc1Data(nullptr)
{
    //
    //  If we have a non-zero size, and zero max size, then default the
    //  max to the size and clear the expand increment since it's now fully
    //  allocated.
    //
    //  Else, we have to check everything.
    //
    if (m_c4Size && !m_c4MaxSize)
    {
        m_c4MaxSize = m_c4Size;
        m_c4ExpandIncrement = 0;
    }
     else
    {
        // Validate the sizes, since we aren't defaulting them
        ValidateSizes(m_c4Size, m_c4MaxSize);

        // If we have expansion room, validate or default the expand increment
        if (m_c4Size != m_c4MaxSize)
        {
            if (!m_c4ExpandIncrement)
            {
                m_c4ExpandIncrement = m_c4MaxSize - m_c4Size;
                if (m_c4ExpandIncrement > kCIDLib_::c4MaxDefMemExpandInc)
                    m_c4ExpandIncrement = kCIDLib_::c4MaxDefMemExpandInc;
            }
             else
            {
                // We didn't default it, so vaalidate it
                ValidateExpInc(m_c4Size, m_c4MaxSize, m_c4ExpandIncrement);
            }
        }
    }
}


//
//  Create a buffer with initial content based on the contents of the passed raw
//  or memory buffer. The passed buffer better be as big as c4Size for the raw
//  buffer scenario since we cannot verify that here.
//
//  If the size is zero, it is defaulted to a minimal size. If max size is
//  zero, then it's set to the actual size allocated, so the buffer becomes
//  fully allocated. Else it must be bigger than the size required.
//
THeapBuf::THeapBuf( const   TMemBuf&        mbufInitData
                    , const tCIDLib::TCard4 c4Size
                    , const tCIDLib::TCard4 c4MaxSize
                    , const tCIDLib::TCard4 c4ExpandIncrement) :

    m_c4ExpandIncrement(c4ExpandIncrement)
    , m_c4Size(c4Size)
    , m_c4MaxSize(c4MaxSize)
    , m_pc1Data(nullptr)
{
    // if zero size, then default it to the size of the source buffer
    if (!m_c4Size)
    {
        m_c4Size = tCIDLib::MinVal(mbufInitData.c4Size(), m_c4MaxSize);
        if (!m_c4Size)
            m_c4Size = CIDLib_HeapMemBuf::c4DefMinAlloc;
    }

    // If max is zero, then set it to the alloc size and zero increment
    if (!m_c4MaxSize)
    {
        m_c4MaxSize = m_c4Size;
        m_c4ExpandIncrement = 0;
    }
     else
    {
        // Validate the sizes, since we aren't defaulting them
        ValidateSizes(m_c4Size, m_c4MaxSize);

        // If we have expansion room, validate or default the expand increment
        if (m_c4Size != m_c4MaxSize)
        {
            if (!m_c4ExpandIncrement)
            {
                m_c4ExpandIncrement = m_c4MaxSize - m_c4Size;
                if (m_c4ExpandIncrement > kCIDLib_::c4MaxDefMemExpandInc)
                    m_c4ExpandIncrement = kCIDLib_::c4MaxDefMemExpandInc;
            }
             else
            {
                // We didn't default it, so vaalidate it
                ValidateExpInc(m_c4Size, m_c4MaxSize, m_c4ExpandIncrement);
            }
        }
    }

    //
    //  Allocate the buffer and copy over the initial contents. Note that
    //  we use the source size for the copy, not our alloc size, since
    //  they aren't necessarily the same.
    //
    m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    try
    {
        TRawMem::CopyMemBuf(m_pc1Data, mbufInitData.pc1Data(), c4Size);
    }

    catch(...)
    {
        delete m_pc1Data;
        throw;
    }
}

THeapBuf::THeapBuf( const   tCIDLib::TCard1* const  apc1InitData
                    , const tCIDLib::TCard4         c4Size
                    , const tCIDLib::TCard4         c4MaxSize
                    , const tCIDLib::TCard4         c4ExpandIncrement) :

    m_c4ExpandIncrement(c4ExpandIncrement)
    , m_c4Size(c4Size)
    , m_c4MaxSize(c4MaxSize)
    , m_pc1Data(nullptr)
{
    // if zero size, then default it
    if (!m_c4Size)
    {
        m_c4Size = tCIDLib::MinVal(CIDLib_HeapMemBuf::c4DefMinAlloc, m_c4MaxSize);
        if (!m_c4Size)
            m_c4Size = CIDLib_HeapMemBuf::c4DefMinAlloc;
    }

    // If it's zero, then set it to the alloc size and zero increment
    if (!m_c4MaxSize)
    {
        m_c4MaxSize = m_c4Size;
        m_c4ExpandIncrement = 0;
    }
     else
    {
        // Validate the sizes, since we aren't defaulting them
        ValidateSizes(m_c4Size, m_c4MaxSize);

        // If we have expansion room, validate or default the expand increment
        if (m_c4Size != m_c4MaxSize)
        {
            if (!m_c4ExpandIncrement)
            {
                m_c4ExpandIncrement = m_c4MaxSize - m_c4Size;
                if (m_c4ExpandIncrement > kCIDLib_::c4MaxDefMemExpandInc)
                    m_c4ExpandIncrement = kCIDLib_::c4MaxDefMemExpandInc;
            }
             else
            {
                // We didn't default it, so vaalidate it
                ValidateExpInc(m_c4Size, m_c4MaxSize, m_c4ExpandIncrement);
            }
        }
    }

    //
    //  Allocate the buffer and copy over the initial contents. Note that
    //  we use the source size for the copy, not our alloc size, since
    //  they aren't necessarily the same.
    //
    m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    try
    {
        TRawMem::CopyMemBuf(m_pc1Data, apc1InitData, c4Size);
    }

    catch(...)
    {
        delete m_pc1Data;
        m_pc1Data = nullptr;
        throw;
    }
}


//
//  Initialize from a source buffer, but only taking a given number of bytes
//  from it. We keep the source buffer's max size and expand increment. If the
//  bytes to copy is zero, we use a minimum default alloc size or the max size,
//  whichever is smaller.
//
THeapBuf::THeapBuf( const   THeapBuf&       mbufSrc
                    , const tCIDLib::TCard4 c4RelevantBytes) :

    TMemBuf(mbufSrc)
    , m_c4ExpandIncrement(mbufSrc.m_c4ExpandIncrement)
    , m_c4Size(c4RelevantBytes)
    , m_c4MaxSize(mbufSrc.m_c4MaxSize)
    , m_pc1Data(nullptr)
{
    // If no relevant bytes, then set our alloc amount to a minimal default.
    if (!m_c4Size)
        m_c4Size = tCIDLib::MinVal(64UL, m_c4MaxSize);

    //
    //  If this now means that the buffer is no longer fully allocated, then
    //  we have to set an expand increment again.
    //
    if (m_c4Size < m_c4MaxSize)
    {
        //
        //  If there is alread an expand increment, keep it, since it has
        //  to be valid for our max size. If no existing one, then calc
        //  a default one.
        //
        if (!m_c4ExpandIncrement)
        {
            m_c4ExpandIncrement = m_c4MaxSize - m_c4Size;
            if (m_c4ExpandIncrement > kCIDLib_::c4MaxDefMemExpandInc)
                m_c4ExpandIncrement = kCIDLib_::c4MaxDefMemExpandInc;
        }
    }

    // Validate everything just to be safe
    ValidateParms(m_c4Size, m_c4MaxSize, m_c4ExpandIncrement);

    //
    //  Allocate the buffer and copy in the source data. Note that we use
    //  our calculated size for the alloc, but the caller's size for the
    //  amount of data to copy from the source buffer, since they can be
    //  different.
    //
    m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    try
    {
        if (c4RelevantBytes)
            CopyIn(mbufSrc, c4RelevantBytes);
    }

    catch(...)
    {
        delete m_pc1Data;
        m_pc1Data = nullptr;
        throw;
    }
}


THeapBuf::THeapBuf(const THeapBuf& mbufSrc) :

    TMemBuf(mbufSrc)
    , m_c4ExpandIncrement(mbufSrc.m_c4ExpandIncrement)
    , m_c4Size(mbufSrc.m_c4Size)
    , m_c4MaxSize(mbufSrc.m_c4MaxSize)
    , m_pc1Data(new tCIDLib::TCard1[m_c4Size])
{
    // Copy the data in from the source
    try
    {
        CopyIn(mbufSrc, m_c4Size);
    }

    catch(...)
    {
        delete m_pc1Data;
        m_pc1Data = nullptr;
        throw;
    }
}

//
//  Set up a null buffer and give it to the source. He'll fault one back
//  in if needed.
//
THeapBuf::THeapBuf(THeapBuf&& mbufSrc) :

    m_c4ExpandIncrement(8)
    , m_c4Size(8)
    , m_c4MaxSize(kCIDLib::c4DefMaxBufferSz)
    , m_pc1Data(nullptr)
{
    *this = tCIDLib::ForceMove(mbufSrc);
}


// We just have to let the buffer go
THeapBuf::~THeapBuf()
{
    if (m_pc1Data)
    {
        delete [] m_pc1Data;
        m_pc1Data = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  THeapBuf: Public operators
// ---------------------------------------------------------------------------
THeapBuf& THeapBuf::operator=(const THeapBuf& mbufSrc)
{
    if (this != &mbufSrc)
    {
        // Do our parent first
        TParent::operator=(mbufSrc);

        // Free the current buffer if not the same size
        if (m_c4Size != mbufSrc.m_c4Size)
        {
            delete [] m_pc1Data;
            m_pc1Data = nullptr;
        }

        // Copy over the members except for the buffer pointer
        m_c4ExpandIncrement = mbufSrc.m_c4ExpandIncrement;
        m_c4Size            = mbufSrc.m_c4Size;
        m_c4MaxSize         = mbufSrc.m_c4MaxSize;

        // Allocate a buffer if we need to
        if (!m_pc1Data)
            m_pc1Data = new tCIDLib::TCard1[m_c4Size];

        // Copy the contents over and return a reference to ourself
        CopyIn(mbufSrc, m_c4Size);
    }
    return *this;
}

THeapBuf& THeapBuf::operator=(THeapBuf&& mbufSrc)
{
    if (this != &mbufSrc)
    {
        TParent::operator=(tCIDLib::ForceMove(mbufSrc));
        tCIDLib::Swap(m_c4ExpandIncrement, mbufSrc.m_c4ExpandIncrement);
        tCIDLib::Swap(m_c4Size, mbufSrc.m_c4Size);
        tCIDLib::Swap(m_c4MaxSize, mbufSrc.m_c4MaxSize);
        tCIDLib::Swap(m_pc1Data, mbufSrc.m_pc1Data);
    }
    return *this;
}


tCIDLib::TBoolean THeapBuf::operator==(const THeapBuf& mbufToTest) const
{
    if (this == &mbufToTest)
        return kCIDLib::True;

    //
    //  If the sizes are different we won't compare the actual bytes which
    //  avoids faulting in a buffer that we don't need to.
    //
    if ((m_c4Size == mbufToTest.m_c4Size)
    &&  (m_c4MaxSize == mbufToTest.m_c4MaxSize)
    &&  (m_c4ExpandIncrement == mbufToTest.m_c4ExpandIncrement)
    &&  TParent::operator==(mbufToTest))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

tCIDLib::TBoolean THeapBuf::operator!=(const THeapBuf& mbufToTest) const
{
    return !TMemBuf::operator==(mbufToTest);
}


// ---------------------------------------------------------------------------
//  THeapBuf: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  This is the same as the StreamTo() method we inherit from the streamable
//  interface, but sometimes we want to only stream part of the current size
//  of the buffer, though it cannot be done generically via the streamable
//  interface. In order to avoid redundant code, StreamTo() just calls us with
//  the current allocation size.
//
tCIDLib::TVoid THeapBuf::StreamCount(       TBinOutStream&  strmToWriteTo
                                    , const tCIDLib::TCard4 c4Count) const
{
    // Make sure that the count is <= the current size
    if (c4Count > m_c4Size)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_SubsetTooBig
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Range
            , TCardinal(c4Count)
            , TCardinal(m_c4Size)
        );
    }

    // Do our parent first
    TParent::StreamTo(strmToWriteTo);

    // Stream out the simple members, then the buffer contents
    strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                    << c4Count
                    << m_c4MaxSize
                    << m_c4ExpandIncrement;

    // This will fault in the buffer if not already created
    if (c4Count)
        strmToWriteTo.c4WriteRawBuffer(pc1QueryBuf(), c4Count);

    // End up with an end object marker
    strmToWriteTo   << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  THeapBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If the info has changed and we have to get rid of any current buffer that we might
//  have, we don't actually create the new one. We'll let it fault in if actually
//  needed.
//
tCIDLib::TVoid THeapBuf::Reset( const   tCIDLib::TCard4 c4Size
                                , const tCIDLib::TCard4 c4MaxSize
                                , const tCIDLib::TCard4 c4ExpandIncrement)
{
    // If the max is zero or maxcard we use the size as the max
    const tCIDLib::TCard4 c4RealMax
    (
        (!c4MaxSize || (c4MaxSize == kCIDLib::c4MaxCard)) ? c4Size : c4MaxSize
    );

    //
    //  If the passed settings are the same as our settings, then we can
    //  just skip the creation of a new buffer and keep the current one. If
    //  we don't have a buffer yet, then no need to create one. It'll get
    //  faulted in later if needed.
    //
    if ((m_c4MaxSize == c4RealMax)
    &&  (m_c4Size == c4Size)
    &&  (c4ExpandIncrement == c4ExpandIncrement))
    {
        return;
    }

    // Validate the sizes
    ValidateSizes(c4Size, c4RealMax);

    // Set and validate the expand increment
    m_c4ExpandIncrement = c4ExpandIncrement;
    if (!m_c4ExpandIncrement && (c4Size != c4RealMax))
    {
        m_c4ExpandIncrement = c4RealMax - c4Size;
        if (m_c4ExpandIncrement > kCIDLib_::c4MaxDefMemExpandInc)
            m_c4ExpandIncrement = kCIDLib_::c4MaxDefMemExpandInc;
    }
     else
    {
        // Validate the expanstion increment
        ValidateExpInc(c4Size, c4RealMax, m_c4ExpandIncrement);
    }

    // Delete our current buffer
    delete [] m_pc1Data;
    m_pc1Data = nullptr;

    // Store our new stuff
    m_c4Size = c4Size;
    m_c4MaxSize = c4RealMax;
}


// ---------------------------------------------------------------------------
//  THeapBuf: Protected inherited methods
// ---------------------------------------------------------------------------

//
//  The base class has to call these to get access to the buffer, so we will fault
//  in the buffer if needed.
//
tCIDLib::TCard1* THeapBuf::pc1QueryBuf()
{
    // Fault it in if not already
    if (!m_pc1Data)
        m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    return m_pc1Data;
}

const tCIDLib::TCard1* THeapBuf::pc1QueryBuf() const
{
    // Fault it in if not already
    if (!m_pc1Data)
        m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    return m_pc1Data;
}

tCIDLib::TCard1*
THeapBuf::pc1QueryBufInfo(tCIDLib::TCard4& c4CurSize, tCIDLib::TCard4& c4MaxSize)
{
    // Fault it in if not already
    if (!m_pc1Data)
        m_pc1Data = new tCIDLib::TCard1[m_c4Size];

    c4CurSize = m_c4Size;
    c4MaxSize = m_c4MaxSize;
    return m_pc1Data;
}

const tCIDLib::TCard1*
THeapBuf::pc1QueryBufInfo(  tCIDLib::TCard4&    c4CurSize
                            , tCIDLib::TCard4&  c4MaxSize) const
{
    // Fault it in if not already
    if (!m_pc1Data)
        m_pc1Data = new tCIDLib::TCard1[m_c4Size];

    c4CurSize = m_c4Size;
    c4MaxSize = m_c4MaxSize;
    return m_pc1Data;
}

tCIDLib::TVoid
THeapBuf::QueryBufInfo( tCIDLib::TCard4&    c4CurSize
                        , tCIDLib::TCard4&  c4MaxSize) const
{
    c4CurSize = m_c4Size;
    c4MaxSize = m_c4MaxSize;
}


tCIDLib::TVoid
THeapBuf::Realloc(  const   tCIDLib::TCard4     c4NewSize
                    , const tCIDLib::TBoolean   bPreserve) const
{
    // Nothing to do
    if (c4NewSize == m_c4Size)
        return;

    //
    //  Calculate the actual next size above the passed size. We have to
    //  do things in terms of the expansion increment.
    //
    tCIDLib::TCard4 c4ActualSize = c4NewSize;
    if (c4ActualSize % m_c4ExpandIncrement)
        c4ActualSize += m_c4ExpandIncrement - (c4ActualSize % m_c4ExpandIncrement);

    // If this would overflow, then we want to clip it back.
    if (c4ActualSize > m_c4MaxSize)
        c4ActualSize = m_c4MaxSize;

    //
    //  Create the new buffer and protect it with a janitor. Make it actually one byte
    //  larger than requested in case actual size came out zero. This won't hurt
    //  anything. We make no assumptions about a buffers position in memory.
    //
    tCIDLib::TCard1* const  pc1NewData = new tCIDLib::TCard1[c4ActualSize + 1];
    TArrayJanitor<tCIDLib::TCard1> janNewData(pc1NewData);

    //
    //  Copy over the new info if we were told to. Else we want to just fill the
    //  fill the old one. We may not have a buffer. If not, then no need to
    //  fault it in, we'll just do a zero fill since that means there was no real
    //
    if (bPreserve)
    {
        if (m_pc1Data)
            TRawMem::CopyMemBuf(pc1NewData, m_pc1Data, m_c4Size);
        else
            TRawMem::SetMemBuf(pc1NewData, kCIDLib::c1MinCard, m_c4Size);
    }

    // Free the existing buffer now if we had one
    delete [] m_pc1Data;
    m_pc1Data = nullptr;

    // Store the new size and pointer
    m_c4Size = c4ActualSize;
    m_pc1Data = pc1NewData;

    // Tell the janitor to orphan the new buffer
    janNewData.Orphan();
}


tCIDLib::TVoid THeapBuf::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Do our parent first
    TParent::StreamFrom(strmToReadFrom);

    // Our stuff should start with a frame marker
    strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);

    // Get the size members into temps
    tCIDLib::TCard4 c4NewSize;
    tCIDLib::TCard4 c4NewMax;
    tCIDLib::TCard4 c4NewExp;
    strmToReadFrom  >> c4NewSize
                    >> c4NewMax
                    >> c4NewExp;

    //
    //  If our current buffer is at least as big as the new size, but less
    //  than or equal to the new max, then we can keep it. Else, we have
    //  to trash it, which will force a new allocation.
    //
    if (m_pc1Data
    &&  ((m_c4Size < c4NewSize) || (m_c4Size > c4NewMax)))
    {
        delete [] m_pc1Data;
        m_pc1Data = nullptr;
    }

    //
    //  If we didn't have one or got rid of the one we had, allocate one.
    //  Be careful to check for a zero size new size and use a minimum
    //  size instead (making sure to pick a min not larger than the new
    //  max.
    //
    if (!m_pc1Data)
    {
        m_c4Size = c4NewSize;
        if (!m_c4Size)
            m_c4Size = tCIDLib::MinVal(c4NewMax, 32UL);
        m_pc1Data = new tCIDLib::TCard1[m_c4Size];
    }

    // Store the new stuff now
    m_c4Size = c4NewSize;
    m_c4MaxSize = c4NewMax;
    m_c4ExpandIncrement = c4NewExp;

    //
    //  And read the buffer contents in. NOTE that we use the new size we
    //  read in, not the (possibly) adjusted new buffer size.
    //
    if (c4NewSize)
        strmToReadFrom.c4ReadRawBuffer(m_pc1Data, c4NewSize);

    // And it should end up with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid THeapBuf::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Just call StreamCount with the current alloc size
    StreamCount(strmToWriteTo, m_c4Size);
}


