//
// FILE NAME: CIDLib_TextMBufStream.cpp
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
//  This file implements simple derivatives of the text stream classes, doing
//  the grunt work setup so that its safe and easy.
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
RTTIDecls(TTextMBufInStream,TTextInStream)
RTTIDecls(TTextMBufOutStream,TTextOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TTextMBufInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextMBufInStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextMBufInStream::
TTextMBufInStream(  const   tCIDLib::TCard4         c4InitSize
                    , const tCIDLib::TCard4         c4MaxSize
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmiBuf(nullptr)
{
    // Create the system buffer that
    THeapBuf* pmbufData = nullptr;
    TBinInStream* pstrmThis = nullptr;
    try
    {
        // Create memory buffer, binary stream impl for it, and a stream for that
        pmbufData = new THeapBuf
        (
            c4InitSize
            , c4MaxSize
            , tCIDLib::MinVal<tCIDLib::TCard4>(c4MaxSize / 16, 0x100000)
        );
        m_pstrmiBuf = new TMemInStreamImpl(pmbufData, 0, tCIDLib::EAdoptOpts::Adopt);
        pstrmThis = new TBinInStream(m_pstrmiBuf);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        //
        //  Clean up what we got created. Since adoption is going on, only
        //  delete the highest level one that got created.
        //
        if (pstrmThis)
            delete pstrmThis;
         else if (m_pstrmiBuf)
            delete m_pstrmiBuf;
         else
            delete pmbufData;
        throw;
    }

    AdoptStream(pstrmThis);
}

TTextMBufInStream::
TTextMBufInStream(  const   TMemBuf* const          pmbufToUse
                    , const tCIDLib::TCard4         c4InitLogicalEnd
                    , const tCIDLib::EAdoptOpts     eAdopt
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmiBuf(nullptr)
{
    TBinInStream* pstrmThis = nullptr;
    try
    {
        m_pstrmiBuf = new TMemInStreamImpl(pmbufToUse, c4InitLogicalEnd, eAdopt);
        pstrmThis = new TBinInStream(m_pstrmiBuf);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        if (pstrmThis)
        {
            delete pstrmThis;
        }
         else if (m_pstrmiBuf)
        {
            delete m_pstrmiBuf;
        }
         else
        {
            // If we owned the buffer then clean it up
            if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete pmbufToUse;
        }
        throw;
    }

    AdoptStream(pstrmThis);
}

TTextMBufInStream::
TTextMBufInStream(          THeapBuf&&              mbufToUse
                    , const tCIDLib::TCard4         c4InitLogicalEnd
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmiBuf(nullptr)
{
    TBinInStream* pstrmThis = nullptr;
    try
    {
        m_pstrmiBuf = new TMemInStreamImpl
        (
            tCIDLib::ForceMove(mbufToUse), c4InitLogicalEnd
        );
        pstrmThis = new TBinInStream(m_pstrmiBuf);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        if (pstrmThis)
        {
            delete pstrmThis;
        }
         else if (m_pstrmiBuf)
        {
            delete m_pstrmiBuf;
        }
        throw;
    }

    AdoptStream(pstrmThis);
}

TTextMBufInStream::TTextMBufInStream(const TTextMBufOutStream& strmToSyncWith) :

    TTextInStream(::pDupObject<TTextConverter>(strmToSyncWith.tcvtThis()))
    , m_pstrmiBuf(nullptr)
{
    //
    //  Create a stream impl object that is synchronized with the impl object
    //  for the binary stream underlying the passed text stream.
    //
    m_pstrmiBuf = new TMemInStreamImpl(*strmToSyncWith.m_pstrmiThis);

    //
    //  Now create the binary stream with this impl object and tell our parent
    //  about it.
    //
    AdoptStream(new TBinInStream(m_pstrmiBuf));
}

TTextMBufInStream::~TTextMBufInStream()
{
}


// ---------------------------------------------------------------------------
//  TTextMBufInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Update the stream impl with the new size, and then reset ourself so that we
//  start reading from the start again.
//
tCIDLib::TVoid TTextMBufInStream::SetEndIndex(const tCIDLib::TCard4 c4NewIndex)
{
    m_pstrmiBuf->SetEndIndex(c4NewIndex);
    Reset();
}



// ---------------------------------------------------------------------------
//   CLASS: TTextMBufOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextMBufOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextMBufOutStream::
TTextMBufOutStream( const   tCIDLib::TCard4         c4InitSize
                    , const tCIDLib::TCard4         c4MaxSize
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextOutStream(ptcvtToAdopt)
    , m_pstrmiThis(nullptr)
{
    // Create the system buffer that
    THeapBuf* pmbufData = nullptr;
    TBinOutStream* pstrmThis = nullptr;
    try
    {
        // Create the memory buffer and then a binary stream impl for it
        pmbufData = new THeapBuf
        (
            c4InitSize
            , c4MaxSize
            , tCIDLib::MinVal<tCIDLib::TCard4>(c4MaxSize / 16, 0x100000)
        );
        m_pstrmiThis = new TMemOutStreamImpl(pmbufData, 0, tCIDLib::EAdoptOpts::Adopt);

        // And now create a binary stream for it
        pstrmThis = new TBinOutStream(m_pstrmiThis);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        //
        //  Clean up what we got created. Since adoption is going on, only
        //  delete the highest level one that got created.
        //
        if (pstrmThis)
            delete pstrmThis;
         else if (m_pstrmiThis)
            delete m_pstrmiThis;
         else
            delete pmbufData;
        throw;
    }

    AdoptStream(pstrmThis);
}

TTextMBufOutStream::
TTextMBufOutStream(         TMemBuf* const          pmbufToUse
                    , const tCIDLib::EAdoptOpts     eAdopt
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextOutStream(ptcvtToAdopt)
    , m_pstrmiThis(nullptr)
{
    TBinOutStream* pstrmThis = nullptr;
    try
    {
        m_pstrmiThis = new TMemOutStreamImpl(pmbufToUse, 0, eAdopt);
        pstrmThis = new TBinOutStream(m_pstrmiThis);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        if (pstrmThis)
            delete pstrmThis;
        else if (m_pstrmiThis)
            delete m_pstrmiThis;
        else
        {
            // If we owned the buffer then clean it up
            if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
                delete pmbufToUse;
        }
        throw;
    }

    AdoptStream(pstrmThis);
}

TTextMBufOutStream::~TTextMBufOutStream()
{
}


// ---------------------------------------------------------------------------
//  TTextMBufOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TTextMBufOutStream::c4CurSize() const
{
    return m_pstrmiThis->c4CurSize();
}


const TMemBuf& TTextMBufOutStream::mbufData() const
{
    return m_pstrmiThis->mbufData();
}




