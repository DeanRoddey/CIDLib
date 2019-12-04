//
// FILE NAME: CIDLib_SysMemBuf.cpp
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
//  This method implements the system based derivative of the basic memory
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
RTTIDecls(TSysBuf,TMemBuf)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_SysMemBuf
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  Our persistent format versions
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;


        // -----------------------------------------------------------------------
        //  If they don't provide a size, we use this default size
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4DefMinSize = 1024;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TSysBuf
//  PREFIX: mbuf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSysBuf: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  Just create a buffer with one page size allocted and the default max
//  size, and read/write access. It will have random initial contents.
//
TSysBuf::TSysBuf() :

    m_c4Size(kCIDLib::c4MemPageSize)
    , m_c4MaxSize(kCIDLib::c4DefMaxBufferSz)
    , m_eAccess(tCIDLib::EMemAccFlags::ReadWrite)
    , m_pc1Data(nullptr)
{
    //
    //  Create the buffer. It will be lazily committed since the initial
    //  size is a lot smaller than the max.
    //
    Create();
}


//
//  Create a bufffer based on the contents of a string. The contents of the
//  string will be copied as is, i.e. no standard flattening, so use a stream
//  if you want that!
//
//  If the max size is zero, it's set to the initial size to make the buffer
//  fully allocated.
//
TSysBuf::TSysBuf(   const   TString&                strInitData
                    , const tCIDLib::TCard4         c4MaxSize
                    , const tCIDLib::EMemAccFlags   eAccessFlags) :

    m_c4Size(0)
    , m_c4MaxSize(c4MaxSize)
    , m_eAccess(eAccessFlags)
    , m_pc1Data(nullptr)
{
    // Calc the source bytes we need to copy
    const tCIDLib::TCard4 c4SrcSize = strInitData.c4Length() * kCIDLib::c4CharBytes;

    // Use this to set the init alloc size. If zero, default it
    m_c4Size = c4SrcSize;
    if (!m_c4Size)
    {
        m_c4Size = tCIDLib::MinVal(CIDLib_SysMemBuf::c4DefMinSize, m_c4MaxSize);
        if (!m_c4Size)
            m_c4Size = CIDLib_SysMemBuf::c4DefMinSize;
    }

    //
    //  If it's zero, then set it to the alloc size. Else, validate the sizes,
    //  since we aren't defaulting them.
    //
    if (!m_c4MaxSize)
        m_c4MaxSize = m_c4Size;
    else
        ValidateSizes(m_c4Size, m_c4MaxSize);

    //
    //  Create the buffer. Our initial size is big enough to hold the initial
    //  string contents, so this is will do enough for now. If c4MaxSize is
    //  bigger, it can grow later.
    //
    Create();

    //
    //  And copy in the text. Note that we use the source size here, not
    //  our calculated initial alloc size.
    //
    if (c4SrcSize)
    {
        TRawMem::CopyMemBuf
        (
            m_pc1Data
            , strInitData.pszBuffer()
            , c4SrcSize
        );
    }
}

TSysBuf::TSysBuf(   const   tCIDLib::TCh* const     pszInitData
                    , const tCIDLib::TCard4         c4MaxSize
                    , const tCIDLib::EMemAccFlags   eAccessFlags) :

    m_c4Size(0)
    , m_c4MaxSize(c4MaxSize)
    , m_eAccess(eAccessFlags)
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
        m_c4Size = tCIDLib::MinVal(CIDLib_SysMemBuf::c4DefMinSize, m_c4MaxSize);
        if (!m_c4Size)
            m_c4Size = CIDLib_SysMemBuf::c4DefMinSize;
    }

    //
    //  If it's zero, then set it to the alloc size. Else, validate the sizes,
    //  since we aren't defaulting them.
    //
    if (!m_c4MaxSize)
        m_c4MaxSize = m_c4Size;
    else
        ValidateSizes(m_c4Size, m_c4MaxSize);

    //
    //  Create the buffer. Our initial size is big enough to hold the initial
    //  string contents, so this is will do enough for now. If c4MaxSize is
    //  bigger, it can grow later.
    //
    Create();

    //
    //  And copy in the text. Note that we use the source size here, not
    //  our calculated initial alloc size.
    //
    TRawMem::CopyMemBuf(m_pc1Data, pszInitData, c4SrcSize);
}


//
//  Create a buffer with the indicated initial size and max size, i.e. a
//  fully allocated buffer of the indicated size. The size cannot be zero.
//  size that would be the max size as well as the init size, so we cannot
//  default the init size up from zero.
//
TSysBuf::TSysBuf(   const   tCIDLib::TCard4         c4Size
                    , const tCIDLib::EMemAccFlags   eAccessFlags) :

    m_c4Size(c4Size)
    , m_c4MaxSize(c4Size)
    , m_eAccess(eAccessFlags)
    , m_pc1Data(nullptr)
{
    // Validate the size, which cannot be zero
    ValidateSizes(c4Size, c4Size);

    //
    //  Create the buffer. Because the max and current size are the same,
    //  it will effectively commit the whole thing.
    //
    Create();
}


//
//  Do the most common scenario of a buffer with a given size and max size,
//  and access. The size must be non-zero. If the max size is zero, it will
//  be defaulted to the size, to create a fully allocated buffer.
//
TSysBuf::TSysBuf(   const   tCIDLib::TCard4         c4Size
                    , const tCIDLib::TCard4         c4MaxSize
                    , const tCIDLib::EMemAccFlags   eAccessFlags) :

    m_c4Size(c4Size)
    , m_c4MaxSize(c4MaxSize)
    , m_eAccess(eAccessFlags)
    , m_pc1Data(nullptr)
{
    //
    //  If we have a non-zero size, and zero max size, then default the
    //  max to the size. Else, we have to check everything.
    //
    if (m_c4Size && !m_c4MaxSize)
        m_c4MaxSize = m_c4Size;
    else
        ValidateSizes(m_c4Size, m_c4MaxSize);

    //
    //  Create the buffer. Since the size and max size are not the same,
    //  only size amount of bytes (rounded to the next page) will actually
    //  be commited.
    //
    Create();
}

// The standard copy ctor, so just dup the source
TSysBuf::TSysBuf(const TSysBuf& mbufSrc) :

    TMemBuf(mbufSrc)
    , m_c4Size(mbufSrc.m_c4Size)
    , m_c4MaxSize(mbufSrc.m_c4MaxSize)
    , m_eAccess(mbufSrc.m_eAccess)
{
    // Create the buffer
    Create();

    // Copy over the source buffer if they are not the same address.
    if (m_pc1Data != mbufSrc.m_pc1Data)
        TRawMem::CopyMemBuf(m_pc1Data, mbufSrc.m_pc1Data, mbufSrc.m_c4Size);
}

// If we got a buffer allocated, then let it go
TSysBuf::~TSysBuf()
{
    if (m_pc1Data)
    {
        if (!TRawMem::bFreeSysMem(m_pc1Data))
        {
            // Log as a warning so we don't propogate out of here
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_FreeSys
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TSysBuf: Public operators
// ---------------------------------------------------------------------------
TSysBuf& TSysBuf::operator=(const TSysBuf& mbufToAssign)
{
    if (this == &mbufToAssign)
        return *this;

    // If there is an existing buffer, then free it
    if (m_pc1Data)
    {
        if (!TRawMem::bFreeSysMem(m_pc1Data))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_FreeSys
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
        m_pc1Data = nullptr;
    }

    // Do our parent first
    TParent::operator=(mbufToAssign);

    // Copy all the members over now
    m_c4MaxSize = mbufToAssign.m_c4MaxSize;
    m_c4Size    = mbufToAssign.m_c4Size;
    m_eAccess   = mbufToAssign.m_eAccess;

    // Create the new buffer
    Create();

    // Copy the data over to our new buffer
    TRawMem::CopyMemBuf(m_pc1Data, mbufToAssign.m_pc1Data, mbufToAssign.m_c4Size);
    return *this;
}

tCIDLib::TBoolean TSysBuf::operator==(const TSysBuf& mbufToTest) const
{
    if (this == &mbufToTest)
        return kCIDLib::True;

    if  ((m_c4Size == mbufToTest.m_c4Size)
    &&   (m_c4MaxSize == mbufToTest.m_c4MaxSize)
    &&   (m_eAccess == mbufToTest.m_eAccess)
    &&   TParent::operator==(mbufToTest))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TSysBuf::operator!=(const TSysBuf& mbufToTest) const
{
    return !operator==(mbufToTest);
}


// ---------------------------------------------------------------------------
//  TSysBuf: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  This is the same as the StreamTo() method we inherit from the streamable
//  interface, but sometimes we want to only stream part of the current size
//  of the buffer, though it cannot be done generically via the streamable
//  interface. In order to avoid redundant code, StreamTo() calls us with
//  the full size.
//
tCIDLib::TVoid TSysBuf::StreamCount(        TBinOutStream&  strmToWriteTo
                                    , const tCIDLib::TCard4 c4Count) const
{
    // Do our parent first
    TParent::StreamTo(strmToWriteTo);

    //
    //  Stream out our members. Start our part with a frame marker and do
    //  a format version so we can auto-upgrade this data later.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                    << CIDLib_SysMemBuf::c2FmtVersion
                    << c4Count
                    << m_eAccess
                    << m_c4MaxSize;

    // Now stream out the actual data
    strmToWriteTo.c4WriteRawBuffer(m_pc1Data, c4Count);

    // And end up with and end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}



// ---------------------------------------------------------------------------
//  TSysBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Returns the number of pages we've allocate so far. See the class header
//  caveats section.
//
tCIDLib::TCard4 TSysBuf::c4AllocatedPages() const
{
    return TRawMem::c4PagesCovered(m_c4Size);
}


tCIDLib::EMemAccFlags TSysBuf::eMemAccFlags() const
{
    return m_eAccess;
}


// ---------------------------------------------------------------------------
//  TSysBuf: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1* TSysBuf::pc1QueryBuf()
{
    return m_pc1Data;
}

const tCIDLib::TCard1* TSysBuf::pc1QueryBuf() const
{
    return m_pc1Data;
}


tCIDLib::TCard1*
TSysBuf::pc1QueryBufInfo(tCIDLib::TCard4& c4CurSize, tCIDLib::TCard4& c4MaxSize)
{
    c4CurSize = m_c4Size;
    c4MaxSize = m_c4MaxSize;
    return m_pc1Data;
}

const tCIDLib::TCard1*
TSysBuf::pc1QueryBufInfo(  tCIDLib::TCard4&    c4CurSize
                           , tCIDLib::TCard4&  c4MaxSize) const
{
    c4CurSize = m_c4Size;
    c4MaxSize = m_c4MaxSize;
    return m_pc1Data;
}


tCIDLib::TVoid
TSysBuf::QueryBufInfo(  tCIDLib::TCard4&    c4CurSize
                        , tCIDLib::TCard4&  c4MaxSize) const
{
    c4CurSize = m_c4Size;
    c4MaxSize = m_c4MaxSize;
}

tCIDLib::TVoid
TSysBuf::Realloc(   const   tCIDLib::TCard4     c4NewSize
                    , const tCIDLib::TBoolean   ) const
{
    #if CID_DEBUG_ON
    // This should have been checked by our parent, but if debugging
    if (c4NewSize < m_c4Size)
    {
    }
    #endif

    //
    //  Since we commit in pages, but we maintain a byte oriented size
    //  for semantic reasons, its possible that our parent can call us to
    //  reallocate, but we really don't need to. If the new size fits
    //  within our existing pages, we can just increase our size value
    //  and leave it at that.
    //
    const tCIDLib::TCard4 c4CurPages = TRawMem::c4PagesCovered(m_c4Size);
    const tCIDLib::TCard4 c4NewPages = TRawMem::c4PagesCovered(c4NewSize);

    if (c4NewPages > c4CurPages)
    {
        //
        //  Get the base page addr of the next page after our current
        //  allocation.
        //
        tCIDLib::TVoid* pNext = TRawMem::pNextPageAdr(m_pc1Data + m_c4Size);
        if (!TRawMem::bCommitPages(pNext, c4NewPages - c4CurPages))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_Commit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(m_c4Size)
                , TCardinal(c4NewSize)
            );
        }
    }

    // Store the new actual size now
    m_c4Size = c4NewSize;
}

tCIDLib::TVoid TSysBuf::StreamFrom(TBinInStream& strmToReadFrom)
{
    //  Delete the current buffer
    if (m_pc1Data)
    {
        if (!TRawMem::bFreeSysMem(m_pc1Data))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_FreeSys
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
            );
        }
    }

    // Do our parent first
    TParent::StreamFrom(strmToReadFrom);

    // We should get a frame marker
    strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDLib_SysMemBuf::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Get the new info out
    strmToReadFrom >> m_c4Size
                   >> m_eAccess
                   >> m_c4MaxSize;

    // Create the buffer
    Create();

    // Now read in the actual data
    strmToReadFrom.c4ReadRawBuffer(m_pc1Data, m_c4Size);

    // We should get an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TSysBuf::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Just call the StreamCount() version with the full size
    StreamCount(strmToWriteTo, m_c4Size);
}


// ---------------------------------------------------------------------------
//  TSysBuf: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method will allocate the buffer. It will allocate the buffer to
//  the max size and commit it up to the current size.
//
tCIDLib::TVoid TSysBuf::Create()
{
    tCIDLib::TVoid* pBuf = nullptr;

    // Do a sanity check on the buffer
    if (!m_c4MaxSize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_ZeroSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    //
    //  If the current and max size is the same, then we make it more
    //  efficient by reserving and commiting all at once. Otherwise we
    //  just reserve the max size and commit below.
    ///
    TRawMem::EMemAllFlags eAlloc = TRawMem::EMemAllFlags::Reserve;

    if (m_c4MaxSize == m_c4Size)
        eAlloc = TRawMem::EMemAllFlags::Commit;

    if (!TRawMem::bAllocSysMem(m_c4MaxSize, m_eAccess, eAlloc, pBuf))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_AllocSys
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(m_c4MaxSize)
        );
    }

    // Store the buffer pointer
    m_pc1Data = static_cast<tCIDLib::TCard1*>(pBuf);

    //
    //  Now if there is any current size and its not the same as the max
    //  size, we want to commit that part of the buffer.
    //
    if (m_c4Size && (m_c4Size != m_c4MaxSize))
    {
        if (!TRawMem::bCommitPages( pBuf
                                    , TRawMem::c4PagesCovered(m_c4Size)
                                    , m_eAccess))
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_Commit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TInteger(0)
                , TCardinal(m_c4Size)
            );
        }
    }
}

