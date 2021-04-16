//
// FILE NAME: CIDLib_SharedMemory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/1997
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
//  This method provides a derivative of TMemBuf that manages named, shared
//  memory buffers. They are much like TSysBuf objects, but are named and
//  can be shared between processes.
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
RTTIDecls(TSharedMemBuf,TMemBuf)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_SharedMemory
{
    namespace
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TSharedMemBuf
//  PREFIX: mbuf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSharedMemBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TSharedMemBuf::TSharedMemBuf(const  tCIDLib::TCard4         c4InitSize
                            , const tCIDLib::TCard4         c4MaxSize
                            , const TResourceName&          rsnToUse
                            ,       tCIDLib::TBoolean&      bCreated
                            , const tCIDLib::EMemAccFlags   eAccessFlags
                            , const tCIDLib::ECreateActs eCreate) :
    m_c4Size(c4InitSize)
    , m_rsnThis(rsnToUse)
{
    //
    //  Validate the size, but not the expand increment which is set
    //  implicitly by us and can never be wrong.
    //
    ValidateSizes(c4InitSize, c4MaxSize);

    //
    //  Determine if we should just pre-commit. If the number of pages needed
    //  to hold the init and max size are the same, we can.
    //
    const tCIDLib::TCard4       c4InitPages = TRawMem::c4PagesCovered(c4InitSize);
    const tCIDLib::TCard4       c4MaxPages  = TRawMem::c4PagesCovered(c4MaxSize);
    const tCIDLib::EAllocTypes  eAlloc      = (c4InitPages == c4MaxPages)
                                              ? tCIDLib::EAllocTypes::Commit
                                              : tCIDLib::EAllocTypes::Lazy;

    if (!m_ksmbThis.bAlloc
    (
        m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory).pszBuffer()
        , c4MaxSize
        , eAlloc
        , eAccessFlags
        , bCreated
        , eCreate))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_AllocShared
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4MaxSize)
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory)
        );
    }

    //
    //  If we created it and its not a fully commited one, then commit the
    //  pages that are required for the initial size. If we didn't create
    //  it, then see if the init size is larger than the allocated size
    //  that we found on the existing buffer, and expand if so.
    //
    if (bCreated && (eAlloc == tCIDLib::EAllocTypes::Lazy))
    {
        if (!m_ksmbThis.bCommitToSize(c4InitSize))
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
                , TCardinal(c4InitSize)
            );
        }
    }
     else if (c4InitSize > (m_ksmbThis.c4AllocatedPages() * kCIDLib::c4MemPageSize))
    {
        if (!m_ksmbThis.bCommitToSize(c4InitSize))
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
                , TCardinal(c4InitSize)
            );
        }
    }
}

TSharedMemBuf::TSharedMemBuf(const TSharedMemBuf& mbufToCopy) :

    TMemBuf(mbufToCopy)
    , m_c4Size(mbufToCopy.m_c4Size)
    , m_rsnThis(mbufToCopy.m_rsnThis)
{
    if (!m_ksmbThis.bDuplicate(mbufToCopy.m_ksmbThis))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_AllocShared
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory)
        );
    }
}

TSharedMemBuf::~TSharedMemBuf()
{
    //
    //  If we have a buffer, then free it. We cannot just let it destruct
    //  because we have to catch any errors and translate them to CIDLib
    //  level errors.
    //
    if (m_ksmbThis.pData())
    {
        if (!m_ksmbThis.bFree())
        {
            // Log as a warning, so it does not propogate out of destructor
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_FreeShared
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory)
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TSharedMemBuf: Public operators
// ---------------------------------------------------------------------------
TSharedMemBuf& TSharedMemBuf::operator=(const TSharedMemBuf& mbufToAssign)
{
    if (this == &mbufToAssign)
        return *this;

    // Do our parent first
    TParent::operator=(mbufToAssign);

    // If there is an existing buffer, then free it
    if (m_ksmbThis.pData())
    {
        if (!m_ksmbThis.bFree())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_FreeShared
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory)
            );
        }
    }

    // Copy all our members over now
    m_c4Size    = mbufToAssign.m_c4Size;
    m_rsnThis   = mbufToAssign.m_rsnThis;

    // And duplicate the buffer
    if (!m_ksmbThis.bDuplicate(mbufToAssign.m_ksmbThis))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_DupShared
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory)
        );
    }
    return *this;
}


tCIDLib::TBoolean
TSharedMemBuf::operator!=(const TSharedMemBuf& mbufToTest) const
{
    return !operator==(mbufToTest);
}

tCIDLib::TBoolean
TSharedMemBuf::operator==(const TSharedMemBuf& mbufToTest) const
{
    if (this == &mbufToTest)
        return kCIDLib::True;

    if ((m_c4Size == mbufToTest.m_c4Size)
    &&  (m_ksmbThis == mbufToTest.m_ksmbThis)
    &&  (m_rsnThis == mbufToTest.m_rsnThis)
    &&   TParent::operator==(mbufToTest))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TSharedMemBuf: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  This is the same as the StreamTo() method we inherit from the streamable
//  interface, but sometimes we want to only stream part of the current size
//  of the buffer, though it cannot be done generically via the streamable
//  interface. In order to avoid redundant code, StreamTo() just calls us with
//  the current allocation size.
//
tCIDLib::TVoid TSharedMemBuf::StreamCount(       TBinOutStream&  strmToWriteTo
                                         , const tCIDLib::TCard4 c4Count) const
{
    // Do our parent first
    TParent::StreamTo(strmToWriteTo);

    //
    //  Stream out our members. Start our part with a frame marker and do
    //  a format version so we can auto-upgrade this data later.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                    << CIDLib_SharedMemory::c2FmtVersion
                    << c4Count
                    << m_ksmbThis.eAccess()
                    << m_ksmbThis.eAllocType()
                    << m_ksmbThis.c4MaxSize()
                    << m_rsnThis;

    // Now stream out the actual data
    strmToWriteTo.c4WriteRawBuffer(m_ksmbThis.pData(), c4Count);

    // And end up with and end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TSharedMemBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EMemAccFlags TSharedMemBuf::eAccess() const
{
    return m_ksmbThis.eAccess();
}


tCIDLib::TCard4 TSharedMemBuf::c4AllocatedPages() const
{
    return m_ksmbThis.c4AllocatedPages();
}


const TResourceName& TSharedMemBuf::rsnName() const
{
    return m_rsnThis;
}


//
//  This is a shared buffer, so it's possible for others to change it and
//  force it to commit more pages. We cache the current size for efficiency,
//  but we have to provide a way for apps to force us to resync with the
//  actual system buffer status.
//
tCIDLib::TVoid TSharedMemBuf::SyncView()
{
    if (!m_ksmbThis.bSyncView())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_SyncView
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Set our size to the allocated pages times the page size
    m_c4Size = m_ksmbThis.c4AllocatedPages() * kCIDLib::c4MemPageSize;
}



// ---------------------------------------------------------------------------
//  TSharedMemBuf Hidden Constructors
//
//  Needed for polymorphic streaming. Note that it will leave the object in
//  a bad condition.
// ---------------------------------------------------------------------------
TSharedMemBuf::TSharedMemBuf() :

    TMemBuf()
    , m_c4Size(0)
{
}


// ---------------------------------------------------------------------------
//  TSharedMemBuf: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1* TSharedMemBuf::pc1QueryBuf()
{
    return static_cast<tCIDLib::TCard1*>(m_ksmbThis.pData());
}

const tCIDLib::TCard1* TSharedMemBuf::pc1QueryBuf() const
{
    return static_cast<const tCIDLib::TCard1*>(m_ksmbThis.pData());
}


tCIDLib::TCard1*
TSharedMemBuf::pc1QueryBufInfo( tCIDLib::TCard4&    c4CurSize
                                , tCIDLib::TCard4&  c4MaxSize)
{
    c4CurSize = m_c4Size;
    c4MaxSize = m_ksmbThis.c4MaxSize();
    return static_cast<tCIDLib::TCard1*>(m_ksmbThis.pData());
}

const tCIDLib::TCard1*
TSharedMemBuf::pc1QueryBufInfo(  tCIDLib::TCard4&    c4CurSize
                            , tCIDLib::TCard4&  c4MaxSize) const
{
    c4CurSize = m_c4Size;
    c4MaxSize = m_ksmbThis.c4MaxSize();
    return static_cast<tCIDLib::TCard1*>(m_ksmbThis.pData());
}


tCIDLib::TVoid
TSharedMemBuf::QueryBufInfo(tCIDLib::TCard4&    c4CurSize
                            , tCIDLib::TCard4&  c4MaxSize) const
{
    c4CurSize = m_c4Size;
    c4MaxSize = m_ksmbThis.c4MaxSize();
}


tCIDLib::TVoid
TSharedMemBuf::Realloc( const   tCIDLib::TCard4     c4NewSize
                        , const tCIDLib::TBoolean   bPreserve) const
{
    //
    //  Since we commit in pages, but we maintain a byte oriented size
    //  for semantic reasons, its possible that our parent can call us to
    //  reallocate, but we really don't need to. If the new size fits
    //  within our existing pages, we can just increase our size value
    //  and leave it at that.
    //
    const tCIDLib::TCard4 c4NewPages = TRawMem::c4PagesCovered(c4NewSize);

    tCIDLib::TCard4 c4CurPages = m_ksmbThis.c4AllocatedPages();
    if (c4CurPages < c4NewPages)
    {
        //
        //  First, see if someone else has already expanded the buffer, in
        //  which case we can just pick up that existing data.
        //
        if (!m_ksmbThis.bSyncView())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_SyncView
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        //
        //  If it filled the whole need, then just bump our size to the
        //  requested size. If we are to preserve, then we can then just
        //  return, since we just picked up existing content that is
        //  already in use.
        //
        c4CurPages = m_ksmbThis.c4AllocatedPages();
        if (c4CurPages >= c4NewPages)
        {
            m_c4Size = c4NewSize;

            if (bPreserve)
                return;
        }

        // Still need to expand a bit more
        if (!m_ksmbThis.bCommitToSize(c4NewSize))
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


tCIDLib::TVoid TSharedMemBuf::StreamFrom(TBinInStream& strmToReadFrom)
{
    //  Delete the current buffer
    if (m_ksmbThis.pData())
    {
        if (!m_ksmbThis.bFree())
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcMBuf_FreeShared
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory)
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
    if (c2FmtVersion != CIDLib_SharedMemory::c2FmtVersion)
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
    tCIDLib::TCard4         c4MaxSize;
    tCIDLib::EMemAccFlags   eAccess;
    tCIDLib::EAllocTypes    eAllocType;

    strmToReadFrom  >> m_c4Size
                    >> eAccess
                    >> eAllocType
                    >> c4MaxSize
                    >> m_rsnThis;

    // Create the buffer
    tCIDLib::TBoolean bCreated;
    if (!m_ksmbThis.bAlloc
    (
        m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory).pszBuffer()
        , c4MaxSize
        , eAllocType
        , eAccess
        , bCreated
        , tCIDLib::ECreateActs::OpenOrCreate))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_AllocShared
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_rsnThis.strFullName(tCIDLib::ENamedRscTypes::Memory)
        );
    }

    // Commit up to where it was before
    if (!m_ksmbThis.bCommitToSize(m_c4Size))
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

    // Now read in the actual data
    strmToReadFrom.c4ReadRawBuffer(m_ksmbThis.pData(), m_c4Size);

    // We should get an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TSharedMemBuf::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Just call the StreamCount() method with our alloc size
    StreamCount(strmToWriteTo, m_c4Size);
}


