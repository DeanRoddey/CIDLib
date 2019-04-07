//
// FILE NAME: CIDOrb_Reply.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/08/2001
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
//  This is implementation file for the reply class, which is used to
//  send back replies from the server to the client.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDOrb_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TOrbReply, TObject)


namespace CIDOrb_Reply
{
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//   CLASS: TOrbReply
//  PREFIX: orbr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbReply: Constructor and Destructor
// ---------------------------------------------------------------------------
TOrbReply::TOrbReply(const tCIDLib::TCard4 c4InitSz) :

    m_bRetStatus(kCIDLib::False)
    , m_c4SequenceId(0)
    , m_mbufData(c4InitSz, kCIDOrb::c4MaxPayload, kCIDLib::c4Sz_1K)
    , m_strmOut(&m_mbufData)
    , m_strmIn(m_strmOut)
{
}

TOrbReply::~TOrbReply()
{
}


// ---------------------------------------------------------------------------
//  TOrbReply: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbReply::Reset(const tCIDLib::TCard4 c4Size)
{
    m_bRetStatus = kCIDLib::False;

    //
    //  If our buffer got expanded out big, reset it so that the few large chunks
    //  that might get transfered don't eventually end up making all of them large.
    //
    //  If it's smaller than requested, reset it up, with some extra
    //
    if (m_mbufData.c4Size() > kCIDLib::c4Sz_16K)
        m_mbufData.Reset(kCIDLib::c4Sz_1K, kCIDOrb::c4MaxPayload, kCIDLib::c4Sz_1K);
    else if (m_mbufData.c4Size() < c4Size)
        m_mbufData.Reset(c4Size + 64, kCIDOrb::c4MaxPayload, kCIDLib::c4Sz_1K);

    // And now reset our streams
    m_strmOut.Reset();
    m_strmIn.Reset();
}


// ---------------------------------------------------------------------------
//  TOrbReply: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TOrbReply::bRetStatus() const
{
    return m_bRetStatus;
}

tCIDLib::TBoolean TOrbReply::bRetStatus(const tCIDLib::TBoolean bNew)
{
    m_bRetStatus = bNew;
    return m_bRetStatus;
}


//
//  Return the current allocation size of our data buffer, mainly for the pool
//  of buffers the facility class creates.
//
tCIDLib::TCard4 TOrbReply::c4BufSize() const
{
    return m_mbufData.c4Size();
}


//
//  This lets the caller know how many bytes have been formatted out into
//  our output buffer.
//
tCIDLib::TCard4 TOrbReply::c4PayloadBytes()
{
    m_strmOut.Flush();
    return m_strmOut.c4CurPos();
}


tCIDLib::TCard4 TOrbReply::c4SequenceId() const
{
    return m_c4SequenceId;
}

tCIDLib::TCard4 TOrbReply::c4SequenceId(const tCIDLib::TCard4 c4ToSet)
{
    m_c4SequenceId = c4ToSet;
    return m_c4SequenceId;
}


TBinMBufInStream& TOrbReply::strmIn()
{
    return m_strmIn;
}

TBinMBufOutStream& TOrbReply::strmOut()
{
    return m_strmOut;
}


// ---------------------------------------------------------------------------
//  TOrbReply: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbReply::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDOrb_Reply::c2FmtVersion)
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

    // Get out the return stats, and sequence id
    strmToReadFrom  >> m_bRetStatus
                    >> m_c4SequenceId;

    //
    //  Looks ok, so we don't have to do anything, since we already have that
    //  value stored. Next, get the byte count out, and then get the safety
    //  value and check it.
    //
    tCIDLib::TCard4 c4Size, c4Size2;
    strmToReadFrom >> c4Size >> c4Size2;

    if (c4Size != (kCIDLib::c4MaxCard ^ c4Size2))
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcCmd_BadPacketSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Read in the buffer data
    strmToReadFrom.c4ReadBuffer(m_mbufData, c4Size);

    // And we end with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

    //
    //  And now reset our input stream, so that subsequent reads by the
    //  outside world will start streaming in the data we just stored.
    //
    m_strmIn.Reset();
    m_strmIn.SetEndIndex(c4Size);
}


tCIDLib::TVoid TOrbReply::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Flush the output stream so we'll be sure the data is in the buffer
    m_strmOut.Flush();

    //
    //  We need to use our current stream position a couple times below,
    //  so get a copy up front.
    //
    const tCIDLib::TCard4 c4Bytes = m_strmOut.c4CurPos();

    //
    //  Stream out the return status, sequence id, and the current stream
    //  position (aka the number of bytes we streamed out to our buffer.) For
    //  safety, we write XOR of the byte count next.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDOrb_Reply::c2FmtVersion
                    << m_bRetStatus
                    << m_c4SequenceId
                    << c4Bytes
                    << (kCIDLib::c4MaxCard ^ c4Bytes);

    //
    //  And now stream the data. We don't stream the memory buffer itself,
    //  since the allocation of the buffer will often be larger than the data
    //  that is valid. So we do a raw buffer stream of just the bytes that
    //  are needed. This is ok, becuause this data is already in canonical
    //  format.
    //
    strmToWriteTo.c4WriteBuffer(m_mbufData, c4Bytes);

    // End with ane end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}


