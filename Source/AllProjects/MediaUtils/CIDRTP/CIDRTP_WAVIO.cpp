//
// FILE NAME: CIDRTP_WAVIO.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 3/12/2015
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
//  This file implements an I/O source derivative for WAV files.
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
#include    "CIDRTP_.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TFileWaveIO
// PREFIX: rtpio
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFileWaveIO: Constructors and Destructor
// ---------------------------------------------------------------------------
TFileWaveIO::TFileWaveIO(const  TString&            strPath
                        , const tCIDLib::TCard1     c1AVPFormat) :

    m_c1AVPPLType(c1AVPFormat)
    , m_c4ClockInc(0)
    , m_c4DataBytes(0)
    , m_c4DataPos(0)
    , m_c4PayloadBytes(0)
    , m_c4PayloadSamples(0)
    , m_strPath(strPath)
{
}

TFileWaveIO::~TFileWaveIO()
{
    // Make sure the file is closed
    try
    {
        m_flSrc.Close();
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TFileWaveIO: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFileWaveIO::bInitialized() const
{
    return m_flSrc.bIsOpen();
}


// Read up to a fully payload of bytes
tCIDLib::TCard4
TFileWaveIO::c4ReadSamples( TMemBuf&                mbufToFill
                            , COP tCIDLib::TCard4&  c4AtSample
                            , COP tCIDLib::TCard4&  c4AtMillisec)
{
    CIDAssert
    (
        (m_c4DataBytes != 0) && (m_c4PayloadBytes != 0)
        , L"The WAV I/O object has not been properly initialized"
    );

    // See how much we can read, up to a fully payload size
    tCIDLib::TCard4 c4ThisTime = m_c4PayloadBytes;
    if (m_c4DataPos + c4ThisTime > m_c4DataBytes)
        c4ThisTime = m_c4DataBytes - m_c4DataPos;

    //
    //  Give back the starting sample and millisecond values. Figure out how many
    //  payloads we've done so far, and that lets us figure the values.
    //
    //  DO THIS BEFORE we add to data pos below, because we want the STARTING
    //  values for this packet.
    //
    const tCIDLib::TCard4 c4PLIndex = m_c4DataPos / m_c4PayloadBytes;
    c4AtSample = c4PLIndex * m_c4ClockInc;
    c4AtMillisec = c4PLIndex * 20;

    if (c4ThisTime)
    {
        // Add this to the bytes read and then read the bytes
        m_c4DataPos += c4ThisTime;
        m_flSrc.c4ReadBuffer(mbufToFill, c4ThisTime, tCIDLib::EAllData::FailIfNotAll);

        //
        //  If it's a multi-byte sample type, we have to byte flip all of the
        //  samples
        //
        if ((m_c1AVPPLType == 10) || (m_c1AVPPLType == 11))
        {
            tCIDLib::TCard4 c4Count = m_c4PayloadSamples;
            if (m_c1AVPPLType == 10)
                c4Count *= 2;

            tCIDLib::TCard1* pc1Ptr = mbufToFill.pc1Data();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                tCIDLib::TCard1 c1First = *pc1Ptr;
                *pc1Ptr = *(pc1Ptr + 1);
                pc1Ptr++;
                *pc1Ptr = c1First;
                pc1Ptr++;
            }
        }
    }

    return c4ThisTime;
}



//
//  Open a binary input stream over the WAV file, and skip over any WAV header
//  after verifying it's ULaw format.
//
tCIDLib::TVoid
TFileWaveIO::Initialize(COP tCIDLib::TCard1&    c1PayloadType
                        , COP tCIDLib::TCard4&  c4PayloadMSs
                        , COP tCIDLib::TCard4&  c4PayloadBytes
                        , COP tCIDLib::TCard4&  c4PayloadSamples
                        , COP tCIDLib::TCard4&  c4SampleBytes
                        , COP tCIDLib::TCard4&  c4TotalMSs
                        , COP tCIDLib::TCard4&  c4TotalBytes)
{
    // If the requested AVP format isn't one we support, give up now
    if ((m_c1AVPPLType != 0)
    &&  (m_c1AVPPLType != 8)
    &&  (m_c1AVPPLType != 10)
    &&  (m_c1AVPPLType != 11))
    {
        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcData_NotSupportedFmt
            , m_strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c1AVPPLType)
        );
    }

    // Set up the file path and try to open it
    m_flSrc.strName(m_strPath);
    m_flSrc.Open
    (
        tCIDLib::EAccessModes::Read
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::RandomAccess
    );

    //
    //  If it's not at least large enough for a header, it can't be valid,
    //  so we just read a header's worth and throw if we can't read that
    //  much.
    //
    tCIDMedia::TWaveHdr Hdr = {0};
    m_flSrc.c4ReadBuffer(&Hdr, sizeof(Hdr), tCIDLib::EAllData::FailIfNotAll);

    // Make sure it's of the sort we want
    if ((TRawStr::eCompareStrN(Hdr.szRiff, "RIFF", 4) != tCIDLib::ESortComps::Equal)
    ||  (TRawStr::eCompareStrN(Hdr.szWavFmt, "WAVEfmt ", 8) != tCIDLib::ESortComps::Equal))
    {
        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcData_NotWAVFile
            , m_strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // If there's extra data after the standard header, skip it
    if (Hdr.c4HdrLen > 16)
        m_flSrc.c8OffsetFilePos(Hdr.c4HdrLen - 16);

    //
    //  This is a chunked format, so we have to find the data chunk. We can't
    //  just assume the data starts after the header. We end up with c4CurPos
    //  at the start of the chunk and c4DataSz with the amount of audio data.
    //
    m_c4DataPos = tCIDLib::TCard4(m_flSrc.c8CurPos());
    c4TotalBytes = 0;
    {
        const tCIDLib::TCard4 c4DataId = 0x61746164;
        const tCIDLib::TCard4 c4FileSize = tCIDLib::TCard4(m_flSrc.c8CurSize());
        while (m_c4DataPos < c4FileSize)
        {
            // Read the current chunk header
            const tCIDLib::TCard4 c4CurId = c4ReadNext();
            const tCIDLib::TCard4 c4CurSz = c4ReadNext();

            //
            //  If it's the id we care about, then remember this as the data size
            //  and break out. We are now positioned at the start of the chunk.
            //
            if (c4CurId == c4DataId)
            {
                c4TotalBytes = c4CurSz;
                break;
            }
        }
    }

    // If we never found the data chunk, give up
    if (!c4TotalBytes)
    {
        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcData_NoWAVData
            , m_strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // If we are big endian, flip any multi-byte values we use
    #if CIDLIB_BIGENDIAN
    Hdr.c2NumChannels = TRawBits::c2SwapBytes(Hdr.c2NumChannels);
    Hdr.c4SampleRate = TRawBits::c4SwapBytes(Hdr.c4SampleRate);
    Hdr.c2BitsPerSample = TRawBits::c2SwapBytes(Hdr.c2BitsPerSample);
    #endif

    tCIDLib::TBoolean bGoodFmt = kCIDLib::False;
    tCIDLib::TCard4 c4ClockInc = 0;
    if ((m_c1AVPPLType == 0)
    ||  (m_c1AVPPLType == 8))
    {
        // Gotta be one of the 8K, mono, byte sized ones
        if ((Hdr.c2NumChannels == 1)
        &&  (Hdr.c4SampleRate == 8000)
        &&  (Hdr.c2BitsPerSample == 8))
        {
            c4PayloadBytes = 160;
            c4PayloadSamples = 160;
            c4SampleBytes = 1;
            c4ClockInc = 160;

            bGoodFmt = kCIDLib::True;
        }
    }
     else if (m_c1AVPPLType == 10)
    {
        // Stereo CD data
        if ((Hdr.c2NumChannels == 2)
        &&  (Hdr.c4SampleRate == 44100)
        &&  (Hdr.c2BitsPerSample == 16))
        {
            c4SampleBytes = 4;
            c4PayloadSamples = 882;
            c4ClockInc = 882; // * 2;
            c4PayloadBytes = c4PayloadSamples * c4SampleBytes;

            bGoodFmt = kCIDLib::True;
        }
    }
     else if (m_c1AVPPLType == 11)
    {
        // Mono CD data
        if ((Hdr.c2NumChannels == 1)
        &&  (Hdr.c4SampleRate == 44100)
        &&  (Hdr.c2BitsPerSample == 16))
        {
            c4SampleBytes = 2;
            c4PayloadSamples = 882;
            c4ClockInc = 882;
            c4PayloadBytes = c4PayloadSamples * c4SampleBytes;

            bGoodFmt = kCIDLib::True;
        }
    }

    if (!bGoodFmt || !c4ClockInc)
    {
        // Not something we support
        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcData_BadSrcFormat
            , m_strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c1AVPPLType)
        );
    }

    // We do 20ms for all of these
    c4PayloadMSs = 20;

    //
    //  Calculate the total milliseconds. This may not include a final, partial
    //  payload, but the payloads are very short so it wouldn't make any difference.
    //  We take the media data chunk size and divide by the payload bytes, which
    //  gets the number of payloads, then multiply by the payload millis.
    //
    c4TotalMSs = (c4TotalBytes / c4PayloadBytes) * c4PayloadMSs;

    // Give him a copy of the AVP payload type
    c1PayloadType = m_c1AVPPLType;

    // Save our own copy of some stuff
    m_c4ClockInc = c4ClockInc;
    m_c4DataBytes = c4TotalBytes;
    m_c4PayloadBytes = c4PayloadBytes;
    m_c4PayloadSamples = c4PayloadSamples;

    // Reset the data pos value to zero now, to start counting data bytes
    m_c4DataPos = 0;
}


// Close our file if it got opened
tCIDLib::TVoid TFileWaveIO::Terminate()
{
    m_flSrc.Close();
}


// ---------------------------------------------------------------------------
//  TFileWaveIO: Private, non-virtual methods
// ---------------------------------------------------------------------------

// A helper to read an unsigned value from the file
tCIDLib::TCard4 TFileWaveIO::c4ReadNext()
{
    tCIDLib::TCard4 c4Ret;
    m_flSrc.c4ReadBuffer(&c4Ret, 4, tCIDLib::EAllData::FailIfNotAll);
    m_c4DataPos += 4;
    #if CIDLIB_BIGENDIAN
    c4Ret = TRawBits::c4SwapBytes(c4Ret);
    #endif
    return c4Ret;
}

