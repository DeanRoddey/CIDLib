//
// FILE NAME: CIDLib_BinaryMBufStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
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
//  This file implements simple derivatives of the binary input and output
//  stream classes. These handle creating the stream implementation object and
//  getting the memory buffer created where applicable. You can do this
//  yourself, but this is safer.
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
RTTIDecls(TBinMBufInStream,TBinInStream)
RTTIDecls(TBinMBufOutStream,TBinOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TBinMBufInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinMBufInStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinMBufInStream::TBinMBufInStream( const   tCIDLib::TCard4         c4InitSize
                                    , const tCIDLib::TCard4         c4MaxSize) :
    m_pstrmiMem(nullptr)
{
    // Create the system buffer that becomes the basis for the stream
    THeapBuf* pmbufData = nullptr;
    TMemInStreamImpl* pstrmiIn = nullptr;
    try
    {
        pmbufData = new THeapBuf
        (
            c4InitSize
            , c4MaxSize
            , tCIDLib::MinVal<tCIDLib::TCard4>(c4MaxSize / 16, 0x100000)
        );

        // Tell the implementation object to adopt the
        pstrmiIn = new TMemInStreamImpl(pmbufData, 0, tCIDLib::EAdoptOpts::Adopt);
    }

    catch(TError& errToCatch)
    {
        if (pstrmiIn)
            delete pstrmiIn;
        else
            delete pmbufData;

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Pass on the new stream implementation object to our base class
    AdoptImplObject(pstrmiIn);

    // And store our version
    m_pstrmiMem = pstrmiIn;
}

TBinMBufInStream::TBinMBufInStream( const   TMemBuf* const      pmbufToUse
                                    , const tCIDLib::TCard4     c4InitLogicalEnd
                                    , const tCIDLib::EAdoptOpts eAdopt) :
    m_pstrmiMem(nullptr)
{
    TMemInStreamImpl* pstrmiIn = nullptr;
    try
    {
        CIDAssert(pmbufToUse->c4Size() >= c4InitLogicalEnd, L"Buffer is smaller than logical end");
        pstrmiIn = new TMemInStreamImpl(pmbufToUse, c4InitLogicalEnd, eAdopt);
    }

    catch(TError& errToCatch)
    {
        // If we created the impl, delete that, else the buffer if we are adopting
        if (pstrmiIn)
            delete pstrmiIn;
        else if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
            delete pmbufToUse;

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Pass the impl object to our parent for adoption
    AdoptImplObject(pstrmiIn);

    // And store our version
    m_pstrmiMem = pstrmiIn;
}


TBinMBufInStream::TBinMBufInStream(         THeapBuf&&          mbufToTake
                                    , const tCIDLib::TCard4     c4InitLogicalEnd) :
    m_pstrmiMem(nullptr)
{
    CIDAssert(mbufToTake.c4Size() >= c4InitLogicalEnd, L"Buffer is smaller than logical end");

    TMemInStreamImpl* pstrmiIn = nullptr;
    THeapBuf* pmbufData = nullptr;
    try
    {
        //
        //  Create an impl object for the passed buffer, giving it the contents
        //  of the passed buffer.
        //
        pmbufData = new THeapBuf(tCIDLib::ForceMove(mbufToTake));
        pstrmiIn = new TMemInStreamImpl
        (
            pmbufData, c4InitLogicalEnd, tCIDLib::EAdoptOpts::Adopt
        );
    }

    catch(TError& errToCatch)
    {
        // If we created the impl delet that, else delete the buffer
        if (pstrmiIn)
            delete pstrmiIn;
        else
            delete pmbufData;

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Pass the impl object to our parent for adoption
    AdoptImplObject(pstrmiIn);

    // And store our version
    m_pstrmiMem = pstrmiIn;
}


TBinMBufInStream::TBinMBufInStream( const   tCIDLib::TCard1* const  pc1InitData
                                    , const tCIDLib::TCard4         c4Count) :
    m_pstrmiMem(nullptr)
{
    TMemInStreamImpl* pstrmiIn = nullptr;
    THeapBuf* pmbufNew = nullptr;
    try
    {
        //
        //  Create an impl object for the passed buffer. In this one we have
        //  to create a memory buffer object with the passed initial content
        //  and initial size big enough to hold it. We then give that to the
        //  impl object and tell him to adopt it.
        //
        pmbufNew = new THeapBuf(pc1InitData, c4Count);
        pstrmiIn = new TMemInStreamImpl
        (
            pmbufNew, c4Count, tCIDLib::EAdoptOpts::Adopt
        );
    }

    catch(TError& errToCatch)
    {
        // If we created the impl delet that, else delete the buffer
        if (pstrmiIn)
            delete pstrmiIn;
        else
            delete pmbufNew;

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Pass the impl object to our parent for adoption
    AdoptImplObject(pstrmiIn);

    // And store our version
    m_pstrmiMem = pstrmiIn;
}

TBinMBufInStream::TBinMBufInStream(const TBinMBufOutStream& strmSyncTo) :

    m_pstrmiMem(nullptr)
{
    // Create a linked impl object
    m_pstrmiMem = new TMemInStreamImpl(strmSyncTo.strmiThis());

    // Let our parent adopt it
    AdoptImplObject(m_pstrmiMem);
}

TBinMBufInStream::~TBinMBufInStream()
{
    // We don't own the impl object
}


// ---------------------------------------------------------------------------
//  TBinMBufInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TBinMBufInStream::c4CurSize() const
{
    return m_pstrmiMem->c4CurSize();
}


const TMemBuf& TBinMBufInStream::mbufData() const
{
    return m_pstrmiMem->mbufData();
}


const TMemInStreamImpl& TBinMBufInStream::strmiThis() const
{
    return *m_pstrmiMem;
}

TMemInStreamImpl& TBinMBufInStream::strmiThis()
{
    return *m_pstrmiMem;
}


tCIDLib::TVoid TBinMBufInStream::SetEndIndex(const tCIDLib::TCard4 c4NewIndex)
{
    m_pstrmiMem->SetEndIndex(c4NewIndex);
}



// ---------------------------------------------------------------------------
//   CLASS: TBinMBufOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinMBufOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinMBufOutStream::TBinMBufOutStream(const  tCIDLib::TCard4 c4InitSize
                                    , const tCIDLib::TCard4 c4MaxSize) :
    m_pstrmiMem(nullptr)
{
    // Create the system buffer that becomes the basis for the stream
    THeapBuf* pmbufData = nullptr;
    TMemOutStreamImpl* pstrmiOut = nullptr;
    try
    {
        // Create the buffer
        pmbufData = new THeapBuf
        (
            c4InitSize
            , c4MaxSize
            , tCIDLib::MinVal<tCIDLib::TCard4>(c4MaxSize / 16, 0x100000)
        );

        // Tell the implementation object to adopt the buffer
        pstrmiOut = new TMemOutStreamImpl(pmbufData, 0, tCIDLib::EAdoptOpts::Adopt);
    }

    catch(TError& errToCatch)
    {
        if (pstrmiOut)
            delete pstrmiOut;
        else
            delete pmbufData;

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Pass on the new stream implementation object to our base class
    AdoptImplObject(pstrmiOut);

    // And store our copy of it
    m_pstrmiMem = pstrmiOut;
}

TBinMBufOutStream::TBinMBufOutStream(       TMemBuf* const      pmbufToUse
                                    , const tCIDLib::EAdoptOpts eAdopt) :
    m_pstrmiMem(nullptr)
{
    TMemOutStreamImpl* pstrmiOut = nullptr;
    try
    {
        // Create the stream implementation object for the passed buffer
        pstrmiOut = new TMemOutStreamImpl(pmbufToUse, 0, eAdopt);
    }

    catch(TError& errToCatch)
    {
        if (pstrmiOut)
            delete pstrmiOut;
        else if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
            delete pmbufToUse;

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Pass the impl object to our parent for adoption
    AdoptImplObject(pstrmiOut);

    // And store our copy of it
    m_pstrmiMem = pstrmiOut;
}

TBinMBufOutStream::~TBinMBufOutStream()
{
    // We don't own the impl object
}


// ---------------------------------------------------------------------------
//  TBinMBufOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TBinMBufOutStream::c4CurSize() const
{
    return m_pstrmiMem->c4CurSize();
}


const TMemBuf& TBinMBufOutStream::mbufData() const
{
    return m_pstrmiMem->mbufData();
}


const TMemOutStreamImpl& TBinMBufOutStream::strmiThis() const
{
    return *m_pstrmiMem;
}

TMemOutStreamImpl& TBinMBufOutStream::strmiThis()
{
    return *m_pstrmiMem;
}


tCIDLib::TVoid TBinMBufOutStream::TruncateAt(const tCIDLib::TCard4 c4At)
{
    //
    //  Flush the cache first, then truncate it. It could fail if the buffer
    //  is full but some is still in the cache, so catch that.
    //
    try
    {
        Flush();
    }

    catch(...)
    {
    }
    m_pstrmiMem->TruncateAt(c4At);
}


